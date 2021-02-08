#ifndef _APP_SERV_H_
#define _APP_SERV_H_

#include "app_cfg.h"

#define CONSOLE_TCP_DEFAULT_PORT    10000


extern CPU_INT32U incas_bill_nom_counter[24];
extern CPU_INT32U incas_common_bill_counter;

#define   KBRD_TASK_STK_SIZE        128
#define   USER_TASK_STK_SIZE        384
#define   MENU_TASK_STK_SIZE        384
#define   COIN_TASK_STK_SIZE        128
#define   VALIDATOR_TASK_STK_SIZE   384
#define   FISCAL_TASK_STK_SIZE      384
#define   MODEM_TASK_STK_SIZE       768

#define  CONSOLE_TASK_STK_SIZE      256
#define  HOST_TASK_STK_SIZE         256


#define   VALIDATOR_TASK_PRIO   USER_HIGHEST_PRIO
#define   USER_TASK_PRIO        (USER_HIGHEST_PRIO + 1)
#define   COIN_TASK_PRIO        (USER_HIGHEST_PRIO + 2)
#define   KBRD_TASK_PRIO        (USER_HIGHEST_PRIO + 3)
#define   FISCAL_TASK_PRIO      (USER_HIGHEST_PRIO + 4)
#define   MENU_TASK_PRIO        (USER_HIGHEST_PRIO + 5)

#define  CONSOLE_TASK_PRIO      (USER_HIGHEST_PRIO + 6)
#define  HOST_TASK_PRIO         (USER_HIGHEST_PRIO + 7)

#define   MODEM_TASK_PRIO       USER_LOWEST_PRIO


enum{
  EVENT_SEC = 1,
  EVENT_STARTUP,
  
  EVENT_COIN_INSERTED,
  EVENT_CASH_INSERTED,
  EVENT_BILL_ESCROW,
  EVENT_BILL_STACKED,
  
  EVENT_MODE_CHANGE,
  
  EVENT_KEY_EMPTY,
  EVENT_KEY_F1,
  EVENT_KEY_F2,
  EVENT_KEY_F3,
  EVENT_KEY_LEFT,
  EVENT_KEY_UP,
  EVENT_KEY_RIGHT,
  EVENT_KEY_CANSEL,
  EVENT_KEY_DOWN,
  EVENT_KEY_START,
  EVENT_KEY_USER_START,

  EVENT_INCASSATION,
  EVENT_INCASSATION_FINISH,
  #ifdef BOARD_POST_CFG
  EVENT_PULSEOUT
  #endif
};

#define EVENT_KEY_LEFT EVENT_KEY_POST2
#define EVENT_KEY_RIGHT EVENT_KEY_POST5

extern void UserStartupFunc(void);
extern void PostUserEvent(int event);

extern void InitUserMenu(void);
extern int GetRecentChannelPrice(CPU_INT08U ch, CPU_INT32U* price, CPU_INT32U* time);

extern void save_config_params(void);
extern void init_config_params(void);
extern void AddOutPulses(int count, int len_ms);

#endif //#ifndef _APP_SERV_H_
