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
  Индекс канала
*************************************/
CPU_INT32U  ChannelIndex=0;
TRangeValueULONG const ChannelIndexRange = {0, CHANNELS_NUM-1};
CPU_INT08U const ChannelIndexName[] = "    ПОСТ";
CPU_INT08U const* ChannelItems[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

TDataDescStruct const ChannelIndexDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  &ChannelIndex,            // указатель на переменную или адрес FRAM
  (void*)&ChannelIndexRange,       // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  ChannelIndexName,         // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  ChannelItems,             // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Индекс канала в меню канальной статистии
*************************************/
CPU_INT08U const ChannelStIndexName[] = "КОР.СЧ.ПОСТ";

TDataDescStruct const ChannelStIndexDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  &ChannelIndex,            // указатель на переменную или адрес FRAM
  (void*)&ChannelIndexRange,       // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  ChannelStIndexName,         // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  ChannelItems,             // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Индекс канала в меню канальной статистии
*************************************/
CPU_INT08U const ChannelStLongIndexName[] = "ДЛ.СЧ.КАНАЛ";

TDataDescStruct const ChannelStLongIndexDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  &ChannelIndex,            // указатель на переменную или адрес FRAM
  (void*)&ChannelIndexRange,       // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  ChannelStLongIndexName,         // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  ChannelItems,             // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Последнее время отправки email
*************************************/
extern TRangeValueULONG const WorkTimeRange;

TDataDescStruct const LastEmailSendTime = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, LastEmailTime),            // указатель на переменную или адрес FRAM
  (void*)&WorkTimeRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  NULL,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Включение канала
*************************************/
TRangeValueULONG const EnableChannelRange = {0, 1};
CPU_INT08U const EnableChannelName[] = "Включить";
CPU_INT08U const EnableChannelList_str0[] = "нет";
CPU_INT08U const EnableChannelList_str1[] = "да";
CPU_INT08U const *EnableChannelList[] = {EnableChannelList_str0, EnableChannelList_str1};


TDataDescStruct const EnableChannelDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.Enable),            // указатель на переменную или адрес FRAM
  (void*)&EnableChannelRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableChannelName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableChannelList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Длина импульса входа монетоприемника, мс
*************************************/
TRangeValueULONG const CoinPulseLenRange = {20, 250};
CPU_INT08U const CoinPulseLenName[] = "Длина имп.,мс";

void OnChangeCoinPulseLen()
{
    CPU_INT32U pulse, pause;
    
    GetData(&CoinPulseLenDesc, &pulse, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&CoinPauseLenDesc, &pause, 0, DATA_FLAG_SYSTEM_INDEX);
    SetCoinPulseParam(pulse, pause);
}

TDataDescStruct const CoinPulseLenDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, coin_pulse_len),            // указатель на переменную или адрес FRAM
  (void*)&CoinPulseLenRange,     // указатель на границы параметра
  OnChangeCoinPulseLen,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  CoinPulseLenName,         // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  50                           
};

/*************************************
  Длина паузы входа монетника, мс
*************************************/
TRangeValueULONG const CoinPauseLenRange = {20, 250};
CPU_INT08U const CoinPauseLenName[] = "Пауза имп.,мс";

TDataDescStruct const CoinPauseLenDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, coin_pause_len),            // указатель на переменную или адрес FRAM
  (void*)&CoinPauseLenRange,     // указатель на границы параметра
  OnChangeCoinPulseLen,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  CoinPauseLenName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  20                           
};

/*************************************
  Длина импульса входа купюрника, мс
*************************************/
TRangeValueULONG const CashPulseLenRange = {20, 250};
CPU_INT08U const CashPulseLenName[] = "Длина имп.,мс";

void OnChangeCashPulseLen()
{
    CPU_INT32U pulse, pause;
    GetData(&CashPulseLenDesc, &pulse, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&CashPauseLenDesc, &pause, 0, DATA_FLAG_SYSTEM_INDEX);
    SetCashPulseParam(pulse, pause);
}


TDataDescStruct const CashPulseLenDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, cash_pulse_len),            // указатель на переменную или адрес FRAM
  (void*)&CashPulseLenRange,     // указатель на границы параметра
  OnChangeCashPulseLen,                     // функция по изменению
  0,       // смещение между элементами в массиве
  CashPulseLenName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  50                           
};

/*************************************
  Длина паузы входа купюрника, мс
*************************************/
TRangeValueULONG const CashPauseLenRange = {20, 250};
CPU_INT08U const CashPauseLenName[] = "Пауза имп.,мс";

TDataDescStruct const CashPauseLenDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, cash_pause_len),            // указатель на переменную или адрес FRAM
  (void*)&CashPauseLenRange,     // указатель на границы параметра
  OnChangeCashPulseLen,                     // функция по изменению
  0,       // смещение между элементами в массиве
  CashPauseLenName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  20                           
};

/*************************************
  Длина импульса входа банковского терминала, мс
*************************************/
TRangeValueULONG const BankPulseLenRange = {20, 250};
CPU_INT08U const BankPulseLenName[] = "Длина имп.,мс";

void OnChangeBankPulseLen()
{
    CPU_INT32U pulse, pause;

    GetData(&BankPulseLenDesc, &pulse, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&BankPauseLenDesc, &pause, 0, DATA_FLAG_SYSTEM_INDEX);
    SetBankPulseParam(pulse, pause);
}

TDataDescStruct const BankPulseLenDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, bank_pulse_len),            // указатель на переменную или адрес FRAM
  (void*)&BankPulseLenRange,     // указатель на границы параметра
  OnChangeBankPulseLen,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  BankPulseLenName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  50                           
};

/*************************************
  Длина паузы входа банковского терминала, мс
*************************************/
TRangeValueULONG const BankPauseLenRange = {20, 250};
CPU_INT08U const BankPauseLenName[] = "Пауза имп.,мс";

TDataDescStruct const BankPauseLenDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, bank_pause_len),            // указатель на переменную или адрес FRAM
  (void*)&BankPauseLenRange,     // указатель на границы параметра
  OnChangeBankPulseLen,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  BankPauseLenName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  20                           
};

/*************************************
  Цена импульса банковского терминала
*************************************/
TRangeValueULONG const BankPerPulseRange = {1, 9999};
CPU_INT08U const BankPerPulseName[] = "Руб./имп.";

TDataDescStruct const BankPerPulseDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.BankPerPulse),            // указатель на переменную или адрес FRAM
  (void*)&BankPerPulseRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  BankPerPulseName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  10                           // значение по умолчанию
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
  Уровень сигнала монетника
*************************************/

TRangeValueULONG const LevelRange = {0, 1};
CPU_INT08U const LevelName[] = "Уровень";
CPU_INT08U const Level_str0[] = "LOW";
CPU_INT08U const Level_str1[] = "HIGH";
CPU_INT08U const *LevelList[] = {Level_str0, Level_str1};

TDataDescStruct const CoinLevelDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.CoinLevel),            // указатель на переменную или адрес FRAM
  (void*)&LevelRange,     // указатель на границы параметра
  OnChangeLevel,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  LevelName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  LevelList,                  // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                          // значение по умолчанию
};

/*************************************
  Уровень сигнала купюрника
*************************************/
TDataDescStruct const CashLevelDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.CashLevel),            // указатель на переменную или адрес FRAM
  (void*)&LevelRange,     // указатель на границы параметра
  OnChangeLevel,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  LevelName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  LevelList,                  // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                          // значение по умолчанию
};

/*************************************
  Уровень сигнала банковского терминала
*************************************/
TDataDescStruct const BankLevelDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.BankLevel),            // указатель на переменную или адрес FRAM
  (void*)&LevelRange,     // указатель на границы параметра
  OnChangeLevel,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  LevelName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  LevelList,                  // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                          // значение по умолчанию
};

/*************************************
  Уровень сигнала хоппера в режиме Cube
*************************************/
TDataDescStruct const HopperLevelDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.HopperLevel),            // указатель на переменную или адрес FRAM
  (void*)&LevelRange,     // указатель на границы параметра
  OnChangeLevel,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  LevelName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  LevelList,                  // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                          // значение по умолчанию
};

/*************************************
  IP-адрес поста
*************************************/
CPU_INT08U const PostIpAddrName[] = "IP";

TDataDescStruct const PostIpAddrDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_IP_ADDR,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.post_ip),            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PostIpAddrName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0xC0A80065
};

/*************************************
  Использовать запрет выбора поста
*************************************/
TRangeValueULONG const SelectProtectNameRange = {0, 1};
CPU_INT08U const SelectProtectNameName[] = "Запрет выбора";
CPU_INT08U const SelectProtectNameList_str0[] = "нет";
CPU_INT08U const SelectProtectNameList_str1[] = "да";
CPU_INT08U const *SelectProtectNameList[] = {SelectProtectNameList_str0, SelectProtectNameList_str1};


TDataDescStruct const SelectProtectDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,                        // размер массива
  &ChannelIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.select_protect),            // указатель на переменную или адрес FRAM
  (void*)&SelectProtectNameRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),                        // смещение между элементами в массиве
  SelectProtectNameName,      // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  SelectProtectNameList,      // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  Цена импульса контроллера ПОСТА
*************************************/
TRangeValueULONG const PostImpCostRange = {1, 9999};
CPU_INT08U const PostImpCostName[] = "Руб./имп.";

TDataDescStruct const PostImpCostDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.imp_cost),            // указатель на переменную или адрес FRAM
  (void*)&PostImpCostRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PostImpCostName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  10                           // значение по умолчанию
};

/*************************************
  Длина импульса контроллера ПОСТА
*************************************/
TRangeValueULONG const PostImpLenRange = {1, 999};
CPU_INT08U const PostImpLenName[] = "Дл.имп.,мс";

TDataDescStruct const PostLenCostDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.imp_len),            // указатель на переменную или адрес FRAM
  (void*)&PostImpLenRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PostImpLenName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  50                           // значение по умолчанию
};

/*************************************
  Цена минуты
*************************************/
TRangeValueULONG const PostMinutePriceRange = {1, 999};
CPU_INT08U const PostMinutePriceName[] = "Стоим.мин.";

TDataDescStruct const PostMinutePriceDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.minute_cost),            // указатель на переменную или адрес FRAM
  (void*)&PostMinutePriceRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PostMinutePriceName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  20                           // значение по умолчанию
};

