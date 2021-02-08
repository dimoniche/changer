#include <includes.h>
#include "app_serv.h"
#include "menu.h"
#include "menudesc.h"
#include "data.h"
#include "datadesc.h"
#include "control.h"
#include "journal.h"
#include "time.h"
#include "mode.h"
#include "version.h"

char FlagForPrintReport=0;

/***********************************
  СТАРТОВОЕ МЕНЮ-ЗАСТАВКА
***********************************/
const CPU_INT08U str_StartMenu_0[] = "-------------------";
const CPU_INT08U str_StartMenu_1[] = "  Идет проверка";
const CPU_INT08U str_StartMenu_2[] = "  оборудования!";
const CPU_INT08U str_StartMenu_3[] = "-------------------";

const TMenuLine line_StartMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_StartMenu_0,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_StartMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_StartMenu_1,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_StartMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_StartMenu_2,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_StartMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_StartMenu_3,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};


const TMenuLineArray arr_StartMenuArray[] = {&line_StartMenu_0, &line_StartMenu_1, &line_StartMenu_2, &line_StartMenu_3, NULL};
const TMenuPanel StartMenuPanel[] = {arr_StartMenuArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
  МЕНЮ ИНКАССАЦИЯ
***********************************/
const CPU_INT08U str_IncasMenu_0[] = "-------------------";
const CPU_INT08U str_IncasMenu_1[] = "     ВНИМАНИЕ     ";
const CPU_INT08U str_IncasMenu_2[] = "    ИНКАССАЦИЯ    ";
CPU_INT08U str_IncasMenu_3[32];

const TMenuLine line_IncasMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_IncasMenu_0,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_IncasMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_IncasMenu_1,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_IncasMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_IncasMenu_2,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_IncasMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_IncasMenu_3,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};


const TMenuLineArray arr_IncasMenuArray[] = {&line_IncasMenu_0, &line_IncasMenu_1, &line_IncasMenu_2, &line_IncasMenu_3, NULL};
const TMenuPanel IncasMenuPanel[] = {arr_IncasMenuArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
 МЕНЮ "В ЖУРНАЛЕ ЗАПИСЕЙ НЕТ"
***********************************/
const CPU_INT08U str_JournalEmptyMenu_0[] = "";
const CPU_INT08U str_JournalEmptyMenu_1[] = "    В журнале";
const CPU_INT08U str_JournalEmptyMenu_2[] = "   записей нет";
const CPU_INT08U str_JournalEmptyMenu_3[] = "";

const TMenuLine line_JournalEmptyMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_JournalEmptyMenu_0,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_JournalEmptyMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_JournalEmptyMenu_1,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_JournalEmptyMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_JournalEmptyMenu_2,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_JournalEmptyMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_JournalEmptyMenu_3,         // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};


const TMenuLineArray arr_JournalEmptyMenuArray[] = {&line_JournalEmptyMenu_0, &line_JournalEmptyMenu_1, &line_JournalEmptyMenu_2, &line_JournalEmptyMenu_3, NULL};
const TMenuPanel JournalEmptyMenuPanel[] = {arr_JournalEmptyMenuArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
  СЕРВИСНОЕ МЕНЮ
***********************************/

const CPU_INT08U str_ServiceMenu_0[] = "СЕРВИС вер.ПО "DEVICE_FW_VERSION;
const CPU_INT08U str_ServiceMenu_1[] = "Настройки";
const CPU_INT08U str_ServiceMenu_2[] = "Статистика";
const CPU_INT08U str_ServiceMenu_3[] = "Журналы";
const CPU_INT08U str_ServiceMenu_4[] = "Отчеты";

const TMenuLine line_ServiceMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ServiceMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ServiceMenu_1 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_ServiceMenu_1,       // указатель на текстовую строку или дескриптор
  (void*)&SettingsMenuPanel                            // панель для перехода
};

const TMenuLine line_ServiceMenu_2 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_ServiceMenu_2,        // указатель на текстовую строку или дескриптор
  (void*)&StatisticsMenuPanel                            // панель для перехода
};

const TMenuLine line_ServiceMenu_3 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_ServiceMenu_3,        // указатель на текстовую строку или дескриптор
  (void*)&SelectJournalMenuPanel                           // панель для перехода
};

const TMenuLine line_ServiceMenu_4 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_ServiceMenu_4,        // указатель на текстовую строку или дескриптор
  (void*)&ReportMenuPanel                           // панель для перехода
};

const TMenuLineArray arr_ServiceMenuArray[] = {&line_ServiceMenu_0, &line_ServiceMenu_1, &line_ServiceMenu_2, &line_ServiceMenu_3, &line_ServiceMenu_4, NULL};
const TMenuPanel ServiceMenuPanel[] = {arr_ServiceMenuArray, NULL, 5, MENU_PANEL_STANDARD};


/***********************************
  МЕНЮ СТАТИСТИКА
***********************************/

const CPU_INT08U str_StatisticsMenu_0[] = "    СТАТИСТИКА";
const CPU_INT08U str_StatisticsMenu_2[] = "Общая";
const CPU_INT08U str_StatisticsMenu_3[] = "Счетчики купюр";
const CPU_INT08U str_StatisticsMenu_4[] = "Очистка статистики";


const TMenuLine line_StatisticsMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_StatisticsMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_StatisticsMenu_2 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_StatisticsMenu_2,        // указатель на текстовую строку или дескриптор
  (void*)CommStatMenuPanel                           // панель для перехода
};

const TMenuLine line_StatisticsMenu_3 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_StatisticsMenu_3,        // указатель на текстовую строку или дескриптор
  (void*)BillCountersPanel                           // панель для перехода
};

const TMenuLine line_StatisticsMenu_4 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_StatisticsMenu_4,        // указатель на текстовую строку или дескриптор
  (void*)ClearStatMenu                           // панель для перехода
};

