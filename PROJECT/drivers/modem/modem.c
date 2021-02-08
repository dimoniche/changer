#include <includes.h>
#include "modem.h"
#include "modem_task.h"
#include "app_serv.h"
#include "data.h"
#include "datadesc.h"
#include "uart2.h"

static char additional_buf[256];

#define ModemUart_Ready Uart2_Ready
#define ModemComPortFlush   Uart2_Flush
#define ModemUart_Getc      Uart2_Getc
#define ModemUart_Gotc      Uart2_Gotc
#define ModemUart_Putc      Uart2_Putc
#define ModemUart_Init      Uart2_Init    


static OS_EVENT *ModemLock = NULL;
static char modem_buf[512];
static int modem_connected;

// получить доступ к модему
static void GetModem(void)
{
    CPU_INT08U err;  
    while (1)
    {
        OSSemPend(ModemLock, 1, &err);
        if (!err) break;
        OSTimeDly(1);
    }
}

// освободить модем
static void FreeModem(void)
{
    OSSemPost(ModemLock);
}

// запись строки в модем
static void ModemWriteStr(char const *str)
{
    while (*str != 0)
    {
        while (!ModemUart_Ready()) OSTimeDly(1);
        ModemUart_Putc(*str++);
    }
}

// чтение строки из модема по таймауту
static int ModemReadStr(char *str, unsigned long timeout)
{
    int byte_ctr = 0;
    while (byte_ctr < 256)
    {
        CPU_INT08U byte;
        unsigned long to = timeout;

        while (!ModemUart_Gotc()) 
        {
            if (to == 0)
            {
                *str = 0x00;
                return 0;
            }
            OSTimeDly(1);
            to--;
        }
        
        int ch = ModemUart_Getc();
        if (ch < 0)
        {
            *str = 0x00;
            return 0;
        }

        byte = (char)ch;
        *str++ = byte;
        byte_ctr++;
        if (byte == 0x0a) break;
    }

    *str = 0x00;
    return 1;
}

static int ModemReadByte(CPU_INT08U *byte, unsigned long timeout)
{
    unsigned long ctr = 0;

    while (!ModemUart_Gotc())
    {
        OSTimeDly(1);
        ctr++;
        if (ctr > timeout) return 0;
    }

    *byte = (CPU_INT08U)ModemUart_Getc();
    return 1;
}
    
// чтение строки из модема из сравнение со строкой
static int ModemReadStrAndComp(char *str, char const *format, unsigned long timeout)
{
    int result;

    if (ModemReadStr(str, timeout))
    {
        result = strcmp ((char *)str, (char const *)format);
    }
    else
    {
        return -1;
    }

    return result;
}

// инициализация/проверка состояния модема
int InitModem(void)
{
    int result;
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    CPU_INT32U val;

    OS_ENTER_CRITICAL();
    email_options.valid = 0;
    modem_connected = 0;
    OS_EXIT_CRITICAL();

    GetData(&EnableModemDesc, &val, 0, DATA_FLAG_SYSTEM_INDEX);
    if (val)
    {
        PINSEL1_bit.P0_24 = 0x0;
        PINMODE1_bit.P0_24 = 0;
        FIO0DIR_bit.P0_24  = 1;
        FIO0MASK_bit.P0_24 = 0;
        FIO0CLR_bit.P0_24  = 1;
        OSTimeDly(1000);
        FIO0SET_bit.P0_24  = 1;

        OSTimeDly(60000);
        
        ModemUart_Init(115200);

        if (!ModemLock) ModemLock = OSSemCreate(1); 
          
        GetModem();
        
        // проверим связь
        sprintf(modem_buf, "AT\r\n");
        result = ModemSendOKCommand(modem_buf, 1000);
        if (result != 0)
        {
          FreeModem();
          goto EXIT_INIT;
        }

        // проверим, введен ли пин
        sprintf(modem_buf, "AT+CPIN?\r\n");
        result = ModemSendOKCommand(modem_buf, 1000);
        if (result != 0)
        {
          FreeModem();
          goto EXIT_INIT;
        }

        FreeModem();

        OS_ENTER_CRITICAL();
        modem_connected = 1;
        OS_EXIT_CRITICAL();
          
        OSTimeDly(1000);
        
        result = InitModemEmailParams();
    }
    else
    {
        result = 0;
    }
EXIT_INIT:
    // создадим задачу и очередь модема
    if (ModemQuery == NULL)
      {    
        ModemQuery = OSQCreate(&ModemTbl[0], MODEM_QUERY_LEN);
        OSTaskCreate(ModemTask, (void *)0, (OS_STK *)&ModemTaskStk[MODEM_TASK_STK_SIZE-1], MODEM_TASK_PRIO);
      }
    return result;
}