/*************************************
  Название канала
*************************************/
TRangeValueULONG const NameChannelRange = {0, 2};
CPU_INT08U const NameChannelName[] = "Название";
CPU_INT08U const NameChannelList_str0[] = "#";
CPU_INT08U const NameChannelList_str1[] = "Солярий";
CPU_INT08U const NameChannelList_str2[] = "Устройство";
CPU_INT08U const *NameChannelList[] = {NameChannelList_str0, NameChannelList_str1, NameChannelList_str2};


TDataDescStruct const NameChannelDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.Name),            // указатель на переменную или адрес FRAM
  (void*)&NameChannelRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  NameChannelName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  NameChannelList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Название кнопки СТАРТ (СТАРТ или ПУСК)
*************************************/
TRangeValueULONG const StartButtonNameRange = {0, 1};
CPU_INT08U const StartButtonNameName[] = "Кнопка";
CPU_INT08U const StartButtonNameList_str0[] = "СТАРТ";
CPU_INT08U const StartButtonNameList_str1[] = "ПУСК";
CPU_INT08U const *StartButtonNameList[] = {StartButtonNameList_str0, StartButtonNameList_str1};


TDataDescStruct const StartButtonNameDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, StartButtonName),            // указатель на переменную или адрес FRAM
  (void*)&StartButtonNameRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  StartButtonNameName,      // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  StartButtonNameList,      // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  Включение купюроприемника
*************************************/
TRangeValueULONG const EnableValidatorRange = {0, 1};
CPU_INT08U const EnableValidatorName[] = "Купюропр-к";
CPU_INT08U const OnOffList_str0[] = "выкл.";
CPU_INT08U const OnOffList_str1[] = "вкл.";
CPU_INT08U const *EnableValidatorList[] = {OnOffList_str0, OnOffList_str1};


TDataDescStruct const EnableValidatorDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.EnableValidator),            // указатель на переменную или адрес FRAM
  (void*)&EnableValidatorRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableValidatorName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableValidatorList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           // значение по умолчанию
};

/*************************************
  Включение банковского терминала
*************************************/
TRangeValueULONG const EnableBankRange = {0, 1};
CPU_INT08U const EnableBankName[] = "Банк.термин.";

TDataDescStruct const EnableBankDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.EnableBank),            // указатель на переменную или адрес FRAM
  (void*)&EnableBankRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableBankName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableValidatorList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           // значение по умолчанию
};

/*************************************
  Включение модема
*************************************/
TRangeValueULONG const EnableModemRange = {0, 1};
CPU_INT08U const EnableModemName[] = "Модем";
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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.EnableModem),            // указатель на переменную или адрес FRAM
  (void*)&EnableModemRange,     // указатель на границы параметра
  OnchangeEnableModem,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableModemName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableModemList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Включить оповещение об ошибках по e-mail
*************************************/
TRangeValueULONG const EnableEmailErrorSendRange = {0, 1};
CPU_INT08U const EnableEmailErrorSendName[] = "Опов.об ош.";
CPU_INT08U const *EnableEmailErrorSendList[] = {OnOffList_str0, OnOffList_str1};

TDataDescStruct const EnableEmailErrorSendDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.EnableEmailErrorSend),            // указатель на переменную или адрес FRAM
  (void*)&EnableEmailErrorSendRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableEmailErrorSendName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableEmailErrorSendList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Статус подключения модема
*************************************/
CPU_INT08U const ModemStatusName[] = "Статус";
CPU_INT08U const *ModemStatusList[] = {"Ок", "Ош.конф.", "Нет связи"};

TDataDescStruct const ModemStatusDesc = {
  DATA_DESC_VIEW,          // тип дескриптора
  DATA_TYPE_ULONG,         // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,           // признак массива
  0,                       // размер массива
  0,                       // указатель на десриптор индекса массива
  &modem_status,           // указатель на переменную или адрес FRAM
  NULL,                    // указатель на границы параметра
  NULL,                    // функция по изменению
  0,      // смещение между элементами в массиве
  ModemStatusName, // указатель на строку названия параметра
  DATA_IS_INDEX,           // признак индексного параметра (список строк)
  ModemStatusList,// указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Включить отправку журналов по e-mail
*************************************/
CPU_INT08U const EnableEmailJournalSendName[] = "Отпр.журналы";

TDataDescStruct const EnableEmailJournalSendDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.EnableEmailJournalSend),            // указатель на переменную или адрес FRAM
  (void*)&EnableEmailErrorSendRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableEmailJournalSendName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableEmailErrorSendList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Очищать журнал после отправки по e-mail НЕ ИСПОЛЬЗУЕТСЯ
*************************************/
CPU_INT08U const ClearJournalAfterSendName[] = "Оч.журналы";

TDataDescStruct const ClearJournalAfterSendDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.ClearJournalAfterSend),            // указатель на переменную или адрес FRAM
  (void*)&EnableEmailErrorSendRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  ClearJournalAfterSendName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableEmailErrorSendList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
Время отправки статистики, час : мин
*************************************/
TRangeValueULONG const StatSendHourRange = {0, 60*24 - 1};
CPU_INT08U const StatSendHourName[] = "Tотпр.";

TDataDescStruct const StatSendHourMinDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_HOUR_MIN,    // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.StatSendHourMin),            // указатель на переменную или адрес FRAM
  (void*)&StatSendHourRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  StatSendHourName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  9                           
};

/*************************************
  Включение монетоприемника
*************************************/
TRangeValueULONG const EnableCoinRange = {0, 1};
CPU_INT08U const EnableCoinName[] = "Монетопр-к";
CPU_INT08U const *EnableCoinList[] = {OnOffList_str0, OnOffList_str1};

void OnchangeEnableCoin(void)
{
}

TDataDescStruct const EnableCoinDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.EnableCoinAcceptor),            // указатель на переменную или адрес FRAM
  (void*)&EnableCoinRange,     // указатель на границы параметра
  OnchangeEnableCoin,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableCoinName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableCoinList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  Цена импульса монетоприемника
*************************************/
TRangeValueULONG const CoinPerPulseRange = {1, 9999};
CPU_INT08U const CoinPerPulseName[] = "Руб./имп.";

TDataDescStruct const CoinPerPulseDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.CoinPerPulse),            // указатель на переменную или адрес FRAM
  (void*)&CoinPerPulseRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  CoinPerPulseName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  10                        // значение по умолчанию
};


/*************************************
  Режим купуюрника
*************************************/
TRangeValueULONG const CashModeRange = {0, 1};
CPU_INT08U const CashModeName[] = "Режим";
CPU_INT08U const CashMode_str0[] = "CCNet";
CPU_INT08U const CashMode_str1[] = "Pulse";
CPU_INT08U const *CashModeList[] = {CashMode_str0, CashMode_str1};

TDataDescStruct const CashModeDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.CashMode),            // указатель на переменную или адрес FRAM
  (void*)&CashModeRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  CashModeName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  CashModeList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Режим печати чека
*************************************/
TRangeValueULONG const PrintModeRange = {0, 1};
CPU_INT08U const PrintModeName[] = "Реж.печати";
CPU_INT08U const PrintMode_str0[] = "таймаут";
CPU_INT08U const PrintMode_str1[] = "кнопка";
CPU_INT08U const *PrintModeList[] = {PrintMode_str0, PrintMode_str1};

TDataDescStruct const PrintModeDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.PrintMode),            // указатель на переменную или адрес FRAM
  (void*)&PrintModeRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PrintModeName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  PrintModeList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  Таймаут печати чека
*************************************/
TRangeValueULONG const PrintTimeoutRange = {0, 3600};
CPU_INT08U const PrintTimeoutName[] = "Таймаут печати";

TDataDescStruct const PrintTimeoutDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.PrintTimeout),            // указатель на переменную или адрес FRAM
  (void*)&PrintTimeoutRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  PrintTimeoutName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           // значение по умолчанию
};

/*************************************
  Таймаут отмены печати чека
*************************************/
TRangeValueULONG const PrintTimeoutAfterRange = {0, 60};
CPU_INT08U const PrintTimeoutAfterName[] = "Таймаут обнул.";

TDataDescStruct const PrintTimeoutAfterDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.PrintTimeoutAfter),            // указатель на переменную или адрес FRAM
  (void*)&PrintTimeoutAfterRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  PrintTimeoutAfterName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  10                           // значение по умолчанию
};

/*************************************
  Цена импульса купюрника в импульсном режиме
*************************************/
TRangeValueULONG const CashPerPulseRange = {1, 9999};
CPU_INT08U const CashPerPulseName[] = "Руб./имп.";

TDataDescStruct const CashPerPulseDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.CashPerPulse),            // указатель на переменную или адрес FRAM
  (void*)&CashPerPulseRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  CashPerPulseName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  10                           // значение по умолчанию
};


/*************************************
  Включение ФР
*************************************/
TRangeValueULONG const EnableFiscalRange = {0, 1};
CPU_INT08U const EnableFiscalName[] = "ФР";
CPU_INT08U const *EnableFiscalList[] = {OnOffList_str0, OnOffList_str1};

TDataDescStruct const EnableFiscalDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.EnableFiscal),            // указатель на переменную или адрес FRAM
  (void*)&EnableFiscalRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableFiscalName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableFiscalList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  Включение ФР
*************************************/
TRangeValueULONG const DisableFiscalErrorsRange = {0, 1};
CPU_INT08U const DisableFiscalErrorsName[] = "Игнорир.ош.ФР";
CPU_INT08U const DisableFiscalErrorsList_str0[] = "нет";
CPU_INT08U const DisableFiscalErrorsList_str1[] = "да";
CPU_INT08U const *DisableFiscalErrorsList[] = {DisableFiscalErrorsList_str0, DisableFiscalErrorsList_str1};

TDataDescStruct const DisableFiscalErrorsDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.DisableFiscalErrors),            // указатель на переменную или адрес FRAM
  (void*)&DisableFiscalErrorsRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  DisableFiscalErrorsName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  DisableFiscalErrorsList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Время идущего сеанса
*************************************/
CPU_INT32U  WorkTime[CHANNELS_NUM];
TRangeValueULONG const WorkTimeRange = {0, 0xffffffffL};
CPU_INT08U const WorkTimeName[] = "Вр.работы";

TDataDescStruct const WorkTimeDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,                     // указатель на десриптор индекса массива
  &WorkTime,                // указатель на переменную или адрес FRAM
  (void*)&WorkTimeRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  WorkTimeName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  Тайм-аут перед запуском поканально
