#include <includes.h>
#include "uart2.h"


static unsigned char  UART2TXBuffer[UART2_TX_BUFSIZE];
static unsigned short UART2TXhead = 0;
static unsigned short UART2TXtail = 0;
static unsigned short UART2TXcount = 0;
static unsigned char  UART2RXBuffer[UART2_RX_BUFSIZE];
static unsigned short UART2RXhead = 0;
static unsigned short UART2RXtail = 0;
static unsigned short UART2RXcount = 0;


void Uart2_Flush(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  UART2TXcount = UART2TXhead = UART2TXtail = 0;
  UART2RXcount = UART2RXhead = UART2RXtail = 0;
  U2IER_bit.THREIE = 0; 
  U2FCR = 0x06;
  OS_EXIT_CRITICAL();
}

int Uart2_Getc(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = -1;

  if (UART2RXcount > 0)
   {
    UART2RXcount--;
    res = UART2RXBuffer[UART2RXhead++];
    UART2RXhead %= UART2_RX_BUFSIZE;
   }
  OS_EXIT_CRITICAL();
  
  return res;
}

int Uart2_Gotc(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;
  if (UART2RXcount > 0) res = 1;
  OS_EXIT_CRITICAL();
  return res;
}

int Uart2_Ready(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;
  if (UART2TXcount < UART2_TX_BUFSIZE) res = 1;
  OS_EXIT_CRITICAL();
  return res;
}

int Uart2_Putc(unsigned char ch)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;

  if (UART2TXcount < UART2_TX_BUFSIZE)
   {
    if (UART2TXcount == 0)
     {
       if (U2LSR_bit.THRE)
       {
         U2THR = ch;
       }
       else
       {
         UART2TXcount++;
         UART2TXBuffer[UART2TXtail++] = ch;
         UART2TXtail %= UART2_TX_BUFSIZE;
         U2IER = 3; 
       }
     }
    else
     {
       UART2TXcount++;
       UART2TXBuffer[UART2TXtail++] = ch;
       UART2TXtail %= UART2_TX_BUFSIZE;
       U2IER = 3; 
     }
   }
  else
   {
    res = -1;
   }
  OS_EXIT_CRITICAL();
  return res;
}

static void Uart2_Isr(void)
{
  CPU_INT08U IIRValue;
  CPU_INT08U u1lsr;
  volatile CPU_INT08U Dummy;

  IIRValue = U2IIR;
  IIRValue >>= 1;      /* skip pending bit in IIR */
  IIRValue &= 0x07;    /* check bit 1~3, interrupt identification */

  if (IIRValue == 2) /* Receive Data Available */
  {
    /* Receive Data Available */
    if (U2LSR_bit.DR)
    {
     if (UART2RXcount < UART2_RX_BUFSIZE)
     {
      UART2RXBuffer[UART2RXtail++] = U2RBR;
      UART2RXtail %= UART2_RX_BUFSIZE;
      UART2RXcount++;
     }
     else
     {
      Dummy = U2RBR;
     }
    }
  }
  else if (IIRValue == 1) /* THRE, transmit holding register empty */
  {
    /* THRE interrupt */
    if (UART2TXcount > 0)
    {
      U2THR = UART2TXBuffer[UART2TXhead++];
      UART2TXhead %= UART2_TX_BUFSIZE;
      UART2TXcount--;
    }
    else
    {
       U2IER = 1; 
    }   
  }
  else
  {
    Dummy = U2RBR;
    u1lsr = U2LSR;
    u1lsr = u1lsr;
  }

}

void Uart2_Init(CPU_INT32U baud_rate)
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
    
    pclk_freq = BSP_CPU_PclkFreq(PCLK_UART2);               /* Get peripheral clock frequency                            */

    div_fp    = (pclk_freq / 16.0 / baud_rate);                 /* Compute divisor for desired baud rate                     */    
    div_int   = (CPU_INT16U)(div_fp + 0.5);                     /* Round the number up                                       */
    
    divlo     = div_int        & 0x00FF;                        /* Split divisor into LOW and HIGH bytes                     */
    divhi     = (div_int >> 8) & 0x00FF;

    PCONP_bit.PCUART2 = 1;                             /* Enable the power bit for  UART0                           */
    
    U2IER = 0;

    U2FCR = 0x06; // enable and reset fifo
      
    U2ACR = 0;
    
    //U1FCR = 0x01; // enable and reset fifo
        
    U2LCR     = 0x80;                                   /* Enable acces to Divisor latches                           */
    
    U2DLL     =  divlo;                                         /* Load divisor                                              */
    U2DLM     =  divhi;
    U2FDR = 0x10;

    U2LCR = 0;

    U2LCR_bit.WLS = 0x03; // 8 bit
    U2LCR_bit.SBS = 0;    // 1 stop bit
    
    U2IER = 1;
      
    PINSEL0_bit.P0_10 = 0x1;
    PINSEL0_bit.P0_11 = 0x1;

    PINMODE0_bit.P0_10 = 0;
    PINMODE0_bit.P0_11 = 0;
    
    FIO0DIR_bit.P0_10 = 1;
    FIO0DIR_bit.P0_11 = 0;
    
    FIO0MASK_bit.P0_10 = 1;
    FIO0MASK_bit.P0_11 = 1;

    VICINTSELECT       &= ~(1 << VIC_UART2);
    VICVECTADDR28       =  (CPU_INT32U)Uart2_Isr;
    VICINTENABLE        =  (1 << VIC_UART2);
    
    Uart2_Flush();
      
    OS_EXIT_CRITICAL();
}



