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
// позиции для редактирования ip-адреса
const CPU_INT08U EditIpPos[12] = 
{
  0, 1, 2,
  4, 5, 6,
  8, 9, 10,
  12, 13, 14
};
CPU_INT08U  EditBuf[32];
#define EDIT_TYPE_NUMBER   0
#define EDIT_TYPE_ITEMS    1

CPU_INT08U  refresh_menu = 0;

void ShowMenuLine(TMenuLine* line_ptr, CPU_INT08U pos)
{
  CPU_INT08U strbuf[32];
    
  if (line_ptr->LineType == MENU_LINE_STRING)
    {
      sprintf((char*)strbuf, " %s", (CPU_INT08U*)line_ptr->Ptr);
      LCD_puts(strbuf, pos);
    }
  else if (line_ptr->LineType == MENU_LINE_SHOW_DESC)
    {
      strbuf[0] = ' ';
      if ((MenuEditStatus) && (pos == EditLine))
        {
           // выводим редактируемую строку
           if (EditDesc->IsIndex)
             {
               GetDataNameStr((const TDataDescStruct*)EditDesc, &strbuf[1]);
               strcat((char*)strbuf, "<");
               strcat((char*)strbuf, (char const*)EditBuf);
               strcat((char*)strbuf, ">");
               LCD_puts(strbuf, pos);
             }
           else 
             {
               GetDataNameStr((const TDataDescStruct*)EditDesc, &strbuf[1]);
               if (EditDesc->Name) strcat((char*)strbuf, "=");
               strcat((char*)strbuf, (char const*)EditBuf);
               LCD_puts(strbuf, pos);
             }
        }
      else
        {
          GetDataFullStr((const TDataDescStruct*)line_ptr->Ptr, &strbuf[1], 0, DATA_FLAG_SYSTEM_INDEX);
          LCD_puts(strbuf, pos);
        }
    }
  else if (line_ptr->LineType == MENU_LINE_GOTO_MENU)
    {
      sprintf((char*)strbuf, " %s", (CPU_INT08U*)line_ptr->Ptr);
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
    
  // если панель статическая, просто выводим первые 4 строки
  if (MenuCurrentPanel->PanelType == MENU_PANEL_STATIC)
    {
      j=0;
      LCD_cursor_off();
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
      return;
    }
  
  
  // в обычном меню первая линия всегда фиксированная
  pLine = (TMenuLine*)MenuCurrentPanel->LineArray[0].pMenuLine;
  if (pLine == NULL) {ClearDisplay(); return;}
  ShowMenuLine(pLine, 0);

  // выводим 3 строки с перемещением по меню
  // MenuFirstLine - верхняя строка для отображения
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
    
  // знак индекса
  CPU_INT08U lineflags = (((TMenuLine*)MenuCurrentPanel->LineArray[0].pMenuLine)->Flags);
  if (lineflags & MENU_INDEX_LINE)
    {
      LCD_putc_embed(SYMB_IND_MARK, MENU_SYMB_NUMBER-2, 0); LCD_putc_embed(SYMB_DESC_MARK, MENU_SYMB_NUMBER-1, 0);
    }

  // курсор (аппаратный)
  if ((MenuEditStatus) && (!EditDesc->IsIndex))
    {
      if (EditDesc->Type == DATA_TYPE_ULONG)
        {
          LCD_goto(EditStart+EditPos, MenuActiveLine-MenuFirstLine+1);
        }
      else if ((EditDesc->Type == DATA_TYPE_TIME) || (EditDesc->Type == DATA_TYPE_HOUR_MIN))
        {
          LCD_goto(EditStart+EditTimePos[EditPos], MenuActiveLine-MenuFirstLine+1);
        }
      else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
        {
          LCD_goto(EditStart+EditIpPos[EditPos], MenuActiveLine-MenuFirstLine+1);
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
      if ((mline->LineType == MENU_LINE_GOTO_MENU) || (mline->LineType == MENU_LINE_SHOW_DESC))
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
  else if ((EditDesc->Type == DATA_TYPE_TIME) || (EditDesc->Type == DATA_TYPE_HOUR_MIN))
    {
      GetDataStr(EditDesc, str, 0, DATA_FLAG_SYSTEM_INDEX);
    }
  else if (EditDesc->Type == DATA_TYPE_IP_ADDR)
    {
      sprintf((char*)str, "%03d.%03d.%03d.%03d", (int)((Val >> 24) & 0xFF), (int)((Val >> 16) & 0xFF), (int)((Val >> 8) & 0xFF), (int)((Val >> 0) & 0xFF));
    }
}

// начало редактирования
void EnterEdit(void)
{
  // позиция редактирования, 0 - самое старшее десятичное число
  EditPos = 0;
  // указатель на редактируемый дескриптор
  EditDesc = (TDataDescStruct*)(MenuCurrentPanel->LineArray[MenuActiveLine+1].pMenuLine)->Ptr;
  // номер строки с редактируемым дескриптором
  EditLine = MenuActiveLine+1-MenuFirstLine;
  // редактируемое число
  GetData(EditDesc, &EditVal, 0, DATA_FLAG_SYSTEM_INDEX);
  // границы 
  TRangeValueULONG* RVal = EditDesc->RangeValue;
  memcpy(&EditMin, &RVal->Min, sizeof(CPU_INT32U));
  memcpy(&EditMax, &RVal->Max, sizeof(CPU_INT32U));
 
  if (EditDesc->IsIndex)
    {
      GetDataStr((const TDataDescStruct*)EditDesc, EditBuf, 0, DATA_FLAG_SYSTEM_INDEX);
    }
  else 
    {
      GetDataNameStr((const TDataDescStruct*)EditDesc, EditBuf);
      if (EditDesc->Name) strcat((char*)EditBuf, "=");
      EditStart = strlen((char const*)EditBuf)+1;
      if (EditDesc->Type == DATA_TYPE_ULONG)
        {
          sprintf((char*)EditBuf, "%u", RVal->Max);
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
  MenuEditStatus = 0;
}


void  MenuTask(void *p_arg)
{
  int pause = 0;

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
            if (key)
              {
                switch (key){
                  
                  case KEY_UP:
                    //if (MenuActiveLine) MenuActiveLine--;
                    MenuActiveLine = GetPrevActiveLine(MenuCurrentPanel, MenuActiveLine);
                    ShowCurrentMenu();
                    break;
                    
                  case KEY_DOWN:
                    //MenuActiveLine++;
                    //if (MenuActiveLine+1 >= MenuCurrentPanel->LineNum) MenuActiveLine--;
                    MenuActiveLine = GetNextActiveLine(MenuCurrentPanel, MenuActiveLine);
                    ShowCurrentMenu();
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
                        ShowCurrentMenu();
                      }
                    }
                    break;
                    
                  case KEY_STOP:
                    // пробуем выйти в предыдущее меню
                    GoToPreviousMenu();
                    break;
                    
                  case KEY_START:
                    {
                    if (MenuCurrentPanel->PanelType == MENU_PANEL_STATIC) {ShowCurrentMenu();break;}
                    TMenuLine* pLine = (TMenuLine*)MenuCurrentPanel->LineArray[MenuActiveLine+1].pMenuLine;
                    if (pLine->LineType == MENU_LINE_SHOW_DESC)
                      {// входим в редактирование, если можно
                        TDataDescStruct* desc = (TDataDescStruct*)pLine->Ptr;
                        if (desc->Desc == DATA_DESC_EDIT) {EnterEdit(); pause=1000;}
                      }
                    else if (pLine->LineType == MENU_LINE_GOTO_MENU)
                      {// пробуем перейти в следующее меню
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
              switch (key){
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
                      if (EditPos) EditPos--;
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
                      if (EditPos < EditLen-1) EditPos++;
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

  MenuStackPtr = 0;
  MenuActiveLine = 0;
  MenuFirstLine = 0;
  MenuEditStatus = 0;
  MenuCurrentPanel = NULL;
    
  memset(&MenuStack, 0, sizeof(TMenuStack)*STACKPANELSIZE);
  
  OSTaskCreate(MenuTask, (void *)0, (OS_STK *)&MenuTaskStk[MENU_TASK_STK_SIZE-1], MENU_TASK_PRIO);
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

