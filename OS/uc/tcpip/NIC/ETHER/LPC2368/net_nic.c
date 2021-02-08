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
*                                        NETWORK INTERFACE CARD
*
*                                          LPC23XX/LPC24XX EMAC
*
* Filename      : net_nic.c
* Version       : V1.89
* Programmer(s) : EHS
*********************************************************************************************************
* Note(s)       : (1) Supports EMAC section of NXP's LPC23XX and LPC24XX microcontrollers
*                     as described in
*
*                         NXP Corporation (NXP; http://www.nxp.com).
*                         (a) "LPC2364/6/8/78 User Manual" (UM10211)    Revision 01 ( 6 October  2006)
*                         (b) "LPC2468 User Manual" (UM10237)           Revision 01 (18 December 2006)
*
*                 (2) REQUIREs Ethernet Network Interface Layer located in the following network directory :
*
*                         \<Network Protocol Suite>\IF\Ether\
*
*                              where
*                                      <Network Protocol Suite>    directory path for network protocol suite.
*
*                 (2) Since the EMAC module is integrated into either an LPC23XX or LPC24XX microcontroller,
*                     the endianness of the registers is the same as the CPU, which is little endian by
*                     default.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_NIC_MODULE
#include  <net.h>
#include  <net_phy.h>
#include  <net_phy_def.h>

/*
*********************************************************************************************************
*                                     NXP LPC23xx/LPC24xx REGISTERS
*********************************************************************************************************
*/


#define  PCONP                      (*((volatile CPU_INT32U *)0xE01FC0C4))

/*
*********************************************************************************************************
*                                   NXP LPC23xx/LPC24xx EMAC REGISTERS
*
* Note(s) : (1) See NXP LPC23XX or NXP LPC24XX documentation for register summary.
*
*           (2) Since the EMAC module is integrated into either a LPC23XX or LPC24XX microcontroller,
*               the endianness of the registers is the same as the CPU, which is little endian by default.
*********************************************************************************************************
*/

                                                                            /* ------------- MAC REGISTERS ---------------- */
#define  MAC1                       (*((volatile CPU_INT32U *)0xFFE00000))  /* MAC Configuration Register 1                 */
#define  MAC2                       (*((volatile CPU_INT32U *)0xFFE00004))  /* MAC Configuration Register 2                 */
#define  IPGT                       (*((volatile CPU_INT32U *)0xFFE00008))  /* Back-to-Back Inter-Packet-Gap Register       */
#define  IPGR                       (*((volatile CPU_INT32U *)0xFFE0000C))  /* Non Back-to-Back Inter-Packet-Gap Register   */
#define  CLRT                       (*((volatile CPU_INT32U *)0xFFE00010))  /* Collision Window/Retyr Register              */
#define  MAXF                       (*((volatile CPU_INT32U *)0xFFE00014))  /* Maximum Frame Register                       */
#define  SUPP                       (*((volatile CPU_INT32U *)0xFFE00018))  /* PHY Support Register                         */
#define  TEST                       (*((volatile CPU_INT32U *)0xFFE0001C))  /* Test Register                                */
#define  MCFG                       (*((volatile CPU_INT32U *)0xFFE00020))  /* MII Mgmt Configuration Register              */
#define  MCMD                       (*((volatile CPU_INT32U *)0xFFE00024))  /* MII Mgmt Command       Register              */
#define  MADR                       (*((volatile CPU_INT32U *)0xFFE00028))  /* MII Mgmt Address       Register              */
#define  MWTD                       (*((volatile CPU_INT32U *)0xFFE0002C))  /* MII Mgmt Write Data    Register              */
#define  MRDD                       (*((volatile CPU_INT32U *)0xFFE00030))  /* MII Mgmt Read  Data    Register              */
#define  MIND                       (*((volatile CPU_INT32U *)0xFFE00034))  /* MII Mgmt Indicators    Register              */
#define  SA0                        (*((volatile CPU_INT32U *)0xFFE00040))  /* Station Address 0 Register                   */
#define  SA1                        (*((volatile CPU_INT32U *)0xFFE00044))  /* Station Address 1 Register                   */
#define  SA2                        (*((volatile CPU_INT32U *)0xFFE00048))  /* Station Address 2 Register                   */

                                                                            /* ----------- CONTROL REGISTERS -------------- */
#define  COMMAND                    (*((volatile CPU_INT32U *)0xFFE00100))  /* Command Register                             */
#define  STATUS                     (*((volatile CPU_INT32U *)0xFFE00104))  /* Status Register                              */
#define  RXDESCRIPTOR               (*((volatile CPU_INT32U *)0xFFE00108))  /* Receive  Descriptor Base Address Register    */
#define  RXSTATUS                   (*((volatile CPU_INT32U *)0xFFE0010C))  /* Receive  Status     Base Address Register    */
#define  RXDESCRIPTORNUMBER         (*((volatile CPU_INT32U *)0xFFE00110))  /* Receive  Number of Descriptors   Register    */
#define  RXPRODUCEINDEX             (*((volatile CPU_INT32U *)0xFFE00114))  /* Receive  Produce Index           Register    */
#define  RXCONSUMEINDEX             (*((volatile CPU_INT32U *)0xFFE00118))  /* Receive  Consume Index           Register    */
#define  TXDESCRIPTOR               (*((volatile CPU_INT32U *)0xFFE0011C))  /* Transmit Descriptor Base Address Register    */
#define  TXSTATUS                   (*((volatile CPU_INT32U *)0xFFE00120))  /* Transmit Status     Base Address Register    */
#define  TXDESCRIPTORNUMBER         (*((volatile CPU_INT32U *)0xFFE00124))  /* Transmit Number of Descriptors   Register    */
#define  TXPRODUCEINDEX             (*((volatile CPU_INT32U *)0xFFE00128))  /* Transmit Produce Index           Register    */
#define  TXCONSUMEINDEX             (*((volatile CPU_INT32U *)0xFFE0012C))  /* Transmit Consume Index           Register    */
#define  TSV0                       (*((volatile CPU_INT32U *)0xFFE00158))  /* Transmit Status Vector 0 Register            */
#define  TSV1                       (*((volatile CPU_INT32U *)0xFFE0015C))  /* Transmit Status Vector 1 Register            */
#define  RSV                        (*((volatile CPU_INT32U *)0xFFE00160))  /* Receive  Status Vector   Register            */
#define  FLOWCONTROLCOUNTER         (*((volatile CPU_INT32U *)0xFFE00170))  /* Flow Control Counter Register                */
#define  FLOWCONTROLSTATUS          (*((volatile CPU_INT32U *)0xFFE00174))  /* Flow Control Status  Register                */

                                                                            /* ---------- RX FILTER REGISTERS ------------- */
#define  RXFILTERCTRL               (*((volatile CPU_INT32U *)0xFFE00200))  /* Receive Filter Control Register              */
#define  RXFILTERWOLSTATUS          (*((volatile CPU_INT32U *)0xFFE00204))  /* Receive Filter WoL Status Register           */
#define  RXFILTERWOLCLEAR           (*((volatile CPU_INT32U *)0xFFE00208))  /* Receive Filter WoL Clear  Register           */
#define  HASHFILTERL                (*((volatile CPU_INT32U *)0xFFE00210))  /* Hash Filter Table LSBs Register              */
#define  HASHFILTERH                (*((volatile CPU_INT32U *)0xFFE00214))  /* Hash Filter Table MSBs Register              */

                                                                            /* -------- MODULE CONTROL REGISTERS ---------- */
#define  INTSTATUS                  (*((volatile CPU_INT32U *)0xFFE00FE0))  /* Interrupt status  register                   */
#define  INTENABLE                  (*((volatile CPU_INT32U *)0xFFE00FE4))  /* Interrupt enable  register                   */
#define  INTCLEAR                   (*((volatile CPU_INT32U *)0xFFE00FE8))  /* Interrupt clear   register                   */
#define  INTSET                     (*((volatile CPU_INT32U *)0xFFE00FEC))  /* Interrupt set     register                   */
#define  POWERDOWN                  (*((volatile CPU_INT32U *)0xFFE00FF4))  /* Power-down        register                   */

/*
*********************************************************************************************************
*                                NXP LPC23xx/LPC24xx EMAC REGISTER BITS
*********************************************************************************************************
*/
                                                                            /* ----------------- MAC1 bits ---------------- */
#define  MAC1_RX_ENABLE                             DEF_BIT_00
#define  MAC1_PASS_ALL_FRAMES                       DEF_BIT_01
#define  MAC1_RESET_TX                              DEF_BIT_08
#define  MAC1_RESET_MCS_TX                          DEF_BIT_09
#define  MAC1_RESET_RX                              DEF_BIT_10
#define  MAC1_RESET_MCS_RX                          DEF_BIT_11
#define  MAC1_RESET_SIM                             DEF_BIT_14
#define  MAC1_RESET_SOFT                            DEF_BIT_15

                                                                            /* ----------------- MAC2 bits ---------------- */
#define  MAC2_FULL_DUPLEX                           DEF_BIT_00
#define  MAC2_CRC_EN                                DEF_BIT_08
#define  MAC2_PAD_EN                                DEF_BIT_09

                                                                            /* ----------------- IPGT bits ---------------- */
#define  IPG_HALF_DUP                                 0x0012
#define  IPG_FULL_DUP                                 0x0015

                                                                            /* ----------------- SUPP bits ---------------- */
#define  SUPP_SPEED                                 DEF_BIT_08

                                                                            /* ----------------- MCFG bits ---------------- */
#define  MCFG_CLKSEL_DIV4                             0x0000
#define  MCFG_CLKSEL_DIV6                             0x0008
#define  MCFG_CLKSEL_DIV8                             0x000C
#define  MCFG_CLKSEL_DIV10                            0x0010
#define  MCFG_CLKSEL_DIV14                            0x0014
#define  MCFG_CLKSEL_DIV20                            0x0018
#define  MCFG_CLKSEL_DIV28                            0x001C
#define  MCFG_RESET_MII_MGMT                        DEF_BIT_15

                                                                            /* ----------------- MCMD bits ---------------- */
#define  MCMD_WRITE                                 DEF_BIT_NONE
#define  MCMD_READ                                  DEF_BIT_00


                                                                            /* --------------- COMMAND bits --------------- */
#define  COMMAND_RX_EN                              DEF_BIT_00
#define  COMMAND_TX_EN                              DEF_BIT_01
#define  COMMAND_RESET_REG                          DEF_BIT_03
#define  COMMAND_RESET_TX                           DEF_BIT_04
#define  COMMAND_RESET_RX                           DEF_BIT_05
#define  COMMAND_PASS_RUNT_FRAMES                   DEF_BIT_06
#define  COMMAND_PASS_ALL_FRAMES                    DEF_BIT_07
#define  COMMAND_RMII                               DEF_BIT_09
#define  COMMAND_FULL_DUPLEX                        DEF_BIT_10

                                                                            /* -------------- INTENABLE bits -------------- */
                                                                            /* -------------- INTSTATUS bits -------------- */
                                                                            /* -------------- INTCLEAR  bits -------------- */
                                                                            /* -------------- INTSET    bits -------------- */
#define  INT_RX_OVERRUN                             DEF_BIT_00
#define  INT_RX_ERROR                               DEF_BIT_01
#define  INT_RX_FINISHED                            DEF_BIT_02
#define  INT_RX_DONE                                DEF_BIT_03
#define  INT_TX_UNDERRUN                            DEF_BIT_04
#define  INT_TX_ERROR                               DEF_BIT_05
#define  INT_TX_FINISHED                            DEF_BIT_06
#define  INT_TX_DONE                                DEF_BIT_07
#define  INT_SOFT                                   DEF_BIT_12
#define  INT_WAKEUP                                 DEF_BIT_13

                                                                            /* ------------- RXFILTERCTRL bits ------------ */
#define  ACCEPT_BROADCAST                           DEF_BIT_01
#define  ACCEPT_PERFECT                             DEF_BIT_05


/*
*********************************************************************************************************
*                      DESCRIPTOR CONTROL AND STATUS BIT DEFINITIONS
*********************************************************************************************************
*/

#define  EMAC_TX_DESC_INT                       0x80000000              /* EMAC Descriptor Tx and Rx Control bits                   */
#define  EMAC_TX_DESC_LAST                      0x40000000
#define  EMAC_TX_DESC_CRC                       0x20000000
#define  EMAC_TX_DESC_PAD                       0x10000000
#define  EMAC_TX_DESC_HUGE                      0x08000000
#define  EMAC_TX_DESC_OVERRIDE                  0x04000000

#define  EMAC_RX_DESC_INT                       0x80000000

#define  TX_DESC_STATUS_ERR                     0x80000000              /* EMAC Tx Status bits                                      */
#define  TX_DESC_STATUS_NODESC                  0x40000000
#define  TX_DESC_STATUS_UNDERRUN                0x20000000
#define  TX_DESC_STATUS_LCOL                    0x10000000
#define  TX_DESC_STATUS_ECOL                    0x08000000
#define  TX_DESC_STATUS_EDEFER                  0x04000000
#define  TX_DESC_STATUS_DEFER                   0x02000000
#define  TX_DESC_STATUS_COLCNT                  0x01E00000	

#define  RX_DESC_STATUS_ERR	                    0x80000000              /* EMAC Rx Status bits                                      */
#define  RX_DESC_STATUS_LAST                    0x40000000
#define  RX_DESC_STATUS_NODESC                  0x20000000
#define  RX_DESC_STATUS_OVERRUN	                0x10000000
#define  RX_DESC_STATUS_ALGNERR	                0x08000000
#define  RX_DESC_STATUS_RNGERR                  0x04000000
#define  RX_DESC_STATUS_LENERR                  0x02000000
#define  RX_DESC_STATUS_SYMERR                  0x01000000
#define  RX_DESC_STATUS_CRCERR                  0x00800000
#define  RX_DESC_STATUS_BCAST                   0x00400000
#define  RX_DESC_STATUS_MCAST                   0x00200000
#define  RX_DESC_STATUS_FAILFLT                 0x00100000
#define  RX_DESC_STATUS_VLAN                    0x00080000
#define  RX_DESC_STATUS_CTLFRAM	                0x00040000
#define  RX_DESC_STATUS_SIZE_MASK               0x000007FF
                                                                        /* Determine addresses of descriptor lists                  */
#define  EMAC_RX_DESC_BASE_ADDR                (EMAC_RAM_BASE_ADDR)
#define  EMAC_RX_STATUS_BASE_ADDR              (EMAC_RX_DESC_BASE_ADDR   + (EMAC_NUM_RX_DESC * sizeof(EMAC_DESCRIPTOR)))
#define  EMAC_TX_DESC_BASE_ADDR                (EMAC_RX_STATUS_BASE_ADDR + (EMAC_NUM_RX_DESC * sizeof(RX_STATUS)))
#define  EMAC_TX_STATUS_BASE_ADDR              (EMAC_TX_DESC_BASE_ADDR   + (EMAC_NUM_TX_DESC * sizeof(EMAC_DESCRIPTOR)))
#define  EMAC_RX_BUFF_BASE_ADDR                (EMAC_TX_STATUS_BASE_ADDR + (EMAC_NUM_TX_DESC * sizeof(TX_STATUS)))
#define  EMAC_TX_BUFF_BASE_ADDR                (EMAC_RX_BUFF_BASE_ADDR   + (EMAC_NUM_RX_DESC * EMAC_RX_BUF_SIZE))

/*
*********************************************************************************************************
*                                          DATA TYPES
*********************************************************************************************************
*/

typedef  struct  emac_descriptor {                                      /* EMAC Descriptor                                          */
    CPU_INT32U  PacketAddr;	                                            /* DMA Buffer Address                                       */
    CPU_INT32U  Control;                                                /* DMA Control bits                                         */
}   EMAC_DESCRIPTOR;

typedef  struct  rx_status {                                            /* Rx Status data type                                      */
    CPU_INT32U  StatusInfo;                                             /* Status information                                       */
    CPU_INT32U  StatusHashCRC;                                          /* Status Hash CRC                                          */
}   RX_STATUS;

typedef  struct  tx_status {                                            /* Rx Status data type                                      */
    CPU_INT32U  StatusInfo;                                             /* Status information                                       */
}   TX_STATUS;

/*
*********************************************************************************************************
*                                      GLOBAL VARIABLES
*********************************************************************************************************
*/

static  CPU_INT08U        MII_Dividers [7][2] =  {{4,  MCFG_CLKSEL_DIV4},
                                                  {6,  MCFG_CLKSEL_DIV6},
                                                  {8,  MCFG_CLKSEL_DIV8},
                                                  {10, MCFG_CLKSEL_DIV10},
                                                  {14, MCFG_CLKSEL_DIV14},
                                                  {20, MCFG_CLKSEL_DIV20},
                                                  {28, MCFG_CLKSEL_DIV28}};

static  EMAC_DESCRIPTOR  *RxDesc;
static  EMAC_DESCRIPTOR  *TxDesc;
static  RX_STATUS        *RxStatus;
static  TX_STATUS        *TxStatus;
static  CPU_INT08U       *RxBufBaseAddr;
static  CPU_INT08U       *TxBufBaseAddr;

static  CPU_INT32U        NIC_RxNRdyCtr;

/*$PAGE*/
/*
*********************************************************************************************************
*                                 LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void        NetNIC_TxPktDiscard      (NET_ERR      *perr);


                                                                        /* ----------- LPC23XX/LPC24XX EMAC FNCTS ------------- */

static  void        EMAC_Init                (NET_ERR      *perr);


                                                                        /* ---------- LPC23XX/LPC24XX EMAC RX FNCTS ----------- */

static  void        EMAC_RxEn                (void);
static  void        EMAC_RxDis               (void);


static  void        EMAC_RxIntEn             (void);

static  void        EMAC_RxPkt               (void         *ppkt,
                                              CPU_INT16U    size,
                                              NET_ERR      *perr);

static  void        EMAC_RxPktDiscard        (CPU_INT16U    size);

                                                                        /* ---------- LPC23XX/LPC24XX EMAC TX FNCTS ----------- */

static  void        EMAC_TxEn                (void);
static  void        EMAC_TxDis               (void);


static  void        EMAC_TxPkt               (void         *ppkt,
                                              CPU_INT16U    size,
                                              NET_ERR      *perr);

static  CPU_INT16U  NIC_RxGetNRdy            (void);

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            NetNIC_Init()
*
* Description : (1) Initialize Network Interface Card :
*
*                   (a) Perform NIC Layer OS initialization
*                   (b) Initialize NIC status
*                   (c) Initialize NIC statistics & error counters
*                   (d) Initialize EMAC
*
*
* Argument(s) : perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                    Network interface card successfully initialized.
*
*                                                                   -------- RETURNED BY NetOS_NIC_Init() : --------
*                               NET_OS_ERR_INIT_NIC_TX_RDY          NIC transmit ready signal NOT successfully
*                                                                       initialized.
*                               NET_OS_ERR_INIT_NIC_TX_RDY_NAME     NIC transmit ready name   NOT successfully
*                                                                       configured.
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
* Note(s)     : (1) This function calls EMAC_Init() which initializes the LPC23XX/LPC24XX hardware.
*********************************************************************************************************
*/

void  NetNIC_Init (NET_ERR  *perr)
{                                                                       /* --------------- PERFORM NIC/OS INIT -------------------- */
    NetOS_NIC_Init(perr);                                               /* Create NIC objs.                                         */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }
                                                                        /* ----------------- INIT NIC STATUS ---------------------- */
    NetNIC_ConnStatus           =  DEF_OFF;
                                                                        /* ------------- INIT NIC STAT & ERR CTRS ----------------- */
#if (NET_CTR_CFG_STAT_EN        == DEF_ENABLED)
    NetNIC_StatRxPktCtr         =  0;
    NetNIC_StatTxPktCtr         =  0;
#endif

#if (NET_CTR_CFG_ERR_EN         == DEF_ENABLED)
    NetNIC_ErrRxPktDiscardedCtr =  0;
    NetNIC_ErrTxPktDiscardedCtr =  0;
#endif
                                                                        /* ----------------- INIT LPC2378 EMAC -------------------- */
    EMAC_Init(perr);
}

