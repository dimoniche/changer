#include <includes.h>
#include "data.h"
#include "datadesc.h"
#include "menu.h"
#include "menudesc.h"
#include "fram.h"
#include "fram_map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "control.h"
#include "fiscal.h"
#include "time.h"
#include "CRC16.h"
#include "modem_task.h"
#include "modem.h"
#include "coin.h"
#include "ftp_app.h"

extern CPU_INT32U modem_status;

/*************************************
  ������ ������
*************************************/
CPU_INT32U  ChannelIndex=0;
TRangeValueULONG const ChannelIndexRange = {0, CHANNELS_NUM-1};
CPU_INT08U const ChannelIndexName[] = "    ����";
CPU_INT08U const* ChannelItems[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

TDataDescStruct const ChannelIndexDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  &ChannelIndex,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&ChannelIndexRange,       // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  ChannelIndexName,         // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  ChannelItems,             // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  ������ ������ � ���� ��������� ���������
*************************************/
CPU_INT08U const ChannelStIndexName[] = "���.��.����";

TDataDescStruct const ChannelStIndexDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  &ChannelIndex,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&ChannelIndexRange,       // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  ChannelStIndexName,         // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  ChannelItems,             // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  ������ ������ � ���� ��������� ���������
*************************************/
CPU_INT08U const ChannelStLongIndexName[] = "��.��.�����";

TDataDescStruct const ChannelStLongIndexDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  &ChannelIndex,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&ChannelIndexRange,       // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  ChannelStLongIndexName,         // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  ChannelItems,             // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  ��������� ����� �������� email
*************************************/
extern TRangeValueULONG const WorkTimeRange;

TDataDescStruct const LastEmailSendTime = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, LastEmailTime),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&WorkTimeRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  NULL,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ��������� ������
*************************************/
TRangeValueULONG const EnableChannelRange = {0, 1};
CPU_INT08U const EnableChannelName[] = "��������";
CPU_INT08U const EnableChannelList_str0[] = "���";
CPU_INT08U const EnableChannelList_str1[] = "��";
CPU_INT08U const *EnableChannelList[] = {EnableChannelList_str0, EnableChannelList_str1};


TDataDescStruct const EnableChannelDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.Enable),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableChannelRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableChannelName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableChannelList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� �������� ����� ���������������, ��
*************************************/
TRangeValueULONG const CoinPulseLenRange = {20, 250};
CPU_INT08U const CoinPulseLenName[] = "����� ���.,��";

void OnChangeCoinPulseLen()
{
    CPU_INT32U pulse, pause;
    
    GetData(&CoinPulseLenDesc, &pulse, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&CoinPauseLenDesc, &pause, 0, DATA_FLAG_SYSTEM_INDEX);
    SetCoinPulseParam(pulse, pause);
}

TDataDescStruct const CoinPulseLenDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, coin_pulse_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&CoinPulseLenRange,     // ��������� �� ������� ���������
  OnChangeCoinPulseLen,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  CoinPulseLenName,         // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  50                           
};

/*************************************
  ����� ����� ����� ���������, ��
*************************************/
TRangeValueULONG const CoinPauseLenRange = {20, 250};
CPU_INT08U const CoinPauseLenName[] = "����� ���.,��";

TDataDescStruct const CoinPauseLenDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, coin_pause_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&CoinPauseLenRange,     // ��������� �� ������� ���������
  OnChangeCoinPulseLen,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  CoinPauseLenName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  20                           
};

/*************************************
  ����� �������� ����� ���������, ��
*************************************/
TRangeValueULONG const CashPulseLenRange = {20, 250};
CPU_INT08U const CashPulseLenName[] = "����� ���.,��";

void OnChangeCashPulseLen()
{
    CPU_INT32U pulse, pause;
    GetData(&CashPulseLenDesc, &pulse, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&CashPauseLenDesc, &pause, 0, DATA_FLAG_SYSTEM_INDEX);
    SetCashPulseParam(pulse, pause);
}


TDataDescStruct const CashPulseLenDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, cash_pulse_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&CashPulseLenRange,     // ��������� �� ������� ���������
  OnChangeCashPulseLen,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  CashPulseLenName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  50                           
};

/*************************************
  ����� ����� ����� ���������, ��
*************************************/
TRangeValueULONG const CashPauseLenRange = {20, 250};
CPU_INT08U const CashPauseLenName[] = "����� ���.,��";

TDataDescStruct const CashPauseLenDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, cash_pause_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&CashPauseLenRange,     // ��������� �� ������� ���������
  OnChangeCashPulseLen,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  CashPauseLenName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  20                           
};

/*************************************
  ����� �������� ����� ����������� ���������, ��
*************************************/
TRangeValueULONG const BankPulseLenRange = {20, 250};
CPU_INT08U const BankPulseLenName[] = "����� ���.,��";

void OnChangeBankPulseLen()
{
    CPU_INT32U pulse, pause;

    GetData(&BankPulseLenDesc, &pulse, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&BankPauseLenDesc, &pause, 0, DATA_FLAG_SYSTEM_INDEX);
    SetBankPulseParam(pulse, pause);
}

TDataDescStruct const BankPulseLenDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, bank_pulse_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&BankPulseLenRange,     // ��������� �� ������� ���������
  OnChangeBankPulseLen,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  BankPulseLenName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  50                           
};

/*************************************
  ����� ����� ����� ����������� ���������, ��
*************************************/
TRangeValueULONG const BankPauseLenRange = {20, 250};
CPU_INT08U const BankPauseLenName[] = "����� ���.,��";

TDataDescStruct const BankPauseLenDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, bank_pause_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&BankPauseLenRange,     // ��������� �� ������� ���������
  OnChangeBankPulseLen,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  BankPauseLenName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  20                           
};

/*************************************
  ���� �������� ����������� ���������
*************************************/
TRangeValueULONG const BankPerPulseRange = {1, 9999};
CPU_INT08U const BankPerPulseName[] = "���./���.";

TDataDescStruct const BankPerPulseDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.BankPerPulse),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&BankPerPulseRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  BankPerPulseName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  10                           // �������� �� ���������
};

void OnChangeLevel()
{
    CPU_INT32U level1, level2, level3, level4;
 
    GetData(&CashLevelDesc, &level1, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&BankLevelDesc, &level2, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&CoinLevelDesc, &level3, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&CoinLevelDesc, &level4, 0, DATA_FLAG_SYSTEM_INDEX);
    
    SetLevelParam(level1, level2, level3, level4);
 
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    OS_ENTER_CRITICAL();
    InitInputPorts();
    OS_EXIT_CRITICAL();
}

void OnChangeLevelWithoutInit()
{
    CPU_INT32U level1, level2, level3, level4;

    GetData(&CashLevelDesc, &level1, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&BankLevelDesc, &level2, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&CoinLevelDesc, &level3, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&CoinLevelDesc, &level4, 0, DATA_FLAG_SYSTEM_INDEX);
    
    SetLevelParam(level1, level2, level3, level4);
}

/*************************************
  ������� ������� ���������
*************************************/

TRangeValueULONG const LevelRange = {0, 1};
CPU_INT08U const LevelName[] = "�������";
CPU_INT08U const Level_str0[] = "LOW";
CPU_INT08U const Level_str1[] = "HIGH";
CPU_INT08U const *LevelList[] = {Level_str0, Level_str1};

TDataDescStruct const CoinLevelDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.CoinLevel),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&LevelRange,     // ��������� �� ������� ���������
  OnChangeLevel,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  LevelName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  LevelList,                  // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                          // �������� �� ���������
};

/*************************************
  ������� ������� ���������
*************************************/
TDataDescStruct const CashLevelDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.CashLevel),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&LevelRange,     // ��������� �� ������� ���������
  OnChangeLevel,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  LevelName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  LevelList,                  // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                          // �������� �� ���������
};

/*************************************
  ������� ������� ����������� ���������
*************************************/
TDataDescStruct const BankLevelDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.BankLevel),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&LevelRange,     // ��������� �� ������� ���������
  OnChangeLevel,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  LevelName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  LevelList,                  // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                          // �������� �� ���������
};

/*************************************
  ������� ������� ������� � ������ Cube
*************************************/
TDataDescStruct const HopperLevelDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.HopperLevel),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&LevelRange,     // ��������� �� ������� ���������
  OnChangeLevel,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  LevelName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  LevelList,                  // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                          // �������� �� ���������
};

/*************************************
  IP-����� �����
*************************************/
CPU_INT08U const PostIpAddrName[] = "IP";

TDataDescStruct const PostIpAddrDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_IP_ADDR,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.post_ip),            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PostIpAddrName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0xC0A80065
};

/*************************************
  ������������ ������ ������ �����
*************************************/
TRangeValueULONG const SelectProtectNameRange = {0, 1};
CPU_INT08U const SelectProtectNameName[] = "������ ������";
CPU_INT08U const SelectProtectNameList_str0[] = "���";
CPU_INT08U const SelectProtectNameList_str1[] = "��";
CPU_INT08U const *SelectProtectNameList[] = {SelectProtectNameList_str0, SelectProtectNameList_str1};


TDataDescStruct const SelectProtectDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,                        // ������ �������
  &ChannelIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.select_protect),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&SelectProtectNameRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),                        // �������� ����� ���������� � �������
  SelectProtectNameName,      // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  SelectProtectNameList,      // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  ���� �������� ����������� �����
*************************************/
TRangeValueULONG const PostImpCostRange = {1, 9999};
CPU_INT08U const PostImpCostName[] = "���./���.";

TDataDescStruct const PostImpCostDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.imp_cost),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PostImpCostRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PostImpCostName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  10                           // �������� �� ���������
};

/*************************************
  ����� �������� ����������� �����
*************************************/
TRangeValueULONG const PostImpLenRange = {1, 999};
CPU_INT08U const PostImpLenName[] = "��.���.,��";

TDataDescStruct const PostLenCostDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.imp_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PostImpLenRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PostImpLenName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  50                           // �������� �� ���������
};

/*************************************
  ���� ������
*************************************/
TRangeValueULONG const PostMinutePriceRange = {1, 999};
CPU_INT08U const PostMinutePriceName[] = "�����.���.";

TDataDescStruct const PostMinutePriceDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.minute_cost),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PostMinutePriceRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PostMinutePriceName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  20                           // �������� �� ���������
};

/*************************************
  �������� ������
*************************************/
TRangeValueULONG const NameChannelRange = {0, 2};
CPU_INT08U const NameChannelName[] = "��������";
CPU_INT08U const NameChannelList_str0[] = "#";
CPU_INT08U const NameChannelList_str1[] = "�������";
CPU_INT08U const NameChannelList_str2[] = "����������";
CPU_INT08U const *NameChannelList[] = {NameChannelList_str0, NameChannelList_str1, NameChannelList_str2};


TDataDescStruct const NameChannelDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.Name),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&NameChannelRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  NameChannelName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  NameChannelList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  �������� ������ ����� (����� ��� ����)
*************************************/
TRangeValueULONG const StartButtonNameRange = {0, 1};
CPU_INT08U const StartButtonNameName[] = "������";
CPU_INT08U const StartButtonNameList_str0[] = "�����";
CPU_INT08U const StartButtonNameList_str1[] = "����";
CPU_INT08U const *StartButtonNameList[] = {StartButtonNameList_str0, StartButtonNameList_str1};


