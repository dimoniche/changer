#include <includes.h>
#include <stddef.h>
#include "journal.h"
#include "fram.h"
#include "fram_map.h"
#include "time.h"
#include "fr.h"
#include "crc16.h"
#include "mode.h"

static CPU_INT32U  GlobalErrorsFlags[JOURNAL_EVENTS_COUNT / 32 + 1] = {0};
static CPU_INT32U PrevFlags[JOURNAL_EVENTS_COUNT / 32 + 1] = {0};
    
void SetErrorFlag(CPU_INT08U error)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  GlobalErrorsFlags[error/32] |= (1L << (error%32));
  OS_EXIT_CRITICAL();
}

void ClrErrorFlag(CPU_INT08U error)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  GlobalErrorsFlags[error/32] &= ~(1L << (error%32));
  OS_EXIT_CRITICAL();
}

int TstErrorFlag(CPU_INT08U error)
{
  CPU_INT32U temp = 0;
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  temp = GlobalErrorsFlags[error/32] & (1L << (error%32));
  OS_EXIT_CRITICAL();
  return temp;
}

int TstCriticalErrors(void)
{
  CPU_INT32U errors = 0;
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  CPU_INT32U ignore_fiscal = 0;
  CPU_INT32U ignore_hopper = 0;

  GetData(&DisableFiscalErrorsDesc, &ignore_fiscal, 0, DATA_FLAG_SYSTEM_INDEX);     

  OS_ENTER_CRITICAL();
  
  if (!ignore_fiscal)
  {
    errors |= TstCriticalFiscalError();  
    errors |= TstErrorFlag(ERROR_FR_CONN);  
    /*
    if (!FReportTest())
    { // �� ���������� ��� ����� ������ 
      errors |= 0x1;
    }
    */
  }

  errors |= TstErrorFlag(ERROR_VALIDATOR_CONN);

  GetData(&DisableHopperErrorsDesc, &ignore_hopper, 0, DATA_FLAG_SYSTEM_INDEX);
  
  if (!ignore_hopper)
  {
    errors |= TstErrorFlag(ERROR_HOPPER);
    errors |= TstErrorFlag(ERROR_NO_MONEY_HOPPER);
  }
  
  OS_EXIT_CRITICAL();
  if (errors) return 1;
  return 0;
}


int TstCriticalValidatorErrors(void)
{
  return 0;
}

void ClrValidatorErrors(void)
{
  for (CPU_INT08U i=ERROR_VALIDATOR_FAILURE; i<ERROR_MODEM_CONN; ++i)
    {
      ClrErrorFlag(i);
    }
}

// �������� ������ �� ������� �������
int GetEventRecord(TEventRecord* record, CPU_INT32U index)
{
    if (index >= EVENT_RECORDS_COUNT) return -1;
    ReadArrayFram(offsetof(TFramMap, EventRecords[0])+index*sizeof(TEventRecord), sizeof(TEventRecord), (unsigned char*)record);
    return 0;
}

// ������ � ������ ������ � �������
void SaveEventRecord(CPU_INT08U channel, CPU_INT08U event, CPU_INT16U data)
{
    TEventRecord record;
    
    // ����� ����� ������ �� ������� ������ � �������� ����� ����� ��
    CPU_INT32U i, ind=0, indm = 0, maxtime = 0;
    for (i = 0; i < EVENT_RECORDS_COUNT; i++)
    {
        ReadArrayFram(offsetof(TFramMap, EventRecords[0])+i*sizeof(TEventRecord), sizeof(TEventRecord), (unsigned char*)&record);
        if ((record.time == 0) || (record.time == 0xffffffff)) {ind = i; break;}
        if (record.time >= maxtime) {maxtime = record.time; indm = i;}
    }
    
    if (i >= EVENT_RECORDS_COUNT) 
    {
        // ��� ������ �������� - ���������� �� ����� ����� ������
        ind = (indm + 1) % EVENT_RECORDS_COUNT;
    }
    
    record.time = GetTimeSec();
    record.channel = channel;
    record.event = event;
    record.data = data;
    WriteArrayFram(offsetof(TFramMap, EventRecords[0])+ind*sizeof(TEventRecord), sizeof(TEventRecord), (unsigned char*)&record);
}


void ClearEventJournal(void)
{
  SetArrayFram(offsetof(TFramMap, EventRecords), sizeof(TEventRecord)*EVENT_RECORDS_COUNT, 0x00);
}