/*
*********************************************************************************************************
*                                            NetNIC_IntEn()
*
* Description : Enable NIC interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*********************************************************************************************************
*/

void  NetNIC_IntEn (NET_ERR  *perr)
{
    EMAC_RxIntEn();                                                     /* Enable Rx interrupts                                     */

    *perr = NET_NIC_ERR_NONE;
}

/*
*********************************************************************************************************
*                                        NetNIC_ConnStatusGet()
*
* Description : Get NIC's network connection status.
*
*               (1) Obtaining the NIC's network connection status is encapsulated in this function for the
*                   possibility that obtaining a NIC's connection status requires a non-trivial procedure.
*
*
* Argument(s) : none.
*
* Return(s)   : NIC network connection status :
*
*                   DEF_OFF     Network connection DOWN.
*                   DEF_ON      Network connection UP.
*
* Caller(s)   : NetIF_Pkt_Tx().
*********************************************************************************************************
*/

CPU_BOOLEAN  NetNIC_ConnStatusGet (void)
{
    return (NetNIC_ConnStatus);
}

/*
*********************************************************************************************************
*                                        EMAC Link Settings Update
*
* Description : This function is called by NetNIC_Init and the PHY ISR in order to update the
*             : speed and duplex settings for the EMAC.
*
* Arguments   : link_speed      Indicates link speed.  This can be one of
*                                   NET_PHY_SPD_0
*                                   NET_PHY_SPD_10
*                                   NET_PHY_SPD_100
*                                   NET_PHY_SPD_1000
*
*               link_duplex     Indicates link duplex.  This can be one of
*                                   NET_PHY_DUPLEX_UNKNOWN
*                                   NET_PHY_DUPLEX_HALF
*                                   NET_PHY_DUPLEX_FULL
*
* Return(s)   : none.
*********************************************************************************************************
*/

