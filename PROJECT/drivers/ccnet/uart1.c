#include <includes.h>
#include "uart1.h"


void Uart1_Send(unsigned char *buf, int len)
{
  while (len--) Uart1_WrByte(*buf++);
}


int Uart1_Receive(unsigned char *buf, int len, int timeout)
{
  while (len--)
    {
      if (!Uart1_RdByteWithTimeOut(buf++, timeout)) return 0;
    }
  return 1;
}


void Uart1_Init(CPU_INT32U baud_rate)
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
    
    pclk_freq = BSP_CPU_PclkFreq(PCLK_UART1);               /* Get peripheral clock frequency                            */

    div_fp    = (pclk_freq / 16.0 / baud_rate);                 /* Compute divisor for desired baud rate                     */    
    div_int   = (CPU_INT16U)(div_fp + 0.5);                     /* Round the number up                                       */
    
    divlo     = div_int        & 0x00FF;                        /* Split divisor into LOW and HIGH bytes                     */
    divhi     = (div_int >> 8) & 0x00FF;

    PCONP_bit.PCUART1 = 1;                             /* Enable the power bit for  UART0                           */
    
    U1LCR     = 0x80;                                   /* Enable acces to Divisor latches                           */
    
    U1DLL     =  divlo;                                         /* Load divisor                                              */
    U1DLM     =  divhi;

    U1FDR = 0x10;
        
    U1LCR = 0;
      
    U1MCR    = 0;
    U1ACR = 0;

    U1FCR_bit.FCRFE = 1; // enable fifo

    U1LCR_bit.WLS = 0x03; // 8 bit
    U1LCR_bit.SBS = 0;    // 1 stop bit
    
    U1IER = 0;
      
    PINSEL4_bit.P2_0 = 0x2;
    PINSEL4_bit.P2_1 = 0x2;

    PINMODE4_bit.P2_0 = 0;
    PINMODE4_bit.P2_1 = 0;
    
    FIO2DIR_bit.P2_0 = 1;
    FIO2DIR_bit.P2_1 = 0;
    
    FIO2MASK_bit.P2_0 = 1;
    FIO2MASK_bit.P2_1 = 1;

    OS_EXIT_CRITICAL();
}

void  Uart1_WrByte(CPU_INT08U tx_byte)
{
    while (!U1LSR_bit.THRE) {
        OSTimeDly(1);
    }

    U1THR = tx_byte;
}

void Uart1_Purge(void)
{
  //очистить прием
  U1FCR_bit.RFR = 1;
}

int Uart1_RdByteWithTimeOut(CPU_INT08U *byte, CPU_INT32U timeout)
{
    CPU_INT32U ctr = 0;

    while (!U1LSR_bit.DR) {
        OSTimeDly(1);
        if (++ctr > timeout) return 0;
    }

    //error = U1LSR;
    *byte = (CPU_INT08U)(U1RBR & 0x00FF);                     /* Remove the data from the holding register                */
    return 1;
}