void GetEventStr(char* str, char event)
{
  switch (event){
    case JOURNAL_EVENT_MONEY_NOTE:
      sprintf(str, "�������.������ ");
      break;
    case JOURNAL_EVENT_MONEY_COIN:
      sprintf(str, "�������.������ ");
      break;
    case JOURNAL_EVENT_START_SESSION:
      sprintf(str, "������ ���� ");
      break;
    case JOURNAL_EVENT_END_SESSION:
      sprintf(str, "���.������ ");
      break;
    case JOURNAL_EVENT_DEVICE_ON:
      sprintf(str, "���������");
      break;
    case JOURNAL_EVENT_PRINT_BILL:
      sprintf(str, "������ ����");
      break;
    case JOURNAL_EVENT_PRINT_Z:
      sprintf(str, "������ ������ � ����.��.");
      break;
    case JOURNAL_EVENT_PRINT_X:
      sprintf(str, "������ X-������");
      break;
    case JOURNAL_EVENT_PRINT_BUF:
      sprintf(str, "������ ���.�� ���.");
      break;
    case JOURNAL_EVENT_CHANGE_MODE:
      sprintf(str, "����� ������");
      break;
    case JOURNAL_EVENT_INCASSATION:
      sprintf(str, "����������");
      break;  
    case JOURNAL_EVENT_PASS_FAIL:
      sprintf(str, "�������� ������");
      break;  
    case JOURNAL_EVENT_EMAIL_FAIL:
      sprintf(str, "������ ����.e-mail");
      break;  
    case JOURNAL_EVENT_EMAIL_OK:
      sprintf(str, "E-mail ����.�������");
      break;
    case JOURNAL_EVENT_MONEY_BANK:
      sprintf(str, "������.������ ");
      break;
    case JOURNAL_EVENT_PRINT_BILL_ONLINE:
      sprintf(str, "������ ���� ������");
      break;
    case JOURNAL_EVENT_COIN_OUT:
      sprintf(str, "������");
      break;
    case JOURNAL_EVENT_FTP_SEND:
      strcpy(str, "�������� �� FTP");
      break;
    default:
      sprintf(str, "���");
      break;
  }
}

void GetEventStrEng(char* str, char event)
{
  switch (event){
    case JOURNAL_EVENT_MONEY_NOTE:
      sprintf(str, " |  Vnesena kupura ");
      break;
    case JOURNAL_EVENT_MONEY_COIN:
      sprintf(str, " |  Vneseny monety ");
      break;
    case JOURNAL_EVENT_START_SESSION:
      sprintf(str, " |  Print bill ");
      break;
    case JOURNAL_EVENT_END_SESSION:
      sprintf(str, " |  Kone� seansa ");
      break;
    case JOURNAL_EVENT_DEVICE_ON:
      sprintf(str, " |  Vkluchenie ");
      break;
    case JOURNAL_EVENT_PRINT_BILL:
      sprintf(str, " |  Pechat' checka ");
      break;
    case JOURNAL_EVENT_PRINT_Z:
      sprintf(str, " |  Pechat' Z-otcheta ");
      break;
    case JOURNAL_EVENT_PRINT_X:
      sprintf(str, " |  Pechat' X-otcheta ");
      break;
    case JOURNAL_EVENT_PRINT_BUF:
      sprintf(str, " |  Pechat' otcheta iz bufera ");
      break;
    case JOURNAL_EVENT_CHANGE_MODE:
      sprintf(str, " |  Smena rejima ");
      break;
    case JOURNAL_EVENT_INCASSATION:
      sprintf(str, " |  Incassaciya ");
      break;  
    case JOURNAL_EVENT_PASS_FAIL:
      sprintf(str, " |  Neverniy parol' ");
      break;  
    case JOURNAL_EVENT_EMAIL_FAIL:
      sprintf(str, " |  Oshibka otpravki e-mail ");
      break;  
    case JOURNAL_EVENT_EMAIL_OK:
      sprintf(str, " |  E-mail otpravleno uspeshno ");
      break; 
    case JOURNAL_EVENT_MONEY_BANK:
      sprintf(str, "��.������ ");
      break;
    case JOURNAL_EVENT_PRINT_BILL_ONLINE:
      sprintf(str, "������ ���� ������");
      break;
    default:
      sprintf(str, " |  Net sobytiya ");
      break;
  }
}

