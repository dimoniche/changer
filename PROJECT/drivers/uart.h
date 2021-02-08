#ifndef _UART_H_
#define _UART_H_


#define UART_NUMBER_0    0
#define UART_NUMBER_1    1
#define UART_NUMBER_2    2
#define UART_NUMBER_3    3


extern void InitUart(CPU_INT08U uart_index, CPU_INT32U uart_speed);
extern void UartFlush(CPU_INT08U uart_index);
extern int UartGotc(CPU_INT08U uart_index);
extern int UartGetc(CPU_INT08U uart_index);
extern int UartReady(CPU_INT08U uart_index);
extern int UartPutc(CPU_INT08U uart_index, CPU_INT08U ch);


#endif //#ifndef _UART_H_

