#include <includes.h>
#include "journal.h"
#include "modem.h"
#include "modem_task.h"
#include "data.h"
#include "datadesc.h"
#include "time.h"

OS_STK    ModemTaskStk[MODEM_TASK_STK_SIZE];
OS_EVENT *ModemQuery = NULL;
void     *ModemTbl[MODEM_QUERY_LEN];

static int index;
static CPU_INT32U enable_journals;

#define STAT_STR_NUM    17

static void GetChannelStatStr(char* str, int ch)
{
    CPU_INT32U val;

    GetData(&CounterChannelMoneyDesc, &val, ch, DATA_FLAG_DIRECT_INDEX);
    sprintf(&str[strlen(str)], "|  %2d   | %11d ", ch+1, val);
    
    GetData(&CounterChannelRunDesc, &val, ch, DATA_FLAG_DIRECT_INDEX);
    sprintf(&str[strlen(str)], "| %7d | ", val);
    
    GetData(&CounterChannelTimeDesc, &val, ch, DATA_FLAG_DIRECT_INDEX);
    PrintSecToHourMinSec(&str[strlen(str)], val);
    
    sprintf(&str[strlen(str)], "\r\n");
}   

static void GetChannelStatStrLong(char* str, int ch)
{
    CPU_INT32U val;

    GetData(&CounterChannelMoneyLongDesc, &val, ch, DATA_FLAG_DIRECT_INDEX);
    sprintf(&str[strlen(str)], "|  %2d   | %11d ", ch+1, val);
    
    GetData(&CounterChannelRunLongDesc, &val, ch, DATA_FLAG_DIRECT_INDEX);
    sprintf(&str[strlen(str)], "| %7d | ", val);
    
    GetData(&CounterChannelTimeLongDesc, &val, ch, DATA_FLAG_DIRECT_INDEX);
    PrintSecToHourMinSec(&str[strlen(str)], val);
    
    sprintf(&str[strlen(str)], "\r\n");
}   

extern CPU_INT32U BillNominals[24];

