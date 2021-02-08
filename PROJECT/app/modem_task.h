#ifndef _MODEM_TASK_H_
#define _MODEM_TASK_H_

#include "app_serv.h"

// длина очереди заданий модема
#define     MODEM_QUERY_LEN     8
// задержка между выполнением заданий модема
#define     MODEM_TASK_DELAY    10000

enum{
  MODEM_TASK_NONE = 0,
  MODEM_TASK_SEND_STATISTICS,
  MODEM_TASK_SEND_TEST_MSG,
  MODEM_TASK_RECONNECT,
  MODEM_TASK_SEND_INCAS,

};

extern OS_STK    ModemTaskStk[MODEM_TASK_STK_SIZE];
extern OS_EVENT *ModemQuery;
extern void     *ModemTbl[MODEM_QUERY_LEN];


extern void  ModemTask(void *p_arg);
extern void PostModemTask(int new_task);

#endif //#ifndef _MODEM_TASK_H_