*************************************/
TRangeValueULONG const TimeOutBeforeRange = {0, 999};
CPU_INT08U const TimeOutBeforeName[] = "Пауза до,сек.";

TDataDescStruct const TimeOutBeforeDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.TimeOutBefore),                // указатель на переменную или адрес FRAM
  (void*)&TimeOutBeforeRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  TimeOutBeforeName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  30                           
};

/*************************************
  Тайм-аут после работы поканально
*************************************/
TRangeValueULONG const TimeOutAfterRange = {0, 99};
CPU_INT08U const TimeOutAfterName[] = "Пауза после,мин.";

TDataDescStruct const TimeOutAfterDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.TimeOutAfter),                // указатель на переменную или адрес FRAM
  (void*)&TimeOutAfterRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  TimeOutAfterName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  3                           
};

/*************************************
  Максимальное время поканально, мин.
*************************************/
TRangeValueULONG const MaxWorkTimeRange = {1, 999};
CPU_INT08U const MaxWorkTimeName[] = "Tmax,мин.";

TDataDescStruct const MaxWorkTimeDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.MaxWorkTime),                // указатель на переменную или адрес FRAM
  (void*)&MaxWorkTimeRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  MaxWorkTimeName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  20                           
};

/*************************************
  Минимальное время поканально, мин.
*************************************/
TRangeValueULONG const MinWorkTimeRange = {1, 999};
CPU_INT08U const MinWorkTimeName[] = "Tmin,мин.";

TDataDescStruct const MinWorkTimeDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.MinWorkTime),                // указатель на переменную или адрес FRAM
  (void*)&MinWorkTimeRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  MinWorkTimeName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  5                           
};

/*************************************
  Настройка выходных
*************************************/
TRangeValueULONG const WeekEndRange = {0, 4};
CPU_INT08U const WeekEndName[] = "Выходные:";
CPU_INT08U const WeekEndList_str0[] = "нет";
CPU_INT08U const WeekEndList_str1[] = "пт-вс";
CPU_INT08U const WeekEndList_str2[] = "сб-вс";
CPU_INT08U const WeekEndList_str3[] = "пт-сб";
CPU_INT08U const WeekEndList_str4[] = "пт-пн";
CPU_INT08U const *WeekEndList[] = {WeekEndList_str0, WeekEndList_str1, WeekEndList_str2, WeekEndList_str3, WeekEndList_str4, NULL};

TDataDescStruct const WeekEndDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.WeekEnd),            // указатель на переменную или адрес FRAM
  (void*)&WeekEndRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  WeekEndName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  WeekEndList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Настройка отложенного старта
*************************************/
TRangeValueULONG const DeferredStartRange = {0, 1};
CPU_INT08U const DeferredStartName[] = "Отлож.старт";
CPU_INT08U const DeferredStart_str0[] = "нет";
CPU_INT08U const DeferredStart_str1[] = "да";
CPU_INT08U const *DeferredStartList[] = {DeferredStart_str0, DeferredStart_str1, NULL};

TDataDescStruct const DeferredStartDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM,             // размер массива
  &ChannelIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DefferedStartEnabled),            // указатель на переменную или адрес FRAM
  (void*)&DeferredStartRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  DeferredStartName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  DeferredStartList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Индекс при настройке цен
*************************************/
TRangeValueULONG const PeriodIndexRange = {0, 0xffffffff/*CHANNELS_NUM*PRICE_PERIODS_NUM-1*/};
CPU_INT08U const PeriodIndexName[] = "";
CPU_INT08U const *PeriodWeekendIndexList[] = {
                                              "КАН.1 ПЕР.1 ВЫХ.",
                                              "КАН.1 ПЕР.2 ВЫХ.",
                                              "КАН.1 ПЕР.3 ВЫХ.",
                                              "КАН.1 ПЕР.4 ВЫХ.",
                                              "КАН.2 ПЕР.1 ВЫХ.",
                                              "КАН.2 ПЕР.2 ВЫХ.",
                                              "КАН.2 ПЕР.3 ВЫХ.",
                                              "КАН.2 ПЕР.4 ВЫХ.",
                                              "КАН.3 ПЕР.1 ВЫХ.",
                                              "КАН.3 ПЕР.2 ВЫХ.",
                                              "КАН.3 ПЕР.3 ВЫХ.",
                                              "КАН.3 ПЕР.4 ВЫХ.",
                                              "КАН.4 ПЕР.1 ВЫХ.",
                                              "КАН.4 ПЕР.2 ВЫХ.",
                                              "КАН.4 ПЕР.3 ВЫХ.",
                                              "КАН.4 ПЕР.4 ВЫХ.",
                                              "КАН.5 ПЕР.1 ВЫХ.",
                                              "КАН.5 ПЕР.2 ВЫХ.",
                                              "КАН.5 ПЕР.3 ВЫХ.",
                                              "КАН.5 ПЕР.4 ВЫХ.",
                                              "КАН.6 ПЕР.1 ВЫХ.",
                                              "КАН.6 ПЕР.2 ВЫХ.",
                                              "КАН.6 ПЕР.3 ВЫХ.",
                                              "КАН.6 ПЕР.4 ВЫХ.",
                                              "КАН.7 ПЕР.1 ВЫХ.",
                                              "КАН.7 ПЕР.2 ВЫХ.",
                                              "КАН.7 ПЕР.3 ВЫХ.",
                                              "КАН.7 ПЕР.4 ВЫХ.",
                                              "КАН.8 ПЕР.1 ВЫХ.",
                                              "КАН.8 ПЕР.2 ВЫХ.",
                                              "КАН.8 ПЕР.3 ВЫХ.",
                                              "КАН.8 ПЕР.4 ВЫХ.",
                                              "КАН.9 ПЕР.1 ВЫХ.",
                                              "КАН.9 ПЕР.2 ВЫХ.",
                                              "КАН.9 ПЕР.3 ВЫХ.",
                                              "КАН.9 ПЕР.4 ВЫХ.",
                                              "КАН.10 ПЕР.1 ВЫХ.",
                                              "КАН.10 ПЕР.2 ВЫХ.",
                                              "КАН.10 ПЕР.3 ВЫХ.",
                                              "КАН.10 ПЕР.4 ВЫХ.",
                                               NULL};

CPU_INT32U PeriodIndex = 0;

void OnChangePeriodIndex(void)
{
  if ((PeriodIndex == 0xffffffff) || (PeriodIndex < ChannelIndex*PRICE_PERIODS_NUM)) PeriodIndex = (ChannelIndex+1)*PRICE_PERIODS_NUM-1;
  else if (PeriodIndex >= (ChannelIndex+1)*PRICE_PERIODS_NUM) PeriodIndex = (ChannelIndex)*PRICE_PERIODS_NUM;
}

TDataDescStruct const PeriodWeekendIndexDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)&PeriodIndex,            // указатель на переменную или адрес FRAM
  (void*)&PeriodIndexRange,     // указатель на границы параметра
  OnChangePeriodIndex,                     // функция по изменению
  0,       // смещение между элементами в массиве
  PeriodIndexName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  PeriodWeekendIndexList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

CPU_INT08U const *PeriodWeekdaysIndexList[] = {
                                              "КАН.1 ПЕР.1 БУД.",
                                              "КАН.1 ПЕР.2 БУД.",
                                              "КАН.1 ПЕР.3 БУД.",
                                              "КАН.1 ПЕР.4 БУД.",
                                              "КАН.2 ПЕР.1 БУД.",
                                              "КАН.2 ПЕР.2 БУД.",
                                              "КАН.2 ПЕР.3 БУД.",
                                              "КАН.2 ПЕР.4 БУД.",
                                              "КАН.3 ПЕР.1 БУД.",
                                              "КАН.3 ПЕР.2 БУД.",
                                              "КАН.3 ПЕР.3 БУД.",
                                              "КАН.3 ПЕР.4 БУД.",
                                              "КАН.4 ПЕР.1 БУД.",
                                              "КАН.4 ПЕР.2 БУД.",
                                              "КАН.4 ПЕР.3 БУД.",
                                              "КАН.4 ПЕР.4 БУД.",
                                              "КАН.5 ПЕР.1 БУД.",
                                              "КАН.5 ПЕР.2 БУД.",
                                              "КАН.5 ПЕР.3 БУД.",
                                              "КАН.5 ПЕР.4 БУД.",
                                              "КАН.6 ПЕР.1 БУД.",
                                              "КАН.6 ПЕР.2 БУД.",
                                              "КАН.6 ПЕР.3 БУД.",
                                              "КАН.6 ПЕР.4 БУД.",
                                              "КАН.7 ПЕР.1 БУД.",
                                              "КАН.7 ПЕР.2 БУД.",
                                              "КАН.7 ПЕР.3 БУД.",
                                              "КАН.7 ПЕР.4 БУД.",
                                              "КАН.8 ПЕР.1 БУД.",
                                              "КАН.8 ПЕР.2 БУД.",
                                              "КАН.8 ПЕР.3 БУД.",
                                              "КАН.8 ПЕР.4 БУД.",
                                              "КАН.9 ПЕР.1 БУД.",
                                              "КАН.9 ПЕР.2 БУД.",
                                              "КАН.9 ПЕР.3 БУД.",
                                              "КАН.9 ПЕР.4 БУД.",
                                              "КАН.10 ПЕР.1 БУД.",
                                              "КАН.10 ПЕР.2 БУД.",
                                              "КАН.10 ПЕР.3 БУД.",
                                              "КАН.10 ПЕР.4 БУД.",
                                               NULL};
TDataDescStruct const PeriodWeekdaysIndexDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)&PeriodIndex,            // указатель на переменную или адрес FRAM
  (void*)&PeriodIndexRange,     // указатель на границы параметра
  OnChangePeriodIndex,                     // функция по изменению
  0,       // смещение между элементами в массиве
  PeriodIndexName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  PeriodWeekdaysIndexList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Цена по выходным
*************************************/
TRangeValueULONG const PriceWeekendRange = {1, MAX_PRICE};
CPU_INT08U const PriceWeekendName[] = "Цена,руб.";

TDataDescStruct const PriceWeekendDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // размер массива
  &PeriodWeekendIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.Price_Weekend),                // указатель на переменную или адрес FRAM
  (void*)&PriceWeekendRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PriceWeekendName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  15                           
};