// посылка сообщения по такому-то номеру такой-то текст
int ModemSendSMSMessage(char const* number, char const* text)
{
    char i; 

    GetModem();

    if (ModemWriteSMS(text, (unsigned char*)&i)) {FreeModem(); return -1;}
    ModemComPortFlush();
    OSTimeDly(200);
    if (ModemSendSMS(number, i)) {FreeModem(); return -1;}
    ModemComPortFlush();
    OSTimeDly(200);
    if (ModemDeleteSMS(i)) {FreeModem(); return -1;}
    OSTimeDly(200);
    ModemComPortFlush();

    FreeModem();
    return 0;
}


// чтение из памяти смс с указанным номером
int ModemReadSMS(char *text, int index)
{
    unsigned char count;
    char *ptr;

    GetModem();
    ModemComPortFlush();
    sprintf((char*)modem_buf, "AT+CMGR=%d\r\n", index);
    ModemWriteStr(modem_buf);
    if (!ModemReadStr(modem_buf, MODEM_RX_TIMEOUT)) {FreeModem(); return -1;}
    if (!ModemReadStr(modem_buf, MODEM_RX_TIMEOUT)) {FreeModem(); return -1;}

    // дальше текст сообщения
    count = 0;
    while (count < 160)
    {
        modem_buf[0] = 0;
        if (!ModemReadStr(modem_buf, MODEM_RX_TIMEOUT)) {FreeModem(); return -1;}
        if (strcmp(modem_buf, "OK\r\n") == 0)
        {
            break;
        }
        else if (strlen(modem_buf) != 0)
        {
            // есть непустая строка
            ptr = (char*)modem_buf;
            if (strlen((char const*)modem_buf) < 1) {FreeModem(); return -1;}
            while (*ptr) {*text++ = *ptr++; count++;}
        }
        else 
        {
            break;
        }
    }

    FreeModem();
    return 0;
}


// посылка смс с номером index из памяти модема по указанному номеру
int ModemSendSMS(char const* number, unsigned char index)
{
    GetModem();
    
    sprintf((char*)modem_buf, "AT+CMSS=%d,%s\r\n", index, number);
    ModemWriteStr(modem_buf);
    if (ModemReadStrAndComp(modem_buf, "\r\n", 10000)) {FreeModem(); return -1;}
    if (!ModemReadStr(modem_buf, 10000)) {FreeModem(); return -1;}
    modem_buf[6] = 0;
    if (strcmp((char const*)modem_buf, "+CMSS:")) {FreeModem(); return -1;}
    if (ModemReadStrAndComp(modem_buf, "\r\n", 10000)) {FreeModem(); return -1;}
    if (ModemReadStrAndComp(modem_buf, "OK\r\n", 10000)) {FreeModem(); return -1;}
    
    FreeModem();
    return 0;
}


// удаление смски с номером index из памяти
int ModemDeleteSMS(unsigned char index)
{
    int result;
    GetModem();
    sprintf((char*)modem_buf, "AT+CMGD=%d\r\n",index);
    result = ModemSendOKCommand(modem_buf, 1000);
    FreeModem();
    return result;
}