void PrintEventJournalRecordEng(char* str, TEventRecord *record)
{
  if (record->event)
    {
      TRTC_Data rtc_data;
      
      // ���������� ����� 
      Sec2Date(&rtc_data, record->time);
      sprintf(str, "|  ");
      PrintRTCDateTimeString(&str[strlen(str)], &rtc_data);
      // ���������� �������
      GetEventStrEng(&str[strlen(str)], record->event);
      
      // ���������� �������������� ����
      if ((record->event == JOURNAL_EVENT_MONEY_NOTE) || (record->event == JOURNAL_EVENT_MONEY_COIN))
        {
          sprintf(&str[strlen(str)], "kanal %d ", record->channel+1);
          sprintf(&str[strlen(str)], "%d rub.", record->data);
        }
      else if (record->event == JOURNAL_EVENT_START_SESSION)
        {
          sprintf(&str[strlen(str)], "");
          PrintSecToHourMinSec(&str[strlen(str)], record->data);
        }
      else if (record->event == JOURNAL_EVENT_END_SESSION)
        {
          sprintf(&str[strlen(str)], "kanal %d ", record->channel+1);
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_DEVICE_ON)
        {
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_BILL)
        {
          sprintf(&str[strlen(str)], " ");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_Z)
        {
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_X)
        {
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_BUF)
        {
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_CHANGE_MODE)
      {
          if (record->data == MODE_WORK) sprintf(&str[strlen(str)], "rabota");
          else sprintf(&str[strlen(str)], "nastroika");
      }
      else if (record->event == JOURNAL_EVENT_INCASSATION)
      {
          sprintf(&str[strlen(str)], "%u rub.", record->data);
      }
      else if (record->event == JOURNAL_EVENT_PASS_FAIL)
      {
          sprintf(&str[strlen(str)], "%u", record->data);
      }
      else if ((record->event == JOURNAL_EVENT_EMAIL_OK) || (record->event == JOURNAL_EVENT_EMAIL_FAIL))
      {
          sprintf(&str[strlen(str)], "");
      }
      sprintf(&str[strlen(str)], "\r\n");
    }
  else
    { // ������ ������
      sprintf(str, "net zapisi\r\n");
    }
}

void IncCounter(CPU_INT32U time, CPU_INT32U money)
{
  CPU_INT32U r, t, m;
  TCountersLong long_ctrs;
  
  // �������� ����� ��������
  ReadArrayFram(offsetof(TFramMap, Counters.CounterRun), sizeof(CPU_INT32U), (unsigned char*)&r);
  ReadArrayFram(offsetof(TFramMap, Counters.CounterTime), sizeof(CPU_INT32U), (unsigned char*)&t);
  ReadArrayFram(offsetof(TFramMap, Counters.CounterMoney), sizeof(CPU_INT32U), (unsigned char*)&m);
  r++;
  t+=time;
  m+=money;
  WriteArrayFram(offsetof(TFramMap, Counters.CounterRun), sizeof(CPU_INT32U), (unsigned char*)&r);
  WriteArrayFram(offsetof(TFramMap, Counters.CounterTime), sizeof(CPU_INT32U), (unsigned char*)&t);
  WriteArrayFram(offsetof(TFramMap, Counters.CounterMoney), sizeof(CPU_INT32U), (unsigned char*)&m);

  // �������� ������� ��������
  ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
  long_ctrs.CounterRunLong++;
  long_ctrs.CounterTimeLong += time;
  long_ctrs.CounterMoneyLong += money;
  long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
  WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
}

void IncCounterBank(CPU_INT32U money)
{
  CPU_INT32U m;
  TCountersLong long_ctrs;
  
  // �������� ���������� ����� ���������� ����� ����
  ReadArrayFram(offsetof(TFramMap, Counters.CounterBank), sizeof(CPU_INT32U), (unsigned char*)&m);
  m+=money;
  WriteArrayFram(offsetof(TFramMap, Counters.CounterBank), sizeof(CPU_INT32U), (unsigned char*)&m);

  // �������� ������� ��������
  ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
  long_ctrs.CounterBankLong += money;
  long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
  WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
}

void IncCounterCoinOut(CPU_INT32U money)
{
  CPU_INT32U m;
  TCountersLong long_ctrs;
  
  // �������� ���������� �������� �������
  ReadArrayFram(offsetof(TFramMap, Counters.CounterCoinOut), sizeof(CPU_INT32U), (unsigned char*)&m);
  m+=money;
  WriteArrayFram(offsetof(TFramMap, Counters.CounterCoinOut), sizeof(CPU_INT32U), (unsigned char*)&m);

  // �������� ������� ��������
  ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
  long_ctrs.CounterCoinOutLong += money;
  long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
  WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
}