/*************************************
  Цена по будням
*************************************/
TRangeValueULONG const PriceWeekdaysRange = {1, MAX_PRICE};
CPU_INT08U const PriceWeekdaysName[] = "Цена,руб.";

TDataDescStruct const PriceWeekdaysDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // размер массива
  &PeriodWeekdaysIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.Price_Weekdays),                // указатель на переменную или адрес FRAM
  (void*)&PriceWeekdaysRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PriceWeekdaysName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  15                         
};


/*************************************
  Время за цену по выходным
*************************************/
TRangeValueULONG const PriceTimeWeekendRange = {1, 999};
CPU_INT08U const PriceTimeWeekendName[] = "За время,мин.";

TDataDescStruct const PriceTimeWeekendDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // размер массива
  &PeriodWeekendIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.PriceTime_Weekend),                // указатель на переменную или адрес FRAM
  (void*)&PriceTimeWeekendRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PriceTimeWeekendName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  Время за цену по будням
*************************************/
TRangeValueULONG const PriceTimeWeekdaysRange = {1, 999};
CPU_INT08U const PriceTimeWeekdaysName[] = "За время,мин.";

TDataDescStruct const PriceTimeWeekdaysDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // размер массива
  &PeriodWeekdaysIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.PriceTime_Weekdays),                // указатель на переменную или адрес FRAM
  (void*)&PriceTimeWeekdaysRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  PriceTimeWeekdaysName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};


/*************************************
  Начало периодов по будням
*************************************/
TRangeValueULONG const T_Start_WeekdaysRange = {0, 24};
CPU_INT08U const T_Start_WeekdaysName[] = "Начало,ч";

TDataDescStruct const T_Start_WeekdaysDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // размер массива
  &PeriodWeekdaysIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.T_Start_Weekdays),                // указатель на переменную или адрес FRAM
  (void*)&T_Start_WeekdaysRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  T_Start_WeekdaysName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Конец периодов по будням
*************************************/
TRangeValueULONG const T_End_WeekdaysRange = {0, 24};
CPU_INT08U const T_End_WeekdaysName[] = "Конец,ч";

TDataDescStruct const T_End_WeekdaysDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // размер массива
  &PeriodWeekdaysIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.T_End_Weekdays),                // указатель на переменную или адрес FRAM
  (void*)&T_End_WeekdaysRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  T_End_WeekdaysName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  24                           
};


/*************************************
  Начало периодов по выходным
*************************************/
TRangeValueULONG const T_Start_WeekendRange = {0, 24};
CPU_INT08U const T_Start_WeekendName[] = "Начало,ч";

TDataDescStruct const T_Start_WeekendDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // размер массива
  &PeriodWeekendIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.T_Start_Weekend),                // указатель на переменную или адрес FRAM
  (void*)&T_Start_WeekendRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  T_Start_WeekendName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Конец периодов по выходным
*************************************/
TRangeValueULONG const T_End_WeekendRange = {0, 24};
CPU_INT08U const T_End_WeekendName[] = "Конец,ч";

TDataDescStruct const T_End_WeekendDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  CHANNELS_NUM*PRICE_PERIODS_NUM,             // размер массива
  &PeriodWeekendIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ChannelConfig.T_End_Weekend),                // указатель на переменную или адрес FRAM
  (void*)&T_End_WeekendRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  T_End_WeekendName,             // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  24                           
};


/*************************************
  Команда инициализации по умолчанию
*************************************/
CPU_INT32U InitByDefault = 0;

TRangeValueULONG const InitByDefaultRange = {0, 1};
CPU_INT08U const InitByDefaultName[] = "Инициализация";
CPU_INT08U const InitByDefaultList_str0[] = "нет";
CPU_INT08U const InitByDefaultList_str1[] = "да";
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
  char name[32] = "Услуги мойки самообслуживания\0\0\0";
  WriteArrayFram(offsetof(TFramMap, manual_service_flag), 4, (unsigned char*)&flag);
  WriteArrayFram(offsetof(TFramMap, manual_service_name), 32, (unsigned char*)&name);
}


TDataDescStruct const InitByDefaultDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&InitByDefault,                // указатель на переменную или адрес FRAM
  (void*)&InitByDefaultRange,           // указатель на границы параметра
  OnChangeInitByDefault,                     // функция по изменению
  0,       // смещение между элементами в массиве
  InitByDefaultName,             // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  InitByDefaultList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  Команда на печать Z-отчета
*************************************/
CPU_INT32U PrintZReportCmd = 0;

CPU_INT08U const PrintZReportName[] = "Отч.о закр.см.";
CPU_INT08U const PrintZReportList_str0[] = "нет";
CPU_INT08U const PrintZReportList_str1[] = "печать";
CPU_INT08U const *PrintZReportList[] = {PrintZReportList_str0, PrintZReportList_str1};


void OnChangePrintZReportCmd(void)
{
}

TDataDescStruct const PrintZReportDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&PrintZReportCmd,                // указатель на переменную или адрес FRAM
  (void*)&InitByDefaultRange,           // указатель на границы параметра
  OnChangePrintZReportCmd,                     // функция по изменению
  0,       // смещение между элементами в массиве
  PrintZReportName,             // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  PrintZReportList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  Команда на печать X-отчета
*************************************/
CPU_INT32U PrintXReportCmd = 0;

CPU_INT08U const PrintXReportName[] = "X-отчет";

void OnChangePrintXReportCmd(void)
{
}

TDataDescStruct const PrintXReportDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&PrintXReportCmd,                // указатель на переменную или адрес FRAM
  (void*)&InitByDefaultRange,           // указатель на границы параметра
  OnChangePrintXReportCmd,                     // функция по изменению
  0,       // смещение между элементами в массиве
  PrintXReportName,             // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  PrintZReportList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Ошибка в журнале ошибок
*************************************/
TRangeValueULONG const ErrorNumberRange = {0, JOURNAL_EVENTS_COUNT - 1};
CPU_INT08U const *ErrorNumberList0[JOURNAL_EVENTS_COUNT] = {
                                        "нет", 
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
                                        "Отчет отправлен",
                                        "Хоппер заполнен",
                                        "Выдано",
                                        
                                        "Хоппер ошибка датчика",
                                        "Хоппер пуст",
                                        
                                       "ошибка связи с", 
                                       "ошибка работы", 
                                       "1Ch60h-выбр.купюры", 
                                       "1Ch61h-выбр.купюры",
                                       "1Ch64h-выбр.купюры",
                                       "1Ch65h-выбр.купюры",
                                       "1Ch66h-выбр.купюры",
                                       "1Ch67h-выбр.купюры",
                                       "1Ch68h-выбр.купюры",
                                       "1Ch69h-выбр.купюры",
                                       "1Ch6Ch-выбр.купюры",
                                       "к/п:41h-кассета",
                                       "к/п:42h-кассета",
                                       "к/п:43h-замин в ",
                                       "к/п:44h-замин",
                                       "к/п:45h-попытка",
                                       "к/п:50h-ошибка",
                                       "к/п:51h-ошибка скор.",
                                       "к/п:52h-ошибка",
                                       "к/п:53h-ошибка мех.",
                                       "к/п:54h-кассета",
                                       "к/п:65h-ошибка",
                                       "к/п:66h-ошибка",
                                       "к/п:67h-ошибка емк.",
                                       "ошибка", 
                                       "ошибка",
                                       "ФР:01h-Неизвестная",
                                       "ФР:02h-Неверное",
                                       "ФР:03h-Ошибка ФН",
                                       "ФР:04h-Ошибка КС",
                                       "ФР:05h-Закончен срок",  
                                       "ФР:06h-Архив ФН", 
                                       "ФР:07h-Неверные дата", 
                                       "ФР:08h-Нет ", 
                                       "ФР:09h-Некорр.", 
                                       "ФР:10h-Превышение", 
                                       "ФР:11h-Нет", 
                                       "ФР:12h-Исчерпан",
                                       "ФР:14h-Исчерпан", 
                                       "ФР:15h-Исчерпан", 
                                       "ФР:16h-Смена более",
                                       "ФР:17h-Неверная", 
                                      "ФР:20h-Сообщение", 
                                      "ФР:2Fh-Таймаут ",
                                      "ФР:30h-ФН ", 
                                      "ФР:33h-Некорректные", 
                                      "ФР:34h-Нет", 
                                      "ФР:35h-Некорректный", 
                                      "ФР:36h-Некорректные", 
                                      "ФР:37h-Команда", 
                                      "ФР:38h-Ошибка", 
                                      "ФР:39h-Внутренняя", 
                                      "ФР:3Ah-Переполнение", 
                                      "ФР:3Ch-Смена", 
                                      "ФР:3Dh-Смена", 
                                      "ФР:3Eh-Переполнение", 
                                      "ФР:3Fh-Переполнение", 
                                      "ФР:40h-Переполнение", 
                                      "ФР:41h-Переполнение", 
                                      "ФР:42h-Переполнение", 
                                      "ФР:43h-Переполнение", 
                                      "ФР:44h-Переполнение", 
                                      "ФР:45h-Cумма", 
                                      "ФР:46h-Не хватает", 
                                      "ФР:47h-Переполнение", 
                                      "ФР:48h-Переполнение", 
                                      "ФР:49h-Опер.невозм.", 
                                      "ФР:4Ah-Открыт чек", 
                                      "ФР:4Bh-Буфер чека", 
                                      "ФР:4Ch-Переполнение", 
                                      "ФР:4Dh-Вносимая", 
                                      "ФР:4Eh-Смена", 
                                      "ФР:4Fh-Неверный пароль", 
                                      "ФР:50h-Идет печать", 
                                      "ФР:51h-Переполнение", 
                                      "ФР:52h-Переполнение", 
                                      "ФР:53h-Переполнение", 
                                      "ФР:54h-Переполнение", 
                                      "ФР:55h-Чек закрыт", 
                                      "ФР:56h-Нет док.", 
                                      "ФР:58h-Ожидание", 
                                      "ФР:59h-Документ", 
                                      "ФР:5Bh-Переполнение", 
                                      "ФР:5Ch-Понижено", 
                                      "ФР:5Dh-Таблица", 
                                      "ФР:5Eh-Некорректная",
                                      "ФР:5Fh-Отриц.", 
                                      "ФР:60h-Переполнение",
                                      "ФР:61h-Переполнение",
                                      "ФР:62h-Переполнение",
                                      "ФР:63h-Переполнение",
                                      "ФР:65h-Не хватает",
                                      "ФР:66h-Переполнение",
                                      "ФР:68h-Не хватает",
                                      "ФР:69h-Переполнение",
                                      "ФР:6Ah-Ошибка",
                                      "ФР:6Bh-Нет чековой",
                                      "ФР:6Ch-Нет контр.",
                                      "ФР:6Dh-Не хватает",
                                      "ФР:6Eh-Переполнение",
                                      "ФР:6Fh-Переполнение",
                                      "ФР:71h-Ошибка", 
                                      "ФР:72h-Команда не", 
                                      "ФР:73h-Команда не", 
                                      "ФР:74h-Ошибка ОЗУ", 
                                      "ФР:75h-Ошибка", 
                                      "ФР:77h-Принтер:", 
                                      "ФР:78h-Замена ПО", 
                                      "ФП:7Ah-Поле не", 
                                      "ФР:7Bh-Ошибка", 
                                      "ФР:7Ch-Не совпадает", 
                                      "ФР:7Dh-Неверный", 
                                      "ФР:7Eh-Неверное", 
                                      "ФР:7Fh-Переполнение", 
                                      "ФР:84h-Переполнение", 
                                      "ФР:85h-Переполнение", 
                                      "ФР:86h-Переполнение", 
                                      "ФР:87h-Переполнение", 
                                      "ФР:88h-Переполнение", 
                                      "ФР:89h-Переполнение", 
                                      "ФР:8Ah-Переполнение", 
                                      "ФР:8Bh-Переполнение", 
                                      "ФР:8Ch-Отриц.", 
                                      "ФР:8Dh-Отрицательный", 
                                      "ФР:8Eh-Нулевой итог", 
                                      "ФР:90h-Поле прев.", 
                                      "ФР:91h-Выход за", 
                                      "ФР:92h-Наложение", 
                                      "ФР:93h-Восстановление", 
                                      "ФР:94h-Исчерпан", 
                                      "ФР:C0h-Контроль", 
                                      "ФР:C2h-Превышение", 
                                      "ФР:C4h-Несовпадение", 
                                      "ФР:C7h-Поле не", 
                                      "ФР:С8h-Отсутствуют"
};