TDataDescStruct const StartButtonNameDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, StartButtonName),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&StartButtonNameRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  StartButtonNameName,      // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  StartButtonNameList,      // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  ��������� ���������������
*************************************/
TRangeValueULONG const EnableValidatorRange = {0, 1};
CPU_INT08U const EnableValidatorName[] = "��������-�";
CPU_INT08U const OnOffList_str0[] = "����.";
CPU_INT08U const OnOffList_str1[] = "���.";
CPU_INT08U const *EnableValidatorList[] = {OnOffList_str0, OnOffList_str1};


TDataDescStruct const EnableValidatorDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.EnableValidator),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableValidatorRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableValidatorName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableValidatorList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           // �������� �� ���������
};

/*************************************
  ��������� ����������� ���������
*************************************/
TRangeValueULONG const EnableBankRange = {0, 1};
CPU_INT08U const EnableBankName[] = "����.������.";

TDataDescStruct const EnableBankDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.EnableBank),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableBankRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableBankName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableValidatorList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           // �������� �� ���������
};

/*************************************
  ��������� ������
*************************************/
TRangeValueULONG const EnableModemRange = {0, 1};
CPU_INT08U const EnableModemName[] = "�����";
CPU_INT08U const *EnableModemList[] = {OnOffList_str0, OnOffList_str1};

void OnchangeEnableModem(void)
{
    CPU_INT32U en = 0;
    GetData(&EnableModemDesc, &en, 0, DATA_FLAG_SYSTEM_INDEX);
    
    if (en)
    {
        if (!IsModemConn())
        {
            modem_status = 2;
        }
        else if (!IsModemConf())
        {
            modem_status = 1;
        }
        PostModemTask(MODEM_TASK_RECONNECT);
    }
}

TDataDescStruct const EnableModemDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.EnableModem),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableModemRange,     // ��������� �� ������� ���������
  OnchangeEnableModem,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableModemName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableModemList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  �������� ���������� �� ������� �� e-mail
*************************************/
TRangeValueULONG const EnableEmailErrorSendRange = {0, 1};
CPU_INT08U const EnableEmailErrorSendName[] = "����.�� ��.";
CPU_INT08U const *EnableEmailErrorSendList[] = {OnOffList_str0, OnOffList_str1};

TDataDescStruct const EnableEmailErrorSendDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.EnableEmailErrorSend),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableEmailErrorSendRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableEmailErrorSendName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableEmailErrorSendList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  ������ ����������� ������
*************************************/
CPU_INT08U const ModemStatusName[] = "������";
CPU_INT08U const *ModemStatusList[] = {"��", "��.����.", "��� �����"};

TDataDescStruct const ModemStatusDesc = {
  DATA_DESC_VIEW,          // ��� �����������
  DATA_TYPE_ULONG,         // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,           // ������� �������
  0,                       // ������ �������
  0,                       // ��������� �� ��������� ������� �������
  &modem_status,           // ��������� �� ���������� ��� ����� FRAM
  NULL,                    // ��������� �� ������� ���������
  NULL,                    // ������� �� ���������
  0,      // �������� ����� ���������� � �������
  ModemStatusName, // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,           // ������� ���������� ��������� (������ �����)
  ModemStatusList,// ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  �������� �������� �������� �� e-mail
*************************************/
CPU_INT08U const EnableEmailJournalSendName[] = "����.�������";

TDataDescStruct const EnableEmailJournalSendDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.EnableEmailJournalSend),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableEmailErrorSendRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableEmailJournalSendName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableEmailErrorSendList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  ������� ������ ����� �������� �� e-mail �� ������������
*************************************/
CPU_INT08U const ClearJournalAfterSendName[] = "��.�������";

TDataDescStruct const ClearJournalAfterSendDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.ClearJournalAfterSend),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableEmailErrorSendRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  ClearJournalAfterSendName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableEmailErrorSendList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
����� �������� ����������, ��� : ���
*************************************/
TRangeValueULONG const StatSendHourRange = {0, 60*24 - 1};
CPU_INT08U const StatSendHourName[] = "T����.";

TDataDescStruct const StatSendHourMinDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_HOUR_MIN,    // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.StatSendHourMin),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&StatSendHourRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  StatSendHourName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  9                           
};

/*************************************
  ��������� ���������������
*************************************/
TRangeValueULONG const EnableCoinRange = {0, 1};
CPU_INT08U const EnableCoinName[] = "��������-�";
CPU_INT08U const *EnableCoinList[] = {OnOffList_str0, OnOffList_str1};

void OnchangeEnableCoin(void)
{
}

TDataDescStruct const EnableCoinDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.EnableCoinAcceptor),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableCoinRange,     // ��������� �� ������� ���������
  OnchangeEnableCoin,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableCoinName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableCoinList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  ���� �������� ���������������
*************************************/
TRangeValueULONG const CoinPerPulseRange = {1, 9999};
CPU_INT08U const CoinPerPulseName[] = "���./���.";

TDataDescStruct const CoinPerPulseDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.CoinPerPulse),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&CoinPerPulseRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  CoinPerPulseName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  10                        // �������� �� ���������
};


/*************************************
  ����� ����������
*************************************/
TRangeValueULONG const CashModeRange = {0, 1};
CPU_INT08U const CashModeName[] = "�����";
CPU_INT08U const CashMode_str0[] = "CCNet";
CPU_INT08U const CashMode_str1[] = "Pulse";
CPU_INT08U const *CashModeList[] = {CashMode_str0, CashMode_str1};

TDataDescStruct const CashModeDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.CashMode),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&CashModeRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  CashModeName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  CashModeList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������ ����
*************************************/
TRangeValueULONG const PrintModeRange = {0, 1};
CPU_INT08U const PrintModeName[] = "���.������";
CPU_INT08U const PrintMode_str0[] = "�������";
CPU_INT08U const PrintMode_str1[] = "������";
CPU_INT08U const *PrintModeList[] = {PrintMode_str0, PrintMode_str1};

TDataDescStruct const PrintModeDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.PrintMode),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PrintModeRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PrintModeName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  PrintModeList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  ������� ������ ����
*************************************/
TRangeValueULONG const PrintTimeoutRange = {0, 3600};
CPU_INT08U const PrintTimeoutName[] = "������� ������";

TDataDescStruct const PrintTimeoutDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.PrintTimeout),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PrintTimeoutRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  PrintTimeoutName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           // �������� �� ���������
};

/*************************************
  ������� ������ ������ ����
*************************************/
TRangeValueULONG const PrintTimeoutAfterRange = {0, 60};
CPU_INT08U const PrintTimeoutAfterName[] = "������� �����.";

TDataDescStruct const PrintTimeoutAfterDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.PrintTimeoutAfter),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PrintTimeoutAfterRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  PrintTimeoutAfterName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  10                           // �������� �� ���������
};

/*************************************
  ���� �������� ��������� � ���������� ������
*************************************/
TRangeValueULONG const CashPerPulseRange = {1, 9999};
CPU_INT08U const CashPerPulseName[] = "���./���.";

TDataDescStruct const CashPerPulseDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.CashPerPulse),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&CashPerPulseRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  CashPerPulseName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  10                           // �������� �� ���������
};


/*************************************
  ��������� ��
*************************************/
TRangeValueULONG const EnableFiscalRange = {0, 1};
CPU_INT08U const EnableFiscalName[] = "��";
CPU_INT08U const *EnableFiscalList[] = {OnOffList_str0, OnOffList_str1};

TDataDescStruct const EnableFiscalDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.EnableFiscal),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableFiscalRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableFiscalName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableFiscalList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  ��������� ��
*************************************/
TRangeValueULONG const DisableFiscalErrorsRange = {0, 1};
CPU_INT08U const DisableFiscalErrorsName[] = "�������.��.��";
CPU_INT08U const DisableFiscalErrorsList_str0[] = "���";
CPU_INT08U const DisableFiscalErrorsList_str1[] = "��";
CPU_INT08U const *DisableFiscalErrorsList[] = {DisableFiscalErrorsList_str0, DisableFiscalErrorsList_str1};

TDataDescStruct const DisableFiscalErrorsDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.DisableFiscalErrors),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&DisableFiscalErrorsRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  DisableFiscalErrorsName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  DisableFiscalErrorsList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ������
*************************************/
CPU_INT32U  WorkTime[CHANNELS_NUM];
TRangeValueULONG const WorkTimeRange = {0, 0xffffffffL};
CPU_INT08U const WorkTimeName[] = "��.������";

TDataDescStruct const WorkTimeDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,                     // ��������� �� ��������� ������� �������
  &WorkTime,                // ��������� �� ���������� ��� ����� FRAM
  (void*)&WorkTimeRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  WorkTimeName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  ����-��� ����� �������� ����������
*************************************/
TRangeValueULONG const TimeOutBeforeRange = {0, 999};
CPU_INT08U const TimeOutBeforeName[] = "����� ��,���.";

TDataDescStruct const TimeOutBeforeDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.TimeOutBefore),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&TimeOutBeforeRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  TimeOutBeforeName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  30                           
};

/*************************************
  ����-��� ����� ������ ����������
*************************************/
TRangeValueULONG const TimeOutAfterRange = {0, 99};
CPU_INT08U const TimeOutAfterName[] = "����� �����,���.";

TDataDescStruct const TimeOutAfterDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.TimeOutAfter),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&TimeOutAfterRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  TimeOutAfterName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  3                           
};

/*************************************
  ������������ ����� ����������, ���.
*************************************/
TRangeValueULONG const MaxWorkTimeRange = {1, 999};
CPU_INT08U const MaxWorkTimeName[] = "Tmax,���.";

TDataDescStruct const MaxWorkTimeDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.MaxWorkTime),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&MaxWorkTimeRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  MaxWorkTimeName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  20                           
};

/*************************************
  ����������� ����� ����������, ���.
*************************************/
TRangeValueULONG const MinWorkTimeRange = {1, 999};
CPU_INT08U const MinWorkTimeName[] = "Tmin,���.";

TDataDescStruct const MinWorkTimeDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.MinWorkTime),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&MinWorkTimeRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  MinWorkTimeName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  5                           
};

/*************************************
  ��������� ��������
*************************************/
TRangeValueULONG const WeekEndRange = {0, 4};
CPU_INT08U const WeekEndName[] = "��������:";
CPU_INT08U const WeekEndList_str0[] = "���";
CPU_INT08U const WeekEndList_str1[] = "��-��";
CPU_INT08U const WeekEndList_str2[] = "��-��";
CPU_INT08U const WeekEndList_str3[] = "��-��";
CPU_INT08U const WeekEndList_str4[] = "��-��";
CPU_INT08U const *WeekEndList[] = {WeekEndList_str0, WeekEndList_str1, WeekEndList_str2, WeekEndList_str3, WeekEndList_str4, NULL};

TDataDescStruct const WeekEndDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.WeekEnd),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&WeekEndRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  WeekEndName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  WeekEndList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ��������� ����������� ������
*************************************/
TRangeValueULONG const DeferredStartRange = {0, 1};
CPU_INT08U const DeferredStartName[] = "�����.�����";
CPU_INT08U const DeferredStart_str0[] = "���";
CPU_INT08U const DeferredStart_str1[] = "��";
CPU_INT08U const *DeferredStartList[] = {DeferredStart_str0, DeferredStart_str1, NULL};

TDataDescStruct const DeferredStartDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM,             // ������ �������
  &ChannelIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DefferedStartEnabled),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&DeferredStartRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  DeferredStartName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  DeferredStartList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ������ ��� ��������� ���
