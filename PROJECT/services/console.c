// задача консоли для ввода на строек по COM-порту
#include <includes.h>
#include "console.h"
#include "console_cmd.h"
#include "fram.h"
#include "fram_map.h"
#include "app_serv.h"

extern const TFramMap config_params;
extern TFramMap *config_ram;

#define  CONSOLE_TCP_SERVER_INVITATION         "Welcome to control console!\n"

#define CONSOLE_BUF_SIZE    128

char reboot_flag = 0;

static char cmd_string[CONSOLE_BUF_SIZE];
static int rx_counter;
static char value_string[CONSOLE_BUF_SIZE];

    
NET_SOCK_ID         sock_listen = 0;
NET_SOCK_ID         sock_req = 0;

static  OS_STK     ConsoleTaskStk[CONSOLE_TASK_STK_SIZE];

const char* prompt = "> ";
int stcmp(char * s1, char * s2);
int stlen(char * st1);
char *nextarg(char * argp);
void showmenu(char* inbuf, TMenuOpt** menu);
extern TMenuOpt* const menu[];

int ConsoleWriteStr(char* str)
{
    int tx_ctr = 0;
    int len = strlen(str);
    while (tx_ctr < len)
    {
        NET_ERR err;
        int tx_size = NetSock_TxData(sock_req, &str[tx_ctr], len - tx_ctr, NET_SOCK_FLAG_NONE, &err);
        OSTimeDly(10);
        if (tx_size > 0)
        {
            tx_ctr += tx_size;        }
        else
        {
            return -1;
        }
    }
    return tx_ctr;
}

void ConsoleIO(char c)
{
    cmd_string[rx_counter] = c;
    rx_counter++;
    
    if (c == '\n')
    {
        char *ptr = strrchr(cmd_string, '\n');
        if (ptr) *ptr = 0;
        ptr = strrchr(cmd_string, '\r');
        if (ptr) *ptr = 0;

        if (strstr(cmd_string, "SET ") == cmd_string)
        {
            if (SetParam(&cmd_string[4]) == 0)
            {
                sprintf(cmd_string, "OK\n");
            }
            else
            {
                sprintf(cmd_string, "ERROR\n");
            }
            ConsoleWriteStr(cmd_string);
        }
#ifndef BOARD_POST_CFG
        else if (strstr(cmd_string, "GETDESC ") == (char*)cmd_string)
        {
            TDataDescStruct const* desc_ptr;
            CPU_INT32U index;
            
            FindDescByName(&cmd_string[8], &desc_ptr, &index);
            if (desc_ptr != NULL)
            {
                GetDataStr(desc_ptr, (CPU_INT08U*)value_string, index, DATA_FLAG_DIRECT_INDEX);
                strcat(value_string, "\nOK\n");
                ConsoleWriteStr(value_string);
            }
            else
            {
                ConsoleWriteStr("ERROR\n");
            }
        }
#endif
        else if (strstr(cmd_string, "GET ") == (char*)cmd_string)
        {
            if (GetParam(&cmd_string[4], value_string, CONSOLE_BUF_SIZE - 8) == 0)
            {
                strcat(value_string, "OK\n");
                ConsoleWriteStr(value_string);
            }
            else
            {
                sprintf(cmd_string, "ERROR\n");
                ConsoleWriteStr(cmd_string);
            }
        }
        
        memset(cmd_string, 0, CONSOLE_BUF_SIZE);
        rx_counter = 0;
    }
    else if (rx_counter >= CONSOLE_BUF_SIZE)
    {
        memset(cmd_string, 0, CONSOLE_BUF_SIZE);
        rx_counter = 0;
    }
}

