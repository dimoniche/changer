#ifndef _FTP_CLIENT_H
#define _FTP_CLIENT_H

#include <includes.h>
#include <stdint.h>

#if uC_TCPIP_MODULE > 0
#define FTP_CLIENT_DEFAULT_TIMEOUT 10000
#define FTP_CLIENT_BUFFER_SIZE 512
#define FTP_CLIENT_WRITE_TIMEOUT 5000

#define FTP_CONTROL_PORT   (21)
#define FTP_DATA_PORT   (20)

// Test macros for FTP response codes
#define FTP_REPLY_CODE_1YZ(code) ((code) >= 100 && (code) < 200)
#define FTP_REPLY_CODE_2YZ(code) ((code) >= 200 && (code) < 300)
#define FTP_REPLY_CODE_3YZ(code) ((code) >= 300 && (code) < 400)
#define FTP_REPLY_CODE_4YZ(code) ((code) >= 400 && (code) < 500)
#define FTP_REPLY_CODE_5YZ(code) ((code) >= 500 && (code) < 600)


/**
 * @brief Connection options
 **/
typedef enum
{
   FTP_NO_SECURITY       = 0,
   FTP_IMPLICIT_SECURITY = 1,
   FTP_EXPLICIT_SECURITY = 2,
   FTP_ACTIVE_MODE       = 0,
   FTP_PASSIVE_MODE      = 4
} FtpConnectionFlags;


/**
 * @brief File opening options
 **/
typedef enum
{
   FTP_FOR_READING   = 0,
   FTP_FOR_WRITING   = 1,
   FTP_FOR_APPENDING = 2,
   FTP_BINARY_TYPE   = 0,
   FTP_TEXT_TYPE     = 4
} FtpFileOpeningFlags;


/**
 * @brief Flags used by I/O functions
 **/
typedef enum
{
   FTP_FLAG_PEEK       = 0x0200,
   FTP_FLAG_WAIT_ALL   = 0x0800,
   FTP_FLAG_BREAK_CHAR = 0x1000,
   FTP_FLAG_BREAK_CRLF = 0x100A,
   FTP_FLAG_WAIT_ACK   = 0x2000
} FtpFlags;


/**
 * @brief FTP client context
 **/
typedef struct
{
   uint32_t serverAddr;                     ///<IP address of the FTP server
   uint8_t passiveMode;                    ///<Passive mode
   NET_SOCK_ID controlSocket;                 ///<Control connection socket
   NET_SOCK_ID dataSocket;                    ///<Data connection socket
   uint8_t buffer[FTP_CLIENT_BUFFER_SIZE]; ///<Memory buffer for input/output operations
} FtpClientContext;


#define FTP_NO_ERROR                    (0)
#define ERROR_OPEN_FAILED               (-100)
#define ERROR_INVALID_PARAMETER         (-101)
#define ERROR_UNEXPECTED_RESPONSE       (-102)
#define ERROR_EMPTY_RECEIVE             (-103)
#define ERROR_INVALID_ADDRESS           (-104)
#define ERROR_INVALID_SYNTAX            (-105)

#define     isdigit(x)      (x >= '0' && x <= '9')


// FTP client related functions
extern int ftpConnect(FtpClientContext *context, uint32_t *serverAddr, uint16_t serverPort, uint32_t flags);
extern int ftpLogin(FtpClientContext *context, const char *username, const char *password, const char *account);
extern int ftpGetWorkingDir(FtpClientContext *context, char *path, uint32_t size);
extern int ftpOpenFile(FtpClientContext *context, const char *path, uint32_t flags);
extern int ftpWriteFile(FtpClientContext *context, const void *data, uint32_t length, uint32_t flags);
extern int ftpCloseFile(FtpClientContext *context);
extern int ftpDeleteFile(FtpClientContext *context, const char *path);
extern int ftpClose(FtpClientContext *context);
extern int ftpSendCommand(FtpClientContext *context, const char *command, uint32_t *replyCode);
extern int ftpChangeWorkingDir(FtpClientContext *context, const char *path);
extern int ftpMakeDir(FtpClientContext *context, const char *path);
extern int ftpChangeToParentDir(FtpClientContext *context);

#endif
#endif