*************************************/
TRangeValueULONG const PeriodIndexRange = {0, 0xffffffff/*CHANNELS_NUM*PRICE_PERIODS_NUM-1*/};
CPU_INT08U const PeriodIndexName[] = "";
CPU_INT08U const *PeriodWeekendIndexList[] = {
                                              "���.1 ���.1 ���.",
                                              "���.1 ���.2 ���.",
                                              "���.1 ���.3 ���.",
                                              "���.1 ���.4 ���.",
                                              "���.2 ���.1 ���.",
                                              "���.2 ���.2 ���.",
                                              "���.2 ���.3 ���.",
                                              "���.2 ���.4 ���.",
                                              "���.3 ���.1 ���.",
                                              "���.3 ���.2 ���.",
                                              "���.3 ���.3 ���.",
                                              "���.3 ���.4 ���.",
                                              "���.4 ���.1 ���.",
                                              "���.4 ���.2 ���.",
                                              "���.4 ���.3 ���.",
                                              "���.4 ���.4 ���.",
                                              "���.5 ���.1 ���.",
                                              "���.5 ���.2 ���.",
                                              "���.5 ���.3 ���.",
                                              "���.5 ���.4 ���.",
                                              "���.6 ���.1 ���.",
                                              "���.6 ���.2 ���.",
                                              "���.6 ���.3 ���.",
                                              "���.6 ���.4 ���.",
                                              "���.7 ���.1 ���.",
                                              "���.7 ���.2 ���.",
                                              "���.7 ���.3 ���.",
                                              "���.7 ���.4 ���.",
                                              "���.8 ���.1 ���.",
                                              "���.8 ���.2 ���.",
                                              "���.8 ���.3 ���.",
                                              "���.8 ���.4 ���.",
                                              "���.9 ���.1 ���.",
                                              "���.9 ���.2 ���.",
                                              "���.9 ���.3 ���.",
                                              "���.9 ���.4 ���.",
                                              "���.10 ���.1 ���.",
                                              "���.10 ���.2 ���.",
                                              "���.10 ���.3 ���.",
                                              "���.10 ���.4 ���.",
                                               NULL};

CPU_INT32U PeriodIndex = 0;

void OnChangePeriodIndex(void)
{
  if ((PeriodIndex == 0xffffffff) || (PeriodIndex < ChannelIndex*PRICE_PERIODS_NUM)) PeriodIndex = (ChannelIndex+1)*PRICE_PERIODS_NUM-1;
  else if (PeriodIndex >= (ChannelIndex+1)*PRICE_PERIODS_NUM) PeriodIndex = (ChannelIndex)*PRICE_PERIODS_NUM;
}

TDataDescStruct const PeriodWeekendIndexDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)&PeriodIndex,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PeriodIndexRange,     // ��������� �� ������� ���������
  OnChangePeriodIndex,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  PeriodIndexName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  PeriodWeekendIndexList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

CPU_INT08U const *PeriodWeekdaysIndexList[] = {
                                              "���.1 ���.1 ���.",
                                              "���.1 ���.2 ���.",
                                              "���.1 ���.3 ���.",
                                              "���.1 ���.4 ���.",
                                              "���.2 ���.1 ���.",
                                              "���.2 ���.2 ���.",
                                              "���.2 ���.3 ���.",
                                              "���.2 ���.4 ���.",
                                              "���.3 ���.1 ���.",
                                              "���.3 ���.2 ���.",
                                              "���.3 ���.3 ���.",
                                              "���.3 ���.4 ���.",
                                              "���.4 ���.1 ���.",
                                              "���.4 ���.2 ���.",
                                              "���.4 ���.3 ���.",
                                              "���.4 ���.4 ���.",
                                              "���.5 ���.1 ���.",
                                              "���.5 ���.2 ���.",
                                              "���.5 ���.3 ���.",
                                              "���.5 ���.4 ���.",
                                              "���.6 ���.1 ���.",
                                              "���.6 ���.2 ���.",
                                              "���.6 ���.3 ���.",
                                              "���.6 ���.4 ���.",
                                              "���.7 ���.1 ���.",
                                              "���.7 ���.2 ���.",
                                              "���.7 ���.3 ���.",
                                              "���.7 ���.4 ���.",
                                              "���.8 ���.1 ���.",
                                              "���.8 ���.2 ���.",
                                              "���.8 ���.3 ���.",
                                              "���.8 ���.4 ���.",
                                              "���.9 ���.1 ���.",
                                              "���.9 ���.2 ���.",
                                              "���.9 ���.3 ���.",
                                              "���.9 ���.4 ���.",
                                              "���.10 ���.1 ���.",
                                              "���.10 ���.2 ���.",
                                              "���.10 ���.3 ���.",
                                              "���.10 ���.4 ���.",
                                               NULL};
TDataDescStruct const PeriodWeekdaysIndexDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)&PeriodIndex,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PeriodIndexRange,     // ��������� �� ������� ���������
  OnChangePeriodIndex,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  PeriodIndexName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  PeriodWeekdaysIndexList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  ���� �� ��������
*************************************/
TRangeValueULONG const PriceWeekendRange = {1, MAX_PRICE};
CPU_INT08U const PriceWeekendName[] = "����,���.";

TDataDescStruct const PriceWeekendDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // ������ �������
  &PeriodWeekendIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.Price_Weekend),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&PriceWeekendRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PriceWeekendName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  15                           
};

/*************************************
  ���� �� ������
*************************************/
TRangeValueULONG const PriceWeekdaysRange = {1, MAX_PRICE};
CPU_INT08U const PriceWeekdaysName[] = "����,���.";

TDataDescStruct const PriceWeekdaysDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // ������ �������
  &PeriodWeekdaysIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.Price_Weekdays),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&PriceWeekdaysRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PriceWeekdaysName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  15                         
};


/*************************************
  ����� �� ���� �� ��������
*************************************/
TRangeValueULONG const PriceTimeWeekendRange = {1, 999};
CPU_INT08U const PriceTimeWeekendName[] = "�� �����,���.";

TDataDescStruct const PriceTimeWeekendDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // ������ �������
  &PeriodWeekendIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.PriceTime_Weekend),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&PriceTimeWeekendRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PriceTimeWeekendName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  ����� �� ���� �� ������
*************************************/
TRangeValueULONG const PriceTimeWeekdaysRange = {1, 999};
CPU_INT08U const PriceTimeWeekdaysName[] = "�� �����,���.";

TDataDescStruct const PriceTimeWeekdaysDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // ������ �������
  &PeriodWeekdaysIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.PriceTime_Weekdays),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&PriceTimeWeekdaysRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  PriceTimeWeekdaysName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};


/*************************************
  ������ �������� �� ������
*************************************/
TRangeValueULONG const T_Start_WeekdaysRange = {0, 24};
CPU_INT08U const T_Start_WeekdaysName[] = "������,�";

TDataDescStruct const T_Start_WeekdaysDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // ������ �������
  &PeriodWeekdaysIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.T_Start_Weekdays),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&T_Start_WeekdaysRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  T_Start_WeekdaysName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� �������� �� ������
*************************************/
TRangeValueULONG const T_End_WeekdaysRange = {0, 24};
CPU_INT08U const T_End_WeekdaysName[] = "�����,�";

TDataDescStruct const T_End_WeekdaysDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // ������ �������
  &PeriodWeekdaysIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.T_End_Weekdays),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&T_End_WeekdaysRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  T_End_WeekdaysName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  24                           
};


/*************************************
  ������ �������� �� ��������
*************************************/
TRangeValueULONG const T_Start_WeekendRange = {0, 24};
CPU_INT08U const T_Start_WeekendName[] = "������,�";

TDataDescStruct const T_Start_WeekendDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // ������ �������
  &PeriodWeekendIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.T_Start_Weekend),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&T_Start_WeekendRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  T_Start_WeekendName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� �������� �� ��������
*************************************/
TRangeValueULONG const T_End_WeekendRange = {0, 24};
CPU_INT08U const T_End_WeekendName[] = "�����,�";

TDataDescStruct const T_End_WeekendDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // ������ �������
  &PeriodWeekendIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ChannelConfig.T_End_Weekend),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&T_End_WeekendRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  T_End_WeekendName,             // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  24                           
};


/*************************************
  ������� ������������� �� ���������
*************************************/
CPU_INT32U InitByDefault = 0;

TRangeValueULONG const InitByDefaultRange = {0, 1};
CPU_INT08U const InitByDefaultName[] = "�������������";
CPU_INT08U const InitByDefaultList_str0[] = "���";
CPU_INT08U const InitByDefaultList_str1[] = "��";
CPU_INT08U const *InitByDefaultList[] = {InitByDefaultList_str0, InitByDefaultList_str1};


void OnChangeInitByDefault(void)
{
  int i = 0;
  if (InitByDefault == 0) return;
  while (AllDataArray[i].ptr != NULL)
    {
      InitDescByDefault(AllDataArray[i].ptr);
      i++;
    } 
  InitByDefault = 0;
  
  CPU_INT08U flag[4] = {0xAA, 0x55, 0x81, 0xC3};
  char name[32] = "������ ����� ����������������\0\0\0";
  WriteArrayFram(offsetof(TFramMap, manual_service_flag), 4, (unsigned char*)&flag);
  WriteArrayFram(offsetof(TFramMap, manual_service_name), 32, (unsigned char*)&name);
}


TDataDescStruct const InitByDefaultDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&InitByDefault,                // ��������� �� ���������� ��� ����� FRAM
  (void*)&InitByDefaultRange,           // ��������� �� ������� ���������
  OnChangeInitByDefault,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  InitByDefaultName,             // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  InitByDefaultList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  ������� �� ������ Z-������
*************************************/
CPU_INT32U PrintZReportCmd = 0;

CPU_INT08U const PrintZReportName[] = "���.� ����.��.";
CPU_INT08U const PrintZReportList_str0[] = "���";
CPU_INT08U const PrintZReportList_str1[] = "������";
CPU_INT08U const *PrintZReportList[] = {PrintZReportList_str0, PrintZReportList_str1};


void OnChangePrintZReportCmd(void)
{
}

TDataDescStruct const PrintZReportDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&PrintZReportCmd,                // ��������� �� ���������� ��� ����� FRAM
  (void*)&InitByDefaultRange,           // ��������� �� ������� ���������
  OnChangePrintZReportCmd,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  PrintZReportName,             // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  PrintZReportList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  ������� �� ������ X-������
*************************************/
CPU_INT32U PrintXReportCmd = 0;

CPU_INT08U const PrintXReportName[] = "X-�����";

void OnChangePrintXReportCmd(void)
{
}

TDataDescStruct const PrintXReportDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&PrintXReportCmd,                // ��������� �� ���������� ��� ����� FRAM
  (void*)&InitByDefaultRange,           // ��������� �� ������� ���������
  OnChangePrintXReportCmd,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  PrintXReportName,             // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  PrintZReportList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  ������ � ������� ������
