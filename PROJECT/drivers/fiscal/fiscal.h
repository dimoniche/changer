#ifndef _FISCAL_H_
#define _FISCAL_H_

extern CPU_INT08U  FiscalState;
#define FISCAL_NOCONN 0
#define FISCAL_CONN   1

// коды служебгых символов
#define FISC_ENQ    0x05
#define FISC_STX    0x02
#define FISC_ACK    0x06
#define FISC_NACK   0x15

// скорость передачи по умолчанию
#define FISC_SPEED      115200

// таймаут ответа по умолчанию
#define FISC_TIMEOUT              105
#define FISC_BYTE_TIMEOUT         10
#define FISC_ANSWER_TIMEOUT       5000

// коды команд ФР
#define FISC_GET_FACTORY_NUMBER   0x0f
#define FISC_GET_SHORT_STATUS     0x10
#define FISC_GET_FULL_STATUS      0x11
#define FISC_PRINT_BOLD_STRING    0x12
        #define FISC_CONTROL_TAPE    0x01
        #define FISC_BILL_TAPE       0x02
        #define FISC_UNDERLAY_DOC    0x04
#define FISC_BEEP                 0x13
#define FISC_PRINT_STRING         0x17
#define FISC_PRINT_DOC_HEADER     0x18
#define FISC_GET_MONEY_REG        0x1A
#define FISC_GET_OPER_REG         0x1B
#define FISC_BILL_CUT             0x25
        #define FISC_FULL_CUT     0
        #define FISC_HALF_CUT     1
#define FISC_PULL_OUT_TAPE        0x29
#define FISC_EJECT_UNDERLAY_DOC   0x2A
        #define FISC_EJECT_DOWN   0
        #define FISC_EJECT_UP     1
#define FISC_PRINT_STRING_BY_FONT 0x2F
#define FISC_PRINT_DAY_REPORT_NO_CLEAR   0x40
#define FISC_PRINT_DAY_REPORT_CLEAR      0x41
#define FISC_PRINT_SECTION_REPORT        0x42
#define FISC_PRINT_TAXES_REPORT          0x43
#define FISC_MAKE_DEPOSIT          0x50
#define FISC_MAKE_PAYOUT           0x51
#define FISC_PRINT_CLICHE          0x52
#define FISC_END_DOC               0x53
        #define FISC_WITHOUT_ADV   0 // с рекламой 
        #define FISC_WITH_ADV      1 // без рекламы
#define FISC_PRINT_ADV_TEXT        0x54
#define FISC_GET_DEVICE_TYPE       0xFC

#define FISC_OPEN_BILL           0x8D
  #define FISC_BILL_SELL  0
  #define FISC_BILL_BUY   1

#define FISC_MAKE_SELL           0x80
#define FISC_CLOSE_BILL          0x85

#define FISC_EXTENDED_CMD           0xFF

#define FISC_OPEN_DAY            0xE0
#define FISC_PRINT_CONTINUE      0xB0

#define FISC_PRINT_DAY_REPORT_TO_BUF      0xC6
#define FISC_PRINT_DAY_REPORT_FROM_BUF    0xC7

// переопределение функций передачи
#define FiscPurgeRx           Uart0_Flush
#define FiscUartSend          Uart0_Send
#define FiscUartRecieve       Uart0_Receive
#define FiscUartRecieveByte   Uart0_RdByteWithTimeOut
#define FiscUartSendByte      Uart0_WrByte
#define FiscSleep(x)          OSTimeDly(x)

// ответ на проверку статуса FiscPoll()
#define FISC_READY   0
#define FISC_BUSY   -1
#define FISC_UNDEF  -2

// коды результатов функций
#define FISC_OK      0
#define FISC_ERR    -1

