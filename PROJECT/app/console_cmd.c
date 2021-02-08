#include "console_cmd.h"
#include "fram.h"
#include "fram_map.h"
#include "time.h"
#include "crc16.h"
#include "app_serv.h"
#include "version.h"
#include <stddef.h>
#include <string.h>

TFramMap *config_ram;

///
static int GetVersion(char *str_value)
{
    strcpy(str_value, DEVICE_FW_VERSION);
    return 0;
}

///
static int SetIpAddr(char *str_value)
{
    NET_IP_ADDR  ip_addr;
    NET_ERR   err;
    ip_addr = NetASCII_Str_to_IP((CPU_CHAR*)str_value, &err);
    if (err != NET_ASCII_ERR_NONE)
    {
        return -1;
    }
    #ifdef BOARD_CENTRAL_CFG
    WriteArrayFram(offsetof(TFramMap, ip), sizeof(CPU_INT32U), (unsigned char*)&ip_addr);
    #else
    config_ram->ip = ip_addr;
    #endif
    return 0;
}

///
static int GetIpAddr(char *str_value)
{
    NET_IP_ADDR  ip_addr;
    NET_ERR   err;
    #ifdef BOARD_CENTRAL_CFG
    ReadArrayFram(offsetof(TFramMap, ip), sizeof(CPU_INT32U), (unsigned char*)&ip_addr);
    #else
    ip_addr = config_ram->ip;
    #endif
    NetASCII_IP_to_Str(ip_addr, (CPU_CHAR*)str_value, DEF_NO, &err);
    if (err != NET_ASCII_ERR_NONE)
    {
        return -1;
    }  
    return 0;
}

///
static int SetNetMask(char *str_value)
{
    NET_IP_ADDR  ip_addr;
    NET_ERR   err;
    ip_addr = NetASCII_Str_to_IP((CPU_CHAR*)str_value, &err);
    if (err != NET_ASCII_ERR_NONE)
    {
        return -1;
    }  
    #ifdef BOARD_CENTRAL_CFG
    WriteArrayFram(offsetof(TFramMap, netmask), sizeof(CPU_INT32U), (unsigned char*)&ip_addr);
    #else
    config_ram->netmask = ip_addr;
    #endif
    return 0;
}

///
static int GetNetMask(char *str_value)
{
    NET_IP_ADDR  ip_addr;
    NET_ERR   err;
    #ifdef BOARD_CENTRAL_CFG
    ReadArrayFram(offsetof(TFramMap, netmask), sizeof(CPU_INT32U), (unsigned char*)&ip_addr);
    #else
    ip_addr = config_ram->netmask;
    #endif
    NetASCII_IP_to_Str(ip_addr, (CPU_CHAR*)str_value, DEF_NO, &err);
    if (err != NET_ASCII_ERR_NONE)
    {
        return -1;
    }  
    return 0;
}

///
static int SetGateWay(char *str_value)
{
    NET_IP_ADDR  ip_addr;
    NET_ERR   err;
    ip_addr = NetASCII_Str_to_IP((CPU_CHAR*)str_value, &err);
    if (err != NET_ASCII_ERR_NONE)
    {
        return -1;
    }  
    #ifdef BOARD_CENTRAL_CFG
    WriteArrayFram(offsetof(TFramMap, gateway), sizeof(CPU_INT32U), (unsigned char*)&ip_addr);
    #else
    config_ram->gateway = ip_addr;
    #endif
    return 0;
}

///
static int GetGateWay(char *str_value)
{
    NET_IP_ADDR  ip_addr;
    NET_ERR   err;
    #ifdef BOARD_CENTRAL_CFG
    ReadArrayFram(offsetof(TFramMap, gateway), sizeof(CPU_INT32U), (unsigned char*)&ip_addr);
    #else
    ip_addr = config_ram->gateway;
    #endif
    NetASCII_IP_to_Str(ip_addr, (CPU_CHAR*)str_value, DEF_NO, &err);
    if (err != NET_ASCII_ERR_NONE)
    {
        return -1;
    }  
    return 0;
}

