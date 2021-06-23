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

  // линия наличных денег
  CPU_INT32U FRAM_AcceptedMoney;
  CPU_INT32U crc_AcceptedMoney;
  
  // линия безналичных денег
  CPU_INT32U FRAM_AcceptedBankMoney;
  CPU_INT32U crc_AcceptedBankMoney;
  
  // линия остатка от продажи жетонов
  CPU_INT32U FRAM_AcceptedRestMoney;
  CPU_INT32U crc_AcceptedRestMoney;
  
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

  // настройки монетника
  CPU_INT32U  coin_pulse_len;
  CPU_INT32U  coin_pause_len;
  
  // настройки купюрника
  CPU_INT32U  cash_pulse_len;
  CPU_INT32U  cash_pause_len;
  
  // настройки банковского терминала
  CPU_INT32U  bank_pulse_len;
  CPU_INT32U  bank_pause_len;
  
  // настройки хоппера в режиме Cube
  CPU_INT32U  hopper_pulse_len;
  CPU_INT32U  hopper_pause_len;
  
  CPU_INT32U  TaxFormat;
  CPU_INT32U  SubjSell;
  CPU_INT32U  CommandV2;
  CPU_INT32U  TaxSystem;

  #ifdef CONFIG_FTP_CLIENT_ENABLE
  CPU_INT32U  FtpEnable;
  CPU_INT32U  FtpServerIpAddr;
  CPU_INT32U  FtpDeviceNumber;
  CPU_INT32U  FtpSendHourMin;
  CPU_INT32U  FtpSendIntervalIndex;
  CPU_INT32U  FtpLastTime;
  CPU_INT32U  FtpLastResult;
  char  FtpLogin[16];
  char  FtpPass[16];
#endif

  // линия выданных жетонов
  CPU_INT32U FRAM_AcceptedCoin;
  CPU_INT32U crc_AcceptedCoin;

}TFramMap;

