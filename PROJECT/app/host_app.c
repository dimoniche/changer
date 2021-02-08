#include <includes.h>
#include "app_serv.h"
#include "host_app.h"

//#define HOST_RUN_TASK_ENABLE

#ifdef HOST_RUN_TASK_ENABLE
    static  OS_STK     HodtTaskStk[HOST_TASK_STK_SIZE];
#endif

#define HOST_TCP_BUF_SIZE  128

static char host_tcp_buf[HOST_TCP_BUF_SIZE];

int host_conn_ctr_all = 0;
int host_conn_ctr_ok = 0;

///
int HostWriteStr(NET_SOCK_ID sock, char* str)
{
    int tx_ctr = 0;
    int len = strlen(str);
    while (tx_ctr < len)
    {
        NET_ERR err;
        int tx_size = NetSock_TxData(sock, &str[tx_ctr], len - tx_ctr, NET_SOCK_FLAG_NONE, &err);
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

///
int HostReadLine(NET_SOCK_ID sock, char *str, CPU_INT32U maxlen, CPU_INT32U timeout, NET_ERR *err)
{
    CPU_INT32U time_stamp;
    CPU_INT32U sleep_ctr = 0;
    char *str_ptr = str;
    CPU_INT32U len = 0;
    CPU_INT08U exit_while;
    
    time_stamp = OSTimeGet();
    do
    {
        char c;
        NET_SOCK_RTN_CODE ret_code;

        exit_while = 0;
        
        if (NetNIC_ConnStatusGet() != DEF_ON)
        {
            *err = NET_SOCK_ERR_FAULT;
            break;
        }
        
        if (++sleep_ctr > 100)
        {
            OSTimeDly(1);
            sleep_ctr = 0;
        }

        ret_code = NetSock_RxData(sock, &c, 1, NET_SOCK_FLAG_RX_NO_BLOCK, err);
        switch (*err)
        {
            case NET_SOCK_ERR_NONE:
                if (ret_code)
                {
                    time_stamp = OSTimeGet();
                    *str_ptr++ = c;
                    len++;
                    if (c == '\n')
                    {
                        char *ptr = strrchr(str, '\n');
                        if (ptr) *ptr = 0;
                        ptr = strrchr(str, '\r');
                        if (ptr) *ptr = 0;
                        len = strlen(str);
                        *err = NET_SOCK_ERR_NONE;
                        exit_while = 1;
                    }    
                }
                break;
            case NET_SOCK_ERR_RX_Q_EMPTY:
                if (OSTimeGet() - time_stamp < timeout)
                {
                    *err = NET_SOCK_ERR_NONE;
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
                *err = NET_ERR_RX;
                break;
        }

    } while ((*err == NET_SOCK_ERR_NONE) && (!exit_while));

    return len;
}

///
NET_SOCK_ID HostConnectSocket(CPU_INT32U ip_addr, CPU_INT16U port, CPU_INT32U timeout, NET_ERR* err)
{
    NET_SOCK_ID sock;
    NET_SOCK_ADDR_IP   server_sock_addr_ip;
        
    CPU_INT32U time_stamp = OSTimeGet();
        
    sock = NetSock_Open(NET_SOCK_ADDR_FAMILY_IP_V4, NET_SOCK_TYPE_STREAM, NET_SOCK_PROTOCOL_TCP, err);
    if (*err != NET_SOCK_ERR_NONE) 
    {
        return -1;
    }

    memset(&server_sock_addr_ip, 0, sizeof(server_sock_addr_ip));
    server_sock_addr_ip.Family = NET_SOCK_ADDR_FAMILY_IP_V4;
    server_sock_addr_ip.Addr = NET_UTIL_HOST_TO_NET_32(ip_addr);
    server_sock_addr_ip.Port = NET_UTIL_HOST_TO_NET_16(port);

    NetSock_Conn((NET_SOCK_ID)sock, (NET_SOCK_ADDR *)&server_sock_addr_ip, (NET_SOCK_ADDR_LEN)sizeof(server_sock_addr_ip), err);

    while (NetSock_IsConn((NET_SOCK_ID)sock, err) != DEF_YES)
    {
        if ((*err != NET_SOCK_ERR_CONN_IN_PROGRESS) && (*err != NET_SOCK_ERR_NONE))
        {
            break;
        }
        if (OSTimeGet() - time_stamp > timeout)
        {
            NetSock_Close(sock, err);
            return -1;
        }
        OSTimeDly(1);
    }

    if (*err != NET_SOCK_ERR_NONE)
    {
        NetSock_Close(sock, err);
        return -1;
    }

    return sock;
}

///
int HostCheckIpDevice(CPU_INT32U ip_addr, CPU_INT16U port, CPU_INT32U timeout)
{
    int res = -1;
    NET_ERR err;
    NET_SOCK_ID sock = HostConnectSocket(ip_addr, port, timeout, &err);
    if (err != NET_SOCK_ERR_NONE)
    {
        return -1;
    }

    memset(host_tcp_buf, 0, HOST_TCP_BUF_SIZE);
    int len = HostReadLine(sock, host_tcp_buf, HOST_TCP_BUF_SIZE - 1, timeout, &err);
    if (err != NET_SOCK_ERR_NONE)
    {
        NetSock_Close(sock, &err);
        return -2;
    }
    
    if (len > 0)
    {
        if (strcmp(host_tcp_buf, "Welcome to control console!") == 0)
        {
            res = 0;
        }
    }
        
    NetSock_Close(sock, &err);
    return res;
}

/// запись параметра
int HostWriteParam(CPU_INT32U ip_addr, CPU_INT16U port, char* param_str, char* param_val, CPU_INT32U timeout)
{
    int res = -1;
    NET_ERR err;
    NET_SOCK_ID sock = HostConnectSocket(ip_addr, port, timeout, &err);
    if (err != NET_SOCK_ERR_NONE)
    {
        return -1;
    }

    memset(host_tcp_buf, 0, HOST_TCP_BUF_SIZE);
    int len = HostReadLine(sock, host_tcp_buf, HOST_TCP_BUF_SIZE - 1, timeout, &err);
    if (err != NET_SOCK_ERR_NONE)
    {
        NetSock_Close(sock, &err);
        return -2;
    }
    
    if (len > 0)
    {
        if (strcmp(host_tcp_buf, "Welcome to control console!") == 0)
        {
            res = 0;
        }
    }
    
    if (res != 0)
    {
        NetSock_Close(sock, &err);
        return res;
    }
    
    strcpy(host_tcp_buf, "SET ");
    strcat(host_tcp_buf, param_str);
    strcat(host_tcp_buf, " ");
    strcat(host_tcp_buf, param_val);
    strcat(host_tcp_buf, "\n");
        
    if (HostWriteStr(sock, host_tcp_buf) != strlen(host_tcp_buf))
    {
        NetSock_Close(sock, &err);
        return -3;
    }

    memset(host_tcp_buf, 0, HOST_TCP_BUF_SIZE);
    len = HostReadLine(sock, host_tcp_buf, HOST_TCP_BUF_SIZE - 1, timeout, &err);
    if (err != NET_SOCK_ERR_NONE)
    {
        NetSock_Close(sock, &err);
        return -4;
    }
    
    if ((len <= 0) || (strcmp(host_tcp_buf, "OK") != 0))
    {
        res = -5;
    }
    
    NetSock_Close(sock, &err);
    return res;
}

/// запись импульсов
int HostWritePulses(CPU_INT32U ip_addr, CPU_INT32U count, CPU_INT32U len_ms)
{
    char str[16];
    sprintf(str, "%d,%d", (int)count, (int)len_ms);
    return HostWriteParam(ip_addr, CONSOLE_TCP_DEFAULT_PORT, "PULSEOUT", str, HOST_SOCKET_DEFAULT_TIMEOUT);
}

/// задача опроса контроллеров постов по сети
void HostAppTask(void *p_arg)
{
    while (1)
    {
        OSTimeDly(1000);
        host_conn_ctr_all++;
        if (HostCheckIpDevice(0xC0A8008C, CONSOLE_TCP_DEFAULT_PORT, HOST_SOCKET_DEFAULT_TIMEOUT) == 0)
        {
            host_conn_ctr_ok++;
        }
    }
}
    
void InitHostApp()
{
#ifdef HOST_RUN_TASK_ENABLE
    OSTaskCreate(HostAppTask, (void *)0, (OS_STK *)&HodtTaskStk[HOST_TASK_STK_SIZE-1], HOST_TASK_PRIO);
    INT8U err;
    OSTaskNameSet(HOST_TASK_PRIO, "Host App Task", &err);
#endif
}
