#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xemacps.h"
#include "xgpiops.h"

#include "platform.h"

XEmacPs EmacInst;
XScuGic InterruptController;
XGpioPs RGB_LEDs_Gpio;
u8 Update_Flag = 0;

#define Gpio_Bank 2 // номер банка Zynq Gpio
// яркость RGB светодидов
// Led_Brightness[0] - LED4 Red, Led_Brightness[1] - LED4 Green, Led_Brightness[2] - LED4 Blue
// Led_Brightness[3] - LED5 Red, Led_Brightness[4] - LED5 Green, Led_Brightness[5] - LED5 Blue
u8 Led_Brightness[6] = {0,0,0,0,0,0};

XEmacPs_Bd *BdTxPtr;
XEmacPs_Bd *BdRxPtr;

u8 *RX_Frame = (u8 *)0x00FA0000;

u32 RxBdAddr = 0x00F10000;
u32 TxBdAddr = 0x00F50000;
u32 RxBdLen = 1;
u32 TxBdLen = 4;


// MAC-адреса приеемника и передатчика
char Source_Mac_Addr[] = { 0x00, 0x0a, 0x35, 0x01, 0x02, 0x03 };
char Distination_Mac_Addr[] = { 0x98, 0x28, 0xa6, 0x23, 0x3c, 0x70 };

int main(){
	// ------------------------------------------------------------------------------------------------------
	// инициализация LEDs Gpio
	XGpioPs_Config *Gpio_ConfigPtr;
	Gpio_ConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	XGpioPs_CfgInitialize(&RGB_LEDs_Gpio, Gpio_ConfigPtr, Gpio_ConfigPtr->BaseAddr);

	// настраиваем порты LEDs Gpio как выходы
	XGpioPs_SetDirection(&RGB_LEDs_Gpio, Gpio_Bank, 0xFFFFFFFF);
	XGpioPs_SetOutputEnable(&RGB_LEDs_Gpio, Gpio_Bank, 0xFFFFFFFF);
	// ------------------------------------------------------------------------------------------------------

	// инициализируем EMAC и устанавливаем MAC-адрес и скорость работы
	init_emac(&EmacInst, Source_Mac_Addr, 1000);

	// инициализация контроллера прерываний и включаем прерывания
	init_intr(&InterruptController);

	// включаем настраиваем обработчики прерываний для GEM
	XScuGic_Connect(&InterruptController, XPAR_XEMACPS_0_INTR, (Xil_ExceptionHandler) XEmacPs_IntrHandler, (void *) &EmacInst);
	XScuGic_Enable(&InterruptController, XPAR_XEMACPS_0_INTR);
	XEmacPs_SetHandler(&EmacInst, XEMACPS_HANDLER_DMASEND, (void *) XEmacPsSendHandler, &EmacInst);
	XEmacPs_SetHandler(&EmacInst, XEMACPS_HANDLER_DMARECV, (void *) XEmacPsRecvHandler, &EmacInst);
	XEmacPs_SetHandler(&EmacInst, XEMACPS_HANDLER_ERROR, (void *) XEmacPsErrorHandler, &EmacInst);

	//инициализируем BD Rings для приема и передачи пустыми BD
	init_BD_rings(&EmacInst,  RxBdAddr, TxBdAddr, RxBdLen, TxBdLen);

	// --------------------------------------------------------------------------------------------
	XEmacPs_BdRingAlloc(&(XEmacPs_GetRxRing(&EmacInst)), 1, &BdRxPtr);
	XEmacPs_BdSetAddressRx(BdRxPtr, (UINTPTR) RX_Frame);
	XEmacPs_BdClearRxNew(BdRxPtr);

	XEmacPs_BdRingToHw(&(XEmacPs_GetRxRing(&EmacInst)), 1, BdRxPtr);
	Xil_DCacheFlushRange((UINTPTR)BdRxPtr, 2);

	XEmacPs_Start(&EmacInst);

	// --------------------------------------------------------------------------------------------
	u8 Data_to_Led_Gpio;
	while(1){
		// обновляем значения яркости
		if (Update_Flag == 1){
			Update_Flag = 0;
			Xil_DCacheInvalidateRange((UINTPTR)RX_Frame, 4+XEMACPS_HDR_SIZE);
			if (RX_Frame[XEMACPS_HDR_SIZE+0] == 255){
				Led_Brightness[0] = RX_Frame[XEMACPS_HDR_SIZE+1];
				Led_Brightness[1] = RX_Frame[XEMACPS_HDR_SIZE+2];
				Led_Brightness[2] = RX_Frame[XEMACPS_HDR_SIZE+3];
			}
			else if (RX_Frame[XEMACPS_HDR_SIZE+0] == 254){
				Led_Brightness[3] = RX_Frame[XEMACPS_HDR_SIZE+1];
				Led_Brightness[4] = RX_Frame[XEMACPS_HDR_SIZE+2];
				Led_Brightness[5] = RX_Frame[XEMACPS_HDR_SIZE+3];
			}
		}

		// управляем яркостью светодиодов с помощью ШИМ
		for(int i = 0; i<256; i++){
			Data_to_Led_Gpio = 0;
	        for (int j = 5; j>=0; j--){
	           	Data_to_Led_Gpio = Data_to_Led_Gpio << 1;
	           	if (Led_Brightness[j] > i)
	           		Data_to_Led_Gpio = Data_to_Led_Gpio + 1;
	        }
	        XGpioPs_Write(&RGB_LEDs_Gpio, Gpio_Bank, Data_to_Led_Gpio);
		}
	}

	return 0;
}
