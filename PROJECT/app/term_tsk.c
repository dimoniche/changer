#include <includes.h>
#include "app_serv.h"
#include "term_tsk.h"
#include "host_app.h"
#include "data.h"
#include "datadesc.h"
#include "crc16.h"
#include "fram.h"
#include "fram_map.h"
#include <string.h>
#include "time.h"
#include "ftp_app.h"

static  OS_STK  TermTaskStk[TERM_TASK_STK_SIZE];

char term_buffer[TERM_BUFFER_SIZE];

static uint32_t check_counter = 0;
static uint32_t term_state;
static uint32_t term_command_curr;
static uint32_t term_command_new;
static uint32_t term_param_new[TERM_PARAM_COUNT];
static uint32_t rx_packet_len;

extern void SolarClearAllCounters(void);

void ClearTerminalInfo()
{
    TerminalCurrInfo info;
    info.ern = 1;
    info.request_active = 0;
    info.time_syn = 0;
    info.time_sverka = 0;
    info.crc16 = crc16((unsigned char*)&info, offsetof(TerminalCurrInfo, crc16));
    WriteArrayFram(offsetof(TFramMap, terminal_info), sizeof(TerminalCurrInfo), (unsigned char *)&info);
}

///
void LoadTerminalInfo(TerminalCurrInfo* info)
{
    ReadArrayFram(offsetof(TFramMap, terminal_info), sizeof(TerminalCurrInfo), (unsigned char *)info);
    CPU_INT16U crc = crc16((unsigned char*)info, offsetof(TerminalCurrInfo, crc16));
    if (crc != info->crc16)
    {
        info->ern = 1;
        info->request_active = 0;
        info->time_syn = 0;
        info->time_sverka = 0;
        info->crc16 = crc16((unsigned char*)info, offsetof(TerminalCurrInfo, crc16));
        WriteArrayFram(offsetof(TFramMap, terminal_info), sizeof(TerminalCurrInfo), (unsigned char *)info);
    }
}

///
void SaveTerminalInfo(TerminalCurrInfo* info)
{
    info->crc16 = crc16((unsigned char*)info, offsetof(TerminalCurrInfo, crc16));
    WriteArrayFram(offsetof(TFramMap, terminal_info), sizeof(TerminalCurrInfo), (unsigned char *)info);
}

///
int TermFlushSocketRx(NET_SOCK_ID sock, CPU_INT32U timeout, NET_ERR *err)
{
    CPU_INT32U time_stamp;
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

        ret_code = NetSock_RxData(sock, &c, 1, NET_SOCK_FLAG_RX_NO_BLOCK, err);
        switch (*err)
        {
            case NET_SOCK_ERR_NONE:
                if (ret_code)
                {
                    time_stamp = OSTimeGet();
                }
                break;
            case NET_SOCK_ERR_RX_Q_EMPTY:
                if (OSTimeGet() - time_stamp < timeout)
                {
                    *err = NET_SOCK_ERR_NONE;
                    OSTimeDly(2);
                }
                break;
            default:
                OSTimeDly(2);
                break;
        }

    } while ((*err == NET_SOCK_ERR_NONE) && (!exit_while));

    return len;
}

/// 
int TermReadChar(NET_SOCK_ID sock, CPU_INT08U *c, CPU_INT32U timeout)
{
    CPU_INT32U time_stamp = OSTimeGet();
    NET_ERR err = NET_SOCK_ERR_NONE;
    do
    {
        NET_SOCK_RTN_CODE ret_code;
        
        if (NetNIC_ConnStatusGet() != DEF_ON)
        {
            err = NET_SOCK_ERR_FAULT;
            break;
        }

        ret_code = NetSock_RxData(sock, c, 1, NET_SOCK_FLAG_RX_NO_BLOCK, &err);
        switch (err)
        {
            case NET_SOCK_ERR_NONE:
                if (ret_code)
                {
                    return 1;
                }
                break;
            case NET_SOCK_ERR_RX_Q_EMPTY:
                if (OSTimeGet() - time_stamp < timeout)
                {
                    OSTimeDly(2);
                    err = NET_SOCK_ERR_NONE;
                }
                break;
            default:
                OSTimeDly(2);
                break;
        }

    } while (err == NET_SOCK_ERR_NONE);
    
    return 0;
}

