#define  BSP_GLOBALS
#include <includes.h>


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

CPU_INT32U  VIC_SpuriousInt;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  PLL_Init         (void);
static  void  MAM_Init         (void);
static  void  GPIO_Init        (void);
static  void  VIC_Init         (void);

static  void  Tmr_TickInit     (void);

static  void  VIC_Dummy        (void);                         /* Prototypes for dummy interrupt handlers                  */
static  void  VIC_DummyWDT     (void);
static  void  VIC_DummySW      (void);
static  void  VIC_DummyDEBUGRX (void);
static  void  VIC_DummyDEBUGTX (void);
static  void  VIC_DummyTIMER0  (void);
static  void  VIC_DummyTIMER1  (void);
static  void  VIC_DummyUART0   (void);
static  void  VIC_DummyUART1   (void);
static  void  VIC_DummyPWM01   (void);
static  void  VIC_DummyI2C0    (void);
static  void  VIC_DummySPI     (void);
static  void  VIC_DummySSP1    (void);
static  void  VIC_DummyPLL     (void);
static  void  VIC_DummyRTC     (void);
static  void  VIC_DummyEINT0   (void);
static  void  VIC_DummyEINT1   (void);
static  void  VIC_DummyEINT2   (void);
static  void  VIC_DummyEINT3   (void);
static  void  VIC_DummyAD0     (void);
static  void  VIC_DummyI2C1    (void);
static  void  VIC_DummyBOD     (void);
static  void  VIC_DummyETHERNET(void);
static  void  VIC_DummyUSB     (void);
static  void  VIC_DummyCAN01   (void);
static  void  VIC_DummyMMC     (void);
static  void  VIC_DummyGP_DMA  (void);
static  void  VIC_DummyTIMER2  (void);
static  void  VIC_DummyTIMER3  (void);
static  void  VIC_DummyUART2   (void);
static  void  VIC_DummyUART3   (void);
static  void  VIC_DummyI2C2    (void);
static  void  VIC_DummyI2S     (void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    PLL_Init();                                                 /* Initialize the PLL                                       */
    MAM_Init();                                                 /* Initialize the Memory Acceleration Module                */
    GPIO_Init();                                                /* Initialize the board's I/Os                              */
    VIC_Init();                                                 /* Initialize the Vectored Interrupt Controller             */

    Tmr_TickInit();                                             /* Initialize the uC/OS-II tick interrupt                   */
}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Get the CPU clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    CPU_INT32U  msel;
    CPU_INT32U  nsel;
    CPU_INT32U  fin;
    CPU_INT32U  pll_clk_feq;                                    /* When the PLL is enabled, this is Fcco                    */
    CPU_INT32U  clk_div;
    CPU_INT32U  clk_freq;


    switch (CLKSRCSEL & 0x03) {                                 /* Determine the current clock source                       */
        case 0:
             fin = IRC_OSC_FRQ;
             break;

        case 1:
             fin = MAIN_OSC_FRQ;
             break;

        case 2:
             fin = RTC_OSC_FRQ;
             break;

        default:
             fin = IRC_OSC_FRQ;
             break;
    }

    if ((PLLSTAT & (1 << 25)) > 0) {                                /* If the PLL is currently enabled and connected        */
        msel        = (CPU_INT32U)(PLLSTAT & 0x3FFF) + 1;           /* Obtain the PLL multiplier                            */
        nsel        = (CPU_INT32U)((PLLSTAT >>   16) & 0x0F) + 1;   /* Obtain the PLL divider                               */
        pll_clk_feq = (2 * msel * (fin / nsel));                    /* Compute the PLL output frequency                     */
    } else {
        pll_clk_feq = (fin);                                        /* The PLL is bypassed                                  */
    }

    clk_div         = (CPU_INT32U)(CCLKCFG & 0xFF) + 1;             /* Obtain the CPU core clock divider                    */
    clk_freq        = (CPU_INT32U)(pll_clk_feq / clk_div);          /* Compute the ARM Core clock frequency                 */

    return (clk_freq);
}