void  NetNIC_LinkChange (CPU_INT32U link_speed, CPU_INT32U link_duplex)
{
    switch (link_speed) {
        case NET_PHY_SPD_0:                                             /* Assume 10Mbps operation until linked                     */
        case NET_PHY_SPD_10:
             SUPP      &=  ~SUPP_SPEED;                                 /* Configure the RMII logic (if used) for 10MBps operation  */
             break;

        case NET_PHY_SPD_100:
             SUPP      |=   SUPP_SPEED;                                 /* Configure the RMII logic (if uses) for 100MBps operation */
             break;
    }

    switch (link_duplex) {
        case NET_PHY_DUPLEX_UNKNOWN:                                    /* Assume half duplex until link duplex is known            */
        case NET_PHY_DUPLEX_HALF:
             MAC2      &=  ~MAC2_FULL_DUPLEX;                           /* Configure the EMAC to run in HALF duplex mode            */
             COMMAND   &=  ~COMMAND_FULL_DUPLEX;                        /* Configure the MII logic for a Half Duplex PHY Link       */
             IPGT       =   IPG_HALF_DUP;	                            /* Set inter packet gap to the recommended Half Duplex      */
             break;

        case NET_PHY_DUPLEX_FULL:
             MAC2      |=   MAC2_FULL_DUPLEX;                           /* Configure the EMAC to run in FULL duplex mode            */
             COMMAND   |=   COMMAND_FULL_DUPLEX;                        /* Configure the MII logic for a Full Duplex PHY Link       */
             IPGT       =   IPG_FULL_DUP;	                            /* Set inter packet gap to the recommended Full Duplex      */
             break;
    }
}