*************************************/
TRangeValueULONG const ErrorNumberRange = {0, JOURNAL_EVENTS_COUNT - 1};
CPU_INT08U const *ErrorNumberList0[JOURNAL_EVENTS_COUNT] = {
                                        "���", 
                                        "", 
                                        "", 
                                        "",
                                        "", 
                                        "", 
                                        "", 
                                        "",
                                        "", 
                                        "", 
                                        "", 
                                        "",
                                        "", 
                                        "", 
                                        "", 
                                        "",
                                        
                                        "",
                                        "",
                                        "����� ���������",
                                        "������ ��������",
                                        "������",
                                        
                                        "������ ������ �������",
                                        "������ ����",
                                        
                                       "������ ����� �", 
                                       "������ ������", 
                                       "1Ch60h-����.������", 
                                       "1Ch61h-����.������",
                                       "1Ch64h-����.������",
                                       "1Ch65h-����.������",
                                       "1Ch66h-����.������",
                                       "1Ch67h-����.������",
                                       "1Ch68h-����.������",
                                       "1Ch69h-����.������",
                                       "1Ch6Ch-����.������",
                                       "�/�:41h-�������",
                                       "�/�:42h-�������",
                                       "�/�:43h-����� � ",
                                       "�/�:44h-�����",
                                       "�/�:45h-�������",
                                       "�/�:50h-������",
                                       "�/�:51h-������ ����.",
                                       "�/�:52h-������",
                                       "�/�:53h-������ ���.",
                                       "�/�:54h-�������",
                                       "�/�:65h-������",
                                       "�/�:66h-������",
                                       "�/�:67h-������ ���.",
                                       "������", 
                                       "������",
                                       "��:01h-�����������",
                                       "��:02h-��������",
                                       "��:03h-������ ��",
                                       "��:04h-������ ��",
                                       "��:05h-�������� ����",  
                                       "��:06h-����� ��", 
                                       "��:07h-�������� ����", 
                                       "��:08h-��� ", 
                                       "��:09h-������.", 
                                       "��:10h-����������", 
                                       "��:11h-���", 
                                       "��:12h-��������",
                                       "��:14h-��������", 
                                       "��:15h-��������", 
                                       "��:16h-����� �����",
                                       "��:17h-��������", 
                                      "��:20h-���������", 
                                      "��:2Fh-������� ",
                                      "��:30h-�� ", 
                                      "��:33h-������������", 
                                      "��:34h-���", 
                                      "��:35h-������������", 
                                      "��:36h-������������", 
                                      "��:37h-�������", 
                                      "��:38h-������", 
                                      "��:39h-����������", 
                                      "��:3Ah-������������", 
                                      "��:3Ch-�����", 
                                      "��:3Dh-�����", 
                                      "��:3Eh-������������", 
                                      "��:3Fh-������������", 
                                      "��:40h-������������", 
                                      "��:41h-������������", 
                                      "��:42h-������������", 
                                      "��:43h-������������", 
                                      "��:44h-������������", 
                                      "��:45h-C����", 
                                      "��:46h-�� �������", 
                                      "��:47h-������������", 
                                      "��:48h-������������", 
                                      "��:49h-����.������.", 
                                      "��:4Ah-������ ���", 
                                      "��:4Bh-����� ����", 
                                      "��:4Ch-������������", 
                                      "��:4Dh-��������", 
                                      "��:4Eh-�����", 
                                      "��:4Fh-�������� ������", 
                                      "��:50h-���� ������", 
                                      "��:51h-������������", 
                                      "��:52h-������������", 
                                      "��:53h-������������", 
                                      "��:54h-������������", 
                                      "��:55h-��� ������", 
                                      "��:56h-��� ���.", 
                                      "��:58h-��������", 
                                      "��:59h-��������", 
                                      "��:5Bh-������������", 
                                      "��:5Ch-��������", 
                                      "��:5Dh-�������", 
                                      "��:5Eh-������������",
                                      "��:5Fh-�����.", 
                                      "��:60h-������������",
                                      "��:61h-������������",
                                      "��:62h-������������",
                                      "��:63h-������������",
                                      "��:65h-�� �������",
                                      "��:66h-������������",
                                      "��:68h-�� �������",
                                      "��:69h-������������",
                                      "��:6Ah-������",
                                      "��:6Bh-��� �������",
                                      "��:6Ch-��� �����.",
                                      "��:6Dh-�� �������",
                                      "��:6Eh-������������",
                                      "��:6Fh-������������",
                                      "��:71h-������", 
                                      "��:72h-������� ��", 
                                      "��:73h-������� ��", 
                                      "��:74h-������ ���", 
                                      "��:75h-������", 
                                      "��:77h-�������:", 
                                      "��:78h-������ ��", 
                                      "��:7Ah-���� ��", 
                                      "��:7Bh-������", 
                                      "��:7Ch-�� ���������", 
                                      "��:7Dh-��������", 
                                      "��:7Eh-��������", 
                                      "��:7Fh-������������", 
                                      "��:84h-������������", 
                                      "��:85h-������������", 
                                      "��:86h-������������", 
                                      "��:87h-������������", 
                                      "��:88h-������������", 
                                      "��:89h-������������", 
                                      "��:8Ah-������������", 
                                      "��:8Bh-������������", 
                                      "��:8Ch-�����.", 
                                      "��:8Dh-�������������", 
                                      "��:8Eh-������� ����", 
                                      "��:90h-���� ����.", 
                                      "��:91h-����� ��", 
                                      "��:92h-���������", 
                                      "��:93h-��������������", 
                                      "��:94h-��������", 
                                      "��:C0h-��������", 
                                      "��:C2h-����������", 
                                      "��:C4h-������������", 
                                      "��:C7h-���� ��", 
                                      "��:�8h-�����������"
};

TDataDescStruct const JournalErrorNumberDesc0 = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  EVENT_RECORDS_COUNT,             // ������ �������
  &EventJournalIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, EventRecords[0].event),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&ErrorNumberRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(TEventRecord),       // �������� ����� ���������� � �������
  NULL,             // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  (void*)ErrorNumberList0,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

CPU_INT08U const *ErrorNumberList1[JOURNAL_EVENTS_COUNT] = {"", "", "", "",
                                        "", "", "", "",
                                        "", "", "", "",
                                        "", "", "", "",
                                        
                                        "",
                                        "",
                                        "",
                                        "",
                                        "",
                                        "",
                                        "",
                                        
                                       "����������������", 
                                       "���������������", 
                                       "��� ������", 
                                       "�� ���.�������",
                                       "��� ���������.",
                                       "�� �������������",
                                       "�� ����������� ",
                                       "�� �����.������� ",
                                       "�� �������",
                                       "�� ������.�������",
                                       "�� �����",
                                       "���������",
                                       "�����������",
                                       "���������������",
                                       "� �������",
                                       "������",
                                       "���������� ������",
                                       "���������.������",
                                       "���������.������",
                                       "������������",
                                       "�����������",
                                       "������",
                                       "���.�������",
                                       "�������",
                                       "������", 
                                       "����� � ��", 
                                       
                                       "�������",  // ��:01h
                                       "��������� ��",                  
                                       "",
                                       "",                                       
                                       "������������ ��",
                                       "����������",
                                       "�/��� �����",
                                       "����������� ������",
                                       "����.���.�-��",
                                       "����.TLV ������",
                                       "������.����.",
                                       "������ ��",
                                       "������ ��������",
                                       "������ ��������",
                                       "24 �����",
                                       "������� �� �������",
                                        "�� ��� �� �������",
                                        "������ � ��",
                                        "�� ��������",
                                        "��������� � �-��",
                                        "������",
                                        "��������",
                                        "���������",    // 36h
                                        "�� ��������������",
                                        "� ���",
                                        "������ �� ���",
                                        "���.�� ����.",
                                        "����.-�������� ������.",
                                        "����.-�������� ������.",       //3Dh
                                        "�����.�� �������",
                                        "�����.�� �������",
                                        "��������� ������",
                                        "��������� ������",
                                        "��������� ������ 2",
                                        "��������� ������ 3",
                                        "��������� ������ 4",
                                        "������ ����� ����",
                                        "���������� � �����",
                                        "���������� �� �������",
                                        "����� ����",
                                        "� ����.����",
                                        "�������� ����������",
                                        "����������",
                                        "�����.�� �������",
                                        "������.����� ������",
                                        "��������� 24 ����",
                                        "",
                                        "����.�������",
                                        "���������� � �����",
                                        "���������� 2 � �����",
                                        "���������� 3 � �����",
                                        "���������� 4 � �����",
                                        "�������� ������.",
                                        "��� �������",
                                        "������� �����������",
                                        "������ ������ ��.",
                                        "��������� ��������",
                                        "���������� 24�",
                                        "�� ����������",
                                        "��������",
                                        "���� ����",
                                        "��� ���������",
                                        "��������� ����",
                                        "��������� ���-��",
                                        "��������� ������",
                                        "����� � ������",
                                        "����� � ������",
                                        "����� �� �������",
                                        "����� �� �������",
                                        "�������",
                                        "�����",
                                        "�����",
                                        "����� �� ������",
                                        "����� �� ������",
                                        "�� ������� � �����",
                                        "���������",
                                        "�������.",
                                        "�������.",
                                        "",
                                        "�������",
                                        "��� �������",
                                        "",
                                        "�������������",
                                        "������������",
                                        "����",
                                        "������ ����",
                                        "�������� � ���� �����",
                                        "��������� ����� ����",
                                        "����������",
                                        "�� ��������",
                                        "�� ��������",
                                        "�� ��������� ������",
                                        "�� ��������� �������",
                                        "�� �������� � �����",
                                        "�� ��������� � ����",
                                        "�� ������� � ����",
                                        "���� ��������",
                                        "���� ������ � ����",
                                        "����",
                                        "������ � ����������",
                                        "������� ���� ������",
                                        "�����",
                                        "��� ������ �������",
                                        "����� �������� � ����",
                                        "���� � �������",
                                        "����������",
                                        "������� ����",
                                        "�������������",
                                        "�������� ����."
};

TDataDescStruct const JournalErrorNumberDesc1 = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  EVENT_RECORDS_COUNT,             // ������ �������
  &EventJournalIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, EventRecords[0].event),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&ErrorNumberRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(TEventRecord),       // �������� ����� ���������� � �������
  NULL,             // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  (void*)ErrorNumberList1,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  ������ � ������� ��-���������
