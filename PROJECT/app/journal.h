#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include "control.h"
#include "fiscal.h"

#define EVENT_RECORDS_COUNT  512 // число записей в журнале

#pragma pack(push, 1)
/// структура записи журнала рабочих событий
typedef struct{
  // время создания записи
  CPU_INT32U time;
  
  // тип события
  CPU_INT08U event;
    #define JOURNAL_EVENT_NO_EVENT         0  // нет события
    #define JOURNAL_EVENT_MONEY_NOTE       1  // событие получения купюры
    #define JOURNAL_EVENT_MONEY_COIN       2  // событие получения монеты (кол-во рублей)
    #define JOURNAL_EVENT_START_SESSION    3  // событие начала сеанса
    #define JOURNAL_EVENT_END_SESSION      4  // событие конца сеанса
    #define JOURNAL_EVENT_DEVICE_ON        6  // включение устройства
    #define JOURNAL_EVENT_PRINT_BILL       7  // печать чека
    #define JOURNAL_EVENT_PRINT_Z          8  // печать z-отчета
    #define JOURNAL_EVENT_PRINT_X          9  // печать x-отчета
    #define JOURNAL_EVENT_PRINT_BUF        10  // печать x-отчета
    #define JOURNAL_EVENT_CHANGE_MODE      11  // смена режима
    #define JOURNAL_EVENT_INCASSATION      12  // инкассация 
    #define JOURNAL_EVENT_PASS_FAIL        13  // неверный ввод пароля
    #define JOURNAL_EVENT_EMAIL_OK         14  // правильно отправлен email
    #define JOURNAL_EVENT_EMAIL_FAIL       15  // ошибка при отправке email

    // ошибка связи с купюрником
    #define ERROR_VALIDATOR_CONN           16
    // критическая ошибка работы купюрника
    #define ERROR_VALIDATOR_FAILURE        17

    // некритические ошибки купюроприемника
    // Выброс купюры при замине
    #define ERROR_VALIDATOR_INSERTION       18
    // Выброс купюры по маг.датчику
    #define ERROR_VALIDATOR_MAGNETIC        19
    // Выброс купюры при транспортировке
    #define ERROR_VALIDATOR_CONVEYING       20
    // Выброс купюры по идентификации
    #define ERROR_VALIDATOR_IDENT           21
    // Выброс купюры по верификации 
    #define ERROR_VALIDATOR_VRFY            22
    // Выброс купюры по оптич.датчику 
    #define ERROR_VALIDATOR_OPT             23
    // Выброс купюры по запрету
    #define ERROR_VALIDATOR_INHIBIT         24
    // Выброс купюры по емкостному датчику
    #define ERROR_VALIDATOR_CAP             25
    // Выброс купюры по длине
    #define ERROR_VALIDATOR_LNG             26
    // Кассета заполнена
    #define ERROR_STACKER_FULL              27
    // Кассета отсутствует
    #define ERROR_STACKER_REMOVED           28
    // Замин в купюроприемнике
    #define ERROR_BV_JAMMED                 29
    // Замин в кассете
    #define ERROR_ST_JAMMED                 30
    // Попытка обмана
    #define ERROR_CHEATED                   31
    // Ошибка стекерного мотора
    #define ERROR_FLR_STACKER               32
    // Ошибка скорости транспорт.мотора
    #define ERROR_TR_SPEED                  33
    // Ошибка транспорт.мотора
    #define ERROR_FLR_TRANSPORT             34
    // Ошибка механизма выравнивания
    #define ERROR_FLR_ALIGNIN               35
    // Кассета отсутствует
    #define ERROR_FLR_INIT_CAS              36
    // Ошибка оптики
    #define ERROR_FLR_OPT                   37
    // Ошибка маг.датчика
    #define ERROR_FLR_MAG                   38
    // Ошибка емкостного датчика
    #define ERROR_FLR_CAP                   39

    // ошибка связи с модемом
    #define ERROR_MODEM_CONN                40
  
    // ошибка связи с фискальником
    #define ERROR_FR_CONN                   41

    // ВСЕ ОШИБКИ ФР ФАТАЛЬНЫЕ
    #define ERROR_FR                        42
  
    #define JOURNAL_EVENTS_COUNT             (ERROR_FR+FR_ERROR_NUMBER)  // число событий

  // канал
  CPU_INT08U channel;
  
  // данные: для получения денег - номинал купюры, для сеанса - длительность оплаченного времени, мин.
  CPU_INT16U data;

}TEventRecord;
#pragma pack(pop)

// структура для хранения счетчиков
typedef struct{
  // число запусков поканально
  CPU_INT32U  CounterChannelRun[CHANNELS_NUM];
  // Суммарное время работы поканально, сек.
  CPU_INT32U  CounterChannelTime[CHANNELS_NUM];
  // Сумма денег поканально  
  CPU_INT32U  CounterChannelMoney[CHANNELS_NUM];
  
  // общее число запусков 
  CPU_INT32U  CounterRun;
  // общее Суммарное время работы, сек.
  CPU_INT32U  CounterTime;
  // общее Сумма денег   
  CPU_INT32U  CounterMoney;
  
  // счетчики купюр в купюрнике по номиналам
  CPU_INT32U  CounterBillNominals[24];
  // общий счетчик купюр (всего в кассете)
  CPU_INT32U  BillsCount;
}TCounters;


// структура для хранения длинных счетчиков
// ведем пока только эти три длинных
typedef struct{
  // число запусков поканально
  CPU_INT32U  CounterChannelRunLong[CHANNELS_NUM];
  // Суммарное время работы поканально, сек.
  CPU_INT32U  CounterChannelTimeLong[CHANNELS_NUM];
  // Сумма денег поканально  
  CPU_INT32U  CounterChannelMoneyLong[CHANNELS_NUM];
  CPU_INT32U  CounterRunLong;
  CPU_INT32U  CounterTimeLong;
  CPU_INT32U  CounterMoneyLong;
  CPU_INT16U  crc;
}TCountersLong;


extern CPU_INT32U GetShortMoney();
extern void IncBillnomCounter(CPU_INT32U index);
extern void CheckLongCounters(void);
extern void SaveEventRecord(CPU_INT08U channel, CPU_INT08U event, CPU_INT16U data);
extern void SetErrorFlag(CPU_INT08U error);
extern void ClrErrorFlag(CPU_INT08U error);
extern int TstErrorFlag(CPU_INT08U error);
extern int TstCriticalErrors(void);
extern void ClearEventJournal(void);
extern void GetEventStr(char* str, char event);
extern int GetEventRecord(TEventRecord* record, CPU_INT32U index);
extern void IncCounter(CPU_INT08U ch, CPU_INT32U time, CPU_INT32U money);
extern void ClearCounters(void);
extern void ErrorServer(void);
extern int TstCriticalValidatorErrors(void);
extern void ClrValidatorErrors(void);
extern void PrintEventJournalRecordEng(char* str, TEventRecord *record);
extern void GetEventStrEng(char* str, char event);
extern void ClearBillnomCounter(void);

#endif //#ifndef _JOURNAL_H_
