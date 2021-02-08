#ifndef __LCD_H__
#define __LCD_H__

/*!
  Драйвер дисплея
  контроллер дисплея ks066, 4 строки

*/

#define LCD_HOME        0x02
#define LCD_CLEAR       0x01
#define LCD_GOTO_LINE_0   0x80
#define LCD_GOTO_LINE_1   0xC0
#define LCD_GOTO_LINE_2   0x94
#define LCD_GOTO_LINE_3   0xD4
#define LCD_1CYCLE      0
#define LCD_2CYCLE      1


#define LCD_PORT PORTB
 #define  D4_pin  PB4
 #define  D5_pin  PB5
 #define  D6_pin  PB6
 #define  D7_pin  PB7

#define LCD_SET_E_PIN()   {FIO2SET_bit.P2_7 = 1;}
#define LCD_CLR_E_PIN()   {FIO2CLR_bit.P2_7 = 1;}

#define LCD_SET_RW_PIN()   {FIO2SET_bit.P2_6 = 1;}
#define LCD_CLR_RW_PIN()   {FIO2CLR_bit.P2_6 = 1;}

#define LCD_SET_RS_PIN()   {FIO2SET_bit.P2_8 = 1;}
#define LCD_CLR_RS_PIN()   {FIO2CLR_bit.P2_8 = 1;}

#define LCD_OUT_DATA(x)   {FIO0PIN &= ~(0x0fL << 19); FIO0PIN |= (((x) & 0xf) << 19);}
#define LCD_IN_DATA()   ((FIO0PIN >> 19) & 0x0f)
#define LCD_SET_DATA_IN()   {FIO0DIR &= ~(0x0fL << 19);}
#define LCD_SET_DATA_OUT()   {FIO0PIN |= (0x0fL << 19);}


extern void InitLcd();
extern void LCD_puts(unsigned char *s, unsigned char n);
extern void LCD_putc(unsigned char c, unsigned char n, unsigned char m);
extern void LCD_clear(void);
extern void LCD_putc_embed(unsigned char c, unsigned char n, unsigned char m);
extern void LCD_cursor_on(void);
extern void LCD_cursor_off(void);
extern void LCD_goto(unsigned char n, unsigned char m);

#endif //#ifndef __LCD_H__