*************************************/
CPU_INT08U const *ErrorNumberListEng[JOURNAL_EVENTS_COUNT] = 
{
  "Net oshibki", "", "", "",
  "", "", "", "",
  "", "", "", "",
  "", "", "", "",
  "Oshibka svyazi c kupuropriemnikom",  
  "Kriticheskaya oshibka kupuropriemnika",
  "Vybros kupury po mag.datchiku",
  "Vybros kupury pri transportirovke",
  "Vybros kupury po identifikacii",
  "Vybros kupury po verifikacii",
  "Vybros kupury po opt.datchiku",
  "Vybros kupury po zapretu",
  "Vybros kupury po emk.datchiku",
  "Vybros kupury po dline",
  "Kasseta zapolnena",
  "Kasseta otsutstvuet",
  "Zamin v kupuropriemnike",
  "Zamin v kassete",
  "Popytka obmana",
  "Oshibka stekernogo motora",
  "Oshibka skorosti transp.motora",
  "Oshibka transp.motora",
  "Oshibka mehanizmavyravnivaniya",
  "Kasseta otsutstvuet",
  "Oshibka optiki",
  "Oshibka magn.datchika",
  "Oshibka emk.datchika",
  "Nekriticheskaya oshibka kupuropriemnika",

  "Oshibka svyazi s modemom",
  "Oshibka svyazi s FR",  
  "Oshibka FR 0x01",
  "Oshibka FR 0x02",
  "Oshibka FR 0x03",
  "Oshibka FR 0x04",
  "Oshibka FR 0x05",
  "Oshibka FR 0x06",
  "Oshibka FR 0x07",
  "Oshibka FR 0x08",
  "Oshibka FR 0x09",
  "Oshibka FR 0x10",
  "Oshibka FR 0x11",
  "Oshibka FR 0x12",
  "Oshibka FR 0x14",
  "Oshibka FR 0x15",
  "Oshibka FR 0x16",
  "Oshibka FR 0x17",
  "Oshibka FR 0x20",
  "Oshibka FR 0x2F",
  "Oshibka FR 0x30",
  "Oshibka FR 0x33",
  "Oshibka FR 0x34",
  "Oshibka FR 0x35",
  "Oshibka FR 0x36",
  "Oshibka FR 0x37",
  "Oshibka FR 0x38",
  "Oshibka FR 0x39",
  "Oshibka FR 0x3A",
  "Oshibka FR 0x3C",
  "Oshibka FR 0x3D",
  "Oshibka FR 0x3E",
  "Oshibka FR 0x3F",
  "Oshibka FR 0x40",
  "Oshibka FR 0x41",
  "Oshibka FR 0x42",
  "Oshibka FR 0x43",
  "Oshibka FR 0x44",
  "Oshibka FR 0x45",
  "Oshibka FR 0x46",
  "Oshibka FR 0x47",
  "Oshibka FR 0x48",
  "Oshibka FR 0x49",
  "Oshibka FR 0x4A",
  "Oshibka FR 0x4B",
  "Oshibka FR 0x4C",
  "Oshibka FR 0x4D",
  "Oshibka FR 0x4E",
  "Oshibka FR 0x4F",
  "Oshibka FR 0x50",
  "Oshibka FR 0x51",
  "Oshibka FR 0x52",
  "Oshibka FR 0x53",
  "Oshibka FR 0x54",
  "Oshibka FR 0x55",
  "Oshibka FR 0x56",
  "Oshibka FR 0x58",
  "Oshibka FR 0x59",
  "Oshibka FR 0x5B",
  "Oshibka FR 0x5C",
  "Oshibka FR 0x5D",
  "Oshibka FR 0x5E",
  "Oshibka FR 0x5F",
  "Oshibka FR 0x60",
  "Oshibka FR 0x61",
  "Oshibka FR 0x62",
  "Oshibka FR 0x63",
  "Oshibka FR 0x65",
  "Oshibka FR 0x66",
  "Oshibka FR 0x68",
  "Oshibka FR 0x69",
  "Oshibka FR 0x6A",
  "Oshibka FR 0x6B",
  "Oshibka FR 0x6C",
  "Oshibka FR 0x6D",
  "Oshibka FR 0x6E",
  "Oshibka FR 0x6F",
  "Oshibka FR 0x71",
  "Oshibka FR 0x72",
  "Oshibka FR 0x73",
  "Oshibka FR 0x74",
  "Oshibka FR 0x75",
  "Oshibka FR 0x77",
  "Oshibka FR 0x78",
  "Oshibka FR 0x7A",
  "Oshibka FR 0x7B",
  "Oshibka FR 0x7C",
  "Oshibka FR 0x7D",
  "Oshibka FR 0x7E",
  "Oshibka FR 0x7F",
  "Oshibka FR 0x84",
  "Oshibka FR 0x85",
  "Oshibka FR 0x86",
  "Oshibka FR 0x87",
  "Oshibka FR 0x88",
  "Oshibka FR 0x89",
  "Oshibka FR 0x8A",
  "Oshibka FR 0x8B",
  "Oshibka FR 0x8C",
  "Oshibka FR 0x8D",
  "Oshibka FR 0x8E",
  "Oshibka FR 0x90",
  "Oshibka FR 0x91",
  "Oshibka FR 0x92",
  "Oshibka FR 0x93",
  "Oshibka FR 0x94",
  "Oshibka FR 0xC0",
  "Oshibka FR 0xC2",
  "Oshibka FR 0xC4",
  "Oshibka FR 0xC7",
  "Oshibka FR 0xC8",
  "",
  ""
};
  
TDataDescStruct const JournalErrorNumberDescEng = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,             // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  EVENT_RECORDS_COUNT,             // ������ �������
  &EventJournalIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, EventRecords[0].event),                // ��������� �� ���������� ��� ����� FRAM
  (void*)&ErrorNumberRange,           // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(TEventRecord),       // �������� ����� ���������� � �������
  NULL,             // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  (void*)ErrorNumberList1,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  ������ ��� ��������� ������� �������
*************************************/
TRangeValueULONG const EventJournalIndexRange = {0, 0xffffffff};
CPU_INT08U const EventJournalIndexName[] = "������ #";
CPU_INT32U EventJournalIndex = 0;

void OnChangeEventJournalIndex(void)
{
  TEventRecord record;

  if (EventJournalIndex == 0xffffffff) EventJournalIndex = EVENT_RECORDS_COUNT - 1;
  else if (EventJournalIndex > EVENT_RECORDS_COUNT - 1) EventJournalIndex = 0;

  GetEventRecord(&record, EventJournalIndex);
  PrintEventJournalRecord(&record);
}

TDataDescStruct const EventJournalIndexDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)&EventJournalIndex,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EventJournalIndexRange,     // ��������� �� ������� ���������
  OnChangeEventJournalIndex,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  EventJournalIndexName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  ����� ������� � ������� �������
*************************************/
TDataDescStruct const JournalEventTimeDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_TIME,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  EVENT_RECORDS_COUNT,             // ������ �������
  &EventJournalIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, EventRecords[0].time),            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(TEventRecord),       // �������� ����� ���������� � �������
  NULL,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ��������� �����
*************************************/
extern CPU_INT32U SystemTime;

TDataDescStruct const SystemTimeDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_TIME,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)&SystemTime,            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  NULL,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};


void OnSetTime(void)
{
  TRTC_Data rtc;
  Sec2Date(&rtc, SystemTime);
  RTC_SetTime(&rtc);
}

TDataDescStruct const SystemTimeEditDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_TIME,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)&SystemTime,            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  OnSetTime,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  NULL,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  ������� �� ������� ��������
*************************************/
CPU_INT32U ClearJournalCmd = 0;

CPU_INT08U const ClearJournalCmdName[] = "�������";
CPU_INT08U const ClearJournalCmdList_str0[] = "���";
CPU_INT08U const ClearJournalCmdList_str1[] = "��";
CPU_INT08U const *ClearJournalCmdList[] = {ClearJournalCmdList_str0, ClearJournalCmdList_str1};

void OnChangeClearJournalCmd(void)
{
  if (ClearJournalCmd)
    {
      ClearEventJournal();
      ClearJournalCmd = 0;
    }
}

TDataDescStruct const ClearJournalCmdDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&ClearJournalCmd,                // ��������� �� ���������� ��� ����� FRAM
  (void*)&InitByDefaultRange,           // ��������� �� ������� ���������
  OnChangeClearJournalCmd,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  ClearJournalCmdName,             // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  ClearJournalCmdList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  ����� ������� ����� ��������
*************************************/
CPU_INT08U const CounterRunName[] = "�������";

TDataDescStruct const CounterRunDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterRun),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterRunName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� �����
*************************************/
CPU_INT08U const CounterMoneyName[] = "����� ���.,���.";

TDataDescStruct const CounterMoneyDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterMoney),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterMoneyName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ������� ������
*************************************/
CPU_INT08U const CounterTimeName[] = "��.���.";

TDataDescStruct const CounterTimeDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_TIME_COUNT,     // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterTime),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterTimeName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  �������� ������
*************************************/
CPU_INT08U const CounterCoinOutName[] = "������";

TDataDescStruct const CounterCoinOutDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterCoinOut),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterCoinOutName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ����� ����� �������� 
*************************************/
CPU_INT08U const CounterCoinName[] = "������,���.";

TDataDescStruct const CounterCoinDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterCoin),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterCoinName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ����� ����� ��������
*************************************/
CPU_INT08U const CounterCashName[] = "������,���.";

TDataDescStruct const CounterCashDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterCash),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterCashName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ������� �����
*************************************/
CPU_INT08U const CounterBillName[] = "����� ���.,���.";

TDataDescStruct const CounterAllCashDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterAllCash),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterBillName,          // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ����� ����� ���������� ��������
*************************************/
CPU_INT08U const CounterBankName[] = "����,���.";

TDataDescStruct const CounterBankDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterBank),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterBankName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ����� �������� �������
*************************************/
TDataDescStruct const CounterLongRunDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CountersLong.CounterRunLong),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterRunName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ����� ������� 
*************************************/
TDataDescStruct const CounterLongMoneyDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CountersLong.CounterMoneyLong),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterMoneyName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  �������� ������ �������
*************************************/
TDataDescStruct const CounterLongCoinOutDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CountersLong.CounterCoinOutLong),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterCoinOutName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ����� ����� �������� ������� 
*************************************/
TDataDescStruct const CounterLongCoinDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CountersLong.CounterCoinLong),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterCoinName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ����� ����� �������� ������� 
*************************************/
TDataDescStruct const CounterLongCashDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CountersLong.CounterCashLong),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterCashName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ������� ����� �������
*************************************/
TDataDescStruct const CounterLongAllCashDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CountersLong.CounterAllCashLong),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterBillName,          // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ����� ����� ���������� �������� ������� 
*************************************/
TDataDescStruct const CounterLongBankDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CountersLong.CounterBankLong),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterBankName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ������� ������� ������
*************************************/
TDataDescStruct const CounterLongTimeDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_TIME_COUNT,     // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CountersLong.CounterTimeLong),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  CounterTimeName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ������� �� ������� ���������� 
*************************************/
CPU_INT32U ClearStatCmd = 0;

CPU_INT08U const ClearStatCmdName[] = "�������";

void OnChangeClearStatCmd(void)
{
  if (ClearStatCmd)
    {
      ClearCounters();
      ClearStatCmd = 0;
    }
}

TDataDescStruct const ClearStatCmdDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&ClearStatCmd,                // ��������� �� ���������� ��� ����� FRAM
  (void*)&InitByDefaultRange,           // ��������� �� ������� ���������
  OnChangeClearStatCmd,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  ClearJournalCmdName,             // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  ClearJournalCmdList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  ���������� ��������������� �������� ����
*************************************/
TRangeValueULONG const EnableFiscalDayClearRange = {0, 1};
CPU_INT08U const EnableFiscalDayClearName[] = "����.�����";
CPU_INT08U const *EnableFiscalDayClearList[] = {"���.", "����", "�����"};

TDataDescStruct const EnableFiscalDayClearDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.EnableFiscalDayClear),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableFiscalDayClearRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableFiscalDayClearName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableFiscalDayClearList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           // �������� �� ���������
};


/*************************************
  ��������� ����
*************************************/
TRangeValueULONG const BillFormatRange = {0, 1};
CPU_INT08U const BillFormatName[] = "���:";
CPU_INT08U const BillFormatList_str0[] = "���-��*����";
CPU_INT08U const BillFormatList_str1[] = "�����";
CPU_INT08U const *BillFormatList[] = {BillFormatList_str0, BillFormatList_str1};