// коды ошибок ФР
#define FR_ERROR_CODE_1     0x1   //Неизвестная команда
#define FR_ERROR_CODE_2     0x2   //Неверное состояние ФН 
#define FR_ERROR_CODE_3     0x3   //Ошибка ФН 
#define FR_ERROR_CODE_4     0x4   //Ошибка КС
#define FR_ERROR_CODE_5     0x5   //Закончен срок эксплуатации ФН
#define FR_ERROR_CODE_6     0x6   //Архив ФН переполнен
#define FR_ERROR_CODE_7     0x7   //Неверные дата и/или время 
#define FR_ERROR_CODE_8     0x8   //Нет запрошенных данных 
#define FR_ERROR_CODE_9     0x9   //Некорректное значение параметров команды
#define FR_ERROR_CODE_10    0x10  //Превышение размеров TLV данных
#define FR_ERROR_CODE_11    0x11  //Нет транспортного соединения
#define FR_ERROR_CODE_12    0x12  //Исчерпан ресурс КС
#define FR_ERROR_CODE_14    0x14  //Исчерпан ресурс хранения
#define FR_ERROR_CODE_15    0x15  //Исчерпан ресурс Ожидания передачи сообщения
#define FR_ERROR_CODE_16    0x16  //Продолжительность смены более 24 часов
#define FR_ERROR_CODE_17    0x17  //Неверная разница во времени между 2 операцими 
#define FR_ERROR_CODE_20    0x20  //Сообщение от ОФД не может быть принято
#define FR_ERROR_CODE_2F    0x2F  //Таймаут обмена с ФН
#define FR_ERROR_CODE_30    0x30  //ФН не отвечает
#define FR_ERROR_CODE_33    0x33  //Некорректные параметры в команде
#define FR_ERROR_CODE_34    0x34  //Нет данных
#define FR_ERROR_CODE_35    0x35  //Некорректный параметр при данных настройках 
#define FR_ERROR_CODE_36    0x36  //Некорректные параметры в команде для данной реализации ККТ
#define FR_ERROR_CODE_37    0x37  //Команда не поддерживается в данной реализации ККТ
#define FR_ERROR_CODE_38    0x38  //Ошибка в ПЗУ
#define FR_ERROR_CODE_39    0x39  //Внутренняя ошибка ПО ККТ
#define FR_ERROR_CODE_3a    0x3A  //Переполнение накопления по надбавкам в смене
#define FR_ERROR_CODE_3c    0x3C  //Смена открыта – операция не возможна
#define FR_ERROR_CODE_3D    0x3D  //Смена не открыта – операция не возможна 
#define FR_ERROR_CODE_3E    0x3E  //Переполнение накопления по секциям в смене
#define FR_ERROR_CODE_3F    0x3F  //Переполнение накопления по скидкам в смене
#define FR_ERROR_CODE_40    0x40  //Переполнение диапазона скидок
#define FR_ERROR_CODE_41    0x41  //Переполнение диапазона оплаты наличными
#define FR_ERROR_CODE_42    0x42  //Переполнение диапазона оплаты типом 2
#define FR_ERROR_CODE_43    0x43  //Переполнение диапазона оплаты типом 3
#define FR_ERROR_CODE_44    0x44  //Переполнение диапазона оплаты типом 4
#define FR_ERROR_CODE_45    0x45  //Cумма всех типов оплаты меньше итога чека
#define FR_ERROR_CODE_46    0x46  //Не хватает наличности в кассе
#define FR_ERROR_CODE_47    0x47  //Переполнение накопления по налогам в смене
#define FR_ERROR_CODE_48    0x48  //Переполнение итога чека
#define FR_ERROR_CODE_49    0x49  //Операция невозможна в открытом чеке данного типа
#define FR_ERROR_CODE_4A    0x4A  //Открыт чек - операция невозможна
#define FR_ERROR_CODE_4B    0x4B  //Буфер чека переполнен
#define FR_ERROR_CODE_4C    0x4C  //Переполнение накопления по обороту налогов в смене
#define FR_ERROR_CODE_4D    0x4D  //Вносимая безналичной оплатой сумма больше суммы чека
#define FR_ERROR_CODE_4E    0x4E  //Смена превысила 24 часа
#define FR_ERROR_CODE_4F    0x4F  //Неверный пароль
#define FR_ERROR_CODE_50    0x50  //Идет печать предыдущей команды
#define FR_ERROR_CODE_51    0x51  //Переполнение накоплений наличными в смене
#define FR_ERROR_CODE_52    0x52  //Переполнение накоплений по типу оплаты 2 в смене
#define FR_ERROR_CODE_53    0x53  //Переполнение накоплений по типу оплаты 3 в смене
#define FR_ERROR_CODE_54    0x54  //Переполнение накоплений по типу оплаты 4 в смене
#define FR_ERROR_CODE_55    0x55  //Чек закрыт – операция невозможна
#define FR_ERROR_CODE_56    0x56  //Нет документа для повтора
#define FR_ERROR_CODE_58    0x58  //Ожидание команды продолжения печати
#define FR_ERROR_CODE_59    0x59  //Документ открыт другим оператором
#define FR_ERROR_CODE_5B    0x5B  //Переполнение диапазона надбавок
#define FR_ERROR_CODE_5C    0x5C  //Понижено напряжение 24В
#define FR_ERROR_CODE_5D    0x5D  //Таблица не определена
#define FR_ERROR_CODE_5E    0x5E  //Некорректная операция
#define FR_ERROR_CODE_5F    0x5F  //Отрицательный итог чека
#define FR_ERROR_CODE_60    0x60  //Переполнение при умножении
#define FR_ERROR_CODE_61    0x61  //Переполнение диапазона цены
#define FR_ERROR_CODE_62    0x62  //Переполнение диапазона количества
#define FR_ERROR_CODE_63    0x63  //Переполнение диапазона отдела
#define FR_ERROR_CODE_65    0x65  //Не хватает денег в секции
#define FR_ERROR_CODE_66    0x66  //Переполнение денег в секции
#define FR_ERROR_CODE_68    0x68  //Не хватает денег по обороту налогов
#define FR_ERROR_CODE_69    0x69  //Переполнение денег по обороту налогов
#define FR_ERROR_CODE_6A    0x6A  //Ошибка питания в момент ответа по I2C
#define FR_ERROR_CODE_6B    0x6B  //Нет чековой ленты 
#define FR_ERROR_CODE_6C    0x6C  //Нет контрольной ленты
#define FR_ERROR_CODE_6D    0x6D  //Не хватает денег по налогу
#define FR_ERROR_CODE_6E    0x6E  //Переполнение денег по налогу
#define FR_ERROR_CODE_6F    0x6F  //Переполнение по выплате в смене
#define FR_ERROR_CODE_71    0x71  //Ошибка отрезчика
#define FR_ERROR_CODE_72    0x72  //Команда не поддерживается в данном подрежиме
#define FR_ERROR_CODE_73    0x73  //Команда не поддерживается в данном режиме
#define FR_ERROR_CODE_74    0x74  //Ошибка ОЗУ
#define FR_ERROR_CODE_75    0x75  //Ошибка питания
#define FR_ERROR_CODE_77    0x77  //Ошибка принтера: нет сигнала с датчиков
#define FR_ERROR_CODE_78    0x78  //Замена ПО
#define FR_ERROR_CODE_7A    0x7A  //Поле не редактируется 
#define FR_ERROR_CODE_7B    0x7B  //Ошибка оборудования
#define FR_ERROR_CODE_7C    0x7C  //Не совпадает дата
#define FR_ERROR_CODE_7D    0x7D  //Неверный формат даты
#define FR_ERROR_CODE_7E    0x7E  //Неверное значение в поле длины
#define FR_ERROR_CODE_7F    0x7F  //Переполнение диапазона итога чека
#define FR_ERROR_CODE_84    0x84  //Переполнение наличности
#define FR_ERROR_CODE_85    0x85  //Переполнение по продажам в смене
#define FR_ERROR_CODE_86    0x86  //Переполнение по покупкам в смене
#define FR_ERROR_CODE_87    0x87  //Переполнение по возвратам продаж в смене
#define FR_ERROR_CODE_88    0x88  //Переполнение по возвратам покупок в смене
#define FR_ERROR_CODE_89    0x89  //Переполнение по внесению в смене
#define FR_ERROR_CODE_8A    0x8A  //Переполнение по надбавкам в чеке
#define FR_ERROR_CODE_8B    0x8B  //Переполнение по скидкам в чеке
#define FR_ERROR_CODE_8C    0x8C  //Отрицательный итог надбавки в чеке
#define FR_ERROR_CODE_8D    0x8D  //Отрицательный итог скидки в чеке
#define FR_ERROR_CODE_8E    0x8E  //Нулевой итог чека
#define FR_ERROR_CODE_90    0x90  //Поле превышает размер, установленный в настройках
#define FR_ERROR_CODE_91    0x91  //Выход за границу поля печати при данных настройках шрифта 
#define FR_ERROR_CODE_92    0x92  //Наложение полей 
#define FR_ERROR_CODE_93    0x93  //Восстановление ОЗУ прошло успешно 
#define FR_ERROR_CODE_94    0x94  //Исчерпан лимит операций в чеке 
#define FR_ERROR_CODE_C0    0xC0  //Контроль даты и времени (подтвердите дату и время) 
#define FR_ERROR_CODE_C2    0xC2  //Превышение напряжения в блоке питания 
#define FR_ERROR_CODE_C4    0xC4  //Несовпадение номеров смен 
#define FR_ERROR_CODE_C7    0xC7  //Поле не редактируется в данном режиме 
#define FR_ERROR_CODE_C8    0xC8  //Отсутствуют импульсы от таходатчика 

