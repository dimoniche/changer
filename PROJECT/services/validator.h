#ifndef _VALIDATOR_H_
#define _VALIDATOR_H_

#define CC_VALIDATOR_SPEED    9600
#define CC_POLL_TIME_OUT      150
#define CC_BUS_RESET_TIME_OUT 100

#define VCONN_STATUS_CONN     0
#define VCONN_STATUS_NOCONN   1

// максимальное число поддерживаемых банкнот
#define V_BILLS_NUMBER  24

extern void StartUpValidator(void);
extern int IsValidatorConnected(void);
extern void VPend(void);
extern void VPost(void);
extern CPU_INT32U GetResetBillCount(CPU_INT32U *bill_index);

#endif //#define _VALIDATOR_H_