TDataDescStruct const JournalErrorNumberDesc0 = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  EVENT_RECORDS_COUNT,             // размер массива
  &EventJournalIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, EventRecords[0].event),                // указатель на переменную или адрес FRAM
  (void*)&ErrorNumberRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(TEventRecord),       // смещение между элементами в массиве
  NULL,             // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  (void*)ErrorNumberList0,                     // указатель на список строк для индексного параметра
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
                                        
                                       "купюроприемником", 
                                       "купюроприемника", 
                                       "при замине", 
                                       "по маг.датчику",
                                       "при транспорт.",
                                       "по идентификации",
                                       "по верификации ",
                                       "по оптич.датчику ",
                                       "по запрету",
                                       "по емкост.датчику",
                                       "по длине",
                                       "заполнена",
                                       "отсутствует",
                                       "купюроприемнике",
                                       "в кассете",
                                       "обмана",
                                       "стекерного мотора",
                                       "транспорт.мотора",
                                       "транспорт.мотора",
                                       "выравнивания",
                                       "отсутствует",
                                       "оптики",
                                       "маг.датчика",
                                       "датчика",
                                       "модема", 
                                       "связи с ФР", 
                                       
                                       "команда",  // ФР:01h
                                       "состояние ФН",                  
                                       "",
                                       "",                                       
                                       "эксплуатации ФН",
                                       "переполнен",
                                       "и/или время",
                                       "запрошенных данных",
                                       "знач.пар.к-ды",
                                       "разм.TLV данных",
                                       "трансп.соед.",
                                       "ресурс КС",
                                       "ресурс хранения",
                                       "ресурс ожидания",
                                       "24 часов",
                                       "разница во времени",
                                        "от ОФД не принято",
                                        "обмена с ФН",
                                        "не отвечает",
                                        "параметры в к-де",
                                        "данных",
                                        "параметр",
                                        "параметры",    // 36h
                                        "не поддерживается",
                                        "в ПЗУ",
                                        "ошибка ПО ККТ",
                                        "нак.по надб.",
                                        "откр.-операция невозм.",
                                        "закр.-операция невозм.",       //3Dh
                                        "накоп.по секциям",
                                        "накоп.по скидкам",
                                        "диапазона скидок",
                                        "диапазона оплаты",
                                        "диапазона оплаты 2",
                                        "диапазона оплаты 3",
                                        "диапазона оплаты 4",
                                        "меньше итога чека",
                                        "наличности в кассе",
                                        "накопления по налогам",
                                        "итога чека",
                                        "в откр.чеке",
                                        "операция невозможна",
                                        "переполнен",
                                        "накоп.по налогам",
                                        "безнал.сумма больше",
                                        "превысила 24 часа",
                                        "",
                                        "пред.команды",
                                        "накоплений в смене",
                                        "накоплений 2 в смене",
                                        "накоплений 3 в смене",
                                        "накоплений 4 в смене",
                                        "операция невозм.",
                                        "для повтора",
                                        "команды продолжения",
                                        "открыт другим оп.",
                                        "диапазона надбавок",
                                        "напряжение 24В",
                                        "не определена",
                                        "операция",
                                        "итог чека",
                                        "при умножении",
                                        "диапазона цены",
                                        "диапазона кол-ва",
                                        "диапазона отдела",
                                        "денег в секции",
                                        "денег в секции",
                                        "денег по налогам",
                                        "денег по налогам",
                                        "питания",
                                        "ленты",
                                        "ленты",
                                        "денег по налогу",
                                        "денег по налогу",
                                        "по выплате в смене",
                                        "отрезчика",
                                        "поддерж.",
                                        "поддерж.",
                                        "",
                                        "питания",
                                        "нет сигнала",
                                        "",
                                        "редактируется",
                                        "оборудования",
                                        "дата",
                                        "формат даты",
                                        "значение в поле длины",
                                        "диапазона итога чека",
                                        "наличности",
                                        "по продажам",
                                        "по покупкам",
                                        "по возвратам продаж",
                                        "по возвратам покупок",
                                        "по внесению в смене",
                                        "по надбавкам в чеке",
                                        "по скидкам в чеке",
                                        "итог надбавки",
                                        "итог скидки в чеке",
                                        "чека",
                                        "размер в настройках",
                                        "границу поля печати",
                                        "полей",
                                        "ОЗУ прошло успешно",
                                        "лимит операций в чеке",
                                        "даты и времени",
                                        "напряжения",
                                        "номеров смен",
                                        "редактируется",
                                        "импульсы тахо."
};

TDataDescStruct const JournalErrorNumberDesc1 = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  EVENT_RECORDS_COUNT,             // размер массива
  &EventJournalIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, EventRecords[0].event),                // указатель на переменную или адрес FRAM
  (void*)&ErrorNumberRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(TEventRecord),       // смещение между элементами в массиве
  NULL,             // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  (void*)ErrorNumberList1,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  Строка с ошибкой по-английски
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
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,             // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  EVENT_RECORDS_COUNT,             // размер массива
  &EventJournalIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, EventRecords[0].event),                // указатель на переменную или адрес FRAM
  (void*)&ErrorNumberRange,           // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(TEventRecord),       // смещение между элементами в массиве
  NULL,             // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  (void*)ErrorNumberList1,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  Индекс при просмотре журнала событий
*************************************/
TRangeValueULONG const EventJournalIndexRange = {0, 0xffffffff};
CPU_INT08U const EventJournalIndexName[] = "Запись #";
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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)&EventJournalIndex,            // указатель на переменную или адрес FRAM
  (void*)&EventJournalIndexRange,     // указатель на границы параметра
  OnChangeEventJournalIndex,                     // функция по изменению
  0,       // смещение между элементами в массиве
  EventJournalIndexName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Время события в журнале событий
*************************************/
TDataDescStruct const JournalEventTimeDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_TIME,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  EVENT_RECORDS_COUNT,             // размер массива
  &EventJournalIndexDesc,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, EventRecords[0].time),            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(TEventRecord),       // смещение между элементами в массиве
  NULL,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Системное время
*************************************/
extern CPU_INT32U SystemTime;

TDataDescStruct const SystemTimeDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_TIME,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)&SystemTime,            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  NULL,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_TIME,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)&SystemTime,            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  OnSetTime,                     // функция по изменению
  0,       // смещение между элементами в массиве
  NULL,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  Команда на очистку журналов
*************************************/
CPU_INT32U ClearJournalCmd = 0;

CPU_INT08U const ClearJournalCmdName[] = "Очистка";
CPU_INT08U const ClearJournalCmdList_str0[] = "нет";
CPU_INT08U const ClearJournalCmdList_str1[] = "да";
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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&ClearJournalCmd,                // указатель на переменную или адрес FRAM
  (void*)&InitByDefaultRange,           // указатель на границы параметра
  OnChangeClearJournalCmd,                     // функция по изменению
  0,       // смещение между элементами в массиве
  ClearJournalCmdName,             // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  ClearJournalCmdList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  Общий счетчик числа запусков
*************************************/
CPU_INT08U const CounterRunName[] = "Запуски";

TDataDescStruct const CounterRunDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterRun),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterRunName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик денег
*************************************/
CPU_INT08U const CounterMoneyName[] = "Сумма нал.,руб.";

TDataDescStruct const CounterMoneyDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterMoney),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterMoneyName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик времени работы
*************************************/
CPU_INT08U const CounterTimeName[] = "Вр.раб.";

TDataDescStruct const CounterTimeDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_TIME_COUNT,     // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterTime),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterTimeName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Выданные жетоны
*************************************/
CPU_INT08U const CounterCoinOutName[] = "Жетоны";

TDataDescStruct const CounterCoinOutDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterCoinOut),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterCoinOutName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик денег через монетник 
*************************************/
CPU_INT08U const CounterCoinName[] = "Монеты,руб.";

TDataDescStruct const CounterCoinDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterCoin),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterCoinName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик денег через купюрник
*************************************/
CPU_INT08U const CounterCashName[] = "Купюры,руб.";

TDataDescStruct const CounterCashDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterCash),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterCashName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик налиных денег
*************************************/
CPU_INT08U const CounterBillName[] = "Сумма нал.,руб.";

