#include "lwip/tcp.h"
#include "xscutimer.h"

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
extern volatile u8 Update_Flag;
extern u8* RX_Frame;

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

	u8* Data;
	Data = (u8 *)(p->payload);

    for(int i=0; i<4; i++)
    	RX_Frame[i] = Data[i];

    Update_Flag = 1;
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