// запись смски с указанным текстом в память, в index записывает позицию в памяти, куда смс записалось
int ModemWriteSMS(char const* text, unsigned char *index)
{
    CPU_INT08U byte;
    unsigned long word;
    int result;
    
    GetModem();
    
    // посылаем команду
    ModemWriteStr("AT+CMGW\r\n");
    if (ModemReadStrAndComp(modem_buf, "\r\n", 1000)) {FreeModem(); return -1;}

    if (!ModemReadByte(&byte, 1000)) {FreeModem(); return -1;}
    if (byte != '>') {FreeModem(); return -1;}

    if (!ModemReadByte(&byte, 1000)) {FreeModem(); return -1;}
    if (byte != ' ') {FreeModem(); return -1;}

    // вводим текст сообщения
    ModemWriteStr(text); 
    // конец сообщения
    ModemUart_Putc(0x1a);
    // принимаем ответ о записи
    if (ModemReadStrAndComp(modem_buf, "\r\n", 1000)) {FreeModem(); return -1;}
    // прочитаем номер смски в памяти, куда она записалась
    if (!ModemReadStr(modem_buf, 5000)) {FreeModem(); return -1;}
    modem_buf[6] = 0;
    if (strcmp((char const*)modem_buf, "+CMGW:")) {FreeModem(); return -1;}
    {
    int len = strlen((char const*)&modem_buf[7]);
    if (len == 3) word = modem_buf[7]-0x30;
    else if (len == 4) {word = (modem_buf[7]-0x30)*10 + (modem_buf[8]-0x30);}
    }

    *index = (unsigned char)word;
    
    result = ModemReadStrAndComp(modem_buf, "\r\n", 1000) + ModemReadStrAndComp(modem_buf, "OK\r\n", 1000);
    FreeModem();
    // ОК
    return result;
}

// посылка строки, на которую должен прийти ответ "OK"
// возвращает 0 если ок
int ModemSendOKCommand(char *str, unsigned long timeout)
{
    int i;
    
    ModemComPortFlush();
    
    ModemWriteStr(str);
    
    i = MODEM_REPEAT_RX;
    while (i--)
    {
        if (ModemReadStr(str, timeout))
        {
            if (strcmp(str, "OK\r\n") == 0)
            {
                return 0;
            }
            else if (strcmp(str, "ERROR\r\n") == 0)
            {
                return -2;
            }
            
            continue;
        }
        else
        {
            return -1;
        }
    }
    
    return -3;
}

int ModemSendOKData(char *str, int len, unsigned long timeout)
{
    int i;
    
    ModemComPortFlush();
    
    for (i = 0; i < len; i++)
    {
        while (!ModemUart_Ready()) OSTimeDly(1);
        ModemUart_Putc(str[i]);
    }
        
    i = MODEM_REPEAT_RX;
    while (i--)
    {
        if (ModemReadStr(str, timeout))
        {
            if (strcmp(str, "OK\r\n") == 0) 
            {
                return 0;
            }
            else if (strcmp(str, "ERROR\r\n") == 0)
            {
                return -2;
            }
            
            continue;
        }
        else
        {
            return -1;
        }
    }
    
    return -3;
}


