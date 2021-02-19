#ifndef _TIME_H_
#define _TIME_H_


#define visocosn(year) ((year % 4) ? 0 : 1)

typedef struct{
  CPU_INT08U sec;
  CPU_INT08U min;
  CPU_INT08U hour;
  CPU_INT08U day;
  CPU_INT08U date;
  CPU_INT08U mon;
  CPU_INT08U year;
}TRTC_Data;

#define SEC_TYPE      1
#define MIN_TYPE      2
#define HOUR_TYPE     3
#define DAY_TYPE      4
#define MONTH_TYPE    5
#define YEAR_TYPE     6

extern void InitRTC(void);
extern void RTC_ReadTime(TRTC_Data *rtc);
extern void RTC_SetTime(TRTC_Data *rtc);
extern void Sec2Date(TRTC_Data *pDest, CPU_INT32U ulSec);
extern void Sec2Hour(TRTC_Data *pDest, CPU_INT32U ulSec);
extern CPU_INT32U GetSec(TRTC_Data *pData);
extern CPU_INT32U Date2Sec(TRTC_Data *pData, CPU_INT08U ucType);
extern void PrintSecToMinSec(char *str, int seconds);
extern void PrintRTSTimeString(char *str, TRTC_Data *rtc);
extern void PrintRTCDateTimeString(char *str, TRTC_Data *rtc);
extern void PrintRTCDateTimeStringRus(char *str, TRTC_Data *rtc);
extern void ScanRTCDateTimeStringRus(char *str, TRTC_Data *rtc);
extern void GetDayText(char* str, char day);
extern CPU_INT32U GetTimeSec(void);
int RTCCheckTime(TRTC_Data *rtc);
extern void PrintTimeString(char *str, CPU_INT32U time);
extern void PrintSecToHourMinSec(char *str, int seconds);
extern void PrintSecToBigHourMinSec(char *str, int seconds);


#endif //#ifndef _TIME_H_