/*
*********************************************************************************************************
*                                         NetNIC_ISR_Handler()
*
* Description : (1) Decode ISR & call appropriate ISR handler :
*
*                   (a) Receive Buffer Not Available ISR     NetNIC_RxPktDiscard().
*                   (b) Receive  ISR                         NetNIC_RxISR_Handler().
*                   (c) Transmit ISR                         NetNIC_TxISR_Handler().
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR
*
* Note(s)     : 1) VIC interrupts do not need to be explicity acknowledged if the BSP exception
*                  handler writes a 0x00 to the VICAddress register after calling the ISR handler.
*                  User ISR handlers (such as this one) need only clear the local interrupt source.
*********************************************************************************************************
*/

void  NetNIC_ISR_Handler (void)
{
    CPU_INT32U  intstatus;
    CPU_INT16U  n_new;
    NET_ERR     err;


    intstatus               =  INTSTATUS;                               /* Read the interrupt status register                       */

    if ((intstatus & INT_RX_DONE) > 0) {                                /* If a receiver event has occured                          */
        n_new               =  NIC_RxGetNRdy() - NIC_RxNRdyCtr;         /* Determine how many NEW franes have been received         */
        while (n_new > 0) {
            NetOS_IF_RxTaskSignal(&err);                                /* Signal Net IF Rx Task that a new frame has arrived       */
            switch (err) {                                              /* Each frame is processed sequentially from the Rx task    */
                case NET_IF_ERR_NONE:
                    if (NIC_RxNRdyCtr < EMAC_NUM_RX_DESC) {
                         NIC_RxNRdyCtr++;
                     }
                     break;

                case NET_IF_ERR_RX_Q_FULL:
                case NET_IF_ERR_RX_Q_POST_FAIL:
                default:
                     NetNIC_RxPktDiscard(0, &err);                      /* If an error occured while signaling the task, discard    */
                     break;                                             /* the received frame                                       */
            }

            n_new--;
        }
        INTCLEAR            = (INT_RX_DONE);                            /* Clear the interrupt flags                                */
    }

    if ((intstatus & (INT_RX_OVERRUN)) > 0) {                           /* If a fator Overrun error has occured                     */
        INTCLEAR            = (INT_RX_OVERRUN);                         /* Clear the overrun interrupt flag                         */
        COMMAND            |=  COMMAND_RESET_RX;                        /* Soft reset the Rx datapath, this disables the receiver   */
        COMMAND            |=  COMMAND_RX_EN;                           /* Re-enable the reciever                                   */
        MAC1               |=  MAC1_RX_ENABLE;                          /* Re-enable the reciever                                   */
    }
}

/*
*********************************************************************************************************
*                                         NetNIC_RxPktGetSize()
*
* Description : Get network packet size from NIC.
*
* Argument(s) : none.
*
* Return(s)   : Size, in octets, of next frame to be read from the EMAC buffers
*
* Caller(s)   : NetIF_RxTaskHandler()
*
* Notes       : None.
*********************************************************************************************************
*/

CPU_INT16U  NetNIC_RxPktGetSize (void)
{
    CPU_INT16S   size;
    CPU_INT32U   rxstatus;


    rxstatus        =  RxStatus[RXCONSUMEINDEX].StatusInfo;             /* Accquire the status word for this desciptor              */

    rxstatus       &= (RX_DESC_STATUS_NODESC    |                       /* Obtain the descriptor error bits                         */
                       RX_DESC_STATUS_OVERRUN   |
                       RX_DESC_STATUS_ALGNERR   |
                       RX_DESC_STATUS_LENERR    |
                       RX_DESC_STATUS_SYMERR    |
                       RX_DESC_STATUS_CRCERR    |
                       RX_DESC_STATUS_FAILFLT);

    if (rxstatus > 0) {                                                 /* If any receive errors (except range error) occured       */
        size        =  0;                                               /* then return 0 so upper layer call NetNIC_RxPktDiscard()  */
    } else {
        size        =  RxStatus[RXCONSUMEINDEX].StatusInfo & 0x7FF;     /* Obtain the fragment size from the status struct pointer  */
        size       -=  3;                                               /* +1 since the size is 0 based, -4 to ignore FCS           */
    }

    if (size < 0) {                                                     /* Ensure that the subtraction didnt cause an underflow     */
        size = 0;
    }

    return (size);                                                      /* Return the size of the current frame                     */
}

