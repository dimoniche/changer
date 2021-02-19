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

// ���� ���������� ���� ������, ����� ��������� ������ �� ������ F1
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

void SetAcceptedBankMoney(CPU_INT32U money);
void ClearAcceptedBankMoney(void);
CPU_INT32U GetAcceptedBankMoney(void);

void SetAcceptedRestMoney(CPU_INT32U money);
void ClearAcceptedRestMoney(void);
CPU_INT32U GetAcceptedRestMoney(void);

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
      // ��������� �������� �������
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
 ������ ��������� ������� ������������
*/
void UserAppTask(void *p_arg)
{
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
  
  int testMoney = 0;
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
              // �������� ������
              CheckMode();
             
              // ��������� ������� �����
              SystemTime = GetTimeSec();
                
              // ������� ������ - ��������, ��������� � �.�.
              WorkServer();

              // �������� ����������, ���� �� ���������
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

              // ������ ������
              ErrorServer();
                
              // ������ ������ � ������� ������
              if (GetMode() != MODE_WORK)
              {
                  break;
              }
                
              // ���� ���� ������, �� ��������
              if (TstCriticalErrors()) 
              {
                  UserPrintErrorMenu(); 
                  RefreshMenu();
  
                  // �������� ����� �����
                  if (was_critical_error == 0)
                  {
                      if (IsValidatorConnected()) CC_CmdBillType(0x000000, 0x000000, ADDR_FL);
                      CoinDisable();
                      BankDisable();
                      was_critical_error = 1;
                  }
                  break;
              }
              
              // ������� ������ ����� �����, ���� ���� ������
              if (was_critical_error)
              {
                  if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffff, ADDR_FL);
                  CoinEnable();
                  BankEnable();
                  
                  was_critical_error = 0;
                  break;
              }

              accmoney = GetAcceptedMoney();
              accmoney += GetAcceptedBankMoney();
              accmoney += GetAcceptedRestMoney();
              
              {
                  // ��������� ������ � �������
                  CPU_INT32U HopperCost = 0;
                  GetData(&HopperCostDesc, &HopperCost, 0, DATA_FLAG_SYSTEM_INDEX);
    
                  if(accmoney >= HopperCost)  // ������� ����� �� ����� - ����� ������ ������
                  {
                      LED_OK_ON();
                  }
                  else
                  {
                      LED_OK_OFF();
                  }
                  
                  // ��������� ������� ��� ����� ������� ������
                  CPU_INT32U HopperSaveCredit = 0;
                  GetData(&HopperSaveCreditDesc, &HopperSaveCredit, 0, DATA_FLAG_SYSTEM_INDEX);
                  
                  if ((HopperSaveCredit > 0) && (labs(OSTimeGet() - money_timestamp) > 60000UL * HopperSaveCredit))
                  {
                      // ���� ��������� ��������� � ������ ����� - ������� �������� ������ �����
                      SetAcceptedRestMoney(0);
                      SetAcceptedBankMoney(0);
                      SetAcceptedMoney(0);
                  }
              }
              
              // ��������� ������
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
                  sprintf((char*)str_IncasMenu_3, "  ����� %u ���.", incas_sum);
                  // ������ ���� ����������
                  GoToMenu(IncasMenuPanel);
                  // �������� ������� � ��������� ����� �����
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
                  // ������� �������� �����
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
                
                money = cpp * GetResetCoinCount() + testMoney;
                
                accmoney = GetAcceptedMoney();
                accmoney += money;
                SetAcceptedMoney(accmoney);
                IncCounterCoin(money);
                
                money_timestamp = OSTimeGet();
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                    UserPrintMoneyMenu();  
                    RefreshMenu();
                }
                if (money) SaveEventRecord(RecentChannel, JOURNAL_EVENT_MONEY_COIN, money);
                
                // ������ ������ �� ������?
                CPU_INT32U hopperStartButton = 0;
                GetData(&HopperButtonStartDesc, &hopperStartButton, 0, DATA_FLAG_SYSTEM_INDEX);
                
                if(!hopperStartButton)
                {
                    // ���� �� �� ������ - �� ������ �������� ��������� ������������� ������ �������
                    PostUserEvent(EVENT_GIVE_COIN);
                }
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
                IncCounterCash(money);
                
                money_timestamp = OSTimeGet();
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                    UserPrintMoneyMenu();  
                    RefreshMenu();
                }
                if (money) SaveEventRecord(RecentChannel, JOURNAL_EVENT_MONEY_NOTE, money);
                CPU_INT32U billnom_index = FindBillIndex(money);
                if (billnom_index != 0xFFFFFFFF) IncBillnomCounter(billnom_index);
                
                // ������ ������ �� ������?
                CPU_INT32U hopperStartButton = 0;
                GetData(&HopperButtonStartDesc, &hopperStartButton, 0, DATA_FLAG_SYSTEM_INDEX);
                
                if(!hopperStartButton)
                {
                    // ���� �� �� ������ - �� ������ �������� ��������� ������������� ������ �������
                    PostUserEvent(EVENT_GIVE_COIN);
                }
              }
              break;
            case EVENT_BANK_INSERTED:
             {
                CPU_INT32U cpp = 1;
                CPU_INT32U money, accmoney;
                GetData(&BankPerPulseDesc, &cpp, 0, DATA_FLAG_SYSTEM_INDEX);
                
                money = cpp * GetResetbankCount() + testMoney;
                
                accmoney = GetAcceptedBankMoney();
                accmoney += money;
                SetAcceptedBankMoney(accmoney);
                IncCounterBank(money);
                
                money_timestamp = OSTimeGet();
                
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                    UserPrintMoneyMenu();  
                    RefreshMenu();
                }
                
                // ������ ������ �� ������?
                CPU_INT32U hopperStartButton = 0;
                GetData(&HopperButtonStartDesc, &hopperStartButton, 0, DATA_FLAG_SYSTEM_INDEX);
                
                if(!hopperStartButton)
                {
                    // ���� �� �� ������ - �� ������ �������� ��������� ������������� ������ �������
                    PostUserEvent(EVENT_GIVE_COIN);
                }
                
                if (money) SaveEventRecord(RecentChannel, JOURNAL_EVENT_MONEY_BANK, money);
              }              
            break;
            case EVENT_BILL_ESCROW:
                // ������ � ��������� ��������
                if (IsValidatorConnected()) if (!CC_CmdPack(ADDR_FL)) SetErrorFlag(ERROR_VALIDATOR_CONN);
                break;
            case EVENT_BILL_STACKED:
              // ������ �������
              {
                CPU_INT32U billnom_index;
                CPU_INT32U note,accmoney;
                note = GetResetBillCount(&billnom_index);
                accmoney = GetAcceptedMoney();
                accmoney += note;
                SetAcceptedMoney(accmoney);
                
                IncCounterCash(note);
                
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
                
                // ������ ������ �� ������?
                CPU_INT32U hopperStartButton = 0;
                GetData(&HopperButtonStartDesc, &hopperStartButton, 0, DATA_FLAG_SYSTEM_INDEX);
                
                if(!hopperStartButton)
                {
                    // ���� �� �� ������ - �� ������ �������� ��������� ������������� ������ �������
                    PostUserEvent(EVENT_GIVE_COIN);
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
            //case EVENT_KEY_USER_START:
              if (incassation) break;
              if (GetMode() != MODE_WORK)
                {
                  if (!FlagForPrintReport) break;
                  if (GetCurrentMenu() == xReportMenuPanel)
                    { // �������� X-�����
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
                   { // �������� Z-�����
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
                   { // �������� Z-������ �� ������
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
                  else if (GetCurrentMenu() == CanselCheckMenuPanel)
                    {
                      int res = CanselFiscalBill();
                      SaveEventRecord(0, JOURNAL_EVENT_PRINT_X, res);
                      CheckFiscalStatus();
                      GoToPreviousMenu();
                    }
                  break;
                }
                
                // � ������� ������ - �������� ����
                //PostUserEvent(EVENT_PRINT_CHECK);
              break;
              
            // ������ ������� ������
            case EVENT_KEY_USER_START:
              if (GetMode() != MODE_WORK) break;
              
              // ������ ������ - ������� ������
              PostUserEvent(EVENT_GIVE_COIN);
            break;
            
            // ������ ������ � ��������
            case EVENT_GIVE_COIN:
              if (GetMode() != MODE_WORK) break;
              
              if (TstCriticalErrors()) 
              {
                UserPrintErrorMenu(); 
                RefreshMenu(); 
                break;
              }
              
              // --------------------------
              // ��������� � ������� ������
              // --------------------------
              
              // ����� ��������� ��������--
              {
                CPU_INT32U hopper_mode = 0;
                GetData(&RegimeHopperDesc, &hopper_mode, 0, DATA_FLAG_SYSTEM_INDEX);
                
                // ��������� ������ � �������
                CPU_INT32U HopperCost = 0;
                GetData(&HopperCostDesc, &HopperCost, 0, DATA_FLAG_SYSTEM_INDEX);
                
                CPU_INT32U accmoney = GetAcceptedMoney();
                accmoney += GetAcceptedBankMoney();
                accmoney += GetAcceptedRestMoney();
                
                if(accmoney >= HopperCost)
                {
                    CPU_INT32U CountCoin = 0;
                    CountCoin = accmoney / HopperCost;
                    
                    // ���� ������� �� ����� - ��� ����������� �� ���� ������ �������
                    if(!hopper_mode)
                    {
                        // ����� Elolution - ��������� ������� ������� ����������
                        for(int j = 0; j < CountCoin; j++)
                        {
                           FIO0CLR_bit.P0_24 = 1;
                           OSTimeDly(50);
                           FIO0SET_bit.P0_24 = 1;
                           OSTimeDly(50);
                        }
                    }
                    else
                    {
                        // �����  Cube
                        
                    }
                    
                    // ����� ������ � �������� - �������� ���� - ������ ���� ������ �����
                    PostUserEvent(EVENT_PRINT_CHECK);
                    
                    // ������ ������� �� ������ ������
                    CPU_INT32U restMoney = accmoney % HopperCost;
                    
                    SetAcceptedRestMoney(restMoney);
                }
              }
              
              break;
            case EVENT_ERROR_HOPPER_ON:
              {
                  if (GetMode() != MODE_WORK) break;
                  
                  // ������ ������ �������
                  SetErrorFlag(ERROR_HOPPER);
                  SaveEventRecord(RecentChannel, ERROR_HOPPER, 0);
              }
              break;
            case EVENT_ERROR_HOPPER_OFF:
              {
                  if (GetMode() != MODE_WORK) break;
                  
                  // ������ ������ ������ �������
                  ClrErrorFlag(ERROR_HOPPER);
              }
              break;
           case EVENT_NOMONEY_HOPPER_ON:
              {
                  if (GetMode() != MODE_WORK) break;
                  
                  // ������ ���������� ����� � �������
                  SetErrorFlag(ERROR_NO_MONEY_HOPPER);
                  SaveEventRecord(RecentChannel, ERROR_NO_MONEY_HOPPER, 0);
              }
              break;
           case EVENT_NOMONEY_HOPPER_OFF:
              {
                  if (GetMode() != MODE_WORK) break;
                  
                  // ������ ������ ���������� ����� � �������
                  ClrErrorFlag(ERROR_NO_MONEY_HOPPER);
              }
              break;              
            case EVENT_PRINT_CHECK:
              
              if (GetMode() != MODE_WORK) break;
              
              if (TstCriticalErrors()) 
              {
                UserPrintErrorMenu(); 
                RefreshMenu(); 
                break;
              }
              
              // --------------------------
              // ��������� � ������� ������
              // --------------------------
              {
                  // �������� ����
                  // ������������ ���� ������ � ����� ����� + ����� ������ �� ������
                  CPU_INT32U accmoney = GetAcceptedMoney();
                  
                  if (accmoney > 0)
                  { 
                      UserPrintPrintBillMenu();
                      RefreshMenu();
                      
                      // ���������� ���
                      if (IsFiscalConnected())
                      {
                        if (PrintFiscalBill(accmoney, 0) == 0)
                        {
                            SaveEventRecord(RecentChannel, JOURNAL_EVENT_PRINT_BILL, GetTimeSec());
                        }
                      }
                      
                      IncCounter(ChannelsPayedTime[RecentChannel], accmoney);
                      SetAcceptedMoney(0);
                      OSTimeDly(1000);
                     
                      // ������� ���� "�������"                      
                      if (IsFiscalConnected())
                      {
                          UserPrintThanksMenu();
                          RefreshMenu();
                      }
                      
                      if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffff, ADDR_FL);
                      
                      OSTimeDly(1000);
                      LED_OK_OFF();
                  }
                    
                  accmoney = GetAcceptedBankMoney();
                  
                  if (accmoney > 0)
                  { 
                      UserPrintPrintBillMenu();
                      RefreshMenu();
                      
                      // ���������� ���
                      if (IsFiscalConnected())
                      {
                        if (PrintFiscalBill(accmoney, 1) == 0)
                        {
                            SaveEventRecord(RecentChannel, JOURNAL_EVENT_PRINT_BILL_ONLINE, GetTimeSec());
                        }
                      }
                      
                      IncCounter(ChannelsPayedTime[RecentChannel], accmoney);
                      SetAcceptedBankMoney(0);
                      OSTimeDly(1000);
                     
                      // ������� ���� "�������"                      
                      if (IsFiscalConnected())
                      {
                          UserPrintThanksMenu();
                          RefreshMenu();
                      }
                      
                      OSTimeDly(1000);
                      LED_OK_OFF();
                  }
              }
              break;
#else

#endif
           case EVENT_KEY_F1:
              //testMoney = 10;
              //PostUserEvent(EVENT_COIN_INSERTED);
              break;
           case EVENT_KEY_F2:
              //testMoney = 50;
              //PostUserEvent(EVENT_ERROR_HOPPER_ON);
              break;
           case EVENT_KEY_F3:
              //PostUserEvent(EVENT_ERROR_HOPPER_OFF);
              break;
              
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
 ���������������� �������������
*/
void UserStartupFunc(void)
{
#ifdef BOARD_CENTRAL_CFG
  // ������������� ������ ������
  InitMode();
  
  // ������������� ������
  CheckAllData();
    
  OnChangeInitByDefault();
      
  // �������� ������� ��������
  CheckLongCounters();
      
  // ����������� ������
  LoadAcceptedMoney();
  
  // �������� ������
  InitPass();
      
  //������������ �������
  InitChannels();
    
  // ������������� ����
  InitMenu();

  OSTimeDly(1000);
  
  // �������� ���������
  StartUpValidator();
  
  OSTimeDly(1000);
  InitFiscal();

  // ����������������� ����
  InitRTC();

  // ������� ������ � ���������
  SaveEventRecord(0, JOURNAL_EVENT_DEVICE_ON, GetTimeSec());

  //CPU_INT32U enable;
  //GetData(&EnableModemDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
  //SetData(&EnableCoinDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
  
  // ������������� ������
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

  // �������� ��������
  InitCoin();

#endif
  
  // �������� ������� � ������
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
  // �������� � ��������� ����, ���� ������
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
    sprintf(buf, " ������� ������");
    PrintUserMenuStr(buf, 1);
    
    accmoney = GetAcceptedMoney();
    accmoney += GetAcceptedBankMoney();
    accmoney += GetAcceptedRestMoney();
    
    sprintf(buf, "������� %d ���.", accmoney);
    PrintUserMenuStr(buf, 2);
    sprintf(buf, " ");
    PrintUserMenuStr(buf, 3);
}
                         
// ����� ���� � ������������ ������
void UserPrintErrorMenu(void)
{
  char buf[32];
  
  if (TstErrorFlag(ERROR_VALIDATOR_CONN) || TstCriticalValidatorErrors())
    {
      sprintf(buf, "������");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "���������������");
      PrintUserMenuStr(buf, 1);
      if (TstErrorFlag(ERROR_VALIDATOR_CONN)) 
        {
          sprintf(buf, "��� �����");
          PrintUserMenuStr(buf, 2);
          sprintf(buf, "");
          PrintUserMenuStr(buf, 3);
        }
    }
  else if (TstErrorFlag(ERROR_FR_CONN))
    {
      sprintf(buf, "������");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "��� ����� � ��");
      PrintUserMenuStr(buf, 1);
      sprintf(buf, "");
      PrintUserMenuStr(buf, 2);
      PrintUserMenuStr(buf, 3);
    }
  else if (TstCriticalFiscalError())
    {
      sprintf(buf, "������");
      PrintUserMenuStr(buf, 0);
      CPU_INT08U errcode = 0;
      sprintf(buf, "������ ��");
      PrintUserMenuStr(buf, 1);
      GetFirstCriticalFiscalError(&errcode);
      GetDataItem(&JournalErrorNumberDesc0, (CPU_INT08U*)buf, errcode);
      PrintUserMenuStr(buf, 2);
      GetDataItem(&JournalErrorNumberDesc1, (CPU_INT08U*)buf, errcode);
      PrintUserMenuStr(buf, 3);
    }
  else if(TstErrorFlag(ERROR_HOPPER) || TstErrorFlag(ERROR_NO_MONEY_HOPPER))
  {
      sprintf(buf, "������");
      PrintUserMenuStr(buf, 0);
      CPU_INT08U errcode = 0;
      sprintf(buf, "������ �������");
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
      sprintf(buf, "�����b ������");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "�����");
      PrintUserMenuStr(buf, 1);
      sprintf(buf, "�������");
      PrintUserMenuStr(buf, 2);
      sprintf(buf, "����������");
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
  sprintf(buf, "��e� ������");
  PrintUserMenuStr(buf, 1);
  sprintf(buf, "   ����");
  PrintUserMenuStr(buf, 2);
  sprintf(buf, " ");
  PrintUserMenuStr(buf, 3);
}

void UserPrintThanksMenu(void)
{
  char buf[32];
  sprintf(buf, " ");
  PrintUserMenuStr(buf, 0);
  sprintf(buf, "   �������");
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
    sprintf(buf, "    �������");
    PrintUserMenuStr(buf, 1);
    sprintf(buf, "    ������");
    PrintUserMenuStr(buf, 2);
    sprintf(buf, " ");
    PrintUserMenuStr(buf, 3);
}


// ��������, ���� �� ��������� ������ �� ���������� �������
void LoadAcceptedMoney(void)
{
  CPU_INT32U m,crc,crct;

  // ������� c���������� ������ �� FRAM
  GetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);    
  // ������� crc16 ���� ����� �� FRAM 
  GetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
  
  crct = crc16((unsigned char*)&m, sizeof(CPU_INT32U));

  if (crct != crc)
    { // ��������, ���� crc �� �������
      m = 0;
      crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
      SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
      SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
    }
  
  // ������� c���������� ������ �� FRAM
  GetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);    
  // ������� crc16 ���� ����� �� FRAM 
  GetData(&AcceptedBankMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
    
    crct = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  
    if (crct != crc)
      { // ��������, ���� crc �� �������
        m = 0;
        crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
        SetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
        SetData(&AcceptedBankMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
      }
    
  // ������� c���������� ������ �� FRAM
  GetData(&AcceptedRestMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);    
  // ������� crc16 ���� ����� �� FRAM 
  GetData(&AcceptedRestMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
    
    crct = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  
    if (crct != crc)
      { // ��������, ���� crc �� �������
        m = 0;
        crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
        SetData(&AcceptedRestMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
        SetData(&AcceptedRestMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
      }
}

// �������� �����
void SetAcceptedMoney(CPU_INT32U money)
{
  CPU_INT32U m,crc;
  m=money;
  crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// �������� ������� �����
void ClearAcceptedMoney(void)
{
  CPU_INT32U m,crc;
  m=0;
  crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// �������� ������� �����
CPU_INT32U GetAcceptedMoney(void)
{
  CPU_INT32U m;
  GetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  return m;
}

// �������� �����
void SetAcceptedBankMoney(CPU_INT32U money)
{
  CPU_INT32U m,crc;

  m=money;
  crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedBankMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// �������� ������� �����
void ClearAcceptedBankMoney(void)
{
  CPU_INT32U m,crc;

  m=0;
  crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedBankMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// �������� ������� �����
CPU_INT32U GetAcceptedBankMoney(void)
{
  CPU_INT32U m;

  GetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  return m;
}

// �������� �����
void SetAcceptedRestMoney(CPU_INT32U money)
{
  CPU_INT32U m,crc;

  m=money;
  crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedRestMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedRestMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// �������� ������� �����
void ClearAcceptedRestMoney(void)
{
  CPU_INT32U m,crc;

  m=0;
  crc = crc16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedRestMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedRestMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// �������� ������� �����
CPU_INT32U GetAcceptedRestMoney(void)
{
  CPU_INT32U m;

  GetData(&AcceptedRestMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  return m;
}

// ������������� ������
void InitPass(void)
{
  CPU_INT32U pass,crc,crct;
  GetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);    
  GetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
  
  crct = crc16((unsigned char*)&pass, sizeof(CPU_INT32U));

  if (crct != crc)
    { // ��������, ���� crc �� �������
      pass = DEFAULT_PASSWORD;
      crc = crc16((unsigned char*)&pass, sizeof(CPU_INT32U));
      SetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);
      SetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
    }
}
