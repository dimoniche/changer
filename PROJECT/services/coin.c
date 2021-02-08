#include "iolpc2368.h"
#include "ucos_ii.h"
#include "cpu.h"
#include "app_serv.h"
#include "coin.h"
#include "data.h"
#include "datadesc.h"
#include "modem.h"
#include <stdlib.h>


OS_STK  CoinTaskStk[COIN_TASK_STK_SIZE];

void  InitImpInput(void);

CPU_INT32U  CoinImpCounter;
CPU_INT32U  CashImpCounter;

static CPU_INT32U cash_pulse = 5000;
static CPU_INT32U cash_pause = 2000;
static char pend_cash_counter = 0;
static CPU_INT32U pend_cash_timestamp;

void SetCashPulseParam(CPU_INT32U pulse, CPU_INT32U pause)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  cash_pulse = pulse * 100;
  cash_pause = pause;
  OS_EXIT_CRITICAL();
}

void CoinTask(void *p_arg)
{
  CPU_INT32U enable_coin;
  CPU_INT32U cash_mode;
  CPU_INT32U cash_enable;
  CPU_INT32U last_cash_count = GetCashCount();
  CPU_INT32U last_cash_time = OSTimeGet();
  CPU_INT32U last_settings_time = OSTimeGet();

  GetData(&EnableCoinDesc, &enable_coin, 0, DATA_FLAG_SYSTEM_INDEX);  
  GetData(&CashModeDesc, &cash_mode, 0, DATA_FLAG_SYSTEM_INDEX);  
  GetData(&EnableValidatorDesc, &cash_enable, 0, DATA_FLAG_SYSTEM_INDEX);

  while(1)
    {
      if (OSTimeGet() - last_settings_time > 1000)
      {
          last_settings_time = OSTimeGet();
          GetData(&EnableCoinDesc, &enable_coin, 0, DATA_FLAG_SYSTEM_INDEX);  
          GetData(&CashModeDesc, &cash_mode, 0, DATA_FLAG_SYSTEM_INDEX);  
          GetData(&EnableValidatorDesc, &cash_enable, 0, DATA_FLAG_SYSTEM_INDEX);
      }
            
      OSTimeDly(1);
      
      if (enable_coin)
        {
          if (GetCoinCount())
          {
            PostUserEvent(EVENT_COIN_INSERTED);
          }
        }
      else
       {
          CoinDisable();
          GetResetCoinCount();
        }

      if (!cash_enable) {GetResetCashCount(); continue;}

      #if OS_CRITICAL_METHOD == 3
      OS_CPU_SR  cpu_sr = 0;
      #endif
      OS_ENTER_CRITICAL();
      if (pend_cash_counter)
      {
          // импульсы инкрементируем только после выдержки паузы
          if (OSTimeGet() - pend_cash_timestamp > cash_pause)
          {
            pend_cash_counter = 0;
            CashImpCounter++;
          }
      }
      OS_EXIT_CRITICAL();
            
      if (cash_mode == 1)
        {
          if (GetCashCount())
          {
            if (last_cash_count == GetCashCount())
            {
                if (labs(OSTimeGet() - last_cash_time) > 500)
                {
                  PostUserEvent(EVENT_CASH_INSERTED);
                }
            }
            else
            {
                last_cash_count = GetCashCount();
                last_cash_time = OSTimeGet();
            }
          }
          else
          {
            last_cash_time = OSTimeGet();
          }
        }
      else
       {
          GetResetCashCount();
       }

    }

}

void CoinDisable(void)
{

}

void CoinEnable(void)
{

}

// получить число монет
CPU_INT32U GetCoinCount(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = CoinImpCounter;
  OS_EXIT_CRITICAL();
  return ctr;
}

// получить число монет и сбросить счетчик
CPU_INT32U GetResetCoinCount(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = CoinImpCounter;
  CoinImpCounter = 0;
  OS_EXIT_CRITICAL();
  return ctr;
}

