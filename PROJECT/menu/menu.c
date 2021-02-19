#include <includes.h>
#include "menu.h"
#include "menudesc.h"
#include "data.h"
#include "keyboard.h"
#include "lcd.h"
#include "mode.h"
#include "app_serv.h"
#include "time.h"

OS_STK  MenuTaskStk[MENU_TASK_STK_SIZE];

#define STACKPANELSIZE  8
TMenuStack  MenuStack[STACKPANELSIZE];
CPU_INT08U  MenuStackPtr;
CPU_INT08U  MenuActiveLine, MenuFirstLine, MenuEditStatus;
TMenuPanel* MenuCurrentPanel;

CPU_INT08U EditPos, EditStart, EditLen, EditLine;
TVariant32 EditVal, EditMax, EditMin;
TDataDescStruct* EditDesc;
TRTC_Data EditTime;
// позиции для редактирования времени
const CPU_INT08U EditTimePos[12] = 
{
  0, 1,
  3, 4,
  6, 7,
  9,10,
  12,13,
  15,16
};
// позиции для редактирования даты
const CPU_INT08U EditDatePos[6] = 
{
  0, 1,
  3, 4,
  6, 7,
};
// позиции для редактирования ip-адреса
const CPU_INT08U EditIpPos[12] = 
{
  0, 1, 2,
  4, 5, 6,
  8, 9, 10,
  12, 13, 14
};
CPU_INT08U  EditBuf[96];
#define EDIT_TYPE_NUMBER   0
#define EDIT_TYPE_ITEMS    1

CPU_INT08U  refresh_menu = 0;
CPU_INT08U  str_start_sym = 0;
CPU_INT32U  str_last_time = 0;
extern TDataDescStruct const EventJournalIndexDesc;

static CPU_INT08U cursor_pos_x;
static CPU_INT08U cursor_pos_y;
static CPU_INT08U cursor_on_flag = 0;

void MenuCursorOn(CPU_INT08U x, CPU_INT08U y)
{
    cursor_pos_x = x;
    cursor_pos_y = y;
    cursor_on_flag = 1;
}

void MenuCursorOff(void)
{
    cursor_on_flag = 0;
}

void ShowMenuLine(TMenuLine* line_ptr, CPU_INT08U pos)
{
    CPU_INT08U strbuf[96];
    
    memset(strbuf, 0, sizeof(strbuf));
    
    if (line_ptr->LineType == MENU_LINE_STRING)
    {
        #if defined(CONFIG_LCD_1602A)
            strcpy((char*)strbuf, (char const*)line_ptr->Ptr);
        #else
        if (MenuCurrentPanel == WORK_MENU) 
            sprintf((char*)strbuf, "%s", (CPU_INT08U*)line_ptr->Ptr);
        else 
            sprintf((char*)strbuf, " %s", (CPU_INT08U*)line_ptr->Ptr);
        #endif
    }
    else if (line_ptr->LineType == MENU_LINE_SHOW_DESC)
    {
        #if !defined(CONFIG_LCD_1602A)
        strbuf[0] = ' ';
        #endif
        
        if ((MenuEditStatus) && (pos == EditLine))
        {
            // выводим редактируемую строку
            if (EditDesc->IsIndex)
            {
                GetDataNameStr((const TDataDescStruct*)EditDesc, &strbuf[strlen((char*)strbuf)]);
                strcat((char*)strbuf, "<");
                strcat((char*)strbuf, (char const*)EditBuf);
                strcat((char*)strbuf, ">");
            }
            else 
            {
                #if defined(CONFIG_LCD_1602A)      
                if ((EditDesc->Type == DATA_TYPE_TIME) || (EditDesc->Type == DATA_TYPE_IP_ADDR))
                {
                    strcat((char*)&strbuf[strlen((char*)strbuf)], (char const*)EditBuf);
                }
                else
                #endif
                if (EditDesc->Type == DATA_TYPE_CHAR_STRING)
                {
                    strcat((char*)&strbuf[strlen((char*)strbuf)], (char const*)EditBuf);
                }
                else
                {
                    GetDataNameStr((const TDataDescStruct*)EditDesc, &strbuf[strlen((char*)strbuf)]);
                    if (EditDesc->Name) strcat((char*)strbuf, "=");
                    strcat((char*)strbuf, (char const*)EditBuf);
                }
            }
        }
        else
        {
            if (line_ptr->Ptr != &EventJournalIndexDesc)
            {
                GetDataFullStr((const TDataDescStruct*)line_ptr->Ptr, &strbuf[strlen((char*)strbuf)], 0, DATA_FLAG_SYSTEM_INDEX);
            }
            else
            {
                GetDataNameStr((const TDataDescStruct*)line_ptr->Ptr, &strbuf[strlen((char*)strbuf)]);
                GetDataStr((const TDataDescStruct*)line_ptr->Ptr, &strbuf[strlen((char*)strbuf)], 0, DATA_FLAG_SYSTEM_INDEX);
            }
        }
    }
    else if (line_ptr->LineType == MENU_LINE_GOTO_MENU)
    {
        #if defined(CONFIG_LCD_1602A)
            sprintf((char*)strbuf, "%s", (CPU_INT08U*)line_ptr->Ptr);
        #else
            sprintf((char*)strbuf, " %s", (CPU_INT08U*)line_ptr->Ptr);
        #endif
    }
    
    if (strlen((char*)strbuf) > MENU_SYMB_NUMBER)
    {
        if (!MenuEditStatus)
        {
            if (OSTimeGet() - str_last_time > 1000)
            {
                str_last_time = OSTimeGet();
                str_start_sym++;
                str_start_sym %= (strlen((char*)strbuf) - MENU_SYMB_NUMBER + 1);
            }
            LCD_puts(&strbuf[str_start_sym], pos);
        }
        else
        {
            if (EditDesc->Type == DATA_TYPE_TIME)
            {
                if (EditStart+EditTimePos[EditPos] > MENU_SYMB_NUMBER - 1)
                {
                    str_start_sym = EditStart+EditTimePos[EditPos] - MENU_SYMB_NUMBER + 1;
                }
                else
                {
                    str_start_sym = 0;
                }
            }
            else
            {
                str_start_sym = 0;
            }
            LCD_puts(&strbuf[str_start_sym], pos);
        }
    }
    else
    {
        LCD_puts(strbuf, pos);
    }
}

