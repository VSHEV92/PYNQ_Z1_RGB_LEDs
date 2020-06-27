// обработчики прерываний
u8 FramesTx = 0;
void XEmacPsErrorHandler(void *Callback, u8 Direction, u32 ErrorWord);
void XEmacPsSendHandler(void *Callback);
void XEmacPsRecvHandler(void *Callback);

void init_emac(XEmacPs* EmacInst, char Mac_Address[], u32 Speed);
void init_intr(XScuGic* IntrInstPtr);