/*
*********************************************************************************************************
*                                            NetNIC_RxPkt()
*
* Description : Read network packet from NIC into buffer.
*
* Argument(s) : ppkt        Pointer to memory buffer to receive NIC packet.
*
*               size        Number of packet frame octets to read into buffer.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully read.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*                               NET_NIC_ERR_NULL_PTR        Argument 'ppkt' passed a NULL pointer.
*                               NET_NIC_ERR_INVALID_SIZE    Invalid size.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : 1) NetNIC_RxPkt() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_RxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
#if (NET_CTR_CFG_STAT_EN     == DEF_ENABLED)
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif
#endif


    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit rx (see Note #2).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

#if (NET_ERR_CFG_ARG_CHK_DBG_EN == DEF_ENABLED)                         /* ------------------- VALIDATE PTR ------------------  */
    if (ppkt == (void *)0) {
       *perr = NET_NIC_ERR_NULL_PTR;
        return;
    }

                                                                        /* ------------------- VALIDATE SIZE -----------------  */
    if (size < NET_IF_FRAME_MIN_SIZE) {
       *perr = NET_NIC_ERR_INVALID_SIZE;
        return;
    }
#endif

    EMAC_RxPkt(ppkt, size, perr);                                       /* Read the received Frame from the EMAC buffers        */
    if (*perr != NET_NIC_ERR_NONE) {
        return;
    }

    NET_CTR_STAT_INC(NetNIC_StatRxPktCtr);

    *perr = NET_NIC_ERR_NONE;
}

/*
*********************************************************************************************************
*                                         NetNIC_RxPktDiscard()
*
* Description : Discard network packet from NIC to free NIC packet frames for new receive packets.
*
* Argument(s) : size        Number of packet frame octets.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE            Packet successfully discarded.
*                               NET_ERR_INIT_INCOMPLETE     Network initialization NOT complete.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
* Note(s)     : (1) NetNIC_RxPktDiscard() blocked until network initialization completes; perform NO action.
*
*               (2) #### 'perr' may NOT be necessary (remove before product release if unnecessary).
*********************************************************************************************************
*/

void  NetNIC_RxPktDiscard (CPU_INT16U   size,
                           NET_ERR     *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif

    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit discard (see Note #1).    */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

    EMAC_RxPktDiscard(size);

    CPU_CRITICAL_ENTER();                                               /* This routine alters shared data. Disable interrupts!     */
    if (NIC_RxNRdyCtr > 0) {                                            /* One less packet to process                               */
        NIC_RxNRdyCtr--;
    }
    CPU_CRITICAL_EXIT();                                                /* Re-Enable interrupts                                     */

    NET_CTR_ERR_INC(NetNIC_ErrRxPktDiscardedCtr);

   *perr = NET_NIC_ERR_NONE;
}

/*
*********************************************************************************************************
*                                             NetNIC_TxPkt()
*
* Description : Transmit data packets from network driver layer to network interface card.
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*               ----        Argument validated in NetIF_Pkt_Tx().
*
*               size        Number of packet frame octets to write to frame.
*               ----        Argument checked   in NetIF_Tx().
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*
*                                                               - RETURNED BY NetNIC_TxPktDiscard() : -
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx().
*
* Note(s)     : (1) NetNIC_TxPkt() blocked until network initialization completes; perform NO action.
*********************************************************************************************************
*/

void  NetNIC_TxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
#if ((NET_CTR_CFG_STAT_EN     == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif


    if (Net_InitDone != DEF_YES) {                                      /* If init NOT complete, exit tx (see Note #1).             */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }


    EMAC_TxPkt(ppkt, size, perr);                                       /* Hardware transmit routi                                  */

    if (*perr != NET_NIC_ERR_NONE) {
        NetNIC_TxPktDiscard(perr);
        return;
    }

    NET_CTR_STAT_INC(NetNIC_StatTxPktCtr);
}

/*
*********************************************************************************************************
*                                        NetNIC_PhyRegRd()
*
* Description : (1) Read PHY register
*
* Argument(s) : phy         PHY address, normally 0.
*               reg         PHY register.
*               perr        Pointer to variable that will hold the return error code from this function
*
* Return(s)   : MRDD        PHY register data.
*
* Caller(s)   : EMAC_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16U  NetNIC_PhyRegRd (CPU_INT08U  phy,
                             CPU_INT08U  reg,
                             NET_ERR    *perr)
{
              CPU_INT08U  retries;
    volatile  CPU_INT32U  i;


    retries     =   0;                                                  /* Initialize retries to 0                                  */

    MCMD        =   0;                                                  /* Clear the Read COMMAND                                   */
    MADR        =  (phy << 8) | reg;	                                /* [12:8] == PHY addr, [4:0]=0x00(BMCR) register addr       */
    MCMD        =   MCMD_READ;                                          /* Issue a Read COMMAND                                     */

    while ((MIND != 0) && (retries < PHY_RDWR_RETRIES)) {               /* Read the Management Indicator register, MII busy if > 0  */
    
        for (i = 0; i < 1000; i++) {                                    /* Delay while the read is in progress                      */
            ;
        }
        retries++;
    }

    if (retries >= PHY_RDWR_RETRIES) {                                  /* If there are no retries remaining                        */
       *perr     = NET_PHY_ERR_REGRD_TIMEOUT;                           /* Return an error code if the PHY Read timed out           */
    } else {
       *perr     = NET_PHY_ERR_NONE;
    }

    return (MRDD);                                                      /* Return the content of the Management Read Data register  */
}

/*
*********************************************************************************************************
*                                        NetNIC_PhyRegWr()
*
* Description : (1) Write PHY register
*
* Argument(s) : phy             PHY address, normally 0.
*               reg             PHY register.
*               val             Data to write to PHY register.
*               perr            Pointer to variable that will hold the return error code from this function
*
* Return(s)   : none.
*
* Caller(s)   : EMAC_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetNIC_PhyRegWr (CPU_INT08U  phy,
                       CPU_INT08U  reg,
                       CPU_INT16U  val,
                       NET_ERR    *perr)
{
              CPU_INT08U  retries;
    volatile  CPU_INT32U  i;


    retries     =   0;                                                  /* Initialize retries to 0                                  */

    MCMD        =   MCMD_WRITE;                                         /* Issue a Write COMMAND                                    */
    MADR        =  (phy << 8) | reg;                                    /* [12:8] == PHY addr, [4:0]=0x00(BMCR) register addr       */
    MWTD        =   val;                                                /* Write the data to the Management Write Data register     */

    while ((MIND != 0) && (retries < PHY_RDWR_RETRIES)) {               /* Read the Management Indicator register, MII busy if > 0  */
    
        for (i = 0; i < 1000; i++) {                                    /* Delay while the read is in progress                      */
            ;
        }
        retries++;
    }

    if (retries >= PHY_RDWR_RETRIES) {                                  /* If there are no retries remaining                        */
       *perr     = NET_PHY_ERR_REGWR_TIMEOUT;                           /* Return an error code if the PHY Read timed out           */
    } else {
       *perr     = NET_PHY_ERR_NONE;
    }
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         NetNIC_TxPktDiscard()
*
* Description : On any Transmit errors, set error.
*
* Argument(s) : pbuf        Pointer to network buffer.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_ERR_TX                  Transmit error; packet discarded.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxPkt().
*********************************************************************************************************
*/

