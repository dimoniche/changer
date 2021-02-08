#include "iolpc2368.h"
#include "ucos_ii.h"
#include <stdlib.h>
#include "cpu.h"
#include "spi.h"

OS_EVENT *SpiLock = NULL;

unsigned char SpiExchange(unsigned char spi, unsigned char ch)
{
  SSP0DR = ch;
  while(SSP0SR_bit.BSY);
  return SSP0DR;
}

void SpiInit(void)
{
  // on spi power
  PCONP_bit.PCSSP0 = 1;

  // clk = cclk
  PCLKSEL1_bit.PCLK_SSP0 = 1;
  
  // pin select
  PINSEL0_bit.P0_15 = 0x2;
  PINSEL1_bit.P0_16 = 0x0; // FRAM CS
  PINSEL1_bit.P0_17 = 0x2;
  PINSEL1_bit.P0_18 = 0x2;
  
  PINMODE0_bit.P0_15 = 2;
  PINMODE1_bit.P0_16 = 0;
  PINMODE1_bit.P0_17 = 2;
  PINMODE1_bit.P0_18 = 2;
  
  // Chip select
  FIO0MASK_bit.P0_16 = 0;
  FIO0DIR_bit.P0_16  = 1;
  FIO0SET_bit.P0_16  = 1;
  
  FIO0MASK_bit.P0_15 = 1;
  FIO0MASK_bit.P0_17 = 1;
  FIO0MASK_bit.P0_18 = 1;
  
  SSP0CR0_bit.DSS = 7; // 8-bit
  SSP0CR0_bit.FRF = 0; // spi
  SSP0CR0_bit.SPO = 0;
  SSP0CR0_bit.SPH = 0;
  SSP0CR0_bit.SCR = 1;
  
  SSP0CR1_bit.LBM = 0;
  SSP0CR1_bit.SSE = 1;
  SSP0CR1_bit.MS = 0; // master
  SSP0CR1_bit.SOD = 0;
  
  SSP0CPSR_bit.CPSDVSR = 2;  
  
  SSP0IMSC = 0;
  SSP0DMACR = 0;
 
  if (!SpiLock) SpiLock = OSSemCreate(1); 
}


unsigned char spi_selectChip(unsigned char spi)
{
 if (spi == FM25_SPI){
   FIO0CLR_bit.P0_16  = 1;
 }
 return 0;
}

unsigned char spi_unselectChip(unsigned char spi)
{
 if (spi == FM25_SPI){
   FIO0SET_bit.P0_16  = 1;
 }
 return 0;
}

void spi_getSem()
{
 CPU_INT08U err;  
 do{
   OSSemPend(SpiLock, 1, &err);
   if (!err) break;
   OSTimeDly(1);
 }while (err);
}

void spi_freeSem()
{
  OSSemPost(SpiLock);
}