TDataDescStruct const BillFormatDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.BillFormat),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&BillFormatRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  BillFormatName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  BillFormatList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  ������� ���������������
*************************************/
TRangeValueULONG const TaxSystemRange = {0, 5};
CPU_INT08U const TaxSystemName[] = "������� ���.";
CPU_INT08U const TaxSystemList_str0[] = "���";
CPU_INT08U const TaxSystemList_str1[] = "��� �";
CPU_INT08U const TaxSystemList_str2[] = "��� �-�";
CPU_INT08U const TaxSystemList_str3[] = "����";
CPU_INT08U const TaxSystemList_str4[] = "���";
CPU_INT08U const TaxSystemList_str5[] = "���";

CPU_INT08U const *TaxSystemList[] = {TaxSystemList_str0, TaxSystemList_str1, TaxSystemList_str2, TaxSystemList_str3, TaxSystemList_str4, TaxSystemList_str5};

TDataDescStruct const TaxSystemDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, TaxSystem),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&TaxSystemRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  TaxSystemName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  TaxSystemList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  ������ ������
*************************************/
TRangeValueULONG const TaxFormatRange = {1, 6};
CPU_INT08U const TaxFormatName[] = "�����:";
CPU_INT08U const TaxFormatList_str0[] = "����.";
CPU_INT08U const TaxFormatList_str1[] = "  1  ";
CPU_INT08U const TaxFormatList_str2[] = "  2  ";
CPU_INT08U const TaxFormatList_str3[] = "  3  ";
CPU_INT08U const TaxFormatList_str4[] = "  4  ";
CPU_INT08U const TaxFormatList_str5[] = "  5  ";
CPU_INT08U const TaxFormatList_str6[] = "  6  ";
CPU_INT08U const *TaxFormatList[] = {TaxFormatList_str0, TaxFormatList_str1, TaxFormatList_str2, TaxFormatList_str3, TaxFormatList_str4, TaxFormatList_str5, TaxFormatList_str6};

TDataDescStruct const TaxFormatDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, TaxFormat),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&TaxFormatRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  TaxFormatName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  TaxFormatList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  4                           
};

/*************************************
  ������� �������
*************************************/
TRangeValueULONG const SubjSellRange = {0, 2};
CPU_INT08U const SubjSellName[] = "�����.�������";
CPU_INT08U const SubjSellList_str0[] = "�����";
CPU_INT08U const SubjSellList_str1[] = "������";
CPU_INT08U const SubjSellList_str2[] = "������";
CPU_INT08U const *SubjSellList[] = {SubjSellList_str0, SubjSellList_str1, SubjSellList_str2};

TDataDescStruct const SubjSellDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, SubjSell),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&SubjSellRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  SubjSellName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  SubjSellList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  2                           
};

/*************************************
  ��� ������ ������ � �����
*************************************/
TRangeValueULONG const CommandV2Range = {0, 1};
CPU_INT08U const CommandV2Name[] = "��� ������";
CPU_INT08U const CommandV2List_str0[] = "������";
CPU_INT08U const CommandV2List_str1[] = "V2";
CPU_INT08U const *CommandV2List[] = {CommandV2List_str0, CommandV2List_str1};

TDataDescStruct const CommandV2Desc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, CommandV2),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&CommandV2Range,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  CommandV2Name,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  CommandV2List,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  �������� ������
*************************************/
TRangeValueULONG const ServiceNameRange = {0, 2};
CPU_INT08U const ServiceNameName[] = "�����";
CPU_INT08U const ServiceNameList_str0[] = "�����";
CPU_INT08U const ServiceNameList_str1[] = "������ �����";
CPU_INT08U const ServiceNameList_str3[] = "������ ������";
CPU_INT08U const *ServiceNameList[] = {ServiceNameList_str0, ServiceNameList_str1, ServiceNameList_str3};

void OnChangeServiceName(void)
{
    CPU_INT08U flag[4] = {0, 0, 0, 0};

    WriteArrayFram(offsetof(TFramMap, manual_service_flag), 4, (unsigned char*)&flag);
}

TDataDescStruct const ServiceNameDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.ServiceName),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&ServiceNameRange,     // ��������� �� ������� ���������
  OnChangeServiceName,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  ServiceNameName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  ServiceNameList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1                           // �������� �� ���������
};

/*************************************
  ���������� ��� ������������������ ���������� ������� �����
*************************************/
TDataDescStruct const AcceptedMoneyDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, FRAM_AcceptedMoney),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ���������� Crc16 ������������������ ���������� ������� �����
*************************************/
TDataDescStruct const AcceptedMoneyCRC16Desc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, crc_AcceptedMoney),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

TDataDescStruct const AcceptedBankMoneyDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, FRAM_AcceptedBankMoney),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0         
};

/*************************************
  ���������� Crc16 ������������������ ����������
*************************************/
TDataDescStruct const AcceptedBankMoneyCRC16Desc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, crc_AcceptedBankMoney),            // ��������� �� ���������� ��� ����� FRAM
   NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0         
};

TDataDescStruct const AcceptedRestMoneyDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, FRAM_AcceptedRestMoney),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0         
};

/*************************************
  ���������� Crc16 ������������������ ����������
*************************************/
TDataDescStruct const AcceptedRestMoneyCRC16Desc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, crc_AcceptedRestMoney),            // ��������� �� ���������� ��� ����� FRAM
   NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0         
};

/*************************************
  ���������� ��� ������������������ ���������� �������� �������
*************************************/
TDataDescStruct const AcceptedCoinDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, FRAM_AcceptedCoin),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ���������� Crc16 ������������������ ���������� �������� �������
*************************************/
TDataDescStruct const AcceptedCoinCRC16Desc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, crc_AcceptedCoin),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ���������� ������
*************************************/
TRangeValueULONG const PassRange = {0, 9999};
CPU_INT08U const PassName[] = "  �����b";

void OnChangePass(void)
{
  // ������� CRC ������  
  CPU_INT32U pass,crc;
  GetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);      
  crc = crc16((unsigned char*)&pass, sizeof(CPU_INT32U));
  SetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

TDataDescStruct const PassDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Pass),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PassRange,                     // ��������� �� ������� ���������
  OnChangePass,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  (void*)&PassName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  DEFAULT_PASSWORD                           
};

/*************************************
  ���������� CRC ������
*************************************/
TDataDescStruct const PassCRCDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, crc_Pass),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ���������� ���������� ������
*************************************/
CPU_INT32U TempPass = 0;

void OnChangeTempPass(void)
{
  CPU_INT32U pass;
  GetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);
    
  if (GetCurrentMenu() == SetPassMenuPanel)
    { // �������� ������ ������
      if (pass != TempPass)
      {
        SaveEventRecord(0, JOURNAL_EVENT_PASS_FAIL, TempPass);
        GoToMenu(ErrorPassPanel);
      }
      else {GoToPreviousMenu(); GoToMenu(SetNewPassMenuPanel);}
    }
  else if (GetCurrentMenu() == ResetSettingsMenuPanel)
    { // ����� ��������
      if (pass != TempPass) 
      {
        SaveEventRecord(0, JOURNAL_EVENT_PASS_FAIL, TempPass);
        GoToMenu(ErrorPassPanel);
      }
      else {InitByDefault = 1; OnChangeInitByDefault(); GoToPreviousMenu(); GoToMenu(SettingsIsReset);}
    }
  else if (GetCurrentMenu() == ClearStatMenu)
    { // ������� ����������
      if (pass != TempPass)
      {
        SaveEventRecord(0, JOURNAL_EVENT_PASS_FAIL, TempPass);
        GoToMenu(ErrorPassPanel);
      }
      else {ClearStatCmd = 1; OnChangeClearStatCmd(); GoToPreviousMenu(); GoToMenu(StatIsReset);}
    }
  else if (GetCurrentMenu() == ClearJournalMenuPanel)
    { // ������� ����������
      if (pass != TempPass) 
      {
        SaveEventRecord(0, JOURNAL_EVENT_PASS_FAIL, TempPass);
        GoToMenu(ErrorPassPanel);
      }
      else {ClearJournalCmd = 1; OnChangeClearJournalCmd(); GoToPreviousMenu(); GoToMenu(JournalIsReset);}
    }

}

TDataDescStruct const PassTempDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&TempPass,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PassRange,                     // ��������� �� ������� ���������
  OnChangeTempPass,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  (void*)&PassName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};


CPU_INT08U const PassTempName1[] = "    �����b";

TDataDescStruct const PassTempDesc1 = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&TempPass,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PassRange,                     // ��������� �� ������� ���������
  OnChangeTempPass,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  (void*)&PassTempName1,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

TRangeValueULONG const MasterPassRange = {0, 99999999};
CPU_INT08U const MasterPassTempName[] = "�����b";

void OnChangeMasterPass(void)
{
    CPU_INT32U pass, crc;
        
    if (TempPass == MASTER_PASSWORD)
    {
        TempPass = 0;
        pass = DEFAULT_PASSWORD;
        crc = crc16((unsigned char*)&pass, sizeof(CPU_INT32U));
        SetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);
        SetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
        
        GoToPreviousMenu();
        GoToPreviousMenu();
        GoToMenu(SetNewPassMenuPanel);
    }
}

TDataDescStruct const MasterPassTempDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&TempPass,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&MasterPassRange,                     // ��������� �� ������� ���������
  OnChangeMasterPass,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  (void*)&MasterPassTempName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

CPU_INT08U const PassTempName2[] = "   �����b";

TDataDescStruct const PassTempDesc2 = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)&TempPass,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&PassRange,                     // ��������� �� ������� ���������
  OnChangeTempPass,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  (void*)&PassTempName2,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ������� ������� �������� ���������
*************************************/
CPU_INT08U const SendTestEmailName[] = "����.����";
CPU_INT32U send_test;

void OnChangeSendTestEmail(void)
{
    if (send_test)
    {
        PostModemTask(MODEM_TASK_SEND_TEST_MSG);
        send_test = 0;
    }
}

TDataDescStruct const SendTestEmailDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)&send_test,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableEmailErrorSendRange,     // ��������� �� ������� ���������
  OnChangeSendTestEmail,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  SendTestEmailName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  DisableFiscalErrorsList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  ������ �������� ������ ��� ��������� ��������� �� ���������
*************************************/
CPU_INT32U  BillnomViewIndex;
TRangeValueULONG const BillnomIndexRange = {0, 23};
CPU_INT08U const BillnomName[] = "   ������� #";
CPU_INT08U const* BillnomItems[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                                    "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24"};

TDataDescStruct const BillnomIndexDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,             // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  &BillnomViewIndex,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&BillnomIndexRange,       // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  BillnomName,         // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  BillnomItems,             // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  �������� �������� ������ ��� ��������� ���������
*************************************/
extern CPU_INT32U BillNominals[24];
CPU_INT08U const BillnomValName[] = "��������,���.";

TDataDescStruct const BillnomDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  24,                       // ������ �������
  (void*)&BillnomIndexDesc,        // ��������� �� ��������� ������� �������
  (void*)&BillNominals,            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  BillnomValName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  �������� ��������� ����� � ���������
*************************************/
CPU_INT08U const BillnomCountersName[] = "���-��";

TDataDescStruct const BillnomCountersDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_IS_ARRAY,            // ������� �������
  24,                        // ������ �������
  &BillnomIndexDesc,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.CounterBillNominals[0]),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),                        // �������� ����� ���������� � �������
  BillnomCountersName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ����� ���������� ����� � ���������
*************************************/
CPU_INT08U const BillCounterName[] = "����� �����";

