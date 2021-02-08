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

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <net.h>
#include  <net_phy.h>
#include  <net_phy_def.h>

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         NetNIC_PhyInit()
*
* Description : Initialize phyter (ethernet link controller)
*
* Argument(s) : none.
*
* Return(s)   : 1 for OK, 0 for error
*
* Caller(s)   : EMAC_Init()
*
* Note(s)     : Assumes the MDI port as already been enabled for the PHY.
*********************************************************************************************************
*/

void  NetNIC_PhyInit (NET_ERR *perr)
{
    CPU_INT16U   i;    
    unsigned short phy_reg, phy_reg2;


/*
    MCFG = MCFG_RESET_MII_MGMT;
    NetBSP_DlyMs(1);
    MCFG = MCFG_HCLK_DIV_20 | MCFG_RESET_MII_MGMT;

#if EMAC_CFG_RMII
    COMMAND |= CMD_RMII;
    SUPP = SUPP_RESET_RMII;
#else
    COMMAND &= ~CMD_RMII;
#endif
    
    MCFG &= ~MCFG_RESET_MII_MGMT;
*/
    
    /* find PHY address */
    phy_reg = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_PHYIDR1, perr) & 0xFFFF;;
    phy_reg2 = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_PHYIDR2, perr) & 0xFFFF;;
    if ((phy_reg  != 0x0022) || (phy_reg2 != 0x1619)) return;

    
    /* reset the PHY */
    NetNIC_PhyRegWr(EMAC_CFG_PHY_ADDR, PHY_BMCR, BMCR_RESET, perr);
    NetBSP_DlyMs(1);                 /* wait 3 usec */
    i = 5;
    do
     {
      phy_reg = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_BMCR, perr);
      NetBSP_DlyMs(100);
    }while ((phy_reg & BMCR_RESET) && (--i != 0));

    if (i==0)
      {
        *perr        =  NET_PHY_ERR_RESET_TIMEOUT;
        return;
      }
    
    /* sanity check... */
    phy_reg = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_BMSR, perr);
    if ((phy_reg & 0xf849) != 0x7849)
      {
        *perr        =  NET_PHY_ERR_RESET_TIMEOUT;
        return;
      }


    NetNIC_PhyAutoNeg();                                                /* Attempt Auto-Negotiation                                 */

    NetNIC_ConnStatus = NetNIC_PhyLinkState();                          /* Set NetNIC_ConnStatus according to link state            */

    if (NetNIC_ConnStatus == DEF_ON) {
        NetNIC_LinkUp();
    } else {
        NetNIC_LinkDown();
    }
    

    /*

  

  phy_reg = eth_phy_read(eth, PHY_BMCR);
  phy_reg &= ~(BMCR_AN | BMCR_RE_AN);
  if (eth->req_speed & (LINKSTAT_SPEED_100 | LINKSTAT_SPEED_100_FD))
  {
    phy_reg |= BMCR_SPEED_100;
    if (!eth->mii)
    {
      SUPP |= SUPP_SPEED;
    }
    dprintf("100 Mbps, ");
  }
  else
  {
    phy_reg &= ~BMCR_SPEED_100;
    dprintf("10 Mbps, ");
  }

  if (eth->req_speed & (LINKSTAT_SPEED_10_FD | LINKSTAT_SPEED_100_FD))
  {
    phy_reg |= BMCR_DUPLEX;
    MAC2    |= MAC2_FULL_DUPLEX;
    COMMAND |= CMD_FULL_DUPLEX;
    IPGT = 0x15;
    dprintf("Full Duplex\n");
  }
  else
  {
    phy_reg &= ~BMCR_DUPLEX;
    MAC2    &= ~MAC2_FULL_DUPLEX;
    COMMAND &= ~CMD_FULL_DUPLEX;
    dprintf("Half Duplex\n");
  }

  eth_phy_write(eth, PHY_BMCR, phy_reg);      // update link info 
 
*/
   
}


/*
*********************************************************************************************************
*                                        NetNIC_PhyAutoNeg()
*
* Description : Do link auto-negotiation
*
* Argument(s) : none.
*
* Return(s)   : 1 = no error, 0 = error
*
* Caller(s)   : NetNIC_PhyInit.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetNIC_PhyAutoNeg (void)
{
    CPU_BOOLEAN  link_state;
    CPU_INT16U   i;
    NET_ERR      err;
    unsigned short phy_reg, phy_reg2;
    
    /* set Line speed */
    phy_reg = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_ANAR, &err);
    phy_reg2 = phy_reg | (ANAR_100BT_FULL | ANAR_100BT | ANAR_10BT_FULL | ANAR_10BT);

    /* advertise capabilities and start negotiation */
    /* cannot advertise more capabilities than were originally strapped */
    /* allow about 3 seconds for operation to complete */
    NetNIC_PhyRegWr(EMAC_CFG_PHY_ADDR, PHY_ANAR, phy_reg2 & phy_reg, &err);
    NetNIC_PhyRegWr(EMAC_CFG_PHY_ADDR, PHY_BMCR, 0, &err);  /* clear BMCR_AN */
    NetNIC_PhyRegWr(EMAC_CFG_PHY_ADDR, PHY_BMCR, BMCR_AN | BMCR_RE_AN, &err);

    i = 3;
    link_state = NetNIC_PhyAutoNegState();
    while ((link_state != DEF_ON) && (i > 0)) {
        NetBSP_DlyMs(200);
        link_state = NetNIC_PhyAutoNegState();
        i--;
    }
}

