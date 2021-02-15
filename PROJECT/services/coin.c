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

static CPU_INT32U  CoinImpCounter;
static CPU_INT32U  CashImpCounter;
static CPU_INT32U  BankImpCounter;
static CPU_INT32U  HopperImpCounter;

static CPU_INT32U coin_pulse = 50;
static CPU_INT32U coin_pause = 20;
static char pend_coin_counter = 0;
static CPU_INT32U pend_coin_timestamp;

static CPU_INT32U cash_pulse = 50;
static CPU_INT32U cash_pause = 20;
static char pend_cash_counter = 0;
static CPU_INT32U pend_cash_timestamp;

static CPU_INT32U bank_pulse = 50;
static CPU_INT32U bank_pause = 20;
static char pend_bank_counter = 0;
static CPU_INT32U pend_bank_timestamp;

static CPU_INT32U hopper_pulse = 50;
static CPU_INT32U hopper_pause = 20;
static char pend_hopper_counter = 0;
static CPU_INT32U pend_hopper_timestamp;

static CPU_INT32U signal_error_hopper_pulse = 1000;
static char pend_upsignal_error_hopper_counter;
static char pend_downsignal_error_hopper_counter;
static CPU_INT32U pend_signal_error_hopper_timestamp;

static CPU_INT32U signal_nomoney_hopper_pulse = 1000;
static char pend_upsignal_nomoney_hopper_counter;
static char pend_downsignal_nomoney_hopper_counter;
static CPU_INT32U pend_signal_nomoney_hopper_timestamp;

static CPU_INT32U signal_button_pulse = 1000;
static char pend_upsignal_button_counter;
static char pend_downsignal_button_counter;
static CPU_INT32U pend_signal_button_timestamp;

static CPU_INT32U cashLevel;
static CPU_INT32U coinLevel;
static CPU_INT32U bankLevel;
static CPU_INT32U hopperLevel;

CPU_INT32U period_coin;
CPU_INT32U period_bank;
CPU_INT32U period_signal;
CPU_INT32U period_hopper;

void SetCoinPulseParam(CPU_INT32U pulse, CPU_INT32U pause)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  coin_pulse = pulse * 1;
  coin_pause = pause;
  OS_EXIT_CRITICAL();
}

void SetCashPulseParam(CPU_INT32U pulse, CPU_INT32U pause)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  cash_pulse = pulse * 1;
  cash_pause = pause;
  OS_EXIT_CRITICAL();
}

void SetBankPulseParam(CPU_INT32U pulse, CPU_INT32U pause)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  bank_pulse = pulse * 1;
  bank_pause = pause;
  OS_EXIT_CRITICAL();
}

void SetHopperPulseParam(CPU_INT32U pulse, CPU_INT32U pause)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  bank_pulse = pulse * 1;
  bank_pause = pause;
  OS_EXIT_CRITICAL();
}

void SetLevelParam(CPU_INT32U level1, CPU_INT32U level2, CPU_INT32U level3,  CPU_INT32U level4)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
 
  cashLevel = level1;
  bankLevel = level2;
  coinLevel = level3;
  hopperLevel = level4;
  
  OS_EXIT_CRITICAL();
}