///
static int SetMacAddr(char *str_value)
{
    CPU_INT08U   mac_addr[6];
    NET_ERR   err;
    NetASCII_Str_to_MAC((CPU_CHAR*)str_value, &mac_addr[0], &err);
    if (err != NET_ASCII_ERR_NONE)
    {
        return -1;
    }
    #ifdef BOARD_CENTRAL_CFG
    WriteArrayFram(offsetof(TFramMap, mac_addr), 6, (unsigned char*)&mac_addr[0]);
    #else
    config_ram->mac_addr[0] = mac_addr[0];
    config_ram->mac_addr[1] = mac_addr[1];
    config_ram->mac_addr[2] = mac_addr[2];
    config_ram->mac_addr[3] = mac_addr[3];
    config_ram->mac_addr[4] = mac_addr[4];
    config_ram->mac_addr[5] = mac_addr[5];
    #endif
    return 0;
}

///
static int GetMacAddr(char *str_value)
{
    CPU_INT08U   mac_addr[6];
    NET_ERR   err;
    #ifdef BOARD_CENTRAL_CFG
    ReadArrayFram(offsetof(TFramMap, mac_addr), 6, (unsigned char*)&mac_addr[0]);
    #else
    mac_addr[0] = config_ram->mac_addr[0];
    mac_addr[1] = config_ram->mac_addr[1];
    mac_addr[2] = config_ram->mac_addr[2];
    mac_addr[3] = config_ram->mac_addr[3];
    mac_addr[4] = config_ram->mac_addr[4];
    mac_addr[5] = config_ram->mac_addr[5];
    #endif
    NetASCII_MAC_to_Str((CPU_INT08U*)&mac_addr[0], (CPU_CHAR*)str_value, DEF_NO, &err);
    if (err != NET_ASCII_ERR_NONE)
    {
        return -1;
    }  
    return 0;
}

///
static int SetIpPort(char *str_value)
{
    int port = 0;
    
    if (sscanf(str_value, "%d", &port) == 1)
    {
        if ((port > 0) && (port < 65000))
        {
            #ifdef BOARD_CENTRAL_CFG
            WriteArrayFram(offsetof(TFramMap, port), sizeof(CPU_INT16U), (unsigned char*)&port);
            #else
            config_ram->port = port;
            #endif
            return 0;
        }
    }
    return -1;
}

///
static int GetIpPort(char *str_value)
{
    int port = 0;
    #ifdef BOARD_CENTRAL_CFG
    ReadArrayFram(offsetof(TFramMap, port), sizeof(CPU_INT16U), (unsigned char*)&port);
    #else
    port = config_ram->port;
    #endif
    sprintf(str_value, "%d", port);
    return 0;
}

///
static int SetProductName(char *str_value)
{
    if (strlen(str_value) < 32)
    {
        CPU_INT08U flag[4] = {0xAA, 0x55, 0x81, 0xC3};
        CPU_INT08U name[32];
        WriteArrayFram(offsetof(TFramMap, manual_service_flag), 4, (unsigned char*)&flag);
        memcpy(name, str_value, 32);
        WriteArrayFram(offsetof(TFramMap, manual_service_name), 32, (unsigned char*)&name);
        return 0;
    }
    
    return -1;
}

///
static int GetProductName(char *str_value)
{
    CPU_INT08U flag[4] = {0, 0, 0, 0};
    CPU_INT08U name[32];
    ReadArrayFram(offsetof(TFramMap, manual_service_flag), 4, (unsigned char*)&flag);
    if (flag[0] == 0xAA && flag[1] == 0x55 && flag[2] == 0x81 && flag[3] == 0xC3)
    {
          int ok = 0;
          ReadArrayFram(offsetof(TFramMap, manual_service_name), 32, (unsigned char*)&name);
          for (int i = 0; i < 32; i++)
          {
            if (name[i] == 0) ok = 1;
          }
          if (!ok) GetDataStr(&ServiceNameDesc, (CPU_INT08U*)name, 0, DATA_FLAG_SYSTEM_INDEX);
    }
    else
    {
      GetDataStr(&ServiceNameDesc, (CPU_INT08U*)name, 0, DATA_FLAG_SYSTEM_INDEX);
    }

    strcpy(str_value, (char const*)name);
    
    return 0;
}