/*
*********************************************************************************************************
*                                            BSP_CPU_PclkFreq()
*
* Description : Get the peripheral clock frequency for a specific peripheral.
*
* Argument(s) : pclk        The peripheral clock ID, one of PCLK_??? defined in bsp.h.
*
* Return(s)   : The peripheral's clock in Hz
*********************************************************************************************************
*/


CPU_INT32U  BSP_CPU_PclkFreq (CPU_INT08U  pclk)
{
    CPU_INT32U  clk_freq;
    CPU_INT32U  selection;


    clk_freq = BSP_CPU_ClkFreq();

    switch (pclk) {
        case PCLK_WDT:
        case PCLK_TIMER0:
        case PCLK_TIMER1:
        case PCLK_UART0:
        case PCLK_UART1:
        case PCLK_PWM0:
        case PCLK_PWM1:
        case PCLK_I2C0:
        case PCLK_SPI:
        case PCLK_RTC:
        case PCLK_SSP1:
        case PCLK_DAC:
        case PCLK_ADC:
        case PCLK_CAN1:
        case PCLK_CAN2:
        case PCLK_ACF:
             selection = ((PCLKSEL0 >> (pclk * 2)) & 0x03);
             if (selection == 0) {
                 return (clk_freq / 4);
             } else if (selection == 1) {
                 return (clk_freq);
             } else if (selection == 2) {
                 return (clk_freq / 2);
             } else {
                 return (clk_freq / 8);
             }

        case PCLK_BAT_RAM:
        case PCLK_GPIO:
        case PCLK_PCB:
        case PCLK_I2C1:
        case PCLK_SSP0:
        case PCLK_TIMER2:
        case PCLK_TIMER3:
        case PCLK_UART2:
        case PCLK_UART3:
        case PCLK_I2C2:
        case PCLK_MCI:
        case PCLK_SYSCON:
             selection = ((PCLKSEL1 >> ((pclk - 16) * 2)) & 0x03);
             if (selection == 0) {
                 return (clk_freq / 4);
             } else if (selection == 1) {
                 return (clk_freq);
             } else if (selection == 2) {
                 return (clk_freq / 2);
             } else {
                 return (clk_freq / 8);
             }

        default:
             return (0);
    }
}


/*
*********************************************************************************************************
*                                          OS_CPU_ExceptHndlr()
*
* Description : Handle any exceptions.
*
* Argument(s) : except_id     ARM exception type:
*
*                                  OS_CPU_ARM_EXCEPT_RESET             0x00
*                                  OS_CPU_ARM_EXCEPT_UNDEF_INSTR       0x01
*                                  OS_CPU_ARM_EXCEPT_SWI               0x02
*                                  OS_CPU_ARM_EXCEPT_PREFETCH_ABORT    0x03
*                                  OS_CPU_ARM_EXCEPT_DATA_ABORT        0x04
*                                  OS_CPU_ARM_EXCEPT_ADDR_ABORT        0x05
*                                  OS_CPU_ARM_EXCEPT_IRQ               0x06
*                                  OS_CPU_ARM_EXCEPT_FIQ               0x07
*
* Return(s)   : none.
*
* Caller(s)   : OS_CPU_ARM_EXCEPT_HANDLER(), which is declared in os_cpu_a.s.
*********************************************************************************************************
*/

