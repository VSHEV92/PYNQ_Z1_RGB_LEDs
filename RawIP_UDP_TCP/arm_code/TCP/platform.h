void timer_callback(XScuTimer* TimerInstance);
err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

void init_timer(XScuTimer* TimerInstancePtr);
void init_intr(XScuGic* IntrInstPtr);
void init_netif(struct netif *NetifPtr);
