#ifndef _DATADESC_H_
#define _DATADESC_H_

#include "data.h"
#include "control.h"

#define INCAS_SEND_FLAG     0x87654321

#define MAX_PRICE 9999

#define DEFAULT_PASSWORD 1111
#define MASTER_PASSWORD 11300045//1234567890L

// структура конфигурации каналов
typedef struct{
  // включение канала
  CPU_INT32U  Enable[CHANNELS_NUM];
  // тайм-аут перед включением, сек.
  CPU_INT32U  TimeOutBefore[CHANNELS_NUM];
  // тайм-аут после выключения, мин.
  CPU_INT32U  TimeOutAfter[CHANNELS_NUM];
  // максимальное время работы, мин.
  CPU_INT32U  MaxWorkTime[CHANNELS_NUM];
  // минимальное время работы, мин.
  CPU_INT32U  MinWorkTime[CHANNELS_NUM];
  // настройка уикенда, индекс
  CPU_INT32U  WeekEnd[CHANNELS_NUM];
    #define WEEKEND_NO                0
    #define WEEKEND_FRIDAY_SUNDAY     1
    #define WEEKEND_SATURDAY_SUNDAY   2
    #define WEEKEND_FRIDAY_SATURDAY   3
    #define WEEKEND_FRIDAY_MONDAY     4
  // название канала
  CPU_INT32U  Name[CHANNELS_NUM];
  
  // периоды
    #define PRICE_PERIODS_NUM   4
  CPU_INT32U  T_Start_Weekdays[CHANNELS_NUM][PRICE_PERIODS_NUM];
  CPU_INT32U  T_End_Weekdays[CHANNELS_NUM][PRICE_PERIODS_NUM];
  CPU_INT32U  T_Start_Weekend[CHANNELS_NUM][PRICE_PERIODS_NUM];
  CPU_INT32U  T_End_Weekend[CHANNELS_NUM][PRICE_PERIODS_NUM];
  // цены
  CPU_INT32U  Price_Weekdays[CHANNELS_NUM][PRICE_PERIODS_NUM];
  CPU_INT32U  Price_Weekend[CHANNELS_NUM][PRICE_PERIODS_NUM];
  CPU_INT32U  PriceTime_Weekdays[CHANNELS_NUM][PRICE_PERIODS_NUM];
  CPU_INT32U  PriceTime_Weekend[CHANNELS_NUM][PRICE_PERIODS_NUM];

  CPU_INT32U  post_ip[CHANNELS_NUM];
  CPU_INT32U  select_protect[CHANNELS_NUM];
  CPU_INT32U  imp_len[CHANNELS_NUM];
  CPU_INT32U  imp_cost[CHANNELS_NUM];
  CPU_INT32U  minute_cost[CHANNELS_NUM];

  CPU_INT32U  Price;
  
}TChannelConfig;



// структура конфигурации аппаратуры
typedef struct{
  CPU_INT32U  EnableValidator;
  CPU_INT32U  EnableCoinAcceptor;
  CPU_INT32U  EnableModem;
  CPU_INT32U  EnableFiscal;
  CPU_INT32U  EnableFiscalDayClear;
  CPU_INT32U  ServiceName;

  CPU_INT32U  CoinPerPulse; // цена импульса монетоприемника
  CPU_INT32U  BillFormat;
  
  CPU_INT32U  DisableFiscalErrors; // отключение реакции на ошибки ФР
  
  CPU_INT32U  EnableEmailErrorSend;
  CPU_INT32U  EnableEmailStatSend;
  CPU_INT32U  EnableEmailJournalSend;
  CPU_INT32U  ClearJournalAfterSend;
  CPU_INT32U  StatSendHourMin;

  CPU_INT32U  CashMode;
  CPU_INT32U  CashPerPulse; // цена импульса купюрника
  CPU_INT32U  PrintTimeout;
  CPU_INT32U  PrintTimeoutAfter;

  CPU_INT32U  PrintMode;

  CPU_INT32U  DeviceId;

}TDeviceConfig;


extern CPU_INT32U PeriodIndex;
extern CPU_INT32U  ChannelIndex;
extern TDataDescStruct const DeviceIDDesc;

extern TDataDescStruct const LastEmailSendTime;

extern TDataDescStruct const ChannelIndexDesc;
extern TDataDescStruct const EnableChannelDesc;
extern TDataDescStruct const TimeOutBeforeDesc;
extern TDataDescStruct const TimeOutAfterDesc;
extern TDataDescStruct const MaxWorkTimeDesc;
extern TDataDescStruct const MinWorkTimeDesc;
extern TDataDescStruct const WeekEndDesc;
extern TDataDescStruct const DeferredStartDesc;

extern TDataDescStruct const PeriodWeekendIndexDesc;
extern TDataDescStruct const PeriodWeekdaysIndexDesc;
extern TDataDescStruct const ServiceNameDesc;
extern TDataDescStruct const PassDesc;
extern TDataDescStruct const PassCRCDesc;
extern TDataDescStruct const PassTempDesc;
extern CPU_INT32U TempPass;
extern TDataDescStruct const PassTempDesc1;
extern TDataDescStruct const PassTempDesc2;