const TMenuLineArray arr_StatisticsMenuArray[] = {&line_StatisticsMenu_0, &line_StatisticsMenu_2, &line_StatisticsMenu_3, &line_StatisticsMenu_4, NULL};
const TMenuPanel StatisticsMenuPanel[] = {arr_StatisticsMenuArray, NULL, 4, MENU_PANEL_STANDARD};


/***********************************
  МЕНЮ ОЧИСТКА СТАТИСТИКИ
***********************************/
const char str_ClearStatMenu_0[] = "   ДЛЯ ОЧИСТКИ";
const char str_ClearStatMenu_1[] = "СТАТИСТИКИ ВВЕДИТЕ";

void OnEnterPanelClearStat(void)
{
  TempPass = 0;
}

const TMenuLine line_ClearStatMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ClearStatMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ClearStatMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ClearStatMenu_1,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ClearStatMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PassTempDesc2,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_ClearStatMenuArray[] = {&line_ClearStatMenu_0, &line_ClearStatMenu_1, &line_ClearStatMenu_2, NULL};
const TMenuPanel ClearStatMenu[] = {arr_ClearStatMenuArray, OnEnterPanelClearStat, 3, MENU_PANEL_STANDARD};


/***********************************
МЕНЮ ОЧИСТКА ЖУРНАЛОВ
***********************************/
const char str_ClearJournalMenu_0[] = "   ДЛЯ ОЧИСТКИ";
const char str_ClearJournalMenu_1[] = " ЖУРНАЛОВ ВВЕДИТЕ";

void OnEnterPanelClearJournal(void)
{
  TempPass = 0;
}

const TMenuLine line_ClearJournalMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ClearJournalMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ClearJournalMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ClearJournalMenu_1,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ClearJournalMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PassTempDesc2,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_ClearJournalMenuArray[] = {&line_ClearJournalMenu_0, &line_ClearJournalMenu_1, &line_ClearJournalMenu_2, NULL};
const TMenuPanel ClearJournalMenuPanel[] = {arr_ClearJournalMenuArray, OnEnterPanelClearStat, 3, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ КАНАЛЬНАЯ СТАТИСТИКА
***********************************/
const TMenuLine line_ChannelCountersMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // доп. флаги  
  (void*)&ChannelStIndexDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelCountersMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterChannelRunDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelCountersMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterChannelMoneyDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelCountersMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterChannelTimeDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_ChannelCountersArray[] = {&line_ChannelCountersMenu_0, &line_ChannelCountersMenu_1, &line_ChannelCountersMenu_2, &line_ChannelCountersMenu_3, NULL};
const TMenuPanel ChannelCountersPanel[] = {arr_ChannelCountersArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
  МЕНЮ СЧЕТЧИКИ КУПЮР
***********************************/
const TMenuLine line_BillCountersMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // доп. флаги  
  (void*)&BillnomIndexDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_BillCountersMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&BillnomDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_BillCountersMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&BillnomCountersDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_BillCountersMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&BillCounterDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

extern CPU_INT32U  BillnomViewIndex;

void OnEnterBillCountersMenu()
{
    BillnomViewIndex = 0;
}

const TMenuLineArray arr_BillCountersArray[] = {&line_BillCountersMenu_0, &line_BillCountersMenu_1, &line_BillCountersMenu_2, &line_BillCountersMenu_3, NULL};
const TMenuPanel BillCountersPanel[] = {arr_BillCountersArray, OnEnterBillCountersMenu, 4, MENU_PANEL_STATIC};


/***********************************
  МЕНЮ ОБЩАЯ СТАТИСТИКА
***********************************/
const CPU_INT08U str_CommonCountersMenu_0[] = "КОРОТКИЕ СЧЕТЧИКИ";

const TMenuLine line_CommonCountersMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)&str_CommonCountersMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
/*
const TMenuLine line_CommonCountersMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterRunDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
*/
const TMenuLine line_CommonCountersMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterMoneyDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
/*
const TMenuLine line_CommonCountersMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterTimeDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
*/
const TMenuLineArray arr_CommonCountersArray[] = {&line_CommonCountersMenu_0, /*&line_CommonCountersMenu_1,*/ &line_CommonCountersMenu_2, /*&line_CommonCountersMenu_3,*/ NULL};
const TMenuPanel CommonCountersPanel[] = {arr_CommonCountersArray, NULL, 2, MENU_PANEL_STATIC};

/***********************************
  МЕНЮ ОБЩАЯ СТАТИСТИКА ДЛИННЫЕ СЧЕТЧИКИ
***********************************/
const CPU_INT08U str_CommonCountersLongMenu_0[] = "ДЛИННЫЕ СЧЕТЧИКИ";

const TMenuLine line_CommonCountersLongMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)&str_CommonCountersLongMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
/*
const TMenuLine line_CommonCountersLongMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterLongRunDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
*/
const TMenuLine line_CommonCountersLongMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterLongMoneyDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
/*
const TMenuLine line_CommonCountersLongMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterLongTimeDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
*/
const TMenuLineArray arr_CommonCountersLongArray[] = {&line_CommonCountersLongMenu_0, /*&line_CommonCountersLongMenu_1,*/ &line_CommonCountersLongMenu_2, /*&line_CommonCountersLongMenu_3, */ NULL};
const TMenuPanel CommonCountersLongPanel[] = {arr_CommonCountersLongArray, NULL, 2, MENU_PANEL_STATIC};

/***********************************
  МЕНЮ НАСТРОЙКИ
***********************************/

const CPU_INT08U str_SettingsMenu_0[] = "    НАСТРОйКИ";
//const CPU_INT08U str_SettingsMenu_1[] = "Посты";
const CPU_INT08U str_SettingsMenu_2[] = "Оборудование";
const CPU_INT08U str_SettingsMenu_6[] = "Сеть";
const CPU_INT08U str_SettingsMenu_3[] = "Установка пароля";
const CPU_INT08U str_SettingsMenu_4[] = "Установка времени";
const CPU_INT08U str_SettingsMenu_5[] = "Сброс настроек";