void CoinTask(void *p_arg)
{
  CPU_INT32U enable_coin;
  CPU_INT32U bank_enable;
  CPU_INT32U regime_hopper;
  
  CPU_INT32U last_coin_count = GetCoinCount();
  CPU_INT32U last_coin_time = OSTimeGet();

  CPU_INT32U last_bank_count = GetbankCount();
  CPU_INT32U last_bank_time = OSTimeGet();

  CPU_INT32U last_hopper_count = GetHopperCount();
  CPU_INT32U last_hopper_time = OSTimeGet();

  CPU_INT32U last_settings_time = OSTimeGet();

  GetData(&EnableCoinDesc, &enable_coin, 0, DATA_FLAG_SYSTEM_INDEX);  
  GetData(&EnableBankDesc, &bank_enable, 0, DATA_FLAG_SYSTEM_INDEX);
  GetData(&RegimeHopperDesc, &regime_hopper, 0, DATA_FLAG_SYSTEM_INDEX);
 
  while(1)
    {
      if (OSTimeGet() - last_settings_time > 1000)
      {
          last_settings_time = OSTimeGet();
          GetData(&EnableCoinDesc, &enable_coin, 0, DATA_FLAG_SYSTEM_INDEX);
          GetData(&EnableBankDesc, &bank_enable, 0, DATA_FLAG_SYSTEM_INDEX);
          GetData(&RegimeHopperDesc, &regime_hopper, 0, DATA_FLAG_SYSTEM_INDEX);
      }
            
      OSTimeDly(1);
      
      #if OS_CRITICAL_METHOD == 3
      OS_CPU_SR  cpu_sr = 0;
      #endif

      if (enable_coin)
      {
        OS_ENTER_CRITICAL();
        if (pend_coin_counter)
        {
          // импульсы инкрементируем только после выдержки паузы
          if (OSTimeGet() - pend_coin_timestamp > coin_pause)
          {
            pend_coin_counter = 0;
            CoinImpCounter++;
          }
        }
        OS_EXIT_CRITICAL();
            
        if (GetCoinCount())
        {
          if (last_coin_count == GetCoinCount())
          {
              if (labs(OSTimeGet() - last_coin_time) > 500)
              {
                PostUserEvent(EVENT_COIN_INSERTED);
              }
          }
          else
          {
              last_coin_count = GetCoinCount();
              last_coin_time = OSTimeGet();
          }
        }
        else
        {
          last_coin_time = OSTimeGet();
        }
      }
      else
      {
          CoinDisable();
          GetResetCoinCount();
          
          OS_ENTER_CRITICAL();
          pend_coin_counter = 0;
          pend_coin_timestamp = 0;
          OS_EXIT_CRITICAL();
      }
      
      if (bank_enable)
      {
        OS_ENTER_CRITICAL();
        if (pend_bank_counter)
        {
          // импульсы инкрементируем только после выдержки паузы
          if (OSTimeGet() - pend_bank_timestamp > bank_pause)
          {
            pend_bank_counter = 0;
            BankImpCounter++;
          }
        }
        OS_EXIT_CRITICAL();
            
        if (GetbankCount())
        {
          if (last_bank_count == GetbankCount())
          {
              if (labs(OSTimeGet() - last_bank_time) > 500)
              {
                PostUserEvent(EVENT_BANK_INSERTED);
              }
          }
          else
          {
              last_bank_count = GetbankCount();
              last_bank_time = OSTimeGet();
          }
        }
        else
        {
          last_bank_time = OSTimeGet();
        }
      }
      else
      {
          BankDisable();
          GetResetbankCount();
          
          OS_ENTER_CRITICAL();
          pend_bank_counter = 0;
          pend_bank_timestamp = 0;
          OS_EXIT_CRITICAL();
      }
      
      if (regime_hopper)
      {
        OS_ENTER_CRITICAL();
        if (pend_hopper_counter)
        {
          // импульсы инкрементируем только после выдержки паузы
          if (OSTimeGet() - pend_hopper_timestamp > hopper_pause)
          {
            pend_hopper_counter = 0;
            HopperImpCounter++;
          }
        }
        OS_EXIT_CRITICAL();
            
        if (GetHopperCount())
        {
          if (last_hopper_count == GetHopperCount())
          {
              if (labs(OSTimeGet() - last_hopper_time) > 500)
              {
                PostUserEvent(EVENT_HOPPER_EXTRACTED);
              }
          }
          else
          {
              last_hopper_count = GetHopperCount();
              last_hopper_time = OSTimeGet();
          }
        }
        else
        {
          last_bank_time = OSTimeGet();
        }
      }
      else
      {
          HopperDisable();
          GetResetHopperCount();
          
          OS_ENTER_CRITICAL();
          pend_hopper_counter = 0;
          pend_hopper_timestamp = 0;
          OS_EXIT_CRITICAL();
      }
      
      OS_ENTER_CRITICAL();

      if (pend_upsignal_error_hopper_counter)
      {
        if (OSTimeGet() - pend_signal_error_hopper_timestamp > signal_error_hopper_pulse)
        {
          // сигнал ошибки снят
          PostUserEvent(EVENT_ERROR_HOPPER_ON);
          pend_upsignal_error_hopper_counter = 0;
        }
      }
      
      if (pend_downsignal_error_hopper_counter)
      {
        if (OSTimeGet() - pend_signal_error_hopper_timestamp > signal_error_hopper_pulse)
        {
          // сигнал ошибки появился
          PostUserEvent(EVENT_ERROR_HOPPER_OFF);
          pend_downsignal_error_hopper_counter = 0;
        }
      }

      if (pend_upsignal_nomoney_hopper_counter)
      {
        if (OSTimeGet() - pend_signal_nomoney_hopper_timestamp > signal_nomoney_hopper_pulse)
        {
          // деньги в хоппере есть
          PostUserEvent(EVENT_NOMONEY_HOPPER_ON);
          pend_upsignal_nomoney_hopper_counter = 0;
        }
      }
      
      if (pend_upsignal_button_counter)
      {
        if (OSTimeGet() - pend_signal_button_timestamp > signal_button_pulse)
        {
          // кнопку нажали
          PostUserEvent(EVENT_BUTTON_PRESS);
          pend_upsignal_button_counter = 0;
        }
      }
      
      OS_EXIT_CRITICAL();
    }
}

void CoinDisable(void)
{

}

