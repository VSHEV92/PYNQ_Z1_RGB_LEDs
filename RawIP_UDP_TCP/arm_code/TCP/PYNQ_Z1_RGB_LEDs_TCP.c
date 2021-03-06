#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/tcp.h"

#include "netif/xadapter.h"

#include "xscutimer.h"
#include "xscugic.h"
#include "xgpiops.h"
#include "xparameters.h"

#include "xil_printf.h"

#include "platform.h"

volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;

struct netif device_netif;
XScuTimer TimerInstance;
XScuGic InterruptController;
XGpioPs RGB_LEDs_Gpio;

u8 Update_Flag = 0;

#define Gpio_Bank 2 // номер банка Zynq Gpio
// яркость RGB светодидов
// Led_Brightness[0] - LED4 Red, Led_Brightness[1] - LED4 Green, Led_Brightness[2] - LED4 Blue
// Led_Brightness[3] - LED5 Red, Led_Brightness[4] - LED5 Green, Led_Brightness[5] - LED5 Blue
u8 Led_Brightness[6] = {0,0,0,0,0,0};
u8 *RX_Frame = (u8 *)0x00FA0000;

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

    // настраиваем таймер для TCP соединения
	init_timer(&TimerInstance);

	// инициализация контроллера прерываний и включаем прерывания
	init_intr(&InterruptController);
	XScuGic_Connect(&InterruptController, XPAR_SCUTIMER_INTR, (Xil_ExceptionHandler) timer_callback, (void *) &TimerInstance);
	XScuGic_Enable(&InterruptController, XPAR_SCUTIMER_INTR);

	// инициализируем сетевой интерфейс
	init_netif(&device_netif);

	// включаем прерывания от таймера
	XScuTimer_EnableInterrupt(&TimerInstance);
	XScuTimer_Start(&TimerInstance);

	// настраиваем TCP соединение
	struct tcp_pcb *pcb;
	u32 TCP_CONN_PORT = 5001;
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	tcp_bind(pcb, IP_ADDR_ANY, TCP_CONN_PORT);
	pcb = tcp_listen(pcb);
	tcp_arg(pcb, NULL);
	tcp_accept(pcb, accept_callback);

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

		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
		xemacif_input(&device_netif);
		}

	return 0;
}





