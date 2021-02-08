#include <includes.h>

#if uC_TCPIP_MODULE > 0
#include <net_phy.h>
#endif

#include "spi.h"
#include "fram.h"
#include "console.h"
#include "fram_map.h"
#include "lcd.h"
#include "keyboard.h"
#include "app_serv.h"
#include "lpc23xx-iap.h"


extern const TFramMap config_params;
extern TFramMap *config_ram;

static  OS_STK     AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  void  AppTaskStart(void  *p_arg);

#if uC_TCPIP_MODULE > 0
static  OS_STK          AppTaskPhyStk[APP_TASK_PHY_STK_SIZE];

        NET_IP_ADDR     AppNetIP;
        NET_IP_ADDR     AppNetMsk;
        NET_IP_ADDR     AppNetGateway;

static  void            AppInitTCPIP                (void);
static  void            AppTaskPhy                  (void *p_arg);
#endif

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.
*
* Arguments   : none
*
* Returns     : none
**********************   ***********************************************************************************
*/

void  main (void)
{
    BSP_IntDisAll();                                            /* Disable all interrupts until we are ready to accept them */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreate(AppTaskStart, (void *)0, (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE-1], APP_TASK_START_PRIO);
    
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}



/*
*********************************************************************************************************
*                                          AppTaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*
*               (2) Interrupts are enabled once the task starts because the I-bit of the CCR register was
*                   set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/
static  void  AppTaskStart (void *p_arg)
{
    (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                                 */

#if OS_TASK_STAT_EN > 0
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

    OSTimeDly(1000);

#ifdef BOARD_CENTRAL_CFG
    // инициализация периферии
    InitLcd();
    InitKbrd();
    SpiInit();
#else
    init_config_params();
#endif

    //OnChangeInitByDefault();

#if uC_TCPIP_MODULE > 0
    AppInitTCPIP();                                             /* Initialize uC/TCP-IP and associated appliations          */
    OSTaskCreate(AppTaskPhy, (void *)0, (OS_STK *)&AppTaskPhyStk[APP_TASK_PHY_STK_SIZE - 1], APP_TASK_PHY_PRIO);
    INT8U err;
    OSTaskNameSet(APP_TASK_PHY_PRIO, "Net Task Phy", &err);
#endif
  
    // пользовательская инициализация
    UserStartupFunc();
    
    while (1) {                                          /* Task body, always written as an infinite loop.           */
      OSTimeDly(1000);
    }
}


