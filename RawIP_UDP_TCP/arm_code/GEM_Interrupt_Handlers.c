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
extern u8 FramesTx;
extern XEmacPs EmacInst;
extern XEmacPs_Bd *BdTxPtr;

void XEmacPsSendHandler(void *Callback){
	xil_printf("Hello Send IRQ\n");
	FramesTx = 1;
	XEmacPs_BdRingFromHwTx(&(XEmacPs_GetTxRing(&EmacInst)), 1, &BdTxPtr);
	XEmacPs_BdRingFree(&(XEmacPs_GetTxRing(&EmacInst)), 1, BdTxPtr);
	XEmacPs_Stop(&EmacInst);
}

// обработчик прерываний, вызванных при приеме данных
void XEmacPsRecvHandler(void *Callback){
	xil_printf("Hello Receive IRQ\n");
}

