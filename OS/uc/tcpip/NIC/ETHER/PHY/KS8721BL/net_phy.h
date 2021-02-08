/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/TCP-IP in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to help
*               you experience uC/TCP-IP.  The fact that the source code is provided does
*               NOT mean that you can use it without paying a licensing fee.
*
*               Network Interface Card (NIC) port files provided, as is, for FREE and do
*               NOT require any additional licensing or licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        NETWORK PHYSICAL LAYER
*
*                                           National DP83848
*
* Filename      : net_phy.c
* Version       : V1.89
* Programmer(s) : EHS
*********************************************************************************************************
* Note(s)       : (1) Supports National Semiconductor DP83848 10/100 PHY
*
*                 (2) The MII interface port is assumed to be part of the host EMAC; consequently,
*                     reads from and writes to the PHY are made through the EMAC.  The functions
*                     NetNIC_PhyRegRd() and NetNIC_PhyRegWr(), which are used to access the PHY, should
*                     be provided in the EMAC driver.
*********************************************************************************************************
*/

#ifndef _NET_PHY_H
#define _NET_PHY_H

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/

#define  DP83848_INIT_AUTO_NEG_RETRIES         3                /* Attempt Auto-Negotiation 3 times         */
#define  DP83848_INIT_RESET_RETRIES           16                /* Check for successful reset 8 times       */

#define  DP83848_OUI                    0x080017
#define  DP83848_VNDR_MDL                   0x09

/*
*********************************************************************************************************
*                                       DP83848 REGISTER DEFINES
*********************************************************************************************************
*/
                                                                /* ------- Generic MII registers ---------- */
#define PHY_BMCR                0x0000
#define PHY_BMSR                0x0001
#define PHY_PHYIDR1             0x0002
#define PHY_PHYIDR2             0x0003
#define PHY_ANAR                0x0004
#define PHY_ANLPAR              0x0005
#define PHY_ANLPARNP            0x0005
#define PHY_ANER                0x0006
#define PHY_ANNPTR              0x0007
#define PHY_LPNPA               0x0008

#define PHY_RECR                0x0015
#define PHY_INTCTRL             0x001B
#define PHY_100PHY              0x001F

/*
*********************************************************************************************************
*                                         DP83848 REGISTER BITS
*********************************************************************************************************
*/
                                                                /* -------- MII_BMCR Register Bits -------- */
/* BMCR bitmap */
#define BMCR_RESET              0x8000
#define BMCR_LOOPBACK           0x4000
#define BMCR_SPEED_100          0x2000
#define BMCR_AN                 0x1000
#define BMCR_POWERDOWN          0x0800
#define BMCR_ISOLATE            0x0400
#define BMCR_RE_AN              0x0200
#define BMCR_DUPLEX             0x0100

/* BMSR bitmap */
#define BMSR_100BE_T4           0x8000
#define BMSR_100TX_FULL         0x4000
#define BMSR_100TX_HALF         0x2000
#define BMSR_10BE_FULL          0x1000
#define BMSR_10BE_HALF          0x0800
#define BMSR_NOPREAM            0x0040
#define BMSR_AUTO_DONE          0x0020
#define BMSR_REMOTE_FAULT       0x0010
#define BMSR_NO_AUTO            0x0008
#define BMSR_LINK_ESTABLISHED   0x0004

/* PHY_ANAR bitmap */
#define ANAR_NEXT_PAGE          0x8000
#define ANAR_REMOTE_FAULT       0x2000
#define ANAR_PAUSE              0x0400
#define ANAR_100BE_T4           0x0200
#define ANAR_100BT_FULL         0x0100
#define ANAR_100BT              0x0080
#define ANAR_10BT_FULL          0x0040
#define ANAR_10BT               0x0020
#define ANAR_SELECTOR           0x001F

/* ANLPAR bitmap */
#define ANLPAR_NEXT_PAGE        0x8000
#define ANLPAR_ACKN             0x4000
#define ANLPAR_REMOTE_FAULT     0x2000
#define ANLPAR_PAUSE            0x0C00
#define ANLPAR_100BE_T4         0x0200
#define ANLPAR_100BT_FULL       0x0100
#define ANLPAR_100BT            0x0080
#define ANLPAR_10BT_FULL        0x0040
#define ANLPAR_10BT             0x0020
#define ANLPAR_SELECTOR         0x001F