void IncCounterCoin(CPU_INT32U money)
{
  CPU_INT32U m;
  TCountersLong long_ctrs;
  
  // �������� ���������� ����� ���������� ����� ��������
  ReadArrayFram(offsetof(TFramMap, Counters.CounterCoin), sizeof(CPU_INT32U), (unsigned char*)&m);
  m+=money;
  WriteArrayFram(offsetof(TFramMap, Counters.CounterCoin), sizeof(CPU_INT32U), (unsigned char*)&m);

  // �������� ������� ��������
  ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
  long_ctrs.CounterCoinLong += money;
  long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
  WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
}

void IncCounterCash(CPU_INT32U money)
{
  CPU_INT32U m;
  TCountersLong long_ctrs;
  
  // �������� ���������� ����� ���������� ����� ��������
  ReadArrayFram(offsetof(TFramMap, Counters.CounterCash), sizeof(CPU_INT32U), (unsigned char*)&m);
  m+=money;
  WriteArrayFram(offsetof(TFramMap, Counters.CounterCash), sizeof(CPU_INT32U), (unsigned char*)&m);

  // �������� ������� ��������
  ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
  long_ctrs.CounterCashLong += money;
  long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
  WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
}

void IncCounterAllCash(CPU_INT32U money)
{
  CPU_INT32U m;
  TCountersLong long_ctrs;
  
  // �������� ���������� ����� ���������� ����� ��������
  ReadArrayFram(offsetof(TFramMap, Counters.CounterAllCash), sizeof(CPU_INT32U), (unsigned char*)&m);
  m+=money;
  WriteArrayFram(offsetof(TFramMap, Counters.CounterAllCash), sizeof(CPU_INT32U), (unsigned char*)&m);

  // �������� ������� ��������
  ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
  long_ctrs.CounterAllCashLong += money;
  long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
  WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
}

CPU_INT32U GetShortMoney()
{
  CPU_INT32U money;
  ReadArrayFram(offsetof(TFramMap, Counters.CounterMoney), sizeof(CPU_INT32U), (unsigned char*)&money);
  return money;
}

void CheckLongCounters(void)
{
    TCountersLong long_ctrs;
    CPU_INT16U crc;
    ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
    crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
    if (crc != long_ctrs.crc)
    {
        memset(&long_ctrs, 0, sizeof(TCountersLong));
        long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
        WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);    
        /// ������� �������� ���� �������
        ClearCounters();
        ClearBillnomCounter();
    }
}

void ClearCounters(void)
{
  SetArrayFram(offsetof(TFramMap, Counters), sizeof(CPU_INT32U)*3, 0x00);
  SetArrayFram(offsetof(TFramMap, Counters.CounterCoinOut), sizeof(CPU_INT32U)*5, 0x00);
}

/// ��������� �������� ����� �� ���������
void IncBillnomCounter(CPU_INT32U index)
{
    CPU_INT32U counter;
    if (index >= 24) return;
    ReadArrayFram(offsetof(TFramMap, Counters.CounterBillNominals)+sizeof(CPU_INT32U)*index, sizeof(CPU_INT32U), (unsigned char*)&counter);
    counter++;
    WriteArrayFram(offsetof(TFramMap, Counters.CounterBillNominals)+sizeof(CPU_INT32U)*index, sizeof(CPU_INT32U), (unsigned char*)&counter);
    
    ReadArrayFram(offsetof(TFramMap, Counters.BillsCount), sizeof(CPU_INT32U), (unsigned char*)&counter);
    counter++;
    WriteArrayFram(offsetof(TFramMap, Counters.BillsCount), sizeof(CPU_INT32U), (unsigned char*)&counter);
}

/// ������� ��������� �����
void ClearBillnomCounter(void)
{
    CPU_INT32U counter = 0;
    CPU_INT32U i;
    
    for (i = 0; i < 24; i++)
    {
        WriteArrayFram(offsetof(TFramMap, Counters.CounterBillNominals)+sizeof(CPU_INT32U)*i, sizeof(CPU_INT32U), (unsigned char*)&counter);    
    }
    
    WriteArrayFram(offsetof(TFramMap, Counters.BillsCount), sizeof(CPU_INT32U), (unsigned char*)&counter);
}

// ������ ������ (��������� ������ � ������)
void ErrorServer(void)
{
    for (int i = ERROR_HOPPER; i < JOURNAL_EVENTS_COUNT; i++)
    {
        if (!(PrevFlags[i/32] & (1L<<(i%32)))
            && (TstErrorFlag(i)))
        {
            // ������� � ������
            SaveEventRecord(0, i, 0);
        }
    }
    
    memcpy(PrevFlags, GlobalErrorsFlags, sizeof(CPU_INT32U) * (JOURNAL_EVENTS_COUNT / 32 + 1));
}

