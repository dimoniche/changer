#include <includes.h>
#include "uart.h"


#define UART1_RX_BUFSIZE 128
#define UART1_TX_BUFSIZE 64

unsigned char UART1TXBuffer[UART1_TX_BUFSIZE];
unsigned short UART1TXhead = 0;
unsigned short UART1TXtail = 0;
unsigned short UART1TXcount = 0;
unsigned char UART1RXBuffer[UART1_RX_BUFSIZE];
unsigned short UART1RXhead = 0;
unsigned short UART1RXtail = 0;
unsigned short UART1RXcount = 0;


void Uart1_Flush(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  UART1TXcount = UART1TXhead = UART1TXtail = 0;
  UART1RXcount = UART1RXhead = UART1RXtail = 0;
  U1IER_bit.THREIE = 0; 
  U1FCR = 0x06;
  OS_EXIT_CRITICAL();
}


int Uart1_Getc(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = -1;

  if (UART1RXcount > 0)
   {
    UART1RXcount--;
    res = UART1RXBuffer[UART1RXhead++];
    UART1RXhead %= UART1_RX_BUFSIZE;
   }
  OS_EXIT_CRITICAL();
  
  return res;
}

int Uart1_Gotc(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;
  if (UART1RXcount > 0) res = 1;
  OS_EXIT_CRITICAL();
  return res;
}


int Uart1_Ready()
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;
  if (UART1TXcount < UART1_TX_BUFSIZE) res = 1;
  OS_EXIT_CRITICAL();
  return res;
}

int Uart1_Putc(unsigned char ch)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int res = 0;

  if (UART1TXcount < UART1_TX_BUFSIZE)
   {
    if (UART1TXcount == 0)
     {
       if (U1LSR_bit.THRE)
       {
         U1THR = ch;
       }
       else
       {
         UART1TXcount++;
         UART1TXBuffer[UART1TXtail++] = ch;
         UART1TXtail %= UART1_TX_BUFSIZE;
         U1IER = 3; 
       }
     }
    else
     {
       UART1TXcount++;
       UART1TXBuffer[UART1TXtail++] = ch;
       UART1TXtail %= UART1_TX_BUFSIZE;
       U1IER = 3; 
     }
   }
  else
   {
    res = -1;
   }
  OS_EXIT_CRITICAL();
  return res;
}


void Uart1_Isr(void)
{
  CPU_INT08U IIRValue;
  CPU_INT08U u1lsr;
  volatile CPU_INT08U Dummy;

  IIRValue = U1IIR;
  IIRValue >>= 1;      /* skip pending bit in IIR */
  IIRValue &= 0x07;    /* check bit 1~3, interrupt identification */

  if (IIRValue == 2) /* Receive Data Available */
  {
    /* Receive Data Available */
    if (U1LSR_bit.DR)
    {
     if (UART1RXcount < UART1_RX_BUFSIZE)
     {
      UART1RXBuffer[UART1RXtail++] = U1RBR;
      UART1RXtail %= UART1_RX_BUFSIZE;
      UART1RXcount++;
     }
     else
     {
      Dummy = U1RBR;
     }
    }
  }
  else if (IIRValue == 1) /* THRE, transmit holding register empty */
  {
    /* THRE interrupt */
    if (UART1TXcount > 0)
    {
      U1THR = UART1TXBuffer[UART1TXhead++];
      UART1TXhead %= UART1_TX_BUFSIZE;
      UART1TXcount--;
    }
    else
    {
       U1IER = 1; 
    }   
  }
  else
  {
    Dummy = U1RBR;
    u1lsr = U1LSR;
    u1lsr = u1lsr;
  }

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
    
    U1IER = 0;

    U1FCR = 0x06; // enable and reset fifo
      
    U1MCR    = 0;
    U1ACR = 0;
    
    //U1FCR = 0x01; // enable and reset fifo
        
    U1LCR     = 0x80;                                   /* Enable acces to Divisor latches                           */
    
    U1DLL     =  divlo;                                         /* Load divisor                                              */
    U1DLM     =  divhi;
    U1FDR = 0x10;

    U1LCR = 0;

    U1LCR_bit.WLS = 0x03; // 8 bit
    U1LCR_bit.SBS = 0;    // 1 stop bit
    
    U1IER = 1;
      
    PINSEL4_bit.P2_0 = 0x2;
    PINSEL4_bit.P2_1 = 0x2;

    PINMODE4_bit.P2_0 = 0;
    PINMODE4_bit.P2_1 = 0;
    
    FIO2DIR_bit.P2_0 = 1;
    FIO2DIR_bit.P2_1 = 0;
    
    FIO2MASK_bit.P2_0 = 1;
    FIO2MASK_bit.P2_1 = 1;

    VICINTSELECT       &= ~(1 << VIC_UART1);
    VICVECTADDR7       =  (CPU_INT32U)Uart1_Isr;
    VICINTENABLE        =  (1 << VIC_UART1);
    
    Uart1_Flush();
      
    OS_EXIT_CRITICAL();
}