TDataDescStruct const CounterAllCashDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterAllCash),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterBillName,          // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик денег через банковский терминал
*************************************/
CPU_INT08U const CounterBankName[] = "Банк,руб.";

TDataDescStruct const CounterBankDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterBank),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterBankName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик числа запусков ДЛИННЫЙ
*************************************/
TDataDescStruct const CounterLongRunDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CountersLong.CounterRunLong),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterRunName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик денег ДЛИННЫЙ 
*************************************/
TDataDescStruct const CounterLongMoneyDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CountersLong.CounterMoneyLong),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterMoneyName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Выданные жетоны ДЛИННЫЙ
*************************************/
TDataDescStruct const CounterLongCoinOutDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CountersLong.CounterCoinOutLong),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterCoinOutName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик денег через монетник ДЛИННЫЙ 
*************************************/
TDataDescStruct const CounterLongCoinDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CountersLong.CounterCoinLong),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterCoinName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик денег через купюрник ДЛИННЫЙ 
*************************************/
TDataDescStruct const CounterLongCashDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CountersLong.CounterCashLong),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterCashName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик налиных денег ДЛИННЫЙ
*************************************/
TDataDescStruct const CounterLongAllCashDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CountersLong.CounterAllCashLong),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterBillName,          // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик денег через банковский терминал ДЛИННЫЙ 
*************************************/
TDataDescStruct const CounterLongBankDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CountersLong.CounterBankLong),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterBankName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общий счетчик времени работы
*************************************/
TDataDescStruct const CounterLongTimeDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_TIME_COUNT,     // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CountersLong.CounterTimeLong),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  CounterTimeName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Команда на очистку статистики 
*************************************/
CPU_INT32U ClearStatCmd = 0;

CPU_INT08U const ClearStatCmdName[] = "Очистка";

void OnChangeClearStatCmd(void)
{
  if (ClearStatCmd)
    {
      ClearCounters();
      ClearStatCmd = 0;
    }
}

TDataDescStruct const ClearStatCmdDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&ClearStatCmd,                // указатель на переменную или адрес FRAM
  (void*)&InitByDefaultRange,           // указатель на границы параметра
  OnChangeClearStatCmd,                     // функция по изменению
  0,       // смещение между элементами в массиве
  ClearJournalCmdName,             // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  ClearJournalCmdList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  Разрешение автоматического закрытия смен
*************************************/
TRangeValueULONG const EnableFiscalDayClearRange = {0, 1};
CPU_INT08U const EnableFiscalDayClearName[] = "Закр.смены";
CPU_INT08U const *EnableFiscalDayClearList[] = {"Руч.", "Авто", "Буфер"};

TDataDescStruct const EnableFiscalDayClearDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.EnableFiscalDayClear),            // указатель на переменную или адрес FRAM
  (void*)&EnableFiscalDayClearRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableFiscalDayClearName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableFiscalDayClearList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           // значение по умолчанию
};


/*************************************
  Настройка чека
*************************************/
TRangeValueULONG const BillFormatRange = {0, 1};
CPU_INT08U const BillFormatName[] = "Чек:";
CPU_INT08U const BillFormatList_str0[] = "кол-во*цена";
CPU_INT08U const BillFormatList_str1[] = "сумма";
CPU_INT08U const *BillFormatList[] = {BillFormatList_str0, BillFormatList_str1};

TDataDescStruct const BillFormatDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.BillFormat),            // указатель на переменную или адрес FRAM
  (void*)&BillFormatRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  BillFormatName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  BillFormatList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  Система налогообложения
*************************************/
TRangeValueULONG const TaxSystemRange = {0, 5};
CPU_INT08U const TaxSystemName[] = "Система нал.";
CPU_INT08U const TaxSystemList_str0[] = "ОСН";
CPU_INT08U const TaxSystemList_str1[] = "УСН д";
CPU_INT08U const TaxSystemList_str2[] = "УСН д-р";
CPU_INT08U const TaxSystemList_str3[] = "ЕНВД";
CPU_INT08U const TaxSystemList_str4[] = "ЕСН";
CPU_INT08U const TaxSystemList_str5[] = "ПСН";

CPU_INT08U const *TaxSystemList[] = {TaxSystemList_str0, TaxSystemList_str1, TaxSystemList_str2, TaxSystemList_str3, TaxSystemList_str4, TaxSystemList_str5};

TDataDescStruct const TaxSystemDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, TaxSystem),            // указатель на переменную или адрес FRAM
  (void*)&TaxSystemRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  TaxSystemName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  TaxSystemList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  Индекс налога
*************************************/
TRangeValueULONG const TaxFormatRange = {1, 6};
CPU_INT08U const TaxFormatName[] = "Налог:";
CPU_INT08U const TaxFormatList_str0[] = "откл.";
CPU_INT08U const TaxFormatList_str1[] = "  1  ";
CPU_INT08U const TaxFormatList_str2[] = "  2  ";
CPU_INT08U const TaxFormatList_str3[] = "  3  ";
CPU_INT08U const TaxFormatList_str4[] = "  4  ";
CPU_INT08U const TaxFormatList_str5[] = "  5  ";
CPU_INT08U const TaxFormatList_str6[] = "  6  ";
CPU_INT08U const *TaxFormatList[] = {TaxFormatList_str0, TaxFormatList_str1, TaxFormatList_str2, TaxFormatList_str3, TaxFormatList_str4, TaxFormatList_str5, TaxFormatList_str6};

TDataDescStruct const TaxFormatDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, TaxFormat),            // указатель на переменную или адрес FRAM
  (void*)&TaxFormatRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  TaxFormatName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  TaxFormatList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  4                           
};

/*************************************
  Предмет расчета
*************************************/
TRangeValueULONG const SubjSellRange = {0, 2};
CPU_INT08U const SubjSellName[] = "Предм.расчета";
CPU_INT08U const SubjSellList_str0[] = "ТОВАР";
CPU_INT08U const SubjSellList_str1[] = "РАБОТА";
CPU_INT08U const SubjSellList_str2[] = "УСЛУГА";
CPU_INT08U const *SubjSellList[] = {SubjSellList_str0, SubjSellList_str1, SubjSellList_str2};

TDataDescStruct const SubjSellDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, SubjSell),            // указатель на переменную или адрес FRAM
  (void*)&SubjSellRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  SubjSellName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  SubjSellList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  2                           
};

/*************************************
  Тип команд работы с чеком
*************************************/
TRangeValueULONG const CommandV2Range = {0, 1};
CPU_INT08U const CommandV2Name[] = "Тип команд";
CPU_INT08U const CommandV2List_str0[] = "старый";
CPU_INT08U const CommandV2List_str1[] = "V2";
CPU_INT08U const *CommandV2List[] = {CommandV2List_str0, CommandV2List_str1};

TDataDescStruct const CommandV2Desc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, CommandV2),            // указатель на переменную или адрес FRAM
  (void*)&CommandV2Range,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  CommandV2Name,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  CommandV2List,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           
};

/*************************************
  Название услуги
*************************************/
TRangeValueULONG const ServiceNameRange = {0, 2};
CPU_INT08U const ServiceNameName[] = "Товар";
CPU_INT08U const ServiceNameList_str0[] = "Жетон";
CPU_INT08U const ServiceNameList_str1[] = "Услуги мойки";
CPU_INT08U const ServiceNameList_str3[] = "Услуга туалет";
CPU_INT08U const *ServiceNameList[] = {ServiceNameList_str0, ServiceNameList_str1, ServiceNameList_str3};

void OnChangeServiceName(void)
{
    CPU_INT08U flag[4] = {0, 0, 0, 0};

    WriteArrayFram(offsetof(TFramMap, manual_service_flag), 4, (unsigned char*)&flag);
}

TDataDescStruct const ServiceNameDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.ServiceName),            // указатель на переменную или адрес FRAM
  (void*)&ServiceNameRange,     // указатель на границы параметра
  OnChangeServiceName,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  ServiceNameName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  ServiceNameList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1                           // значение по умолчанию
};

/*************************************
  Дескриптор для энергонезависимого сохранения текущих денег
*************************************/
TDataDescStruct const AcceptedMoneyDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, FRAM_AcceptedMoney),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Дескриптор Crc16 энергонезависимого сохранения текущих денег
*************************************/
TDataDescStruct const AcceptedMoneyCRC16Desc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, crc_AcceptedMoney),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

TDataDescStruct const AcceptedBankMoneyDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, FRAM_AcceptedBankMoney),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0         
};

/*************************************
  Дескриптор Crc16 энергонезависимого сохранения
*************************************/
TDataDescStruct const AcceptedBankMoneyCRC16Desc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, crc_AcceptedBankMoney),            // указатель на переменную или адрес FRAM
   NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0         
};

TDataDescStruct const AcceptedRestMoneyDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, FRAM_AcceptedRestMoney),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0         
};

/*************************************
  Дескриптор Crc16 энергонезависимого сохранения
*************************************/
TDataDescStruct const AcceptedRestMoneyCRC16Desc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, crc_AcceptedRestMoney),            // указатель на переменную или адрес FRAM
   NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0         
};

/*************************************
  Дескриптор для энергонезависимого сохранения выданных жетонов
*************************************/
TDataDescStruct const AcceptedCoinDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, FRAM_AcceptedCoin),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Дескриптор Crc16 энергонезависимого сохранения выданных жетонов
*************************************/
TDataDescStruct const AcceptedCoinCRC16Desc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, crc_AcceptedCoin),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Дескриптор пароля
*************************************/
TRangeValueULONG const PassRange = {0, 9999};
CPU_INT08U const PassName[] = "  ПАРОЛb";

void OnChangePass(void)
{
  // обновим CRC пароля  
  CPU_INT32U pass,crc;
  GetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);      
  crc = crc16((unsigned char*)&pass, sizeof(CPU_INT32U));
  SetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

TDataDescStruct const PassDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Pass),            // указатель на переменную или адрес FRAM
  (void*)&PassRange,                     // указатель на границы параметра
  OnChangePass,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  (void*)&PassName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  DEFAULT_PASSWORD                           
};

/*************************************
  Дескриптор CRC пароля
*************************************/
TDataDescStruct const PassCRCDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, crc_Pass),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Дескриптор временного пароля
*************************************/
CPU_INT32U TempPass = 0;