const TMenuLine line_SettingsMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_SettingsMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
/*
const TMenuLine line_SettingsMenu_1 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_SettingsMenu_1,       // указатель на текстовую строку или дескриптор
  (void*)&ChannelMenuPanel                            // панель для перехода
};
*/
const TMenuLine line_SettingsMenu_2 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_SettingsMenu_2,        // указатель на текстовую строку или дескриптор
  (void*)&DeviceMenuPanel                           // панель для перехода
};

const TMenuLine line_SettingsMenu_8 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_SettingsMenu_6,        // указатель на текстовую строку или дескриптор
  (void*)&NetworkMenuPanel                           // панель для перехода
};

const TMenuLine line_SettingsMenu_3 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_SettingsMenu_3,        // указатель на текстовую строку или дескриптор
  (void*)&SetPassMenuPanel                            // панель для перехода
};

const TMenuLine line_SettingsMenu_5 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_SettingsMenu_4,        // указатель на текстовую строку или дескриптор
  (void*)&TimeSetupMenuPanel                           // панель для перехода
};

const TMenuLine line_SettingsMenu_6 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_SettingsMenu_5,        // указатель на текстовую строку или дескриптор
  (void*)&ResetSettingsMenuPanel                           // панель для перехода
};

/*
const TMenuLine line_SettingsMenu_7 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&StartButtonNameDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};
*/
const TMenuLineArray arr_SettingsMenuArray[] = {&line_SettingsMenu_0, &line_SettingsMenu_2, &line_SettingsMenu_5, &line_SettingsMenu_3, &line_SettingsMenu_6, &line_SettingsMenu_8, NULL};
const TMenuPanel SettingsMenuPanel[] = {arr_SettingsMenuArray, NULL, 6, MENU_PANEL_STANDARD};



/***********************************
  МЕНЮ НАСТРОЙКА КАНАЛОВ
***********************************/
const TMenuLine line_ChannelMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // доп. флаги  
  (void*)&ChannelIndexDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&EnableChannelDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&PostIpAddrDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&SelectProtectDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelMenu_4 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&PostImpCostDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelMenu_5 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&PostLenCostDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelMenu_6 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&PostMinutePriceDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_ChannelMenuArray[] = {&line_ChannelMenu_0,
                                               &line_ChannelMenu_1,
                                               &line_ChannelMenu_2,
                                               &line_ChannelMenu_4,
                                               &line_ChannelMenu_5,
                                               &line_ChannelMenu_3,
                                               &line_ChannelMenu_6,
                                               NULL};
char flag_enter_periods=0;

void OnEnterChannelSettingsMenu(void)
{
  if (!flag_enter_periods)
    {
      ChannelIndex = 0;
    }
  else
    {
      flag_enter_periods = 0;
    }
}

const TMenuPanel ChannelMenuPanel[] = {arr_ChannelMenuArray, OnEnterChannelSettingsMenu, 7, MENU_PANEL_STANDARD};


/***********************************
  МЕНЮ НАСТРОЙКА СЕТИ
***********************************/
const CPU_INT08U str_NetworkMenu_0[] = "  НАСТРОйКА СЕТИ";

const TMenuLine line_NetworkMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_NetworkMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_NetworkMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                // доп. флаги  
  (void*)&IpAddrDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_NetworkMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                // доп. флаги  
  (void*)&NetMaskDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_NetworkMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                // доп. флаги  
  (void*)&GatewayDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_NetworkMenuArray[] = {&line_NetworkMenu_0, &line_NetworkMenu_1, &line_NetworkMenu_2, &line_NetworkMenu_3, NULL};
