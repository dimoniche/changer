#include <includes.h>
#include "uart0.h"

#define UART0_RX_BUFSIZE 128
#define UART0_TX_BUFSIZE 64

unsigned char UART0TXBuffer[UART0_TX_BUFSIZE];
unsigned short UART0TXhead = 0;
unsigned short UART0TXtail = 0;
unsigned short UART0TXcount = 0;
unsigned char UART0RXBuffer[UART0_RX_BUFSIZE];
unsigned short UART0RXhead = 0;
unsigned short UART0RXtail = 0;
unsigned short UART0RXcount = 0;


void Uart0_Flush(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  UART0TXcount = UART0TXhead = UART0TXtail = 0;
  UART0RXcount = UART0RXhead = UART0RXtail = 0;
  U0IER_bit.THREIE = 0; 
  U0FCR = 0x06;
  OS_EXIT_CRITICAL();
}


int Uart0_Getc(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = -1;

  if (UART0RXcount > 0)
   {
    UART0RXcount--;
    res = UART0RXBuffer[UART0RXhead++];
    UART0RXhead %= UART0_RX_BUFSIZE;
   }
  OS_EXIT_CRITICAL();
  
  return res;
}

int Uart0_Gotc(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;
  if (UART0RXcount > 0) res = 1;
  OS_EXIT_CRITICAL();
  return res;
}


int Uart0_Ready()
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;
  if (UART0TXcount < UART0_TX_BUFSIZE) res = 1;
  OS_EXIT_CRITICAL();
  return res;
}

int Uart0_Putc(unsigned char ch)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;

  if (UART0TXcount < UART0_TX_BUFSIZE)
   {
    if (UART0TXcount == 0)
     {
       if (U0LSR_bit.THRE)
       {
         U0THR = ch;
       }
       else
       {
         UART0TXcount++;
         UART0TXBuffer[UART0TXtail++] = ch;
         UART0TXtail %= UART0_TX_BUFSIZE;
         U0IER = 3; 
       }
     }
    else
     {
       UART0TXcount++;
       UART0TXBuffer[UART0TXtail++] = ch;
       UART0TXtail %= UART0_TX_BUFSIZE;
       U0IER = 3; 
     }
   }
  else
   {
    res = -1;
   }
  OS_EXIT_CRITICAL();
  return res;
}


void Uart0_Isr(void)
{
  CPU_INT08U IIRValue;
  CPU_INT08U u1lsr;
  volatile CPU_INT08U Dummy;

  IIRValue = U0IIR;
  IIRValue >>= 1;      /* skip pending bit in IIR */
  IIRValue &= 0x07;    /* check bit 1~3, interrupt identification */

  if (IIRValue == 2) /* Receive Data Available */
  {
    /* Receive Data Available */
    if (U0LSR_bit.DR)
    {
     if (UART0RXcount < UART0_RX_BUFSIZE)
     {
      UART0RXBuffer[UART0RXtail++] = U0RBR;
      UART0RXtail %= UART0_RX_BUFSIZE;
      UART0RXcount++;
     }
     else
     {
      Dummy = U0RBR;
     }
    }
  }
  else if (IIRValue == 1) /* THRE, transmit holding register empty */
  {
    /* THRE interrupt */
    if (UART0TXcount > 0)
    {
      U0THR = UART0TXBuffer[UART0TXhead++];
      UART0TXhead %= UART0_TX_BUFSIZE;
      UART0TXcount--;
    }
    else
    {
       U0IER = 1; 
    }   
  }
  else
  {
    Dummy = U0RBR;
    u1lsr = U0LSR;
    u1lsr = u1lsr;
  }

}

void Uart0_Init(CPU_INT32U baud_rate)
{
    float    div_fp;                                         /* Baud rate divisor floating point precision                */
    CPU_INT16U  div_int;                                        /* Baud rate divisor floating point precision                */
    CPU_INT08U  divlo;
    CPU_INT08U  divhi;
    CPU_INT32U  pclk_freq;
    
    #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
    #endif

    OS_ENTER_CRITICAL();
    
    pclk_freq = BSP_CPU_PclkFreq(3);               /* Get peripheral clock frequency                            */

    div_fp    = (pclk_freq / 16.0 / baud_rate);                 /* Compute divisor for desired baud rate                     */    
    div_int   = (CPU_INT16U)(div_fp + 0.5);                     /* Round the number up                                       */
    
    divlo     = div_int        & 0x00FF;                        /* Split divisor into LOW and HIGH bytes                     */
    divhi     = (div_int >> 8) & 0x00FF;

    PCONP_bit.PCUART2 = 1;                             /* Enable the power bit for  UART0                           */
    
    U0IER = 0;

    U0FCR = 0x06; // enable and reset fifo
      
    U0ACR = 0;
    
    //U1FCR = 0x01; // enable and reset fifo
        
    U0LCR     = 0x80;                                   /* Enable acces to Divisor latches                           */
    
    U0DLL     =  divlo;                                         /* Load divisor                                              */
    U0DLM     =  divhi;
    U0FDR = 0x10;

    U0LCR = 0;

    U0LCR_bit.WLS = 0x03; // 8 bit
    U0LCR_bit.SBS = 0;    // 1 stop bit
    
    U0IER = 1;
      
    PINSEL0_bit.P0_2 = 0x1;
    PINSEL0_bit.P0_3 = 0x1;

    PINMODE0_bit.P0_2 = 0;
    PINMODE0_bit.P0_3 = 0;
    
    FIO0DIR_bit.P0_2 = 1;
    FIO0DIR_bit.P0_3 = 0;
    
    FIO0MASK_bit.P0_2 = 1;
    FIO0MASK_bit.P0_3 = 1;

    VICINTSELECT       &= ~(1 << VIC_UART0);
    VICVECTADDR6       =  (CPU_INT32U)Uart0_Isr;
    VICINTENABLE        =  (1 << VIC_UART0);
    
    Uart0_Flush();
      
    OS_EXIT_CRITICAL();
}


void  Uart0_WrByte(CPU_INT08U tx_byte)
{
    while (Uart0_Putc(tx_byte) != 0) OSTimeDly(1);
}


void Uart0_Send(unsigned char *buf, int len)
{
  while (len--) Uart0_WrByte(*buf++);
}


int Uart0_RdByteWithTimeOut(CPU_INT08U *byte, CPU_INT32U timeout)
{
    CPU_INT32U ctr = 0;
    int res = -1;
    
    while (res < 0) {
        res = Uart0_Getc();
        if (res >= 0) break;
        OSTimeDly(1);
        if (ctr++ > timeout) return 0;
    }

    *byte = res;
    return 1;
}


int Uart0_Receive(unsigned char *buf, int len, int timeout)
{
  while (len--)
    {
      if (!Uart0_RdByteWithTimeOut(buf++, timeout)) return 0;
    }
  return 1;
}