void ClearMenuLine(CPU_INT08U pos)
{
  CPU_INT08U c = 0;
  LCD_puts(&c, pos);
}

void ClearDisplay(void)
{
  LCD_clear();
}


TMenuPanel* GetCurrentMenu(void)
{
  return MenuCurrentPanel;
}

// вывод текущего меню
void ShowCurrentMenu(void)
{
  // подсчитаем число строк в текущей панели  
  CPU_INT08U i, j;
  TMenuLine* pLine;
  //MenuCurrentPanel->LineNum

  #if !defined(CONFIG_LCD_1602A)
  LCD_cursor_off();
  #endif
  
  // если панель статическая, просто выводим первые 4 строки
  if (MenuCurrentPanel->PanelType == MENU_PANEL_STATIC)
    {
      j=0;
      if (!cursor_on_flag) LCD_cursor_off();

      for (i=0; i<MENU_LINES_NUMBER; i++)
        {
          pLine = (TMenuLine*)MenuCurrentPanel->LineArray[i].pMenuLine;
          if (pLine == NULL) break;
          ShowMenuLine(pLine, j++);
        }
      while (j<MENU_LINES_NUMBER)
        {
          ClearMenuLine(j++);
        }
      if ((((TMenuLine*)MenuCurrentPanel->LineArray[0].pMenuLine)->Flags) & MENU_INDEX_LINE)
        {
          LCD_putc_embed(SYMB_IND_MARK, MENU_SYMB_NUMBER-2, 0); LCD_putc_embed(SYMB_DESC_MARK, MENU_SYMB_NUMBER-1, 0);
        }
      
      if (cursor_on_flag) {LCD_goto(cursor_pos_x, cursor_pos_y); LCD_cursor_on();}
  
      return;
    }
  
  
  // в обычном меню первая линия всегда фиксированная
  pLine = (TMenuLine*)MenuCurrentPanel->LineArray[0].pMenuLine;
  if (pLine == NULL) {ClearDisplay(); return;}
  ShowMenuLine(pLine, 0);

  // выводим 3 строки с перемещением по меню
  // MenuFirstLine - верхняя строка для отображения
#if !defined(CONFIG_LCD_1602A)
  if (MenuActiveLine == 0) MenuFirstLine = 0;
  else
    {
      if (MenuCurrentPanel->LineNum < MENU_LINES_NUMBER) MenuFirstLine = MenuActiveLine-1;
      else
        {
          if (MenuActiveLine < MenuCurrentPanel->LineNum-2) MenuFirstLine = MenuActiveLine-1;
          else MenuFirstLine = MenuActiveLine-2;
        }
    }
#else
   MenuFirstLine = MenuActiveLine;
#endif
    
  i=MenuFirstLine+1;
  j=1;
  while((j<MENU_LINES_NUMBER) && (i<MenuCurrentPanel->LineNum))
    {
      TMenuLine* pline = (TMenuLine*)MenuCurrentPanel->LineArray[i].pMenuLine;
      if (pline == NULL) ClearMenuLine(j);
      else ShowMenuLine(pline, j);
      i++;
      j++;
    }
  
  while(j<MENU_LINES_NUMBER) ClearMenuLine(j++);
  
  // стрелочка текущего выбранного пункта
  #if !defined(CONFIG_LCD_1602A)
  CPU_INT08U linetype = (((TMenuLine*)MenuCurrentPanel->LineArray[MenuActiveLine+1].pMenuLine)->LineType);
  if (linetype == MENU_LINE_STRING) 
    {
      LCD_putc_embed(SYMB_POINT_MARK, 0, MenuActiveLine-MenuFirstLine+1);
    }
  else if (linetype == MENU_LINE_GOTO_MENU) 
    {
      LCD_putc_embed(SYMB_RIGHT_ARROW, 0, MenuActiveLine-MenuFirstLine+1);  
    }
  else if (linetype == MENU_LINE_SHOW_DESC) 
    {
      TMenuLine* mline = ((TMenuLine*)MenuCurrentPanel->LineArray[MenuActiveLine+1].pMenuLine);
      
      if (((const TDataDescStruct*)mline->Ptr)->Desc == DATA_DESC_EDIT)
        LCD_putc_embed(SYMB_DESC_MARK, 0, MenuActiveLine-MenuFirstLine+1);
      else 
        LCD_putc_embed(SYMB_POINT_MARK, 0, MenuActiveLine-MenuFirstLine+1);
    }
  #endif
  
  // знак индекса
  #if !defined(CONFIG_LCD_1602A)
  CPU_INT08U lineflags = (((TMenuLine*)MenuCurrentPanel->LineArray[0].pMenuLine)->Flags);
  if (lineflags & MENU_INDEX_LINE)
    {
      LCD_putc_embed(SYMB_IND_MARK, MENU_SYMB_NUMBER-2, 0); LCD_putc_embed(SYMB_DESC_MARK, MENU_SYMB_NUMBER-1, 0);
    }
  #endif
  
  // курсор (аппаратный)
  if ((MenuEditStatus) && (!EditDesc->IsIndex))
    {
      if (EditDesc->Type == DATA_TYPE_ULONG)
        {
          LCD_goto(EditStart+EditPos, MenuActiveLine-MenuFirstLine+1);
        }
      else if (EditDesc->Type == DATA_TYPE_RUB_CENT)
        {
          LCD_goto(EditStart+EditPos, MenuActiveLine-MenuFirstLine+1);
        }
      else if ((EditDesc->Type == DATA_TYPE_TIME) || (EditDesc->Type == DATA_TYPE_HOUR_MIN))
        {
          if (EditStart+EditTimePos[EditPos] >= MENU_SYMB_NUMBER - 1)
          {
            LCD_goto(MENU_SYMB_NUMBER - 1, MenuActiveLine-MenuFirstLine+1);
          }
          else
          {
            LCD_goto(EditStart+EditTimePos[EditPos], MenuActiveLine-MenuFirstLine+1);
          }
        }
      else if (EditDesc->Type == DATA_TYPE_DATE)
        {
          if (EditStart+EditDatePos[EditPos] >= MENU_SYMB_NUMBER - 1)
          {
            LCD_goto(MENU_SYMB_NUMBER - 1, MenuActiveLine-MenuFirstLine+1);
          }
          else
          {
            LCD_goto(EditStart+EditDatePos[EditPos], MenuActiveLine-MenuFirstLine+1);
          }
        }
      else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
      {
          LCD_goto(EditStart+EditIpPos[EditPos], MenuActiveLine-MenuFirstLine+1);
      }
      else if (EditDesc->Type == DATA_TYPE_CHAR_STRING)
      {
          LCD_goto(EditStart+EditPos, MenuActiveLine-MenuFirstLine+1);
      }
      LCD_cursor_on();
    }
  else 
    {
      LCD_cursor_off();
    }
}