#ifdef BOARD_CENTRAL_CFG
static int GetDateTime(char *str_value)
{
    PrintTimeString(str_value, GetTimeSec());
    return 0;
}

static int SetDateTime(char *str_value)
{
    TRTC_Data rtc;
    ScanRTCDateTimeStringRus(str_value, &rtc);
    if (RTCCheckTime(&rtc) == 0)
    {
        RTC_SetTime(&rtc);
        return 0;
    }
    return -1;
}
#endif

#ifdef BOARD_POST_CFG
// формат: кол-во импульсов, длина импульса мс
static int SetPulseOut(char *str_value)
{
    int len_ms = 0, count = 0;
    
    if (sscanf(str_value, "%d,%d", &count, &len_ms) == 2)
    {
        if ((len_ms > 0) && (len_ms <= 1000) && (count > 0) && (count < 1000))
        {
            AddOutPulses(count, len_ms);
            PostUserEvent(EVENT_PULSEOUT);
            return 0;
        }
    }
    return -1;
}
#endif

extern char reboot_flag;

///
static int SetReboot(char *str_value)
{
    reboot_flag = 1;
    return 0;
}


static int GetStatus(char *str_value);

///
static const ParamDesc params[PARAM_COUNT] = 
{
    {"VERSION",     NULL,           GetVersion},
    {"MAC",         SetMacAddr,     GetMacAddr},
    {"IP",          SetIpAddr,      GetIpAddr},
    {"NETMASK",     SetNetMask,     GetNetMask},
    {"GATEWAY",     SetGateWay,     GetGateWay},
    {"PORT",        SetIpPort,      GetIpPort},

#ifdef BOARD_POST_CFG
    {"PULSEOUT",    SetPulseOut,      NULL},
#else
    {"DATETIME",    SetDateTime,    GetDateTime},
    {"PRODUCT",    SetProductName,    GetProductName},
#endif
    
    {"REBOOT",      SetReboot,      NULL},
    {"STATUS",      NULL,           GetStatus},

};

extern int ConsoleWriteStr(char* str);

/// весь статус одним ответом
static int GetStatus(char *str_value)
{
    for (uint16_t i = 0; i < PARAM_COUNT - 1; i++)
    {
        sprintf(str_value, params[i].name);
        strcat(str_value, " ");
        if (params[i].get != NULL)
        {
            params[i].get(&str_value[strlen(str_value)]);
        }
        else
        {
            strcat(str_value, "NULL");
        }
        strcat(str_value, "\n");
        ConsoleWriteStr(str_value);
    }
    
    sprintf(str_value, "STATUS READ");
    return 0;
}

int SetParam(char *cmd)
{
    for (uint16_t i = 0; i < PARAM_COUNT; i++)
    {
        if (strstr(cmd, params[i].name) == cmd)
        {
            if (params[i].set != NULL)
            {
                if (params[i].set(&cmd[strlen(params[i].name) + 1]) == 0)
                {
                    return 0;
                }
                else
                {
                    return -2;
                }
            }
        }
    }
    
    return -1;
}

int GetParam(char *cmd, char *answer, uint16_t len)
{
    for (uint16_t i = 0; i < PARAM_COUNT; i++)
    {
        if (strstr(cmd, params[i].name) == cmd)
        {
            if (params[i].get != NULL)
            {
                if (params[i].get(&answer[0]) == 0)
                {
                    strcat(&answer[0], "\n");
                    return 0;
                }
                else
                {
                    return -2;
                }
            }
        }
    }
    
    return -1;;
}

