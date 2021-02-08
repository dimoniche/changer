#include <includes.h>
#include "control.h"
#include "data.h"
#include "datadesc.h"
#include <stdlib.h>
#include <string.h>


CPU_INT16U ChannelStatus; // ������� ������� ������� �������

// �������� �����
void ChannelOn(CPU_INT08U ch)
{
  ChannelStatus |= (1L << ch);
}

// ��������� �����
void ChannelOff(CPU_INT08U ch)
{
  ChannelStatus &= ~(1L << ch);
}



// ������������� ������� ��� ������ �������
void InitChannels(void)
{
    ChannelStatus = 0;
}


