#ifndef _FTP_APP_H_
#define _FTP_APP_H_

#include <includes.h>

extern CPU_INT08U time_to_ftp;

#define FTP_FLAG_SEND_COUNTERS          0x01
#define FTP_FLAG_SEND_LOGS              0x02
#define FTP_FLAG_CLEAR_COUNTERS         0x04
#define FTP_FLAG_CLEAR_LOGS             0x08

#define TERM_BUFFER_SIZE    2048

#if uC_TCPIP_MODULE > 0
extern int FtpUploadCsvReport(NET_IP_ADDR ip, CPU_INT32U id, char* login, char* pass, CPU_INT32U time, CPU_INT08U flags);
extern void FtpCheckTimeToSend(CPU_INT32U systime);
#endif

extern void InitFTPApp();

#endif //_FTP_APP_H_
