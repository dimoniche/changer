#include <includes.h>
#include "data.h"
#include "datadesc.h"
#include "fram.h"
#include "time.h"
#include <stdlib.h>
#include <stddef.h>

// получение доступного индекса массива
CPU_INT32U GetDataValidIndex(const TDataDescStruct* desc, CPU_INT32U index)
{
    if (desc->IsArray)
    {
        if (index >= desc->ArraySize)
        {
            return desc->ArraySize;
        }
        else
        {
            return index;
        }
    }
    return 0;
}

// получение данных
int GetData(const TDataDescStruct* desc, void* buf, CPU_INT32U index, CPU_INT08U flags)
{
  TVariant32 Val;
  CPU_INT32U ofst = 0;

  if (desc->Type == DATA_TYPE_CHAR_STRING)
  {
    if (desc->Location == DATA_LOC_FRAM)
    {
        ReadArrayFram((CPU_INT32U)desc->Data, desc->ArraySize, buf);
    }
    return DATA_OK;
  }
  
  // определим доп. смещение для массива
  if (desc->IsArray)
    {
      if (flags == DATA_FLAG_DIRECT_INDEX)
        {
          ofst = (index >= desc->ArraySize) ? desc->ArrayOffset*(desc->ArraySize-1) : desc->ArrayOffset*index;
        }
      else 
        {
          GetData((TDataDescStruct*)desc->ArrayIndex, &ofst, 0, DATA_FLAG_SYSTEM_INDEX);
          ofst *= desc->ArrayOffset;
        }
    }
    
  // считаем значение параметра
  if (desc->Location == DATA_LOC_RAM)
    {
      #if OS_CRITICAL_METHOD == 3
      OS_CPU_SR  cpu_sr = 0;
      #endif
      OS_ENTER_CRITICAL();
      memcpy(&Val, (CPU_INT08U*)desc->Data+ofst, sizeof(CPU_INT32U));
      OS_EXIT_CRITICAL();
    }
  else if (desc->Location == DATA_LOC_FRAM)
    {
      ReadArrayFram((CPU_INT32U)desc->Data+ofst, sizeof(CPU_INT32U), (CPU_INT08U*)&Val);
    }
  else return DATA_ERR;

  // всё ОК
  memcpy(buf, &Val, sizeof(CPU_INT32U));
  
  return DATA_OK;
}

