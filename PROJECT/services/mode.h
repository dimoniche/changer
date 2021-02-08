#ifndef _MODE_H_
#define _MODE_H_

#include "cpu.h"

// определения режимов
#define MODE_WORK     0
#define MODE_SERVICE  1

// получить текущий режим работы
extern CPU_INT08U GetMode(void);
// проверить, не изменился ли режим работы
extern CPU_INT08U CheckMode(void);
// инициализация
extern void InitMode(void);

#endif //#ifndef _MODE_H_
