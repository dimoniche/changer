#ifndef _COIN_H_
#define _COIN_H_



#define COIN_IMP_MIN_LEN 2200 // мс/100
#define COIN_IMP_MAX_LEN 9000 // мс/100

// запас измерения периода, плюс-минус
#define COIN_IMP_SPAN 1000 // мс/100


extern void InitCoin(void);
extern CPU_INT32U GetCoinCount(void);
extern CPU_INT32U GetResetCoinCount(void);
extern void CoinDisable(void);
extern void CoinEnable(void);
extern CPU_INT32U GetCashCount(void);
extern CPU_INT32U GetResetCashCount(void);
extern void SetCashPulseParam(CPU_INT32U pulse, CPU_INT32U pause);

#define BIT(bit)          (1UL << (bit))

#define SETBIT(Val,bit)   ((Val) |= BIT(bit))
#define CLRBIT(Val,bit)   ((Val) &= ~BIT(bit))
#define XORBIT(Val,bit)   ((Val) ^= BIT(bit))
#define TSTBIT(Val,bit)   ((Val) & BIT(bit))

#endif //#ifndef _COIN_H_


