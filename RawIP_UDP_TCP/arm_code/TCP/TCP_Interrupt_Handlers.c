#include "lwip/tcp.h"
#include "xscutimer.h"

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;

// обработчик прерываний от таймера
void timer_callback(XScuTimer * TimerInstance)
{
	TcpFastTmrFlag = 1;
	TcpSlowTmrFlag = 1;
	XScuTimer_ClearInterruptStatus(TimerInstance);
}

// callback функция при приеме пакета
err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	// если получаем пакет, а соединение еще не установлено
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	// указывваем packet control buffer сколько получили данных
	tcp_recved(tpcb, p->len);

	char* Data;
    int Data_Len;

	Data = (char *)(p->payload);
	Data_Len = p->len;
    for(int i=0; i<Data_Len; i++)
    	xil_printf("%c", Data[i]);
    xil_printf("\n");

	// освобождаем буфер пакета
	pbuf_free(p);

	return ERR_OK;
}

// callback функция при установке соединения
err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	tcp_recv(newpcb, recv_callback);
	tcp_arg(newpcb, NULL);
	return ERR_OK;
}
