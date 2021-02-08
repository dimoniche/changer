#include <includes.h>
#include <intrinsics.h>
#include "lcd.h"

const unsigned char rus_sym_table[64]=
                        { 0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA3,0xA4,
                         0xA5,0x03,0x4B,0xA7,0x4D,0x48,0x4F,0xA8,
                         0x50,0x43,0x54,0xA9,0xAA,0x58,0xE1,0xAB,
                         0xAC,0xE2,0xAD,0xAE,0x02,0xAF,0xB0,0xB1,
                         0x61,0xB2,0xB3,0xB4,0xE3,0x65,0xB6,0xB7,
                         0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x6F,0xBE,
                         0x70,0x63,0xBF,0x79,0xE4,0x78,0xE5,0xC0,
                         0xC1,0xE6,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7 };
  

void us_delay(unsigned long x)
{
  while (x--)
    {
      for (int i=0; i<5; i++) __no_operation();
    }
}


void LCD_putch(unsigned char data)
{
  if (data >= 0xC0) data = rus_sym_table[data-0xC0];
        
  LCD_SET_DATA_OUT();
  // LCD Upper 4 bits data
  LCD_SET_RS_PIN();   // RS = 1, E = 1
  LCD_SET_E_PIN();
  LCD_CLR_RW_PIN();

  us_delay(1); 
  LCD_OUT_DATA(data >> 4);
  us_delay(1); 

  // E=0; write data
  LCD_CLR_E_PIN();
  us_delay(1);       

  // LCD Lower 4 bits data
  LCD_SET_E_PIN();    // RS = 1, E = 1 
  us_delay(1); 
  LCD_OUT_DATA(data);
  us_delay(1); 

  // E=0; write data
  LCD_CLR_E_PIN();
  us_delay(100); 
//  OSTimeDly(1);     // Wait for busy flag (BF)
}

void LCD_putch_table(unsigned char data)
{
  LCD_SET_DATA_OUT();
  // LCD Upper 4 bits data
  LCD_SET_RS_PIN();   // RS = 1, E = 1
  LCD_SET_E_PIN();
  LCD_CLR_RW_PIN();

  us_delay(1); 
  LCD_OUT_DATA(data >> 4);
  us_delay(1); 

  // E=0; write data
  LCD_CLR_E_PIN();
  us_delay(1);       

  // LCD Lower 4 bits data
  LCD_SET_E_PIN();    // RS = 1, E = 1 
  us_delay(1); 
  LCD_OUT_DATA(data);
  us_delay(1); 

  // E=0; write data
  LCD_CLR_E_PIN();
  us_delay(100); 
//  OSTimeDly(1);     // Wait for busy flag (BF)
}

unsigned char LCD_getch()
{
  unsigned char data_h, data_l;
  LCD_SET_DATA_IN();
  // LCD Upper 4 bits data
  LCD_SET_RS_PIN();   // RS = 1, E = 1
  LCD_SET_E_PIN();
  LCD_SET_RW_PIN();

  us_delay(1);  
  data_h = LCD_IN_DATA();
  LCD_CLR_E_PIN();
  us_delay(1);       

  // LCD Lower 4 bits data
  LCD_SET_E_PIN();    // RS = 1, E = 1 
  us_delay(1); 
  data_l = LCD_IN_DATA();
  LCD_CLR_E_PIN();
  us_delay(100); 

  return (data_h << 4) | data_l;
}


unsigned char LCD_get_status()
{
  unsigned char data_h, data_l;
  LCD_SET_DATA_IN();
  // LCD Upper 4 bits data
  LCD_CLR_RS_PIN();   // RS = 0, E = 1
  LCD_SET_E_PIN();
  LCD_SET_RW_PIN();

  us_delay(1);  
  data_h = LCD_IN_DATA();
  LCD_CLR_E_PIN();
  us_delay(1);       

  // LCD Lower 4 bits data
  LCD_SET_E_PIN();    // RS = 0, E = 1 
  us_delay(1); 
  data_l = LCD_IN_DATA();
  LCD_CLR_E_PIN();
  us_delay(100); 

  return (data_h << 4) | data_l;
}

void LCD_putcmd(unsigned char data,unsigned char cmdtype)
{
  LCD_SET_DATA_OUT();
  // LCD Upper 4 bits data
  LCD_CLR_RS_PIN();   // RS = 0, E = 1
  LCD_SET_E_PIN();
  LCD_CLR_RW_PIN();
  us_delay(1); 
  LCD_OUT_DATA(data >> 4);
  us_delay(1); 
  // E=0; write data
  LCD_CLR_E_PIN();

  // cmdtype = 0; One cycle write, cmdtype = 1; Two cycle writes
  if (cmdtype == LCD_2CYCLE) {
    // LCD Lower 4 bits data
    us_delay(1);       
    LCD_SET_E_PIN();      // RS = 0, E = 1
    us_delay(1); 
    LCD_OUT_DATA(data);
    us_delay(1); 
    // E=0; write data
    LCD_CLR_E_PIN();
  
    us_delay(1); 
    
    //while(LCD_get_status() & 0x80);
    OSTimeDly(2);    // Wait for busy flag (BF)
    
    //LCD_get_status();
  }
}


