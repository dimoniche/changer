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

CPU_INT08U  coin_enabled;
CPU_INT08U  bank_enabled;

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
static char pend_upsignal_error_hopper_counter = 0;
static char pend_downsignal_error_hopper_counter = 0;
static CPU_INT32U pend_signal_error_hopper_timestamp;

static CPU_INT32U signal_nomoney_hopper_pulse = 1000;
static char pend_upsignal_nomoney_hopper_counter = 0;
volatile char event_nomoney_hopper = 0;
static char pend_downsignal_nomoney_hopper_counter = 0;
static CPU_INT32U pend_signal_nomoney_hopper_timestamp;

static CPU_INT32U cashLevel;
static CPU_INT32U coinLevel;
static CPU_INT32U bankLevel;
static CPU_INT32U hopperLevel;

CPU_INT32U period_coin;
CPU_INT32U period_bank;
CPU_INT32U period_signal;
CPU_INT32U period_hopper;

CPU_INT32U firstHopperEvent = 1;

// ����� �������
CPU_INT32U regime_hopper = 0;

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
  hopper_pulse = pulse * 1;
  hopper_pause = pause;
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
  
  CPU_INT32U last_coin_count = GetCoinCount();
  CPU_INT32U last_coin_time = OSTimeGet();

  CPU_INT32U last_bank_count = GetbankCount();
  CPU_INT32U last_bank_time = OSTimeGet();

  CPU_INT32U last_hopper_count = GetHopperCount();
  CPU_INT32U last_hopper_time = OSTimeGet();

  CPU_INT32U last_settings_time = OSTimeGet();

  GetData(&EnableCoinDesc, &enable_coin, 0, DATA_FLAG_SYSTEM_INDEX);  
  GetData(&EnableBankDesc, &bank_enable, 0, DATA_FLAG_SYSTEM_INDEX);

  // ��������� ���������� ����� ������ �������
  GetData(&RegimeHopperDesc, &regime_hopper, 0, DATA_FLAG_SYSTEM_INDEX);

  while(1)
    {
      if (OSTimeGet() - last_settings_time > 1000)
      {
          last_settings_time = OSTimeGet();
          GetData(&EnableCoinDesc, &enable_coin, 0, DATA_FLAG_SYSTEM_INDEX);
          GetData(&EnableBankDesc, &bank_enable, 0, DATA_FLAG_SYSTEM_INDEX);

          // ��������� ���������� ����� ������ �������
          GetData(&RegimeHopperDesc, &regime_hopper, 0, DATA_FLAG_SYSTEM_INDEX);
      }
            
      OSTimeDly(1);
      
      #if OS_CRITICAL_METHOD == 3
      OS_CPU_SR  cpu_sr = 0;
      #endif

      if (enable_coin && coin_enabled)
      {
        // ��� ���������� ��������� �������� ������ �������
        FIO1CLR_bit.P1_31 = 1;
        
        OS_ENTER_CRITICAL();
        if (pend_coin_counter)
        {
          // �������� �������������� ������ ����� �������� �����
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
          // ��� ������� ��������� �������� ������� �������
          FIO1SET_bit.P1_31 = 1;

          if (enable_coin && GetCoinCount())
          {
              //PostUserEvent(EVENT_COIN_INSERTED);
          }
          else
          {
              GetResetCoinCount();
          }
          
          OS_ENTER_CRITICAL();
          pend_coin_counter = 0;
          pend_coin_timestamp = 0;
          OS_EXIT_CRITICAL();
      }
      
      if (bank_enable && bank_enabled)
      {
        // ��� ���������� ��������� �������� ������ �������
        FIO0CLR_bit.P0_25 = 1;
        
        OS_ENTER_CRITICAL();
        if (pend_bank_counter)
        {
          // �������� �������������� ������ ����� �������� �����
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
          // ��� ������� ����� �������� ������� �������
          FIO0SET_bit.P0_25 = 1;

          if (bank_enable && GetbankCount())
          {
              //PostUserEvent(EVENT_BANK_INSERTED);
          }
          else
          {
              GetResetbankCount();
          }
          
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
              pend_hopper_counter = 0;
              
              if(firstHopperEvent) { HopperImpCounter = 0; firstHopperEvent = 0; }
              else HopperImpCounter++;
          }
          OS_EXIT_CRITICAL();
              
          if (GetHopperCount())
          {
              if (last_hopper_count < GetHopperCount())
              {
                  // ������� �� ������� ���� ����� ����� - ����� ������ ��� ������� ����������
                  PostUserEvent(EVENT_HOPPER_EXTRACTED);
                  last_hopper_count = GetHopperCount();
              }
          }
          else
          {
              last_hopper_time = OSTimeGet();
              last_hopper_count = 0;
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

      if (!regime_hopper)
      {
          if (pend_upsignal_error_hopper_counter)
          {
              if (OSTimeGet() - pend_signal_error_hopper_timestamp > signal_error_hopper_pulse)
              {
                // ������ ������ ����
                PostUserEvent(EVENT_ERROR_HOPPER_ON);
                pend_upsignal_error_hopper_counter = 0;
              }
          }
      
          if (pend_downsignal_error_hopper_counter)
          {
              if (OSTimeGet() - pend_signal_error_hopper_timestamp > signal_error_hopper_pulse)
              {
                // ������ ������ ��������
                PostUserEvent(EVENT_ERROR_HOPPER_OFF);
                pend_downsignal_error_hopper_counter = 0;
              }
          }
      }

      if (pend_upsignal_nomoney_hopper_counter)
      {
        if (OSTimeGet() - pend_signal_nomoney_hopper_timestamp > signal_nomoney_hopper_pulse)
        {
          // ������ � ������� ���������
          PostUserEvent(EVENT_NOMONEY_HOPPER_ON);
          event_nomoney_hopper = 1;
          pend_upsignal_nomoney_hopper_counter = 0;
        }
      }

      if (pend_downsignal_nomoney_hopper_counter)
      {
        if (OSTimeGet() - pend_signal_nomoney_hopper_timestamp > signal_nomoney_hopper_pulse)
        {
          // ������ � ������� ����
          PostUserEvent(EVENT_NOMONEY_HOPPER_OFF);
          event_nomoney_hopper = 0;
          pend_downsignal_nomoney_hopper_counter = 0;
        }
      }
      
      OS_EXIT_CRITICAL();
    }
}

void CoinDisable(void)
{
    coin_enabled = 0;
}

void CoinEnable(void)
{
    coin_enabled = 1;
}

void BankDisable(void)
{
    bank_enabled = 0;
}

void BankEnable(void)
{
    bank_enabled = 1;
}

void HopperDisable(void)
{

}

void HopperEnable(void)
{

}

// �������� ����� �����
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

// �������� ����� ����� � �������� �������
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

// �������� ����� ��������� �� ���������
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

// �������� ����� ��������� �� ��������� � �������� �������
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

// �������� ����� ��������� �� ����������� ���������
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

// �������� ����� ��������� �� ����������� ��������� � �������� �������
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

// �������� ����� ��������� �� �������
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

// �������� ����� ��������� �� ������� � �������� �������
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

// ������������� ���������������
void InitCoin(void)
{
  CoinImpCounter = 0;
  CashImpCounter = 0;
  BankImpCounter = 0;
  HopperImpCounter = 0;
  
  coin_enabled = 0;
  bank_enabled = 0;
  
  InitImpInput();
  OSTaskCreate(CoinTask, (void *)0, (OS_STK *)&CoinTaskStk[COIN_TASK_STK_SIZE-1], COIN_TASK_PRIO);
}

CPU_INT32U input_register()
{
  CPU_INT32U input = 0;
  
  // 0 ��� ��������������
  if (FIO0PIN_bit.P0_23)
  {
     SETBIT(input, 0);
  }
  // 1 ��� - ��������� ��������
  if (FIO0PIN_bit.P0_26)
  {
     SETBIT(input, 1);
  }
  
  if(regime_hopper)
  {
      // 2 ��� - �������� �� ������� � ������ Cube
      if (FIO1PIN_bit.P1_23)
      {
         SETBIT(input, 2);
      }
  }
  else
  {
      // 3 ��� - ������� ����� ������ �� ������� � ������� ������
      if (FIO1PIN_bit.P1_23)
      {
         SETBIT(input, 3);
      }
  }

  // 4 ��� - ������ ������� - ��� �����
  if (FIO1PIN_bit.P1_24)
  {
     SETBIT(input, 4);
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

  // ���������� ����
  T3CR++;

  curr_input = input_register();
  input_event = curr_input^prev_input;
  prev_input = curr_input;

  // ��������������
  if(TSTBIT(input_event, 0))
  {
    if ((!FIO0PIN_bit.P0_23 && coinLevel) || (FIO0PIN_bit.P0_23 && !coinLevel))
      { // ������ ������ �����
        CPU_INT32U cr=T3CR;
        cr -= period_coin;
        
        if (cr > (coin_pulse - COIN_IMP_SPAN))
        {
            pend_coin_counter = 1;
            pend_coin_timestamp = OSTimeGet();
        }
      }
    else
      { // ������ �������� �����
        period_coin = T3CR;
        pend_coin_counter = 0;
      }
  }

  // ���������� ��������
  if(TSTBIT(input_event, 1))
  {
    if ((!FIO0PIN_bit.P0_26 && bankLevel) || (FIO0PIN_bit.P0_26 && !bankLevel))
      { // ������ ������ �����
        CPU_INT32U cr=T3CR;
        cr -= period_bank;
        
        if (cr > (bank_pulse - COIN_IMP_SPAN))
        {
            pend_bank_counter = 1;
            pend_bank_timestamp = OSTimeGet();
        }
      }
    else
      { // ������ �������� �����
        period_bank = T3CR;
        pend_bank_counter = 0;
      }
  }
 
  // ������ � ������ ���������
  if(TSTBIT(input_event, 2))
  {
    if (FIO1PIN_bit.P1_23)
      {   // ������ ������ �����
          pend_hopper_counter = 1;
          pend_hopper_timestamp = OSTimeGet();
      }
  }
  
  // ������ ������ �������  
  if(TSTBIT(input_event, 3))
  {
    pend_signal_error_hopper_timestamp = OSTimeGet();
    
    if (FIO1PIN_bit.P1_23)
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
  
  // ������ ���������� ����� � �������
  if(TSTBIT(input_event, 4))
  {
    pend_signal_nomoney_hopper_timestamp = OSTimeGet();
    
    if (FIO1PIN_bit.P1_24)
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
}

extern CPU_INT32U  BSP_CPU_PclkFreq (CPU_INT08U  pclk);

/*
P0.23	MK_P9	���������� ����� ���������������
P0.26   MK_P6   ���������� ����� ����������� ���������

P1.23   MK_P37  Security Output � �������� ��� � ������� - LOW, � ������ Cube - ���������� ����� �������, ������� ��������
P1.24   MK_P38  ������ ������� �����. ���� ������ - ������ LOW.

*/
void InitInputPorts()
{
    // ��������� ���������� ����� ������ �������
    GetData(&RegimeHopperDesc, &regime_hopper, 0, DATA_FLAG_SYSTEM_INDEX);

    // ��������������
    PINSEL1_bit.P0_23 = 0;
    if(cashLevel)PINMODE1_bit.P0_23 = 3;
    else PINMODE1_bit.P0_23 = 0;
    FIO0DIR_bit.P0_23  = 0;
    FIO0MASK_bit.P0_23 = 0;
    
    // ���������� ��������
    PINSEL1_bit.P0_26 = 0;
    if(bankLevel)PINMODE1_bit.P0_26 = 3;
    else PINMODE1_bit.P0_26 = 0;
    FIO0DIR_bit.P0_26  = 0;
    FIO0MASK_bit.P0_26 = 0;
    
    if(regime_hopper)
    {
        // ������ � ������ ��������� - ����� Cube - ����� ���������� �������
        PINSEL3_bit.P1_23 = 0;
        if(hopperLevel)PINMODE3_bit.P1_23 = 3;
        else PINMODE3_bit.P1_23 = 0;
        FIO1DIR_bit.P1_23  = 0;
        FIO1MASK_bit.P1_23 = 0;
    }
    else
    {
        // ������ ��������� ������� - ����� ���������� ����������
        PINSEL3_bit.P1_23 = 0;
        PINMODE3_bit.P1_23 = 0;
        FIO1DIR_bit.P1_23  = 0;
        FIO1MASK_bit.P1_23 = 0;
    }
    
    // ������ ������� ����� � �������
    PINSEL3_bit.P1_24 = 0;
    PINMODE3_bit.P1_24 = 0;
    FIO1DIR_bit.P1_24  = 0;
    FIO1MASK_bit.P1_24 = 0;
}

/*
P0.24	MK_P8	���������� ��������
P0.25   MK_P7   ������ ����������� ���������
P1.31   MK_P20  ������ ���������
*/

void initHopper(void)
{
    // ���������� ��������: ������ ��������� �� ������ ��� ����������� ������ - HIGH - ���������� � ���������� ������ ��������, �� �������� ����� ��������
    PINSEL1_bit.P0_24 = 0;
    PINMODE1_bit.P0_24 = 0;
    FIO0DIR_bit.P0_24  = 1;
    FIO0MASK_bit.P0_24 = 0;
    
    FIO0CLR_bit.P0_24 = 1; // LOW
}

// ��������� �������� ��� ����������
void initOutputPorts(void)
{
    initHopper();

    // ������ ����������� ��������� - HIGH - ������
    PINSEL1_bit.P0_25 = 0;
    PINMODE1_bit.P0_25 = 0;
    FIO0DIR_bit.P0_25  = 1;
    FIO0MASK_bit.P0_25 = 0;
    
    // ������ ��������� - HIGH - ������
    PINSEL3_bit.P1_31 = 0;
    PINMODE3_bit.P1_31 = 0;
    FIO1DIR_bit.P1_31  = 1;
    FIO1MASK_bit.P1_31 = 0; 

    FIO0CLR_bit.P0_25 = 1; // LOW
    FIO1CLR_bit.P1_31 = 1; // LOW
}

// ������������� ����������� �����
// ������������ CAP3.0
void  InitImpInput (void)
{
    #define INPUT_CAPTURE_FREQ  1000  // ������� ������������ ��������� ������
  
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
    
    // �������� ��� �����
    InitInputPorts();
    // �������������� �������� �����
    initOutputPorts();

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

