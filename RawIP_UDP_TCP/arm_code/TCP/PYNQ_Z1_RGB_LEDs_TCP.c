#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/tcp.h"

#include "netif/xadapter.h"

#include "xscutimer.h"
#include "xscugic.h"
#include "xparameters.h"

#include "xil_printf.h"

#include "platform.h"

volatile int TcpFastTmrFlag = 0;
volatile int TcpSlowTmrFlag = 0;

struct netif device_netif;
XScuTimer TimerInstance;
XScuGic InterruptController;

int main(){

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

	while (1) {
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





