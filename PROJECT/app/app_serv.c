#include <includes.h>
#include "app_serv.h"
#include "modem.h"
#include "validator.h"
#include "coin.h"
#include "time.h"
#include "fiscal.h"
#include "menu.h"
#include "data.h"
#include "mode.h"
#include "menudesc.h"
#include "datadesc.h"
#include "control.h"
#include "validator.h"
#include "CCRSProtocol.h"
#include "menu.h"
#include "journal.h"
#include "fr.h"
#include "CRC16.h"
#include "modem_task.h"
#include "host_app.h"
#include "console.h"
#include "keyboard.h"

// если определить этот макрос, будут вноситься деньги по кнопке F1
//#define _DEBUG_MONEY

CPU_INT32U SystemTime;
CPU_INT32U money_timestamp;
CPU_INT08U EnabledChannelsNum;
CPU_INT08U RecentChannel;
CPU_INT08U UserMenuState;
  #define USER_STATE_FIRST_PAGE                 0
  #define USER_STATE_ACCEPT_MONEY               1
  #define USER_STATE_SHOW_THANKS                2
CPU_INT08U ThanksCtr;

CPU_INT08U ChannelsState[CHANNELS_NUM];
  #define CHANNEL_STATE_FREE            0
  #define CHANNEL_STATE_BUSY            1
  #define CHANNEL_STATE_DISABLED        2
CPU_INT32U ChannelsCounters[CHANNELS_NUM];
CPU_INT32U ChannelsPayedTime[CHANNELS_NUM];

extern CPU_INT32U BillNominals[24];
CPU_INT32U incas_bill_nom_counter[24];
CPU_INT32U incas_common_bill_counter;

#define USER_QUERY_LEN  64

OS_STK    UserTaskStk[USER_TASK_STK_SIZE];
OS_EVENT *UserQuery = NULL;
void     *UserTbl[USER_QUERY_LEN];

int GetUserEvent(int* event);
void UserPrintMoneyMenu(void);
void WorkServer(void);
void UserPrintThanksMenu(void);
void UserPrintFirstMenu(void);
void UserPrintErrorMenu(void);
CPU_INT32U GetChannelsTimeForFree(CPU_INT08U ch);
void LoadAcceptedMoney(void);
void SetAcceptedMoney(CPU_INT32U money);
void ClearAcceptedMoney(void);
CPU_INT32U GetAcceptedMoney(void);
void InitPass(void);
int CheckChannelEnabled(CPU_INT08U channel);
int ChannelBusy(CPU_INT08U ch);
void UserPrintIpDeviceErrMenu(CPU_INT08U post);
void UserPrintPrintBillMenu(void);

#ifdef BOARD_CENTRAL_CFG
static char incassation;
static char was_critical_error;
#endif

#ifdef BOARD_POST_CFG

static int out_pulse_count = 0;
static int out_pulse_len = 0;

void AddOutPulses(int count, int len_ms)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    OS_ENTER_CRITICAL();
    
    out_pulse_count += count;
    out_pulse_len = len_ms;

    OS_EXIT_CRITICAL();
}

#endif

CPU_INT32U FindBillIndex(CPU_INT32U nom)
{
  CPU_INT32U cash_mode;      
  GetData(&CashModeDesc, &cash_mode, 0, DATA_FLAG_SYSTEM_INDEX);
  if (cash_mode == 1)
  {
      // проставим номиналы вручную
      BillNominals[0] = 10;
      BillNominals[1] = 50;
      BillNominals[2] = 100;
      BillNominals[3] = 500;
      BillNominals[4] = 1000;
      BillNominals[5] = 5000;
  }
  
  for (int i = 0; i < 6; i++)
  {
    if (BillNominals[i] == nom) return i;
  }
  
  return 0xFFFFFFFF;
}

