#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include "control.h"
#include "fiscal.h"

#define EVENT_RECORDS_COUNT  512 // ����� ������� � �������

#pragma pack(push, 1)
/// ��������� ������ ������� ������� �������
typedef struct{
  // ����� �������� ������
  CPU_INT32U time;
  
  // ��� �������
  CPU_INT08U event;
    #define JOURNAL_EVENT_NO_EVENT         0  // ��� �������
    #define JOURNAL_EVENT_MONEY_NOTE       1  // ������� ��������� ������
    #define JOURNAL_EVENT_MONEY_COIN       2  // ������� ��������� ������ (���-�� ������)
    #define JOURNAL_EVENT_START_SESSION    3  // ������� ������ ������
    #define JOURNAL_EVENT_END_SESSION      4  // ������� ����� ������
    #define JOURNAL_EVENT_DEVICE_ON        6  // ��������� ����������
    #define JOURNAL_EVENT_PRINT_BILL       7  // ������ ����
    #define JOURNAL_EVENT_PRINT_Z          8  // ������ z-������
    #define JOURNAL_EVENT_PRINT_X          9  // ������ x-������
    #define JOURNAL_EVENT_PRINT_BUF        10  // ������ x-������
    #define JOURNAL_EVENT_CHANGE_MODE      11  // ����� ������
    #define JOURNAL_EVENT_INCASSATION      12  // ���������� 
    #define JOURNAL_EVENT_PASS_FAIL        13  // �������� ���� ������
    #define JOURNAL_EVENT_EMAIL_OK         14  // ��������� ��������� email
    #define JOURNAL_EVENT_EMAIL_FAIL       15  // ������ ��� �������� email

    // ������ ����� � ����������
    #define ERROR_VALIDATOR_CONN           16
    // ����������� ������ ������ ���������
    #define ERROR_VALIDATOR_FAILURE        17

    // ������������� ������ ���������������
    // ������ ������ ��� ������
    #define ERROR_VALIDATOR_INSERTION       18
    // ������ ������ �� ���.�������
    #define ERROR_VALIDATOR_MAGNETIC        19
    // ������ ������ ��� ���������������
    #define ERROR_VALIDATOR_CONVEYING       20
    // ������ ������ �� �������������
    #define ERROR_VALIDATOR_IDENT           21
    // ������ ������ �� ����������� 
    #define ERROR_VALIDATOR_VRFY            22
    // ������ ������ �� �����.������� 
    #define ERROR_VALIDATOR_OPT             23
    // ������ ������ �� �������
    #define ERROR_VALIDATOR_INHIBIT         24
    // ������ ������ �� ���������� �������
    #define ERROR_VALIDATOR_CAP             25
    // ������ ������ �� �����
    #define ERROR_VALIDATOR_LNG             26
    // ������� ���������
    #define ERROR_STACKER_FULL              27
    // ������� �����������
    #define ERROR_STACKER_REMOVED           28
    // ����� � ���������������
    #define ERROR_BV_JAMMED                 29
    // ����� � �������
    #define ERROR_ST_JAMMED                 30
    // ������� ������
    #define ERROR_CHEATED                   31
    // ������ ���������� ������
    #define ERROR_FLR_STACKER               32
    // ������ �������� ���������.������
    #define ERROR_TR_SPEED                  33
    // ������ ���������.������
    #define ERROR_FLR_TRANSPORT             34
    // ������ ��������� ������������
    #define ERROR_FLR_ALIGNIN               35
    // ������� �����������
    #define ERROR_FLR_INIT_CAS              36
    // ������ ������
    #define ERROR_FLR_OPT                   37
    // ������ ���.�������
    #define ERROR_FLR_MAG                   38
    // ������ ���������� �������
    #define ERROR_FLR_CAP                   39

    // ������ ����� � �������
    #define ERROR_MODEM_CONN                40
  
    // ������ ����� � ������������
    #define ERROR_FR_CONN                   41

    // ��� ������ �� ���������
    #define ERROR_FR                        42
  
    #define JOURNAL_EVENTS_COUNT             (ERROR_FR+FR_ERROR_NUMBER)  // ����� �������

  // �����
  CPU_INT08U channel;
  
  // ������: ��� ��������� ����� - ������� ������, ��� ������ - ������������ ����������� �������, ���.
  CPU_INT16U data;

}TEventRecord;
#pragma pack(pop)

// ��������� ��� �������� ���������
typedef struct{
  // ����� �������� ����������
  CPU_INT32U  CounterChannelRun[CHANNELS_NUM];
  // ��������� ����� ������ ����������, ���.
  CPU_INT32U  CounterChannelTime[CHANNELS_NUM];
  // ����� ����� ����������  
  CPU_INT32U  CounterChannelMoney[CHANNELS_NUM];
  
  // ����� ����� �������� 
  CPU_INT32U  CounterRun;
  // ����� ��������� ����� ������, ���.
  CPU_INT32U  CounterTime;
  // ����� ����� �����   
  CPU_INT32U  CounterMoney;
  
  // �������� ����� � ��������� �� ���������
  CPU_INT32U  CounterBillNominals[24];
  // ����� ������� ����� (����� � �������)
  CPU_INT32U  BillsCount;
}TCounters;


// ��������� ��� �������� ������� ���������
// ����� ���� ������ ��� ��� �������
typedef struct{
  // ����� �������� ����������
  CPU_INT32U  CounterChannelRunLong[CHANNELS_NUM];
  // ��������� ����� ������ ����������, ���.
  CPU_INT32U  CounterChannelTimeLong[CHANNELS_NUM];
  // ����� ����� ����������  
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