// посылка строки, с возвратом ответа на неё, отличного от "\r\n"
// возвращает 0 если ок
int ModemSendCommand(char *str, unsigned long timeout)
{
    CPU_INT08U i;
    
    ModemComPortFlush();
    
    ModemWriteStr(str);
    
    i = MODEM_REPEAT_RX;
    while (i--)
    {
        if (ModemReadStr(str, timeout))
        {
            if (strcmp(str, "\r\n") == 0)
            {
                continue;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return -1;
        }
    }
    
    return -3;
}


// прием сигнала о новой смске, в num записывает номер принятого сообщения, если оно принято
int ModemRxNewSMS(unsigned long *num)
{
    int len;

    GetModem();

    if (!ModemReadStr(modem_buf, 10)) {FreeModem(); return 0;}

    if (!strcmp((char const*)modem_buf, "\r\n"))
    {
        // принимаем ещё одну строку
        if (!ModemReadStr(modem_buf, 100)) {FreeModem(); return 0;}
    }

    if (strlen((char const*)modem_buf) < 13) {FreeModem(); return 0;}

    modem_buf[6] = 0;
    if (strcmp((char const*)modem_buf, "+CMTI:")) {FreeModem(); return 0;}

    len = strlen((char*)&modem_buf[12]);
    if (len == 3) *num = modem_buf[12]-0x30;
    else if (len == 4) {*num = (modem_buf[12]-0x30)*10 + (modem_buf[13]-0x30);}

    OSTimeDly(200);
    FreeModem();
    return 1;
}

typedef struct
{
    char const *name; // строка названия элемента
    char *dest;     // куда сожранить значение
} CfgElem;

static const CfgElem email_cfg_list[EMAIL_CFG_ELEM_COUNT] =
{
  {"[receiver]=",      email_options.receiver},
  {"[ap_dns]=",        email_options.ap_dns},
  {"[ap_password]=",   email_options.ap_password},
  {"[ap_ip]=",         email_options.ap_ip},
  {"[ap_user]=",       email_options.ap_user},
  {"[ap_apn]=",        email_options.ap_apn},
  {"[smtp_user]=",     email_options.smtp_user},
  {"[smtp_password]=", email_options.smtp_password},
  {"[smtp_mail]=",     email_options.smtp_mail},
  {"[smtp_server]=",   email_options.smtp_server},
  {"[smtp_port]=",     email_options.smtp_port}
};


// инициализация параметров для настройки почты
int InitModemEmailParams(void)
{
    CPU_INT08U i, j;
    char *text;
    CPU_INT32U status = 0;
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
  
    text = additional_buf;
    
    OS_ENTER_CRITICAL();
    email_options.valid = 0;
    OS_EXIT_CRITICAL();
      
    for (i = 1; i < 24; i++)
    {
        if (ModemReadSMS(text, i))
        {
            // не прочиталось
            continue;
        }          
    
        for (j = 0; j < EMAIL_CFG_ELEM_COUNT; j++)
        {
            int str_len = strlen(email_cfg_list[j].name);
            char flag = 0;
            while (str_len--)
            {
                if (email_cfg_list[j].name[str_len] != text[str_len])
                    {
                        flag = 1;
                        break;
                    }
            }
            if (!flag)
            {
                strcpy(email_cfg_list[j].dest, &text[strlen(email_cfg_list[j].name)]);  
                for (str_len = 0; str_len < 64; str_len++)
                {
                    if (email_cfg_list[j].dest[str_len] == 0) break;
                    if (email_cfg_list[j].dest[str_len] == '\r') {email_cfg_list[j].dest[str_len] = 0; break;}
                    if (email_cfg_list[j].dest[str_len] == '\n') {email_cfg_list[j].dest[str_len] = 0; break;}
                }
                  
                // нашли опцию в смске
                status |= 1L << j;
                break;
            }
        }
        
        OSTimeDly(200);
    }
 
    i = 1;
    for (j = 0; j < EMAIL_CFG_ELEM_COUNT; j++)
    {
        if ((status & (1 << j)) == 0) 
        {
            i = 0;
            break;
        }
    }
     
    OS_ENTER_CRITICAL();
    email_options.valid = i;
    OS_EXIT_CRITICAL();      
    
    return 0;
}

void ResetModemValid(void)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    OS_ENTER_CRITICAL();
    email_options.valid = 0;
    modem_connected = 0;
    OS_EXIT_CRITICAL(); 
}


// проверка, можно ли использовать модем (корректность настроек и подключения)
CPU_INT08U IsModemValid(void)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    CPU_INT08U result = 0;
    
    OS_ENTER_CRITICAL();
    if (email_options.valid && modem_connected)
    {
        result = 1;
    }
    OS_EXIT_CRITICAL(); 
    
    return result;
}