#define UART2_RX_BUFSIZE 128
#define UART2_TX_BUFSIZE 64

unsigned char UART2TXBuffer[UART2_TX_BUFSIZE];
unsigned short UART2TXhead = 0;
unsigned short UART2TXtail = 0;
unsigned short UART2TXcount = 0;
unsigned char UART2RXBuffer[UART2_RX_BUFSIZE];
unsigned short UART2RXhead = 0;
unsigned short UART2RXtail = 0;
unsigned short UART2RXcount = 0;


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


int Uart2_Ready()
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


void Uart2_Isr(void)
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


void Uart0_Flush(void)
{
  U0FCR = 0x06;
}


int Uart0_Getc(void)
{
  int res = -1;
  if (U0LSR_bit.DR) res = U0RBR;
  return res;
}


int Uart0_Gotc(void)
{
  int res = 0;
  if (U0LSR_bit.DR) res = 1;
  return res;
}


int Uart0_Ready()
{
  int res = 0;
  if (U0LSR_bit.THRE) res = 1;
  return res;
}

int Uart0_Putc(unsigned char ch)
{
  int res = 0;
  while (U0LSR_bit.THRE == 0)
    {
      OSTimeDly(1);
    }
  U0THR = ch;
  return res;
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
    
    pclk_freq = BSP_CPU_PclkFreq(PCLK_UART0);               /* Get peripheral clock frequency                            */

    div_fp    = (pclk_freq / 16.0 / baud_rate);                 /* Compute divisor for desired baud rate                     */    
    div_int   = (CPU_INT16U)(div_fp + 0.5);                     /* Round the number up                                       */
    
    divlo     = div_int        & 0x00FF;                        /* Split divisor into LOW and HIGH bytes                     */
    divhi     = (div_int >> 8) & 0x00FF;

    PCONP_bit.PCUART0 = 1;                             /* Enable the power bit for  UART0                           */
    
    U0IER = 0;

    U0FCR = 0x06; // enable and reset fifo
      
    U0ACR = 0;
        
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

    Uart0_Flush();
      
    OS_EXIT_CRITICAL();
}


void InitUart(CPU_INT08U uart_index, CPU_INT32U uart_speed)
{
  switch (uart_index){
    case UART_NUMBER_0:
      // #0
      Uart0_Init(uart_speed);
      break;
    case UART_NUMBER_1:
      // #1
      Uart1_Init(uart_speed);
      break;
    case UART_NUMBER_2:
      // #2
      Uart2_Init(uart_speed);
      break;
    case UART_NUMBER_3:
      break;
  }
}

void UartFlush(CPU_INT08U uart_index)
{
  switch (uart_index){
    case UART_NUMBER_0:
      // #0
      Uart0_Flush();
      break;
    case UART_NUMBER_1:
      // #1
      Uart1_Flush();
      break;
    case UART_NUMBER_2:
      // #2
      Uart2_Flush();
      break;
    case UART_NUMBER_3:
      break;
  }
}

int UartGotc(CPU_INT08U uart_index)
{
  switch (uart_index){
    case UART_NUMBER_0:
      // #0
      return Uart0_Gotc();
    case UART_NUMBER_1:
      // #1
      return Uart1_Gotc();
    case UART_NUMBER_2:
      // #2
      return Uart2_Gotc();
    case UART_NUMBER_3:
      break;
  }
  return 0;
}

int UartGetc(CPU_INT08U uart_index)
{
  switch (uart_index){
    case UART_NUMBER_0:
      // #0
      return Uart0_Getc();
    case UART_NUMBER_1:
      // #1
      return Uart1_Getc();
    case UART_NUMBER_2:
      // #2
      return Uart2_Getc();
    case UART_NUMBER_3:
      break;
  }
  return 0;
}


int UartReady(CPU_INT08U uart_index)
{
  switch (uart_index){
    case UART_NUMBER_0:
      // #0
      return Uart0_Ready();
    case UART_NUMBER_1:
      // #1
      return Uart1_Ready();
    case UART_NUMBER_2:
      // #2
      return Uart2_Ready();
    case UART_NUMBER_3:
      break;
  }
  return 0;
}


int UartPutc(CPU_INT08U uart_index, CPU_INT08U ch)
{
  switch (uart_index){
    case UART_NUMBER_0:
      // #0
      return Uart0_Putc(ch);
    case UART_NUMBER_1:
      // #1
      return Uart1_Putc(ch);
    case UART_NUMBER_2:
      // #2
      return Uart2_Putc(ch);
    case UART_NUMBER_3:
      break;
  }
  return 0;
}
