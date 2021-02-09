#include <includes.h>
#include <intrinsics.h>
#include "lcd.h"
#include "symtab.h"


unsigned char lcd_x = 0, lcd_y = 0;

#ifdef CONFIG_LCD_1602A
    // максимальное число пользовательских знаков в знакогенераторе
    #define LCD_MAX_CHAR_SYG    8   
    // список текущих символов в знакогенераторе
    unsigned char current_rus_syg[LCD_MAX_CHAR_SYG] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char current_syg_index = 0;
    unsigned char lcd_lines[2][16];
    void LCD_putcmd(unsigned char data,unsigned char cmdtype);
#endif


const unsigned char rus_sym_table[64]=
                        { 0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA3,0xA4,
                         0xA5,0x03,0x4B,0xA7,0x4D,0x48,0x4F,0xA8,
                         0x50,0x43,0x54,0xA9,0xAA,0x58,0xE1,0xAB,
						 #ifdef CONFIG_LCD_1602A
                         0xAC,0xE2,0xAD,0xAE,'b',0xAF,0xB0,0xB1,
					 	 #else
                         0xAC,0xE2,0xAD,0xAE,0x02,0xAF,0xB0,0xB1,
						 #endif
                         0x61,0xB2,0xB3,0xB4,0xE3,0x65,0xB6,0xB7,
                         0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x6F,0xBE,
                         0x70,0x63,0xBF,0x79,0xE4,0x78,0xE5,0xC0,
                         0xC1,0xE6,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7 };
  

void us_delay(unsigned long x)
{
  while (x--)
    {
      for (int i=0; i<10; i++) __no_operation();
    }
}

#ifdef CONFIG_LCD_1602A

//
void LCD_WriteCGRAM(unsigned char data)
{
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
}

unsigned char LCD_ConvertSymOrient(unsigned int sym_ofst, unsigned char byte)
{
    unsigned char data = 0;
    unsigned char i;
    for (i = 0; i < 5; i++)
    {
        if (symtab[sym_ofst + i] & (1 << byte)) data |= (1 << (4 - i));
    }
    return data;
}

//
void LCD_CreateChar(unsigned char syg_index, unsigned char data)
{
    unsigned int sym_ofst = (data - 0x20) * 5UL;
    LCD_putcmd(0x40 + syg_index * 8, LCD_2CYCLE);
    LCD_WriteCGRAM(LCD_ConvertSymOrient(sym_ofst, 0));
    LCD_WriteCGRAM(LCD_ConvertSymOrient(sym_ofst, 1));
    LCD_WriteCGRAM(LCD_ConvertSymOrient(sym_ofst, 2));
    LCD_WriteCGRAM(LCD_ConvertSymOrient(sym_ofst, 3));
    LCD_WriteCGRAM(LCD_ConvertSymOrient(sym_ofst, 4));
    LCD_WriteCGRAM(LCD_ConvertSymOrient(sym_ofst, 5));
    LCD_WriteCGRAM(LCD_ConvertSymOrient(sym_ofst, 6));
    LCD_WriteCGRAM(LCD_ConvertSymOrient(sym_ofst, 7));
}

#endif

void LCD_putch(unsigned char data)
{
  unsigned char data_out;
#ifndef CONFIG_LCD_1602A
  if (data >= 0xC0) data_out = rus_sym_table[data-0xC0];
  else data_out = data;
#else
  if (data >= 0xC0)
  {
      if (rus_sym_table[data-0xC0] < 0xA0)
      {
          data_out = rus_sym_table[data-0xC0];
      }
      else
      {
          CPU_INT08U i;
          CPU_INT08U finded = 0;
          for (i = 0; i < LCD_MAX_CHAR_SYG; i++)
          {
            if (data == current_rus_syg[i])
            {
                data_out = i;
                finded = 1;
                break;
            }
          }
          if (!finded)
          {
              CPU_INT08U n, j;
              CPU_INT08U *ptr;
              for (j = 0; j < LCD_MAX_CHAR_SYG; j++)
              {
                  if (current_rus_syg[j] == 0) break;
                  ptr = (CPU_INT08U *)lcd_lines;
                  for (n = 0; n < 32; n++, ptr++)
                  {
                      if (current_rus_syg[j] == *ptr)
                      {
                           break;
                      }
                  }
                  if (n == 32)
                  {
                    current_rus_syg[j] = 0;
                    break;
                  }
              }
              if (j < LCD_MAX_CHAR_SYG) current_syg_index = j;
              LCD_CreateChar(current_syg_index, data);
              current_rus_syg[current_syg_index] = data;
              data_out = current_syg_index;
              LCD_goto(lcd_x, lcd_y);
          }
      }
  }
  else
  {
      data_out = data;
  }
  lcd_lines[lcd_y][lcd_x] = data;
#endif
      
  LCD_SET_DATA_OUT();
  // LCD Upper 4 bits data
  LCD_SET_RS_PIN();   // RS = 1, E = 1
  LCD_SET_E_PIN();
  LCD_CLR_RW_PIN();

  us_delay(1); 
  LCD_OUT_DATA(data_out >> 4);
  us_delay(1); 

  // E=0; write data
  LCD_CLR_E_PIN();
  us_delay(1);       

  // LCD Lower 4 bits data
  LCD_SET_E_PIN();    // RS = 1, E = 1 
  us_delay(1); 
  LCD_OUT_DATA(data_out);
  us_delay(1); 

  // E=0; write data
  LCD_CLR_E_PIN();
  us_delay(100); 
//  OSTimeDly(1);     // Wait for busy flag (BF)
  lcd_x++;
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
  lcd_x++;
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
  if (cmdtype == LCD_2CYCLE) 
  {
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
    OSTimeDly(3);    // Wait for busy flag (BF)
    
    //LCD_get_status();
  }
  else
  {
    OSTimeDly(3);
  }
}


