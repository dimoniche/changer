#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__



#define  uC_TCPIP_MODULE                 DEF_ENABLED


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                   5

#define  NET_OS_CFG_IF_RX_TASK_PRIO            6
#define  NET_OS_CFG_TMR_TASK_PRIO              7
#define  APP_TASK_PHY_PRIO                     8

#define  USER_HIGHEST_PRIO                     9

#define  USER_LOWEST_PRIO                      (OS_LOWEST_PRIO - 3)

#define  OS_TASK_TMR_PRIO                      (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE             128
#define  NET_OS_CFG_TMR_TASK_STK_SIZE        300
#define  NET_OS_CFG_IF_RX_TASK_STK_SIZE      256
#define  APP_TASK_PHY_STK_SIZE               256

/*
*********************************************************************************************************
*                                       uC/OS-II DCC CONFIGURATION
*********************************************************************************************************
*/

#define  OS_CPU_ARM_DCC_EN                       0

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                       0
#define  TRACE_LEVEL_INFO                      1
#define  TRACE_LEVEL_DEBUG                     2

#define  APP_TRACE_LEVEL                TRACE_LEVEL_DEBUG
#define  APP_TRACE                        printf

#define  APP_TRACE_INFO(x)            ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DEBUG(x)           ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)




#endif
