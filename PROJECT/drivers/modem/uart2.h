#ifndef _UART_H_
#define _UART_H_


#define UART2_RX_BUFSIZE 128
#define UART2_TX_BUFSIZE 64


extern void Uart2_Init(CPU_INT32U uart_speed);
extern void Uart2_Flush(void);
extern int  Uart2_Gotc(void);
extern int  Uart2_Getc(void);
extern int  Uart2_Ready(void);
extern int  Uart2_Putc(CPU_INT08U ch);


#endif //#ifndef _UART_H_

