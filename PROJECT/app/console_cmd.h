#ifndef _CONSOLE_CMD_H_
#define _CONSOLE_CMD_H_

#include <includes.h>
#include <stdint.h>

#ifdef BOARD_POST_CFG
    #define PARAM_COUNT    9
#else
    #define PARAM_COUNT    10
#endif

///
typedef struct
{
    char const* name;
    int (*set)(char*);
    int (*get)(char*);
} ParamDesc;


extern int CheckInit(void);
extern void InitConfigDefault(int reset_ctr);
extern int SetParam(char *cmd);
extern int GetParam(char *cmd, char *answer, uint16_t len);

#endif // _CONSOLE_CMD_H_
