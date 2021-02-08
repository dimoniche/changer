#ifndef _MENUDESC_H_
#define _MENUDESC_H_

#include "journal.h"

#define START_MENU      StartMenuPanel
#define WORK_MENU       FirstMenuPanel
#define SERVICE_MENU    ServiceMenuPanel

extern char FlagForPrintReport;
extern CPU_INT08U str_IncasMenu_3[32];

extern const TMenuPanel NetworkMenuPanel[];
extern const TMenuPanel ChannelCountersLongPanel[];
extern const TMenuPanel ChanStatMenuPanel[];
extern const TMenuPanel CommStatMenuPanel[];
extern const TMenuPanel MasterPassMenuPanel[];
extern const TMenuPanel IncasMenuPanel[];
extern const TMenuPanel CommonCountersLongPanel[];
extern const TMenuPanel BillCountersPanel[];
extern const TMenuPanel ModemSetupPanel[];
extern const TMenuPanel CoinSetupPanel[];
extern const TMenuPanel JournalIsReset[];
extern const TMenuPanel ClearJournalMenuPanel[];
extern const TMenuPanel StatIsReset[];
extern const TMenuPanel ClearStatMenu[];
extern const TMenuPanel SettingsIsReset[];
extern const TMenuPanel ResetSettingsMenuPanel[];
extern const TMenuPanel ErrorPassPanel[];
extern const TMenuPanel SetNewPassMenuPanel[];
extern const TMenuPanel SetPassMenuPanel[];
extern const TMenuPanel FrMenuPanel[];
extern const TMenuPanel FrIsOffMenuPanel[];
extern const TMenuPanel zReportMenuPanel[];
extern const TMenuPanel xReportMenuPanel[];
extern const TMenuPanel ReportMenuPanel[];
extern const TMenuPanel FirstMenuPanel[];
extern const TMenuPanel ServiceMenuPanel[];
extern const TMenuPanel StartMenuPanel[];
extern const TMenuPanel SettingsMenuPanel[];
extern const TMenuPanel ChannelMenuPanel[];
extern const TMenuPanel DeviceMenuPanel[];
extern const TMenuPanel GetMoneyMenuPanel[];
extern const TMenuPanel PriceWeekdaysMenuPanel[];
extern const TMenuPanel PriceWeekendMenuPanel[];
extern const TMenuPanel StatisticsMenuPanel[];
extern const TMenuPanel ErrorJournalMenuPanel[];
extern const TMenuPanel SelectJournalMenuPanel[];
extern const TMenuPanel TimeSetupMenuPanel[];
extern const TMenuPanel EventJournalMenuPanel[];
extern const TMenuPanel JournalEmptyMenuPanel[];
extern const TMenuPanel ChannelCountersPanel[];
extern const TMenuPanel CommonCountersPanel[];
extern const TMenuPanel bufReportMenuPanel[];
extern const TMenuPanel ValidatorSetupPanel[];

extern void PrintUserMenuStr(char* str, CPU_INT08U n);
extern void PrintEventJournalRecord(TEventRecord *record);

extern char str_EventNumber[24];
extern char str_EventData[24];


#endif //#ifndef _MENUDESC_H_