const TMenuPanel NetworkMenuPanel[] = {arr_NetworkMenuArray, NULL, 4, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ НАСТРОЙКА ЦЕНЫ НА БУДНЯХ
***********************************/
void OnEnterPanelPrice(void)
{
  PeriodIndex = ChannelIndex*PRICE_PERIODS_NUM;
  flag_enter_periods = 1;
}

const TMenuLine line_PriceMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // доп. флаги  
  (void*)&PeriodWeekdaysIndexDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_PriceMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&T_Start_WeekdaysDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_PriceMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&T_End_WeekdaysDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_PriceMenu_3 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PriceWeekdaysDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_PriceMenu_4 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PriceTimeWeekdaysDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_PriceMenuArray[] = {&line_PriceMenu_0, &line_PriceMenu_1, &line_PriceMenu_2, &line_PriceMenu_3, &line_PriceMenu_4, NULL};
const TMenuPanel PriceWeekdaysMenuPanel[] = {arr_PriceMenuArray, OnEnterPanelPrice, 5, MENU_PANEL_STANDARD};


/***********************************
  МЕНЮ ВВОД ТЕКУЩЕГО ПАРОЛЯ
***********************************/
const char str_SetPassMenu_0[] = "  ВВЕДИТЕ ТЕКУЩИй";

void OnEnterPanelSetPass(void)
{
  TempPass = 0;
}

const TMenuLine line_SetPassMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_SetPassMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_SetPassMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PassTempDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_SetPassMenu_2 = {
  MENU_LINE_GOTO_MENU,          // тип пункта меню
  0,                            // доп. флаги  
  "Ввод мастер-пароля",       // указатель на текстовую строку или дескриптор
  (void*)MasterPassMenuPanel                          // панель для перехода
};

const TMenuLineArray arr_SetPassMenuArray[] = {&line_SetPassMenu_0, &line_SetPassMenu_1, &line_SetPassMenu_2, NULL};
const TMenuPanel SetPassMenuPanel[] = {arr_SetPassMenuArray, OnEnterPanelSetPass, 3, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ ВВОД МАСТЕР-ПАРОЛЯ
***********************************/
const char str_MasterPassMenu_0[] = "ВВЕДИТЕ МАСТЕР-";

void OnEnterPanelMasterPass(void)
{
  TempPass = 0;
}

const TMenuLine line_MasterPassMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_MasterPassMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_MasterPassMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&MasterPassTempDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_MasterPassMenuArray[] = {&line_MasterPassMenu_0, &line_MasterPassMenu_1, NULL};
const TMenuPanel MasterPassMenuPanel[] = {arr_MasterPassMenuArray, OnEnterPanelSetPass, 3, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ ВВОД ТЕКУЩЕГО ПАРОЛЯ
***********************************/
const char str_ResetSetingsMenu_0[] = "    ДЛЯ СБРОСА";
const char str_ResetSetingsMenu_1[] = " НАСТРОЕК ВВЕДИТЕ";

void OnEnterPanelResetSetings(void)
{
  TempPass = 0;
}

const TMenuLine line_ResetSetingsMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ResetSetingsMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ResetSetingsMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ResetSetingsMenu_1,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ResetSetingsMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PassTempDesc1,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_ResetSettingsMenuArray[] = {&line_ResetSetingsMenu_0, &line_ResetSetingsMenu_1, &line_ResetSetingsMenu_2, NULL};
const TMenuPanel ResetSettingsMenuPanel[] = {arr_ResetSettingsMenuArray, OnEnterPanelResetSetings, 3, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ ВВОД НОВОГО ПАРОЛЯ
***********************************/
const char str_SetNewPassMenu_0[] = "  ВВЕДИТЕ НОВЫй";

const TMenuLine line_SetNewPassMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_SetNewPassMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_SetNewPassMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PassDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_SetNewPassMenuArray[] = {&line_SetNewPassMenu_0, &line_SetNewPassMenu_1, NULL};
const TMenuPanel SetNewPassMenuPanel[] = {arr_SetNewPassMenuArray, NULL, 2, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ НАСТРОЙКА ЦЕНЫ НА ВЫХОДНЫХ
***********************************/
const TMenuLine line_PriceMenuWend_0 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // доп. флаги  
  (void*)&PeriodWeekendIndexDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_PriceMenuWend_1 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&T_Start_WeekendDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_PriceMenuWend_2 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&T_End_WeekendDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_PriceMenuWend_3 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PriceWeekendDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_PriceMenuWend_4 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&PriceTimeWeekendDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_PriceMenuArrayWend[] = {&line_PriceMenuWend_0, &line_PriceMenuWend_1, &line_PriceMenuWend_2, &line_PriceMenuWend_3, &line_PriceMenuWend_4, NULL};
const TMenuPanel PriceWeekendMenuPanel[] = {arr_PriceMenuArrayWend, OnEnterPanelPrice, 5, MENU_PANEL_STANDARD};


/***********************************
  МЕНЮ НАСТРОЙКА ОБОРУДОВАНИЯ
***********************************/

const CPU_INT08U str_DeviceMenu_0[] = "   ОБОРУДОВАНИЕ";
const CPU_INT08U str_DeviceMenu_1[] = "ФР";
const CPU_INT08U str_DeviceMenu_2[] = "Монетоприемник";
const CPU_INT08U str_DeviceMenu_3[] = "Модем";
const CPU_INT08U str_DeviceMenu_4[] = "Купюроприемник";

const TMenuLine line_DeviceMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_DeviceMenu_0,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_DeviceMenu_1 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_DeviceMenu_1,       // указатель на текстовую строку или дескриптор
  (void*)&FrMenuPanel                            // панель для перехода
};

const TMenuLine line_DeviceMenu_2 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_DeviceMenu_4,       // указатель на текстовую строку или дескриптор
  (void*)&ValidatorSetupPanel                            // панель для перехода
};

const TMenuLine line_DeviceMenu_3 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_DeviceMenu_2,       // указатель на текстовую строку или дескриптор
  (void*)&CoinSetupPanel                            // панель для перехода
};

const TMenuLine line_DeviceMenu_4 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_DeviceMenu_3,       // указатель на текстовую строку или дескриптор
  (void*)&ModemSetupPanel                            // панель для перехода
};

const TMenuLineArray arr_DeviceMenuArray[] = {&line_DeviceMenu_0, &line_DeviceMenu_1, &line_DeviceMenu_2, &line_DeviceMenu_3, &line_DeviceMenu_4, NULL};
const TMenuPanel DeviceMenuPanel[] = {arr_DeviceMenuArray, NULL, 5, MENU_PANEL_STANDARD};


/***********************************
  МЕНЮ НАСТРОЙКА ФР
***********************************/
const CPU_INT08U str_FrMenu_0[] = "   НАСТРОйКИ ФР";

const TMenuLine line_FrMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_FrMenu_0,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_FrMenu_1 = {
  MENU_LINE_SHOW_DESC, 
  0,                   
  (void*)&EnableFiscalDesc,
  NULL                     
};

const TMenuLine line_FrMenu_2 = {
  MENU_LINE_SHOW_DESC, 
  0,                   
  (void*)&PrintModeDesc,
  NULL                     
};


const TMenuLine line_FrMenu_3 = {
  MENU_LINE_SHOW_DESC, 
  0,                   
  (void*)&PrintTimeoutDesc,
  NULL                     
};

const TMenuLine line_FrMenu_4 = {
  MENU_LINE_SHOW_DESC, 
  0,                   
  (void*)&PrintTimeoutAfterDesc,
  NULL                     
};

const TMenuLine line_FrMenu_5 = {
  MENU_LINE_SHOW_DESC,
  0,
  (void*)&BillFormatDesc,
  NULL
};

const TMenuLine line_FrMenu_6 = {
  MENU_LINE_SHOW_DESC,
  0,
  (void*)&CommandV2Desc,
  NULL
};

const TMenuLine line_FrMenu_7 = {
  MENU_LINE_SHOW_DESC,
  0,
  (void*)&TaxSystemDesc,
  NULL
};

const TMenuLine line_FrMenu_8 = {
  MENU_LINE_SHOW_DESC,
  0,
  (void*)&TaxFormatDesc,
  NULL
};

const TMenuLine line_FrMenu_9 = {
  MENU_LINE_SHOW_DESC,
  0,
  (void*)&SubjSellDesc,
  NULL
};

const TMenuLine line_FrMenu_10 = {
  MENU_LINE_SHOW_DESC,
  0,
  (void*)&PriceDesc,
  NULL
};

const TMenuLine line_FrMenu_11 = {
  MENU_LINE_SHOW_DESC,
  0,
  (void*)&ServiceNameDesc,
  NULL
};

const TMenuLine line_FrMenu_12 = {
  MENU_LINE_SHOW_DESC, 
  0,                   
  (void*)&DisableFiscalErrorsDesc,
  NULL                     
};

const TMenuLineArray arr_FrMenuArray[] = {&line_FrMenu_0, &line_FrMenu_1, &line_FrMenu_2, &line_FrMenu_3, &line_FrMenu_4, &line_FrMenu_5, &line_FrMenu_6, &line_FrMenu_7, &line_FrMenu_8, &line_FrMenu_9, &line_FrMenu_10, &line_FrMenu_11, &line_FrMenu_12, NULL};
const TMenuPanel FrMenuPanel[] = {arr_FrMenuArray, NULL, 13, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ НАСТРОЙКА МОНЕТОПРИЕМНИКА
***********************************/
const CPU_INT08U str_CoinMenu_0[] = "НАСТРОйКИ МОНЕТОПР.";

const TMenuLine line_CoinMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_CoinMenu_0,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_CoinMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&EnableCoinDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_CoinMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CoinPerPulseDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_CoinMenuArray[] = {&line_CoinMenu_0, &line_CoinMenu_1, &line_CoinMenu_2, NULL};
const TMenuPanel CoinSetupPanel[] = {arr_CoinMenuArray, NULL, 3, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ НАСТРОЙКА КУПЮРОПРИЕМНИКА
***********************************/
const CPU_INT08U str_CashMenu_0[] = "НАСТРОйКИ КУПЮРОПР.";

const TMenuLine line_CashMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_CashMenu_0,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_CashMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&EnableValidatorDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_CashMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CashModeDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_CashMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CashPerPulseDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_CashMenu_4 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CashPulseLenDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_CashMenu_5 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CashPauseLenDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_ValidatorMenuArray[] = {&line_CashMenu_0, &line_CashMenu_1, &line_CashMenu_2, &line_CashMenu_3, &line_CashMenu_4, &line_CashMenu_5, NULL};
const TMenuPanel ValidatorSetupPanel[] = {arr_ValidatorMenuArray, NULL, 6, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ НАСТРОЙКА МОДЕМА
***********************************/
const CPU_INT08U str_ModemMenu_0[] = " НАСТРОйКИ МОДЕМА";

const TMenuLine line_ModemMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ModemMenu_0,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ModemMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&EnableModemDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ModemMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&ModemStatusDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ModemMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&StatSendHourMinDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ModemMenu_4 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&EnableEmailJournalSendDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ModemMenu_5 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&DeviceIDDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ModemMenu_6 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&SendTestEmailDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};


const TMenuLineArray arr_ModemMenuArray[] = {&line_ModemMenu_0, &line_ModemMenu_1, &line_ModemMenu_2, &line_ModemMenu_3, &line_ModemMenu_4, &line_ModemMenu_5, &line_ModemMenu_6, NULL};
const TMenuPanel ModemSetupPanel[] = {arr_ModemMenuArray, NULL, 7, MENU_PANEL_STANDARD};

/***********************************
  ЗАСТАВКА ДЛЯ КЛИЕНТА
***********************************/

char str_UserMenu_0[22] = "";
char str_UserMenu_1[22] = "";
char str_UserMenu_2[22] = "";
char str_UserMenu_3[22] = "";

char str_buf[22];


void PrintUserMenuStr(char* str, CPU_INT08U n)
{
  char *strptr;
  char *instr;
  // выровняем по центру для красоты
    
  switch (n)
  {
    case 0:
      strptr = str_UserMenu_0;
      break;
    case 1:
      strptr = str_UserMenu_1;
      break;
    case 2:
      strptr = str_UserMenu_2;
      break;
    case 3:
      strptr = str_UserMenu_3;
      break;
    default:
      return;
  }

  // найдем начало строки, отличное от пробела
  instr = str;
  while (*instr==0x20) instr++;
  
  memset(strptr, 0x20, 20);
  
  int len = strlen(instr);
  if ((len >= 20) || ((10-len/2-1) < 0)) {strcpy(strptr, instr); return;}

  strcpy(&strptr[10-len/2-1], instr);
}

const TMenuLine line_FirstMenu_0 = {
  MENU_LINE_STRING,                 // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_UserMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_FirstMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_UserMenu_1,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_FirstMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_UserMenu_2,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLine line_FirstMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_UserMenu_3,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};


const TMenuLineArray FirstMenuArray[] = {&line_FirstMenu_0, &line_FirstMenu_1, &line_FirstMenu_2, &line_FirstMenu_3, NULL};
const TMenuPanel FirstMenuPanel[] = {FirstMenuArray, InitUserMenu, 4, MENU_PANEL_STATIC};

/***********************************
  ПАНЕЛЬ "НЕВЕРНЫЙ ПАРОЛЬ"
***********************************/
const CPU_INT08U str_ErrPass_0[] = "     Ошибка ";
const CPU_INT08U str_ErrPass_1[] = " Неверный пароль";
const CPU_INT08U str_ErrPass_2[] = "";

const TMenuLine line_ErrPassMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_ErrPass_0,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLine line_ErrPassMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_ErrPass_1,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};
const TMenuLine line_ErrPassMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_ErrPass_2,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLineArray ErrPassMenuArray[] = {&line_ErrPassMenu_2, &line_ErrPassMenu_0, &line_ErrPassMenu_1, &line_ErrPassMenu_2, NULL};
const TMenuPanel ErrorPassPanel[] = {ErrPassMenuArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
  ПАНЕЛЬ "НАСТРОЙКИ СБРОШЕНЫ"
***********************************/
const CPU_INT08U str_SettingsIsResetPass_0[] = "   Установлены  ";
const CPU_INT08U str_SettingsIsResetPass_1[] = "    заводские ";
const CPU_INT08U str_SettingsIsResetPass_2[] = "     значения";
const CPU_INT08U str_SettingsIsResetPass_3[] = "    параметров";

const TMenuLine line__SettingsIsResetMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_SettingsIsResetPass_0,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLine line__SettingsIsResetMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_SettingsIsResetPass_1,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};
const TMenuLine line__SettingsIsResetMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_SettingsIsResetPass_2,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLine line__SettingsIsResetMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_SettingsIsResetPass_3,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLineArray SettingsIsResetMenuArray[] = {&line__SettingsIsResetMenu_0, &line__SettingsIsResetMenu_1, &line__SettingsIsResetMenu_2, &line__SettingsIsResetMenu_3, NULL};
const TMenuPanel SettingsIsReset[] = {SettingsIsResetMenuArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
  ПАНЕЛЬ "СТАТИСТИКА ОЧИЩЕНА"
***********************************/
const CPU_INT08U str_StatIsResetPass_0[] = "    Статистика";
const CPU_INT08U str_StatIsResetPass_1[] = "     очищена";
const CPU_INT08U str_StatIsResetPass_2[] = "";

const TMenuLine line__StatIsResetMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_StatIsResetPass_0,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLine line__StatIsResetMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_StatIsResetPass_1,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};
const TMenuLine line__StatIsResetMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_StatIsResetPass_2,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLineArray StatIsResetMenuArray[] = {&line__StatIsResetMenu_2, &line__StatIsResetMenu_0, &line__StatIsResetMenu_1, &line__StatIsResetMenu_2, NULL};
const TMenuPanel StatIsReset[] = {StatIsResetMenuArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
ПАНЕЛЬ "ЖУРНАЛЫ ОЧИЩЕНЫ"
***********************************/
const CPU_INT08U str_JournalIsResetPass_0[] = "     Журналы ";
const CPU_INT08U str_JournalIsResetPass_1[] = "     очищены";
const CPU_INT08U str_JournalIsResetPass_2[] = "";

const TMenuLine line__JournalIsResetMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_JournalIsResetPass_0,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLine line__JournalIsResetMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_JournalIsResetPass_1,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};
const TMenuLine line__JournalIsResetMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                // доп. флаги  
  (void*)str_JournalIsResetPass_2,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLineArray JournalIsResetMenuArray[] = {&line__JournalIsResetMenu_2, &line__JournalIsResetMenu_0, &line__JournalIsResetMenu_1, &line__JournalIsResetMenu_2, NULL};
const TMenuPanel JournalIsReset[] = {JournalIsResetMenuArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
  ПАНЕЛЬ ВНЕСЕНИЯ ДЕНЕГ
***********************************/

const CPU_INT08U str_GetMoney_0[] = " Внесите деньги";

const TMenuLine line_GetMoneyMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_GetMoney_0,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLine line_GetMoneyMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)&AcceptedMoneyDesc,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_GetMoneyMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_GetMoney_0,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};

const TMenuLine line_GetMoneyMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_GetMoney_0,        // указатель на текстовую строку или дескриптор
  NULL                           // панель для перехода
};


const TMenuLineArray GetMoneyMenuArray[] = {&line_GetMoneyMenu_0, &line_GetMoneyMenu_1, &line_GetMoneyMenu_2, &line_GetMoneyMenu_3, NULL};
const TMenuPanel GetMoneyMenuPanel[] = {GetMoneyMenuArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
  МЕНЮ ВЫБОР ЖУРНАЛА ДЛЯ ПРОСМОТРА
***********************************/
const CPU_INT08U str_SelectJournalMenu_0[] = "ПРОСМОТР ЖУРНАЛОВ";
const CPU_INT08U str_SelectJournalMenu_1[] = "Журнал событий";
const CPU_INT08U str_SelectJournalMenu_2[] = "Очистка журналов";

const TMenuLine line_SelectJournalMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_SelectJournalMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_SelectJournalMenu_1 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_SelectJournalMenu_1,        // указатель на текстовую строку или дескриптор
  (void*)&EventJournalMenuPanel                            // панель для перехода
};

const TMenuLine line_SelectJournalMenu_2 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_SelectJournalMenu_2,        // указатель на текстовую строку или дескриптор
  (void*)&ClearJournalMenuPanel                            // панель для перехода
};