void InitLcdPins()
{
  /*
P2.8	MK_P65	Сигнал A0 индикатора
P2.7	MK_P66	Сигнал E индикатора
P2.6	MK_P67	Сигнал R_W индикатора
P0.19	MK_P59	DB4
P0.20	MK_P58	DB5
P0.21	MK_P57	DB6
P0.22	MK_P56	DB7
  */
  #if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  
  PINSEL4_bit.P2_8 = 0x0;
  PINSEL4_bit.P2_7 = 0x0;
  PINSEL4_bit.P2_6 = 0x0;

  PINMODE4_bit.P2_8 = 0;
  PINMODE4_bit.P2_7 = 0;
  PINMODE4_bit.P2_6 = 0;
    
  FIO2DIR_bit.P2_8 = 1;
  FIO2DIR_bit.P2_7 = 1;
  FIO2DIR_bit.P2_6 = 1;

  FIO2MASK_bit.P2_8 = 0;
  FIO2MASK_bit.P2_7 = 0;
  FIO2MASK_bit.P2_6 = 0;
  
  PINSEL1_bit.P0_19 = 0x0;
  PINSEL1_bit.P0_20 = 0x0;
  PINSEL1_bit.P0_21 = 0x0;
  PINSEL1_bit.P0_22 = 0x0;
  
  PINMODE1_bit.P0_19 = 0;
  PINMODE1_bit.P0_20 = 0;
  PINMODE1_bit.P0_21 = 0;
  PINMODE1_bit.P0_22 = 0;
  
  FIO0DIR_bit.P0_19 = 1;
  FIO0DIR_bit.P0_20 = 1;
  FIO0DIR_bit.P0_21 = 1;
  FIO0DIR_bit.P0_22 = 1;

  FIO0MASK_bit.P0_19 = 0;
  FIO0MASK_bit.P0_20 = 0;
  FIO0MASK_bit.P0_21 = 0;
  FIO0MASK_bit.P0_22 = 0;

  LCD_CLR_E_PIN();
  LCD_CLR_RW_PIN();
  LCD_CLR_RS_PIN();
  
  OS_EXIT_CRITICAL();
}

unsigned char test = 0;

void InitLcd()
{
  InitLcdPins();
  
  // Wait for more than 15 ms after VCC rises to 4.5 V
  OSTimeDly(30);
  // Send Command 0x30
  LCD_putcmd(0x30,LCD_1CYCLE);
  us_delay(40); 
  // Send Command 0x30
  LCD_putcmd(0x30,LCD_1CYCLE);
  us_delay(40); 
  // Send Command 0x30
  LCD_putcmd(0x30,LCD_1CYCLE);
  us_delay(40); 
  // Function set: Set interface to be 4 bits long (only 1 cycle write).
  LCD_putcmd(0x20,LCD_1CYCLE);
  us_delay(40); 
  // Function set: DL=0;Interface is 4 bits, N=1; 2 Lines, F=0; 5x8 dots font)
  LCD_putcmd(0x28,LCD_2CYCLE);
  // Display Off: D=0; Display off, C=0; Cursor Off, B=0; Blinking Off
  LCD_putcmd(0x08,LCD_2CYCLE);
  // Display Clear
  LCD_putcmd(0x01,LCD_2CYCLE);
  // Entry Mode Set: I/D=1; Increment, S=0; No shift
  LCD_putcmd(0x06,LCD_2CYCLE);
  // Display On, Cursor Off
  LCD_putcmd(0x0C,LCD_2CYCLE);      

  
}

void LCD_gotoline(unsigned char n)
{
   switch (n){
     case 0: LCD_putcmd(LCD_GOTO_LINE_0, LCD_2CYCLE); break;
     case 1: LCD_putcmd(LCD_GOTO_LINE_1, LCD_2CYCLE); break;
     case 2: LCD_putcmd(LCD_GOTO_LINE_2, LCD_2CYCLE); break;
     case 3: LCD_putcmd(LCD_GOTO_LINE_3, LCD_2CYCLE); break;
     default: return;
   }
}

// запись строки с номером n
void LCD_puts(unsigned char *s, unsigned char n)
{
   unsigned char i;
   LCD_gotoline(n);
     
   i=0;
   while((*s != 0) && (*s != '\n') && (i<20))
   {
     LCD_putch(*s);
     s++;
     i++;
   }
   
   while(i<20)
   {
     LCD_putch(' ');
     i++;
   }
}


void LCD_goto(unsigned char m, unsigned char n)
{
   switch (n){
     case 0: LCD_putcmd(LCD_GOTO_LINE_0+m, LCD_2CYCLE); break;
     case 1: LCD_putcmd(LCD_GOTO_LINE_1+m, LCD_2CYCLE); break;
     case 2: LCD_putcmd(LCD_GOTO_LINE_2+m, LCD_2CYCLE); break;
     case 3: LCD_putcmd(LCD_GOTO_LINE_3+m, LCD_2CYCLE); break;
     default: return;
   }
}

// запись символа в строку с номером n в позицию m
void LCD_putc(unsigned char c, unsigned char m, unsigned char n)
{
   LCD_goto(m, n);
   LCD_putch(c);
}

// запись символа в строку с номером n в позицию m
void LCD_putc_embed(unsigned char c, unsigned char m, unsigned char n)
{
   LCD_goto(m, n);
   LCD_putch_table(c);
}


void LCD_clear(void)
{
  // Display Clear
  LCD_putcmd(0x01,LCD_2CYCLE);
}

void LCD_cursor_on(void)
{
  LCD_putcmd(0x0F,LCD_2CYCLE);    
}

void LCD_cursor_off(void)
{
  LCD_putcmd(0x0C,LCD_2CYCLE);    
}