static int GetEmailStr(char *str)
{    
    if (index < STAT_STR_NUM)
    {
        str[0] = 0;
        if (index == 0)
        {
            // заголовок 
            TRTC_Data rtc;
            RTC_ReadTime(&rtc);
            sprintf(str, "Systemnoe vremya: ");
            PrintRTCDateTimeString(&str[strlen(str)], &rtc);
            sprintf(&str[strlen(str)], "\r\n\r\n-------------------------------------------------------------\r\nStatistika obshaya. Korotkie schetchiki.\r\n-------------------------------------------------------------\r\n");
        }
        else if (index == 1)
        {
            CPU_INT32U val;
            // общие счетчики
            sprintf(str, "| Vsego deneg, rub. | Vsego seansov | Vsego narabotka h:m:s \r\n");
            
            GetData(&CounterMoneyDesc, &val, 0, DATA_FLAG_SYSTEM_INDEX);
            sprintf(&str[strlen(str)], "| %16d  ", val);
            
            GetData(&CounterRunDesc, &val, 0, DATA_FLAG_SYSTEM_INDEX);
            sprintf(&str[strlen(str)], "| %12d  | ", val);
            
            GetData(&CounterTimeDesc, &val, 0, DATA_FLAG_SYSTEM_INDEX);
            PrintSecToHourMinSec(&str[strlen(str)], val);
            
            sprintf(&str[strlen(str)], "\r\n");
        }
        else if (index == 2)
        {
            sprintf(&str[strlen(str)], "\r\n-------------------------------------------------------------\r\nStatistika obshaya. Dlinnye schetchiki\r\n-------------------------------------------------------------\r\n");
        }
        else if (index == 3)
        {
            CPU_INT32U val;
            // необнуляемые счетчики
            sprintf(str, "| Vsego deneg, rub. | Vsego seansov | Vsego narabotka h:m:s \r\n");
            
            GetData(&CounterLongMoneyDesc, &val, 0, DATA_FLAG_SYSTEM_INDEX);
            sprintf(&str[strlen(str)], "| %16d  ", val);
            
            GetData(&CounterLongRunDesc, &val, 0, DATA_FLAG_SYSTEM_INDEX);
            sprintf(&str[strlen(str)], "| %12d  | ", val);
            
            GetData(&CounterLongTimeDesc, &val, 0, DATA_FLAG_SYSTEM_INDEX);
            PrintSecToHourMinSec(&str[strlen(str)], val);
            
            sprintf(&str[strlen(str)], "\r\n");
            
        }
        else if (index == 4)
        {
            sprintf(str, "\r\n-------------------------------------------------------------\r\nStatistika po kanalam. Korotkie schetchiki.\r\n-------------------------------------------------------------\r\n");
            sprintf(&str[strlen(str)], "| Kanal | Deneg, rub. | Seansov | Narabotka h:m:s\r\n");
        }
        else if (index == 5)
        {
            int i;
            for (i = 0; i < 3; i++)
            {
                GetChannelStatStr(&str[strlen(str)], i);
            }
        }
        else if (index == 6)
        {
            int i;
            for (i = 3; i < 6; i++)
            {
                GetChannelStatStr(&str[strlen(str)], i);
            }
        }
        else if (index == 7)
        {
            int i;
            for (i = 6; i < 10; i++)
            {
                GetChannelStatStr(&str[strlen(str)], i);
            }
        }
        else if (index == 8)
        {
            sprintf(str, "\r\n-------------------------------------------------------------\r\nStatistika po kanalam. Dlinnye schetchiki.\r\n-------------------------------------------------------------\r\n");
            sprintf(&str[strlen(str)], "| Kanal | Deneg, rub. | Seansov | Narabotka h:m:s\r\n");
        }
        else if (index == 9)
        {
            int i;
            for (i = 0; i < 3; i++)
            {
                GetChannelStatStrLong(&str[strlen(str)], i);
            }
        }
        else if (index == 10)
        {
            int i;
            for (i = 3; i < 6; i++)
            {
                GetChannelStatStrLong(&str[strlen(str)], i);
            }
        }
        else if (index == 11)
        {
            int i;
            for (i = 6; i < 10; i++)
            {
                GetChannelStatStrLong(&str[strlen(str)], i);
            }
        }
        else if (index == 12)
        {
            CPU_INT32U val;
            // впечатаем число купюр в купюрнике
            sprintf(str, "\r\n-------------------------------------------------------------\r\nKupuropriemnik.\r\n-------------------------------------------------------------\r\n");
            GetData(&BillCounterDesc, &val, 0, DATA_FLAG_SYSTEM_INDEX);
            if (!val)
            {
              // купюрник пуст
              sprintf(&str[strlen(str)], "Kupuropriemnik pust.\r\n\r\n");
              index++;
              index++;
            }
            else
            {
              sprintf(&str[strlen(str)], "|      Nominal, rub.     |     Kolichestvo\r\n");            
            }
        }
        else if (index == 13)
        {
          int i;
          str[0] = 0;
          for (i = 0; i < 6; i++)
          {
            CPU_INT32U val;
            GetData(&BillnomCountersDesc, &val, i, DATA_FLAG_DIRECT_INDEX);
            if (val)
            {
              sprintf(&str[strlen(str)], "|      %13d     |     %11d      \r\n", BillNominals[i], val);
            }
            val = strlen(str);
          }
        }
        else if (index == 14)
        {
          int i;
          str[0] = 0;
          for (i = 6; i < 12; i++)
          {
            CPU_INT32U val;
            GetData(&BillnomCountersDesc, &val, i, DATA_FLAG_DIRECT_INDEX);
            if (val)
            {
              sprintf(&str[strlen(str)], "|      %13d     |     %11d      \r\n", BillNominals[i], val);
            }
          }
        }
        else if (index == 15)
        {
          int i;
          str[0] = 0;
          for (i = 12; i < 18; i++)
          {
            CPU_INT32U val;
            GetData(&BillnomCountersDesc, &val, i, DATA_FLAG_DIRECT_INDEX);
            if (val)
            {
              sprintf(&str[strlen(str)], "|      %13d     |     %11d      \r\n", BillNominals[i], val);
            }
          }
        }
        else if (index == 16)
        {
          int i;
          str[0] = 0;
          for (i = 18; i < 24; i++)
          {
            CPU_INT32U val;
            GetData(&BillnomCountersDesc, &val, i, DATA_FLAG_DIRECT_INDEX);
            if (val)
            {
              sprintf(&str[strlen(str)], "|      %13d     |     %11d      \r\n", BillNominals[i], val);
            }
          }
        }
        index++;
        return 0;
    }
    else if (index < STAT_STR_NUM+EVENT_RECORDS_COUNT)
	{
        TEventRecord record;
        
        if (enable_journals == 0) return -1;
        
        GetEventRecord(&record, index-STAT_STR_NUM);
        
          
        str[0] = 0;
        if ((index-STAT_STR_NUM) == 0)
        {
            sprintf(str, "\r\n-------------------------------------------------------------\r\nZhurnal sobytiy\r\n-------------------------------------------------------------\r\n");
        }          
        if (record.time == 0x00000000)
        {
            index++;
            return 0;
        }
        PrintEventJournalRecordEng(&str[strlen(str)], &record);
        index++;
        return 0;
    }
    
    return -1;
}