///
int SetDataFromStr(const TDataDescStruct* desc, char* buf, CPU_INT32U index, CPU_INT08U flags)
{
    CPU_INT32U value;
    
    switch (desc->Type)
    {
        case DATA_TYPE_RUB_CENT:
            {
                CPU_INT32U value1, value2;
                if ((sscanf(buf, "%d,%02d", &value1, &value2) == 2) && (value2 < 100))
                {
                    value1 = value1 * 100 + value2;
                    return SetData(desc, (void*)&value1, index, flags);
                }
            }
            break;
        case DATA_TYPE_TIME_COUNT:
        case DATA_TYPE_SLONG:
        case DATA_TYPE_ULONG:
            if (sscanf(buf, "%d", &value) == 1)
            {
                return SetData(desc, (void*)&value, index, flags);
            }
            else
            {
                if ((desc->IsIndex) && (desc->Desc == DATA_DESC_EDIT))
                {
                    // попробуем найти индекс текстового значения
                    if ((desc->RangeValue) && (desc->Items))
                    {
                        TRangeValueULONG* range = (TRangeValueULONG*)desc->RangeValue;
                        CPU_INT32U val_i;
                        for (val_i = range->Min; val_i <= range->Max; val_i++)
                        {
                            if (strcmp(buf, (char const*)desc->Items[val_i]) == 0)
                            {
                                value = val_i;
                                return SetData(desc, (void*)&value, index, flags);
                            }
                        }
                    }
                }
            }
            break;
        case DATA_TYPE_FLOAT:
            {
                float value_fl;
                if (sscanf(buf, "%f", &value_fl) == 1)
                {
                    return SetData(desc, (void*)&value_fl, index, flags);
                }
            }
            break;
        case DATA_TYPE_TIME:
            {
                TRTC_Data rtc;
                ScanRTCDateTimeStringRus((char*)buf, &rtc);
                if (RTCCheckTime(&rtc) == 0)
                {
                    value = GetSec(&rtc);
                    return SetData(desc, &value, index, flags);
                }
            }
            break;
        case DATA_TYPE_HOUR_MIN:
            {
                int hour, min, hour_min;
                if (sscanf((char*)buf, "%02d:%02d", &hour, &min) == 2)
                {
                    hour_min = hour * 60 + min;
                    return SetData(desc, &hour_min, index, flags);
                }
            }
            break;
        case DATA_TYPE_TIME_SEC_H_MM:
            {
                int hour, min, sec;
                if (sscanf((char*)buf, "%02d:%02d", &hour, &min) == 2)
                {
                    sec = hour * 3600 + min * 60;
                    return SetData(desc, &sec, index, flags);
                }
            }
            break;
        case DATA_TYPE_TIME_SEC_M:
            {
                int min, sec;
                if (sscanf((char*)buf, "%d", &min) == 1)
                {
                    sec = min * 60;
                    return SetData(desc, &sec, index, flags);
                }
            }
            break;
        case DATA_TYPE_DATE:
            {
                TRTC_Data rtc;
                ScanRTCDateStringRus((char*)buf, &rtc);
                if (RTCCheckTime(&rtc) == 0)
                {
                    value = GetSec(&rtc);
                    return SetData(desc, &value, index, flags);
                }
            }
            break;
        case DATA_TYPE_IP_ADDR:
            {
                CPU_INT32U ip[4];
                if ((sscanf((char*)buf, "%d.%d.%d.%d", &ip[3], &ip[2], &ip[1], &ip[0]) == 4)
                    && (ip[0] <= 255)
                    && (ip[1] <= 255)
                    && (ip[2] <= 255)
                    && (ip[3] <= 255)
                   )
                {
                    value = ip[0] + (ip[1] << 8) + (ip[2] << 16) + (ip[3] << 24);
                    return SetData(desc, &ip[0], index, flags);
                }
            }
            break;
        case DATA_TYPE_CHAR_STRING:
            {
                return SetData(desc, buf, index, flags);
            }
            break;
    }
        
    return DATA_ERR;
}


// запись данных
int SetData(const TDataDescStruct* desc, void* buf, CPU_INT32U index, CPU_INT08U flags)
{
  TVariant32 Val;
  CPU_INT32U ofst = 0;

  if (desc->Desc == DATA_DESC_VIEW) return DATA_ERR;
    
  if (desc->Type == DATA_TYPE_CHAR_STRING)
  {
      char *s = buf;
      int i;
      for (i = 0; i < desc->ArraySize; i++, s++)
      {
        if (*s == 0) break;
        if ((*s < '!') ||  (*s > 'z'))
        {
          return DATA_ERR;
        }
      }
      
      if (desc->Location == DATA_LOC_RAM)
        {
          #if OS_CRITICAL_METHOD == 3
          OS_CPU_SR  cpu_sr = 0;
          #endif
          OS_ENTER_CRITICAL();
          memcpy((CPU_INT08U*)desc->Data, buf, i + 1);
          OS_EXIT_CRITICAL();
        }
      else if (desc->Location == DATA_LOC_FRAM)
        {
          CPU_INT08U byte = 0;
          WriteArrayFram((CPU_INT32U)desc->Data, i, buf);
          WriteArrayFram((CPU_INT32U)desc->Data + i, 1, &byte);
        }
      else return DATA_ERR;
  
      if (desc->OnchangeFunc) desc->OnchangeFunc();
          
      return DATA_OK;
  }

      
  // проверим допустимость значений
  if (desc->RangeValue)
  {
      TVariant32 ValMin, ValMax;
      TRangeValueULONG* RVal = desc->RangeValue;
      
      memcpy(&ValMin, &RVal->Min, sizeof(CPU_INT32U));
      memcpy(&ValMax, &RVal->Max, sizeof(CPU_INT32U));
      memcpy(&Val, buf, sizeof(CPU_INT32U));
      
      if (desc->Type == DATA_TYPE_ULONG)
        {
          if ((Val.Val32U > ValMax.Val32U) || (Val.Val32U < ValMin.Val32U)) return DATA_ERR;
        }
      else if (desc->Type == DATA_TYPE_SLONG)
        {
          if ((Val.Val32S > ValMax.Val32S) || (Val.Val32S < ValMin.Val32S)) return DATA_ERR;
        }
      else if (desc->Type == DATA_TYPE_FLOAT)
        {
          if ((Val.ValFloat > ValMax.ValFloat) || (Val.ValFloat < ValMin.ValFloat)) return DATA_ERR;
        }
      else if (desc->Type == DATA_TYPE_TIME)
        {
        
        }
      else if (desc->Type == DATA_TYPE_IP_ADDR)
        {
        
        }
      else if (desc->Type == DATA_TYPE_RUB_CENT)
        {
        
        }
      else if (desc->Type == DATA_TYPE_HOUR_MIN)
        {
          if (Val.Val32U >= 24*60) return DATA_ERR;
        }
      else if (desc->Type == DATA_TYPE_TIME_SEC_H_MM)
      {
      
      }
      else return DATA_ERR;
  }     
  else
  {
    memcpy(&Val, buf, sizeof(CPU_INT32U));
  }
  // определим доп. смещение для массива
  if (desc->IsArray)
    {
      if (flags == DATA_FLAG_DIRECT_INDEX)
        {
          ofst = (index >= desc->ArraySize) ? desc->ArrayOffset*(desc->ArraySize-1) : desc->ArrayOffset*index;
        }
      else 
        {
          GetData((TDataDescStruct*)desc->ArrayIndex, &ofst, 0, DATA_FLAG_SYSTEM_INDEX);
          ofst *= desc->ArrayOffset;
        }
    }

  // запишем значение
  if (desc->Location == DATA_LOC_RAM)
    {
      #if OS_CRITICAL_METHOD == 3
      OS_CPU_SR  cpu_sr = 0;
      #endif
      OS_ENTER_CRITICAL();
      memcpy((CPU_INT08U*)desc->Data+ofst, &Val, sizeof(CPU_INT32U));
      OS_EXIT_CRITICAL();
    }
  else if (desc->Location == DATA_LOC_FRAM)
    {
      WriteArrayFram((CPU_INT32U)desc->Data+ofst, sizeof(CPU_INT32U), (CPU_INT08U*)&Val);
    }
  else return DATA_ERR;
  
  // функция по изменению
  if (desc->OnchangeFunc) desc->OnchangeFunc();
  
  // всё ОК
  return DATA_OK;
}

