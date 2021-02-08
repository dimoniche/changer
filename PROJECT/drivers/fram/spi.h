#ifndef __SPI_H__
#define __SPI_H__
   
// номер spi для fram
#define FM25_SPI   0

extern unsigned char spi_unselectChip(unsigned char spi);
extern unsigned char spi_selectChip(unsigned char spi);
extern void SpiInit(void);
extern unsigned char SpiExchange(unsigned char spi, unsigned char ch);
extern void spi_getSem();
extern void spi_freeSem();


#endif //#ifndef __SPI_H__