void  OS_CPU_ExceptHndlr (CPU_DATA except_id)
{
    CPU_FNCT_VOID  pfnct;
    CPU_INT32U    *sp;

                                                                        /* If this exception is either an IRQ or FIQ        */
    if ((except_id == OS_CPU_ARM_EXCEPT_IRQ) || (except_id == OS_CPU_ARM_EXCEPT_FIQ)) {
        pfnct = (CPU_FNCT_VOID)VICADDRESS;                              /* Read the interrupt vector from the VIC           */
        if (pfnct != (CPU_FNCT_VOID)0) {                                /* Make sure we don't have a NULL pointer           */
          (*pfnct)();                                                   /* Execute the ISR for the interrupting device      */
            VICADDRESS = 1;                                             /* Acknowlege the VIC interrupt                     */
        }
    } else {
        sp = (CPU_INT32U *)OSTCBCur->OSTCBStkPtr;
        APP_TRACE_INFO(("\nCPU_ARM_EXCEPTION #%d trapped.\n", except_id));
        APP_TRACE_INFO(("R0  : 0x%08x\n", *(sp + 0x01)));
        APP_TRACE_INFO(("R1  : 0x%08x\n", *(sp + 0x02)));
        APP_TRACE_INFO(("R2  : 0x%08x\n", *(sp + 0x03)));
        APP_TRACE_INFO(("R3  : 0x%08x\n", *(sp + 0x04)));
        APP_TRACE_INFO(("R4  : 0x%08x\n", *(sp + 0x05)));
        APP_TRACE_INFO(("R5  : 0x%08x\n", *(sp + 0x06)));
        APP_TRACE_INFO(("R6  : 0x%08x\n", *(sp + 0x07)));
        APP_TRACE_INFO(("R7  : 0x%08x\n", *(sp + 0x08)));
        APP_TRACE_INFO(("R8  : 0x%08x\n", *(sp + 0x09)));
        APP_TRACE_INFO(("R9  : 0x%08x\n", *(sp + 0x0A)));
        APP_TRACE_INFO(("R10 : 0x%08x\n", *(sp + 0x0B)));
        APP_TRACE_INFO(("R11 : 0x%08x\n", *(sp + 0x0C)));
        APP_TRACE_INFO(("R12 : 0x%08x\n", *(sp + 0x0D)));
        APP_TRACE_INFO(("SP  : 0x%08x\n",   sp));
        APP_TRACE_INFO(("LR  : 0x%08x\n", *(sp + 0x0E)));
        APP_TRACE_INFO(("PC  : 0x%08x\n", *(sp + 0x0F)));
        APP_TRACE_INFO(("CPSR: 0x%08x\n", *(sp + 0x00)));

                                                                /* Infinite loop on other exceptions.                       */
                                                                /* Should be replaced by other behavior (reboot, etc.)      */
        while (DEF_TRUE) {
            ;
        }
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    VICINTENCLEAR = 0xFFFFFFFFL;                                /* Disable ALL interrupts                                   */
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                     uC/OS-II TIMER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            Tmr_TickInit()
*
* Description : Initialize uC/OS-II's tick source.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  Tmr_TickInit (void)
{
    CPU_INT32U  pclk_freq;
    CPU_INT32U  rld_cnts;

                                                                /* VIC timer #0 Initialization                              */
    VICINTSELECT &= ~(1 << VIC_TIMER0);                         /* Configure the timer interrupt as an IRQ source           */
    VICVECTADDR4  =  (CPU_INT32U)Tmr_TickISR_Handler;           /* Set the vector address                                   */
    VICINTENABLE  =  (1 << VIC_TIMER0);                         /* Enable the timer interrupt source                        */

    pclk_freq     =   BSP_CPU_PclkFreq(PCLK_TIMER0);            /* Get the peripheral clock frequency                       */

    rld_cnts      =   pclk_freq / OS_TICKS_PER_SEC;             /* Calculate the # of counts necessary for the OS ticker    */

    T0TCR         =  (1 << 1);                                  /* Disable and reset counter 0 and the prescale counter 0   */
    T0TCR         =   0;                                        /* Clear the reset bit                                      */
    T0PC          =   0;                                        /* Prescaler is set to no division                          */

    T0MR0         =   rld_cnts-1;
    T0MCR         =   3;                                        /* Interrupt on MR0 (reset TC), stop TC                     */

    T0CCR         =   0;                                        /* Capture is disabled.                                     */
    T0EMR         =   0;                                        /* No external match output.                                */
    T0TCR         =   1;                                        /* Enable timer 0                                           */
}


/*
*********************************************************************************************************
*                                       Tmr_TickISR_Handler()
*
* Description : Handle the timer interrupt that is used to generate TICKs for uC/OS-II.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  Tmr_TickISR_Handler (void)
{
    T0IR = 0xFF;                                                /* Clear timer #0 interrupt                                 */

    OSTimeTick();                                               /* Call uC/OS-II's OSTimeTick()                             */
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                          LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            PLL_Init()
*
* Description : Set up and activate the PLL.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) The PLL output frequency is calculated by:
*
*                           Fcco = 2 * Fin * m / n
*
*                   where
*
*                           Fin is the PLL input clock (here, the main oscillator)
*                           M   is the PLL clock multiplier. The value (M - 1) is programmed in PLLCFG.
*                           N   is the PLL clock divider.    The value (N - 1) is programmed in PLLCFG.
*
*               (2) Fcco must be between 250 and 550 MHz. The ARM Core clock must never exceed 72 MHz.
*                   Set clk_div to divide Fcco accordingly.
*
*               (3) When using the USB device, you must choose Fcco as a multiple of 96 MHz, and then
*                   set clk_div_usb to divide Fcco to exactly 48 MHz.
*
*               (4) In this example
*
*                         Fin         = 12MHz,
*                         M           = 12,
*                         N           =  1,
*                         clk_div     =  6, and
*                         clk_div_usb =  6.
*
*                 Therefore, Fcco        = 2 * Fin * M / N      = (2 * 12 * 12 / 1) = 288MHz.
*                 The processor clock    = (Fcco / clk_div)     = (288MHz / 6)      =  48MHz.
*                 Finally, the USB clock = (Fcco / clk_div_usb) = (288MHz / 6)      =  48MHz.
*
*               (5) A PLL errata on early revisions of the part prevent Fcco from being greater than 288MHz.
*
*               (6) For later revisions, M = 20, clk_div = 8, and clk_div_usb = 10 will yield 60MHz for
*                   the processor clock and 48MHz for the USB clock.
*********************************************************************************************************
*/

static  void  PLL_Init (void)
{
#if CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL /* Allocate storage for CPU status register                 */
    CPU_SR  cpu_sr = 0;
#endif

    CPU_INT32U  m;
    CPU_INT32U  n;
    CPU_INT32U  clk_div;
    CPU_INT32U  clk_div_usb;


    m           = 11;                                           /* PLL Multiplier = 20, MSEL bits = 12 - 1 = 11             */
    n           =  0;                                           /* PLL Divider    =  1, NSEL bits =  1 - 1 =  0             */
    clk_div     =  3;                                           /* Configure the  ARM Core clock div to 6. CCLKSEL =  6 - 1 */
    clk_div_usb =  5;                                           /* Configure the USB clock divider to 6, USBSEL  = 6 - 1    */

    if ((PLLSTAT & DEF_BIT_25) > 0) {                           /* If the PLL is already running                            */
        CPU_CRITICAL_ENTER();
        PLLCON  &= ~DEF_BIT_01;                                 /* Disconnect the PLL                                       */
        PLLFEED  =  0xAA;                                       /* PLL register update sequence, 0xAA, 0x55                 */
        PLLFEED  =  0x55;
        CPU_CRITICAL_EXIT();
    }

    CPU_CRITICAL_ENTER();
    PLLCON   &= ~DEF_BIT_00;                                    /* Disable the PLL                                          */
    PLLFEED   =  0xAA;                                          /* PLL register update sequence, 0xAA, 0x55                 */
    PLLFEED   =  0x55;
    CPU_CRITICAL_EXIT();

    SCS      |=  DEF_BIT_00;                                    /* enable fast IO           */
    SCS      &= ~DEF_BIT_04;                                    /* OSCRANGE = 0, Main OSC is between 1 and 20 Mhz           */
    SCS      |=  DEF_BIT_05;                                    /* OSCEN = 1, Enable the main oscillator                    */

    while ((SCS &  DEF_BIT_06) == 0) {                          /* Wait until OSCSTAT is set (Main OSC ready to be used)    */
        ;
    }

    CLKSRCSEL = DEF_BIT_00;                                     /* Select main OSC, 12MHz, as the PLL clock source          */

    CPU_CRITICAL_ENTER();
    PLLCFG    = (m << 0) | (n << 16);                           /* Configure the PLL multiplier and divider                 */
    PLLFEED   = 0xAA;                                           /* PLL register update sequence, 0xAA, 0x55                 */
    PLLFEED   = 0x55;
    CPU_CRITICAL_EXIT();

    CPU_CRITICAL_ENTER();
    PLLCON   |= DEF_BIT_00;                                     /* Enable the PLL                                           */
    PLLFEED   = 0xAA;                                           /* PLL register update sequence, 0xAA, 0x55                 */
    PLLFEED   = 0x55;
    CPU_CRITICAL_EXIT();

    CCLKCFG   = clk_div;                                        /* Configure the ARM Core Processor clock divider           */
    USBCLKCFG = clk_div_usb;                                    /* Configure the USB clock divider                          */

    while ((PLLSTAT & DEF_BIT_26) == 0) {                       /* Wait for PLOCK to become set                             */
        ;
    }

    PCLKSEL0  = 0xAAAAAAAA;                                     /* Set peripheral clocks to be half of main clock           */
    PCLKSEL1  = 0x22AAA8AA;

    CPU_CRITICAL_ENTER();
    PLLCON   |= DEF_BIT_01;                                     /* Connect the PLL. The PLL is now the active clock source  */
    PLLFEED   = 0xAA;                                           /* PLL register update sequence, 0xAA, 0x55                 */
    PLLFEED   = 0x55;
    CPU_CRITICAL_EXIT();

    while ((PLLSTAT & DEF_BIT_25) == 0) {                       /* Wait PLLC, the PLL connect status bit to become set      */
        ;
    }
}


/*
*********************************************************************************************************
*                                       MAM_Init()
*
* Description : Initialize the Memory Acceleration Module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  MAM_Init (void)
{
    CPU_INT32U  clk_freq;


    clk_freq = BSP_CPU_ClkFreq();                               /* Get the current core clock frequency                     */

    MAMCR    = 0;                                               /* Disable MAM functionality                                */

    if (clk_freq < 20000000) {                                  /* Compare current clock frequency with MAM modes           */
        MAMTIM = 1;                                             /* Set MAM fetch cycles to 1 processor clock in duration    */
    }

    if (clk_freq < 40000000) {
        MAMTIM = 2;                                             /* Set MAM fetch cycles to 2 processor clock in duration    */
    }

    if (clk_freq >= 40000000) {
        MAMTIM = 3;                                             /* Set MAM fetch cycles to 3 processor clock in duration    */
    }

    MAMCR = 2;                                                  /* Enable full MAM functionality                            */
}


