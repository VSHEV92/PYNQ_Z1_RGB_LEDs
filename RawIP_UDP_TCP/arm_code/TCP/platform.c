#include "xscutimer.h"
#include "xscugic.h"
#include "xparameters.h"
#include "lwip/netif.h"

// настраиваем таймер для TCP соединения
void init_timer(XScuTimer* TimerInstancePtr){

	XScuTimer_Config *ConfigPtr;
	ConfigPtr = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
	XScuTimer_CfgInitialize(TimerInstancePtr, ConfigPtr, ConfigPtr->BaseAddr);
	XScuTimer_EnableAutoReload(TimerInstancePtr);

    // устанавливаем тайм-аут в 250 мсек
	int TimerLoadValue =  XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 8;
	XScuTimer_LoadTimer(TimerInstancePtr, TimerLoadValue);
}

// инициализация контроллера прерываний и включаем прерывания
void init_intr(XScuGic* IntrInstPtr){
	XScuGic_Config *Intc_Config;
	Intc_Config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	XScuGic_CfgInitialize(IntrInstPtr, Intc_Config, Intc_Config->CpuBaseAddress);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler,	IntrInstPtr);
	Xil_ExceptionEnable();

}
void init_netif(struct netif *NetifPtr){
// инициализируем сетевой интерфейс
	unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	ip_addr_t ipaddr, netmask, gw;
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);

	lwip_init();
	xemac_add(NetifPtr, &ipaddr, &netmask,	&gw, mac_ethernet_address, XPAR_XEMACPS_0_BASEADDR);
	netif_set_default(&NetifPtr);
	netif_set_up(NetifPtr);
}
