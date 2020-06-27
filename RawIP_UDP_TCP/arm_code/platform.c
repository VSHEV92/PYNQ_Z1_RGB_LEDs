#include "xemacps.h"
#include "xscugic.h"
#include "xparameters.h"

// инициализируем EMAC и устанавливаем MAC-адрес и скорость работы
void init_emac(XEmacPs* EmacInstPtr, char Mac_Address[], u32 Speed){
	XEmacPs_Config *Config;
	Config = XEmacPs_LookupConfig(XPAR_PS7_ETHERNET_0_DEVICE_ID);
	XEmacPs_CfgInitialize(EmacInstPtr, Config, Config->BaseAddress);
	XEmacPs_SetMacAddress(EmacInstPtr, Mac_Address, 1);
	XEmacPs_SetOperatingSpeed(EmacInstPtr, Speed);
}

// инициализация контроллера прерываний и включаем прерывания
void init_intr(XScuGic* IntrInstPtr){
	XScuGic_Config *Intc_Config;
	Intc_Config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	XScuGic_CfgInitialize(IntrInstPtr, Intc_Config, Intc_Config->CpuBaseAddress);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler,	IntrInstPtr);
	Xil_ExceptionEnable();

}