extern TDataDescStruct const PriceWeekendDesc;
extern TDataDescStruct const PriceWeekdaysDesc;
extern TDataDescStruct const PriceTimeWeekendDesc;
extern TDataDescStruct const PriceTimeWeekdaysDesc;
extern TDataDescStruct const T_Start_WeekdaysDesc;
extern TDataDescStruct const T_End_WeekdaysDesc;
extern TDataDescStruct const T_Start_WeekendDesc;
extern TDataDescStruct const T_End_WeekendDesc;

extern TDataDescStruct const EnableFiscalDesc;
extern TDataDescStruct const EnableCoinDesc;
extern TDataDescStruct const EnableModemDesc;
extern TDataDescStruct const EnableValidatorDesc;
extern TDataDescStruct const CoinPerPulseDesc;
extern TDataDescStruct const EnableFiscalDayClearDesc;

extern TDataDescStruct const InitByDefaultDesc;
extern TDataDescStruct const PrintZReportDesc;
extern TDataDescStruct const PrintXReportDesc;

extern TDataDescStruct const EventJournalIndexDesc; 
extern TDataDescStruct const JournalEventTimeDesc;

extern TDataDescStruct const JournalErrorNumberDesc0;
extern TDataDescStruct const JournalErrorNumberDesc1;
extern TDataDescStruct const ClearJournalCmdDesc;


extern TDataDescStruct const SystemTimeDesc;
extern TDataDescStruct const SystemTimeEditDesc;

extern const TDataDescArrayStruct AllDataArray[];

extern CPU_INT32U ErrorJournalIndex;
extern CPU_INT32U EventJournalIndex;

extern TDataDescStruct const CounterRunDesc;
extern TDataDescStruct const CounterMoneyDesc;
extern TDataDescStruct const CounterTimeDesc;
extern TDataDescStruct const CounterChannelRunDesc;
extern TDataDescStruct const CounterChannelMoneyDesc;
extern TDataDescStruct const CounterChannelTimeDesc;
extern TDataDescStruct const ChannelStIndexDesc;
extern TDataDescStruct const ClearStatCmdDesc;
extern TDataDescStruct const BillFormatDesc;
extern TDataDescStruct const NameChannelDesc;

extern TDataDescStruct const AcceptedMoneyDesc;
extern TDataDescStruct const AcceptedMoneyCRC16Desc;

extern TDataDescStruct const DisableFiscalErrorsDesc;

extern TDataDescStruct const StartButtonNameDesc;

extern TDataDescStruct const EnableEmailErrorSendDesc;
extern TDataDescStruct const EnableEmailJournalSendDesc;
extern TDataDescStruct const ClearJournalAfterSendDesc;
extern TDataDescStruct const StatSendPeriodDesc;
extern TDataDescStruct const JournalErrorNumberDescEng;
extern TDataDescStruct const SendTestEmailDesc;
extern TDataDescStruct const ModemStatusDesc;

extern TDataDescStruct const BillnomIndexDesc;
extern TDataDescStruct const BillnomDesc;
extern TDataDescStruct const BillnomCountersDesc;
extern TDataDescStruct const BillCounterDesc;

extern TDataDescStruct const CounterLongRunDesc;
extern TDataDescStruct const CounterLongMoneyDesc;
extern TDataDescStruct const CounterLongTimeDesc;

extern TDataDescStruct const MasterPassTempDesc;

extern TDataDescStruct const CounterChannelRunLongDesc;
extern TDataDescStruct const CounterChannelMoneyLongDesc;
extern TDataDescStruct const CounterChannelTimeLongDesc;
extern TDataDescStruct const ChannelStLongIndexDesc;

extern TDataDescStruct const StatSendHourMinDesc;
extern TDataDescStruct const IncasSendFlagDesc;
extern TDataDescStruct const IncasMoneyDesc;
extern TDataDescStruct const IncasTimeDesc;

extern TDataDescStruct const GatewayDesc;
extern TDataDescStruct const NetMaskDesc;
extern TDataDescStruct const IpAddrDesc;

extern TDataDescStruct const PostIpAddrDesc;
extern TDataDescStruct const SelectProtectDesc;
extern TDataDescStruct const PostImpCostDesc;
extern TDataDescStruct const PostLenCostDesc;
extern TDataDescStruct const PostMinutePriceDesc;

extern TDataDescStruct const CashModeDesc;
extern TDataDescStruct const CashPerPulseDesc;
extern TDataDescStruct const PrintTimeoutDesc;

extern TDataDescStruct const PriceDesc;

extern void OnChangeInitByDefault(void);
extern void OnChangeServiceName(void);

extern TDataDescStruct const CashPulseLenDesc;
extern TDataDescStruct const CashPauseLenDesc;
extern void OnChangeCashPulseLen();

extern TDataDescStruct const PrintModeDesc;
extern TDataDescStruct const PrintTimeoutAfterDesc;
extern TDataDescStruct const TaxFormatDesc;
extern TDataDescStruct const SubjSellDesc;
extern TDataDescStruct const CommandV2Desc;
extern TDataDescStruct const TaxSystemDesc;

#endif //#ifndef _DATADESC_H_
