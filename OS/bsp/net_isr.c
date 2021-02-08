/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2005; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                BOARD SUPPORT PACKAGE (BSP) FUNCTIONS
*
*                                             NXP LPC2378
*                                                on the
*                                    KEIL MCB2300 Evaluation Board
*
* Filename      : net_isr.c
* Version       : V1.89
* Programmer(s) : Brian Nagel
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>
#include  <net.h>

/*
*********************************************************************************************************
*                                   NETWORK INTERRUPT INITIALIZATION
*
* Description : This function is called to initialize the interrupt controller associated with the NIC.
*
* Arguments   : None.
*
* Return(s)   : None.
*********************************************************************************************************
*/

void  NetNIC_IntInit  (void)
{
    VICINTSELECT       &= ~(1 << VIC_ETHERNET);                         /* Configure the Ethernet VIC interrupt source for IRQ      */
    VICVECTADDR21       =  (CPU_INT32U)NetNIC_ISR_Handler;              /* Set the vector address                                   */
    VICINTENABLE        =  (1 << VIC_ETHERNET);                         /* Enable the VIC interrupt source, but no local sources    */

}
