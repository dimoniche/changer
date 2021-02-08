#ifndef _UART0_H_
#define _UART0_H_

extern void Uart0_Send(unsigned char *buf, int len);
extern int Uart0_Receive(unsigned char *buf, int len, int timeout);
extern void Uart0_Init(CPU_INT32U baud_rate);
extern void  Uart0_WrByte(CPU_INT08U tx_byte);
extern void Uart0_Flush(void);
extern int Uart0_RdByteWithTimeOut(CPU_INT08U *byte, CPU_INT32U timeout);
extern int Uart0_Ready();
extern int Uart0_Gotc(void);
extern int Uart0_Getc(void);
extern int Uart0_Putc(unsigned char ch);

#endif //#ifndef _UART0_H_