/*
*********************************************************************************************************
*                                          GPIO_Init()
*
* Description : Initializes the GPIO pins.  All the I/O pins are initialized in this function
*               so you don't have to look at multiple places for I/O initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) Refer to the LPC2378 User Manual, Chapter 9 for a detailed Pin Assignment
*********************************************************************************************************
*/

static  void  GPIO_Init (void)
{
    IO0DIR     =  0;
    IO1DIR     =  0;
    FIO0DIR    =  0;
    FIO1DIR    =  0;
    FIO2DIR    =  0;
    FIO3DIR    =  0;
    FIO4DIR    =  0;

    FIO0MASK   =  0;
    FIO1MASK   =  0;
    FIO2MASK   =  0;
    FIO3MASK   =  0;
    FIO4MASK   =  0;

    PINSEL0    =  0;
    PINSEL1    =  0;
    PINSEL2    =  0;
    PINSEL3    =  0;
    PINSEL4    =  0;
    PINSEL5    =  0;
    PINSEL6    =  0;
    PINSEL7    =  0;
    PINSEL8    =  0;
    PINSEL9    =  0;
    PINSEL10   =  0;
}


/*
*********************************************************************************************************
*                                     VIC_Init()
*
* Description : Initialize the Vectored Interrupt Controller
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

static  void  VIC_Init (void)
{
    VICINTENCLEAR =  0xFFFFFFFF;                                /* Disable ALL interrupts                                   */
    VICADDRESS    =  0;                                         /* Acknowlege any pending VIC interrupt                     */
    VICPROTECTION =  0;                                         /* Allow VIC register access in User of Priviledged modes   */

    VICVECTADDR0  = (CPU_INT32U)VIC_DummyWDT;                   /* Set the vector address                                   */
    VICVECTADDR1  = (CPU_INT32U)VIC_DummySW;
    VICVECTADDR2  = (CPU_INT32U)VIC_DummyDEBUGRX;
    VICVECTADDR3  = (CPU_INT32U)VIC_DummyDEBUGTX;
    VICVECTADDR4  = (CPU_INT32U)VIC_DummyTIMER0;
    VICVECTADDR5  = (CPU_INT32U)VIC_DummyTIMER1;
    VICVECTADDR6  = (CPU_INT32U)VIC_DummyUART0;
    VICVECTADDR7  = (CPU_INT32U)VIC_DummyUART1;
    VICVECTADDR8  = (CPU_INT32U)VIC_DummyPWM01;
    VICVECTADDR9  = (CPU_INT32U)VIC_DummyI2C0;
    VICVECTADDR10 = (CPU_INT32U)VIC_DummySPI;
    VICVECTADDR11 = (CPU_INT32U)VIC_DummySSP1;
    VICVECTADDR12 = (CPU_INT32U)VIC_DummyPLL;
    VICVECTADDR13 = (CPU_INT32U)VIC_DummyRTC;
    VICVECTADDR14 = (CPU_INT32U)VIC_DummyEINT0;
    VICVECTADDR15 = (CPU_INT32U)VIC_DummyEINT1;
    VICVECTADDR16 = (CPU_INT32U)VIC_DummyEINT2;
    VICVECTADDR17 = (CPU_INT32U)VIC_DummyEINT3;
    VICVECTADDR18 = (CPU_INT32U)VIC_DummyAD0;
    VICVECTADDR19 = (CPU_INT32U)VIC_DummyI2C1;
    VICVECTADDR20 = (CPU_INT32U)VIC_DummyBOD;
    VICVECTADDR21 = (CPU_INT32U)VIC_DummyETHERNET;
    VICVECTADDR22 = (CPU_INT32U)VIC_DummyUSB;
    VICVECTADDR23 = (CPU_INT32U)VIC_DummyCAN01;
    VICVECTADDR24 = (CPU_INT32U)VIC_DummyMMC;
    VICVECTADDR25 = (CPU_INT32U)VIC_DummyGP_DMA;
    VICVECTADDR26 = (CPU_INT32U)VIC_DummyTIMER2;
    VICVECTADDR27 = (CPU_INT32U)VIC_DummyTIMER3;
    VICVECTADDR28 = (CPU_INT32U)VIC_DummyUART2;
    VICVECTADDR29 = (CPU_INT32U)VIC_DummyUART3;
    VICVECTADDR30 = (CPU_INT32U)VIC_DummyI2C2;
    VICVECTADDR31 = (CPU_INT32U)VIC_DummyI2S;
}


