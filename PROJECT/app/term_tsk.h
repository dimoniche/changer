#ifndef _TERM_TSK_H_
#define _TERM_TSK_H_

#include <stdint.h>

#define TERM_PARAM_COUNT    1
#define TERM_BUFFER_SIZE    2048
#define TERM_SOCKET_CONNECT_TIMEOUT     1000
#define TERM_READ_PACKET_TIMEOUT        3000


/** \name —осто€ни€ терминала
*/
/**{*/
    #define TERM_STATE_IDLE             0
    #define TERM_STATE_PENDING_PUR      1
    #define TERM_STATE_WAITING_PUR      2
    #define TERM_STATE_ABORTING         3
    #define TERM_STATE_DONE_PUR         4
    #define TERM_STATE_ERR_PUR          5
/**}*/


/** \name  оманды терминала
*/
/**{*/
    #define TERM_COMMAND_NONE           0x0000
    #define TERM_COMMAND_PEND_PUR       0x0001
    #define TERM_COMMAND_ABORT_PUR      0x0002
    #define TERM_COMMAND_CLEAR_STATE    0x0004
    #define TERM_COMMAND_MAKE_SVERKA    0x0008
    #define TERM_COMMAND_MAKE_SYNCHRO   0x0010
/**}*/


/// структура дл€ сохранени€ информции о текущем запросе
typedef struct
{
    /// номер кассового документа
    uint32_t ern;

    /// врем€ последней сверки
    uint32_t time_sverka;

    /// врем€ последней синхронизации журнала
    uint32_t time_syn;

    /// признак активного текущего запроса
    uint8_t request_active;

    /// контрольна€ сумма
    uint16_t crc16;
}TerminalCurrInfo;



extern void InitTerminalApp();
extern uint32_t GetTermState(void);
extern uint32_t SetTermCommand(uint32_t cmd, uint32_t* param);
extern int ttk2_get_field_string(uint16_t field, char* str, uint16_t maxlen);
extern CPU_INT32U term_protocol(void);

#endif //_TERM_TSK_H_
