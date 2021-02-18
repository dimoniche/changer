#ifdef CONFIG_FTP_CLIENT_ENABLE

#include <includes.h>
#include "app_serv.h"
#include "ftp_client.h"
#include <string.h>
#include "time.h"
#include "term_tsk.h"
#include "journal.h"
#include "data.h"
#include "datadesc.h"
#include "ftp_app.h"

/*
92.53.96.10
xmiker_morozov
Qwerty11
*/

// испольузем общий с терминалом буфер данных
extern char term_buffer[TERM_BUFFER_SIZE];
extern void PrintEventJournalRecord(TEventRecord *record, char *str_event, char *str_data);
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
    CPU_INT32U i;
    static const char header1[] = ";Солярий 1 Коллатэн;Солярий 1 Ультрафиолет;Солярий 1 Максимальный;Солярий 2 Коллатэн;Солярий 2 Ультрафиолет;Солярий 2 Максимальный;Солярий 3 Коллатэн;Солярий 3 Ультрафиолет;Солярий 3 Максимальный\r\n";
    static const char line1[] = "Деньги, руб.";
    static const char line2[] = "Запусков";
    static const char line3[] = "Наработка, мин.";
    static const char header2[] = ";Коллатэн;Ультрафиолет;Максимальный\r\n";
    static const char header3[] = ";Солярий 1;Солярий 2;Солярий 3\r\n";
    static const char line4[] = "Коллатэн, ч:мм";
    static const char line5[] = "Ультрафиолет, ч:мм";
    static const char header4[] = "Наличные, руб.;Банк, руб.;Всего, руб.\r\n";
    static const char line6[] = "Тестовых запусков";
    static const char line7[] = "Время теста, мин.";
    static const char line10[] = "Количество уборок";
    static const char line11[] = "Время уборок, мин.";
    static const char line12[] = "Среднее время уборки, м:сс";
    static const char line13[] = "Среднее время теста, м:сс";

    switch (index)
    {
        case 0:
            strcpy(buf, header1);
            break;
//        case 1:
//            strcpy(buf, line1);
//            for (i = 0; i < CHANNELS_NUM * SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterSolarMoneyDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 2:
//            strcpy(buf, line2);
//            for (i = 0; i < CHANNELS_NUM * SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterSolarRunsDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 3:
//            strcpy(buf, line3);
//            for (i = 0; i < CHANNELS_NUM * SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterSolarWorkTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 4:
//            strcpy(buf, line6);
//            for (i = 0; i < CHANNELS_NUM * SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterSolarTestRunsDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 5:
//            strcpy(buf, line6);
//            for (i = 0; i < CHANNELS_NUM * SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterSolarTestWorkTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 6:
//            strcpy(buf, "\r\n");
//            break;
//        case 7:
//            strcpy(buf, header2);
//            break;
//        case 8:
//            strcpy(buf, line1);
//            for (i = 0; i < SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterModeMoneyDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 9:
//            strcpy(buf, line2);
//            for (i = 0; i < SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterModeRunsDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 10:
//            strcpy(buf, line3);
//            for (i = 0; i < SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterModeWorkTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 11:
//            strcpy(buf, line6);
//            for (i = 0; i < SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterModeTestRunsDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 12:
//            strcpy(buf, line7);
//            for (i = 0; i < SOLAR_MODES_COUNT; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterModeWorkTestTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 13:
//            strcpy(buf, "\r\n");
//            break;
//        case 14:
//            strcpy(buf, header3);
//            break;
//        case 15:
//            strcpy(buf, line4);
//            for (i = 0; i < CHANNELS_NUM; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterCollatenTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 16:
//            strcpy(buf, line5);
//            for (i = 0; i < CHANNELS_NUM; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterUFTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 17:
//            strcpy(buf, line6);
//            for (i = 0; i < CHANNELS_NUM; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterAllTestCountDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 18:
//            strcpy(buf, line7);
//            for (i = 0; i < CHANNELS_NUM; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterAllTestTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 19:
//            strcpy(buf, line13);
//            for (i = 0; i < CHANNELS_NUM; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterTestMeanTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 20:
//            strcpy(buf, line10);
//            for (i = 0; i < CHANNELS_NUM; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterCleaningCountDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 21:
//            strcpy(buf, line11);
//            for (i = 0; i < CHANNELS_NUM; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterCleaningTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 22:
//            strcpy(buf, line12);
//            for (i = 0; i < CHANNELS_NUM; i++)
//            {
//                strcat(buf, ";");
//                GetDataStr(&CounterCleaningMeanTimeDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            }
//            strcat(buf, "\r\n");
//            break;
//        case 23:
//            strcpy(buf, "\r\n");
//            break;
//        case 24:
//            strcpy(buf, header4);
//            break;
//        case 25:
//            GetDataStr(&CounterCashMoneyDesc, (CPU_INT08U*)buf, i, DATA_FLAG_DIRECT_INDEX);
//            strcat(buf, ";");
//            GetDataStr(&CounterCardMoneyDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            strcat(buf, ";");
//            GetDataStr(&CounterCommonMoneyDesc, (CPU_INT08U*)&buf[strlen(buf)], i, DATA_FLAG_DIRECT_INDEX);
//            strcat(buf, "\r\n");
//            break;
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
        PrintEventJournalRecord(&record, &buf[strlen(buf)], &buf[128]);
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
    
    // создадим путь к каталогу выгрузки /solarium/00000001/
    res_ftp = ftpChangeWorkingDir(ftp_context, "solarium");
    if (res_ftp != 0)
    {
        res_ftp = ftpMakeDir(ftp_context, "solarium");
        if (res_ftp != 0)
        {
            ftpClose(ftp_context);
            return -3;
        }
        res_ftp = ftpChangeWorkingDir(ftp_context, "solarium");
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


#endif //#ifdef CONFIG_FTP_CLIENT_ENABLE