/*
*********************************************************************************************************
*********************************************************************************************************
**                                       DUMMY INTERRUPT HANDLERS
*********************************************************************************************************
*********************************************************************************************************
*/

static  void  VIC_Dummy (void)
{
    while (DEF_TRUE) {
        ;
    }
}

static  void  VIC_DummyWDT (void)
{
    VIC_SpuriousInt = VIC_WDT;
    VIC_Dummy();
}

static  void  VIC_DummySW (void)
{
    VIC_SpuriousInt = VIC_SW;
    VIC_Dummy();
}

static  void  VIC_DummyDEBUGRX (void)
{
    VIC_SpuriousInt = VIC_DEBUGRX;
    VIC_Dummy();
}

static  void  VIC_DummyDEBUGTX (void)
{
    VIC_SpuriousInt = VIC_DEBUGTX;
    VIC_Dummy();
}

static  void  VIC_DummyTIMER0 (void)
{
    VIC_SpuriousInt = VIC_TIMER0;
    VIC_Dummy();
}

static  void  VIC_DummyTIMER1 (void)
{
    VIC_SpuriousInt = VIC_TIMER1;
    VIC_Dummy();
}

static  void  VIC_DummyUART0 (void)
{
    VIC_SpuriousInt = VIC_UART0;
    VIC_Dummy();
}

