#ifndef _MENU_H_
#define _MENU_H_

#include "cpu.h"

#define MENU_LINES_NUMBER   4
#define MENU_SYMB_NUMBER   20


// служебные символы
#define SYMB_GO_UP    0x87
#define SYMB_GO_DOWN  0x86

#define SYMB_RIGHT_ARROW  0x13
#define SYMB_DESC_MARK    0x84
#define SYMB_IND_MARK     0x85
#define SYMB_POINT_MARK   0xDF



// структура описания пункта меню
typedef struct{
  
  // тип пункта меню
  CPU_INT08U LineType;
    #define MENU_LINE_STRING      0 // просто строка
    #define MENU_LINE_GOTO_MENU   1 // переход на другое меню
    #define MENU_LINE_SHOW_DESC   2 // отображение параметра
    #define MENU_LINE_INDEX       3 // индекс массива
  
  // доп. флаги
  CPU_INT08U Flags;
    #define MENU_FIXED_LINE       0x01 // фиксированная строка
    #define MENU_INDEX_LINE       0x02 // строка с индексом массива
  
  // указатель на текстовую строку или дескриптор
  void* Ptr;
  
  // панель для перехода
  void* GoToPtr;

}TMenuLine;


// для массива строк
typedef struct{
  const TMenuLine* pMenuLine;
}TMenuLineArray;


// структура описания панели меню
typedef struct{

  // указатель на массив строк
  const TMenuLineArray* LineArray;
  
  // указатель на функцию, выполняемую перед входом в панель
  void (*InitFunc)(void);
  
  // число строк в данном меню
  CPU_INT08U LineNum;

  // тип панели
  CPU_INT08U PanelType;
    #define MENU_PANEL_STANDARD     0  // обычная панель, с перемещением по меню
    #define MENU_PANEL_STATIC       1  // статическая панель

}TMenuPanel;


// элемент стека возвратов меню
typedef struct{

  // указатель на предыдущее меню
  const TMenuPanel* PrevMenu;
  // активная линия в предыдущем меню
  CPU_INT08U PrevActiveLine;

}TMenuStack;


///////////////////////////////////
// функции для работы с меню
///////////////////////////////////
// инициализация меню
extern void InitMenu(void);
// переход на заданную панель меню
extern void GoToMenu(const TMenuPanel* Menu);
extern void SetMenu(const TMenuPanel* Menu);
// возврат в предыдущее меню
extern void GoToPreviousMenu(void);
// перезапуск меню
extern void ReInitMenu(void);

extern void MenuSprintf(CPU_INT08U* str, CPU_INT08U len, CPU_INT32U Val);
extern TMenuPanel* GetCurrentMenu(void);
extern void RefreshMenu(void);

#endif //#ifndef _MENU_H_