void RefreshMenu(void)
{
  refresh_menu = 1;
}

// поиск первой активной строки в панели меню
CPU_INT08U GetFirstActiveLine(TMenuPanel *menu)
{
  CPU_INT08U line = 0;
  CPU_INT08U i = 0;
  while (i++ < menu->LineNum)
    {
      TMenuLine* mline = (TMenuLine*)menu->LineArray[i].pMenuLine;
      if ((mline->LineType == MENU_LINE_GOTO_MENU) || (mline->LineType == MENU_LINE_SHOW_DESC))
        {
          line = i;
          break;
        }
    }
  return line-1;
}

// поиск следующей  активной строки в панели меню
CPU_INT08U GetNextActiveLine(TMenuPanel *menu, CPU_INT08U recent)
{
  CPU_INT08U line = recent+1;
  CPU_INT08U i = recent+1;
  while (i++ < menu->LineNum)
    {
      TMenuLine* mline = (TMenuLine*)menu->LineArray[i].pMenuLine;
      if ((mline->LineType == MENU_LINE_GOTO_MENU) || (mline->LineType == MENU_LINE_SHOW_DESC)
          #if defined(CONFIG_LCD_1602A)
          || (mline->LineType == MENU_LINE_STRING)
          #endif
          )
        {
          line = i;
          break;
        }
    }
  return line-1;
}

// поиск предыдущей активной строки в панели меню
CPU_INT08U GetPrevActiveLine(TMenuPanel *menu, CPU_INT08U recent)
{
  CPU_INT08U line = recent+1;
  CPU_INT08U i = recent+1;
  if (recent == 0) return 0;
  while (i--)
    {
      TMenuLine* mline = (TMenuLine*)menu->LineArray[i].pMenuLine;
      if ((mline->LineType == MENU_LINE_GOTO_MENU) || (mline->LineType == MENU_LINE_SHOW_DESC))
        {
          line = i;
          break;
        }
    }
  return line-1;
}