void  ConsoleTask(void *p_arg)
{
    NET_SOCK_ADDR_IP    server_sock_addr_ip;
    NET_SOCK_ADDR_LEN   server_sock_addr_ip_size;
    NET_SOCK_ADDR_IP    client_sock_addr_ip;
    NET_SOCK_ADDR_LEN   client_sock_addr_ip_size;
    CPU_BOOLEAN         attempt_conn;
    CPU_CHAR           *pbuf;
    CPU_INT16S          buf_len;
    NET_SOCK_RTN_CODE   tx_size;
    NET_ERR             err;
    CPU_INT16U          port;
    
    while (1)
    {
        memset(cmd_string, 0, CONSOLE_BUF_SIZE);
        memset(value_string, 0, CONSOLE_BUF_SIZE);
        rx_counter = 0;
        OSTimeDly(10);
    
        if (NetNIC_ConnStatusGet() != DEF_ON)
        {
            continue;
        }
    
        sock_listen = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4, NET_SOCK_TYPE_STREAM, NET_SOCK_PROTOCOL_TCP, &err);
        if (err != NET_SOCK_ERR_NONE)
        {
            continue;
        }
  
        server_sock_addr_ip_size = sizeof(server_sock_addr_ip);
        Mem_Clr((void*)&server_sock_addr_ip, (CPU_SIZE_T)server_sock_addr_ip_size);
        server_sock_addr_ip.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
        server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(NET_SOCK_ADDR_IP_WILD_CARD);
        
#ifdef BOARD_POST_CFG
        port = CONSOLE_TCP_DEFAULT_PORT;
#else
//        ReadArrayFram(offsetof(TFramMap, port), sizeof(CPU_INT16U), (unsigned char*)&port);
        port = CONSOLE_TCP_DEFAULT_PORT;
#endif
        
        server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(port);
        
        NetSock_Bind((NET_SOCK_ID )sock_listen, (NET_SOCK_ADDR *)&server_sock_addr_ip, (NET_SOCK_ADDR_LEN)NET_SOCK_ADDR_SIZE, (NET_ERR *)&err);
        if (err != NET_SOCK_ERR_NONE) 
        {
            NetSock_Close(sock_listen, &err);
            continue;
        }
  
        NetSock_Listen(sock_listen, 1, &err);
        if (err != NET_SOCK_ERR_NONE) 
        {
            NetSock_Close(sock_listen, &err);
            continue;
        }
  
        do 
        {
            client_sock_addr_ip_size = sizeof(client_sock_addr_ip);
  
            if (NetNIC_ConnStatusGet() != DEF_ON)
            {
                err = NET_SOCK_ERR_NONE_AVAIL;
                break;
            }

            OSTimeDly(1);
            
            sock_req = NetSock_Accept((NET_SOCK_ID )sock_listen, (NET_SOCK_ADDR *)&client_sock_addr_ip, (NET_SOCK_ADDR_LEN *)&client_sock_addr_ip_size, (NET_ERR *)&err);
            switch (err) 
            {
                case NET_SOCK_ERR_NONE:
                    attempt_conn = DEF_NO;
                    break;
                case NET_ERR_INIT_INCOMPLETE:
                case NET_SOCK_ERR_NULL_PTR:
                case NET_SOCK_ERR_NONE_AVAIL:
                case NET_SOCK_ERR_CONN_ACCEPT_Q_NONE_AVAIL:
                case NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT:
                case NET_OS_ERR_LOCK:
                    attempt_conn = DEF_YES;
                    break;
      
                default:
                    attempt_conn = DEF_NO;
                    break;
            }
        } while (attempt_conn == DEF_YES);
  
        if (err != NET_SOCK_ERR_NONE) 
        {
            NetSock_Close(sock_listen, &err);
            NetSock_Close(sock_req, &err);
            continue;
        }
        
        pbuf    = CONSOLE_TCP_SERVER_INVITATION;
        buf_len = Str_Len(CONSOLE_TCP_SERVER_INVITATION);
        tx_size = NetSock_TxData(sock_req, pbuf, buf_len, NET_SOCK_FLAG_NONE, &err);
        if (tx_size != buf_len)
        {
            NetSock_Close(sock_req, &err);
            NetSock_Close(sock_listen, &err);
            continue;
        }
          
#define SOCKET_RX_TIMEOUT   60000
        CPU_INT32U time_stamp;
        time_stamp = OSTimeGet();
        do
        {
            char c[32];
            NET_SOCK_RTN_CODE ret_code;
            
            if (NetNIC_ConnStatusGet() != DEF_ON)
            {
                err = NET_SOCK_ERR_FAULT;
                break;
            }
            
            OSTimeDly(1);
            memset(c, 0, 32);
            ret_code = NetSock_RxData(sock_req, &c, 32, NET_SOCK_FLAG_RX_NO_BLOCK, &err);
            switch (err)
            {
                case NET_SOCK_ERR_NONE:
                    if (ret_code)
                    {
                        char *c_ptr = c;
                        time_stamp = OSTimeGet();
                        while (ret_code--)
                        {
                            ConsoleIO(*c_ptr++);
                        }
                    }
                    break;
                case NET_SOCK_ERR_RX_Q_EMPTY:
                    if (OSTimeGet() - time_stamp < SOCKET_RX_TIMEOUT)
                    {
                        err = NET_SOCK_ERR_NONE;
                    }
                    break;
                case NET_SOCK_ERR_INVALID_DATA_SIZE:
                case NET_ERR_INIT_INCOMPLETE:
                case NET_SOCK_ERR_NULL_PTR:
                case NET_SOCK_ERR_NULL_SIZE:
                case NET_SOCK_ERR_NOT_USED:
                case NET_SOCK_ERR_CLOSED:
                case NET_SOCK_ERR_FAULT:
                case NET_SOCK_ERR_INVALID_SOCK:
                case NET_SOCK_ERR_INVALID_FAMILY:
                case NET_SOCK_ERR_INVALID_PROTOCOL:
                case NET_SOCK_ERR_INVALID_TYPE:
                case NET_SOCK_ERR_INVALID_STATE:
                case NET_SOCK_ERR_INVALID_OP:
                case NET_SOCK_ERR_INVALID_FLAG:
                case NET_SOCK_ERR_INVALID_ADDR_LEN:
                case NET_SOCK_ERR_RX_Q_CLOSED:
                case NET_ERR_RX:
                case NET_CONN_ERR_INVALID_CONN:
                case NET_CONN_ERR_NOT_USED:
                case NET_CONN_ERR_NULL_PTR:
                case NET_CONN_ERR_INVALID_ADDR_LEN:
                case NET_CONN_ERR_ADDR_NOT_USED:
                case NET_OS_ERR_LOCK:
                    break;
                default:
                    err = NET_ERR_RX;
                    break;
            }
            if (reboot_flag) break;
    
        } while (err == NET_SOCK_ERR_NONE);
        
        NetSock_Close(sock_req, &err);
        NetSock_Close(sock_listen, &err);
        if (reboot_flag)
        {
            OSTimeDly(1000);
            #ifdef BOARD_POST_CFG
            if (memcmp(config_ram, &config_params, sizeof(config_params)) != 0)
            {
                #if OS_CRITICAL_METHOD == 3
                OS_CPU_SR  cpu_sr = 0;
                #endif
                OS_ENTER_CRITICAL();
                save_config_params();
                Reset();  
                OS_EXIT_CRITICAL();
            }
            #endif
            Reset();  
        }
    }
}

/// запуск консоли
void InitConsole(void)
{
    INT8U err;
    OSTaskCreate(ConsoleTask, (void *)0, (OS_STK *)&ConsoleTaskStk[CONSOLE_TASK_STK_SIZE-1], CONSOLE_TASK_PRIO);
    OSTaskNameSet(CONSOLE_TASK_PRIO, "Console Task", &err);
}
