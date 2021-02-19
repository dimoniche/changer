#include "iolpc2368.h"
#include "ucos_ii.h"
#include "cpu.h"
#include "app_serv.h"
#include "time.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void  RTC_Isr (void)
{
  CPU_INT32U ilr;
  
  ilr = ILR & 0x7;
  if (ilr & 0x1) PostUserEvent(EVENT_SEC); 
  
  ILR = ilr;
}


void RTC_ReadTime(TRTC_Data *rtc)
{
  #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  rtc->sec = SEC;
  rtc->min = MIN;
  rtc->hour = HOUR;
  rtc->day = DOW;
  rtc->date = DOM;
  rtc->mon = MONTH;
  rtc->year = YEAR-2000;
  OS_EXIT_CRITICAL();
}

void RTC_SetTime(TRTC_Data *rtc)
{
  #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  SEC = rtc->sec;
  MIN = rtc->min;
  HOUR = rtc->hour;
  DOW = rtc->day;
  DOM = rtc->date;
  MONTH = rtc->mon;
  YEAR = rtc->year+2000;
  OS_EXIT_CRITICAL();
}


extern CPU_INT32U  BSP_CPU_PclkFreq (CPU_INT08U  pclk);

void InitRTC(void)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
#endif
   
    PCONP_bit.PCRTC = 1;
    PCLKSEL0_bit.PCLK_RTC = 0;
    
    OS_ENTER_CRITICAL();

    CCR     = 0x0011;
    PREINT  = (BSP_CPU_PclkFreq(9) / 32768) - 1;
    PREFRAC = BSP_CPU_PclkFreq(9) - ((PREINT + 1) * 32768);

    // If year is corrupt, set reasonable date.
    if ((YEAR < 2010) || (HOUR > 23) || (YEAR > 2100)) {
        CCR     = 0x0000;
        YEAR   = 2010;
        MONTH  = 8;
        DOM    = 8;
        HOUR   = 7;
        MIN    = 0;
        CCR     = 0x0011;
    }
    
    VICVECTADDR13 = (CPU_INT32U)RTC_Isr;
    VICINTSELECT &= ~(1 << VIC_RTC);
    VICVECTPRIORITY13 = 7;
    VICINTENABLE =  (1 << VIC_RTC);
    
    CISS = 0;
    AMR = 0xff;
    CIIR = 0x1;  // sec interrupt
    
    OS_EXIT_CRITICAL();
}


const char str_sunday[] = "ВОСКРЕСЕНbЕ";
const char str_monday[] = "ПОНЕДЕЛbНИК";
const char str_tuesday[] = "ВТОРНИК";
const char str_wednesday[] = "СРЕДА";
const char str_thirsday[] = "ЧЕТВЕРГ";
const char str_friday[] = "ПЯТНИЦА";
const char str_saturday[] = "СУББОТА";
const char* weekday[]={str_sunday, str_monday, str_tuesday, str_wednesday, str_thirsday, str_friday, str_saturday};

void GetDayText(char* str, char day)
{
  strcpy(str, weekday[day]);
}

const char str_jan[] = "ЯНВАРЯ";
const char str_feb[] = "ФЕВРАЛЯ";
const char str_mar[] = "МАРТА";
const char str_apr[] = "АПРЕЛЯ";
const char str_may[] = "МАЯ";
const char str_jun[] = "ИЮНЯ";
const char str_jul[] = "ИЮЛЯ";
const char str_aug[] = "АВГУСТА";
const char str_sep[] = "СЕНТЯБРЯ";
const char str_okt[] = "ОКТЯБРЯ";
const char str_nov[] = "НОЯБРЯ";
const char str_dec[] = "ДЕКАБРЯ";
const char* month[]={str_jan, str_feb, str_mar, str_apr, str_may, str_jun, str_jul, str_aug, str_sep, str_okt, str_nov, str_dec};


//Число секунд от Рождества Христова до 1 января 1970
#define JESUS_TO_70     2007437056
//Число минут от Рождества Христова до 1 января 1970
#define JESUS_TO_70MIN  1035616320