void GoToMenu(const TMenuPanel* Menu)
{
  if (MenuStackPtr >= STACKPANELSIZE) return;
  MenuStack[MenuStackPtr].PrevMenu = MenuCurrentPanel;
  MenuStack[MenuStackPtr].PrevActiveLine = MenuActiveLine;
  MenuStackPtr++;
  
  MenuCurrentPanel = (TMenuPanel*)Menu;
  MenuActiveLine = GetFirstActiveLine(MenuCurrentPanel);
  if (Menu->InitFunc) Menu->InitFunc();
  refresh_menu = 1;
}

void SetMenu(const TMenuPanel* Menu)
{
  MenuCurrentPanel = (TMenuPanel*)Menu;
  MenuActiveLine = GetFirstActiveLine(MenuCurrentPanel);
  if (Menu->InitFunc) Menu->InitFunc();
  refresh_menu = 1;
}

void GoToPreviousMenu(void)
{
  if (!MenuStackPtr) return;
  --MenuStackPtr;
  MenuCurrentPanel = (TMenuPanel*)MenuStack[MenuStackPtr].PrevMenu;
  MenuActiveLine = MenuStack[MenuStackPtr].PrevActiveLine;
  if (MenuCurrentPanel->InitFunc) MenuCurrentPanel->InitFunc();
  refresh_menu = 1;
}

void GoToNextMenu(void)
{
  if ((MenuCurrentPanel->LineArray[MenuActiveLine+1].pMenuLine)->GoToPtr) 
    GoToMenu((MenuCurrentPanel->LineArray[MenuActiveLine+1].pMenuLine)->GoToPtr);
}

void MenuSprintf(CPU_INT08U* str, CPU_INT08U len, CPU_INT32U Val)
{
  if (EditDesc->Type == DATA_TYPE_ULONG)
    {
      CPU_INT08U format[6];
      format[0]='%';
      format[1]='0';
      format[2]='0'+len/10;
      format[3]='0'+len%10;
      format[4]='u';
      format[5]=0;
      sprintf((char*)str, (char const*)format, Val);
    }
  else if (EditDesc->Type == DATA_TYPE_RUB_CENT)
    { 
      CPU_INT08U format[10];
      format[0]='%';
      format[1]='0';
      format[2]='0'+(strlen((char*)str) - 3);
      format[3]='u';
      format[4]=',';
      format[5]='%';
      format[6]='0';
      format[7]='2';
      format[8]='d';
      format[9]=0;
      sprintf((char*)str, (char const*)format, Val / 100, Val % 100);
    }          
  else if ((EditDesc->Type == DATA_TYPE_TIME) || (EditDesc->Type == DATA_TYPE_HOUR_MIN) || (EditDesc->Type == DATA_TYPE_DATE) || (EditDesc->Type == DATA_TYPE_TIME_SEC_H_MM) || (EditDesc->Type == DATA_TYPE_TIME_SEC_M))
    {
      GetDataStr(EditDesc, str, 0, DATA_FLAG_SYSTEM_INDEX);
    }
  else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
    {
      sprintf((char*)str, "%03d.%03d.%03d.%03d", (int)((Val >> 24) & 0xFF), (int)((Val >> 16) & 0xFF), (int)((Val >> 8) & 0xFF), (int)((Val >> 0) & 0xFF));
    }
  else if (EditDesc->Type == DATA_TYPE_CHAR_STRING)
  {
    GetDataStr(EditDesc, str, 0, DATA_FLAG_SYSTEM_INDEX);
    while (strlen((char*)str) < len)
    {
        strcat((char*)str, " ");
    }
  }
}