/*!
 Сервер обработки событий пользователя
*/
void UserAppTask(void *p_arg)
{
  CPU_INT32U print_timeout;
  CPU_INT32U print_mode;
  CPU_INT32U accmoney;
  int event;
#ifdef BOARD_CENTRAL_CFG
  CPU_INT32U temp;
#endif
  
#ifdef BOARD_CENTRAL_CFG
  
  static CPU_INT08U fr_conn_ctr = 0;

  {
    CPU_INT32U m=0;
    GetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);     
    if (m)
    {
         EnabledChannelsNum = 0;
         for (CPU_INT08U i=0; i<CHANNELS_NUM; i++)
         {
            CPU_INT32U en = 0;
            GetData(&EnableChannelDesc, &en, i, DATA_FLAG_DIRECT_INDEX);
            if (en) {EnabledChannelsNum++;}
         }
         UserMenuState = USER_STATE_ACCEPT_MONEY;
    }
  }
    
  incassation = 0;
  was_critical_error = 0;
  
  GetData(&IncasSendFlagDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
  if (temp == INCAS_SEND_FLAG)
  {
      PostModemTask(MODEM_TASK_SEND_INCAS);
  }
#endif
      
  while (1)
    {
      if (GetUserEvent(&event))
        {
          switch (event){
#ifdef BOARD_CENTRAL_CFG
            case EVENT_SEC:
              // проверка режима
              CheckMode();
             
              // прочитаем текущее время
              SystemTime = GetTimeSec();
                
              // рабочий сервер - счетчики, состояния и т.п.
              WorkServer();

              // проверим фискальник, если он отвалился
              if ((++fr_conn_ctr % 10) == 0)
              {
                 if ((FiscalConnState == FISCAL_NOCONN) || (TstCriticalFiscalError()))
                 {
                    if (ConnectFiscalFast() == 0)
                    {
                        CheckFiscalStatus();
                    }
                 }
              }

              // сервер ошибок
              ErrorServer();
                
              // дальше только в рабочем режиме
              if (GetMode() != MODE_WORK)
              {
                  break;
              }
                
              // если есть ошибки, не работаем
              if (TstCriticalErrors()) 
              {
                UserPrintErrorMenu(); 
                RefreshMenu(); 
                // выключим прием денег
                if (was_critical_error == 0)
                {
                    if (IsValidatorConnected()) CC_CmdBillType(0x000000, 0x000000, ADDR_FL);
                    CoinDisable();
                    was_critical_error = 1;
                }
                break;
              }
              
              // включим заново прием денег, если была ошибка
              if (was_critical_error)
              {
                  if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffff, ADDR_FL);
                  was_critical_error = 0;
                  break;
              }

              accmoney = GetAcceptedMoney();
              if (accmoney > 0)
              {
                  LED_OK_ON();
                  CheckFiscalStatus();
                  GetData(&PrintModeDesc, &print_mode, 0, DATA_FLAG_SYSTEM_INDEX);
                  if (print_mode == 0)
                  {
                      // если настроена печать ПО ТАЙМАУТУ
                      GetData(&PrintTimeoutDesc, &print_timeout, 0, DATA_FLAG_SYSTEM_INDEX);
                      if (labs(OSTimeGet() - money_timestamp) > 1000UL * print_timeout)
                      {
                          UserPrintPrintBillMenu();
                          RefreshMenu();
                          
                          // напечатаем чек
                          if (IsFiscalConnected())
                          {
                            if (PrintFiscalBill(accmoney) == 0)
                            {
                                SaveEventRecord(RecentChannel, JOURNAL_EVENT_PRINT_BILL, GetTimeSec());
                            }
                          }
                          IncCounter(RecentChannel, ChannelsPayedTime[RecentChannel], accmoney);
                          SetAcceptedMoney(0);
                          OSTimeDly(1000);
                          
                          // повесим меню "СПАСИБО"                      
                          if (IsFiscalConnected())
                          {
                              UserPrintThanksMenu();
                              RefreshMenu();
                          }
                          
                          if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffff, ADDR_FL);
                          
                          OSTimeDly(1000);
                          LED_OK_OFF();
                      }
                  }
                  else if (print_mode == 1)
                  {
                      // если настроена печать ПО КНОПКЕ, ждем таймаут отмены
                      GetData(&PrintTimeoutAfterDesc, &print_timeout, 0, DATA_FLAG_SYSTEM_INDEX);
                      if (labs(OSTimeGet() - money_timestamp) > 1000UL * print_timeout)
                      {
                          SetAcceptedMoney(0);
                          UserPrintThanksMenu();
                          RefreshMenu();
                          if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffff, ADDR_FL);
                          OSTimeDly(1000);
                          LED_OK_OFF();
                      }
                  }
              }
              else
              {
                  LED_OK_OFF();
              }
              
              // принимаем деньги
              UserPrintMoneyMenu();
              RefreshMenu();
              break;
            case EVENT_INCASSATION:
              {
                  CPU_INT32U incas_sum = 0, temp;              
                  for (CPU_INT32U i = 0; i < 24; i++)
                  {
                      CPU_INT32U val = 0;
                      GetData(&BillnomCountersDesc, &val, i, DATA_FLAG_DIRECT_INDEX);
                      incas_sum += val*BillNominals[i];
                  }              
                  incassation = 1;
                  sprintf((char*)str_IncasMenu_3, "  СУММА %u руб.", incas_sum);
                  // вешаем меню инкассация
                  GoToMenu(IncasMenuPanel);
                  // сохраним событие с указанием суммы денег
                  SaveEventRecord(0, JOURNAL_EVENT_INCASSATION, incas_sum);
                  GetData(&BillCounterDesc, &incas_common_bill_counter, 0, DATA_FLAG_SYSTEM_INDEX);
                  for (CPU_INT32U i = 0; i < 24; i++)
                  {
                     GetData(&BillnomCountersDesc, &incas_bill_nom_counter[i], i, DATA_FLAG_DIRECT_INDEX);
                  }
                  
                  SetData(&IncasMoneyDesc, &incas_sum, 0, DATA_FLAG_SYSTEM_INDEX);
                  
                  temp = GetTimeSec();
                  SetData(&IncasTimeDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
                  
                  temp = INCAS_SEND_FLAG;
                  SetData(&IncasSendFlagDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
                  PostModemTask(MODEM_TASK_SEND_INCAS);
                  // очищаем счетчики купюр
                  ClearBillnomCounter();
              }
              break;
            case EVENT_INCASSATION_FINISH:
              incassation = 0;
              GoToPreviousMenu();
              break;
            case EVENT_MODE_CHANGE:
              ReInitMenu();
              SaveEventRecord(0, JOURNAL_EVENT_CHANGE_MODE, GetMode());
              if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffff, ADDR_FL);
              break;
            case EVENT_COIN_INSERTED:
              {
                CPU_INT32U cpp = 1;
                CPU_INT32U money, accmoney;
                GetData(&CoinPerPulseDesc, &cpp, 0, DATA_FLAG_SYSTEM_INDEX);
                money = cpp*GetResetCoinCount();
                accmoney = GetAcceptedMoney();
                accmoney += money;
                SetAcceptedMoney(accmoney);
                money_timestamp = OSTimeGet();
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                    UserPrintMoneyMenu();  
                    RefreshMenu();
                }
                if (money) SaveEventRecord(RecentChannel, JOURNAL_EVENT_MONEY_COIN, money);
              }
              break;
            case EVENT_CASH_INSERTED:
              {
                CPU_INT32U cpp = 1;
                CPU_INT32U money, accmoney;
                GetData(&CashPerPulseDesc, &cpp, 0, DATA_FLAG_SYSTEM_INDEX);
                money = cpp * GetResetCashCount();
                accmoney = GetAcceptedMoney();
                accmoney += money;
                SetAcceptedMoney(accmoney);
                money_timestamp = OSTimeGet();
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                    UserPrintMoneyMenu();  
                    RefreshMenu();
                }
                if (money) SaveEventRecord(RecentChannel, JOURNAL_EVENT_MONEY_NOTE, money);
                CPU_INT32U billnom_index = FindBillIndex(money);
                if (billnom_index != 0xFFFFFFFF) IncBillnomCounter(billnom_index);
              }
              break;
            case EVENT_BILL_ESCROW:
                // купюра в положении возврата
                if (IsValidatorConnected()) if (!CC_CmdPack(ADDR_FL)) SetErrorFlag(ERROR_VALIDATOR_CONN);
                break;
            case EVENT_BILL_STACKED:
              // купюра уложена
              {
                CPU_INT32U billnom_index;
                CPU_INT32U note,accmoney;
                note = GetResetBillCount(&billnom_index);
                accmoney = GetAcceptedMoney();
                accmoney += note;
                SetAcceptedMoney(accmoney);
                money_timestamp = OSTimeGet();
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                    UserPrintMoneyMenu();  
                    RefreshMenu();
                }
                if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffffff, ADDR_FL);
                if (note)
                {
                    SaveEventRecord(RecentChannel, JOURNAL_EVENT_MONEY_NOTE, note);
                    IncBillnomCounter(billnom_index);
                }
              }
              break;                  

            case EVENT_KEY_CANSEL:
