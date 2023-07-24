#ifndef _COIN_H_
#define _COIN_H_

#define COIN_IMP_MIN_LEN 22 // мс/100
#define COIN_IMP_MAX_LEN 90 // мс/100

// запас измерения периода, плюс-минус
#define COIN_IMP_SPAN 10 // мс/100

extern volatile char event_nomoney_hopper;

extern void InitCoin(void);
extern CPU_INT32U GetCoinCount(void);
extern CPU_INT32U GetResetCoinCount(void);
extern void CoinDisable(void);
extern void CoinEnable(void);
extern void BankDisable(void);
extern void BankEnable(void);
extern void HopperDisable(void);
extern void HopperEnable(void);

extern void InitInputPorts();
extern CPU_INT32U GetCashCount(void);
extern CPU_INT32U GetResetCashCount(void);
extern CPU_INT32U GetbankCount(void);
extern CPU_INT32U GetResetbankCount(void);
extern CPU_INT32U GetHopperCount();
extern CPU_INT32U GetResetHopperCount(void);

extern void SetCoinPulseParam(CPU_INT32U pulse, CPU_INT32U pause);
extern void SetCashPulseParam(CPU_INT32U pulse, CPU_INT32U pause);
extern void SetBankPulseParam(CPU_INT32U pulse, CPU_INT32U pause);
extern void SetHopperPulseParam(CPU_INT32U pulse, CPU_INT32U pause);
extern void SetLevelParam(CPU_INT32U level1, CPU_INT32U level2, CPU_INT32U level3, CPU_INT32U level4);

extern void initOutputPorts(void);
extern void initHopper(void);

#define BIT(bit)          (1UL << (bit))

#define SETBIT(Val,bit)   ((Val) |= BIT(bit))
#define CLRBIT(Val,bit)   ((Val) &= ~BIT(bit))
#define XORBIT(Val,bit)   ((Val) ^= BIT(bit))
#define TSTBIT(Val,bit)   ((Val) & BIT(bit))

#endif //#ifndef _COIN_H_