void OnChangeTempPass(void)
{
  CPU_INT32U pass;
  GetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);
    
  if (GetCurrentMenu() == SetPassMenuPanel)
    { // утановка нового пароля
      if (pass != TempPass)
      {
        SaveEventRecord(0, JOURNAL_EVENT_PASS_FAIL, TempPass);
        GoToMenu(ErrorPassPanel);
      }
      else {GoToPreviousMenu(); GoToMenu(SetNewPassMenuPanel);}
    }
  else if (GetCurrentMenu() == ResetSettingsMenuPanel)
    { // сброс настроек
      if (pass != TempPass) 
      {
        SaveEventRecord(0, JOURNAL_EVENT_PASS_FAIL, TempPass);
        GoToMenu(ErrorPassPanel);
      }
      else {InitByDefault = 1; OnChangeInitByDefault(); GoToPreviousMenu(); GoToMenu(SettingsIsReset);}
    }
  else if (GetCurrentMenu() == ClearStatMenu)
    { // очистка статистики
      if (pass != TempPass)
      {
        SaveEventRecord(0, JOURNAL_EVENT_PASS_FAIL, TempPass);
        GoToMenu(ErrorPassPanel);
      }
      else {ClearStatCmd = 1; OnChangeClearStatCmd(); GoToPreviousMenu(); GoToMenu(StatIsReset);}
    }
  else if (GetCurrentMenu() == ClearJournalMenuPanel)
    { // очистка статистики
      if (pass != TempPass) 
      {
        SaveEventRecord(0, JOURNAL_EVENT_PASS_FAIL, TempPass);
        GoToMenu(ErrorPassPanel);
      }
      else {ClearJournalCmd = 1; OnChangeClearJournalCmd(); GoToPreviousMenu(); GoToMenu(JournalIsReset);}
    }

}

TDataDescStruct const PassTempDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&TempPass,            // указатель на переменную или адрес FRAM
  (void*)&PassRange,                     // указатель на границы параметра
  OnChangeTempPass,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  (void*)&PassName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};


CPU_INT08U const PassTempName1[] = "    ПАРОЛb";

TDataDescStruct const PassTempDesc1 = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&TempPass,            // указатель на переменную или адрес FRAM
  (void*)&PassRange,                     // указатель на границы параметра
  OnChangeTempPass,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  (void*)&PassTempName1,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

TRangeValueULONG const MasterPassRange = {0, 99999999};
CPU_INT08U const MasterPassTempName[] = "ПАРОЛb";

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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&TempPass,            // указатель на переменную или адрес FRAM
  (void*)&MasterPassRange,                     // указатель на границы параметра
  OnChangeMasterPass,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  (void*)&MasterPassTempName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

CPU_INT08U const PassTempName2[] = "   ПАРОЛb";

TDataDescStruct const PassTempDesc2 = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)&TempPass,            // указатель на переменную или адрес FRAM
  (void*)&PassRange,                     // указатель на границы параметра
  OnChangeTempPass,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  (void*)&PassTempName2,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Команда послать тестовое сообщение
*************************************/
CPU_INT08U const SendTestEmailName[] = "Отпр.тест";
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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)&send_test,            // указатель на переменную или адрес FRAM
  (void*)&EnableEmailErrorSendRange,     // указатель на границы параметра
  OnChangeSendTestEmail,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  SendTestEmailName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  DisableFiscalErrorsList,                     // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};


/*************************************
  Индекс номинала купюры для просмотра счетчиков по номиналам
*************************************/
CPU_INT32U  BillnomViewIndex;
TRangeValueULONG const BillnomIndexRange = {0, 23};
CPU_INT08U const BillnomName[] = "   НОМИНАЛ #";
CPU_INT08U const* BillnomItems[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                                    "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24"};

TDataDescStruct const BillnomIndexDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,             // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  &BillnomViewIndex,            // указатель на переменную или адрес FRAM
  (void*)&BillnomIndexRange,       // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  BillnomName,         // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  BillnomItems,             // указатель на список строк для индексного параметра
  DATA_INIT_ENABLE,
  0                           
};

/*************************************
  Значение номинала купюры для просмотра счетчиков
*************************************/
extern CPU_INT32U BillNominals[24];
CPU_INT08U const BillnomValName[] = "Значение,руб.";

TDataDescStruct const BillnomDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  24,                       // размер массива
  (void*)&BillnomIndexDesc,        // указатель на десриптор индекса массива
  (void*)&BillNominals,            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  BillnomValName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Счетчики номиналов купюр в купюрнике
*************************************/
CPU_INT08U const BillnomCountersName[] = "Кол-во";

TDataDescStruct const BillnomCountersDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_IS_ARRAY,            // признак массива
  24,                        // размер массива
  &BillnomIndexDesc,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.CounterBillNominals[0]),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),                        // смещение между элементами в массиве
  BillnomCountersName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Общее количество купюр в купюрнике
*************************************/
CPU_INT08U const BillCounterName[] = "Всего купюр";

TDataDescStruct const BillCounterDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, Counters.BillsCount),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  BillCounterName,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};


/*************************************
  ID устройства 
*************************************/
CPU_INT08U const DeviceIDName[] = "ID уст-ва";
TRangeValueULONG const DeviceIDRange = {0, 9999};

TDataDescStruct const DeviceIDDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.DeviceId),            // указатель на переменную или адрес FRAM
  (void*)&DeviceIDRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  DeviceIDName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Признак отправки сообщения об инкассации
*************************************/
TDataDescStruct const IncasSendFlagDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  1,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, IncasEmailFlag),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Деньги от последней инкассации
*************************************/
TDataDescStruct const IncasMoneyDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  1,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, IncasMoney),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  IP-адрес свой
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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_IP_ADDR,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  1,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, ip),            // указатель на переменную или адрес FRAM
  (void*)&DeferredStartRange,     // указатель на границы параметра
  OnChangeIpMask,                     // функция по изменению
  0,       // смещение между элементами в массиве
  IpAddrName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  DeferredStartList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0xC0A80064
};

/*************************************
  Меска сети
*************************************/
CPU_INT08U const NetMaskName[] = "MSK";

TDataDescStruct const NetMaskDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_IP_ADDR,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  1,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, netmask),            // указатель на переменную или адрес FRAM
  (void*)&DeferredStartRange,     // указатель на границы параметра
  OnChangeIpMask,                     // функция по изменению
  0,       // смещение между элементами в массиве
  NetMaskName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  DeferredStartList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0xFFFFFF00
};

/*************************************
  Шлюз
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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_IP_ADDR,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  1,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, gateway),            // указатель на переменную или адрес FRAM
  (void*)&DeferredStartRange,     // указатель на границы параметра
  OnChangeGateway,                     // функция по изменению
  0,       // смещение между элементами в массиве
  GatewayName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  DeferredStartList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0xC0A80001
};

/*************************************
  Время последней инкассации
*************************************/
TDataDescStruct const IncasTimeDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  1,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, IncasTime),            // указатель на переменную или адрес FRAM
  NULL,                     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),                        // смещение между элементами в массиве
  NULL,           // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

/*************************************
  Стоимость жетона, руб. хоппера
*************************************/
CPU_INT08U const HopperCostName[] = "Жетон,руб";
TRangeValueULONG const HopperCostRange = {0, 9999};

TDataDescStruct const HopperCostDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.hopperCost),            // указатель на переменную или адрес FRAM
  (void*)&HopperCostRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  HopperCostName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  10                          
};

/*************************************
  остановка мотора хоппера, сек
*************************************/
CPU_INT08U const HopperStopEngineName[] = "Останов,сек";
TRangeValueULONG const HopperStopEngineRange = {1, 20};

TDataDescStruct const HopperStopEngineDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.hopperStopEngine),            // указатель на переменную или адрес FRAM
  (void*)&HopperStopEngineRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  HopperStopEngineName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  5                          
};

/*************************************
  Хранить кредит, мин
*************************************/
CPU_INT08U const HopperSaveCreditName[] = "Кредит,мин";
TRangeValueULONG const HopperSaveCreditRange = {0, 60};

TDataDescStruct const HopperSaveCreditDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.hopperSaveCredit),            // указатель на переменную или адрес FRAM
  (void*)&HopperSaveCreditRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  HopperSaveCreditName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                          
};

/*************************************
  Кнопка старт - да/нет
*************************************/
CPU_INT08U const HopperButtonStartName[] = "Кнопка";
TRangeValueULONG const HopperButtonStartRange = {0, 1};

CPU_INT08U const ButtonStartList_str0[] = "Нет";
CPU_INT08U const ButtonStartList_str1[] = "Да";
CPU_INT08U const *ButtonStartList[] = {ButtonStartList_str0, ButtonStartList_str1};

TDataDescStruct const HopperButtonStartDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,                        // размер массива
  NULL,                     // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.hopperButtonStart),            // указатель на переменную или адрес FRAM
  (void*)&HopperButtonStartRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  HopperButtonStartName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  ButtonStartList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                          
};

/*************************************
  Длина импульса входа хоппера в режиме Cube, мс
*************************************/
TRangeValueULONG const HopperPulseLenRange = {20, 2000};
CPU_INT08U const HopperPulseLenName[] = "Длина имп.,мс";

void OnChangeHopperPulseLen()
{
    CPU_INT32U pulse, pause;
    GetData(&HopperPulseLenDesc, &pulse, 0, DATA_FLAG_SYSTEM_INDEX);
    GetData(&HopperPauseLenDesc, &pause, 0, DATA_FLAG_SYSTEM_INDEX);
    SetHopperPulseParam(pulse, pause);
}

TDataDescStruct const HopperPulseLenDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, hopper_pulse_len),            // указатель на переменную или адрес FRAM
  (void*)&HopperPulseLenRange,     // указатель на границы параметра
  OnChangeHopperPulseLen,                     // функция по изменению
  0,       // смещение между элементами в массиве
  HopperPulseLenName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  1000                           
};

/*************************************
  Длина паузы входа хоппера в режиме Cube, мс
*************************************/
TRangeValueULONG const HopperPauseLenRange = {20, 2000};
CPU_INT08U const HopperPauseLenName[] = "Пауза имп.,мс";

TDataDescStruct const HopperPauseLenDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, hopper_pause_len),            // указатель на переменную или адрес FRAM
  (void*)&HopperPauseLenRange,     // указатель на границы параметра
  OnChangeHopperPulseLen,                     // функция по изменению
  0,       // смещение между элементами в массиве
  HopperPauseLenName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  20
};