const TMenuLineArray arr_SelectJournalMenuArray[] = {&line_SelectJournalMenu_0, &line_SelectJournalMenu_1, &line_SelectJournalMenu_2, NULL};
const TMenuPanel SelectJournalMenuPanel[] = {arr_SelectJournalMenuArray, NULL, 3, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ ПЕЧАТЬ ОТЧЕТОВ
***********************************/
const CPU_INT08U str_ReportMenu_0[] = "      ОТЧЕТЫ";
const CPU_INT08U str_ReportMenu_1[] = "X-отчет";
const CPU_INT08U str_ReportMenu_2[] = "Отчет о закр.смены";
const CPU_INT08U str_ReportMenu_3[] = "Z-отчет из буфера";

const TMenuLine line_ReportMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ReportMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ReportMenu_1 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_ReportMenu_1,       // указатель на текстовую строку или дескриптор
  (void*)&xReportMenuPanel                            // панель для перехода
};

const TMenuLine line_ReportMenu_2 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_ReportMenu_2,        // указатель на текстовую строку или дескриптор
  (void*)&zReportMenuPanel                            // панель для перехода
};
/*
const TMenuLine line_ReportMenu_3 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_ReportMenu_3,        // указатель на текстовую строку или дескриптор
  (void*)&bufReportMenuPanel                            // панель для перехода
};
*/