#if 0
              if ((GetMode() != MODE_WORK) || (incassation)) break;
              if (TstCriticalErrors())
              {
                  UserPrintErrorMenu(); 
                  RefreshMenu(); 
                  break;
              }
              UserMenuState = USER_STATE_FIRST_PAGE;
              UserPrintFirstMenu();
              RefreshMenu();
              if (IsValidatorConnected()) CC_CmdBillType(0x000000, 0x000000, ADDR_FL);
#endif
              break;
              
            case EVENT_KEY_START:
            case EVENT_KEY_USER_START:
              if (incassation) break;
              if (GetMode() != MODE_WORK)
                {
                  if (!FlagForPrintReport) break;
                  if (GetCurrentMenu() == xReportMenuPanel)
                    { // печатаем X-отчет
                     CPU_INT08U err;
                     if (IsFiscalConnected())
                      {
                        FPend();
                        FiscPrintDayReportNoClear(30, &err);
                        FPost();
                        if (err) {SetFiscalErrorByCode(err);}
                        SaveEventRecord(0, JOURNAL_EVENT_PRINT_X, GetTimeSec());
                        GoToPreviousMenu();
                      }
                    }
                  else if (GetCurrentMenu() == zReportMenuPanel)
                   { // печатаем Z-отчет
                     CPU_INT08U err;
                     if (IsFiscalConnected()) 
                      {
                        FPend();
                        FiscPrintDayReportClear(30, &err);
                        FPost();
                        if (err) {SetFiscalErrorByCode(err);}
                        SaveEventRecord(0, JOURNAL_EVENT_PRINT_Z, GetTimeSec());
                        GoToPreviousMenu();
                        ClrFiscalErrorByCode(FR_ERROR_CODE_4E);
                      }
                   }
                  else if (GetCurrentMenu() == bufReportMenuPanel)
                   { // печатаем Z-отчеты из буфера
                     CPU_INT08U err;
                     if (IsFiscalConnected()) 
                      {
                        FPend();
                        FiscPrintDayReportsFromBuf(30, &err);
                        FPost();
                        if (err) {SetFiscalErrorByCode(err);}
                        SaveEventRecord(0, JOURNAL_EVENT_PRINT_BUF, GetTimeSec());
                        GoToPreviousMenu();
                      }
                   }
                  break;
                }
            
              if (TstCriticalErrors()) 
              {
                UserPrintErrorMenu(); 
                RefreshMenu(); 
                break;
              }
              
              // --------------------------
              // находимся в рабочем режиме
              // --------------------------
                GetData(&PrintModeDesc, &print_mode, 0, DATA_FLAG_SYSTEM_INDEX);
                if (print_mode == 1)
                {
                  // пользователь внес деньги и нажал СТАРТ + режим печати ПО КНОПКЕ
                  CPU_INT32U accmoney = GetAcceptedMoney();
                  
                  if (accmoney > 0)
                  { 
                      UserPrintPrintBillMenu();
                      RefreshMenu();
                      
                      // напечатаем чек
                      if (IsFiscalConnected())
                      {
                        if (PrintFiscalBill(accmoney) == 0)
                        {
                            SaveEventRecord(RecentChannel, JOURNAL_EVENT_PRINT_BILL, GetTimeSec());
                        }
                      }
                      IncCounter(RecentChannel, ChannelsPayedTime[RecentChannel], accmoney);
                      SetAcceptedMoney(0);
                      OSTimeDly(1000);
                     
                      // повесим меню "СПАСИБО"                      
                      if (IsFiscalConnected())
                      {
                          UserPrintThanksMenu();
                          RefreshMenu();
                      }
                      
                      if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffff, ADDR_FL);
                      
                      OSTimeDly(1000);
                      LED_OK_OFF();
                  }
                }
                
                  
              break;