// начало редактирования
void EnterEdit(void)
{
  TRangeValueULONG* RVal;
  // позиция редактирования, 0 - самое старшее десятичное число
  EditPos = 0;
  // указатель на редактируемый дескриптор
  EditDesc = (TDataDescStruct*)(MenuCurrentPanel->LineArray[MenuActiveLine+1].pMenuLine)->Ptr;
  // номер строки с редактируемым дескриптором
  EditLine = MenuActiveLine+1-MenuFirstLine;
  // редактируемое число
  if (EditDesc->Type != DATA_TYPE_CHAR_STRING)
  {
      GetData(EditDesc, &EditVal, 0, DATA_FLAG_SYSTEM_INDEX);
      // границы 
      RVal = EditDesc->RangeValue;
      memcpy(&EditMin, &RVal->Min, sizeof(CPU_INT32U));
      memcpy(&EditMax, &RVal->Max, sizeof(CPU_INT32U));
  }
  
  memset(EditBuf, 0, sizeof(EditBuf));
     
  if (EditDesc->IsIndex)
    {
      GetDataStr((const TDataDescStruct*)EditDesc, EditBuf, 0, DATA_FLAG_SYSTEM_INDEX);
    }
  else 
    {
      #if defined(CONFIG_LCD_1602A)      
      if ((EditDesc->Type != DATA_TYPE_TIME) && (EditDesc->Type != DATA_TYPE_IP_ADDR))
      {
      #endif
        GetDataNameStr((const TDataDescStruct*)EditDesc, EditBuf);
        if (EditDesc->Name) strcat((char*)EditBuf, "=");
      #if defined(CONFIG_LCD_1602A)      
      }
      #endif
      #if defined(CONFIG_LCD_1602A)      
      EditStart = strlen((char const*)EditBuf);
      #else 
      EditStart = strlen((char const*)EditBuf)+1;
      #endif
      if (EditDesc->Type == DATA_TYPE_ULONG)
        {
          sprintf((char*)EditBuf, "%u", RVal->Max);
          EditLen = strlen((char const*)EditBuf);
        }
      else if (EditDesc->Type == DATA_TYPE_RUB_CENT)
        {
          if (RVal->Max / 100 < 10)
          {
            sprintf((char*)EditBuf, "%d,%02d", RVal->Max / 100, RVal->Max % 100);
          }
          else if ((RVal->Max / 100 >= 10) && ((RVal->Max / 100 < 100)))
          {
            sprintf((char*)EditBuf, "%02d,%02d", RVal->Max / 100, RVal->Max % 100);
          }
          else if ((RVal->Max / 100 >= 100) && ((RVal->Max / 100 < 1000)))
          {
            sprintf((char*)EditBuf, "%03d,%02d", RVal->Max / 100, RVal->Max % 100);
          }
          EditLen = strlen((char const*)EditBuf);
        }
      else if (EditDesc->Type == DATA_TYPE_TIME)
        {
          EditLen = 12;
        }
      else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
        {
          EditLen = 12;
        }
      else if (EditDesc->Type == DATA_TYPE_HOUR_MIN)
        {
          EditLen = 4;
        }
      else if (EditDesc->Type == DATA_TYPE_DATE)
        {
          EditLen = 6;
        }
      else if (EditDesc->Type == DATA_TYPE_CHAR_STRING)
        {
          EditStart=1;
          EditLen = EditDesc->ArraySize;
        }
      else
        {
          EditLen = 0;
        }
      MenuSprintf(EditBuf, EditLen, EditVal.Val32U);
    }
  
  MenuEditStatus = 1; 
}

// выход из редактирования без сохранения
void EscEdit(void)
{
  MenuEditStatus = 0;
}

// сохранение параметра
void SaveEdit(void)
{
  if (EditDesc->Type == DATA_TYPE_ULONG)
    {
      if (!EditDesc->IsIndex)
      {
          sscanf((char const*)EditBuf, "%d", &EditVal.Val32U);
      }
      SetData(EditDesc, &EditVal, 0, DATA_FLAG_SYSTEM_INDEX);
    }
  else if (EditDesc->Type == DATA_TYPE_RUB_CENT)
    {
      SetDataFromStr(EditDesc, (char*)EditBuf, 0, DATA_FLAG_SYSTEM_INDEX);
    }
  else if (EditDesc->Type == DATA_TYPE_TIME)
    {
      TRTC_Data rtc;
      ScanRTCDateTimeStringRus((char*)EditBuf, &rtc);
      if (RTCCheckTime(&rtc) == 0)
        { // ок
          CPU_INT32U time;
          time = GetSec(&rtc);
          SetData(EditDesc, &time, 0, DATA_FLAG_SYSTEM_INDEX);
        }
    }
  else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
    {
        CPU_INT32U ip[4];
        if (sscanf((char*)EditBuf, "%03d.%03d.%03d.%03d", &ip[3], &ip[2], &ip[1], &ip[0]) == 4)
        {
            ip[0] = ip[0] + (ip[1] << 8) + (ip[2] << 16) + (ip[3] << 24);
            SetData(EditDesc, &ip[0], 0, DATA_FLAG_SYSTEM_INDEX);
        }
    }
  else if (EditDesc->Type == DATA_TYPE_HOUR_MIN)
    {
      int hour, min, hour_min;
      sscanf((char*)EditBuf, "%02d:%02d", &hour, &min);
      hour_min = hour * 60 + min;
      SetData(EditDesc, &hour_min, 0, DATA_FLAG_SYSTEM_INDEX);
    }
  else if (EditDesc->Type == DATA_TYPE_DATE)
    {
      TRTC_Data rtc;
      ScanRTCDateStringRus((char*)EditBuf, &rtc);
      if (RTCCheckTime(&rtc) == 0)
        { // ок
          CPU_INT32U time;
          time = GetSec(&rtc);
          SetData(EditDesc, &time, 0, DATA_FLAG_SYSTEM_INDEX);
        }
    }
  else if (EditDesc->Type == DATA_TYPE_CHAR_STRING)
  {
    for (int i = 0; i < EditDesc->ArraySize; i++)
    {
        if (EditBuf[i] == ' ')
        {
            EditBuf[i] = 0;
            break;
        }
    }
    SetData(EditDesc, EditBuf, 0, DATA_FLAG_SYSTEM_INDEX);
  }
  MenuEditStatus = 0;
}