static  void  VIC_DummyUART1 (void)
{
    VIC_SpuriousInt = VIC_UART1;
    VIC_Dummy();
}

static  void  VIC_DummyPWM01 (void)
{
    VIC_SpuriousInt = VIC_PWM1;
    VIC_Dummy();
}

static  void  VIC_DummyI2C0 (void)
{
    VIC_SpuriousInt = VIC_I2C0;
    VIC_Dummy();
}

static  void  VIC_DummySPI (void)
{
    VIC_SpuriousInt = VIC_SPI;
    VIC_Dummy();
}

static  void  VIC_DummySSP1 (void)
{
    VIC_SpuriousInt = VIC_SSP1;
    VIC_Dummy();
}

static  void  VIC_DummyPLL (void)
{
    VIC_SpuriousInt = VIC_PLL;
    VIC_Dummy();
}

static  void  VIC_DummyRTC (void)
{
    VIC_SpuriousInt = VIC_RTC;
    VIC_Dummy();
}

static  void  VIC_DummyEINT0 (void)
{
    VIC_SpuriousInt = VIC_EINT0;
    VIC_Dummy();
}

static  void  VIC_DummyEINT1 (void)
{
    VIC_SpuriousInt = VIC_EINT1;
    VIC_Dummy();
}

static  void  VIC_DummyEINT2 (void)
{
    VIC_SpuriousInt = VIC_EINT2;
    VIC_Dummy();
}