static  void  NetNIC_TxPktDiscard (NET_ERR  *perr)
{
#if (NET_CTR_CFG_STAT_EN     == DEF_ENABLED)
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR   cpu_sr;
#endif
#endif


    NET_CTR_ERR_INC(NetNIC_ErrTxPktDiscardedCtr);

   *perr = NET_ERR_TX;
}

/*
*********************************************************************************************************
*                                           EMAC_Init()
*
* Description : (1) Initialize the LPC23XX/LPC24XX integrated EMAC
*
*                   (a) Initialize Registers
*                   (b) Initialize MAC  Address
*                   (c) Initialize Auto Negotiation
*                   (d) Initialize Interrupts
*                   (e) Enable     Receiver/Transmitter
*
* Argument(s) : perr    a return error code indicating the result of initialization
*                       NET_NIC_ERR_NONE      -  success
*                       NET_NIC_ERR_NIC_OFF   -  link is down, or init failure occured
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_Init().
*
* Note(s)     : (1) (a) Assumes MAC address to set has previously been initialized by
*                       (1) Application code          for EMAC_MAC_ADDR_SEL_CFG
*
*               (2) Interrupts MUST be enabled ONLY after ALL network initialization is complete (see also
*                   'net.c  Net_Init()  Note #4d').
*
*              (3) P1.6, ENET-TX_CLK, has to be set for EMAC to address a BUG in early revisions of the LPC2378.
*                  This pin must be set even if RMII is not used.  The bug has been fixed on newer revisions of
*                  the chip and can be used for GPIO in some cicumstances. The Keil MCB2300 that this driver
*                  has been developed with contains the errata. RMII mode can be switched on and off from
*                  net_bsp.h by adjusting the EMAC_CFG_RMII macro accordingly.
*
*             (4)  The external PHY address is defined in net_phy.h as PHY_ADDR and should be set to
*                  the address of your external PHY on the MII bus. This value is shifted over to the
*                  left by 8 bits and written to the MAC_MADR register during PHY communications.
*                  Consult your PHY documentation for additional information on how to determine the
*                  address of your PHY.
*
*             (5)  The EMAC does not support the use of serial EEPROMS for automatically loading
*                  the MAC address after reset. Therefore, the macro EMAC_CFG_MAC_ADDR_SEL in
*                  net_bsp.h MUST be configured to EMAC_CFG_MAC_ADDR_SEL_CFG. This will allow
*                  the driver to load the Station Address registers with the data found in
*                  NetIF_MAC_Addr[], which is set by the user in app.c before calling Net_Init().
*                  If necessary, the user may take steps to read a serial EEPROM and then
*                  fill the content of NetIF_MAC_Addr[] accordingly.
*
*             (6)  Interrupts are enabled by uC/TCP-IP, Net_Init() calling NetNIC_IntEn().
*
*             (7) Rx descriptors must be 32 bit aligned, Rx status descriptors must be 64 bit aligned.
*
*             (8) The EMAC has 16KB of dedicated RAM located on the AHB2 bus at address 0x7FE00000,
*                 see net_bsp.h.
*********************************************************************************************************
*/
static  void  EMAC_Init (NET_ERR *perr)
{
    CPU_INT16U  i;
    CPU_INT32U  clk_freq;


    NIC_RxNRdyCtr       =   0;                                          /* Initialize global Rx packet count to 0                   */

    NetNIC_LinkUp();                                                    /* Set NetNIC_ConnStatus to TRUE by default (for uC/TCP-IP) */

                                                                        /* ------- Reset and configure the EMAC registers --------- */

   *perr                =   NET_NIC_ERR_NONE;                           /* Initialize the returned error code to NO Error           */

    PCONP              |=  (1 << 30);                                   /* Enable the EMAC clock                                    */

    NetBSP_Phy_HW_Init();                                               /* Configure the I/O pins necessary for PHY communication   */

    MAC1                =   MAC1_RESET_TX       |                       /* [15],[14],[11:8] -> soft resets all MAC internal modules */
                            MAC1_RESET_MCS_TX   |
                            MAC1_RESET_RX       |
                            MAC1_RESET_MCS_RX   |
                            MAC1_RESET_SIM      |
                            MAC1_RESET_SOFT;

    COMMAND             =   COMMAND_RESET_REG   |                       /* Reset all datapaths and host registers                   */
                            COMMAND_RESET_TX    |
                            COMMAND_RESET_RX;

    NetBSP_DlyMs(2);                                                    /* Delay for a few milliseconds after reset                 */

    MAC1                =   0;                                          /* Deassert all prior resets                                */

    EMAC_TxDis();                                                       /* Disable the transmitter                                  */
    EMAC_RxDis();                                                       /* Disable the receiver                                     */

#if EMAC_CFG_RMII
    COMMAND            |=   COMMAND_RMII;                               /* Configure EMAC / PHY communication to RMII mode          */
#else
    COMMAND            &=  ~COMMAND_RMII;                               /* Configure EMAC / PHY communication to MII mode           */
#endif

    SUPP                =   0x0000;                                     /* Assume and configure RMII link speed logic for 10Mbit    */
    NetBSP_DlyMs(2);                                                    /* Delay for a few milliseconds after reset                 */

    TEST                =   0;                                          /* Default the test register. No tests in progress          */

    MAXF                =   0x600;                                      /* Accept maximum sized frames, 1536 octects (default)      */

    MAC1               |=   MAC1_PASS_ALL_FRAMES;                       /* Pass All Frames: Normal and Control                      */
    MAC2                =   MAC2_CRC_EN | MAC2_PAD_EN;                  /* Configure the EMAC to PAD short frames and append CRC    */

    RXFILTERCTRL        =   ACCEPT_BROADCAST | ACCEPT_PERFECT;          /* Accept Broadcast and Perfect Address frames              */

                                                                        /* ---- Configure the MDI port (RMII / MII capable) ------- */
    clk_freq            =   NetBSP_GetHostClkFreq();                    /* Get the LPC23xx/LPC24xx cClkFrq (Hz) which feeds the EMAC*/
    clk_freq           /=   100000;                                     /* Div by 1000000 for MHz, Mult by 10 so floating point     */
                                                                        /* math not necessary when calculating the divider below    */

    MCFG               |=   0x8018;                                     /* Apply a reset to the MII logic, set default divider = 20 */
    MCMD                =   0;                                          /* Clear MII command register                               */

    for (i = 0; i < 7; i++) {                                           /* Check dividers to yield MII frequency ~2.5 MHz           */
        if ((clk_freq / MII_Dividers[i][0]) <=  25) {                   /* Index [i][0] = decimal div value, [i][1] = MCFG reg val  */
            MCFG        =   MII_Dividers[i][1];                         /* Remove reset, set proper MIIM divider                    */
            break;
        }
    }

    NetBSP_DlyMs(10);                                                   /* Short delay while PHY exits reset and new divider is set */

    NetNIC_PhyInit(perr);                                               /* --------------- Initialize the PHY --------------------- */

    IPGR                =   0x0C12;                                     /* Set the Non Back to Back Inter Pkt Gap to recm'd value   */
    CLRT                =   0x370F;                                     /* Collission Windows Retry register (default value)        */


    SA0                 =  (NetIF_MAC_Addr[5] << 8) |                   /* Write the MAC Address, octect 2 and 1 to the EMAC        */
                           (NetIF_MAC_Addr[4]);

    SA1                 =  (NetIF_MAC_Addr[3] << 8) |                   /* Write the MAC Address, octect 4 and 3 to the EMAC        */
                           (NetIF_MAC_Addr[2]);

    SA2                 =  (NetIF_MAC_Addr[1] << 8) |                   /* Write the MAC Address, octect 6 and 5 to the EMAC        */
                           (NetIF_MAC_Addr[0]);


    NetIF_MAC_AddrValid =   DEF_YES;                                    /* Inform uC/TCP-IP, that we have a valid MAC address set   */


                                                                        /* ----- Descriptor List Global Pointer Initialization ---- */
    RxDesc              =  (EMAC_DESCRIPTOR *)(EMAC_RX_DESC_BASE_ADDR);
    TxDesc              =  (EMAC_DESCRIPTOR *)(EMAC_TX_DESC_BASE_ADDR);
    RxStatus            =  (RX_STATUS  *)(EMAC_RX_STATUS_BASE_ADDR);
    TxStatus            =  (TX_STATUS  *)(EMAC_TX_STATUS_BASE_ADDR);
    RxBufBaseAddr       =  (CPU_INT08U *)(EMAC_RX_BUFF_BASE_ADDR);
    TxBufBaseAddr       =  (CPU_INT08U *)(EMAC_TX_BUFF_BASE_ADDR);

                                                                        /* ---------------- Rx Descriptor Initialization ----------- */

                                                                        /* Rx Descriptor and Status array initialization            */
                                                                        /* Set the addr of the DMA Rx buf                           */
                                                                        /* Interrupt when Rx complete, set buf size (-1 encoded)    */
                                                                        /* Initialize the Status Info to 0                          */
                                                                        /* Initialize the Status Hash CRC to 0                      */
    for (i = 0; i < EMAC_NUM_RX_DESC; i++) {
        RxDesc[i].PacketAddr        =  (CPU_INT32U)(RxBufBaseAddr + (i * EMAC_RX_BUF_SIZE));
        RxDesc[i].Control           =   EMAC_RX_DESC_INT | (EMAC_RX_BUF_SIZE - 1);
        RxStatus[i].StatusInfo      =   0;
        RxStatus[i].StatusHashCRC   =   0;
    }

    RXDESCRIPTOR        =  (CPU_INT32U)(RxDesc);                        /* Write the Rx Descriptor base address register            */
    RXSTATUS            =  (CPU_INT32U)(RxStatus);                      /* Write the Rx Status base address register                */
    RXDESCRIPTORNUMBER  =   EMAC_NUM_RX_DESC - 1;                       /* Write the Rx Descriptor Number register with NumDesc - 1 */
    RXCONSUMEINDEX      =   0;                                          /* Configure the Rx Consume Index register to 0             */

                                                                        /* ------- Tx Descriptor and Status Initialization -------- */

                                                                        /* Rx Descriptor and Status array initialization            */
                                                                        /* Set the addr of the DMA Rx buf, assume 1536B frames */
                                                                        /* Configure the control bits in the EMAC_TxPkt() function  */
                                                                        /* Initialize the Status Info to 0                          */
    for (i = 0; i < EMAC_NUM_TX_DESC; i++) {
        TxDesc[i].PacketAddr        =  (CPU_INT32U)(TxBufBaseAddr + (i * 1536));
        TxDesc[i].Control           =   0;
        TxStatus[i].StatusInfo      =   0;
    }

    TXDESCRIPTOR        =  (CPU_INT32U)(TxDesc);                        /* Write the Tx Descriptor base address register            */
    TXSTATUS            =  (CPU_INT32U)(TxStatus);                      /* Write the Tx Status base address register                */
    TXDESCRIPTORNUMBER  =   EMAC_NUM_TX_DESC - 1;                       /* Write the Tx Descriptor Number register with NumDesc - 1 */
    TXPRODUCEINDEX      =   0;                                          /* Configure the Tx Produce Index register to 0             */

                                                                        /* ------- Configure Interrupts on the VIC Level ---------- */

    INTCLEAR            =  (INT_RX_OVERRUN   |                          /* Clear all EMAC interrupt sources                         */
                            INT_RX_ERROR     |
                            INT_RX_FINISHED  |
                            INT_RX_DONE      |
                            INT_TX_UNDERRUN  |
                            INT_TX_ERROR     |
                            INT_TX_FINISHED  |
                            INT_TX_DONE      |
                            INT_SOFT         |
                            INT_WAKEUP);

    NetNIC_IntInit();

                                                                        /* -------- Enable the transmitter and receiver ----------- */

    EMAC_TxEn();                                                        /* Enable the EMAC transmitter                              */
    EMAC_RxEn();                                                        /* Enable the EMAC receiver                                 */
   *perr                =   NET_NIC_ERR_NONE;                           /* Initialize the returned error code to NO Error           */
}