/*
*********************************************************************************************************
*                                    NetNIC_PhyAutoNegState()
*
* Description : Returns state of auto-negotiation
*
* Argument(s) : none.
*
* Return(s)   : State of auto-negociation (DEF_OFF = not completed, DEF_ON = completed).
*
* Caller(s)   : NetNIC_PhyInit().
*
* Note(s)     : If any error is encountered while reading the PHY, this function
*               will return Auto Negotiation State = DEF_OFF (incomplete).
*********************************************************************************************************
*/

CPU_BOOLEAN  NetNIC_PhyAutoNegState (void)
{
    CPU_INT32U  reg_val;
    NET_ERR     err;
    
    reg_val = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_BMSR, &err);

    if (err   != NET_PHY_ERR_NONE) {
        reg_val = 0;
    }

    if ((reg_val & (BMSR_AUTO_DONE | BMSR_LINK_ESTABLISHED)) !=
              (BMSR_AUTO_DONE | BMSR_LINK_ESTABLISHED)) return (DEF_OFF);
    
    
    return (DEF_ON);
}

/*
*********************************************************************************************************
*                                     NetNIC_PhyLinkState()
*
* Description : Returns state of ethernet link
*
* Argument(s) : none.
*
* Return(s)   : State of ethernet link (DEF_OFF = link down, DEF_ON = link up).
*
* Note(s)     : If any error is encountered while reading the PHY, this function
*               will return link state = DEF_OFF.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetNIC_PhyLinkState (void)
{
    NET_ERR     err;
    CPU_INT16U  reg_val;


    reg_val      = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_BMSR, &err);

    if (err == NET_PHY_ERR_NONE) {

        if ((reg_val & BMSR_LINK_ESTABLISHED) != 0) {

            return (DEF_ON);
        } else {

            return (DEF_OFF);
        }

    } else {

        return (DEF_OFF);
    }
}

/*
*********************************************************************************************************
*                                     NetPHY_GetLinkSpeed()
*
* Description : Returns the speed of the current Ethernet link
*
* Argument(s) : none.
*
* Return(s)   : 0 = No Link, 10 = 10mbps, 100 = 100mbps
*
* Caller(s)   : EMAC_Init()
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  NetNIC_PhyLinkSpeed (void)
{
    NET_ERR     err;
    CPU_INT16U  reg_val;


    if (NetNIC_PhyLinkState() == DEF_OFF) {

        return (NET_PHY_SPD_0);

    } else {

          // Link established from here on 
          reg_val = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_100PHY, &err);

          if (err != NET_PHY_ERR_NONE) return (NET_PHY_SPD_0);
            
          // successful negotiations; update link info 
          switch(reg_val & PHYCR_MODE)
          {
            case 0x04:  // 10 BASE T Half-duplex
            case 0x14:  // 10 BASE T Full-duplex
              return (NET_PHY_SPD_10);
              
            case 0x08:  // 100 BASE TX Half-duplex
            case 0x18: // 100 BASE TX Full-duplex
              return (NET_PHY_SPD_100);
          }
    }
    return (NET_PHY_SPD_0);
}

/*
*********************************************************************************************************
*                                     NetPHY_GetDuplex()
*
* Description : Returns the duplex mode of the current Ethernet link
*
* Argument(s) : none.
*
* Return(s)   : 0 = Unknown (Auto-Neg in progress), 1 = Half Duplex, 2 = Full Duplex
*
* Caller(s)   : EMAC_Init()
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  NetNIC_PhyLinkDuplex (void)
{
    CPU_INT16U  reg_val;
    NET_ERR     err;


    if (NetNIC_PhyLinkState() == DEF_OFF) {

        return (NET_PHY_DUPLEX_UNKNOWN);

    } else {

          // Link established from here on 
          reg_val = NetNIC_PhyRegRd(EMAC_CFG_PHY_ADDR, PHY_100PHY, &err);

          if (err != NET_PHY_ERR_NONE) return (NET_PHY_DUPLEX_UNKNOWN);
          
          // successful negotiations; update link info 
          switch(reg_val & PHYCR_MODE)
          {
            case 0x04:  // 10 BASE T Half-duplex
            case 0x08:  // 100 BASE TX Half-duplex
              return (NET_PHY_DUPLEX_HALF);
              
            case 0x14:  // 10 BASE T Full-duplex
            case 0x18: // 100 BASE TX Full-duplex
              return (NET_PHY_DUPLEX_FULL);
          }
    }
  return (NET_PHY_DUPLEX_UNKNOWN);
}