/* PHY_100PHY setting */
#define PHYCR_MDIX_DIS        0x2000
#define PHYCR_ENR_DET         0x1000
#define PHYCR_FORCE_LINK      0x0800
#define PHYCR_POWER_SAVING    0x0400
#define PHYCR_INTR_LEVEL      0x0200
#define PHYCR_JABBER_ENA      0x0100
#define PHYCR_PAUSE_ENA       0x0080
#define PHYCR_PHY_ISO         0x0040
#define PHYCR_MODE            0x001C
#define PHYCR_SQE_TST_ENA     0x0002
#define PHYCR_SCRAM_DIS       0x0001

/* MAC PHY address */
#define MAC_PHY_ADDR          1

/* Set these bits in eth->req_speed prior to starting driver */
#define LINKSTAT_SPEED_10           0x1
#define LINKSTAT_SPEED_10_FD        0x2
#define LINKSTAT_SPEED_100          0x4
#define LINKSTAT_SPEED_100_FD       0x8

#define LINKSTAT_LINKDOWN           0x8000
#define LINKSTAT_AUTO_NEGOTIATE     0x4000

/*
*********************************************************************************************************
*                                   PHY ERROR CODES 12,000 -> 13,000
*********************************************************************************************************
*/

#define  NET_PHY_ERR_NONE                  12000
#define  NET_PHY_ERR_REGRD_TIMEOUT         12010
#define  NET_PHY_ERR_REGWR_TIMEOUT         12020
#define  NET_PHY_ERR_AUTONEG_TIMEOUT       12030
#define  NET_PHY_ERR_RESET_TIMEOUT         12040

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

extern void         NetNIC_PhyInit          (NET_ERR      *perr);

extern void         NetNIC_PhyAutoNeg       (void);                    /* Do link auto-negotiation                             */

                                                                /* -----------------PHY STATUS FNCTS ------------------ */
extern CPU_BOOLEAN  NetNIC_PhyAutoNegState  (void);                    /* Get PHY auto-negotiation state                       */

extern CPU_BOOLEAN  NetNIC_PhyLinkState     (void);                    /* Get PHY link state                                   */

extern CPU_INT32U   NetNIC_PhyLinkSpeed     (void);                    /* Get PHY link speed                                   */

extern CPU_INT32U   NetNIC_PhyLinkDuplex    (void);                    /* Get PHY duplex mode                                  */

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                                    DEFINED IN PRODUCT'S  net_bsp.c
*********************************************************************************************************
*/

void         NetNIC_LinkUp           (void);                    /* Message from NIC that the ethernet link is up.       */
                                                                /* Called in interruption context most of the time.     */

void         NetNIC_LinkDown         (void);                    /* Message from NIC that the ethernet link is down.     */
                                                                /* Called in interruption context most of the time.     */


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_NIC_CFG_INT_CTRL_EN
#error  "NET_NIC_CFG_INT_CTRL_EN           not #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#elif  ((NET_NIC_CFG_INT_CTRL_EN != DEF_DISABLED) && \
        (NET_NIC_CFG_INT_CTRL_EN != DEF_ENABLED ))
#error  "NET_NIC_CFG_INT_CTRL_EN     illegally #define'd in 'net_cfg.h'"
#error  "                            [MUST be  DEF_DISABLED]           "
#error  "                            [     ||  DEF_ENABLED ]           "
#endif

#ifndef   EMAC_CFG_PHY_ADDR
#error   "EMAC_CFG_PHY_ADDR           not #define'd in 'net_bsp.h'"
#endif

#ifndef   EMAC_CFG_RMII
#error   "EMAC_CFG_RMII               not #define'd in 'net_bsp.h'"
#error   "                            [MUST be  DEF_YES   ]"
#error   "                            [     ||  DEF_NO    ]"
#elif   ((EMAC_CFG_RMII != DEF_YES) && \
         (EMAC_CFG_RMII != DEF_NO ))
#error   "EMAC_CFG_RMII               illegally #define'd in 'net_bsp.h'"
#error   "                            [MUST be  DEF_YES]"
#error   "                            [     ||  DEF_NO ]"
#endif




#endif