void OnEnterReportsPanel(void)
{
  CPU_INT32U enable;
  GetData(&EnableFiscalDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
  if (!enable) 
    {
      GoToPreviousMenu();
      GoToMenu(FrIsOffMenuPanel);
    }
  FlagForPrintReport = 0;
}

const TMenuLineArray arr_ReportMenuArray[] = {&line_ReportMenu_0, &line_ReportMenu_1, &line_ReportMenu_2, /*&line_ReportMenu_3,*/ NULL};
const TMenuPanel ReportMenuPanel[] = {arr_ReportMenuArray, OnEnterReportsPanel, 3, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ ПЕЧАТЬ X-ОТЧЕТА
***********************************/
const CPU_INT08U str_xReportMenu_0[] = "     X-ОТЧЕТ";
const CPU_INT08U str_xReportMenu_1[] = "Напечатать суточный";
const CPU_INT08U str_xReportMenu_2[] = " отчет без гашения?";
const CPU_INT08U str_xReportMenu_3[] = "ДА-ОК    НЕТ-ОТМЕНА";

const TMenuLine line_xReportMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_xReportMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_xReportMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_xReportMenu_1,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_xReportMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_xReportMenu_2,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_xReportMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_xReportMenu_3,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

void OnEnterZXReportPanel(void)
{
  OSTimeDly(20);
  FlagForPrintReport = 1;
}

const TMenuLineArray arr_xReportMenuArray[] = {&line_xReportMenu_0, &line_xReportMenu_1, &line_xReportMenu_2, &line_xReportMenu_3, NULL};
const TMenuPanel xReportMenuPanel[] = {arr_xReportMenuArray, OnEnterZXReportPanel, 4, MENU_PANEL_STATIC};


/***********************************
  МЕНЮ ПЕЧАТЬ Z-ОТЧЕТА
***********************************/
const CPU_INT08U str_zReportMenu_0[] = " ОТЧЕТ О ЗАКР.СМЕНЫ";
const CPU_INT08U str_zReportMenu_1[] = "  Напечатать отчет";
const CPU_INT08U str_zReportMenu_2[] = "  о закрытии смены?";

const TMenuLine line_zReportMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_zReportMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_zReportMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_zReportMenu_1,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_zReportMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_zReportMenu_2,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_zReportMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_xReportMenu_3,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_zReportMenuArray[] = {&line_zReportMenu_0, &line_zReportMenu_1, &line_zReportMenu_2, &line_zReportMenu_3, NULL};
const TMenuPanel zReportMenuPanel[] = {arr_zReportMenuArray, OnEnterZXReportPanel, 4, MENU_PANEL_STATIC};


/***********************************
  МЕНЮ ПЕЧАТЬ Z-ОТЧЕТОВ ИЗ БУФЕРА
***********************************/
const CPU_INT08U str_bufReportMenu_0[] = " Z-ОТЧЕТЫ ИЗ БУФЕРА";
const CPU_INT08U str_bufReportMenu_1[] = "Напечатать суточные";
const CPU_INT08U str_bufReportMenu_2[] = " отчеты из буфера?";

const TMenuLine line_bufReportMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_bufReportMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_bufReportMenu_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_bufReportMenu_1,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_bufReportMenu_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_bufReportMenu_2,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_bufReportMenu_3 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_xReportMenu_3,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_bufReportMenuArray[] = {&line_bufReportMenu_0, &line_bufReportMenu_1, &line_bufReportMenu_2, &line_xReportMenu_3, NULL};
const TMenuPanel bufReportMenuPanel[] = {arr_bufReportMenuArray, OnEnterZXReportPanel, 4, MENU_PANEL_STATIC};

/***********************************
  МЕНЮ ФР ОТКЛЮЧЕН
***********************************/
const CPU_INT08U str_FrIsOff_0[] = "";
const CPU_INT08U str_FrIsOff_1[] = "   ФР отключен";
const CPU_INT08U str_FrIsOff_2[] = "печать невозможна";

const TMenuLine line_FrIsOff_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_FrIsOff_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_FrIsOff_1 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_FrIsOff_1,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_FrIsOff_2 = {
  MENU_LINE_STRING,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_FrIsOff_2,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_FrIsOffMenuArray[] = {&line_FrIsOff_0, &line_FrIsOff_1, &line_FrIsOff_2, NULL};
const TMenuPanel FrIsOffMenuPanel[] = {arr_FrIsOffMenuArray, NULL, 3, MENU_PANEL_STATIC};

/***********************************
  МЕНЮ ПРОСМОТР ЖУРНАЛА СОБЫТИЙ
***********************************/
char str_EventNumber[24];
char str_EventData[24];

const TMenuLine line_EventJournalMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // доп. флаги  
  (void*)&EventJournalIndexDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_EventJournalMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  0,                // доп. флаги  
  (void*)&JournalEventTimeDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_EventJournalMenu_2 = {
  MENU_LINE_STRING,                    // тип пункта меню
  0,                                   // доп. флаги  
  (void*)str_EventNumber,      // указатель на текстовую строку или дескриптор
  NULL                                 // панель для перехода
};

const TMenuLine line_EventJournalMenu_3 = {
  MENU_LINE_STRING,                    // тип пункта меню
  0,                                   // доп. флаги  
  (void*)str_EventData,      // указатель на текстовую строку или дескриптор
  NULL                                 // панель для перехода
};


void PrintEventJournalRecord(TEventRecord *record)
{
  if (record->event)
    { // есть событие
      GetEventStr(str_EventNumber, record->event);
      if ((record->event == JOURNAL_EVENT_MONEY_NOTE) || (record->event == JOURNAL_EVENT_MONEY_COIN))
        {
          sprintf(str_EventData, "%d руб.", record->data);
        }
      else if (record->event == JOURNAL_EVENT_START_SESSION)
        {
          sprintf(&str_EventNumber[strlen(str_EventNumber)], "");
          PrintSecToHourMinSec(str_EventData, record->data);
        }
      else if (record->event == JOURNAL_EVENT_END_SESSION)
        {
          sprintf(&str_EventNumber[strlen(str_EventNumber)], "кан.%d", record->channel+1);
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_DEVICE_ON)
        {
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_BILL)
        {
          sprintf(str_EventData, " ");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_Z)
        {
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_X)
        {
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_BUF)
        {
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_CHANGE_MODE)
        {
          if (record->data == MODE_WORK) sprintf(str_EventData, "работа");
          else sprintf(str_EventData, "настройка");
        }
      else if (record->event == JOURNAL_EVENT_INCASSATION)
        {
            sprintf(str_EventData, "%u руб.", record->data);
        }
      else if (record->event == JOURNAL_EVENT_PASS_FAIL)
        {
            sprintf(str_EventData, "%u", record->data);
        }
      else if ((record->event == JOURNAL_EVENT_EMAIL_OK) || (record->event == JOURNAL_EVENT_EMAIL_FAIL))
        {
            sprintf(str_EventData, "");
        }
      else
      {
          GetDataItem(&JournalErrorNumberDesc0, (CPU_INT08U*)str_EventNumber, record->event);
          GetDataItem(&JournalErrorNumberDesc1, (CPU_INT08U*)str_EventData, record->event);
      }
    }
  else
    { // пустая запись
      sprintf(str_EventNumber, "пусто");
      sprintf(str_EventData, "пусто");
    }
}

void OnEnterEventJournal(void)
{
  TEventRecord record;
  EventJournalIndex = 0;
  CPU_INT32U last = 0;
  
  for (CPU_INT32U i=0; i<EVENT_RECORDS_COUNT; i++)
    {
      GetEventRecord(&record, i);
      if (record.time >= last)
        {
          last = record.time;
          EventJournalIndex = i;
        }
    }
  if (!last) {SetMenu(JournalEmptyMenuPanel); return;}
    
  GetEventRecord(&record, EventJournalIndex);
  PrintEventJournalRecord(&record);
}

const TMenuLineArray arr_EventJournalMenuArray[] = {&line_EventJournalMenu_0, &line_EventJournalMenu_1, &line_EventJournalMenu_2, &line_EventJournalMenu_3 ,NULL};
const TMenuPanel EventJournalMenuPanel[] = {arr_EventJournalMenuArray, OnEnterEventJournal, 4, MENU_PANEL_STATIC};


/***********************************
  МЕНЮ НАСТРОЙКА ВРЕМЕНИ
***********************************/

const CPU_INT08U str_TimeSetupMenu_0[] = "УСТАНОВКА ВРЕМЕНИ";

const TMenuLine line_TimeSetupMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_TimeSetupMenu_0,        // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_TimeSetupMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&SystemTimeEditDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_TimeSetupMenuArray[] = {&line_TimeSetupMenu_0, &line_TimeSetupMenu_1, NULL};
const TMenuPanel TimeSetupMenuPanel[] = {arr_TimeSetupMenuArray, NULL, 2, MENU_PANEL_STANDARD};


/***********************************
  МЕНЮ Общая статистика
***********************************/
const CPU_INT08U str_CommonStatMenu_0[] = "  ОБЩАЯ СТАТИСТИКА";
const CPU_INT08U str_CommonStatMenu_1[] = "Короткие счетчики";
const CPU_INT08U str_CommonStatMenu_2[] = "Длинные счетчики";

const TMenuLine line_StatMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_CommonStatMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_StatMenu_1 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_CommonStatMenu_1,       // указатель на текстовую строку или дескриптор
  (void*)CommonCountersPanel                            // панель для перехода
};

const TMenuLine line_StatMenu_2 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_CommonStatMenu_2,        // указатель на текстовую строку или дескриптор
  (void*)CommonCountersLongPanel                           // панель для перехода
};

const TMenuLineArray arr_StatMenuArray[] = {&line_StatMenu_0, &line_StatMenu_1, &line_StatMenu_2, NULL};
const TMenuPanel CommStatMenuPanel[] = {arr_StatMenuArray, NULL, 3, MENU_PANEL_STANDARD};


/***********************************
  МЕНЮ Канальная статистика статистика
***********************************/
const CPU_INT08U str_ChanStatMenu_0[] = " СТАТ-КА ПО ПОСТАМ";

const TMenuLine line_ChanStatMenu_0 = {
  MENU_LINE_STRING,               // тип пункта меню
  MENU_FIXED_LINE,                // доп. флаги  
  (void*)str_ChanStatMenu_0,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChanStatMenu_1 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_CommonStatMenu_1,       // указатель на текстовую строку или дескриптор
  (void*)ChannelCountersPanel                            // панель для перехода
};

const TMenuLine line_ChanStatMenu_2 = {
  MENU_LINE_GOTO_MENU,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)str_CommonStatMenu_2,        // указатель на текстовую строку или дескриптор
  (void*)ChannelCountersLongPanel                           // панель для перехода
};

