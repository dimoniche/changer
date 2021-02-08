#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_


// кнопки не нажаты
#define KEY_EMPTY     0
// кнопки клавиатуры
#define KEY_F1        1
#define KEY_F2        2
#define KEY_F3        3
#define KEY_LEFT      4
#define KEY_UP        5
#define KEY_RIGHT     6
#define KEY_STOP      7
#define KEY_DOWN      8
#define KEY_START     9
// кнопка пользователя
#define KEY_USER_START      10

#define KEY_COUNT           11

#define LED_OK_ON()  {FIO1SET_bit.P1_21 = 1;}
#define LED_OK_OFF() {FIO1CLR_bit.P1_21 = 1;}
            
extern void InitKbrd();
extern int GetKbrdEvent(int* event);

#endif //#ifndef _KEYBOARD_H_