#else

#endif
            default:
              break;
          }
        }
      else 
        {
          OSTimeDly(1);
        }
    }
}

/*!
 Пользовательская инициализация
*/
void UserStartupFunc(void)
{
#ifdef BOARD_CENTRAL_CFG
  // инициализация режима работы
  InitMode();
    
  // инициализация данных
  CheckAllData();
    
  OnChangeInitByDefault();
      
  // проверим длинные счетчики
  CheckLongCounters();
      
  // восстановим деньги
  LoadAcceptedMoney();
  
  // проверим пароль
  InitPass();
      
  //нициализация каналов
  InitChannels();
    
  // инициализация меню
  InitMenu();

  OSTimeDly(1000);
  
  // запустим валидатор
  StartUpValidator();
  
  OSTimeDly(10000);
  InitFiscal();

  // проинициализируем часы
  InitRTC();

  // сделаем запись о включении
  SaveEventRecord(0, JOURNAL_EVENT_DEVICE_ON, GetTimeSec());

  //CPU_INT32U enable;
  //GetData(&EnableModemDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
  //SetData(&EnableCoinDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
  
  // инициализация модема
#ifdef MODEM_ENABLE
  if (InitModem() != 0)
  {
    SetErrorFlag(ERROR_MODEM_CONN);
  }
  else
#endif
  {
    ClrErrorFlag(ERROR_MODEM_CONN);
  }

  // запустим монетник
  InitCoin();

#endif
  
  // создадим очередь и задачу
  if (UserQuery == NULL)
    {    
      UserQuery = OSQCreate(&UserTbl[0], USER_QUERY_LEN);
      OSTaskCreate(UserAppTask, (void *)0, (OS_STK *)&UserTaskStk[USER_TASK_STK_SIZE-1], USER_TASK_PRIO);
    }

  InitConsole();
  
#ifdef BOARD_CENTRAL_CFG
  InitHostApp();
#endif
      
  SystemTime = GetTimeSec();
  
#ifdef BOARD_CENTRAL_CFG
  // перейдем в стартовое меню, если работа
  if (GetMode() == MODE_WORK) {SetMenu(WORK_MENU);}
  else SetMenu(SERVICE_MENU);
#endif
  
}