const TMenuLineArray arr_ChanStatMenuArray[] = {&line_ChanStatMenu_0, &line_ChanStatMenu_1, &line_ChanStatMenu_2, NULL};
const TMenuPanel ChanStatMenuPanel[] = {arr_ChanStatMenuArray, NULL, 3, MENU_PANEL_STANDARD};

/***********************************
  МЕНЮ КАНАЛЬНАЯ СТАТИСТИКА ДЛИННЫЕ СЧЕТЧИКИ
***********************************/
const TMenuLine line_ChannelCountersLongMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // тип пункта меню
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // доп. флаги  
  (void*)&ChannelStLongIndexDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelCountersLongMenu_1 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterChannelRunLongDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelCountersLongMenu_2 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterChannelMoneyLongDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLine line_ChannelCountersLongMenu_3 = {
  MENU_LINE_SHOW_DESC,               // тип пункта меню
  0,                              // доп. флаги  
  (void*)&CounterChannelTimeLongDesc,       // указатель на текстовую строку или дескриптор
  NULL                            // панель для перехода
};

const TMenuLineArray arr_ChannelCountersLongArray[] = {&line_ChannelCountersLongMenu_0, &line_ChannelCountersLongMenu_1, &line_ChannelCountersLongMenu_2, &line_ChannelCountersLongMenu_3, NULL};
const TMenuPanel ChannelCountersLongPanel[] = {arr_ChannelCountersLongArray, NULL, 4, MENU_PANEL_STATIC};
