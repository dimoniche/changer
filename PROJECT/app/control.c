#include <includes.h>
#include "control.h"
#include "data.h"
#include "datadesc.h"
#include <stdlib.h>
#include <string.h>


CPU_INT16U ChannelStatus; // побитно текущие статусы каналов

// включить канал
void ChannelOn(CPU_INT08U ch)
{
  ChannelStatus |= (1L << ch);
}

// выключить канал
void ChannelOff(CPU_INT08U ch)
{
  ChannelStatus &= ~(1L << ch);
}



// инициализация каналов при старте системы
void InitChannels(void)
{
    ChannelStatus = 0;
}