/*************************************
  Длина паузы остановки мотора в режиме Cube, мс
*************************************/
TRangeValueULONG const HopperPauseEngineOffRange = {0, 50};
CPU_INT08U const HopperPauseEngineOffName[] = "Ост.мот.,мс";

TDataDescStruct const HopperPauseEngineOffDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, hopper_pause_engine_off),            // указатель на переменную или адрес FRAM
  (void*)&HopperPauseEngineOffRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  HopperPauseEngineOffName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0
};

/*************************************
  режим хоппера
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
CPU_INT08U const RegimeHopperName[] = "Режим";
CPU_INT08U const RegimeHopper_str0[] = "Elolution";
CPU_INT08U const RegimeHopper_str1[] = "Cube";
CPU_INT08U const *RegimeHopperList[] = {RegimeHopper_str0, RegimeHopper_str1};

TDataDescStruct const RegimeHopperDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.hopperRegime),            // указатель на переменную или адрес FRAM
  (void*)&RegimeHopperRange,     // указатель на границы параметра
  OnChangeHopperRegime,          // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  RegimeHopperName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  RegimeHopperList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           // значение по умолчанию
};

/*************************************
  Включение игнорирования ошибок хоппера
*************************************/
TRangeValueULONG const DisableHopperErrorsRange = {0, 1};
CPU_INT08U const DisableHopperErrorsName[] = "Игнорир.ош.";
CPU_INT08U const DisableHopperErrorsList_str0[] = "нет";
CPU_INT08U const DisableHopperErrorsList_str1[] = "да";
CPU_INT08U const *DisableHopperErrorsList[] = {DisableHopperErrorsList_str0, DisableHopperErrorsList_str1};

TDataDescStruct const DisableHopperErrorsDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на десриптор индекса массива
  (void*)offsetof(TFramMap, DeviceConfig.hopperDisableErrors),            // указатель на переменную или адрес FRAM
  (void*)&DisableHopperErrorsRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  DisableHopperErrorsName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  DisableHopperErrorsList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

#ifdef CONFIG_FTP_CLIENT_ENABLE

/*************************************
  Включение FTP
*************************************/
TDataDescStruct const FtpEnableDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpEnable),            // указатель на переменную или адрес FRAM
  (void*)&EnableChannelRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  EnableChannelName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableChannelList,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};

char const FtpEnableDescId[] = "FtpEnableDesc";

/*************************************
  IP-адрес FTP-сервера 
*************************************/
CPU_INT08U const FtpServerIpAddrName[] = "FTP";

TDataDescStruct const FtpServerIpAddrDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_IP_ADDR,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpServerIpAddr),            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  NULL,                     // функция по изменению
  sizeof(CPU_INT32U),       // смещение между элементами в массиве
  FtpServerIpAddrName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0x5C35600A // "92.53.96.10"
};
char const FtpServerIpAddrDescId[] = "FtpServerIpAddrDesc";

/*************************************
  Логин FTP-сервера 
*************************************/
CPU_INT08U const FtpServerLoginName[] = "Логин";
CPU_INT08U const FtpServerLoginDefault[] = "xmiker_morozov";

TDataDescStruct const FtpServerLoginDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_CHAR_STRING,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  15,             // размер массива
  NULL,        // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpLogin),            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  FtpServerLoginName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  (CPU_INT32U)&FtpServerLoginDefault[0]
};
char const FtpServerLoginDescId[] = "FtpServerLoginDesc";

/*************************************
  Пароль FTP-сервера 
*************************************/
CPU_INT08U const FtpServerPassName[] = "Пароль";
CPU_INT08U const FtpServerPassDefault[] = "Qwerty11";

TDataDescStruct const FtpServerPassDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_CHAR_STRING,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  15,             // размер массива
  NULL,        // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpPass),            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  FtpServerPassName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  (CPU_INT32U)&FtpServerPassDefault[0]
};
char const FtpServerPassDescId[] = "FtpServerPassDesc";

/*************************************
  FTP идентификатор устройства
*************************************/
CPU_INT08U const FtpDeviceNumberName[] = "ID устр.";
TRangeValueULONG const FtpDeviceNumberRange = {0, 99999999};

TDataDescStruct const FtpDeviceNumberDesc = {
  DATA_DESC_EDIT,          // тип дескриптора
  DATA_TYPE_ULONG,         // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,           // признак массива
  0,                       // размер массива
  NULL,                       // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpDeviceNumber),            // указатель на переменную или адрес FRAM
  (void*)&FtpDeviceNumberRange,                    // указатель на границы параметра
  NULL,                    // функция по изменению
  sizeof(CPU_INT32U),                       // смещение между элементами в массиве
  FtpDeviceNumberName,        // указатель на строку названия параметра
  DATA_NO_INDEX,           // признак индексного параметра (список строк)
  NULL,        // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};
char const FtpDeviceNumberDescId[] = "FtpDeviceNumberDesc";

/*************************************
Время отправки статистики на FTP, час : мин
*************************************/
CPU_INT08U const FtpSendHourMinName[] = "Вр.отпр.";

TDataDescStruct const FtpSendHourMinDesc = {
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_HOUR_MIN,    // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  0,        // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpSendHourMin),            // указатель на переменную или адрес FRAM
  (void*)&StatSendHourRange,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  FtpSendHourMinName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  60 * 1
};
char const FtpSendHourMinDescId[] = "FtpSendHourMinDesc";

/*************************************
  Интервал отправки на FTP
*************************************/
CPU_INT08U const FtpSendIntervalName[] = "Интервал";
TRangeValueULONG const FtpSendIntervalRange = {0, 7};
CPU_INT08U const FtpSendInterval_str0[] = "1ч";
CPU_INT08U const FtpSendInterval_str1[] = "2ч";
CPU_INT08U const FtpSendInterval_str2[] = "3ч";
CPU_INT08U const FtpSendInterval_str3[] = "4ч";
CPU_INT08U const FtpSendInterval_str4[] = "6ч";
CPU_INT08U const FtpSendInterval_str5[] = "8ч";
CPU_INT08U const FtpSendInterval_str6[] = "12ч";
CPU_INT08U const FtpSendInterval_str7[] = "24ч";
CPU_INT08U const *FtpSendIntervalList[] = {FtpSendInterval_str0, FtpSendInterval_str1, FtpSendInterval_str2, FtpSendInterval_str3,
                                           FtpSendInterval_str4, FtpSendInterval_str5, FtpSendInterval_str6, FtpSendInterval_str7};

TDataDescStruct const FtpSendIntervalDesc = {
  DATA_DESC_EDIT,          // тип дескриптора
  DATA_TYPE_ULONG,         // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,           // признак массива
  0,                       // размер массива
  NULL,                       // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpSendIntervalIndex),            // указатель на переменную или адрес FRAM
  (void*)&FtpSendIntervalRange,                    // указатель на границы параметра
  NULL,                    // функция по изменению
  0,                       // смещение между элементами в массиве
  FtpSendIntervalName,        // указатель на строку названия параметра
  DATA_IS_INDEX,           // признак индексного параметра (список строк)
  FtpSendIntervalList,        // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  7                           
};
char const FtpSendIntervalDescId[] = "FtpSendIntervalDesc";

/*************************************
  Последнее время отправки на ftp
*************************************/
CPU_INT08U const FtpLastSendTimeName[] = "Посл.вр.";

TDataDescStruct const FtpLastSendTimeDesc = {
  DATA_DESC_VIEW,           // тип дескриптора
  DATA_TYPE_TIME,          // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpLastTime),            // указатель на переменную или адрес FRAM
  NULL,     // указатель на границы параметра
  NULL,                     // функция по изменению
  0,       // смещение между элементами в массиве
  FtpLastSendTimeName,       // указатель на строку названия параметра
  DATA_NO_INDEX,            // признак индексного параметра (список строк)
  NULL,                     // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  0                           
};
char const FtpLastSendTimeDescId[] = "FtpLastSendTimeDesc";

/*************************************
  Последний результат отправки на FTP
*************************************/
CPU_INT08U const FtpLastSendResultName[] = "Посл.статус";
TRangeValueULONG const FtpLastSendResultRange = {0, 1};
CPU_INT08U const FtpLastSendResult_str0[] = "ОШИБКА";
CPU_INT08U const FtpLastSendResult_str1[] = "ОК";
CPU_INT08U const FtpLastSendResult_str2[] = "НЕИЗВЕСТНО";
CPU_INT08U const *FtpLastSendResultList[] = {FtpLastSendResult_str0, FtpLastSendResult_str1, FtpLastSendResult_str2};

TDataDescStruct const FtpLastSendResultDesc = {
  DATA_DESC_VIEW,          // тип дескриптора
  DATA_TYPE_ULONG,         // тип параметра
  DATA_LOC_FRAM,            // расположение параметра
  DATA_NO_ARRAY,           // признак массива
  0,                       // размер массива
  NULL,                       // указатель на дескриптор индекса массива
  (void*)offsetof(TFramMap, FtpLastResult),            // указатель на переменную или адрес FRAM
  (void*)&FtpLastSendResultRange,                    // указатель на границы параметра
  NULL,                    // функция по изменению
  0,                       // смещение между элементами в массиве
  FtpLastSendResultName,        // указатель на строку названия параметра
  DATA_IS_INDEX,           // признак индексного параметра (список строк)
  FtpLastSendResultList,        // указатель на список строк для индексного параметра
  DATA_INIT_DISABLE,
  2
};
char const FtpLastSendResultDescId[] = "FtpLastSendResultDesc";

/*************************************
  Команда отправить данные на ftp сейчас 
*************************************/
CPU_INT08U const FtpSendNowCmdName[] = "Отправить";

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
  DATA_DESC_EDIT,           // тип дескриптора
  DATA_TYPE_ULONG,          // тип параметра
  DATA_LOC_RAM,            // расположение параметра
  DATA_NO_ARRAY,            // признак массива
  0,             // размер массива
  NULL,        // указатель на дескриптор индекса массива
  (void*)&ftp_send_cmd,            // указатель на переменную или адрес FRAM
  (void*)&EnableChannelRange,     // указатель на границы параметра
  OnChangeFtpSendNowCmd,                     // функция по изменению
  0,                        // смещение между элементами в массиве
  FtpSendNowCmdName,       // указатель на строку названия параметра
  DATA_IS_INDEX,            // признак индексного параметра (список строк)
  EnableChannelList,                     // указатель на список строк для индексного параметра
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