/// отправка e-mail со счетчиками и журналами
int SendStatistics(void)
{
    CPU_INT32U dev_id;
    char theme[48];
    index = 0;
    // надо ли отправлять журналы
    GetData(&EnableEmailJournalSendDesc, &enable_journals, 0, DATA_FLAG_SYSTEM_INDEX);
    // id устройства укажем в теме
    GetData(&DeviceIDDesc, &dev_id, 0, DATA_FLAG_SYSTEM_INDEX);
    sprintf(theme, "Report from solarium device id %d.", dev_id);
    return ModemSendEmail(theme, GetEmailStr);
}

// выемка нового задания для модема
static int GetModemTask(int* event)
{
    CPU_INT08U err = 0; 
    int evt  = (int)OSQPend(ModemQuery, 1, &err);
    if (err != 0) return 0;
    *event = evt;
    return 1;  
}   

int GetTestText(char *str)
{    
    if (index == 0)
    {
        index++; 
        sprintf(str, "Test message from solarium.");
        return 0;
    }
    
    return -1;
}
// отправка тестового сообщения
int SendTest(void)
{
    CPU_INT32U dev_id;
    char theme[48];
    
    GetData(&DeviceIDDesc, &dev_id, 0, DATA_FLAG_SYSTEM_INDEX);
    sprintf(theme, "Test message. Device id %d.", dev_id);
    
    index = 0;
    return ModemSendEmail(theme, GetTestText);
}


static CPU_INT32U incas_money;
static CPU_INT32U incas_time;


int GetIncasText(char *str)
{    
    if (index == 0)
    {
        // заголовок 
        TRTC_Data rtc;
        Sec2Date(&rtc, incas_time);
        sprintf(str, "Vremya incassacii: ");
        PrintRTCDateTimeString(&str[strlen(str)], &rtc);
        sprintf(&str[strlen(str)], "\r\n\r\nSumma %d rub.\r\n", incas_money);
    }
    else if (index == 1)
    {
        CPU_INT32U val;
        // впечатаем число купюр в купюрнике
        val = incas_common_bill_counter;
        if (!val)
        {
          // купюрник пуст
          sprintf(&str[strlen(str)], "Kupuropriemnik byl pust.\r\n\r\n");
          index=100;
        }
        else
        {
          sprintf(str, "\r\n-------------------------------------------------------------\r\nKupuropriemnik.\r\n-------------------------------------------------------------\r\n");
          sprintf(&str[strlen(str)], "|      Nominal, rub.     |     Kolichestvo\r\n");            
        }
    }
    else if (index == 2)
    {
      int i;
      str[0] = 0;
      for (i = 0; i < 6; i++)
      {
        CPU_INT32U val;
        val = incas_bill_nom_counter[i];
        if (val)
        {
          sprintf(&str[strlen(str)], "|      %13d     |     %11d      \r\n", BillNominals[i], val);
        }
        val = strlen(str);
      }
    }
    else if (index == 3)
    {
      int i;
      str[0] = 0;
      for (i = 6; i < 12; i++)
      {
        CPU_INT32U val;
        val = incas_bill_nom_counter[i];
        if (val)
        {
          sprintf(&str[strlen(str)], "|      %13d     |     %11d      \r\n", BillNominals[i], val);
        }
      }
    }
    else if (index == 4)
    {
      int i;
      str[0] = 0;
      for (i = 12; i < 18; i++)
      {
        CPU_INT32U val;
        val = incas_bill_nom_counter[i];
        if (val)
        {
          sprintf(&str[strlen(str)], "|      %13d     |     %11d      \r\n", BillNominals[i], val);
        }
      }
    }
    else if (index == 5)
    {
      int i;
      str[0] = 0;
      for (i = 18; i < 24; i++)
      {
        CPU_INT32U val;
        val = incas_bill_nom_counter[i];
        if (val)
        {
          sprintf(&str[strlen(str)], "|      %13d     |     %11d      \r\n", BillNominals[i], val);
        }
      }
    }
   else
   {
        return -1;
   }
      
   index++;
   return 0;    
}

