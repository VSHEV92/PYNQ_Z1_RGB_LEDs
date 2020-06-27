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

//инициализируем BD Rings для приема и передачи пустыми BD
void init_BD_rings(XEmacPs* EmacInstPtr,  u32 RxBdAddr, u32 TxBdAddr, u32 RxBdLen, u32 TxBdLen){

	u8 *RxBdSpacePtr = (u8 *)RxBdAddr;
	u8 *TxBdSpacePtr = (u8 *)TxBdAddr;

	XEmacPs_Bd BdTemplate;
	XEmacPs_BdClear(&BdTemplate); // RX
	XEmacPs_BdSetStatus(&BdTemplate, XEMACPS_RXBUF_NEW_MASK);
	XEmacPs_BdRingCreate(&(XEmacPs_GetRxRing(EmacInstPtr)), (UINTPTR) RxBdSpacePtr, (UINTPTR) RxBdSpacePtr, XEMACPS_BD_ALIGNMENT, RxBdLen);
	XEmacPs_BdRingClone(&(XEmacPs_GetRxRing(EmacInstPtr)), &BdTemplate, XEMACPS_RECV);

	XEmacPs_BdClear(&BdTemplate); // TX
	XEmacPs_BdSetStatus(&BdTemplate, XEMACPS_TXBUF_USED_MASK);
	XEmacPs_BdRingCreate(&(XEmacPs_GetTxRing(EmacInstPtr)), (UINTPTR) TxBdSpacePtr, (UINTPTR) TxBdSpacePtr, XEMACPS_BD_ALIGNMENT, TxBdLen);
	XEmacPs_BdRingClone(&(XEmacPs_GetTxRing(EmacInstPtr)), &BdTemplate, XEMACPS_SEND);
}

//создаем заголовок кадра
void create_MAC_Header(u8* Frame_Header, char* Dist_Addr, char* Sourse_Addr, u32 PayloadSize){
	// Destination address
	for (int Idx = 0; Idx < XEMACPS_MAC_ADDR_SIZE; Idx++)
		Frame_Header[Idx] = Dist_Addr[Idx];

	// Source address
	for (int Idx = 0; Idx < XEMACPS_MAC_ADDR_SIZE; Idx++)
		Frame_Header[Idx + XEMACPS_MAC_ADDR_SIZE] = Sourse_Addr[Idx];

	// Frame Type
	Frame_Header[12] = (PayloadSize & 0xFF00) >> 8;
	Frame_Header[13] = PayloadSize & 0x00FF;
	Xil_DCacheFlushRange((UINTPTR)Frame_Header, XEMACPS_HDR_SIZE);
}

// конфигурирование заголовка кадра
void Header_Frame_BD_Record(XEmacPs_Bd* Temp_BdTxPtr, u8* Frame_Header){
	XEmacPs_BdSetAddressTx(Temp_BdTxPtr, (UINTPTR)Frame_Header);
	XEmacPs_BdSetLength(Temp_BdTxPtr, XEMACPS_HDR_SIZE);
	XEmacPs_BdClearTxUsed(Temp_BdTxPtr);
}

// конфигурирование полезные данные кадра
void Payload_Frame_BD_Record(XEmacPs_Bd* Temp_BdTxPtr, u8* Frame_Payload, u32 PayloadSize){
	XEmacPs_BdSetAddressTx(Temp_BdTxPtr, (UINTPTR)Frame_Payload);
	XEmacPs_BdSetLength(Temp_BdTxPtr, PayloadSize);
	XEmacPs_BdClearTxUsed(Temp_BdTxPtr);
	XEmacPs_BdSetLast(Temp_BdTxPtr);
}