///
int TermReadPacket(NET_SOCK_ID sock, CPU_INT08U *packet, CPU_INT32U maxlen, CPU_INT32U timeout, CPU_INT32U proto, NET_ERR *err)
{
    CPU_INT32U len = 0;
    CPU_INT08U c;
    CPU_INT16U packet_len = 0;

    // длина 
    if (!TermReadChar(sock, &packet[0], timeout)) return len;
    len++;
    if (!TermReadChar(sock, &packet[1], timeout)) return len;
    len++;
    packet_len = (uint16_t)packet[0] * 256UL + (uint16_t)packet[1];
    if (packet_len < 2) return 0;

    // метка
    if (!TermReadChar(sock, &packet[2], timeout)) return len;
    len++;
    if (!TermReadChar(sock, &packet[3], timeout)) return len;
    len++;
    if (proto == TERMINAL_PROTOCOL_TTK2)
    {
        if ((packet[2] != 0x97) || (packet[3] != 0xF2)) return 0;
    }
    else if (proto == TERMINAL_PROTOCOL_VTK)
    {
        if ((packet[2] != 0x97) || (packet[3] != 0xFB)) return 0;
    }

    // тэги
    while (len < packet_len + 2)
    {
        CPU_INT16U tag;
        CPU_INT32U tag_len;
            
        tag = 0x0000;
        tag_len = 0;
         
        if (!TermReadChar(sock, &c, timeout)) return len;
        packet[len] = c; len++; if ((len >= packet_len + 2) || (len >= maxlen)) break;

        tag = c;
        if ((tag & 0x1F) == 0x1F)
        {
            if (!TermReadChar(sock, &c, timeout)) return len;
            packet[len] = c; len++; if ((len >= packet_len + 2) || (len >= maxlen)) break;
            tag = (tag << 8) + c;
        }
        
        if (!TermReadChar(sock, &c, timeout)) return len;
        packet[len] = c; len++; if ((len >= packet_len + 2) || (len >= maxlen)) break;        
        if (c & 0x80)
        {
            CPU_INT08U bytes = c & 0x7F;
            if (bytes > 4) break;
            if (tag == 0x9E)
            {
                len -= 2;
                packet_len -= 2;
                packet_len -= bytes;
                packet[0] = packet_len >> 8;
                packet[1] = packet_len & 0xFF;
            }
    
            while (bytes)
            {
                if (!TermReadChar(sock, &c, timeout)) return len;
                if (tag != 0x9E)
                {
                    packet[len] = c; len++; if ((len >= packet_len + 2) || (len >= maxlen)) break;
                }
                tag_len |= c;
                tag_len <<= (8 * (bytes - 1));
                bytes--;
            }
        }
        else
        {
            tag_len = c;
            if (tag == 0x9E)
            {
                len -= 2;
                packet_len -= 2;
                packet[0] = packet_len >> 8;
                packet[1] = packet_len & 0xFF;
            }
        }
        
        if (tag == 0x9E)
        {
            packet_len -= tag_len;
            packet[0] = packet_len >> 8;
            packet[1] = packet_len & 0xFF;
            while (tag_len)
            {
                if (!TermReadChar(sock, &c, timeout)) return len;
                tag_len--;
            }
        }
        else
        {
            while (tag_len)
            {
                if (!TermReadChar(sock, &c, timeout)) return len;
                packet[len] = c; len++; if ((len >= packet_len + 2) || (len >= maxlen)) break;
                tag_len--;
            }
        }
    }

    return len;
}

///
uint32_t GetTermState(void)
{
    uint32_t state;
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    OS_ENTER_CRITICAL();
    state = term_state;
    OS_EXIT_CRITICAL();
    return state;
}

///
uint32_t SetTermCommand(uint32_t cmd, uint32_t* param)
{
    uint32_t curr_cmd;
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif
    OS_ENTER_CRITICAL();
    curr_cmd = term_command_curr;
    term_command_new |= cmd;
    if (param)
    {
        term_param_new[0] = param[0];
    }
    OS_EXIT_CRITICAL();
    return curr_cmd;
}

/// создать заголовок
uint32_t ttk2_tag_header(char *buf)
{
    buf[2] = '\x96';
    buf[3] = '\xf2';
    return 4;
}

/// создать заголовок
uint32_t vtk_tag_header(char *buf)
{
    buf[2] = '\x96';
    buf[3] = '\xfb';
    return 4;
}

/// задать длину
uint32_t ttk2_tag_setlen(char *buf, uint16_t len)
{
    buf[0] = (len >> 8) & 0xFF;
    buf[1] = len & 0xFF;
    return 2;
}

/// записать тэг, возвращает длину
uint32_t ttk2_tag_add(char *buf, uint16_t id, char *value, uint8_t value_len)
{
    buf[0] = (char)(id & 0xFF);
    buf[1] = (char)(value_len & 0xFF);
    memcpy(&buf[2], value, value_len);
    return value_len + 2;
}

///
int ttk2_get_field(CPU_INT08U *packet, uint32_t packet_len, uint16_t field, uint8_t *field_data, uint32_t maxlen)
{
    CPU_INT08U *data = packet + 4;
    int len = packet_len - 4;

    while (len > 0)
    {
        uint16_t tag = 0x0000;
        uint16_t taglen = 0;
        uint16_t offset = 0;
        
        tag |= data[0];
        offset = 1;
        if ((tag & 0x1F) == 0x1F)
        {
            tag = (tag << 8) + data[1];
            offset++;
        }
        
        taglen = data[offset++];
        if (taglen & 0x80)
        {
            uint8_t bytecount = taglen & 0x7F;
            taglen = 0;
            while (bytecount > 0)
            {
                taglen += (CPU_INT32U)data[offset] << (8 * (bytecount - 1));
                bytecount--;
                offset++;
            }
        }
        
        if (tag == field)
        {
            if (maxlen >= taglen)
            {
                memcpy(field_data, &data[offset], taglen);
                return taglen;
            }
            else
            {
                return -1;
            }
        }
        
        data += (offset + taglen);
        len -= (offset + taglen);
    }
    
    return 0;
}

