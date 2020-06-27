#include "xil_printf.h"
#include "xemacps.h"

// обработчик прерываний, вызванных ошибками
void XEmacPsErrorHandler(void *Callback, u8 Direction, u32 ErrorWord){
	//XEmacPs *EmacPsInstancePtr = (XEmacPs *) Callback;

	switch (Direction) {
	case XEMACPS_RECV:
		if (ErrorWord & XEMACPS_RXSR_HRESPNOK_MASK) {
			xil_printf("Receive DMA error\r\n");
		}
		if (ErrorWord & XEMACPS_RXSR_RXOVR_MASK) {
			xil_printf("Receive over run\r\n");
		}
		if (ErrorWord & XEMACPS_RXSR_BUFFNA_MASK) {
			xil_printf("Receive buffer not available\r\n");
		}
		break;
	case XEMACPS_SEND:
		if (ErrorWord & XEMACPS_TXSR_HRESPNOK_MASK) {
			xil_printf("Transmit DMA error\r\n");
		}
		if (ErrorWord & XEMACPS_TXSR_URUN_MASK) {
			xil_printf("Transmit under run\r\n");
		}
		if (ErrorWord & XEMACPS_TXSR_BUFEXH_MASK) {
			xil_printf("Transmit buffer exhausted\r\n");
		}
		if (ErrorWord & XEMACPS_TXSR_RXOVR_MASK) {
			xil_printf("Transmit retry excessed limits\r\n");
		}
		if (ErrorWord & XEMACPS_TXSR_FRAMERX_MASK) {
			xil_printf("Transmit collision\r\n");
		}
		if (ErrorWord & XEMACPS_TXSR_USEDREAD_MASK) {
			xil_printf("Transmit buffer not available\r\n");
		}
		break;
	}
}

// обработчик прерываний, вызванных при передачи данных
extern XEmacPs EmacInst;
extern XEmacPs_Bd *BdTxPtr;
extern XEmacPs_Bd *BdRxPtr;
extern u32 TxBdLen;
extern u8* RX_Frame;
extern u8 Update_Flag;

void XEmacPsSendHandler(void *Callback){
	XEmacPs_BdRingFromHwTx(&(XEmacPs_GetTxRing(&EmacInst)), TxBdLen, &BdTxPtr);
	XEmacPs_BdRingFree(&(XEmacPs_GetTxRing(&EmacInst)), TxBdLen, BdTxPtr);
}

// обработчик прерываний, вызванных при приеме данных
void XEmacPsRecvHandler(void *Callback){
	u32 Frame_Length;

	// освобождаем буфер пакета
	XEmacPs_BdRingFromHwRx(&(XEmacPs_GetRxRing(&EmacInst)), 1, &BdRxPtr);
	XEmacPs_BdRingFree(&(XEmacPs_GetRxRing(&EmacInst)), 1, BdRxPtr);

	Frame_Length = XEmacPs_BdGetLength(BdRxPtr) - XEMACPS_HDR_SIZE;
	if (Frame_Length == 101){
		Update_Flag = 1;
}

	// выделяем новый буфер
	XEmacPs_BdRingAlloc(&(XEmacPs_GetRxRing(&EmacInst)), 1, &BdRxPtr);
	XEmacPs_BdSetAddressRx(BdRxPtr, (UINTPTR) RX_Frame);
	XEmacPs_BdClearRxNew(BdRxPtr);
	XEmacPs_BdRingToHw(&(XEmacPs_GetRxRing(&EmacInst)), 1, BdRxPtr);
    Xil_DCacheFlushRange((UINTPTR)BdRxPtr, 2);
}