/*
*********************************************************************************************************
*                                         NIC_RxGetNRdy()
*
* Description : Determines how many packets we are ready to be processed.
*
* Parameters  : None.
*
* Returns     : Number of NIC buffers that are ready to be processed by the stack.
*********************************************************************************************************
*/

static  CPU_INT16U  NIC_RxGetNRdy (void)
{
    CPU_INT16U     n_rdy;
    CPU_INT16U     rxconsumeix;
    CPU_INT16U     rxproduceix;


    rxconsumeix =   RXCONSUMEINDEX;
    rxproduceix =   RXPRODUCEINDEX;

    if (rxproduceix < rxconsumeix) {                                    /* If the produce index has wrapped around                  */
        n_rdy   =   EMAC_NUM_RX_DESC - rxconsumeix + rxproduceix;
    } else {                                                            /* If consumeix is < produceix, then no wrap around occured */
        n_rdy   =   rxproduceix - rxconsumeix;
    }

    return (n_rdy);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                              LOCAL FUNCTIONS: EMAC RX FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          EMAC_RxEn()
*
* Description : Enable LPC23XX/LPC24XX EMAC Receiver.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : EMAC_Init().
*********************************************************************************************************
*/

static  void  EMAC_RxEn (void)
{
    COMMAND    |=  COMMAND_RX_EN;                                       /* Enable the receiver                                      */
    MAC1       |=  COMMAND_RX_EN;                                       /* Enable the receiver                                      */
}

/*
*********************************************************************************************************
*                                          EMAC_RxDis()
*
* Description : Disable LPC23XX/LPC24XX EMAC Receiver.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : EMAC_Init().
*********************************************************************************************************
*/

static  void  EMAC_RxDis (void)
{
    COMMAND    &= ~COMMAND_RX_EN;                                       /* Disable the receiver                                     */
    MAC1       &= ~COMMAND_RX_EN;                                       /* Disable the receiver (same bit position as COMMAND reg)  */
}

/*
*********************************************************************************************************
*                                       EMAC_RxIntEn()
*
* Description : Enable EMAC Receiver Interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_IntEn(), NetNIC_RxPktGetSize(), NetNIC_RxPkt().
*
* Notes       : 1) The RxDone interrupt occurs when a Rx descriptor has been produced by the EMAC.
*********************************************************************************************************
*/

static  void  EMAC_RxIntEn (void)
{
    INTENABLE  |= (INT_RX_DONE | INT_RX_OVERRUN);                       /* Enable RxDone interrupts and Rx Overrun interrupts       */
}

/*
*********************************************************************************************************
*                                        EMAC_RxPkt()
*
* Description : (1) Read network packet from NIC into buffer :
*                   (a) Read received frame DMA memory space.
*                   (b) Release descriptors once data has been read
*
* Argument(s) : ppkt        Pointer to memory buffer to receive NIC packet.
*               ----        Argument checked in NetNIC_RxPkt().
*
*               size        Number of packet frame octets to read into buffer.
*               ----        Argument checked in NetNIC_RxPkt().
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully transmitted.
*                               NET_ERR_RX                      Error Reading Buffers (EOF not found)
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_RxPkt().
*
* Notes       : none.
*********************************************************************************************************
*/

static  void  EMAC_RxPkt (void        *ppkt,
                          CPU_INT16U   size,
                          NET_ERR     *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR       cpu_sr = 0;
#endif
    void         *prxbuf;


   *perr                =  NET_NIC_ERR_NONE;                            /* Assume no errors have occured                            */
    prxbuf              = (void *)RxDesc[RXCONSUMEINDEX].PacketAddr;    /* Get a pointer to the Rx fragment / frame buffer          */

    RxStatus[RXCONSUMEINDEX].StatusInfo     = 0;                        /* Clear status for debugging purposes                      */

    Mem_Copy(ppkt, prxbuf, size);                                       /* Copy the fragment / frame into the stacks buffer         */

    RXCONSUMEINDEX      = (RXCONSUMEINDEX + 1) % EMAC_NUM_RX_DESC;      /* Discard the fragment / frame                             */

    CPU_CRITICAL_ENTER();                                               /* This routine alters shared data. Disable interrupts!     */
    if (NIC_RxNRdyCtr > 0) {                                            /* One less packet to process                               */
        NIC_RxNRdyCtr--;
    }
    CPU_CRITICAL_EXIT();                                                /* Re-Enable interrupts                                     */
}

/*
*********************************************************************************************************
*                                    EMAC_RxPktDiscard()
*
* Description : Discard network packet from NIC to free NIC packet frames for new receive packets.
*
* Argument(s) : Size: how much data to discard. However, we will discard an entire frame
*               so this information is not necessary.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_RxPktDiscard().
*********************************************************************************************************
*/

static  void  EMAC_RxPktDiscard (CPU_INT16U  size)
{
   (void)size;                                                          /* Size is not required since frames are discarded by index */
    RXCONSUMEINDEX      = (RXCONSUMEINDEX + 1) % EMAC_NUM_RX_DESC;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                              LOCAL FUNCTIONS: MACB TX FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          EMAC_TxEn()
*
* Description : Enable LPC23XX/LPC24XX EMAC Transmitter.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : EMAC_Init().
*********************************************************************************************************
*/

static  void  EMAC_TxEn (void)
{
    COMMAND    |=  COMMAND_TX_EN;                                       /* Enable the transmitter                                   */
}

/*
*********************************************************************************************************
*                                          EMAC_TxDis()
*
* Description : Disable LPC23XX/LPC24XX EMAC Transmitter.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : EMAC_Init().
*********************************************************************************************************
*/

static  void  EMAC_TxDis (void)
{
    COMMAND    &= ~COMMAND_TX_EN;                                       /* Disable the transmitter                                  */
}

/*
*********************************************************************************************************
*                                        EMAC_TxPkt()
*
* Description : (1) Instruct EMAC to send network packet :
*                   (a) Check if transmitter ready.
*                   (b) Clear all transmitter errors.
*                   (c) Inform transmitter about buffer address and size.
*                       This starts actual transmission of the packet.
*
* Argument(s) : ppkt        Pointer to memory buffer to transmit NIC packet.
*               ----        Argument checked in NetNIC_TxPkt().
*
*               size        Number of packet frame octets to write to frame.
*
*               perr        Pointer to variable that will hold the return error code from this function :
*
*                               NET_NIC_ERR_NONE                Packet successfully transmitted.
*                               EMAC_ERR_TX_BUSY                Transmitter not ready.
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxPkt().
*
* Notes       : 1) The TCP-IP stack is signaled immediately after issuing the Tx command
*                  because it is assumed that the hardware transmits frames much faster
*                  than the CPU can load the next Tx descriptor.
*********************************************************************************************************
*/

static  void  EMAC_TxPkt (void        *ppkt,
                          CPU_INT16U   size,
                          NET_ERR     *perr)
{
    Mem_Copy((void *)(TxDesc[TXPRODUCEINDEX].PacketAddr), ppkt, size);  /* Copy the packet into the EMAC AHB2 RAM                   */

    TxDesc[TXPRODUCEINDEX].Control =  (EMAC_TX_DESC_OVERRIDE   |        /* Override the defaults from the MAC internal registers    */
                                       EMAC_TX_DESC_PAD        |        /* Add padding for frames < 64 bytes                        */
                                       EMAC_TX_DESC_LAST       |        /* No additional descriptors to follow, this is the last    */
                                       EMAC_TX_DESC_CRC)       |        /* Append the CRC automatically                             */
                                      (size - 1);                       /* Write the size of the frame, starting from 0             */

    TXPRODUCEINDEX      =   (TXPRODUCEINDEX + 1) % EMAC_NUM_TX_DESC;    /* Increment the produce Ix register, initiate Tx of frame  */

   *perr                =    NET_NIC_ERR_NONE;                          /* No error condition                                       */

    NetOS_NIC_TxRdySignal();                                            /* Signal the stack that it may transmit another frame      */
}
