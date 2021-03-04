#ifdef CONFIG_FTP_CLIENT_ENABLE

#include <includes.h>
#include "app_serv.h"
#include "ftp_client.h"
#include <string.h>
#include "time.h"
#include "journal.h"
#include "data.h"
#include "datadesc.h"
#include "ftp_app.h"
#include "fram.h"
#include "fram_map.h"

/*
92.53.96.10
xmiker_morozov
Qwerty11
*/

static  OS_STK  FtpTaskStk[TERM_TASK_STK_SIZE];
char term_buffer[TERM_BUFFER_SIZE];

extern void PrintEventJournalRecordFtp(TEventRecord *record, char *str_event, char *str_data);
CPU_INT08U time_to_ftp = 0;

///
void FtpCheckTimeToSend(CPU_INT32U systime)
{
    CPU_INT32U enabled;
    GetData(&FtpEnableDesc, &enabled, 0, DATA_FLAG_SYSTEM_INDEX);
    if (enabled)
    {
        static const CPU_INT32U intervals_sec[8] = {1, 2, 3, 4, 6, 8, 12, 24};
        CPU_INT32U hh_mm;
        CPU_INT32U interval;
        CPU_INT32U last_time;
        TRTC_Data rtc_last, rtc_hhmm, rtc_curr;
            
        GetData(&FtpSendIntervalDesc, &interval, 0, DATA_FLAG_SYSTEM_INDEX);
        if (interval > 7) return;
        interval = intervals_sec[interval];
        
        GetData(&FtpSendHourMinDesc, &hh_mm, 0, DATA_FLAG_SYSTEM_INDEX);
        hh_mm *= 60;
        GetData(&FtpLastSendTimeDesc, &last_time, 0, DATA_FLAG_SYSTEM_INDEX);
        
        Sec2Date(&rtc_last, last_time);
        Sec2Date(&rtc_hhmm, hh_mm);
        Sec2Date(&rtc_curr, systime);

        if (systime - last_time > interval * 3600)
        {
            // отправляли больше чем заданный интервал назад -> отправляем всё
            time_to_ftp = FTP_FLAG_SEND_COUNTERS | FTP_FLAG_SEND_LOGS;
        }
        else if ((rtc_curr.hour == rtc_hhmm.hour) && (rtc_curr.min == rtc_hhmm.min) && (systime - last_time >= 60))
        {
            // совпал заданный час и минута в сутках + отправляли раньше, чем минуту назад -> отправляем всё
            time_to_ftp = FTP_FLAG_SEND_COUNTERS | FTP_FLAG_SEND_LOGS;
        }
        else if (((rtc_curr.hour % interval) == (rtc_hhmm.hour % interval)) && (rtc_curr.min == rtc_hhmm.min) && (systime - last_time >= 60))
        {
            // если совпал интервал с периодом в течение суток -> отправляем счетчики
            time_to_ftp |= FTP_FLAG_SEND_COUNTERS;
        }
    }
}