#define FR_ERROR_NUMBER   109

// структура заводского номера и РНМ
#pragma pack(1)
typedef struct{
  CPU_INT08U FactoryNumber[7];
  CPU_INT08U RNM[7];
}TFiscFactoryNumber;

// структура короткого состояния
#pragma pack(1)
typedef struct{
  CPU_INT08U OperatorNumber; // номер оператора 1..30
  CPU_INT16U Flags; // флаги ФР
  /*
   Флаги ФР  Битовое поле (назначение бит): 
      0 – Рулон операционного журнала (0 – нет, 1 – есть) 
      1 – Рулон чековой ленты (0 – нет, 1 – есть) 
      2 – Верхний датчик подкладного документа (0 – нет, 1 – да) 
      3 – Нижний датчик подкладного документа (0 – нет, 1 – да) 
      4 – Положение десятичной точки (0 – 0 знаков, 1 – 2 знака) 
      5 – ЭКЛЗ (0 – нет, 1 – есть) 
      6 – Оптический датчик операционного журнала (0 – бумаги нет, 1 – бумага есть) 
      7 – Оптический датчик чековой ленты (0 – бумаги нет, 1 – бумага есть) 
      8 – Рычаг термоголовки контрольной ленты (0 – поднят, 1 – опущен) 
      9 – Рычаг термоголовки чековой ленты (0 – поднят, 1 – опущен) 
      10 – Крышка корпуса ФР (0 – опущена, 1 – поднята) 
      11 – Денежный ящик (0 – закрыт, 1 – окрыт) 
      12а – Отказ правого датчика принтера (0 – нет, 1 – да) 
      12б – Бумага на входе в презентер (0 – нет, 1 – да) 
      13а – Отказ левого датчика принтера (0 – нет, 1 – да) 
      13б – Бумага на выходе из презентера (0 – нет, 1 – да) 
      14 – ЭКЛЗ почти заполнена (0 – нет, 1 – да) 
      15а – Увеличенная точность количества (0 – нормальная точность, 1 – увеличенная 
      точность) [для ККМ без ЭКЛЗ]. Для ККМ с ЭКЛЗ (1 – нормальная точность, 0 – 
      увеличенная точность) 
      15б – Буфер принтера непуст (0 – пуст, 1 – непуст) 
  */
  CPU_INT08U Mode;  // режим ФР
  CPU_INT08U SubMode;  // подрежим ФР
  CPU_INT08U BillOperationNumber_L; // кол-во опреаций в чеке, мл. байт
  CPU_INT08U BatteryVoltage;  // напряжение резервной батареи
  CPU_INT08U PowerSupplyVoltage;  // напряжение источника питания
  CPU_INT08U ErrorCodeFP; // код ошибки ФП
  CPU_INT08U ErrorCodeEKLZ; // код ошибки ЭКЛЗ
  CPU_INT08U BillOperationNumber_H; // кол-во опреаций в чеке, ст. байт
  CPU_INT08U Reserve[3]; // резерв  
}TFiscShortStatus;