#if defined(BOARD_SOLARIUM_WEB)
void ProcessDigitKey(int key)
{
    if (EditDesc->Type == DATA_TYPE_ULONG)
    {
      EditBuf[EditPos] = '0' + key - KEY_DIGIT0;
    }
    else if (EditDesc->Type == DATA_TYPE_RUB_CENT)
    {
      EditBuf[EditPos] = '0' + key - KEY_DIGIT0;
    }
    else if (EditDesc->Type == DATA_TYPE_TIME)
    {
      EditBuf[EditTimePos[EditPos]] = '0' + key - KEY_DIGIT0;
    }
    else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
    {
      EditBuf[EditIpPos[EditPos]] = '0' + key - KEY_DIGIT0;
    }
    else if (EditDesc->Type == DATA_TYPE_HOUR_MIN)
    {
      EditBuf[EditTimePos[EditPos]] = '0' + key - KEY_DIGIT0;;
    }
    else if (EditDesc->Type == DATA_TYPE_DATE)
    {
      EditBuf[EditDatePos[EditPos]] = '0' + key - KEY_DIGIT0;;
    }
    if (EditPos < EditLen-1) EditPos++;
    else
    {
        if (EditDesc->Type != DATA_TYPE_TIME) EditPos = 0;
    }
}

#endif

