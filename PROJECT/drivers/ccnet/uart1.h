#ifndef _UART1_H_
#define _UART1_H_

extern void Uart1_Purge(void);
extern void Uart1_Send(unsigned char *buf, int len);
extern int Uart1_Receive(unsigned char *buf, int len, int timeout);
extern void  Uart1_WrByte(CPU_INT08U tx_byte);
extern int Uart1_RdByteWithTimeOut(CPU_INT08U *byte, CPU_INT32U timeout);
extern void Uart1_Init(CPU_INT32U baud_rate);

#endif //#ifndef _UART0_H_