// отправка сообщения об инкассации
int SendIncas(void)
{
    CPU_INT32U dev_id;
    char theme[48];
    
    GetData(&DeviceIDDesc, &dev_id, 0, DATA_FLAG_SYSTEM_INDEX);
    sprintf(theme, "Incassation. Device id %d.", dev_id);
    
    index = 0;
    return ModemSendEmail(theme, GetIncasText);
}



// список возможных интервалов отправки
const int send_period_list[7] = {0, 1, 2, 4, 8, 12, 24};
// глобальный статус модема
CPU_INT32U modem_status = 0;

// проверка, в одном ли периоде находятся два времени
int SamePeriod(CPU_INT32U time1, CPU_INT32U time2, CPU_INT08U period)
{
    TRTC_Data rtc1, rtc2;
    CPU_INT08U hour1, hour2;
    
    Sec2Date(&rtc1, time1);
    Sec2Date(&rtc2, time2);
    
    if ((rtc1.year != rtc2.year) || (rtc1.mon != rtc2.mon) || (rtc1.date != rtc2.date))
    {
        // дата не совпадает - точно разный часовой период
        return 0;
    }

    // time1 - текущее время
    // определим время начала текущего периода
    hour1 = rtc1.hour - (rtc1.hour % period);
    // определим время начала периода в последнем времени отправки
    hour2 = rtc2.hour - (rtc2.hour % period);
    
    if (hour1 != hour2)
    {
        return 0;
    }
    
    return 1;
}

#define MODEM_REPEAT_NUM    3

static CPU_INT32U last_stat_send_time;
            