TDataDescStruct const BillCounterDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, Counters.BillsCount),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  BillCounterName,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  ID ���������� 
*************************************/
CPU_INT08U const DeviceIDName[] = "ID ���-��";
TRangeValueULONG const DeviceIDRange = {0, 9999};

TDataDescStruct const DeviceIDDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.DeviceId),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&DeviceIDRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  DeviceIDName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ������� �������� ��������� �� ����������
*************************************/
TDataDescStruct const IncasSendFlagDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  1,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, IncasEmailFlag),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ������ �� ��������� ����������
*************************************/
TDataDescStruct const IncasMoneyDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  1,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, IncasMoney),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  IP-����� ����
*************************************/
void OnChangeIpMask()
{
    CPU_INT32U ip, mask;
    GetData(&IpAddrDesc, &ip, 0, DATA_FLAG_SYSTEM_INDEX);  
    GetData(&NetMaskDesc, &mask, 0, DATA_FLAG_SYSTEM_INDEX);  
    #if uC_TCPIP_MODULE > 0
    NetIP_CfgAddrThisHost(ip, mask);
    #endif
}
    
CPU_INT08U const IpAddrName[] = "IP";

TDataDescStruct const IpAddrDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_IP_ADDR,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  1,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, ip),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&DeferredStartRange,     // ��������� �� ������� ���������
  OnChangeIpMask,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  IpAddrName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  DeferredStartList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0xC0A80064
};

/*************************************
  ����� ����
*************************************/
CPU_INT08U const NetMaskName[] = "MSK";

TDataDescStruct const NetMaskDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_IP_ADDR,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  1,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, netmask),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&DeferredStartRange,     // ��������� �� ������� ���������
  OnChangeIpMask,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  NetMaskName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  DeferredStartList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0xFFFFFF00
};

/*************************************
  ����
*************************************/

void OnChangeGateway()
{
    CPU_INT32U gw;
    GetData(&GatewayDesc, &gw, 0, DATA_FLAG_SYSTEM_INDEX);  
    #if uC_TCPIP_MODULE > 0
    NetIP_CfgAddrDfltGateway(gw);
    #endif
}
    
CPU_INT08U const GatewayName[] = "GW";

TDataDescStruct const GatewayDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_IP_ADDR,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  1,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, gateway),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&DeferredStartRange,     // ��������� �� ������� ���������
  OnChangeGateway,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  GatewayName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  DeferredStartList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0xC0A80001
};

/*************************************
  ����� ��������� ����������
*************************************/
TDataDescStruct const IncasTimeDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  1,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, IncasTime),            // ��������� �� ���������� ��� ����� FRAM
  NULL,                     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),                        // �������� ����� ���������� � �������
  NULL,           // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  ��������� ������, ���. �������
*************************************/
CPU_INT08U const HopperCostName[] = "�����,���";
TRangeValueULONG const HopperCostRange = {0, 9999};

TDataDescStruct const HopperCostDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.hopperCost),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&HopperCostRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  HopperCostName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  10                          
};

/*************************************
  ��������� ������ �������, ���
*************************************/
CPU_INT08U const HopperStopEngineName[] = "�������,���";
TRangeValueULONG const HopperStopEngineRange = {1, 20};

TDataDescStruct const HopperStopEngineDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.hopperStopEngine),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&HopperStopEngineRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  HopperStopEngineName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  5                          
};

/*************************************
  ������� ������, ���
*************************************/
CPU_INT08U const HopperSaveCreditName[] = "������,���";
TRangeValueULONG const HopperSaveCreditRange = {0, 60};

TDataDescStruct const HopperSaveCreditDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.hopperSaveCredit),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&HopperSaveCreditRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  HopperSaveCreditName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                          
};

/*************************************
  ������ ����� - ��/���
*************************************/
CPU_INT08U const HopperButtonStartName[] = "������";
TRangeValueULONG const HopperButtonStartRange = {0, 1};

CPU_INT08U const ButtonStartList_str0[] = "���";
CPU_INT08U const ButtonStartList_str1[] = "��";
CPU_INT08U const *ButtonStartList[] = {ButtonStartList_str0, ButtonStartList_str1};

TDataDescStruct const HopperButtonStartDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,                        // ������ �������
  NULL,                     // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.hopperButtonStart),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&HopperButtonStartRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  HopperButtonStartName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  ButtonStartList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                          
};

/*************************************
  ����� �������� ����� ������� � ������ Cube, ��
*************************************/
TRangeValueULONG const HopperPulseLenRange = {20, 2000};
CPU_INT08U const HopperPulseLenName[] = "����� ���.,��";

void OnChangeHopperPulseLen()
{
    CPU_INT32U pulse, pause;
    GetData(&HopperPulseLenDesc, &pulse, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&HopperPauseLenDesc, &pause, 0, DATA_FLAG_SYSTEM_INDEX);
    SetHopperPulseParam(pulse, pause);
}

TDataDescStruct const HopperPulseLenDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, hopper_pulse_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&HopperPulseLenRange,     // ��������� �� ������� ���������
  OnChangeHopperPulseLen,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  HopperPulseLenName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  1000                           
};

/*************************************
  ����� ����� ����� ������� � ������ Cube, ��
*************************************/
TRangeValueULONG const HopperPauseLenRange = {20, 2000};
CPU_INT08U const HopperPauseLenName[] = "����� ���.,��";

TDataDescStruct const HopperPauseLenDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, hopper_pause_len),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&HopperPauseLenRange,     // ��������� �� ������� ���������
  OnChangeHopperPulseLen,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  HopperPauseLenName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  20
};

/*************************************
  ����� ����� ��������� ������ � ������ Cube, ��
*************************************/
TRangeValueULONG const HopperPauseEngineOffRange = {0, 50};
CPU_INT08U const HopperPauseEngineOffName[] = "���.���.,��";

TDataDescStruct const HopperPauseEngineOffDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, hopper_pause_engine_off),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&HopperPauseEngineOffRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  HopperPauseEngineOffName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0
};

/*************************************
  ����� �������
*************************************/
void OnChangeHopperRegime()
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    OS_ENTER_CRITICAL();
    InitInputPorts();
    OS_EXIT_CRITICAL();
}

TRangeValueULONG const RegimeHopperRange = {0, 1};
CPU_INT08U const RegimeHopperName[] = "�����";
CPU_INT08U const RegimeHopper_str0[] = "Elolution";
CPU_INT08U const RegimeHopper_str1[] = "Cube";
CPU_INT08U const *RegimeHopperList[] = {RegimeHopper_str0, RegimeHopper_str1};

TDataDescStruct const RegimeHopperDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.hopperRegime),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&RegimeHopperRange,     // ��������� �� ������� ���������
  OnChangeHopperRegime,          // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  RegimeHopperName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  RegimeHopperList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           // �������� �� ���������
};

/*************************************
  ��������� ������������� ������ �������
*************************************/
TRangeValueULONG const DisableHopperErrorsRange = {0, 1};
CPU_INT08U const DisableHopperErrorsName[] = "�������.��.";
CPU_INT08U const DisableHopperErrorsList_str0[] = "���";
CPU_INT08U const DisableHopperErrorsList_str1[] = "��";
CPU_INT08U const *DisableHopperErrorsList[] = {DisableHopperErrorsList_str0, DisableHopperErrorsList_str1};

TDataDescStruct const DisableHopperErrorsDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ��������� ������� �������
  (void*)offsetof(TFramMap, DeviceConfig.hopperDisableErrors),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&DisableHopperErrorsRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  DisableHopperErrorsName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  DisableHopperErrorsList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

#ifdef CONFIG_FTP_CLIENT_ENABLE

/*************************************
  ��������� FTP
*************************************/
TDataDescStruct const FtpEnableDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpEnable),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableChannelRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  EnableChannelName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableChannelList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};

char const FtpEnableDescId[] = "FtpEnableDesc";

/*************************************
  IP-����� FTP-������� 
*************************************/
CPU_INT08U const FtpServerIpAddrName[] = "FTP";

TDataDescStruct const FtpServerIpAddrDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_IP_ADDR,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpServerIpAddr),            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  sizeof(CPU_INT32U),       // �������� ����� ���������� � �������
  FtpServerIpAddrName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0x5C35600A // "92.53.96.10"
};
char const FtpServerIpAddrDescId[] = "FtpServerIpAddrDesc";

/*************************************
  ����� FTP-������� 
*************************************/
CPU_INT08U const FtpServerLoginName[] = "�����";
CPU_INT08U const FtpServerLoginDefault[] = "xmiker_morozov";

TDataDescStruct const FtpServerLoginDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_CHAR_STRING,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  15,             // ������ �������
  NULL,        // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpLogin),            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  FtpServerLoginName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  (CPU_INT32U)&FtpServerLoginDefault[0]
};
char const FtpServerLoginDescId[] = "FtpServerLoginDesc";

/*************************************
  ������ FTP-������� 
*************************************/
CPU_INT08U const FtpServerPassName[] = "������";
CPU_INT08U const FtpServerPassDefault[] = "Qwerty11";

TDataDescStruct const FtpServerPassDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_CHAR_STRING,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  15,             // ������ �������
  NULL,        // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpPass),            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  FtpServerPassName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  (CPU_INT32U)&FtpServerPassDefault[0]
};
char const FtpServerPassDescId[] = "FtpServerPassDesc";

/*************************************
  FTP ������������� ����������
*************************************/
CPU_INT08U const FtpDeviceNumberName[] = "ID ����.";
TRangeValueULONG const FtpDeviceNumberRange = {0, 99999999};

TDataDescStruct const FtpDeviceNumberDesc = {
  DATA_DESC_EDIT,          // ��� �����������
  DATA_TYPE_ULONG,         // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,           // ������� �������
  0,                       // ������ �������
  NULL,                       // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpDeviceNumber),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&FtpDeviceNumberRange,                    // ��������� �� ������� ���������
  NULL,                    // ������� �� ���������
  sizeof(CPU_INT32U),                       // �������� ����� ���������� � �������
  FtpDeviceNumberName,        // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,           // ������� ���������� ��������� (������ �����)
  NULL,        // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};
char const FtpDeviceNumberDescId[] = "FtpDeviceNumberDesc";

/*************************************
����� �������� ���������� �� FTP, ��� : ���
*************************************/
CPU_INT08U const FtpSendHourMinName[] = "��.����.";

TDataDescStruct const FtpSendHourMinDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_HOUR_MIN,    // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  0,        // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpSendHourMin),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&StatSendHourRange,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  FtpSendHourMinName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  60 * 1
};
char const FtpSendHourMinDescId[] = "FtpSendHourMinDesc";

/*************************************
  �������� �������� �� FTP
*************************************/
CPU_INT08U const FtpSendIntervalName[] = "��������";
TRangeValueULONG const FtpSendIntervalRange = {0, 7};
CPU_INT08U const FtpSendInterval_str0[] = "1�";
CPU_INT08U const FtpSendInterval_str1[] = "2�";
CPU_INT08U const FtpSendInterval_str2[] = "3�";
CPU_INT08U const FtpSendInterval_str3[] = "4�";
CPU_INT08U const FtpSendInterval_str4[] = "6�";
CPU_INT08U const FtpSendInterval_str5[] = "8�";
CPU_INT08U const FtpSendInterval_str6[] = "12�";
CPU_INT08U const FtpSendInterval_str7[] = "24�";
CPU_INT08U const *FtpSendIntervalList[] = {FtpSendInterval_str0, FtpSendInterval_str1, FtpSendInterval_str2, FtpSendInterval_str3,
                                           FtpSendInterval_str4, FtpSendInterval_str5, FtpSendInterval_str6, FtpSendInterval_str7};