/*
*********************************************************************************************************
*                                      AppInit_TCPIP()
*
* Description : This function is called by AppTaskStart() and is responsible for initializing uC/TCP-IP
*               uC/HTTPs, uC/TFTPs and uC/DHCPc if enabled.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

#if uC_TCPIP_MODULE > 0
static  void  AppInitTCPIP (void)
{
    NET_ERR  err;

#ifdef BOARD_CENTRAL_CFG
    // загрузим настройки интерфейса из fram
    ReadArrayFram(offsetof(TFramMap, mac_addr), 6, (unsigned char*)&NetIF_MAC_Addr);
    if (((NetIF_MAC_Addr[0] == 0xff) && (NetIF_MAC_Addr[1] == 0xff) && (NetIF_MAC_Addr[2] == 0xff)
         && (NetIF_MAC_Addr[3] == 0xff) && (NetIF_MAC_Addr[4] == 0xff) && (NetIF_MAC_Addr[5] == 0xff))
      ||
        ((NetIF_MAC_Addr[0] == 0x00) && (NetIF_MAC_Addr[1] == 0x00) && (NetIF_MAC_Addr[2] == 0x00)
         && (NetIF_MAC_Addr[3] == 0x00) && (NetIF_MAC_Addr[4] == 0x00) && (NetIF_MAC_Addr[5] == 0x00))
          )
      {
          // проинициализируем
          NetIF_MAC_Addr[0] = 0x00;
          NetIF_MAC_Addr[1] = 0x50;
          NetIF_MAC_Addr[2] = 0xC2;
          NetIF_MAC_Addr[3] = 0x25;
          NetIF_MAC_Addr[4] = 0x61;
          NetIF_MAC_Addr[5] = 0x34;
          WriteArrayFram(offsetof(TFramMap, mac_addr), 6, (unsigned char*)&NetIF_MAC_Addr);
      }

    ReadArrayFram(offsetof(TFramMap, ip), sizeof(CPU_INT32U), (unsigned char*)&AppNetIP);
    ReadArrayFram(offsetof(TFramMap, netmask), sizeof(CPU_INT32U), (unsigned char*)&AppNetMsk);
    ReadArrayFram(offsetof(TFramMap, gateway), sizeof(CPU_INT32U), (unsigned char*)&AppNetGateway);
    if ((AppNetIP == 0xffffffff) || (AppNetGateway == 0xffffffff) ||
        (AppNetIP == 0x00000000) || (AppNetGateway == 0x00000000))
      {
        // проинициализируем все настройки сети
        AppNetIP = NetASCII_Str_to_IP("192.168.0.100",  &err);      
        WriteArrayFram(offsetof(TFramMap, ip), sizeof(CPU_INT32U), (unsigned char*)&AppNetIP);
        AppNetGateway   = NetASCII_Str_to_IP("192.168.0.1",   &err);
        WriteArrayFram(offsetof(TFramMap, gateway), sizeof(CPU_INT32U), (unsigned char*)&AppNetGateway);
        AppNetMsk       = NetASCII_Str_to_IP("255.255.255.0", &err);
        WriteArrayFram(offsetof(TFramMap, netmask), sizeof(CPU_INT32U), (unsigned char*)&AppNetMsk);
        // порт
        CPU_INT16U port = CONSOLE_TCP_DEFAULT_PORT;
        WriteArrayFram(offsetof(TFramMap, port), sizeof(CPU_INT16U), (unsigned char*)&port);
      }
    
#else
    
    NetIF_MAC_Addr[0] = config_ram->mac_addr[0];
    NetIF_MAC_Addr[1] = config_ram->mac_addr[1];
    NetIF_MAC_Addr[2] = config_ram->mac_addr[2];
    NetIF_MAC_Addr[3] = config_ram->mac_addr[3];
    NetIF_MAC_Addr[4] = config_ram->mac_addr[4];
    NetIF_MAC_Addr[5] = config_ram->mac_addr[5];
          
    AppNetIP = config_ram->ip;
    AppNetGateway = config_ram->gateway;
    AppNetMsk = config_ram->netmask;
          
    config_ram->port = CONSOLE_TCP_DEFAULT_PORT;

#endif
    
    err             = Net_Init();                               /* Initialize uC/TCP-IP                                     */
    err             = NetIP_CfgAddrThisHost(AppNetIP, AppNetMsk);
    err             = NetIP_CfgAddrDfltGateway(AppNetGateway);

    err = err;
}
#endif


/*
*********************************************************************************************************
*                                         PHY STATUS TASK
*
* Description : This task monitors the link state and updates the NetNIC_ConnStatus.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskPhy()' by 'OSTaskCreate()'.
*
* Returns     : none
*********************************************************************************************************
*/
#if uC_TCPIP_MODULE > 0
static  void  AppTaskPhy (void *p_arg)
{
    (void)p_arg;


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */

        OSTimeDlyHMSM(0, 0, 0, 100);

        NetNIC_ConnStatus = NetNIC_PhyLinkState();              /* Set NetNIC_ConnStatus according to link state            */

        if (NetNIC_ConnStatus == DEF_ON) {
            NetNIC_LinkUp();
        } else {
            NetNIC_LinkDown();
        }
    }
}
#endif

#ifdef BOARD_POST_CFG

#pragma location = ".config"
const TFramMap config_params = {
    // mac
    {0x00, 0x50, 0xC2, 0x25, 0x61, 0x36},
    // ip
    0xC0A80065, // 192.168.0.140
    // mask
    0xFFFFFF00, // 255.255.255.0
    // gw
    0xC0A80001, // 192.168.0.1
    //port
    CONSOLE_TCP_DEFAULT_PORT 
};

CPU_INT08U config_ram_placeholder[0x1000];

void init_config_params(void)
{
    memset(config_ram_placeholder, 0, 0x1000);
    config_ram = (TFramMap *)&config_ram_placeholder;
    *config_ram = config_params;
}

void save_config_params(void)
{
    config_ram->port = CONSOLE_TCP_DEFAULT_PORT;
    flashrom_erase((uint8_t *)&config_params);
    flashrom_write((uint8_t *)&config_params, (const uint8_t *)config_ram, 0x1000);
}


#endif
