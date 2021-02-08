#include <includes.h>
#include "data.h"
#include "datadesc.h"
#include "fram.h"
#include "time.h"
#include <stdlib.h>
#include <stddef.h>


// получение данных
int GetData(const TDataDescStruct* desc, void* buf, CPU_INT32U index, CPU_INT08U flags)
{
  TVariant32 Val;
  CPU_INT32U ofst = 0;

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

// запись данных
int SetData(const TDataDescStruct* desc, void* buf, CPU_INT32U index, CPU_INT08U flags)
{
  TVariant32 Val;
  CPU_INT32U ofst = 0;

  if (desc->Desc == DATA_DESC_VIEW) return DATA_ERR;
    
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
      else if (desc->Type == DATA_TYPE_HOUR_MIN)
        {
          if (Val.Val32U >= 24*60) return DATA_ERR;
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
  SetData(desc, (void*)&desc->DefaultValue, index, DATA_FLAG_DIRECT_INDEX);
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

static char justname[128];

/// поиск дескриптора для имени
void FindDescByName(char* name, TDataDescStruct const** desc, CPU_INT32U *index)
{
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