CPU_INT08U IsModemConn(void)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    CPU_INT08U result = 0;
    
    OS_ENTER_CRITICAL();
    if (modem_connected)
    {
        result = 1;
    }
    OS_EXIT_CRITICAL(); 
    
    return result;
}

CPU_INT08U IsModemConf(void)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    CPU_INT08U result = 0;
    
    OS_ENTER_CRITICAL();
    if (email_options.valid)
    {
        result = 1;
    }
    OS_EXIT_CRITICAL(); 
    
    return result;
}


// м.б. запускать при старте, чтобы проверять, настраивается ли можем этими настройками
int ModemConfigGprs()
{
    int repeat = 0;
    if (!email_options.valid) return -1;

    GetModem();
    
REP_1:
    sprintf(modem_buf, "AT^SICS=0,conType,GPRS0\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        sprintf(modem_buf, "AT^SISC=0\r\n");
        ModemSendOKCommand(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT);
        OSTimeDly(500);
        if (repeat++ == 0) goto REP_1;
        FreeModem();
        return -5;
    }

    sprintf(modem_buf, "AT^SICS=0,inactTO,\"20\"\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -6;
    }
    
    sprintf(modem_buf, "AT^SICS=0,dns1,\"%s\"\r\n", email_options.ap_dns);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -7;
    }
    
    sprintf(modem_buf, "AT^SICS=0,passwd,\"%s\"\r\n", email_options.ap_password);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -8;
    }
        
    sprintf(modem_buf, "AT^SICS=0,apn,\"%s\"\r\n", email_options.ap_apn);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -9;
    }
        
    sprintf(modem_buf, "AT^SICS=0,user,\"%s\"\r\n", email_options.ap_user);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -10;
    }

    FreeModem();
    return 0;
}

int ModemConfigSmtp()
{
    int repeat = 0;
    if (!email_options.valid) return -1;

    GetModem();
REP_2:
    // Select service type SMTP.
    sprintf(modem_buf, "AT^SISS=0,srvType,\"Smtp\"\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        sprintf(modem_buf, "AT^SISC=0\r\n");
        ModemSendOKCommand(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT);
        OSTimeDly(10000);
        if (repeat++ == 0) goto REP_2;
        FreeModem();
        return -25;
    }
        
    // Choose ASCII alphabet.
    sprintf(modem_buf, "AT^SISS=0,alphabet,\"1\"\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -26;
    }
        
    // Select connection profile 0.
    sprintf(modem_buf, "AT^SISS=0,conId,\"0\"\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -27;
    }
        
    // Specify SMTP server address.
    sprintf(modem_buf, "AT^SISS=0,address,\"%s\"\r\n", email_options.smtp_server);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -28;
    }
    
    // Specify sender's user name required for SMTP authentication.
    sprintf(modem_buf, "AT^SISS=0,user,\"%s\"\r\n", email_options.smtp_user);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -29;
    }

    // Specify password used by the sender for SMTP authentication.
    sprintf(modem_buf, "AT^SISS=0,passwd,\"%s\"\r\n", email_options.smtp_password);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -30;
    }

    // Sender name and password can be used for SMTP authentication.
    sprintf(modem_buf, "AT^SISS=0,smAuth,\"1\"\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -31;
    }

    // Port for SMTP connection.
    sprintf(modem_buf, "AT^SISS=0,tcpPort,\"%s\"\r\n", email_options.smtp_port);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -32;
    }

    FreeModem();
    return 0;
}

