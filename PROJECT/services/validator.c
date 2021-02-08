#include <includes.h>
#include "validator.h"
#include "CCRSProtocol.h"
#include "uart1.h"
#include "app_serv.h"
#include "journal.h"
#include "data.h"
#include "datadesc.h"


OS_STK    ValidatorTaskStk[VALIDATOR_TASK_STK_SIZE];
OS_EVENT *VLock = NULL;

void CC_BusReset(void);
int ValidatorConnect(void);

// статус подключения купюрника
unsigned char VConnStat = VCONN_STATUS_NOCONN;

TBillRecord VBillTable[24];
/// таблица с целочисленными значениями номиналов - для GUI 
CPU_INT32U BillNominals[24];

TBillStatus VBillStatus;
/// счетчик денег
CPU_INT32U  VBillCount = 0; 
/// индекс номинала последней приянтой купюры
CPU_INT32U  VLastBillIndex = V_BILLS_NUMBER; 

void ValidatorTask(void *p_arg)
{
  char incas = 0;
  // 1. если нет связи, пробуем коннектиться командой RESET
  // 2. BUS RESET - min 100 ms
  // 3. POLL надо делать каждые 100-200 мс, рекомендуется больше 200
  
  VBillCount = 0;
  VConnStat = VCONN_STATUS_NOCONN;
  ClrErrorFlag(ERROR_VALIDATOR_CONN);
  ClrErrorFlag(ERROR_VALIDATOR_FAILURE);
  
  while(1)
    {
      TPollResults polldata; 
      OSTimeDly(CC_POLL_TIME_OUT);

      CPU_INT32U enable, cash_mode;      
      GetData(&EnableValidatorDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);
      GetData(&CashModeDesc, &cash_mode, 0, DATA_FLAG_SYSTEM_INDEX);  

      // обработка статуса связи
      if (VConnStat == VCONN_STATUS_NOCONN)
        { // нет подключения
          if ((!enable) || (cash_mode != 0))
            { // не включен
              VConnStat = VCONN_STATUS_NOCONN; 
              ClrErrorFlag(ERROR_VALIDATOR_CONN); 
              ClrErrorFlag(ERROR_VALIDATOR_FAILURE);
              continue;
            }
          // надо проинициализироваться
          if (ValidatorConnect() != 0) {SetErrorFlag(ERROR_VALIDATOR_CONN); continue;}
          // успешно
          ClrErrorFlag(ERROR_VALIDATOR_CONN);
          ClrErrorFlag(ERROR_VALIDATOR_FAILURE);
          VConnStat = VCONN_STATUS_CONN;
        }
      else
        { // подключение есть
          if (!enable) 
            { // не включен
              VConnStat = VCONN_STATUS_NOCONN; 
              ClrErrorFlag(ERROR_VALIDATOR_CONN); 
              ClrErrorFlag(ERROR_VALIDATOR_FAILURE);
              continue;
            }
        }
      
      // запрос статуса увалидатора
      if (CC_CmdPoll(ADDR_FL, &polldata))
        {// разбираем ответ на poll

          if ((incas) && (polldata.Z1 != ST_BOX))
          {
              incas = 0;
              PostUserEvent(EVENT_INCASSATION_FINISH);
          }
                
          switch (polldata.Z1){
            // States CCNET states and events
            case ST_POWER_UP:
            case ST_POWER_BILL_ESCROW:
            case ST_POWER_BILL_STACKER:
            case ST_INITIALIZE:
            case ST_IDLING:
            case ST_ACCEPTING:
            case ST_PACKING:
            case ST_RETURNING:
            case ST_DISABLED:
            case ST_HOLDING:
            case ST_BUSY:
                ClrValidatorErrors();
                break;
            case ST_REJECTING:
                ClrErrorFlag(ERROR_VALIDATOR_FAILURE);
                switch (polldata.Z2){
                  case RJ_INSERTION:   //!< Rejection because of insertion problem
                  case RJ_REMAINING:   //!< Rejection because of other bill remaining in the device
                    SetErrorFlag(ERROR_VALIDATOR_INSERTION);
                    break;
                  case RJ_MAGNETIC:    //!< Rejection because of invalid magnetic pattern
                    SetErrorFlag(ERROR_VALIDATOR_MAGNETIC);
                    break;
                  case RJ_CONVEYING:   //!< Rejection because of conveying 
                    SetErrorFlag(ERROR_VALIDATOR_CONVEYING);
                    break;
                  case RJ_IDENT:       //!< Rejection because of identification failure
                    SetErrorFlag(ERROR_VALIDATOR_IDENT);
                    break;
                  case RJ_VRFY:        //!< Rejection because of verification failure
                    SetErrorFlag(ERROR_VALIDATOR_VRFY);
                    break;
                  case RJ_OPT:         //!< Rejection because of optical pattern mismatch
                    SetErrorFlag(ERROR_VALIDATOR_OPT);
                    break;
                  case RJ_INHIBIT:     //!< Rejection because the denomination is inhibited
                    SetErrorFlag(ERROR_VALIDATOR_INHIBIT);
                    break;
                  case RJ_CAP:         //!< Rejection because of capacity sensor pattern mismatch
                    SetErrorFlag(ERROR_VALIDATOR_CAP);
                    break;
                  case RJ_LNG:         //!< Rejection because of invalid bill length
                    SetErrorFlag(ERROR_VALIDATOR_LNG);
                    break;
                  default:
                    break;
                }
                break;
            case ST_ST_FULL:    //!< DROP CASSETTE IS FULL state
              SetErrorFlag(ERROR_STACKER_FULL);
              break;
            case ST_BOX:        //!< DROP CASSETTE REMOVED state 
              //SetErrorFlag(ERROR_STACKER_REMOVED);
              if (incas == 0)
              {
                PostUserEvent(EVENT_INCASSATION);
                ClrValidatorErrors();
                incas = 1;
              }
              break;
            case ST_BV_JAMMED:  //!< JAM IN VALIDATOR state
              SetErrorFlag(ERROR_BV_JAMMED);
              break;
            case ST_ST_JAMMED:  //!< JAM IN STACKER state
              SetErrorFlag(ERROR_ST_JAMMED);
              break;
            case ST_CHEATED:    //!< CHEATED event
              SetErrorFlag(ERROR_CHEATED);
              break;
            case ST_FAILURE:
              switch (polldata.Z2){
                case FLR_STACKER:     //!< Stacking mechanism failure
                  SetErrorFlag(ERROR_FLR_STACKER);
                  break;              
                case FLR_TR_SPEED:    //!< Invalid speed of transport mechanism
                  SetErrorFlag(ERROR_TR_SPEED);
                  break;              
                case FLR_TRANSPORT:   //!< Transport mechanism failure
                  SetErrorFlag(ERROR_FLR_TRANSPORT);
                  break;              
                case FLR_ALIGNING:    //!< Aligning mechanism failure
                  SetErrorFlag(ERROR_FLR_ALIGNIN);
                  break;              
                case FLR_INIT_CAS:    //!< Initial cassette status failure
                  SetErrorFlag(ERROR_FLR_INIT_CAS);
                  break;              
                case FLR_OPT:         //!< Optical channel failure
                  SetErrorFlag(ERROR_FLR_OPT);
                  break;              
                case FLR_MAG:         //!< Inductive channel failure
                  SetErrorFlag(ERROR_FLR_MAG);
                  break;              
                case FLR_CAP:         //!< Capacity sensor failure                  
                  SetErrorFlag(ERROR_FLR_CAP);
                  break;              
              }
              // надо переподключиться к модему, а то так и будет висеть
              SetErrorFlag(ERROR_VALIDATOR_CONN);
              VConnStat = VCONN_STATUS_NOCONN;
              break;
            // Failure codes 
            case FLR_STACKER:
            case FLR_TR_SPEED:
            case FLR_TRANSPORT:
            case FLR_ALIGNING:
            case FLR_INIT_CAS:
            case FLR_OPT:
            case FLR_MAG:
            case FLR_CAP:
                SetErrorFlag(ERROR_VALIDATOR_FAILURE);
                break;
            // Credit events
            case ST_PACKED:
                // банкнота упакована
                // запретим прием
                if (!CC_CmdBillType(0x000000, 0x000000, ADDR_FL)){SetErrorFlag(ERROR_VALIDATOR_CONN);}
                // запостим событие пользовательскому серверу
                if (polldata.Z2 < V_BILLS_NUMBER)
                {
                    VBillCount += VBillTable[polldata.Z2].Denomination;
                    // сохраняем индекс купюры
                    VLastBillIndex = polldata.Z2;
                }
                else
                {
                    VBillCount = 0;
                    // неправильный индекс
                    VLastBillIndex = V_BILLS_NUMBER;
                }
                PostUserEvent(EVENT_BILL_STACKED);
                ClrValidatorErrors();
                break;
            case ESCROW:
                if (polldata.Z2 < V_BILLS_NUMBER)
                {
                    //VBillCount += VBillTable[polldata.Z2].Denomination;
                    // сохраняем индекс купюры
                    //VLastBillIndex = polldata.Z2;
                }
                else
                {
                    VBillCount = 0;
                    // неправильный индекс
                    VLastBillIndex = V_BILLS_NUMBER;
                }
                PostUserEvent(EVENT_BILL_ESCROW);
                ClrValidatorErrors();
                break;
            case RETURNED:
                ClrValidatorErrors();
                break;
            default:
                ClrValidatorErrors();
                break;
          }// switch (polldata.Z1)
          ClrErrorFlag(ERROR_VALIDATOR_CONN);
          VConnStat = VCONN_STATUS_CONN;
        }
      else
        { // ошибка
          SetErrorFlag(ERROR_VALIDATOR_CONN);
          VConnStat = VCONN_STATUS_NOCONN;
        }
      
    }

}