void CoinEnable(void)
{

}

void BankDisable(void)
{

}

void BankEnable(void)
{

}

void HopperDisable(void)
{

}

void HopperEnable(void)
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

// получить число импульсов от банковского терминала
CPU_INT32U GetbankCount()
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = BankImpCounter;
  OS_EXIT_CRITICAL();
  return ctr;
}

// получить число импульсов от банковского терминала и сбросить счетчик
CPU_INT32U GetResetbankCount()
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = BankImpCounter;
  BankImpCounter = 0;
  OS_EXIT_CRITICAL();
  return ctr;
}

// получить число импульсов от хоппера
CPU_INT32U GetHopperCount()
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = HopperImpCounter;
  OS_EXIT_CRITICAL();
  return ctr;
}

// получить число импульсов от хоппера и сбросить счетчик
CPU_INT32U GetResetHopperCount()
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = HopperImpCounter;
  HopperImpCounter = 0;
  OS_EXIT_CRITICAL();
  return ctr;
}

// инициализация монетоприемника
void InitCoin(void)
{
  CoinImpCounter = 0;
  CashImpCounter = 0;
  BankImpCounter = 0;
  HopperImpCounter = 0;
  
  InitImpInput();
  OSTaskCreate(CoinTask, (void *)0, (OS_STK *)&CoinTaskStk[COIN_TASK_STK_SIZE-1], COIN_TASK_PRIO);
}

CPU_INT32U input_register()
{
  CPU_INT32U input = 0;
  
  // 0 бит монетоприемник
  if (FIO0PIN_bit.P0_23)
  {
     SETBIT(input, 0);
  }
  // 1 бит
  if (FIO0PIN_bit.P0_26)
  {
     SETBIT(input, 1);
  }
  // 2 бит
  if (FIO1PIN_bit.P1_25)
  {
     SETBIT(input, 2);
  }
  // 3 бит
  if (FIO1PIN_bit.P1_24)
  {
     SETBIT(input, 2);
  }
  // 4 бит
  if (FIO1PIN_bit.P1_23)
  {
     SETBIT(input, 3);
  }
  // 5 бит
  if (FIO1PIN_bit.P1_20)
  {
     SETBIT(input, 3);
  }
  
  return input;
}

static CPU_INT32U input_event = 0;
static CPU_INT32U prev_input = 0;
static CPU_INT32U curr_input = 0;

CPU_INT32U T3CR = 0;

void InputCapture_ISR(void)
{
  T3IR = 0xFF;

  // наращиваем тики
  T3CR++;

  curr_input = input_register();
  input_event = curr_input^prev_input;
  prev_input = curr_input;

  // монетоприемник
  if(TSTBIT(input_event, 0))
  {
    if ((!FIO0PIN_bit.P0_23 && coinLevel) || (FIO0PIN_bit.P0_23 && !coinLevel))
      { // пришел задний фронт
        CPU_INT32U cr=T3CR;
        cr -= period_coin;
        
        if (cr > (coin_pulse - COIN_IMP_SPAN))
        {
            pend_coin_counter = 1;
            pend_coin_timestamp = OSTimeGet();
        }
      }
    else
      { // пришел передний фронт
        period_coin = T3CR;
        pend_coin_counter = 0;
      }
  }

  // банковский терминал
  if(TSTBIT(input_event, 1))
  {
    if ((!FIO0PIN_bit.P0_26 && bankLevel) || (FIO0PIN_bit.P0_26 && !bankLevel))
      { // пришел задний фронт
        CPU_INT32U cr=T3CR;
        cr -= period_bank;
        
        if (cr > (bank_pulse - COIN_IMP_SPAN))
        {
            pend_bank_counter = 1;
            pend_bank_timestamp = OSTimeGet();
        }
      }
    else
      { // пришел передний фронт
        period_bank = T3CR;
        pend_bank_counter = 0;
      }
  }
 
  // хоппер в режиме импульсов
  if(TSTBIT(input_event, 2))
  {
    if ((!FIO1PIN_bit.P1_25 && hopperLevel) || (FIO1PIN_bit.P1_25 && !hopperLevel))
      { // пришел задний фронт
        CPU_INT32U cr=T3CR;
        cr -= period_hopper;
        
        if (cr > (hopper_pulse - COIN_IMP_SPAN))
        {
            pend_hopper_counter = 1;
            pend_hopper_timestamp = OSTimeGet();
        }
      }
    else
      { // пришел передний фронт
        period_hopper = T3CR;
        pend_hopper_counter = 0;
      }
  }
  
  // сигнал ошибки хоппера  
  if(TSTBIT(input_event, 3))
  {
    pend_signal_error_hopper_timestamp = OSTimeGet();
    
    if ((FIO1PIN_bit.P1_23 && hopperLevel) || (!FIO1PIN_bit.P1_23 && !hopperLevel))
      {
          pend_upsignal_error_hopper_counter = 1;
          pend_downsignal_error_hopper_counter = 0;
      }
    else
      {
          pend_upsignal_error_hopper_counter = 0;
          pend_downsignal_error_hopper_counter = 1;
      }
  }
  
  // сигнал отсутствия денег в хоппере
  if(TSTBIT(input_event, 4))
  {
    pend_signal_nomoney_hopper_timestamp = OSTimeGet();
    
    if ((FIO1PIN_bit.P1_24 && hopperLevel) || (!FIO1PIN_bit.P1_24 && !hopperLevel))
      {
          pend_upsignal_nomoney_hopper_counter = 1;
          pend_downsignal_nomoney_hopper_counter = 0;
      }
    else
      {
          pend_upsignal_nomoney_hopper_counter = 0;
          pend_downsignal_nomoney_hopper_counter = 1;
      }
  }
  
  // сигнал нажатия кнопки
  if(TSTBIT(input_event, 5))
  {
    pend_signal_button_timestamp = OSTimeGet();
    
    if (FIO1PIN_bit.P1_20)
      {
          pend_upsignal_button_counter = 1;
          pend_downsignal_button_counter = 0;
      }
    else
      {
          pend_upsignal_button_counter = 0;
          pend_downsignal_button_counter = 1;
      }
  }
}

