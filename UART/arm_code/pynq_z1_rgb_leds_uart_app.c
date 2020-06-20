#include "xparameters.h"
#include "xgpiops.h"
#include "xuartps.h"
#include "xscugic.h"

XGpioPs RGB_LEDs_Gpio;
XUartPs Uart_Port;
XScuGic InterruptController;

#define Gpio_Bank 2 // номер банка Zynq Gpio

// яркость RGB светодидов
// Led_Brightness[0] - LED4 Red, Led_Brightness[1] - LED4 Green, Led_Brightness[2] - LED4 Blue
// Led_Brightness[3] - LED5 Red, Led_Brightness[4] - LED5 Green, Led_Brightness[5] - LED5 Blue
static u8 Led_Brightness[6] = {0,0,0,0,0,0};

// входной буфер для UART
static u8 Uart_RX_Buffer[4];

// обработчик прерывания при поступлении данные UART
void Uart_Intr_Handler(void *CallBackRef, u32 Event, unsigned int EventData)
{
	XUartPs_Recv(&Uart_Port, Uart_RX_Buffer, 4);
	if (Uart_RX_Buffer[0] == 255){
		Led_Brightness[0] = Uart_RX_Buffer[1];
	    Led_Brightness[1] = Uart_RX_Buffer[2];
	    Led_Brightness[2] = Uart_RX_Buffer[3];
	}
	else if (Uart_RX_Buffer[0] == 254){
		Led_Brightness[3] = Uart_RX_Buffer[1];
	    Led_Brightness[4] = Uart_RX_Buffer[2];
		Led_Brightness[5] = Uart_RX_Buffer[3];
	}
}

// инициализация и начальная настройка периферии
void platform_init(){
	    // инициализация LEDs Gpio
		XGpioPs_Config *Gpio_ConfigPtr;
		Gpio_ConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
		XGpioPs_CfgInitialize(&RGB_LEDs_Gpio, Gpio_ConfigPtr, Gpio_ConfigPtr->BaseAddr);

		// настраиваем порты LEDs Gpio как выходы
		XGpioPs_SetDirection(&RGB_LEDs_Gpio, Gpio_Bank, 0xFFFFFFFF);
		XGpioPs_SetOutputEnable(&RGB_LEDs_Gpio, Gpio_Bank, 0xFFFFFFFF);

		// ---------------------------------------------------------------------------------------------------

		// инициализация Uart контроллера
	    XUartPs_Config *Uart_Config;
		Uart_Config = XUartPs_LookupConfig(XPAR_PS7_UART_0_DEVICE_ID);
		XUartPs_CfgInitialize(&Uart_Port, Uart_Config, Uart_Config->BaseAddress);

		// настраиваем параметры работы UART
		XUartPsFormat Uart_Configuration;
		Uart_Configuration.BaudRate = 9600;
		Uart_Configuration.DataBits = XUARTPS_FORMAT_8_BITS;
		Uart_Configuration.Parity = XUARTPS_FORMAT_NO_PARITY;
		Uart_Configuration.StopBits = XUARTPS_FORMAT_1_STOP_BIT;
		XUartPs_SetDataFormat(&Uart_Port, &Uart_Configuration);

		// ---------------------------------------------------------------------------------------------------

		// инициализация контроллера прерываний
		XScuGic_Config *Intc_Config;
		Intc_Config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
		XScuGic_CfgInitialize(&InterruptController, Intc_Config, Intc_Config->CpuBaseAddress);

		// подключаем обработчик прерываний
		XScuGic_Connect(&InterruptController, XPAR_XUARTPS_0_INTR, (Xil_ExceptionHandler) XUartPs_InterruptHandler, (void *) &Uart_Port);

		// включаем прерывания
		Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler,	&InterruptController);
		XScuGic_Enable(&InterruptController, XPAR_XUARTPS_0_INTR);
		Xil_ExceptionEnable();

		// ---------------------------------------------------------------------------------------------------

		// подключаем обработчик и включаем прерывания для UART
		XUartPs_SetHandler(&Uart_Port, (XUartPs_Handler)Uart_Intr_Handler, &Uart_Port);
		XUartPs_SetInterruptMask(&Uart_Port, XUARTPS_IXR_RXOVR);

		// устанавливаем, чтобы прерывание срабатывало при поступлении 4 байт
		XUartPs_SetFifoThreshold(&Uart_Port, 4);
}

int main(){
	u8 Data_to_Led_Gpio;

	platform_init();

	while(1){
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