// получение максимума параметра дескриптора
int GetDataMax(const TDataDescStruct* desc, void* buf)
{
  if (desc->RangeValue)
  {
  TRangeValueULONG* RVal = desc->RangeValue;
  memcpy(buf, &RVal->Max, sizeof(CPU_INT32U));
  }
  else
  {
  *(CPU_INT32U*)&buf = 0;
  }
  return DATA_OK;
}

// получение минимума параметра дескриптора
int GetDataMin(const TDataDescStruct* desc, void* buf)
{
  if (desc->RangeValue)
  {
  TRangeValueULONG* RVal = desc->RangeValue;
  memcpy(buf, &RVal->Min, sizeof(CPU_INT32U));
  }
  else
  {
  *(CPU_INT32U*)&buf = 0;
  }
  return DATA_OK;
}

// получение строки с отформатированным значением
int GetDataStr(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U index, CPU_INT08U flags)
{
  TVariant32 Val;
  
  if (desc->Type == DATA_TYPE_CHAR_STRING)
  {
      return GetData(desc, buf, index, flags);
  }
      
  GetData(desc, &Val, index, flags);
  
  if (desc->Type == DATA_TYPE_ULONG)
    {
      if (desc->IsIndex)
        { // индексный параметр
          if (desc->RangeValue)
          {
            TRangeValueULONG* range = (TRangeValueULONG*)desc->RangeValue;
            if ((Val.Val32U >= range->Min) && (Val.Val32U <= range->Max)) strcpy((char*)buf, (char const*)desc->Items[Val.Val32U]);
            else {strcpy((char*)buf, ""); return DATA_ERR;}
          }
          else if (desc->Desc == DATA_DESC_VIEW)
          {
            strcpy((char*)buf, (char const*)desc->Items[Val.Val32U]);
          }
          else
          {
            strcpy((char*)buf, "");
          }
        }
      else
        {
          sprintf((char*)buf, "%d", Val.Val32U);
        }
    }
  else if (desc->Type == DATA_TYPE_SLONG)
    {
      sprintf((char*)buf, "%d", Val.Val32S);      
    }
  else if (desc->Type == DATA_TYPE_FLOAT)
    {
      sprintf((char*)buf, "%0.3f", Val.ValFloat);            
    }
  else if (desc->Type == DATA_TYPE_TIME)
    {
      PrintTimeString((char*)buf, Val.Val32U);
    }
  else if (desc->Type == DATA_TYPE_RUB_CENT)
    {
      sprintf((char*)buf, "%d,%02d", Val.Val32U / 100, Val.Val32U % 100);            
    }
  else if (desc->Type == DATA_TYPE_IP_ADDR)
    {
      NET_ERR   err;
      NetASCII_IP_to_Str(Val.Val32U, (CPU_CHAR*)buf, DEF_NO, &err);
    }
  else if (desc->Type == DATA_TYPE_TIME_COUNT)
    {
      PrintSecToBigHourMinSec((char*)buf, Val.Val32U);
    }
  else if (desc->Type == DATA_TYPE_HOUR_MIN)
    {
      int min_ = Val.Val32U % 60;
      int hour_ = Val.Val32U / 60;
      sprintf((char*)buf, "%02d:%02d", hour_, min_);
    }
  else if (desc->Type == DATA_TYPE_TIME_SEC_H_MM)
    {
      int min_ = (Val.Val32U / 60) % 60;
      int hour_ = Val.Val32U / 3600;
      sprintf((char*)buf, "%d:%02d", hour_, min_);
    }
  else if (desc->Type == DATA_TYPE_TIME_SEC_M_SS)
    {
      int sec_ = Val.Val32U % 60;
      int min_ = Val.Val32U / 60;
      if (min_ < 60)
      {
        sprintf((char*)buf, "%d:%02d", min_, sec_);
      }
      else
      {
        int hour_ = min_ / 60;
        min_ = (Val.Val32U / 60) % 60;
        sprintf((char*)buf, "%d:%02d:%02d", hour_, min_, sec_);
      }
    }
  else if (desc->Type == DATA_TYPE_TIME_SEC_M)
    {
      int min_ = Val.Val32U / 60;
      sprintf((char*)buf, "%d", min_);
    }
  else if (desc->Type == DATA_TYPE_DATE)
    {
      PrintDateString((char*)buf, Val.Val32U);
    }
  else return DATA_ERR;
  
  return DATA_OK;
}