// Число дней с начала года
CPU_INT16U  const mon_add[13] = {0,0,31,59,90,120,151,181,212,243,273,304,334};
// Число дней в месяцах
CPU_INT08U const  mon_len[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

void PrintRTCDateTimeStringRus(char *str, TRTC_Data *rtc)
{
  sprintf(str, "%02d:%02d:%02d %02d/%02d/%02d", rtc->hour, rtc->min, rtc->sec, rtc->date, rtc->mon, rtc->year);
}      

void ScanRTCDateTimeStringRus(char *str, TRTC_Data *rtc)
{
  int hour, min, sec, date, mon, year;
  sscanf(str, "%02d:%02d:%02d %02d/%02d/%02d", &hour, &min, &sec, &date, &mon, &year);
  rtc->year = year;
  rtc->mon = mon;
  rtc->date = date;
  rtc->hour = hour;
  rtc->min = min;
  rtc->sec = sec;  
}      

// проверка корректности даты/времени
int RTCCheckTime(TRTC_Data *rtc)
{
  if (rtc->hour > 23) return -1;
  if (rtc->min > 59)  return -2;
  if (rtc->sec > 59)  return -3;
  if ((rtc->mon > 12) || (rtc->mon < 1)) return -5;
  if (rtc->year > 99) return -6;

  if ((rtc->year%4 == 0) && (rtc->mon == 2))
    { // висоскосный
      if ((rtc->date == 0) || (rtc->date > 29)) return -4;     
    }
  else
    {
      if ((rtc->date == 0) || (rtc->date > mon_len[rtc->mon])) return -4;     
    }
  return 0;
}

void PrintRTCDateTimeString(char *str, TRTC_Data *rtc)
{
  sprintf(str, "20%02d/%02d/%02d %02d:%02d:%02d", rtc->year, rtc->mon, rtc->date, rtc->hour, rtc->min, rtc->sec);
}      

void PrintRTCTimeString(char *str, TRTC_Data *rtc)
{
  sprintf(str, "%02d:%02d:%02d", rtc->hour, rtc->min, rtc->sec);
}      

void PrintTimeString(char *str, CPU_INT32U time)
{
  TRTC_Data rtc_data;
  Sec2Date(&rtc_data, time);
  PrintRTCDateTimeStringRus(str, &rtc_data);
}

void PrintSecToMinSec(char *str, int seconds)
{
  int min_ = seconds/60;
  int sec_ = seconds%60;
  sprintf(str, "%02d:%02d", min_, sec_);
}      
      
void PrintSecToHourMinSec(char *str, int seconds)
{
  int min_ = (seconds/60)%60;
  int sec_ = seconds%60;
  int hour_ = seconds/3600;
  sprintf(str, "%02d:%02d:%02d", hour_, min_, sec_);
}      

void PrintSecToBigHourMinSec(char *str, int seconds)
{
  int min_ = (seconds/60)%60;
  int sec_ = seconds%60;
  int hour_ = seconds/3600;
  sprintf(str, "%d:%02d:%02d", hour_, min_, sec_);
}      

CPU_INT32U Date2Sec(TRTC_Data *pData, CPU_INT08U ucType)
{ // Расчет года от Рождества Христова
  CPU_INT16U year = (CPU_INT16U)pData->year + ((pData->year >= 70) ? 1900 : 2000);

  //Расчет месяцев от Рождества Христова
  CPU_INT32U ulIndex = (CPU_INT32U)(((year - 1) * 12) + pData->mon - 1);

  //Для месячного архива расчет закончен.
  if ( ucType != MONTH_TYPE )
  { //Расчет дней от Рожедства  Христова
    ulIndex = ((year - 1) * 365L) + ((year - 1)/4) + mon_add[pData->mon] + pData->date;

    //Коррекция на текущий високосный год.
    if ( (!(year % 4)) && (pData->mon > 2) ) {ulIndex++;}

    //Для суточного архива расчет закончен
    if ( ucType != DAY_TYPE )
    { //Число чаcов
      ulIndex = ulIndex * 24L + pData->hour;

      if ( ucType != HOUR_TYPE )
      { //Число минут.
        ulIndex = ulIndex * 60L + pData->min;

        if ( ucType != MIN_TYPE )
        { //Число секунд.
          ulIndex = ulIndex * 60L + pData->sec;
        }//if(  )
      }//if(  )
    }//if(  )
  }//if(  )

  return ulIndex;
}

//------------------------------------------------------------------------
//Расчет секунд с 1 января 1970 до момента, определяемого структурой pData
//------------------------------------------------------------------------
CPU_INT32U GetSec( TRTC_Data *pData)
{
  CPU_INT32U ulIndex = Date2Sec(pData, MIN_TYPE);//Получаем число минут о Рождества Христова.
  ulIndex -= JESUS_TO_70MIN;              //Получаем число минут с 1 января 1970 года.
  ulIndex *= 60L;                         //Переводим в секунды
  ulIndex += pData->sec;                //Добавляем число секунд в данной минуте.

  return ulIndex;
}

// получение системного времени в сeкундах
CPU_INT32U GetTimeSec(void)
{
  TRTC_Data rtc;
  RTC_ReadTime(&rtc);
  return GetSec(&rtc);
}


//Перевод числа секунд в часы
void Sec2Hour(TRTC_Data *pDest, CPU_INT32U ulSec)
{
  pDest->year  = 0;
  pDest->mon = 0;
  pDest->date   = 0;
  pDest->hour  = ulSec/3600L; //часы

  ulSec %= 3600L;
  pDest->min   = ulSec/60L;    //минуты

  ulSec %= 60L;
  pDest->sec   = ulSec;        //секунды
}


// Перевод числа секунд с 1 января 1970 года в параметры структуры TRTC_Data.
void Sec2Date(TRTC_Data *pDest, CPU_INT32U ulSec)
{
  CPU_INT32U  dl = 0;

  dl = (ulSec)/86400L;
  // узнали день недели
  pDest->day = (dl+4)%7;

  if ( ulSec >= 946684800L )
  { //Дата позже 1 января 2000 года
    ulSec -= 946684800L;
    pDest->year = 0;
  }//if(  )
  else
  { //Дата от 1970 года до 1999 года
    pDest->year = 70;
  }//else (  )

  for ( dl = 365L;
        ulSec >= (dl = 86400L * (365L + visocosn(pDest->year)));
        ulSec -= dl, pDest->year++ ){;}

  for ( pDest->mon = 1;
        ulSec >= ( dl = 86400L *
        (mon_len[pDest->mon] + ((pDest->mon == 2) ?
         visocosn(pDest->year) : 0))); ulSec -= dl, pDest->mon++){;}

  pDest->date = ulSec / (86400L) + 1;
  ulSec %= 86400L;
  pDest->hour = ulSec / 3600L;
  ulSec %= 3600L;
  pDest->min = ulSec / 60L;
  ulSec %= 60L;
  pDest->sec = ulSec;
}