// получить число импульсов от купюрника
CPU_INT32U GetCashCount(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = CashImpCounter;
  OS_EXIT_CRITICAL();
  return ctr;
}

// получить число импульсов от купюрника и сбросить счетчик
CPU_INT32U GetResetCashCount(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = CashImpCounter;
  CashImpCounter = 0;
  OS_EXIT_CRITICAL();
  return ctr;
}

// инициализация монетоприемника
void InitCoin(void)
{
  CoinImpCounter = 0;
  CashImpCounter = 0;
  InitImpInput();
  OSTaskCreate(CoinTask, (void *)0, (OS_STK *)&CoinTaskStk[COIN_TASK_STK_SIZE-1], COIN_TASK_PRIO);
}


void InputCapture_ISR(void)
{
  CPU_INT08U ir = T3IR;
  static CPU_INT32U period = 0;
  static CPU_INT32U period_cash = 0;
  T3IR = 0xFF;

  if (ir & 0x10)
    {// CR0 interrupt
  
      if (FIO0PIN_bit.P0_23)
        {// пришел задний фронт
          CPU_INT32U cr=T3CR0;
          if (((cr-period) > COIN_IMP_MIN_LEN)
               &&  ((cr-period) < COIN_IMP_MAX_LEN))
            CoinImpCounter++;
        }
      else
        {// пришел передний фронт
          period = T3CR0;
        }
    }

  if (ir & 0x20)
    {// CR1 interrupt
  
      if (FIO0PIN_bit.P0_24)
        {// пришел задний фронт
          CPU_INT32U cr=T3CR1;
          cr -= period_cash;
          if ((cr > (cash_pulse - COIN_IMP_SPAN))
               &&  (cr < (cash_pulse + COIN_IMP_SPAN)))
          {
              pend_cash_counter = 1;
              pend_cash_timestamp = OSTimeGet();
          }
        }
      else
        {// пришел передний фронт
          period_cash = T3CR1;
          pend_cash_counter = 0;
        }
    }
}


extern CPU_INT32U  BSP_CPU_PclkFreq (CPU_INT08U  pclk);

/*
P0.23	MK_P9	IMPULSE OUTPUT (импульсный выход монетоприемника)
P0.24	MK_P8	INHIBIT (блокировка)
*/
// инициализация импульсного входа
// используется CAP3.0
void  InitImpInput (void)
{
    #define INPUT_CAPTURE_FREQ  100000  // частота тактирования частотных входов
  
    CPU_INT32U  pclk_freq;
    CPU_INT32U  rld_cnts;

    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif

    OnChangeCashPulseLen();
        
    OS_ENTER_CRITICAL();

    PCONP_bit.PCTIM3 = 1;
    PCLKSEL1_bit.PCLK_TIMER3 = 2;
      
    PINSEL1_bit.P0_23 = 0x3;
    PINMODE1_bit.P0_23 = 0;
    FIO0DIR_bit.P0_23  = 0;
    FIO0MASK_bit.P0_23 = 0;

    PINSEL1_bit.P0_24 = 0x3;
    PINMODE1_bit.P0_24 = 0;
    FIO0DIR_bit.P0_24  = 0;
    FIO0MASK_bit.P0_24 = 0;
  
    pclk_freq         =   BSP_CPU_PclkFreq(23);
    rld_cnts          =   pclk_freq / INPUT_CAPTURE_FREQ;

    T3CTCR_bit.CTM    =   0;
    T3CTCR_bit.CIS    =   0;          // select CAP3.0 input
    T3PR              =   rld_cnts-1;
    T3MCR             =   0;
    T3CCR             =   0x3F;
    T3EMR             =   0;
    T3TCR             =   0x03;
    T3TCR             =   0x01;
    
    VICINTSELECT     &= ~(1 << VIC_TIMER3);
    VICVECTADDR27     =  (CPU_INT32U)InputCapture_ISR;
    VICVECTPRIORITY27 =  10;                       
    VICINTENABLE      =  (1 << VIC_TIMER3);       

    T3IR = 0xFF;
    
    OS_EXIT_CRITICAL();
}