void InitLcdPins()
{
  /*
P2.8	MK_P65	—игнал A0 индикатора
P2.7	MK_P66	—игнал E индикатора
P2.6	MK_P67	—игнал R_W индикатора
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
  OSTimeDly(100);
  // Send Command 0x30
  LCD_putcmd(0x30,LCD_1CYCLE);
  // Send Command 0x30
  LCD_putcmd(0x30,LCD_1CYCLE);
  // Send Command 0x30
  LCD_putcmd(0x30,LCD_1CYCLE);
  // Function set: Set interface to be 4 bits long (only 1 cycle write).
  LCD_putcmd(0x20,LCD_1CYCLE);
  // Function set: DL=0;Interface is 4 bits, N=1; 2 Lines, F=0; 5x8 dots font)
  LCD_putcmd(0x28,LCD_2CYCLE);
  // Display Off: D=0; Display off, C=0; Cursor Off, B=0; Blinking Off
  //LCD_putcmd(0x08,LCD_2CYCLE);
  // Display On, Cursor Off
  LCD_putcmd(0x0C,LCD_2CYCLE);      
  // Display Clear
  LCD_putcmd(0x01,LCD_2CYCLE);
  // Entry Mode Set: I/D=1; Increment, S=0; No shift
  LCD_putcmd(0x06,LCD_2CYCLE);
}

void LCD_gotoline(unsigned char n)
{
   switch (n){
     case 0: LCD_putcmd(LCD_GOTO_LINE_0, LCD_2CYCLE); lcd_x = 0; lcd_y = n; break;
     case 1: LCD_putcmd(LCD_GOTO_LINE_1, LCD_2CYCLE); lcd_x = 0; lcd_y = n; break;
#ifndef CONFIG_LCD_1602A
     case 2: LCD_putcmd(LCD_GOTO_LINE_2, LCD_2CYCLE); lcd_x = 0; lcd_y = n; break;
     case 3: LCD_putcmd(LCD_GOTO_LINE_3, LCD_2CYCLE); lcd_x = 0; lcd_y = n; break;
#endif
     default: return;
   }
}

// запись строки с номером n
void LCD_puts(unsigned char *s, unsigned char n)
{
   unsigned char i;
   LCD_putcmd(0x28,LCD_2CYCLE);
   LCD_gotoline(n);
     
   i=0;
#ifdef CONFIG_LCD_1602A
   while((*s != 0) && (*s != '\n') && (i<16))
#else
   while((*s != 0) && (*s != '\n') && (i<20))
#endif
   {
     LCD_putch(*s);
     s++;
     i++;
   }
   
#ifdef CONFIG_LCD_1602A
   while(i<16)
#else   
   while(i<20)
#endif
   {
     LCD_putch(' ');
     i++;
   }
}


void LCD_goto(unsigned char m, unsigned char n)
{
   switch (n){
     case 0: LCD_putcmd(LCD_GOTO_LINE_0+m, LCD_2CYCLE); lcd_x = m; lcd_y = n; break;
     case 1: LCD_putcmd(LCD_GOTO_LINE_1+m, LCD_2CYCLE); lcd_x = m; lcd_y = n; break;
#ifndef CONFIG_LCD_1602A
     case 2: LCD_putcmd(LCD_GOTO_LINE_2+m, LCD_2CYCLE); lcd_x = m; lcd_y = n; break;
     case 3: LCD_putcmd(LCD_GOTO_LINE_3+m, LCD_2CYCLE); lcd_x = m; lcd_y = n; break;
#endif
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
  lcd_x = 0; lcd_y = 0;
#ifdef CONFIG_LCD_1602A
  memset(lcd_lines, 0, sizeof(lcd_lines));
#endif
}

void LCD_cursor_on(void)
{
  LCD_putcmd(0x0F,LCD_2CYCLE);    
}

void LCD_cursor_off(void)
{
  LCD_putcmd(0x0C,LCD_2CYCLE);    
}