int GetUserEvent(int* event)
{
  CPU_INT08U err = 0; 
  int evt  = (int)OSQPend(UserQuery, 1, &err);
  if (err != 0) return 0;
  *event = evt;
  return 1;  
}   


void PostUserEvent(int event)
{
  OSQPost(UserQuery, (void *)event);
}


void InitUserMenu(void)
{
  for (int i = 0; i < CHANNELS_NUM; i++)
  {
    CPU_INT32U en = 0;
    GetData(&EnableChannelDesc, &en, i, DATA_FLAG_DIRECT_INDEX);
    if (en)
    {
        ChannelsState[i] = CHANNEL_STATE_FREE;
    }
    else
    {
        ChannelsState[i] = CHANNEL_STATE_DISABLED;
    }
  }
}

void UserPrintMoneyMenu(void)
{
    char buf[32];
    CPU_INT32U accmoney;

    strcpy(buf, " ");
    PrintUserMenuStr(buf, 0);
    sprintf(buf, " Внесите деньги");
    PrintUserMenuStr(buf, 1);
    accmoney = GetAcceptedMoney();
    sprintf(buf, "Принято %d руб.", accmoney);
    PrintUserMenuStr(buf, 2);
    sprintf(buf, " ");
    PrintUserMenuStr(buf, 3);
}
                         