/// получение параметров из последнего удачного запроса (информация для чека)
int ttk2_get_field_string(uint16_t field, char* str, uint16_t maxlen)
{
    int tag_size;

    memset(str, 0, maxlen);
    tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, rx_packet_len, field, (uint8_t*)str, maxlen);
    if (tag_size > 0)
    {
        return tag_size;
    }
    
    return 0;
}

/// 
CPU_INT32U term_protocol(void)
{
    CPU_INT32U proto;
    GetData(&TerminalProtocolDesc, &proto, 0, DATA_FLAG_SYSTEM_INDEX);
    return proto;
}


CPU_INT32U op_timeout = 0;
CPU_INT32U op_number = 0;

/// 
void TermAppTask(void *p_arg)
{
    TerminalCurrInfo terminal_info;
    CPU_INT32U time_stamp;
    CPU_INT32U idl_stamp;

    LoadTerminalInfo(&terminal_info);
    idl_stamp = OSTimeGet();
    
    while (1)
    {
        CPU_INT32U enabled;
        CPU_INT32U proto;
        GetData(&EnableTerminalDesc, &enabled, 0, DATA_FLAG_SYSTEM_INDEX);
  
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
                    #if defined(BOARD_SOLARIUM_VLAD)
                          SolarClearAllCounters();
                    #endif
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
        if (!enabled)
        {
            OSTimeDly(1000);
            term_state = TERM_STATE_IDLE;
            term_command_new = 0;
            continue;
        }
    
        GetData(&TerminalProtocolDesc, &proto, 0, DATA_FLAG_SYSTEM_INDEX);
        if (proto == TERMINAL_PROTOCOL_VTK)
        {
            // дальше протокол VTK
            // периодический запрос IDL
            if (OSTimeGet() - idl_stamp > op_timeout * 1000UL / 2)
            {
                NET_ERR err;
                CPU_INT32U ip, port;
                idl_stamp = OSTimeGet();
                GetData(&TerminalIpAddrDesc, &ip, 0, DATA_FLAG_SYSTEM_INDEX);
                GetData(&TerminalPortDesc, &port, 0, DATA_FLAG_SYSTEM_INDEX);

                NET_SOCK_ID sock = HostConnectSocket(ip, port, TERM_SOCKET_CONNECT_TIMEOUT, &err);
                if ((err == NET_SOCK_ERR_NONE) && (sock >= 0))
                {
                    uint32_t packet_len;
                    char *content;
                            
                    // заголовок
                    vtk_tag_header(term_buffer);
                    // содержимое
                    content = &term_buffer[4];
                    packet_len = 0;
                    // код операции
                    packet_len += ttk2_tag_add(&content[packet_len], 0x01, "IDL", 3);
                    ttk2_tag_setlen(term_buffer, packet_len + 2);
                    // общая длина для передачи
                    packet_len += 4;
                        
                    OSTimeDly(50);
                    int writed = HostWriteData(sock, term_buffer, packet_len);
                    OSTimeDly(50);
                    if (writed == packet_len)
                    {
                        int plen;
                        memset(term_buffer, 0, TERM_BUFFER_SIZE);
                        plen = TermReadPacket(sock, (CPU_INT08U *)term_buffer, TERM_BUFFER_SIZE, TERM_READ_PACKET_TIMEOUT, proto, &err);
                        if ((plen > 0) && (plen == (CPU_INT16U)term_buffer[0] * 256 + (CPU_INT16U)term_buffer[1] + 2))
                        {
                            char opcode[16];
                            int tag_size;
                            memset(opcode, 0, 16);
                            // код сообщения
                            tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x01, (uint8_t*)opcode, 15);
                            if ((tag_size == 3) && (strcmp(opcode, "IDL") == 0))
                            {
                                // таймаут операции
                                int value;
                                memset(opcode, 0, 16);
                                tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x06, (uint8_t*)opcode, 15);
                                if (sscanf(opcode, "%d", &value) == 1)
                                {
                                    op_timeout = value;
                                }
                                // номер операции
                                memset(opcode, 0, 16);
                                tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x03, (uint8_t*)opcode, 15);
                                if (sscanf(opcode, "%d", &value) == 1)
                                {
                                    op_number = value;
                                }
                            }
                        }
                    }
                        
                    NetSock_Close(sock, &err);
                }
                else
                {
                    OSTimeDly(10000);
                }
            }
            
            if (term_command_new & TERM_COMMAND_PEND_PUR)
            {
                term_command_new &= ~TERM_COMMAND_PEND_PUR;
                term_state = TERM_STATE_PENDING_PUR;
             
                NET_ERR err;
                CPU_INT32U ip, port;
                GetData(&TerminalIpAddrDesc, &ip, 0, DATA_FLAG_SYSTEM_INDEX);
                GetData(&TerminalPortDesc, &port, 0, DATA_FLAG_SYSTEM_INDEX);

                NET_SOCK_ID sock = HostConnectSocket(ip, port, TERM_SOCKET_CONNECT_TIMEOUT, &err);
                if (err != NET_SOCK_ERR_NONE)
                {
                    term_state = TERM_STATE_ERR_PUR;
                }
                else
                {
                    uint32_t packet_len;
                    char *content;
                    char str[16];
                            
                    term_state = TERM_STATE_WAITING_PUR;
                    term_command_new &= ~TERM_COMMAND_ABORT_PUR;

                    vtk_tag_header(term_buffer);
                    
                    content = &term_buffer[4];
                    packet_len = 0;
                    // код операции
                    packet_len += ttk2_tag_add(&content[packet_len], 0x01, "VRP", 3);
                    // номер операции
                    ++op_number;
                    sprintf(str, "%d", op_number);
                    packet_len += ttk2_tag_add(&content[packet_len], 0x03, str, strlen(str)); 
                    // transaction amount (money * 100)
                    int money = term_param_new[0];
                    #if !defined(BOARD_SOLARIUM_VLAD)
                    money *= 100;
                    #endif
                    sprintf(str, "%d", money);
                    packet_len += ttk2_tag_add(&content[packet_len], 0x04, str, strlen(str)); 
                    // product id
                    packet_len += ttk2_tag_add(&content[packet_len], 0x09, "1", 1); 
                    // timeout
                    packet_len += ttk2_tag_add(&content[packet_len], 0x06, "60", 1); 
                    // длина в пакете
                    ttk2_tag_setlen(term_buffer, packet_len + 2);
                    // общая длина для передачи
                    packet_len += 4;
                    
                    OSTimeDly(50);
                    int writed = HostWriteData(sock, term_buffer, packet_len);
                    OSTimeDly(50);
                    if (writed != packet_len)
                    {
                        term_state = TERM_STATE_ERR_PUR;
                    }
                    else
                    {
                        int plen;
                        
                        time_stamp = OSTimeGet();
                        do
                        {
                            char opcode[16];
                            int tag_size;

                            if (OSTimeGet() - time_stamp > 120000)
                            {
                                term_state = TERM_STATE_ERR_PUR;
                                break;
                            }
                            
                            if (term_command_new & TERM_COMMAND_ABORT_PUR)
                            {
                                uint32_t packet_len;
                                char *content;
                                char str[16];
                            
                                term_state = TERM_STATE_ABORTING;
                                term_command_new &= ~TERM_COMMAND_ABORT_PUR;

                                vtk_tag_header(term_buffer);
                    
                                content = &term_buffer[4];
                                packet_len = 0;
                                // код операции
                                packet_len += ttk2_tag_add(&content[packet_len], 0x01, "ABR", 3);
                                // номер операции
                                sprintf(str, "%d", op_number);
                                packet_len += ttk2_tag_add(&content[packet_len], 0x03, str, strlen(str)); 
                                // длина в пакете
                                ttk2_tag_setlen(term_buffer, packet_len + 2);
                                // общая длина для передачи
                                packet_len += 4;
                    
                                int writed = HostWriteData(sock, term_buffer, packet_len);
                                OSTimeDly(100);
                    
                                term_state = TERM_STATE_IDLE;
                                break;
                            }
                                
                            memset(term_buffer, 0, TERM_BUFFER_SIZE);
                            plen = TermReadPacket(sock, (CPU_INT08U *)term_buffer, TERM_BUFFER_SIZE, TERM_READ_PACKET_TIMEOUT, TERMINAL_PROTOCOL_VTK, &err);
                            
                            if ((plen > 0) && (plen == (CPU_INT16U)term_buffer[0] * 256 + (CPU_INT16U)term_buffer[1] + 2))
                            {
                                memset(opcode, 0, 16);
                                tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x01, (uint8_t*)opcode, 3);
                                if (strcmp(opcode, "VRP") == 0)
                                {
                                    memset(opcode, 0, 16);
                                    tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x03, (uint8_t*)opcode, 15);
                                    sprintf(str, "%d", op_number);
                                    if ((tag_size > 0) && (strcmp(opcode, str) == 0))
                                    {
                                        memset(opcode, 0, 16);
                                        sprintf(str, "%d", money);
                                        tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x04, (uint8_t*)opcode, 15);
                                        if ((tag_size > 0) && (strcmp(opcode, str) == 0))
                                        {
                                            // VRP OK, посылаем FIN
                                            vtk_tag_header(term_buffer);
                    
                                            content = &term_buffer[4];
                                            packet_len = 0;
                                            // код операции
                                            packet_len += ttk2_tag_add(&content[packet_len], 0x01, "FIN", 3);
                                            // номер операции
                                            sprintf(str, "%d", op_number);
                                            packet_len += ttk2_tag_add(&content[packet_len], 0x03, str, strlen(str)); 
                                            // transaction amount (money * 100)
                                            sprintf(str, "%d", money);
                                            packet_len += ttk2_tag_add(&content[packet_len], 0x04, str, strlen(str)); 
                                            // product id
                                            packet_len += ttk2_tag_add(&content[packet_len], 0x09, "1", 1); 
                                            // длина в пакете
                                            ttk2_tag_setlen(term_buffer, packet_len + 2);
                                            // общая длина для передачи
                                            packet_len += 4;
                                            
                                            OSTimeDly(50);
                                            int writed = HostWriteData(sock, term_buffer, packet_len);
                                            OSTimeDly(50);
                                            if (writed == packet_len)
                                            {
                                                memset(term_buffer, 0, TERM_BUFFER_SIZE);
                                                plen = TermReadPacket(sock, (CPU_INT08U *)term_buffer, TERM_BUFFER_SIZE, TERM_READ_PACKET_TIMEOUT, TERMINAL_PROTOCOL_VTK, &err);
                                                
                                                if ((plen > 0) && (plen == (CPU_INT16U)term_buffer[0] * 256 + (CPU_INT16U)term_buffer[1] + 2))
                                                {
                                                    memset(opcode, 0, 16);
                                                    tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x01, (uint8_t*)opcode, 3);
                                                    if (strcmp(opcode, "FIN") == 0)
                                                    {
                                                        memset(opcode, 0, 16);
                                                        tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x03, (uint8_t*)opcode, 15);
                                                        sprintf(str, "%d", op_number);
                                                        if ((tag_size > 0) && (strcmp(opcode, str) == 0))
                                                        {
                                                            memset(opcode, 0, 16);
                                                            sprintf(str, "%d", money);
                                                            tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x04, (uint8_t*)opcode, 15);
                                                            if ((tag_size > 0) && (strcmp(opcode, str) == 0))
                                                            {
                                                                term_state = TERM_STATE_DONE_PUR;
                                                                break;
                                                            }
                                                            else
                                                            {
                                                                term_state = TERM_STATE_ERR_PUR;
                                                                break;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            term_state = TERM_STATE_ERR_PUR;
                                                            break;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        term_state = TERM_STATE_ERR_PUR;
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    term_state = TERM_STATE_ERR_PUR;
                                                    break;
                                                }
                                            }
                                            else
                                            {
                                                term_state = TERM_STATE_ERR_PUR;
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            term_state = TERM_STATE_ERR_PUR;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        term_state = TERM_STATE_ERR_PUR;
                                        break;
                                    }
                                }
                                else
                                {
                                    term_state = TERM_STATE_ERR_PUR;
                                    break;
                                }
                            }
                        } while (1);
                        
                        op_timeout = 0;
                    }
                    
                    OSTimeDly(250);
                    NetSock_Close(sock, &err);
                }
            }
 
            if (term_command_new & TERM_COMMAND_CLEAR_STATE)
            {
                term_command_new &= ~TERM_COMMAND_CLEAR_STATE;
                term_command_new &= ~TERM_COMMAND_ABORT_PUR;
                term_state = TERM_STATE_IDLE;
            }
        
            OSTimeDly(100);
            continue;
        }
        
        // дальше протокол ТТК2 
        if (term_state == TERM_STATE_IDLE)
        {
            if (term_command_new & TERM_COMMAND_MAKE_SVERKA)
            {
                term_command_new &= ~TERM_COMMAND_MAKE_SVERKA;
                
                NET_ERR err;
                CPU_INT32U ip, port;
                GetData(&TerminalIpAddrDesc, &ip, 0, DATA_FLAG_SYSTEM_INDEX);
                GetData(&TerminalPortDesc, &port, 0, DATA_FLAG_SYSTEM_INDEX);

                NET_SOCK_ID sock = HostConnectSocket(ip, port, TERM_SOCKET_CONNECT_TIMEOUT, &err);
                if (err == NET_SOCK_ERR_NONE)
                {
                    uint32_t packet_len;
                    char *content;
                    char str[16];

                    LoadTerminalInfo(&terminal_info);
                    
                    ttk2_tag_header(term_buffer);
                    
                    content = &term_buffer[4];
                    packet_len = 0;
                    // код операции
                    packet_len += ttk2_tag_add(&content[packet_len], 0x01, "SRV", 3);
                    // номер клиента
                    packet_len += ttk2_tag_add(&content[packet_len], 0x02, "1", 1); 
                    // номер документа (ERN)
                    sprintf(str, "%d", terminal_info.ern);
                    packet_len += ttk2_tag_add(&content[packet_len], 0x03, str, strlen(str)); 
                    // код операции
                    packet_len += ttk2_tag_add(&content[packet_len], 0x1A, "2", 1); 

                    ttk2_tag_setlen(term_buffer, packet_len + 2);
                    // общая длина для передачи
                    packet_len += 4;
                    
                    OSTimeDly(50);
                    int writed = HostWriteData(sock, term_buffer, packet_len);
                    OSTimeDly(50);
                    if (writed == packet_len)
                    {
                        int plen;
                        
                        time_stamp = OSTimeGet();
                        do
                        {
                            char opcode[4];
                            int tag_size;

                            if (OSTimeGet() - time_stamp > 120000)
                            {
                                break;
                            }
                                
                            memset(term_buffer, 0, TERM_BUFFER_SIZE);
                            plen = TermReadPacket(sock, (CPU_INT08U *)term_buffer, TERM_BUFFER_SIZE, TERM_READ_PACKET_TIMEOUT, TERMINAL_PROTOCOL_TTK2, &err);
                            
                            if (plen == 0)
                            {
                                continue;
                            }
                            
                            if (plen != (CPU_INT16U)term_buffer[0] * 256 + (CPU_INT16U)term_buffer[1] + 2)
                            {
                                continue;
                            }

                            if (term_buffer[2] != '\x97' || term_buffer[3] != '\xF2')
                            {
                                break;
                            }

                            memset(opcode, 0, 4);
                            tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x81, (uint8_t*)opcode, 3);
                            
                            if (tag_size == 0)
                            {
                                continue;
                            }
                            else if (tag_size < 0)
                            {
                                break;
                            }
                            else
                            {
                                if (strcmp(opcode, "SRV") == 0)
                                {
                                    char pur_result = 'N';
                                    tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0xA1, (uint8_t*)&pur_result, 1);
                                    if ((tag_size > 0) && (pur_result == 'Y'))
                                    {
                                    }
                                    else
                                    {
                                    }
                                    
                                    break;
                                }
                                else if (strcmp(opcode, "INF") == 0)
                                {
                                    continue;
                                }
                            }
                            
                        } while (1);
                        
                        terminal_info.ern++;
                        SaveTerminalInfo(&terminal_info);
                    }
                }

                NetSock_Close(sock, &err);
            }
            
            if (term_command_new & TERM_COMMAND_MAKE_SYNCHRO)
            {
                term_command_new &= ~TERM_COMMAND_MAKE_SYNCHRO;
                NET_ERR err;
                CPU_INT32U ip, port;
                GetData(&TerminalIpAddrDesc, &ip, 0, DATA_FLAG_SYSTEM_INDEX);
                GetData(&TerminalPortDesc, &port, 0, DATA_FLAG_SYSTEM_INDEX);

                NET_SOCK_ID sock = HostConnectSocket(ip, port, TERM_SOCKET_CONNECT_TIMEOUT, &err);
                if (err == NET_SOCK_ERR_NONE)
                {
                    uint32_t packet_len;
                    char *content;
                    char str[16];
                            
                    LoadTerminalInfo(&terminal_info);
                    
                    ttk2_tag_header(term_buffer);
                    
                    content = &term_buffer[4];
                    packet_len = 0;
                    // код операции
                    packet_len += ttk2_tag_add(&content[packet_len], 0x01, "SRV", 3);
                    // номер клиента
                    packet_len += ttk2_tag_add(&content[packet_len], 0x02, "1", 1); 
                    // номер документа (ERN)
                    sprintf(str, "%d", terminal_info.ern);
                    packet_len += ttk2_tag_add(&content[packet_len], 0x03, str, strlen(str)); 
                    // код операции
                    packet_len += ttk2_tag_add(&content[packet_len], 0x1A, "8", 1); 

                    ttk2_tag_setlen(term_buffer, packet_len + 2);
                    // общая длина для передачи
                    packet_len += 4;
                    
                    OSTimeDly(50);
                    int writed = HostWriteData(sock, term_buffer, packet_len);
                    OSTimeDly(50);
                    if (writed == packet_len)
                    {
                        int plen;
                        
                        time_stamp = OSTimeGet();
                        do
                        {
                            char opcode[4];
                            int tag_size;

                            if (OSTimeGet() - time_stamp > 120000)
                            {
                                break;
                            }
                                
                            memset(term_buffer, 0, TERM_BUFFER_SIZE);
                            plen = TermReadPacket(sock, (CPU_INT08U *)term_buffer, TERM_BUFFER_SIZE, TERM_READ_PACKET_TIMEOUT, TERMINAL_PROTOCOL_TTK2, &err);
                            
                            if (plen == 0)
                            {
                                continue;
                            }
                            
                            if (plen != (CPU_INT16U)term_buffer[0] * 256 + (CPU_INT16U)term_buffer[1] + 2)
                            {
                                continue;
                            }

                            if (term_buffer[2] != '\x97' || term_buffer[3] != '\xF2')
                            {
                                break;
                            }

                            memset(opcode, 0, 4);
                            tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x81, (uint8_t*)opcode, 3);
                            
                            if (tag_size == 0)
                            {
                                continue;
                            }
                            else if (tag_size < 0)
                            {
                                break;
                            }
                            else
                            {
                                if (strcmp(opcode, "SRV") == 0)
                                {
                                    char pur_result = 'N';
                                    tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0xA1, (uint8_t*)&pur_result, 1);
                                    if ((tag_size > 0) && (pur_result == 'Y'))
                                    {
                                    }
                                    else
                                    {
                                    }
                                    
                                    break;
                                }
                                else if (strcmp(opcode, "INF") == 0)
                                {
                                    continue;
                                }
                            }
                            
                        } while (1);
                        
                        terminal_info.ern++;
                        SaveTerminalInfo(&terminal_info);
                    }
                }

                NetSock_Close(sock, &err);
            }
                 
            if (term_command_new & TERM_COMMAND_PEND_PUR)
            {
                term_command_new &= ~TERM_COMMAND_PEND_PUR;
                term_state = TERM_STATE_PENDING_PUR;
             
                NET_ERR err;
                CPU_INT32U ip, port;
                GetData(&TerminalIpAddrDesc, &ip, 0, DATA_FLAG_SYSTEM_INDEX);
                GetData(&TerminalPortDesc, &port, 0, DATA_FLAG_SYSTEM_INDEX);

                NET_SOCK_ID sock = HostConnectSocket(ip, port, TERM_SOCKET_CONNECT_TIMEOUT, &err);
                if (err != NET_SOCK_ERR_NONE)
                {
                    term_state = TERM_STATE_ERR_PUR;
                }
                else
                {
                    uint32_t packet_len;
                    char *content;
                    char str[16];
                            
                    term_state = TERM_STATE_WAITING_PUR;
                    term_command_new &= ~TERM_COMMAND_ABORT_PUR;

                    LoadTerminalInfo(&terminal_info);
                    
                    ttk2_tag_header(term_buffer);
                    
                    content = &term_buffer[4];
                    packet_len = 0;
                    // код операции
                    packet_len += ttk2_tag_add(&content[packet_len], 0x01, "PUR", 3);
                    // номер клиента
                    packet_len += ttk2_tag_add(&content[packet_len], 0x02, "1", 1); 
                    // номер документа (ERN)
                    sprintf(str, "%d", terminal_info.ern);
                    packet_len += ttk2_tag_add(&content[packet_len], 0x03, str, strlen(str)); 
                    // transaction amount (money * 100)
                    int money = term_param_new[0];
                    #if !defined(BOARD_SOLARIUM_VLAD)
                    money *= 100;
                    #endif
                    sprintf(str, "%d", money);
                    packet_len += ttk2_tag_add(&content[packet_len], 0x04, str, strlen(str)); 
                    // transaction mode (bit field)
                    packet_len += ttk2_tag_add(&content[packet_len], 0x08, "\xC0", 1); 

                    ttk2_tag_setlen(term_buffer, packet_len + 2);
                    // общая длина для передачи
                    packet_len += 4;
                    
                    // сохраним признак операции
                    terminal_info.request_active = 1;
                    SaveTerminalInfo(&terminal_info);
                        
                    OSTimeDly(50);
                    int writed = HostWriteData(sock, term_buffer, packet_len);
                    OSTimeDly(50);
                    if (writed != packet_len)
                    {
                        term_state = TERM_STATE_ERR_PUR;
                        // надо отменить, наверное
                        terminal_info.ern++;
                        terminal_info.request_active = 0;
                        SaveTerminalInfo(&terminal_info);
                    }
                    else
                    {
                        int plen;
                        
                        time_stamp = OSTimeGet();
                        do
                        {
                            char opcode[4];
                            int tag_size;

                            if (OSTimeGet() - time_stamp > 120000)
                            {
                                term_state = TERM_STATE_ERR_PUR;
                                break;
                            }
                            
                            if (term_command_new & TERM_COMMAND_ABORT_PUR)
                            {
                                uint32_t packet_len;
                                char *content;
                                char str[16];
                            
                                term_state = TERM_STATE_ABORTING;
                                term_command_new &= ~TERM_COMMAND_ABORT_PUR;

                                LoadTerminalInfo(&terminal_info);
                    
                                ttk2_tag_header(term_buffer);
                    
                                content = &term_buffer[4];
                                packet_len = 0;
                                // код операции
                                packet_len += ttk2_tag_add(&content[packet_len], 0x01, "ABR", 3);
                                // номер клиента
                                packet_len += ttk2_tag_add(&content[packet_len], 0x02, "1", 1); 
                                // номер документа (ERN)
                                sprintf(str, "%d", terminal_info.ern);
                                packet_len += ttk2_tag_add(&content[packet_len], 0x03, str, strlen(str)); 
        
                                ttk2_tag_setlen(term_buffer, packet_len + 2);
                                // общая длина для передачи
                                packet_len += 4;
                    
                                int writed = HostWriteData(sock, term_buffer, packet_len);
                                OSTimeDly(100);
                    
                                // сбросим признак операции
                                terminal_info.request_active = 0;
                                terminal_info.ern++;
                                SaveTerminalInfo(&terminal_info);
                                term_state = TERM_STATE_IDLE;
                                break;
                            }
                                
                            memset(term_buffer, 0, TERM_BUFFER_SIZE);
                            plen = TermReadPacket(sock, (CPU_INT08U *)term_buffer, TERM_BUFFER_SIZE, TERM_READ_PACKET_TIMEOUT, TERMINAL_PROTOCOL_TTK2, &err);
                            
                            if (plen == 0)
                            {
                                continue;
                            }
                            
                            if (plen != (CPU_INT16U)term_buffer[0] * 256 + (CPU_INT16U)term_buffer[1] + 2)
                            {
                                continue;
                            }

                            if (term_buffer[2] != '\x97' || term_buffer[3] != '\xF2')
                            {
                                term_state = TERM_STATE_ERR_PUR;
                                break;
                            }

                            memset(opcode, 0, 4);
                            tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x81, (uint8_t*)opcode, 3);
                            
                            if (tag_size == 0)
                            {
                                continue;
                            }
                            else if (tag_size < 0)
                            {
                                term_state = TERM_STATE_ERR_PUR;
                                break;
                            }
                            else
                            {
                                if (strcmp(opcode, "PUR") == 0)
                                {
                                    char pur_result = 'N';
                                    tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0xA1, (uint8_t*)&pur_result, 1);
                                    if ((tag_size > 0) && (pur_result == 'Y'))
                                    {
                                        term_state = TERM_STATE_DONE_PUR;
                                        rx_packet_len = plen;
                                    }
                                    else
                                    {
                                        term_state = TERM_STATE_ERR_PUR;
                                        rx_packet_len = 0;
                                    }
                                    
                                    // Код ответа
                                    char resp_code[3] = "\x0\x0\x0";
                                    tag_size = ttk2_get_field((CPU_INT08U *)term_buffer, plen, 0x9B, (uint8_t*)&resp_code, 2);
                                    if (tag_size > 0)
                                    {
                                        #if OS_CRITICAL_METHOD == 3
                                        OS_CPU_SR  cpu_sr = 0;
                                        #endif
                                        if (strcmp(resp_code, "JF") == 0)
                                        {
                                            OS_ENTER_CRITICAL();
                                            term_command_new |= TERM_COMMAND_MAKE_SVERKA;
                                            OS_EXIT_CRITICAL();
                                        }
                                        else if (strcmp(resp_code, "BB") == 0)
                                        {
                                            // требуется синхронизация журнала
                                            OS_ENTER_CRITICAL();
                                            term_command_new |= TERM_COMMAND_MAKE_SYNCHRO;
                                            OS_EXIT_CRITICAL();
                                        }
                                    }
                                    
                                    break;
                                }
                                else if (strcmp(opcode, "INF") == 0)
                                {
                                    continue;
                                }
                            }
                            
                        } while (1);
                        
                        terminal_info.ern++;
                        terminal_info.request_active = 0;
                        SaveTerminalInfo(&terminal_info);
                    }

                    NetSock_Close(sock, &err);
                }
            }
            
            if (++check_counter > 100)
            {
                check_counter = 0;
                LoadTerminalInfo(&terminal_info);
                if (GetTimeSec() - terminal_info.time_sverka > 60 * 60 * 3)
                {
                    #if OS_CRITICAL_METHOD == 3
                    OS_CPU_SR  cpu_sr = 0;
                    #endif
                    OS_ENTER_CRITICAL();
                    term_command_new |= TERM_COMMAND_MAKE_SVERKA;
                    OS_EXIT_CRITICAL();
                    terminal_info.time_sverka = GetTimeSec();
                }
                if (GetTimeSec() - terminal_info.time_syn > 60 * 60 * 3)
                {
                    #if OS_CRITICAL_METHOD == 3
                    OS_CPU_SR  cpu_sr = 0;
                    #endif
                    OS_ENTER_CRITICAL();
                    term_command_new |= TERM_COMMAND_MAKE_SYNCHRO;
                    OS_EXIT_CRITICAL();
                    terminal_info.time_syn = GetTimeSec();
                }
                SaveTerminalInfo(&terminal_info);
            }

        }
        else if (term_state == TERM_STATE_WAITING_PUR)
        {
            term_command_new |= TERM_COMMAND_CLEAR_STATE;
        }
        else if (term_command_new & TERM_COMMAND_CLEAR_STATE)
        {
            term_command_new &= ~TERM_COMMAND_CLEAR_STATE;
            term_command_new &= ~TERM_COMMAND_ABORT_PUR;
            term_state = TERM_STATE_IDLE;
        }
            
        OSTimeDly(100);
    }
}
    
void InitTerminalApp()
{
    term_command_new = term_command_curr = TERM_COMMAND_NONE;
    term_state = TERM_STATE_IDLE;
    OSTaskCreate(TermAppTask, (void *)0, (OS_STK *)&TermTaskStk[TERM_TASK_STK_SIZE-1], TERM_TASK_PRIO);
    INT8U err;
    OSTaskNameSet(TERM_TASK_PRIO, "Terminal Task", &err);
}
