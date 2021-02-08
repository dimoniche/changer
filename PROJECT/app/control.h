#ifndef _CONTROL_H_
#define _CONTROL_H_

#define CHANNELS_NUM    6

#define CHANNEL_1    0
#define CHANNEL_2    1
#define CHANNEL_3    2
#define CHANNEL_4    3
#define CHANNEL_5    4
#define CHANNEL_6    5
#define CHANNEL_7    6
#define CHANNEL_8    7
#define CHANNEL_9    8
#define CHANNEL_10   9


extern void InitChannels(void);
extern void ChannelOn(CPU_INT08U ch);
extern void ChannelOff(CPU_INT08U ch);
extern int IsChannelOn(CPU_INT08U ch);


#endif //#ifndef _CONTROL_H_
