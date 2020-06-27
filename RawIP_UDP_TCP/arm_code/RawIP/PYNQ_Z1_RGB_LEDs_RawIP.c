#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xemacps.h"

#include "platform.h"

XEmacPs EmacInst;
XScuGic InterruptController;
XEmacPs_Bd *BdTxPtr;
XEmacPs_Bd *BdRxPtr;

u8 RX_Frame[1600];

u32 RxBdAddr = 0x00F00000;
u32 TxBdAddr = 0x00F10000;
u32 RxBdLen = 2;
u32 TxBdLen = 4;


// MAC-адреса приеемника и передатчика
char Source_Mac_Addr[] = { 0x00, 0x0a, 0x35, 0x01, 0x02, 0x03 };
char Distination_Mac_Addr[] = { 0x98, 0x28, 0xa6, 0x23, 0x3c, 0x70 };

int main(){
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
//	u32 PayloadSize = 1000;
//	u8 Frame_Header[XEMACPS_HDR_SIZE];
//	u8 Frame_1_Payload[PayloadSize];
//	u8 Frame_2_Payload[PayloadSize];
//
//	// создаем заголовок кадра
//	create_MAC_Header(Frame_Header, Distination_Mac_Addr, Source_Mac_Addr, PayloadSize);
//
//	// создаем полезные данные для двух кадров
//    for (int Idx = 0; Idx < PayloadSize; Idx++)
//    	Frame_1_Payload[Idx] = Idx + 1;
//
//    for (int Idx = 0; Idx < PayloadSize; Idx++)
//    	Frame_2_Payload[Idx] = Idx + 30;
//
//	Xil_DCacheFlushRange((UINTPTR)Frame_1_Payload, PayloadSize);
//	Xil_DCacheFlushRange((UINTPTR)Frame_2_Payload, PayloadSize);
//
//	// конфигурируем Tx BD Ring
//	XEmacPs_Bd* Temp_BdTxPtr;
//
//	XEmacPs_BdRingAlloc(&(XEmacPs_GetTxRing(&EmacInst)), TxBdLen, &BdTxPtr);
//	Temp_BdTxPtr = BdTxPtr;
//
//	// делаем записи для ethernet кадров
//	Header_Frame_BD_Record(Temp_BdTxPtr, Frame_Header);
//	Temp_BdTxPtr =  XEmacPs_BdRingNext(&(XEmacPs_GetTxRing((&EmacInst))), Temp_BdTxPtr);
//
//	Payload_Frame_BD_Record(Temp_BdTxPtr, Frame_1_Payload, PayloadSize);
//	Temp_BdTxPtr =  XEmacPs_BdRingNext(&(XEmacPs_GetTxRing((&EmacInst))), Temp_BdTxPtr);
//
//	Header_Frame_BD_Record(Temp_BdTxPtr, Frame_Header);
//	Temp_BdTxPtr =  XEmacPs_BdRingNext(&(XEmacPs_GetTxRing((&EmacInst))), Temp_BdTxPtr);
//
//	Payload_Frame_BD_Record(Temp_BdTxPtr, Frame_2_Payload, PayloadSize);
//	Temp_BdTxPtr =  XEmacPs_BdRingNext(&(XEmacPs_GetTxRing((&EmacInst))), Temp_BdTxPtr);
//
//	//передаем Ring Hw для передачи
//	XEmacPs_BdRingToHw(&(XEmacPs_GetTxRing(&EmacInst)), TxBdLen, BdTxPtr);
//	Xil_DCacheFlushRange((UINTPTR)BdTxPtr, 2*TxBdLen);
// ---------------------------------------------

	XEmacPs_BdRingAlloc(&(XEmacPs_GetRxRing(&EmacInst)), 1, &BdRxPtr);
	XEmacPs_BdSetAddressRx(BdRxPtr, (UINTPTR) RX_Frame);
	XEmacPs_BdClearRxNew(BdRxPtr);
	XEmacPs_BdRingToHw(&(XEmacPs_GetRxRing(&EmacInst)), 1, BdRxPtr);
	Xil_DCacheFlushRange((UINTPTR)BdRxPtr, 2);

	XEmacPs_Start(&EmacInst);

	while(1);

//	XEmacPs_Transmit(&EmacInst);

	return 0;
}