// структура полного состояния
#pragma pack(1)
typedef struct{
/*   
Порядковый номер оператора (1 байт) 1…30 
Версия ПО ФР (2 байта) 
Сборка ПО ФР (2 байта) 
Дата ПО ФР (3 байта) ДД-ММ-ГГ 
Номер в зале (1 байт) 
Сквозной номер текущего документа (2 байта) 
Флаги ФР (2 байта) 
Режим ФР (1 байт) 
Подрежим ФР (1 байт) 
Порт ФР (1 байт) 
Версия ПО ФП (2 байта) 
Сборка ПО ФП (2 байта) 
Дата ПО ФП (3 байта) ДД-ММ-ГГ 
Дата (3 байта) ДД-ММ-ГГ 
Время (3 байта) ЧЧ-ММ-СС 
Флаги ФП (1 байт) 
Битовое поле (назначение бит): 
0 – ФП 1 (0 –нет, 1 – есть) 
1 – ФП 2 (0 –нет, 1 – есть) 
2 – Лицензия (0 – не введена, 1 – введена) 
3 – Переполнение ФП (0 – нет, 1 – есть) 
4 – Батарея ФП (0 – >80%, 1 – <80%) 
5 – Последняя запись ФП (0 – испорчена, 1 – корректна) 
6 – Смена в ФП (0 – закрыта, 1 – открыта) 
7 – 24 часа в ФП (0 – не кончились, 1 – кончились) 
Заводской номер (4 байта) 
Номер последней закрытой смены (2 байта) 
Количество свободных записей в ФП (2 байта) 
Количество перерегистраций (фискализаций) (1 байт) 
Количество оставшихся перерегистраций (фискализаций) (1 байт) 
  ИНН (6 байт) 
*/    
    CPU_INT08U OperatorNumber; // номер оператора 1..30
    CPU_INT08U version[4];
    CPU_INT08U version_date[3];
    CPU_INT08U nomer_v_zale;
    CPU_INT16U document_number;
    CPU_INT16U Flags; // флаги ФР
    CPU_INT08U Mode;  // режим ФР
    CPU_INT08U SubMode;  // подрежим ФР    
    CPU_INT08U port;
    CPU_INT08U ver_fp[4];
    CPU_INT08U date_fp[3];
    CPU_INT08U date[3];
    CPU_INT08U time[3];
    CPU_INT08U fp_flags;
    CPU_INT32U FactoryNumber;
    CPU_INT16U LastSmena;
    CPU_INT16U FpFreePlace;
    CPU_INT08U perereg;
    CPU_INT08U perereg_free;
    CPU_INT08U inn[6];

}TFiscFullStatus;

