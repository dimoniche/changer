#include <includes.h>
#include "spi.h"
#include "fram.h"

void Write_ON(unsigned short address)
{
  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_WRITE_ON);
  spi_unselectChip(FM25_SPI);
}

void Write_OFF(unsigned short address)
{
  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_WRITE_OFF);
  spi_unselectChip(FM25_SPI);
}

void Write_Status_Register(unsigned char byte, unsigned short address)
{
  spi_getSem();
  
  Write_ON(address);

  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_WRITE_STATUS);
  SpiExchange(FM25_SPI, byte);
  spi_unselectChip(FM25_SPI);
  
  spi_freeSem();
}

void WriteArrayFram(unsigned short uAddress,unsigned short uBytesNumber,unsigned char * Array)
{
  unsigned short i;

  spi_getSem();

  Write_ON(uAddress);

  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_WRITE);
  SpiExchange(FM25_SPI, (char)(uAddress >> 8));
  SpiExchange(FM25_SPI, (char)uAddress);
  for(i=0;i<uBytesNumber;i++)
  {
    SpiExchange(FM25_SPI, *(Array+i));
  }
  spi_unselectChip(FM25_SPI);

  Write_OFF(uAddress);  

  spi_freeSem();
}

// запись одного и того же байта в массив адресов фрам
void SetArrayFram(unsigned short uAddress, unsigned short uBytesNumber, unsigned char byte)
{
  unsigned short i;

  spi_getSem();

  Write_ON(uAddress);

  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_WRITE);
  SpiExchange(FM25_SPI, (char)(uAddress >> 8));
  SpiExchange(FM25_SPI, (char)uAddress);
  for(i=0;i<uBytesNumber;i++)
  {
    SpiExchange(FM25_SPI, byte);
  }
  spi_unselectChip(FM25_SPI);

  Write_OFF(uAddress);  

  spi_freeSem();
}

void WriteByteFram(unsigned short adress,unsigned char byte)
{
  spi_getSem();

  Write_ON(adress);
  
  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_WRITE);
  SpiExchange(FM25_SPI, (char)(adress >> 8));
  SpiExchange(FM25_SPI, (char)adress);
  SpiExchange(FM25_SPI, byte);
  spi_unselectChip(FM25_SPI);

  Write_OFF(adress);

  spi_freeSem();
}

unsigned char Read_Status_Register(unsigned short adress)
{
  unsigned char temp = 0;

  spi_getSem();

  Write_OFF(adress);

  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_READ_STATUS);
  temp = SpiExchange(FM25_SPI, 0x00);
  spi_unselectChip(FM25_SPI);

  spi_freeSem();
  
  return temp;
}

void ReadArrayFram(unsigned short uAddress,unsigned short uBytesNumber,unsigned char * Array)
{
  unsigned short i;

  spi_getSem();

  Write_OFF(uAddress);

  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_READ);
  SpiExchange(FM25_SPI, (char)(uAddress >> 8));
  SpiExchange(FM25_SPI, (char)uAddress);
  for(i=0;i<uBytesNumber;i++)
  {
    *(Array + i) = SpiExchange(FM25_SPI, 0xFF);
  }
  spi_unselectChip(FM25_SPI);

  spi_freeSem();
}

unsigned char ReadByteFram(unsigned short adress)
{
  unsigned char temp = 0;
   
  spi_getSem();

  Write_OFF(adress);

  spi_selectChip(FM25_SPI);
  SpiExchange(FM25_SPI, FRAM_READ);
  SpiExchange(FM25_SPI, (char)(adress >> 8));
  SpiExchange(FM25_SPI, (char)adress);
  temp = SpiExchange(FM25_SPI, 0xFF);
  spi_unselectChip(FM25_SPI);

  spi_freeSem();

  return temp;
}

void WriteFullControl(void)
{

}

