#ifndef _DATA_H_
#define _DATA_H_

/*!
  Описание структур и способа доступа к данным
 
  пока все параметры - 32 - битные float или signed long

*/

#include "cpu.h"

#pragma pack(push, 4)
typedef union{
  CPU_INT32U Val32U;
  CPU_INT32S Val32S;
  CPU_FP32   ValFloat;
}TVariant32;

// структуры границ
typedef struct{
  CPU_INT32U Min;
  CPU_INT32U Max;
}TRangeValueULONG;

typedef struct{
  CPU_INT32S Min;
  CPU_INT32S Max;
}TRangeValueSLONG;

typedef struct{
  CPU_FP32 Min;
  CPU_FP32 Max;
}TRangeValueFLOAT;
#pragma pack(pop)

// универсальный дескриптор данных
typedef struct{
  
  // тип дескриптора
  CPU_INT08U Desc; 
    #define DATA_DESC_EDIT  0 // редактируемый
    #define DATA_DESC_VIEW  1 // для просмотра 
  
  // тип параметра
  CPU_INT08U Type; 
    //#define DATA_TYPE_UCHAR  0
    //#define DATA_TYPE_SCHAR  1  
    #define DATA_TYPE_ULONG       2
    #define DATA_TYPE_SLONG       3  
    #define DATA_TYPE_FLOAT       4
    #define DATA_TYPE_TIME        5
    #define DATA_TYPE_TIME_COUNT  6
    #define DATA_TYPE_HOUR_MIN    7
    #define DATA_TYPE_IP_ADDR     8
  
  // расположение параметра
  CPU_INT08U Location; 
    #define DATA_LOC_RAM    0
    #define DATA_LOC_FRAM   1

  // признак массива
  CPU_INT08U IsArray; 
    #define DATA_NO_ARRAY    0
    #define DATA_IS_ARRAY    1

  // размер массива
  CPU_INT32U ArraySize; 

  // указатель на десриптор индекса массива
  const void* ArrayIndex; //TDataDescStruct*
  
  // указатель на переменную или адрес FRAM
  void* Data;
  
  // указатель на границы параметра
  void* RangeValue;
    
  // функция по изменению
  void (*OnchangeFunc)(void);
  
  // смещение между элементами в массиве
  CPU_INT32U ArrayOffset; 
  
  // указатель на строку названия параметра
  const CPU_INT08U* Name;

  // признак индексного параметра (список строк)
  CPU_INT08U IsIndex; 
    #define DATA_NO_INDEX    0
    #define DATA_IS_INDEX    1

  // указатель на список строк для индексного параметра
  const CPU_INT08U** Items;

  // разрешить инициализацию значением по умолчанию при старте системы
  CPU_INT08U EnableInit; 
    #define DATA_INIT_DISABLE   0
    #define DATA_INIT_ENABLE    1

  // значение по умолчанию
  TVariant32 DefaultValue;
    
}TDataDescStruct;


// структура для массива всех параметров
typedef struct{
  const TDataDescStruct* ptr;
  const char* name;
}TDataDescArrayStruct;

// флаги
#define DATA_FLAG_SYSTEM_INDEX  0
#define DATA_FLAG_DIRECT_INDEX  1

// возвращаемые значения 
#define DATA_OK    0
#define DATA_ERR  -1

// необходимые функции 

// получение данных
extern int GetData(const TDataDescStruct* desc, void* buf, CPU_INT32U index, CPU_INT08U flags);
// запись данных
extern int SetData(const TDataDescStruct* desc, void* buf, CPU_INT32U index, CPU_INT08U flags);
// получение строки с отформатированным значением
extern int GetDataStr(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U index, CPU_INT08U flags);
// получение полной строки с названием и значением
extern int GetDataFullStr(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U index, CPU_INT08U flags);
// получение строки с именем
extern int GetDataNameStr(const TDataDescStruct* desc, CPU_INT08U* buf);
// инициализация по умолчанию
extern int InitDataByDefault(const TDataDescStruct* desc, CPU_INT32U index);
// проверка всех дестрипторов
extern int CheckAllData(void);
// инициализация 
extern int InitData(const TDataDescStruct* desc);
extern int GetDataItem(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U itemindex);
extern int InitDescByDefault(const TDataDescStruct* desc);
extern int GetDataMin(const TDataDescStruct* desc, void* buf);
extern int GetDataMax(const TDataDescStruct* desc, void* buf);

extern void FindDescByName(char* name, TDataDescStruct const** desc, CPU_INT32U *index);

#endif //#ifndef _DATA_H_
