#include "cpu.h"
#include "datadesc.h"
#include "journal.h"


typedef struct
{

#ifdef BOARD_CENTRAL_CFG
    
  CPU_INT32U SerialNum;

  TChannelConfig ChannelConfig;

  TDeviceConfig DeviceConfig;

  // счетчики
  TCounters Counters;

  // длинные счетчики с CRC16 
  TCountersLong CountersLong;

  CPU_INT32U FRAM_AcceptedMoney;
  CPU_INT32U crc_AcceptedMoney;
  
  // журнал событий+ошибок
  TEventRecord EventRecords[EVENT_RECORDS_COUNT];

  CPU_INT32U Pass;
  CPU_INT32U crc_Pass;

  CPU_INT32U LastEmailTime;

  CPU_INT32U IncasEmailFlag;
  CPU_INT32U IncasMoney;
  CPU_INT32U IncasTime;

  CPU_INT32U StartButtonName;

  CPU_INT32U  DefferedStartEnabled[CHANNELS_NUM];
#endif
  
  CPU_INT08U  mac_addr[6];
  CPU_INT32U  ip;
  CPU_INT32U  netmask;
  CPU_INT32U  gateway;

  CPU_INT16U  port;
  
  CPU_INT08U manual_service_flag[4];
  char manual_service_name[32];

  CPU_INT32U  cash_pulse_len;
  CPU_INT32U  cash_pause_len;

  CPU_INT32U  TaxFormat;
  CPU_INT32U  SubjSell;
  CPU_INT32U  CommandV2;
  CPU_INT32U  TaxSystem;

}TFramMap;

