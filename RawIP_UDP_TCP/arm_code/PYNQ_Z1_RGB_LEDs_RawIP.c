#include "xil_printf.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xemacps.h"

#include "platform.h"

XEmacPs EmacInst;
XScuGic InterruptController;
XEmacPs_Bd *BdTxPtr;

// области памяти, где будет хранится TX и RX BDs
XEmacPs_Bd BdTemplate;
u8 *RxBdSpacePtr = (u8 *)0x00F00000;
u8 *TxBdSpacePtr = (u8 *)0x00F10000;

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

	// --------------------------------------------------------------------------------------------
	//инициализируем BD Rings для приема и передачи пустыми BD
	XEmacPs_BdClear(&BdTemplate); // RX
	XEmacPs_BdRingCreate(&(XEmacPs_GetRxRing(&EmacInst)), (UINTPTR) RxBdSpacePtr, (UINTPTR) RxBdSpacePtr, XEMACPS_BD_ALIGNMENT, 1);
	XEmacPs_BdRingClone(&(XEmacPs_GetRxRing(&EmacInst)), &BdTemplate, XEMACPS_RECV);

	XEmacPs_BdClear(&BdTemplate); // TX
	XEmacPs_BdRingCreate(&(XEmacPs_GetTxRing(&EmacInst)), (UINTPTR) TxBdSpacePtr, (UINTPTR) TxBdSpacePtr, XEMACPS_BD_ALIGNMENT, 1);
	XEmacPs_BdRingClone(&(XEmacPs_GetTxRing(&EmacInst)), &BdTemplate, XEMACPS_SEND);

	// --------------------------------------------------------------------------------------------
	// создаем Frame для передачи
	u32 PayloadSize = 1000;                               // количество полезных данных
	u32 TxFrameLength = XEMACPS_HDR_SIZE + PayloadSize;  // длина пакета

	u8 TxFrame[TxFrameLength];

	// Destination address
	for (int Idx = 0; Idx < XEMACPS_MAC_ADDR_SIZE; Idx++)
		TxFrame[Idx] = Distination_Mac_Addr[Idx];

	// Source address
	for (int Idx = 0; Idx < XEMACPS_MAC_ADDR_SIZE; Idx++)
		TxFrame[Idx + XEMACPS_MAC_ADDR_SIZE] = Source_Mac_Addr[Idx];

	// Frame Type
	TxFrame[12] = (PayloadSize & 0xFF00) >> 8;
	TxFrame[13] = PayloadSize & 0x00FF;

    // Frame Payload
    for (int Idx = 0; Idx < PayloadSize; Idx++)
    	TxFrame[Idx + XEMACPS_HDR_SIZE] = Idx + 1;

	Xil_DCacheFlushRange((UINTPTR)TxFrame, TxFrameLength);
	// --------------------------------------------------------------------------------------------

	// передаем кадр для выдачи
	XEmacPs_BdRingAlloc(&(XEmacPs_GetTxRing(&EmacInst)), 1, &BdTxPtr);

	// устанавливаем адрес кадра, длину и говорим, что он последний
	XEmacPs_BdSetAddressTx(BdTxPtr, (UINTPTR)TxFrame);
	XEmacPs_BdSetLength(BdTxPtr, TxFrameLength);
	XEmacPs_BdSetLast(BdTxPtr);

	//передаем Ring Hw для передачи
	XEmacPs_BdRingToHw(&(XEmacPs_GetTxRing(&EmacInst)), 1, BdTxPtr);
	Xil_DCacheFlushRange((UINTPTR)BdTxPtr, 2);

	XEmacPs_Start(&EmacInst);
	XEmacPs_Transmit(&EmacInst);

	while (!FramesTx);

//	XEmacPs_BdRingFromHwTx(&(XEmacPs_GetTxRing(&EmacInst)), 1, &BdTxPtr);
//	XEmacPs_BdRingFree(&(XEmacPs_GetTxRing(&EmacInst)), 1, BdTxPtr);
//	XEmacPs_Stop(&EmacInst);

	return 0;
}
