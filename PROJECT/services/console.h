#ifndef _CONSOLE_H_
#define _CONSOLE_H_



// структура для описания команды
typedef struct
{
   const char* opt;           /* the option name */
   int (*func)(char * buf);   /* routine name to execute the option */
   const char* desc;             /* description of the option */
}TMenuOpt;



extern void InitConsole(void);

#endif //#ifndef _CONSOLE_H_
