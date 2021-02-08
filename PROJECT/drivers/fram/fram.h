#ifndef _FRAM_H_
#define _FRAM_H_

#define FRAM_WRITE         0x02
#define FRAM_READ          0x03

#define FRAM_WRITE_ON      0x06
#define FRAM_WRITE_OFF     0x04

#define FRAM_READ_STATUS   0x05
#define FRAM_WRITE_STATUS  0x01

extern void WriteByteFram(unsigned short adress,unsigned char byte);
extern unsigned char ReadByteFram(unsigned short adress);

extern unsigned char Read_Status_Register(unsigned short adress);
extern void Write_Status_Register(unsigned char byte, unsigned short address);

extern void ReadArrayFram(unsigned short uAddress,unsigned short uBytesNumber,unsigned char * Array);
extern void WriteArrayFram(unsigned short uAddress,unsigned short uBytesNumber,unsigned char * Array);
extern void SetArrayFram(unsigned short uAddress, unsigned short uBytesNumber, unsigned char byte);

#endif