// чтение очередной строки для создания файла csv счетчиков
int ReadFtpCountersString(int index, char *buf)
{
    static const char header1[] = "Жетоны;Купюры, руб.;Монеты, руб;Наличные, руб.;Банк, руб.\r\n";
    static const char header2[] = "Жетоны;Купюры, руб.;Монеты, руб;Наличные, руб.;Банк, руб.\r\n";
    
    switch (index)
    {
        case 0:
            strcpy(buf, header1);
            break;
        case 1:
            GetDataStr(&CounterCoinOutDesc, (CPU_INT08U*)buf, 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, ";");
            GetDataStr(&CounterCashDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, ";");
            GetDataStr(&CounterCoinDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, ";");
            GetDataStr(&CounterAllCashDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, ";");
            GetDataStr(&CounterBankDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, "\r\n");
            break;
       case 2:
            strcpy(buf, header2);
            break;
       case 3:
            GetDataStr(&CounterLongCoinOutDesc, (CPU_INT08U*)buf, 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, ";");
            GetDataStr(&CounterLongCashDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, ";");
            GetDataStr(&CounterLongCoinDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, ";");
            GetDataStr(&CounterLongAllCashDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, ";");
            GetDataStr(&CounterLongBankDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
            strcat(buf, "\r\n");
            break;
        default:
            return 0;
    }
    return 1;
}

// чтение очередной строки для создания файла csv журналов
int ReadFtpLogString(int index, char *buf)
{
    static const char header[] = "Номер записи;Время;Событие;Данные\r\n";
    if (index == 0)
    {
        strcpy(buf, header);
        return 1;
    }
    else if ((index >= 1) && (index <= EVENT_RECORDS_COUNT))
    {
        TEventRecord record;
        index -= 1;
        GetEventRecord(&record, index);
        sprintf(buf, "%d;", index);
        PrintTimeString(&buf[strlen(buf)], record.time);
        strcat(buf, ";");
        PrintEventJournalRecordFtp(&record, &buf[strlen(buf)], &buf[128]);
        strcat(buf, ";");
        strcpy(&buf[strlen(buf)], &buf[128]);
        strcat(buf, "\r\n");
        return 1;
    }
    return 0;
}


/// отправка на ftp-сервер отчета счетчиков в формате csv
int FtpUploadCsvReport(NET_IP_ADDR ip, CPU_INT32U id, char* login, char* pass, CPU_INT32U time, CPU_INT08U flags)
{
    uint16_t port = FTP_CONTROL_PORT;
    FtpClientContext *ftp_context = (FtpClientContext *)term_buffer;
    char *bufstr = &term_buffer[sizeof(FtpClientContext)];
    int str_index;
    int res_ftp;
    char str[48];

    memset(ftp_context, 0, sizeof(FtpClientContext));
        
    if (ftpConnect(ftp_context, &ip, port, FTP_PASSIVE_MODE) != 0)
    {
        return -1;
    }
    
    if (ftpLogin(ftp_context, login, pass, NULL) != 0)
    {
        ftpClose(ftp_context);
        return -2;
    }
    
    // создадим путь к каталогу выгрузки /changer/00000001/
    res_ftp = ftpChangeWorkingDir(ftp_context, "changer");
    if (res_ftp != 0)
    {
        res_ftp = ftpMakeDir(ftp_context, "changer");
        if (res_ftp != 0)
        {
            ftpClose(ftp_context);
            return -3;
        }
        res_ftp = ftpChangeWorkingDir(ftp_context, "changer");
        if (res_ftp != 0)
        {
            ftpClose(ftp_context);
            return -3;
        }
    }
    
    sprintf(str, "%08d", id);
    res_ftp = ftpChangeWorkingDir(ftp_context, str);
    if (res_ftp != 0)
    {
        res_ftp = ftpMakeDir(ftp_context, str);
        if (res_ftp != 0)
        {
            ftpClose(ftp_context);
            return -3;
        }
        res_ftp = ftpChangeWorkingDir(ftp_context, str);
        if (res_ftp != 0)
        {
            ftpClose(ftp_context);
            return -3;
        }
    }
    
    if (flags & FTP_FLAG_SEND_COUNTERS)
    {
        // СЧЕТЧИКИ
        res_ftp = ftpChangeWorkingDir(ftp_context, "counters");
        if (res_ftp != 0)
        {
            res_ftp = ftpMakeDir(ftp_context, "counters");
            if (res_ftp != 0)
            {
                ftpClose(ftp_context);
                return -3;
            }
            res_ftp = ftpChangeWorkingDir(ftp_context, "counters");
            if (res_ftp != 0)
            {
                ftpClose(ftp_context);
                return -3;
            }
        }
            
        // имя файла counters/counters_20191201_121005.csv
        strcpy(str, "counters_");
        PrintSecDateTimeStringRaw(&str[strlen(str)], time);
        strcpy(&str[strlen(str)], ".csv");
        res_ftp = ftpOpenFile(ftp_context, str, FTP_FOR_WRITING | FTP_BINARY_TYPE);
        if (res_ftp != 0)
        {
            ftpClose(ftp_context);
            return -4;
        }
        
        str_index = 0;
        while (ReadFtpCountersString(str_index, bufstr))
        {
            res_ftp = ftpWriteFile(ftp_context, bufstr, strlen(bufstr), 0);
            if (res_ftp != 0)
            {
                ftpDeleteFile(ftp_context, str);
                ftpClose(ftp_context);
                return -5;
            }
            str_index++;
        }
            
        res_ftp = ftpCloseFile(ftp_context);
        if (res_ftp != 0)
        {
            ftpDeleteFile(ftp_context, str);
            ftpClose(ftp_context);
            return -6;
        }
        
        ftpChangeToParentDir(ftp_context);

        // имя файла counters.csv
        strcpy(str, "counters.csv");
        res_ftp = ftpOpenFile(ftp_context, str, FTP_FOR_WRITING | FTP_BINARY_TYPE);
        if (res_ftp != 0)
        {
            ftpClose(ftp_context);
            return -4;
        }
        
        str_index = 0;
        while (ReadFtpCountersString(str_index, bufstr))
        {
            res_ftp = ftpWriteFile(ftp_context, bufstr, strlen(bufstr), 0);
            if (res_ftp != 0)
            {
                ftpDeleteFile(ftp_context, str);
                ftpClose(ftp_context);
                return -5;
            }
            str_index++;
        }
            
        res_ftp = ftpCloseFile(ftp_context);
        if (res_ftp != 0)
        {
            ftpDeleteFile(ftp_context, str);
            ftpClose(ftp_context);
            return -6;
        }
    }
 
    // ЖУРНАЛ
    // имя файла log_20191201_121005.csv
    if (flags & FTP_FLAG_SEND_LOGS)
    {
        res_ftp = ftpChangeWorkingDir(ftp_context, "logs");
        if (res_ftp != 0)
        {
            res_ftp = ftpMakeDir(ftp_context, "logs");
            if (res_ftp != 0)
            {
                ftpClose(ftp_context);
                return -3;
            }
            res_ftp = ftpChangeWorkingDir(ftp_context, "logs");
            if (res_ftp != 0)
            {
                ftpClose(ftp_context);
                return -3;
            }
        }
            
        strcpy(str, "log_");
        PrintSecDateTimeStringRaw(&str[strlen(str)], time);
        strcpy(&str[strlen(str)], ".csv");
        res_ftp = ftpOpenFile(ftp_context, str, FTP_FOR_WRITING | FTP_BINARY_TYPE);
        if (res_ftp != 0)
        {
            ftpClose(ftp_context);
            return -4;
        }
        
        str_index = 0;
        while (ReadFtpLogString(str_index, bufstr))
        {
            res_ftp = ftpWriteFile(ftp_context, bufstr, strlen(bufstr), 0);
            if (res_ftp != 0)
            {
                ftpDeleteFile(ftp_context, str);
                ftpClose(ftp_context);
                return -5;
            }
            str_index++;
        }
            
        res_ftp = ftpCloseFile(ftp_context);
        if (res_ftp != 0)
        {
            ftpDeleteFile(ftp_context, str);
            ftpClose(ftp_context);
            return -6;
        }
    }
    
    ftpClose(ftp_context);
    return 0;
}

void FtpAppTask(void *p_arg)
{
    while(1)
    {
#ifdef CONFIG_FTP_CLIENT_ENABLE
        if (time_to_ftp)
        {
            CPU_INT32U ip;
            CPU_INT32U id;
            CPU_INT32U time = SystemTime;
            CPU_INT32U result;
            char login[16];
            char pass[16];
            
            GetData(&FtpDeviceNumberDesc, &id, 0, DATA_FLAG_SYSTEM_INDEX); 
            GetData(&FtpServerIpAddrDesc, &ip, 0, DATA_FLAG_SYSTEM_INDEX); 
            GetData(&FtpServerLoginDesc, login, 0, DATA_FLAG_SYSTEM_INDEX); 
            GetData(&FtpServerPassDesc, pass, 0, DATA_FLAG_SYSTEM_INDEX); 
                
            if (FtpUploadCsvReport(ip, id, login, pass, time, time_to_ftp) == 0)
            {
                result = 1;
                SaveEventRecord(0, JOURNAL_EVENT_FTP_SEND, (time_to_ftp << 1) | 0);
                if (time_to_ftp & FTP_FLAG_CLEAR_COUNTERS)
                {
                    ClearCounters();
                }
                if (time_to_ftp & FTP_FLAG_CLEAR_LOGS)
                {
                    ClearEventJournal();
                }
            }
            else
            {
                result = 0;
                SaveEventRecord(0, JOURNAL_EVENT_FTP_SEND, (time_to_ftp << 1) | 1);
            }
            WriteArrayFram(offsetof(TFramMap, FtpLastResult), sizeof(CPU_INT32U), (unsigned char *)&result);
            WriteArrayFram(offsetof(TFramMap, FtpLastTime), sizeof(CPU_INT32U), (unsigned char *)&time);
            time_to_ftp = 0;
            ftp_send_cmd = 0;
        }
#endif
        OSTimeDly(1000);
    }
}

void InitFTPApp()
{
    OSTaskCreate(FtpAppTask, (void *)0, (OS_STK *)&FtpTaskStk[TERM_TASK_STK_SIZE-1], TERM_TASK_PRIO);
    INT8U err;
    OSTaskNameSet(TERM_TASK_PRIO, "Ftp Task", &err);
}

#endif //#ifdef CONFIG_FTP_CLIENT_ENABLE