// вывод меню о невозможости работы
void UserPrintErrorMenu(void)
{
  char buf[32];
  
  if (TstErrorFlag(ERROR_VALIDATOR_CONN) || TstCriticalValidatorErrors())
    {
      sprintf(buf, "ОШИБКА");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "КУПЮРОПРИЕМНИКА");
      PrintUserMenuStr(buf, 1);
      if (TstErrorFlag(ERROR_VALIDATOR_CONN)) 
        {
          sprintf(buf, "НЕТ СВЯЗИ");
          PrintUserMenuStr(buf, 2);
          sprintf(buf, "");
          PrintUserMenuStr(buf, 3);
        }
    }
  else if (TstErrorFlag(ERROR_FR_CONN))
    {
      sprintf(buf, "ОШИБКА");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "НЕТ СВЯЗИ С ФР");
      PrintUserMenuStr(buf, 1);
      sprintf(buf, "");
      PrintUserMenuStr(buf, 2);
      PrintUserMenuStr(buf, 3);
    }
  else if (TstCriticalFiscalError())
    {
      sprintf(buf, "ОШИБКА");
      PrintUserMenuStr(buf, 0);
      CPU_INT08U errcode = 0;
      sprintf(buf, "ОШИБКА ФР");
      PrintUserMenuStr(buf, 1);
      GetFirstCriticalFiscalError(&errcode);
      GetDataItem(&JournalErrorNumberDesc0, (CPU_INT08U*)buf, errcode);
      PrintUserMenuStr(buf, 2);
      GetDataItem(&JournalErrorNumberDesc1, (CPU_INT08U*)buf, errcode);
      PrintUserMenuStr(buf, 3);
    }
  /*  
  else if (!FReportTest())
    {
      sprintf(buf, "ПЕЧАТb ОТЧЕТА");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "ЖДИТЕ");
      PrintUserMenuStr(buf, 1);
      sprintf(buf, "ПРИНТЕР");
      PrintUserMenuStr(buf, 2);
      sprintf(buf, "НЕДОСТУПЕН");
      PrintUserMenuStr(buf, 3);
    }
  */
}

int CheckChannelEnabled(CPU_INT08U channel)
{
    CPU_INT32U en = 0;
    GetData(&EnableChannelDesc, &en, channel, DATA_FLAG_DIRECT_INDEX);
    if (en)
    {
        return 1;
    }
    return 0;
}


void WorkServer(void)
{

}

void UserPrintPrintBillMenu(void)
{
  char buf[32];
  sprintf(buf, " ");
  PrintUserMenuStr(buf, 0);
  sprintf(buf, "Идeт печать");
  PrintUserMenuStr(buf, 1);
  sprintf(buf, "   чека");
  PrintUserMenuStr(buf, 2);
  sprintf(buf, " ");
  PrintUserMenuStr(buf, 3);
}

void UserPrintThanksMenu(void)
{
  char buf[32];
  sprintf(buf, " ");
  PrintUserMenuStr(buf, 0);
  sprintf(buf, "   СПАСИБО");
  PrintUserMenuStr(buf, 1);
  sprintf(buf, " ");
  PrintUserMenuStr(buf, 2);
  sprintf(buf, " ");
  PrintUserMenuStr(buf, 3);
}


int ChannelBusy(CPU_INT08U ch)
{
    return 0;
}
            
void UserPrintFirstMenu(void)
{
    char buf[32];
    sprintf(buf, " ");
    PrintUserMenuStr(buf, 0);
    sprintf(buf, "    ВНЕСИТЕ");
    PrintUserMenuStr(buf, 1);
    sprintf(buf, "    ДЕНЬГИ");
    PrintUserMenuStr(buf, 2);
    sprintf(buf, " ");
    PrintUserMenuStr(buf, 3);
}


// проверка, были ли сохранены деньги до выключения питания
void LoadAcceptedMoney(void)
{
  CPU_INT32U m,crc,crct;

  // считаем cохраненные деньги из FRAM
  GetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);    
  // считаем crc16 этих денег из FRAM 
  GetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
  
  crct = crc16((unsigned char*)&m, sizeof(CPU_INT32U));

  if (crct != crc)
    { // обнуляем, если crc не сошлась
      m = 0;
      crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
      SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
      SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
    }
    
}

// добавить денег
void SetAcceptedMoney(CPU_INT32U money)
{
  CPU_INT32U m,crc;
  m=money;
  crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// очистить счетчик денег
void ClearAcceptedMoney(void)
{
  CPU_INT32U m,crc;
  m=0;
  crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// очистить счетчик денег
CPU_INT32U GetAcceptedMoney(void)
{
  CPU_INT32U m;
  GetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  return m;
}

// инициализация пароля
void InitPass(void)
{
  CPU_INT32U pass,crc,crct;
  GetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);    
  GetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
  
  crct = crc16((unsigned char*)&pass, sizeof(CPU_INT32U));

  if (crct != crc)
    { // обнуляем, если crc не сошлась
      pass = DEFAULT_PASSWORD;
      crc = crc16((unsigned char*)&pass, sizeof(CPU_INT32U));
      SetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);
      SetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
    }
}