// задача сервера модема
void  ModemTask(void *p_arg)
{
    int task;
    int send_res, send_ctr, repeat_ctr;
                        
    while (1)
    {
        CPU_INT32U en = 0;

        OSTimeDly(100);

        if (!IsModemValid())
        {
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
    
                  // попытаемся переподключить модем
                  if (InitModem() != 0)
                  {
                     SetErrorFlag(ERROR_MODEM_CONN);
                  }
                  else
                  {
                     ClrErrorFlag(ERROR_MODEM_CONN);
                  }
              }
              else
              {
                  modem_status = 0;
              }
              continue;
        }

        modem_status = 0;
        
        GetData(&EnableModemDesc, &en, 0, DATA_FLAG_SYSTEM_INDEX);
        if (!en)
        {
          ResetModemValid();
          continue;
        }
        
        if (GetModemTask(&task))
        {
            // разберем задание 
            switch (task)
            {
                case MODEM_TASK_SEND_INCAS:
                  {
                    CPU_INT32U temp;
                    GetData(&IncasSendFlagDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
                    if (temp != INCAS_SEND_FLAG)
                    {
                        break;
                    }
                    GetData(&IncasMoneyDesc, &incas_money, 0, DATA_FLAG_SYSTEM_INDEX);
                    GetData(&IncasTimeDesc, &incas_time, 0, DATA_FLAG_SYSTEM_INDEX);
                    repeat_ctr = 0;
                    while (repeat_ctr < MODEM_REPEAT_NUM)
                    {
                        send_ctr = 0;                        
                        while (send_ctr < 3)
                        {
                            send_res = SendIncas();
                            if (send_res == 0) break;
                            OSTimeDly(1000);
                            send_ctr++;
                        }
                        if (send_ctr < 3)
                        {
                            break;
                        }
                        else
                        {
                            // не отправилось - переинициализируем модем
                            InitModem();
                        }
                        repeat_ctr++;
                    }
                    if (repeat_ctr >= MODEM_REPEAT_NUM)
                    {
                        // точно ошибка модема
                        SaveEventRecord(0, JOURNAL_EVENT_EMAIL_FAIL, 0);
                        // ещё раз запостим, чтобы отправлять, пока не отправится
                        PostModemTask(MODEM_TASK_SEND_INCAS);
                    }
                    else
                    {
                        temp = 0;
                        SetData(&IncasSendFlagDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
                        SetData(&IncasMoneyDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
                        SetData(&IncasTimeDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
                        SaveEventRecord(0, JOURNAL_EVENT_EMAIL_OK, 0);
                    }
                  }
                    break;
                
                case MODEM_TASK_SEND_STATISTICS:
                    repeat_ctr = 0;
                    while (repeat_ctr < MODEM_REPEAT_NUM)
                    {
                        send_ctr = 0;                        
                        while (send_ctr < 3)
                        {
                            send_res = SendStatistics();
                            if (send_res == 0) break;
                            OSTimeDly(1000);
                            send_ctr++;
                        }
                        if (send_ctr < 3)
                        {
                            break;
                        }
                        else
                        {
                            // не отправилось - переинициализируем модем
                            InitModem();
                        }
                        repeat_ctr++;
                    }
                    if (repeat_ctr >= MODEM_REPEAT_NUM)
                    {
                        // точно ошибка модема
                        SaveEventRecord(0, JOURNAL_EVENT_EMAIL_FAIL, 0);
                        // ещё раз заспостим, чтобы отправлять, пока не отправится
                        // не надо - само запостится, т.к. время мы ещё не сохранили
                        //PostModemTask(MODEM_TASK_SEND_STATISTICS);
                    }
                    else
                    {
                        SaveEventRecord(0, JOURNAL_EVENT_EMAIL_OK, 0);
                        ClearCounters();
                        SetData(&LastEmailSendTime, &last_stat_send_time, 0, DATA_FLAG_SYSTEM_INDEX);
                    }
                    break;
                    
                case MODEM_TASK_SEND_TEST_MSG:
                    repeat_ctr = 0;
                    while (repeat_ctr < 3)
                    {
                        send_ctr = 0;                        
                        while (send_ctr < 3)
                        {
                            send_res = SendTest();
                            if (send_res == 0) break;
                            OSTimeDly(1000);
                            send_ctr++;
                        }
                        if (send_ctr < 3)
                        {
                            break;
                        }
                        else
                        {
                            // не отправилось - переинициализируем модем
                            InitModem();
                        }
                        repeat_ctr++;
                    }
                    if (repeat_ctr >= 3)
                    {
                        // точно ошибка модема, забиваем на отправку
                        SaveEventRecord(0, JOURNAL_EVENT_EMAIL_FAIL, 0);
                    }
                    else
                    {
                        SaveEventRecord(0, JOURNAL_EVENT_EMAIL_OK, 0);
                    }
                    break;
                    
                case MODEM_TASK_RECONNECT:
                    // попытаемся переподключить модем
                    InitModem();
                    break;
            }
        }
        else
        {
            CPU_INT32U send_time, send_hour, send_minute;
            CPU_INT32U last_send_time, sec;
            TRTC_Data rtc_1, rtc_2;
       
            OSTimeDly(MODEM_TASK_DELAY);
            // посмотрим текущее время
            GetData(&SystemTimeDesc, &sec, 0, DATA_FLAG_SYSTEM_INDEX);

            GetData(&StatSendHourMinDesc, &send_time, 0, DATA_FLAG_SYSTEM_INDEX);
            send_hour = send_time / 60;
            send_minute = send_time % 60;
            
            // прочитаем последнее время отправки 
            GetData(&LastEmailSendTime, &last_send_time, 0, DATA_FLAG_SYSTEM_INDEX);
   
            Sec2Date(&rtc_1, sec);
            Sec2Date(&rtc_2, last_send_time);

            // запостим команду, если в текущем периоде ещё не отправлялось
            // отправляется всегда раз в сутки в 9.10
            if ((rtc_1.hour >= send_hour) && (rtc_1.min >= send_minute) && ((rtc_2.date != rtc_1.date) || (rtc_2.year != rtc_1.year) || (rtc_2.mon != rtc_1.mon)))
            {
                PostModemTask(MODEM_TASK_SEND_STATISTICS);
                last_stat_send_time = sec;
                //SetData(&LastEmailSendTime, &last_stat_send_time, 0, DATA_FLAG_SYSTEM_INDEX);
                // последнее время отправки установим потом, только в случае успешной отправки
            }
        }
    }
}

// постановка новой задачи в очередь задач модема
void PostModemTask(int new_task)
{
    OSQPost(ModemQuery, (void *)new_task);
}