extern CPU_INT32U  BSP_CPU_PclkFreq (CPU_INT08U  pclk);

void InitInputPorts()
{
    // монетоприемник
    PINSEL1_bit.P0_23 = 0;
    if(cashLevel)PINMODE1_bit.P0_23 = 3;
    else PINMODE1_bit.P0_23 = 0;
    FIO0DIR_bit.P0_23  = 0;
    FIO0MASK_bit.P0_23 = 0;
    
    // банковский терминал
    PINSEL1_bit.P0_26 = 0;
    if(bankLevel)PINMODE1_bit.P0_26 = 3;
    else PINMODE1_bit.P0_26 = 0;
    FIO0DIR_bit.P0_26  = 0;
    FIO0MASK_bit.P0_26 = 0;
    
    // хоппер в режиме импульсов
    PINSEL3_bit.P1_25 = 0;
    if(hopperLevel)PINMODE3_bit.P1_25 = 3;
    else PINMODE3_bit.P1_25 = 0;
    FIO1DIR_bit.P1_25  = 0;
    FIO1MASK_bit.P1_25 = 0;
    
    // сигнал состояния хоппера
    PINSEL3_bit.P1_23 = 0;
    PINMODE3_bit.P1_23 = 0;
    FIO1DIR_bit.P1_23  = 0;
    FIO1MASK_bit.P1_23 = 0;
    
    // сигнал наличия монет в хоппере
    PINSEL3_bit.P1_24 = 0;
    PINMODE3_bit.P1_24 = 0;
    FIO1DIR_bit.P1_24  = 0;
    FIO1MASK_bit.P1_24 = 0;
    
    // сигнал наличия монет в хоппере
    PINSEL3_bit.P1_20 = 0;
    PINMODE3_bit.P1_20 = 0;
    FIO1DIR_bit.P1_20  = 0;
    FIO1MASK_bit.P1_20 = 0;
}

/*
P0.23	MK_P9	импульсный выход монетоприемника
P0.26   MK_P6   импульсный выход банковского терминала
P1.25   MK_P39  импульсный выход хоппера

P1.23   MK_P37  Security Output с хоппером все в порядке - LOW
P1.24   MK_P38  Низкий уровень монет. Есть монеты - сигнал LOW.

P1.20   MK_P34  Сигнал от кнопки
*/

// инициализация импульсного входа
// используется CAP3.0
void  InitImpInput (void)
{
    #define INPUT_CAPTURE_FREQ  1000  // частота тактирования частотных входов
  
    CPU_INT32U  pclk_freq;
    CPU_INT32U  rld_cnts;

    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif

    OnChangeCoinPulseLen();
    OnChangeCashPulseLen();
    OnChangeBankPulseLen();
    OnChangeHopperPulseLen();
     
    OS_ENTER_CRITICAL();
    
    // назначим все ножки
    InitInputPorts();
 
    PCONP_bit.PCTIM3 = 1;
    PCLKSEL1_bit.PCLK_TIMER3 = 2;

    pclk_freq         =   BSP_CPU_PclkFreq(23);
    rld_cnts          =   pclk_freq / INPUT_CAPTURE_FREQ / 2;

    T3CTCR_bit.CTM    =   0;
    T3CTCR_bit.CIS    =   0;          // select CAP3.0 input
    T3PR              =   rld_cnts-1;
    
    T3MR0             =   1;
    T3MCR             =   3;
    
    T3CCR             =   0x00;
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

