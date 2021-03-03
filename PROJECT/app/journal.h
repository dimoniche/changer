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

    #define JOURNAL_EVENT_MONEY_BANK            16   // событие получения денег с банковского терминала (кол-во рублей)
    #define JOURNAL_EVENT_PRINT_BILL_ONLINE     17   // печать чека с деньгами с банковского терминала
    #define JOURNAL_EVENT_FTP_SEND          18       // событие отправки журнала

    #define EVENT_FULL_MONEY_HOPPER         19       // хоппер - заполнен
    #define JOURNAL_EVENT_COIN_OUT          20       // выдача жетона
  
    #define ERROR_HOPPER                    21       // ошибка хоппера
    #define ERROR_NO_MONEY_HOPPER           22       // ошибка хоппера - кончились деньги
  
    // ошибка связи с купюрником
    #define ERROR_VALIDATOR_CONN           23
    // критическая ошибка работы купюрника
    #define ERROR_VALIDATOR_FAILURE        24

    // некритические ошибки купюроприемника
    // Выброс купюры при замине
    #define ERROR_VALIDATOR_INSERTION       25
    // Выброс купюры по маг.датчику
    #define ERROR_VALIDATOR_MAGNETIC        26
    // Выброс купюры при транспортировке
    #define ERROR_VALIDATOR_CONVEYING       27
    // Выброс купюры по идентификации
    #define ERROR_VALIDATOR_IDENT           28
    // Выброс купюры по верификации 
    #define ERROR_VALIDATOR_VRFY            29
    // Выброс купюры по оптич.датчику 
    #define ERROR_VALIDATOR_OPT             30
    // Выброс купюры по запрету
    #define ERROR_VALIDATOR_INHIBIT         31
    // Выброс купюры по емкостному датчику
    #define ERROR_VALIDATOR_CAP             32
    // Выброс купюры по длине
    #define ERROR_VALIDATOR_LNG             33
    // Кассета заполнена
    #define ERROR_STACKER_FULL              34
    // Кассета отсутствует
    #define ERROR_STACKER_REMOVED           35
    // Замин в купюроприемнике
    #define ERROR_BV_JAMMED                 36
    // Замин в кассете
    #define ERROR_ST_JAMMED                 37
    // Попытка обмана
    #define ERROR_CHEATED                   38
    // Ошибка стекерного мотора
    #define ERROR_FLR_STACKER               39
    // Ошибка скорости транспорт.мотора
    #define ERROR_TR_SPEED                  40
    // Ошибка транспорт.мотора
    #define ERROR_FLR_TRANSPORT             41
    // Ошибка механизма выравнивания
    #define ERROR_FLR_ALIGNIN               42
    // Кассета отсутствует
    #define ERROR_FLR_INIT_CAS              43
    // Ошибка оптики
    #define ERROR_FLR_OPT                   44
    // Ошибка маг.датчика
    #define ERROR_FLR_MAG                   45
    // Ошибка емкостного датчика
    #define ERROR_FLR_CAP                   46

    // ошибка связи с модемом
    #define ERROR_MODEM_CONN                47
  
    // ошибка связи с фискальником
    #define ERROR_FR_CONN                   48

    // ВСЕ ОШИБКИ ФР ФАТАЛЬНЫЕ
    #define ERROR_FR                        49                    

    #define JOURNAL_EVENTS_COUNT            (ERROR_FR+FR_ERROR_NUMBER)          // число событий
 
  // канал
  CPU_INT08U channel;
  
  // данные: для получения денег - номинал купюры, для сеанса - длительность оплаченного времени, мин.
  CPU_INT16U data;

}TEventRecord;
#pragma pack(pop)

// структура для хранения счетчиков
typedef struct{
  
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
  
  CPU_INT32U  CounterCoinOut; // счетчик выданных жетонов
  CPU_INT32U  CounterCoin;    // счетчик полученных монет
  CPU_INT32U  CounterCash;    // счетчик полученных Банктнот
  CPU_INT32U  CounterAllCash; // счетчик полученных наличных
  CPU_INT32U  CounterBank;    // счетчик безналичных денег
  
}TCounters;

// структура для хранения длинных счетчиков
typedef struct{

  CPU_INT32U  CounterRunLong;
  CPU_INT32U  CounterTimeLong;
  CPU_INT32U  CounterMoneyLong;
  
  CPU_INT32U  CounterCoinOutLong; // счетчик выданных жетонов
  CPU_INT32U  CounterCoinLong;    // счетчик полученных монет
  CPU_INT32U  CounterCashLong;    // счетчик полученных банкнот
  CPU_INT32U  CounterAllCashLong; // счетчик полученных наличных
  CPU_INT32U  CounterBankLong;    // счетчик безналичных денег
  
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
extern void IncCounter(CPU_INT32U time, CPU_INT32U money);
extern void IncCounterBank(CPU_INT32U money);
extern void IncCounterCoinOut(CPU_INT32U money);
extern void IncCounterCoin(CPU_INT32U money);
extern void IncCounterCash(CPU_INT32U money);
extern void IncCounterAllCash(CPU_INT32U money);
extern void ClearCounters(void);
extern void ErrorServer(void);
extern int TstCriticalValidatorErrors(void);
extern void ClrValidatorErrors(void);
extern void PrintEventJournalRecordEng(char* str, TEventRecord *record);
extern void GetEventStrEng(char* str, char event);
extern void ClearBillnomCounter(void);

#endif //#ifndef _JOURNAL_H_