// Отправка почты: получатель и отправитель уже есть в настроках модема,
// задаем тему и передаем указатель на callback функцию, с помощью которой получаем текст письма
int ModemSendEmail(char *subj, TextCallbackFunc text_callback)
{
    char *text;
    int result = -1;
    
    if (!email_options.valid) return -50;
        
    result = ModemConfigGprs();
    if (result)
        {
            return result;
        }
    
    result = ModemConfigSmtp();
    if (result)
        {
            return result;
        }
    
    GetModem();
    
    // Sender's email address.
    sprintf(modem_buf, "AT^SISS=0,smFrom,\"%s\"\r\n", email_options.smtp_mail);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -51;
    } 

    // Recipient's email address.
    sprintf(modem_buf, "AT^SISS=0,smRcpt,\"%s\"\r\n", email_options.receiver);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -52;
    } 
        
    // Enter text for subject field.
    sprintf(modem_buf, "AT^SISS=0,smSubj,\"%s\"\r\n", subj);
    if (ModemSendOKCommand(modem_buf, MODEM_RX_TIMEOUT) != 0)
    {
        FreeModem();
        return -53;
    } 

    sprintf(modem_buf, "AT^SISC=0\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT) != 0)
    {
        FreeModem();
        return -54;
    } 
    
    OSTimeDly(5000);
    // Open the service, i.e. start to send the email.
    sprintf(modem_buf, "AT^SISO=0\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT) != 0)
    {
        FreeModem();
        return -55;
    } 

    if (!ModemReadStr(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT))
    {
        FreeModem();
        return -56;
    }
    
    if (!ModemReadStr(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT))
    {
        FreeModem();
        return -57;
    }

    if (strcmp(modem_buf, "^SISW: 0, 1\r\n") != 0)
    {
        return -58;
    }
    
    text = additional_buf;
    
    OSTimeDly(1000);
    
    // запись сообщения
    while (text_callback(text) == 0)
    {
        int lenght = strlen(text);
        int wr_len, reg, eof;
        int wr_ctr = 0;
        
        while (wr_ctr < lenght)
        {
            sprintf(modem_buf, "AT^SISW=0,%d\r\n", lenght-wr_ctr);
            if (ModemSendCommand(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT))
            {
                result = -59;
                goto EMAIL_EXIT_FAIL;
            }
            
            sscanf(modem_buf, "^SISW: %d, %d, %d", &reg, &wr_len, &eof);
            
            if ((reg != 0) || (eof != 0) || (wr_len > lenght-wr_ctr) || (wr_len == 0))
            {
                result = -60;
                goto EMAIL_EXIT_FAIL;
            }
    
            if (ModemSendOKData(text, wr_len, MODEM_OPEN_SERVICE_TIMEOUT) != 0)
            {
                result = -61;
                goto EMAIL_EXIT_FAIL;
            }
            
            if (!ModemReadStr(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT))
            {
                result = -62;
                goto EMAIL_EXIT_FAIL;
            }
            
            wr_ctr += wr_len;
            OSTimeDly(50);
        }
    }
    
    // Set the  <eodFlag> to mark the end of the email body.
    // The <eodFlag> is accepted by the service.
    sprintf(modem_buf, "AT^SISW=0,0,1\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT) != 0)
    {
        result = -63;
        goto EMAIL_EXIT_FAIL;   
    }     
    
    // The "^SISW" URC confirms that all data is sent suc-cessfully.
    if (!ModemReadStr(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT)) // odoa
    {
        result = -64;
        goto EMAIL_EXIT_FAIL;   
    }
    if (!ModemReadStr(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT)) // ^sisw
    {
        result = -65;
        goto EMAIL_EXIT_FAIL;   
    }
    if (strcmp(modem_buf, "^SISW: 0, 2\r\n") != 0)
    {
        result = -66;
        goto EMAIL_EXIT_FAIL;       
    }
    
    // Close the service. 
    sprintf(modem_buf, "AT^SISC=0\r\n");
    if (ModemSendOKCommand(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT) != 0)
    {
        result = -67;
        goto EMAIL_EXIT_FAIL;       
    } 
    
    FreeModem();
    return 0;
    
EMAIL_EXIT_FAIL:
    sprintf(modem_buf, "AT^SISC=0\r\n");
    OSTimeDly(10000);
    ModemSendOKCommand(modem_buf, MODEM_OPEN_SERVICE_TIMEOUT);
    FreeModem();
    return result;
}