// получение полной строки с названием и значением
int GetDataFullStr(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U index, CPU_INT08U flags)
{
  GetDataNameStr(desc, buf);
  if (desc->Name)
    {
      if (desc->IsIndex) strcat((char*)&buf[strlen((char*)buf)], " ");
      else strcat((char*)&buf[strlen((char*)buf)], "=");
    }
  GetDataStr(desc, &buf[strlen((char*)buf)], index, flags);
  return DATA_OK;
}

// получение строки с именем
int GetDataNameStr(const TDataDescStruct* desc, CPU_INT08U* buf)
{
  if (desc->Name) strcpy((char*)buf, (char const*)desc->Name);
  else strcpy((char*)buf, "");
  return DATA_OK;
}


// получение строки со значением ндексной строки по индексу
int GetDataItem(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U itemindex)
{
  if (!desc->IsIndex) {buf[0]=0;return DATA_ERR;}
    
  if (desc->Type != DATA_TYPE_ULONG) {buf[0]=0;return DATA_ERR;}

  // индексный параметр
  if (desc->RangeValue)
  {
  TRangeValueULONG* range = (TRangeValueULONG*)desc->RangeValue;
  if ((itemindex >= range->Min) && (itemindex <= range->Max)) strcpy((char*)buf, (char const*)desc->Items[itemindex]);
  else return DATA_ERR;
  }
  else
  {
  strcpy((char*)buf, "");
  }
 
  return DATA_OK;
}

// инициализация по умолчанию
int InitDataByDefault(const TDataDescStruct* desc, CPU_INT32U index)
{
  if (desc->Type == DATA_TYPE_CHAR_STRING) SetData(desc, (void*)desc->DefaultValue.Val32U, 0, DATA_FLAG_SYSTEM_INDEX);
  else SetData(desc, (void*)&desc->DefaultValue, index, DATA_FLAG_DIRECT_INDEX);
  return DATA_OK;
}

// инициализация при старте
int InitData(const TDataDescStruct* desc)
{
  return DATA_OK;
}