static  void  VIC_DummyEINT3 (void)
{
    VIC_SpuriousInt = VIC_EINT3;
    VIC_Dummy();
}

static  void  VIC_DummyAD0 (void)
{
    VIC_SpuriousInt = VIC_AD0;
    VIC_Dummy();
}

static  void  VIC_DummyI2C1 (void)
{
    VIC_SpuriousInt = VIC_I2C1;
    VIC_Dummy();
}

static  void  VIC_DummyBOD (void)
{
    VIC_SpuriousInt = VIC_BOD;
    VIC_Dummy();
}

static  void  VIC_DummyETHERNET (void)
{
    VIC_SpuriousInt = VIC_ETHERNET;
    VIC_Dummy();
}

static  void  VIC_DummyUSB (void)
{
    VIC_SpuriousInt = VIC_USB;
    VIC_Dummy();
}

static  void  VIC_DummyCAN01 (void)
{
    VIC_SpuriousInt = VIC_CAN12;
    VIC_Dummy();
}

static  void  VIC_DummyMMC (void)
{
    VIC_SpuriousInt = VIC_MMC;
    VIC_Dummy();
}

static  void  VIC_DummyGP_DMA (void)
{
    VIC_SpuriousInt = VIC_GP_DMA;
    VIC_Dummy();
}

static  void  VIC_DummyTIMER2 (void)
{
    VIC_SpuriousInt = VIC_TIMER2;
    VIC_Dummy();
}

static  void  VIC_DummyTIMER3 (void)
{
    VIC_SpuriousInt = VIC_TIMER3;
    VIC_Dummy();
}

static  void  VIC_DummyUART2 (void)
{
    VIC_SpuriousInt = VIC_UART2;
    VIC_Dummy();
}

static  void  VIC_DummyUART3 (void)
{
    VIC_SpuriousInt = VIC_UART3;
    VIC_Dummy();
}

static  void  VIC_DummyI2C2 (void)
{
    VIC_SpuriousInt = VIC_I2C2;
    VIC_Dummy();
}

static  void  VIC_DummyI2S (void)
{
    VIC_SpuriousInt = VIC_I2S;
    VIC_Dummy();
}


void Reset(void)
{
#if CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL /* Allocate storage for CPU status register                 */
  volatile CPU_SR  cpu_sr = 0;
#endif

  CPU_CRITICAL_ENTER();    
  
  WDCLKSEL = 0x01;
  WDTC = 0x00FFFF;  
  WDMOD = 3;
  WDFEED = 0xAA;  
  WDFEED = 0x55;

  for(;;);
}