int ValidatorConnect(void)
{
  int i;
  OSTimeDly(200);

  for (i = 0; i < 24; i++)
  {
    BillNominals[i] = 0;
  }
  
  CC_BusReset();

  // сбросим валидатор  
  if (!CC_CmdReset(ADDR_FL)) return -1;

  // прочитаем таблицу номиналов валют
  if (!CC_CmdGetBillTable(ADDR_FL, VBillTable)) return -2;
  
  for (i = 0; i < 24; i++)
  {
    BillNominals[i] = (CPU_INT32U)VBillTable[i].Denomination;
  }
  
  // разрешим прием
  if (!CC_CmdBillType(0xffffff, 0xffffffff, ADDR_FL)) return -3;
  
  VConnStat = VCONN_STATUS_CONN;
  
  return 0;
}


int IsValidatorConnected(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int retval;
  if (VConnStat == VCONN_STATUS_CONN) retval=1;
  else retval=0;
  OS_EXIT_CRITICAL();
  return retval;
}


// запуск залачи валидатора
void StartUpValidator(void)
{
  Uart1_Init(CC_VALIDATOR_SPEED);

  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif

  OS_ENTER_CRITICAL();
    
  // BUS RESET init
  PINSEL4_bit.P2_4 = 0;
  PINMODE4_bit.P2_4 = 0;
  FIO2DIR_bit.P2_4 = 1;
  FIO2MASK_bit.P2_4 = 0;
  FIO2CLR_bit.P2_4 = 1;
    
  OS_EXIT_CRITICAL();

  if (!VLock)
    {
      VLock = OSSemCreate(1);         
      OSTaskCreate(ValidatorTask, (void *)0, (OS_STK *)&ValidatorTaskStk[VALIDATOR_TASK_STK_SIZE-1], VALIDATOR_TASK_PRIO);
    }
}


void CC_BusReset(void)
{
  VPend();
  FIO2CLR_bit.P2_4 = 1;
  OSTimeDly(CC_BUS_RESET_TIME_OUT);
  FIO2SET_bit.P2_4 = 1;
  VPost();
}

// занять валидатор
void VPend(void)
{
 CPU_INT08U err;  
 do{
   OSSemPend(VLock, 1, &err);
   if (!err) break;
   OSTimeDly(1);
 }while (err);
}

// освободить валидатор
void VPost(void)
{
  OSSemPost(VLock);
}

// прочитать счетчик денег от купюрника и сбросить
CPU_INT32U GetResetBillCount(CPU_INT32U *bill_index)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();  
  CPU_INT32U count = VBillCount;
  VBillCount = 0;
  *bill_index = VLastBillIndex;
  OS_EXIT_CRITICAL();
  return count;
}