void  MenuTask(void *p_arg)
{
  int pause = 0;

  str_last_time = OSTimeGet();
  SetMenu(START_MENU);
  
  while (1)
    {
      int key=0; 
      if (GetKbrdEvent(&key) || (++pause >= 1000) || ((pause >= 500) && (MenuEditStatus)) || (refresh_menu != 0))
        {
          if (refresh_menu) {refresh_menu = 0; ShowCurrentMenu();}
          // отработка клавиатуры
          if (!MenuEditStatus)
          { // отображение меню
            pause = 0;
        
            #if defined(BOARD_SOLARIUM_WEB)
            key_repeat1:
            #endif
            if (key)
              {
                switch (key){
                  #if defined(BOARD_SOLARIUM_WEB)
                  case KEY_DIGIT0:
                      break;
                  case KEY_DIGIT1:
                      break;
                  case KEY_DIGIT2:
                      key = KEY_UP;
                      goto key_repeat1;
                  case KEY_DIGIT3:
                      break;
                  case KEY_DIGIT4:
                      key = KEY_LEFT;
                      goto key_repeat1;
                  case KEY_DIGIT5:
                      break;
                  case KEY_DIGIT6:
                      key = KEY_RIGHT;
                      goto key_repeat1;
                  case KEY_DIGIT7:
                      break;
                  case KEY_DIGIT8:
                      key = KEY_DOWN;
                      goto key_repeat1;
                  case KEY_DIGIT9:
                      break;
                  case KEY_STAR:
                      key = KEY_CANSEL;
                      goto key_repeat1;
                  case KEY_SHARP:
                      key = KEY_START;
                      goto key_repeat1;
                  #endif
                      
                  case KEY_UP:
                    {
                        CPU_INT08U active_line = GetPrevActiveLine(MenuCurrentPanel, MenuActiveLine);
                        if (MenuActiveLine != active_line)
                        {
                            str_start_sym = 0;
                            str_last_time = OSTimeGet();
                        }
                        MenuActiveLine = active_line;
                        ShowCurrentMenu();
                    }
                    break;
                    
                  case KEY_DOWN:
                    {
                        CPU_INT08U active_line = GetNextActiveLine(MenuCurrentPanel, MenuActiveLine);
                        if (MenuActiveLine != active_line)
                        {
                            str_start_sym = 0;
                            str_last_time = OSTimeGet();
                        }
                        MenuActiveLine = active_line;
                        ShowCurrentMenu();
                    }
                    break;
                    
                  case KEY_LEFT:
                    {
                      TMenuLine* pLine = (TMenuLine*)MenuCurrentPanel->LineArray[0].pMenuLine;
                      if (pLine->Flags & MENU_INDEX_LINE)
                      {// сверху индексный параметр - попробуем его переключить
                        TDataDescStruct* desc = (TDataDescStruct*)pLine->Ptr;
                        if (desc->Type == DATA_TYPE_ULONG)
                          {
                            CPU_INT32U i, min, max;
                            GetData(desc, &i, 0, DATA_FLAG_SYSTEM_INDEX);
                            GetDataMin(desc, &min);
                            GetDataMax(desc, &max);
                            i--;
                            if ((i < min) || (i > max)) i = max;
                            SetData(desc, &i, 0, DATA_FLAG_SYSTEM_INDEX);
                          }
                        str_start_sym = 0;
                        str_last_time = OSTimeGet();
                        ShowCurrentMenu();
                      }
                    }
                    break;
                    
                  case KEY_RIGHT:
                    {
                      TMenuLine* pLine = (TMenuLine*)MenuCurrentPanel->LineArray[0].pMenuLine;
                      if (pLine->Flags & MENU_INDEX_LINE)
                      {// сверху индексный параметр - попробуем его переключить
                        TDataDescStruct* desc = (TDataDescStruct*)pLine->Ptr;
                        if (desc->Type == DATA_TYPE_ULONG)
                          {
                            CPU_INT32U i, min, max;
                            GetData(desc, &i, 0, DATA_FLAG_SYSTEM_INDEX);
                            GetDataMin(desc, &min);
                            GetDataMax(desc, &max);
                            i++;
                            if ((i < min) || (i > max)) i = min;
                            SetData(desc, &i, 0, DATA_FLAG_SYSTEM_INDEX);
                          }
                        str_start_sym = 0;
                        str_last_time = OSTimeGet();
                        ShowCurrentMenu();
                      }
                    }
                    break;
                    
                  case KEY_STOP:
                    // пробуем выйти в предыдущее меню
                    str_start_sym = 0;
                    str_last_time = OSTimeGet();
                    GoToPreviousMenu();
                    break;
                    
                  case KEY_START:
                    {
                    if (MenuCurrentPanel->PanelType == MENU_PANEL_STATIC) {ShowCurrentMenu();break;}
                    TMenuLine* pLine = (TMenuLine*)MenuCurrentPanel->LineArray[MenuActiveLine+1].pMenuLine;
                    if (pLine->LineType == MENU_LINE_SHOW_DESC)
                      {// входим в редактирование, если можно
                        TDataDescStruct* desc = (TDataDescStruct*)pLine->Ptr;
                        str_start_sym = 0;
                        str_last_time = OSTimeGet();
                        if (desc->Desc == DATA_DESC_EDIT) {EnterEdit(); pause=1000;}
                      }
                    else if (pLine->LineType == MENU_LINE_GOTO_MENU)
                      {// пробуем перейти в следующее меню
                        str_start_sym = 0;
                        str_last_time = OSTimeGet();
                        GoToNextMenu();
                      }
                    }
                    break;
              }//switch (key)
            }//if (key)
          else 
            {
              ShowCurrentMenu();
            }
        }
        else//if (!MenuEditStatus)
        { // редактирование параметра
          if (key)
            {
              #if defined(BOARD_SOLARIUM_WEB)
              key_repeat2:
              #endif
              switch (key){
                #if defined(BOARD_SOLARIUM_WEB)
                case KEY_DIGIT8:
                  if (EditDesc->IsIndex)
                  {
                      key = KEY_DOWN;
                      goto key_repeat2;
                  }
                  ProcessDigitKey(key);
                  break;
                case KEY_DIGIT2:
                  if (EditDesc->IsIndex)
                  {
                      key = KEY_UP;
                      goto key_repeat2;
                  }
                  ProcessDigitKey(key);
                  break;
                case KEY_DIGIT0:
                case KEY_DIGIT1:
                case KEY_DIGIT3:
                case KEY_DIGIT4:
                case KEY_DIGIT5:
                case KEY_DIGIT6:
                case KEY_DIGIT7:
                case KEY_DIGIT9:
                  ProcessDigitKey(key);
                  break;
                case KEY_STAR:
                  key = KEY_CANSEL;
                  goto key_repeat2;
                case KEY_SHARP:
                  key = KEY_START;
                  goto key_repeat2;
                #endif
                  
                case KEY_UP:
                  if (EditDesc->IsIndex)
                    {
                      CPU_INT32U min, max;
                      GetDataMin(EditDesc, &min);
                      GetDataMax(EditDesc, &max);
                      EditVal.Val32U++;
                      if ((EditVal.Val32U < min) || (EditVal.Val32U > max)) EditVal.Val32U = min;
                      GetDataItem(EditDesc, EditBuf, EditVal.Val32U);
                    }
                  else
                    {
                      if (EditDesc->Type == DATA_TYPE_ULONG)
                        {
                          EditBuf[EditPos]++;
                          if (EditBuf[EditPos] > '9') EditBuf[EditPos] = '0';
                        }
                      else if (EditDesc->Type == DATA_TYPE_RUB_CENT)
                        {
                          EditBuf[EditPos]++;
                          if (EditBuf[EditPos] > '9') EditBuf[EditPos] = '0';
                        }
                      else if (EditDesc->Type == DATA_TYPE_TIME)
                        {
                          EditBuf[EditTimePos[EditPos]]++;
                          if (EditBuf[EditTimePos[EditPos]] > '9') EditBuf[EditTimePos[EditPos]] = '0';
                        }
                      else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
                        {
                          EditBuf[EditIpPos[EditPos]]++;
                          if (EditBuf[EditIpPos[EditPos]] > '9') EditBuf[EditIpPos[EditPos]] = '0';
                        }
                      else if (EditDesc->Type == DATA_TYPE_HOUR_MIN)
                        {
                          EditBuf[EditTimePos[EditPos]]++;
                          if (EditBuf[EditTimePos[EditPos]] > '9') EditBuf[EditTimePos[EditPos]] = '0';
                        }
                      else if (EditDesc->Type == DATA_TYPE_DATE)
                        {
                          EditBuf[EditDatePos[EditPos]]++;
                          if (EditBuf[EditDatePos[EditPos]] > '9') EditBuf[EditDatePos[EditPos]] = '0';
                        }
                      else if (EditDesc->Type == DATA_TYPE_CHAR_STRING)
                        {
                          EditBuf[EditPos]++;
                          if (EditBuf[EditPos] > 'z') EditBuf[EditPos] = ' ';
                        }
                    }
                  break;
                case KEY_DOWN:
                  if (EditDesc->IsIndex)
                    {
                      CPU_INT32U min, max;
                      GetDataMin(EditDesc, &min);
                      GetDataMax(EditDesc, &max);
                      EditVal.Val32U--;
                      if ((EditVal.Val32U < min) || (EditVal.Val32U > max)) EditVal.Val32U = max;
                      GetDataItem(EditDesc, EditBuf, EditVal.Val32U);
                    }
                  else
                    {
                      if (EditDesc->Type == DATA_TYPE_ULONG)
                        {
                          EditBuf[EditPos]--;
                          if (EditBuf[EditPos] < '0') EditBuf[EditPos] = '9';
                        }
                      else if (EditDesc->Type == DATA_TYPE_RUB_CENT)
                        {
                          EditBuf[EditPos]--;
                          if (EditBuf[EditPos] < '0') EditBuf[EditPos] = '9';
                        }
                      else if (EditDesc->Type == DATA_TYPE_TIME)
                        {
                          EditBuf[EditTimePos[EditPos]]--;
                          if (EditBuf[EditTimePos[EditPos]] < '0') EditBuf[EditTimePos[EditPos]] = '9';
                        }
                      else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
                        {
                          EditBuf[EditIpPos[EditPos]]--;
                          if (EditBuf[EditIpPos[EditPos]] < '0') EditBuf[EditIpPos[EditPos]] = '9';
                        }
                      else if (EditDesc->Type == DATA_TYPE_HOUR_MIN)
                        {
                          EditBuf[EditTimePos[EditPos]]--;
                          if (EditBuf[EditTimePos[EditPos]] < '0') EditBuf[EditTimePos[EditPos]] = '9';
                        }
                      else if (EditDesc->Type == DATA_TYPE_DATE)
                        {
                          EditBuf[EditDatePos[EditPos]]--;
                          if (EditBuf[EditDatePos[EditPos]] < '0') EditBuf[EditDatePos[EditPos]] = '9';
                        }
                      else if (EditDesc->Type == DATA_TYPE_CHAR_STRING)
                        {
                          EditBuf[EditPos]--;
                          if (EditBuf[EditPos] < ' ') EditBuf[EditPos] = 'z';
                        }

                    }
                  break;
                case KEY_LEFT:
                  if (EditDesc->IsIndex)
                    {
                      CPU_INT32U min, max;
                      GetDataMin(EditDesc, &min);
                      GetDataMax(EditDesc, &max);
                      EditVal.Val32U--;
                      if ((EditVal.Val32U < min) || (EditVal.Val32U > max)) EditVal.Val32U = max;
                      GetDataItem(EditDesc, EditBuf, EditVal.Val32U);
                    }
                  else
                    {
                      if (EditPos) 
                      {
                          EditPos--;
                          if (EditDesc->Type == DATA_TYPE_RUB_CENT)
                          {
                            if (EditBuf[EditPos] == ',') EditPos--;
                          }
                      }
                    }
                  break;
                case KEY_RIGHT:
                  if (EditDesc->IsIndex)
                    {
                      CPU_INT32U min, max;
                      GetDataMin(EditDesc, &min);
                      GetDataMax(EditDesc, &max);
                      EditVal.Val32U++;
                      if ((EditVal.Val32U < min) || (EditVal.Val32U > max)) EditVal.Val32U = min;
                      GetDataItem(EditDesc, EditBuf, EditVal.Val32U);
                    }
                  else
                    {
                      if (EditPos < EditLen-1)
                      {
                          EditPos++;
                          if (EditDesc->Type == DATA_TYPE_RUB_CENT)
                          {
                            if (EditBuf[EditPos] == ',') EditPos++;
                          }
                      }
                    }
                  break;
                case KEY_STOP:
                  EscEdit();
                  break;
                case KEY_START:
                  SaveEdit();
                  break;
                }//switch (key)
                
              ShowCurrentMenu();
            }//if (key)
          else
            {
              pause = 0;  
              ShowCurrentMenu();
            }
        }//if (!MenuEditStatus)
      
      }
    }
}



void InitMenu(void)
{
  INT8U err;

  MenuStackPtr = 0;
  MenuActiveLine = 0;
  MenuFirstLine = 0;
  MenuEditStatus = 0;
  MenuCurrentPanel = NULL;
    
  memset(&MenuStack, 0, sizeof(TMenuStack)*STACKPANELSIZE);
  
  OSTaskCreate(MenuTask, (void *)0, (OS_STK *)&MenuTaskStk[MENU_TASK_STK_SIZE-1], MENU_TASK_PRIO);
  OSTaskNameSet(MENU_TASK_PRIO, "Menu Task", &err);
}


void ReInitMenu(void)
{
  OSTaskDel(MENU_TASK_PRIO);
  ClearDisplay();
  OSTimeDly(100);
  InitMenu();
  OSTimeDly(100);
  if (GetMode() == MODE_WORK) SetMenu(WORK_MENU);
  else SetMenu(SERVICE_MENU);
}