TDataDescStruct const FtpSendIntervalDesc = {
  DATA_DESC_EDIT,          // ��� �����������
  DATA_TYPE_ULONG,         // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,           // ������� �������
  0,                       // ������ �������
  NULL,                       // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpSendIntervalIndex),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&FtpSendIntervalRange,                    // ��������� �� ������� ���������
  NULL,                    // ������� �� ���������
  0,                       // �������� ����� ���������� � �������
  FtpSendIntervalName,        // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,           // ������� ���������� ��������� (������ �����)
  FtpSendIntervalList,        // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  7                           
};
char const FtpSendIntervalDescId[] = "FtpSendIntervalDesc";

/*************************************
  ��������� ����� �������� �� ftp
*************************************/
CPU_INT08U const FtpLastSendTimeName[] = "����.��.";

TDataDescStruct const FtpLastSendTimeDesc = {
  DATA_DESC_VIEW,           // ��� �����������
  DATA_TYPE_TIME,          // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpLastTime),            // ��������� �� ���������� ��� ����� FRAM
  NULL,     // ��������� �� ������� ���������
  NULL,                     // ������� �� ���������
  0,       // �������� ����� ���������� � �������
  FtpLastSendTimeName,       // ��������� �� ������ �������� ���������
  DATA_NO_INDEX,            // ������� ���������� ��������� (������ �����)
  NULL,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  0                           
};
char const FtpLastSendTimeDescId[] = "FtpLastSendTimeDesc";

/*************************************
  ��������� ��������� �������� �� FTP
*************************************/
CPU_INT08U const FtpLastSendResultName[] = "����.������";
TRangeValueULONG const FtpLastSendResultRange = {0, 1};
CPU_INT08U const FtpLastSendResult_str0[] = "������";
CPU_INT08U const FtpLastSendResult_str1[] = "��";
CPU_INT08U const FtpLastSendResult_str2[] = "����������";
CPU_INT08U const *FtpLastSendResultList[] = {FtpLastSendResult_str0, FtpLastSendResult_str1, FtpLastSendResult_str2};

TDataDescStruct const FtpLastSendResultDesc = {
  DATA_DESC_VIEW,          // ��� �����������
  DATA_TYPE_ULONG,         // ��� ���������
  DATA_LOC_FRAM,            // ������������ ���������
  DATA_NO_ARRAY,           // ������� �������
  0,                       // ������ �������
  NULL,                       // ��������� �� ���������� ������� �������
  (void*)offsetof(TFramMap, FtpLastResult),            // ��������� �� ���������� ��� ����� FRAM
  (void*)&FtpLastSendResultRange,                    // ��������� �� ������� ���������
  NULL,                    // ������� �� ���������
  0,                       // �������� ����� ���������� � �������
  FtpLastSendResultName,        // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,           // ������� ���������� ��������� (������ �����)
  FtpLastSendResultList,        // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_DISABLE,
  2
};
char const FtpLastSendResultDescId[] = "FtpLastSendResultDesc";

/*************************************
  ������� ��������� ������ �� ftp ������ 
*************************************/
CPU_INT08U const FtpSendNowCmdName[] = "���������";

CPU_INT32U ftp_send_cmd;
extern CPU_INT08U time_to_ftp;

void OnChangeFtpSendNowCmd(void)
{
    if (ftp_send_cmd != 0)
    {
        time_to_ftp = FTP_FLAG_SEND_COUNTERS | FTP_FLAG_SEND_LOGS;
    }
}

TDataDescStruct const FtpSendNowCmdDesc = {
  DATA_DESC_EDIT,           // ��� �����������
  DATA_TYPE_ULONG,          // ��� ���������
  DATA_LOC_RAM,            // ������������ ���������
  DATA_NO_ARRAY,            // ������� �������
  0,             // ������ �������
  NULL,        // ��������� �� ���������� ������� �������
  (void*)&ftp_send_cmd,            // ��������� �� ���������� ��� ����� FRAM
  (void*)&EnableChannelRange,     // ��������� �� ������� ���������
  OnChangeFtpSendNowCmd,                     // ������� �� ���������
  0,                        // �������� ����� ���������� � �������
  FtpSendNowCmdName,       // ��������� �� ������ �������� ���������
  DATA_IS_INDEX,            // ������� ���������� ��������� (������ �����)
  EnableChannelList,                     // ��������� �� ������ ����� ��� ���������� ���������
  DATA_INIT_ENABLE,
  0
};
char const FtpSendNowCmdDescId[] = "FtpSendNowCmdDesc";

#endif // #ifdef CONFIG_FTP_CLIENT_ENABLE

//**************************************************
//**************************************************
//**************************************************
const TDataDescArrayStruct AllDataArray[] = 
{
    {&WorkTimeDesc, "WorkTimeDesc"},
    {&ChannelIndexDesc,  "ChannelIndexDesc"},
    {&EnableChannelDesc,  "EnableChannelDesc"},
    {&EnableValidatorDesc,  "EnableValidatorDesc"},
    {&EnableModemDesc,  "EnableModemDesc"},
    {&EnableFiscalDesc,  "EnableFiscalDesc"},
    {&EnableCoinDesc, "EnableCoinDesc"},
    {&TimeOutBeforeDesc, "TimeOutBeforeDesc"},
    {&TimeOutAfterDesc, "TimeOutAfterDesc"},
    {&MaxWorkTimeDesc, "MaxWorkTimeDesc"},
    {&MinWorkTimeDesc, "MinWorkTimeDesc"},
    {&WeekEndDesc, "WeekEndDesc"},
    {&PeriodWeekendIndexDesc, "PeriodWeekendIndexDesc"},
    {&PeriodWeekdaysIndexDesc, "PeriodWeekdaysIndexDesc"},
    
    {&PriceWeekendDesc, "PriceWeekendDesc"},
    {&PriceWeekdaysDesc, "PriceWeekdaysDesc"},
    
    {&PriceTimeWeekendDesc, "PriceTimeWeekendDesc"},
    {&PriceTimeWeekdaysDesc, "PriceTimeWeekdaysDesc"},
    {&T_Start_WeekdaysDesc, "T_Start_WeekdaysDesc"},
    {&T_End_WeekdaysDesc, "T_End_WeekdaysDesc"},
    {&T_Start_WeekendDesc, "T_Start_WeekendDesc"},
    {&T_End_WeekendDesc, "T_End_WeekendDesc"},
    
    {&PrintZReportDesc, "PrintZReportDesc"},
    {&PrintXReportDesc, "PrintXReportDesc"},
    {&SystemTimeDesc, "SystemTimeDesc"},
    {&SystemTimeEditDesc, "SystemTimeEditDesc"},
    {&CoinPerPulseDesc, "CoinPerPulseDesc"},
    
    {&BillFormatDesc, "BillFormatDesc"},
    {&NameChannelDesc, "NameChannelDesc"},
    
    {&PassDesc, "PassDesc"},
    {&DeviceIDDesc, "DeviceIDDesc"},
    
    {&EnableEmailErrorSendDesc, "EnableEmailErrorSendDesc"},
    {&EnableEmailJournalSendDesc, "EnableEmailJournalSendDesc"},
    {&ClearJournalAfterSendDesc, "ClearJournalAfterSendDesc"},
    {&StatSendHourMinDesc, "StatSendHourMinDesc"},
    {&SendTestEmailDesc, "SendTestEmailDesc"},
    {&BillnomIndexDesc, "BillnomIndexDesc"},
    
    {&DeferredStartDesc, "DeferredStartDesc"},
    {&StartButtonNameDesc, "StartButtonNameDesc"},
    
    {&GatewayDesc, "GatewayDesc"},
    {&NetMaskDesc, "NetMaskDesc"},
    {&IpAddrDesc, "IpAddrDesc"},
    
    {&SelectProtectDesc, "SelectProtectDesc"},
    {&PostIpAddrDesc, "PostIpAddrDesc"},
    
    {&PostImpCostDesc, "PostImpCostDesc"},
    {&PostLenCostDesc, "PostLenCostDesc"},
    {&PostMinutePriceDesc, "PostMinutePriceDesc"},

    {&CashModeDesc, "CashModeDesc"},
    {&CashPerPulseDesc, "CashPerPulseDesc"},

    {&EnableFiscalDayClearDesc, "EnableFiscalDayClearDesc"},
    {&PrintTimeoutDesc, "PrintTimeoutDesc"},

    {&ServiceNameDesc, "ServiceNameDesc"},

    {&CashPulseLenDesc, "CashPulseLenDesc"},
    {&CashPauseLenDesc, "CashPauseLenDesc"},

    {&EnableBankDesc, "EnableBankDesc"},
    {&BankPerPulseDesc, "BankPerPulseDesc"},
    {&BankPulseLenDesc, "BankPulseLenDesc"},
    {&BankPauseLenDesc, "BankPauseLenDesc"},
    {&BankLevelDesc, "BankLevelDesc"},
    {&CashLevelDesc, "CashLevelDesc"},
    {&CoinLevelDesc, "CoinLevelDesc"},
    
    {&CoinPulseLenDesc, "CoinPulseLenDesc"},
    {&CoinPauseLenDesc, "CoinPauseLenDesc"},
 
    {&HopperCostDesc, "HopperCostDesc"},
    {&HopperStopEngineDesc, "HopperStopEngineDesc"},
    {&HopperSaveCreditDesc, "HopperSaveCreditDesc"},
    {&HopperButtonStartDesc, "HopperButtonStartDesc"},
    {&HopperPauseLenDesc, "HopperPauseLenDesc"},
    {&HopperPauseEngineOffDesc, "HopperPauseEngineOffDesc"},
    {&HopperPulseLenDesc, "HopperPulseLenDesc"},
    {&RegimeHopperDesc, "RegimeHopperDesc"},
    
    {&PrintModeDesc, "PrintModeDesc"},
    {&PrintTimeoutAfterDesc, "PrintTimeoutAfterDesc"},

#ifdef CONFIG_FTP_CLIENT_ENABLE
    {&FtpEnableDesc,  "FtpEnableDesc"},
    {&FtpServerIpAddrDesc,  "FtpServerIpAddrDesc"},
    {&FtpSendHourMinDesc,  "FtpSendHourMinDesc"},
    {&FtpSendIntervalDesc,  "FtpSendIntervalDesc"},
    {&FtpLastSendTimeDesc,  "FtpLastSendTimeDesc"},
    {&FtpLastSendResultDesc,  "FtpLastSendResultDesc"},
    {&FtpSendNowCmdDesc,  "FtpSendNowCmdDesc"},
    {&FtpDeviceNumberDesc,  "FtpDeviceNumberDesc"},
    {&FtpServerLoginDesc,  "FtpServerLoginDesc"},
    {&FtpServerPassDesc,  "FtpServerPassDesc"},
#endif

    {&TaxSystemDesc,"TaxSystemDesc"},
    {&TaxFormatDesc,"TaxFormatDesc"},
    {&SubjSellDesc, "SubjSellDesc"},
    {&DisableFiscalErrorsDesc, "DisableFiscalErrorsDesc"},
    {&CommandV2Desc, "CommandV2Desc"},
    
    {&HopperLevelDesc, "HopperLevelDesc"},
    {&DisableHopperErrorsDesc, "DisableHopperErrorsDesc"},
    
    {NULL, ""}
};


