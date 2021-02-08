#ifndef __MODEM_H__
#define __MODEM_H__

#define	 MODEM_REPEAT_RX	5
#define  MODEM_RX_TIMEOUT	5000
#define  MODEM_OPEN_SERVICE_TIMEOUT	30000

/*
    strings.append("[receiver]=" + config.get('Receiver', 'email'))    
    strings.append("[ap_dns]=" + config.get('AccessPoint', 'dns', ''))
    strings.append("[ap_password]=" + config.get('AccessPoint', 'password', ''))
    strings.append("[ap_ip]=" + config.get('AccessPoint', 'ip', ''))
    strings.append("[ap_user]=" + config.get('AccessPoint', 'user', ''))
    strings.append("[ap_apn]=" + config.get('AccessPoint', 'apn', ''))    
    strings.append("[smtp_user]=" + config.get('Smtp', 'user', ''))
    strings.append("[smtp_password]=" + config.get('Smtp', 'password', ''))
    strings.append("[smtp_mail]=" + config.get('Smtp', 'mail', ''))
    strings.append("[smtp_server]=" + config.get('Smtp', 'smtp_server', ''))
    strings.append("[smtp_port]=" + config.get('Smtp', 'port', '25'))
*/
typedef struct
{
    char receiver[64];
    char ap_dns[64];
    char ap_password[64];
    char ap_ip[64];
    char ap_user[64];
    char ap_apn[64];
    char smtp_user[64];
    char smtp_password[64];
    char smtp_mail[64];
    char smtp_server[64];
    char smtp_port[64];
    
    CPU_INT08U valid;
} EmailOptions;
static EmailOptions email_options;

#define EMAIL_CFG_ELEM_COUNT    11


typedef int (*TextCallbackFunc)(char *str);


extern int  InitModem(void);
extern void ModemWriteStr(char const *str);
extern int  ModemSendOKCommand(char *str, unsigned long timeout);
extern int  ModemDeleteSMS(unsigned char index);
extern int  ModemWriteSMS(char const* text, unsigned char *index);
extern int  ModemSendSMS(char const* number, unsigned char index);
extern int  ModemReadSMS(char *text, int index);
extern int  ModemSendSMSMessage(char const* number, char const* text);
extern int  ModemRxNewSMS(unsigned long *num);
extern int  ModemSendCommand(char *str, unsigned long timeout);
extern CPU_INT08U IsModemConn(void);
extern CPU_INT08U IsModemConf(void);

extern int InitModemEmailParams(void);
extern int ModemSendEmail(char *subj, TextCallbackFunc text_callback);
extern CPU_INT08U IsModemValid(void);
extern void ResetModemValid(void);

#endif //#ifndef __MODEM_H__