// проверка границ
int CheckDataRange(const TDataDescStruct* desc)
{
  TVariant32 ValMin, ValMax, Val;
  TRangeValueULONG* RVal = desc->RangeValue;
  
  if (!desc->RangeValue) return DATA_OK;
    
  memcpy(&ValMin, &RVal->Min, sizeof(CPU_INT32U));
  memcpy(&ValMax, &RVal->Max, sizeof(CPU_INT32U));
  
  if (desc->IsArray)
  {
    for (int i = 0; i < desc->ArraySize; i++)
    {
      GetData(desc, &Val, i, DATA_FLAG_DIRECT_INDEX);
      if (desc->Type == DATA_TYPE_ULONG)
        {
          if ((Val.Val32U > ValMax.Val32U) || (Val.Val32U < ValMin.Val32U)) InitDataByDefault(desc, i);
        }
      else if (desc->Type == DATA_TYPE_SLONG)
        {
          if ((Val.Val32S > ValMax.Val32S) || (Val.Val32S < ValMin.Val32S)) InitDataByDefault(desc, i);
        }
      else if (desc->Type == DATA_TYPE_FLOAT)
        {
          if ((Val.ValFloat > ValMax.ValFloat) || (Val.ValFloat < ValMin.ValFloat)) InitDataByDefault(desc, i);
        }
      else return DATA_ERR;
    }
  }
  else 
  {
      GetData(desc, &Val, 0, DATA_FLAG_DIRECT_INDEX);
      if (desc->Type == DATA_TYPE_ULONG)
        {
          if ((Val.Val32U > ValMax.Val32U) || (Val.Val32U < ValMin.Val32U)) InitDataByDefault(desc, 0);
        }
      else if (desc->Type == DATA_TYPE_SLONG)
        {
          if ((Val.Val32S > ValMax.Val32S) || (Val.Val32S < ValMin.Val32S)) InitDataByDefault(desc, 0);
        }
      else if (desc->Type == DATA_TYPE_FLOAT)
        {
          if ((Val.ValFloat > ValMax.ValFloat) || (Val.ValFloat < ValMin.ValFloat)) InitDataByDefault(desc, 0);
        }
      else return DATA_ERR;

  }
  
  
  return DATA_OK;
}

// инициализация по умолчанию, включая массивы
int InitDescByDefault(const TDataDescStruct* desc)
{
  if (desc->IsArray)
  {
    for (int i = 0; i < desc->ArraySize; i++) InitDataByDefault(desc, i);
  }
  else 
  {
    InitDataByDefault(desc, 0);
  }

  return DATA_OK;
}

// проверка всех дестрипторов
int CheckAllData(void)
{
  int i = 0;
  while (AllDataArray[i].ptr != NULL)
    {
      CheckDataRange(AllDataArray[i].ptr);
      i++;
    }
  
  return DATA_OK;
}


/// поиск дескриптора для имени
void FindDescByName(char* name, TDataDescStruct const** desc, CPU_INT32U *index)
{
    char justname[48];
    char *name_ptr = name;
    *index = 0;
    
    if (sscanf(name, "%s %d", justname, index) == 2)
    {   
        name_ptr = justname;
    }

    *desc = NULL;
    int i = 0;
    while (AllDataArray[i].ptr != NULL)
    {
        if (strcmp(AllDataArray[i].name, name_ptr) == 0)
        {
            if ((AllDataArray[i].ptr->IsArray) && (*index >= AllDataArray[i].ptr->ArraySize))
            {
                *desc = NULL;
            }
            else
            {
                *desc = AllDataArray[i].ptr;
            }
            break;
        }
        i++;
    }
}

/// чтение имени дескриптора
void GetDescIdStr(TDataDescStruct const* desc, char* name)
{
    *name = '\0';
    int i = 0;
    while (AllDataArray[i].ptr != NULL)
    {
        if (desc == AllDataArray[i].ptr)
        {
            strcpy(name, AllDataArray[i].name);
            break;
        }
        i++;
    }
}

/// поиск дескриптора по имени
void GetDescByIdStr(char* name, TDataDescStruct const** desc)
{
    int i = 0;

    *desc = NULL;
    
    while (AllDataArray[i].ptr != NULL)
    {
        if (strcmp(AllDataArray[i].name, name) == 0)
        {
            *desc = AllDataArray[i].ptr;
            break;
        }
        i++;
    }
}
