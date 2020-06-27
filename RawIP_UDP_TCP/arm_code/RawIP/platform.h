// обработчики прерываний
void XEmacPsErrorHandler(void *Callback, u8 Direction, u32 ErrorWord);
void XEmacPsSendHandler(void *Callback);
void XEmacPsRecvHandler(void *Callback);

void init_emac(XEmacPs* EmacInst, char Mac_Address[], u32 Speed);
void init_intr(XScuGic* IntrInstPtr);
void init_BD_rings(XEmacPs* EmacInstPtr,  u32 RxBdAddr, u32 TxBdAddr, u32 RxBdLen, u32 TxBdLen);
void create_MAC_Header(u8* Frame_Header, char* Dist_Addr, char* Sourse_Addr, u32 PayloadSize);
void single_Frame_BD_Record(XEmacPs* EmacInstPtr, XEmacPs_Bd* Temp_BdTxPtr, u8* Frame_Header, u8* Frame_Payload, u32 PayloadSize);
void Header_Frame_BD_Record(XEmacPs_Bd* Temp_BdTxPtr, u8* Frame_Header);
void Payload_Frame_BD_Record(XEmacPs_Bd* Temp_BdTxPtr, u8* Frame_Payload, u32 PayloadSize);