// структура информации об устройстве
#pragma pack(1)
typedef struct{ 
  CPU_INT08U Type;                // Тип устройства (1 байт) 0…255 
  CPU_INT08U Subtype;             // Подтип устройства (1 байт) 0…255 
  CPU_INT08U ProtocolVersion;     // Версия протокола для данного устройства (1 байт) 0…255 
  CPU_INT08U ProtocolSubVersion;  // Подверсия протокола для данного устройства (1 байт) 0…255 
  CPU_INT08U Model;               // Модель устройства (1 байт) 0…255 
  CPU_INT08U Language;            // Язык устройства (1 байт) 0…255 русский – 0; английский – 1; 
      #define FISC_LANG_RUS   0
      #define FISC_LANG_ENG   1
  CPU_INT08U Name[32];            // Название устройства – строка символов в кодировке WIN1251. Количество 
                                  // байт, отводимое под название устройства, определяется в каждом конкретном 
                                  // случае самостоятельно разработчиками устройства (X байт) 
}TFiscDevType;
extern TFiscDevType FiscDevInfo;

// функции
extern int FiscPoll(void);
extern int FiscGetFactoryNumber(CPU_INT32U pass, TFiscFactoryNumber* fnum, CPU_INT08U* err);
extern int FiscGetShortStatus(CPU_INT32U pass, TFiscShortStatus* stat, CPU_INT08U* err);
extern int FiscGetFullStatus(CPU_INT32U pass, TFiscFullStatus* stat, CPU_INT08U* err);
extern int FiscPrintBoldString(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U* str, CPU_INT08U* err);
extern int FiscBeep(CPU_INT32U pass, CPU_INT08U* err);
extern int FiscPrintString(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U* str, CPU_INT08U* err);
extern int FiscPrintDocHeader(CPU_INT32U pass, CPU_INT08U* str, CPU_INT16U number, CPU_INT08U* err);
extern int FiscGetMoneyReg(CPU_INT32U pass, CPU_INT08U reg, CPU_INT64U* value, CPU_INT08U* err);
extern int FiscGetOperReg(CPU_INT32U pass, CPU_INT08U reg, CPU_INT16U* value, CPU_INT08U* err);
extern int FiscBillCut(CPU_INT32U pass, CPU_INT08U type, CPU_INT08U* err);
extern int FiscPullOutTape(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U strnum, CPU_INT08U* err);
extern int FiscEjectUnderlayDoc(CPU_INT32U pass, CPU_INT08U dir, CPU_INT08U* err);
extern int FiscPrintStringByFont(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U font, CPU_INT08U* str, CPU_INT08U* err);
extern int FiscPrintDayReportNoClear(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscPrintDayReportClear(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscPrintSectionReport(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscPrintTaxesReport(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscMakeDeposit(CPU_INT32U pass, CPU_INT32U sum, CPU_INT16U* doc, CPU_INT08U* err);
extern int FiscMakePayout(CPU_INT32U pass, CPU_INT32U sum, CPU_INT16U* doc, CPU_INT08U* err);
extern int FiscPrintCliche(CPU_INT32U pass, CPU_INT08U* err);
extern int FiscEndDoc(CPU_INT32U pass, CPU_INT08U param, CPU_INT08U* err);
extern int FiscPrintAdvText(CPU_INT32U pass, CPU_INT08U* err);
extern int FiscGetDeviceType(TFiscDevType* dev, CPU_INT08U* err);
extern int FiscPrintContinue(CPU_INT32U pass, CPU_INT08U* err);

extern int FiscOpenBill(CPU_INT32U pass, CPU_INT08U type, CPU_INT08U* err);
extern int FiscMakeSell(CPU_INT32U pass, CPU_INT64U *count, CPU_INT64U *price, CPU_INT08U department, CPU_INT08U* tax, char* text, CPU_INT08U* err);
extern int FiscCloseBill(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U* tax, char* text, CPU_INT08U* err);

extern int FiscPrintDayReportToBuf(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscPrintDayReportsFromBuf(CPU_INT32U admpass, CPU_INT08U* err);

extern int FiscPollExt(void);
extern int FiscOpenDay(CPU_INT32U pass, CPU_INT08U* err);

extern int FiscMakeSellV2(CPU_INT32U pass, CPU_INT64U *count, CPU_INT64U *price, CPU_INT08U department, CPU_INT08U* tax, CPU_INT08U subj, char* text, CPU_INT08U* err);
extern int FiscCloseBillV2(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U taxsys, char* text, CPU_INT08U* err);

#endif //#ifndef _FISCAL_H_
