#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/udp.h"

#include "netif/xadapter.h"

#include "xscugic.h"
#include "xgpiops.h"
#include "xparameters.h"

#include "xil_printf.h"

#include "platform.h"


struct netif device_netif;
XScuGic InterruptController;
XGpioPs RGB_LEDs_Gpio;

u8 Update_Flag = 0;

#define Gpio_Bank 2 // номер банка Zynq Gpio
// яркость RGB светодидов
// Led_Brightness[0] - LED4 Red, Led_Brightness[1] - LED4 Green, Led_Brightness[2] - LED4 Blue
// Led_Brightness[3] - LED5 Red, Led_Brightness[4] - LED5 Green, Led_Brightness[5] - LED5 Blue
u8 Led_Brightness[6] = {0,0,0,0,0,0};
u8 *RX_Frame = (u8 *)0x00FA0000;

// callback функция при приеме пакета
void recv_callback(void *arg, struct udp_pcb *tpcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
	u8* Data;
	Data = (u8 *)(p->payload);

    for(int i=0; i<4; i++)
    	RX_Frame[i] = Data[i];

    Update_Flag = 1;
	// освобождаем буфер пакета
	pbuf_free(p);
}

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

	// инициализация контроллера прерываний и включаем прерывания
	init_intr(&InterruptController);

	// инициализируем сетевой интерфейс
	init_netif(&device_netif);

	// настраиваем TCP соединение
	struct udp_pcb *pcb;
	pcb = udp_new();
	udp_bind(pcb, IP_ADDR_ANY, 5001);
	udp_recv(pcb, recv_callback, NULL);

	u8 Data_to_Led_Gpio;
	while (1) {
		// обновляем значения яркости
		if (Update_Flag == 1){
			Update_Flag = 0;
			Xil_DCacheInvalidateRange((UINTPTR)RX_Frame, 4);
			if (RX_Frame[0] == 255){
				Led_Brightness[0] = RX_Frame[1];
				Led_Brightness[1] = RX_Frame[2];
				Led_Brightness[2] = RX_Frame[3];
			}
			else if (RX_Frame[0] == 254){
				Led_Brightness[3] = RX_Frame[1];
				Led_Brightness[4] = RX_Frame[2];
				Led_Brightness[5] = RX_Frame[3];
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

		xemacif_input(&device_netif);
	}

	return 0;
}






