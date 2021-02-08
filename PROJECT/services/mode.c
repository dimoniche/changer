#include <includes.h>
#include "mode.h"
#include "app_serv.h"

// текущий режим работы
CPU_INT08U  RecentMode;


void InitMode(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  
  PINSEL3_bit.P1_22 = 0x0;
  PINMODE3_bit.P1_22 = 0;
  FIO1DIR_bit.P1_22 = 0;
  FIO1MASK_bit.P1_22 = 0;
  
  OS_EXIT_CRITICAL();

  OSTimeDly(1);
  
  OS_ENTER_CRITICAL();
  if (FIO1PIN_bit.P1_22) RecentMode = MODE_WORK;
  else RecentMode = MODE_SERVICE;
  OS_EXIT_CRITICAL();
}


CPU_INT08U GetMode(void)
{
  CPU_INT08U mode;
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  mode = RecentMode;
  OS_EXIT_CRITICAL();
  return mode;
}


CPU_INT08U CheckMode(void)
{
  CPU_INT08U newmode;
  if (FIO1PIN_bit.P1_22) newmode = MODE_WORK;
  else newmode = MODE_SERVICE; 
  if (newmode != RecentMode)
    {
      RecentMode = newmode;
      PostUserEvent(EVENT_MODE_CHANGE);
      return 1;
    }
  return 0;  
}
