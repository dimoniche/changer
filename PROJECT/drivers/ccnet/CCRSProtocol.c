#include <includes.h>
#include "CCRSProtocol.h"
#include "VMCConst.h"
#include "validator.h"
#include "uart1.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

int iCmdDelay=20;
int iLastError=0; 

// обновление crc
unsigned short crc16_ccitt(unsigned char data, unsigned short crc)
{	register unsigned short a=0x8408, d = crc, i;
	d^= data;
	for (i=0; i<8; i++)
	{if (d & 0x0001)
		{d>>= 1;
		d^=a;}
		else d>>=1;
	}
	return d;
}

// подсчет crc
unsigned short CalculateCRC(unsigned char *pBuffer)
{
	unsigned short wCRC=0;
        int Len = (pBuffer[2]) ? pBuffer[2] : ((unsigned short)pBuffer[4]<<8)+pBuffer[5];
	for (int i=0;i<Len-2;i++)wCRC=crc16_ccitt(pBuffer[i],wCRC);
	return wCRC;
}


// посылка подготовленных данных из буфера и прием ответа
int SendCommand(unsigned char *BufOut, unsigned char *BufIn)
{
        int iRecievingError, iBytesToRecieve;
	iRecievingError=RE_TIMEOUT;
	for(int iErrCount=0;iErrCount<1;iErrCount++)
	{
		iBytesToRecieve=6;
                PurgeComm();
		
                if(!BufOut[2]) COMPort_Send(BufOut, ((unsigned short)BufOut[4]<<8)+BufOut[5]);
		else           COMPort_Send(BufOut, BufOut[2]);
		if((BufOut[3]==ACK)||(BufOut[3]==NAK))
			return iRecievingError=RE_NONE;
		
		if(COMPort_Recieve(BufIn, iBytesToRecieve))
		{
			if(BufIn[0]!=SYNC)
				iRecievingError=RE_SYNC;
			else
			{
				int iLen=((BufIn[2])?BufIn[2]:(BufIn[5]+((unsigned short)BufIn[4]<<8)))-iBytesToRecieve;
				if(iLen>0)
				{
				if(COMPort_Recieve(BufIn+iBytesToRecieve, iLen))	
				{
					iRecievingError=RE_NONE;		
					break;
				}
				else 
				{
					iRecievingError=RE_DATA;
					PurgeComm();					
				}
				}
				else
				{
					iRecievingError=RE_NONE;
					break;
				}
			}
		}
	}
	return iRecievingError;
}


/**	
  Посылка команды
  дополняет команду адресом устройства и crc
  возвращает номер ошибки и данные по указателю

  The function is a simple wrapper for the CCCRSProtocol::SendCommand(LPBYTE BufOut, LPBYTE BufIn) member function
  and performs the following actions:
	-# Complementing the output frame with the device address and CRC16
	-# Sending the frame and receiving a response using CCCRSProtocol::SendCommand(LPBYTE BufOut, LPBYTE BufIn) member function
	-# Checking received frame integrity (by CRC16 value)
	-# Returning the response wrapped in the CCommand object

	\param	Cmd	a parameter of type CCommand & containing output frame (should contain all required information except of device address and CRC)
	\param	Addr	a parameter of type BYTE containing the device address used for communication. 
					Refer to \link Addr Device address list \endlink for the valid values

	
*/
unsigned char tmpBuffer[256];

int TransmitCMD(unsigned char *Cmd, unsigned char Addr, unsigned char **data)
{	
	int i=(Cmd[2]) ? Cmd[2]
                       : ((unsigned short)Cmd[4]<<8)+Cmd[5];
	Cmd[1] = Addr;
	unsigned short wCRC=CalculateCRC(Cmd);
	Cmd[i-2] = (unsigned char)wCRC;
        Cmd[i-1] = (unsigned char)(wCRC>>8);

	int iErrCode=SendCommand(Cmd, tmpBuffer);
	if((!iErrCode)&&(Cmd[3])&&(0xFF!=Cmd[3]))
	{
          if (tmpBuffer[2])
            {
              wCRC = (unsigned short)tmpBuffer[tmpBuffer[2]-2];
              wCRC += (unsigned short)((unsigned short)tmpBuffer[tmpBuffer[2]-1])<<8;
            }
          else
            {
              wCRC = 0;
		//wCRC=tmpBuffer[(tmpBuffer[2])?tmpBuffer[2]:
                //                  (((unsigned short)(tmpBuffer[4]))<<8)+tmpBuffer[5]-2]+
		//	(tmpBuffer[(tmpBuffer[2])?tmpBuffer[2]:(((unsigned short)(tmpBuffer[4]))<<8)+tmpBuffer[5]-1]<<8);
            }
	  if(CalculateCRC(tmpBuffer)!=wCRC) iErrCode=RE_CRC;
	}
	*data = tmpBuffer;
	return iErrCode;
}

/**	\brief	The CCCRSProtocol::Transmit function carries complete protocol exchange

  The function is a simple wrapper for the CCCRSProtocol::TransmitCMD(CCommand CMD, BYTE Addr) member function
  and performs the following actions:
	-# Sending the frame and receiving a response using CCCRSProtocol::TransmitCMD(CCommand CMD, BYTE Addr) member function
	-# Checking the device response and determining whether ACK or NAK should be sent 
	-# Sending ACK or NAK message to the device or retransmitting the command up to 3 times untill communication is successfully completed
	-# Returning the response wrapped in the CCommand object

	\param	CMD	a parameter of type CCommand & containing output frame (should contain all required information except of device address and CRC)
	\param	Addr	a parameter of type BYTE containing the device address used for communication. 
					Refer to \link Addr Device address list \endlink for the valid values

	\return	CCommand - an object containing response data and communication error code

	
*/
// функция посылает команду, посылает ACK и повторяет команду, если ошибка
int Transmit(unsigned char *CMD, unsigned char Addr)
{
	unsigned char *cmdRes, cmdACK[16];
        int error;
        
        VPend();
	for (int i=0; i<3; i++)
	  {
	    error=TransmitCMD(CMD, Addr, &cmdRes);
	    cmdACK[0] = SYNC;
	    cmdACK[2] = 6;
	    cmdACK[3] = ACK;
	
	    if (error == RE_NONE)
              {
		if((ACK==cmdRes[3]) && (cmdRes[2]==6))
		{ // ошибки нет
                  VPost();
		  return error;
		}
		if((NAK==cmdRes[3]) && (cmdRes[2]==6))
		{
  		  if (iCmdDelay) Sleep(iCmdDelay);//5	
		}
		else
		{
		  cmdACK[3] = ACK;
		  TransmitCMD(cmdACK, Addr, &cmdRes);
		  if (iCmdDelay) Sleep(iCmdDelay);//5
		  break;
		}
	      }
	    else
	      {
		if(error != RE_TIMEOUT)
		  {
			cmdACK[3] = NAK;
			TransmitCMD(cmdACK, Addr, &cmdRes);
			if (iCmdDelay) Sleep(iCmdDelay);//5
		  }
	      }
	  }
        
        VPost();
	return error;
}

//////////////////////////////////////////////////////////////////////
// CCNET Commands implementation
/** \defgroup CCNETCommands CCNET protocol commands and requests

	The group contains member functions providing interface to CCNET commands and requests.
	All functions return a bool result showing whether operation was successfully completed.
	In the case of error the error code (refer to \link ErrCode Possible error codes \endlink) 
	is stored in the CCCRSProtocol::iLastError member variable, which can be used in further analysis.

  @{
*/

/**	\brief	The CCCRSProtocol::CmdReset function sends a RESET command to the device

	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if command was acknowledged

	
*/

unsigned char cc_buf[256];

int CC_CmdReset(unsigned char Addr)
{
	const unsigned char Data[]={SYNC,0,6,RESET,0,0};
        memcpy(cc_buf, Data, sizeof(Data));
	int Response = Transmit(cc_buf, Addr);
	unsigned char ack;
	if (!Response)
	  {
            ack = tmpBuffer[3];
	    if(ack != ACK)
		{
			iLastError = (ack != ST_INV_CMD) ? ER_NAK : ER_INVALID_CMD;
			return 0;
		}
	    else 
		{
			return 1;
		}

	  }
	else 
	  {
		return 0;
	  }
}


/**	\brief	The CCCRSProtocol::CmdPoll function sends POLL command to the device

  The function sends POLL command and fills bytes Z1 and Z2 of the response into the CCCRSProtocol::PollResults structure.

	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if exchange was successfully completed

	
*/
int CC_CmdPoll(unsigned char Addr, TPollResults *PollResults)
{
	const unsigned char Data[]={SYNC,0,6,POLL,0,0};
        memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf, Addr);
	if(!Response)
	{
		PollResults->Z1 = tmpBuffer[3];
		PollResults->Z2 = tmpBuffer[4];
		return 1;
	}
	else 
	{
		PollResults->Z1=0;
		PollResults->Z2=0;
		return 0;
	}
}

/**	\brief	The CCCRSProtocol::CmdStatus function sends STATUS REQUEST to the device

  The response status data is stored in the CCCRSProtocol::BillStatus member structure.

	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if exchange was successfully completed

	
*/
int CC_CmdStatus(unsigned char Addr, TBillStatus* BillStatus)
{
	const unsigned char Data[]={SYNC,0,6,GET_STATUS,0,0};
        memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf,Addr);
	if(!Response)
	{
		if((tmpBuffer[3]==ST_INV_CMD)&&(tmpBuffer[2]==6))
		{
			iLastError=ER_INVALID_CMD;
			BillStatus->Enabled=0;
			BillStatus->Security=0;
			BillStatus->Routing=0;
			return 0;
		}
		BillStatus->Enabled=tmpBuffer[5]+((unsigned long)tmpBuffer[4]<<8)+((unsigned long)tmpBuffer[3]<<16);
		BillStatus->Security=tmpBuffer[8]+((unsigned long)tmpBuffer[7]<<8)+((unsigned long)tmpBuffer[6]<<16);
		BillStatus->Routing=tmpBuffer[11]+((unsigned long)tmpBuffer[10]<<8)+((unsigned long)tmpBuffer[9]<<16);
		return 1;
	}
	else 
	{
		return 0;
	}

}

/**	\brief	The CCCRSProtocol::CmdIdentification function sends IDENTIFICATION request

  The function sends IDENTIFICATION request and stores device identification in the member CCCRSProtocol::Ident structure.
  The function supports both new and old identification formats of Bill-To-Bill units.

	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if the exchange was successfully completed and data received

	
*/
int CC_CmdIdentification(unsigned char Addr, TIdent* Ident)
{
	const unsigned char Data[]={SYNC,0,6,IDENTIFICATION,0,0};
	memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf,Addr);
	if (!Response)
	{
		if((tmpBuffer[3]==ST_INV_CMD)&&(tmpBuffer[2]==6))
		{
			iLastError=ER_INVALID_CMD;
			return 0;
		}
		strcpy(Ident->BCCPUBoot,"N/A");
		strcpy(Ident->BCCPUVersion,"N/A");
		strcpy(Ident->BCCS1Boot,"N/A");
		strcpy(Ident->BCCS2Boot,"N/A");
		strcpy(Ident->BCCS3Boot,"N/A");
		strcpy(Ident->BCCSVersion,"N/A");
		strcpy(Ident->BCDispenserBoot,"N/A");
		strcpy(Ident->BCDispenserVersion,"N/A");
		strcpy(Ident->BVBootVersion,"N/A");
		strcpy(Ident->BVVersion,"N/A");
		strcpy(Ident->PartNumber,"N/A");
		char sTemp[64];
		int iPos=3,iLen=15;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->PartNumber,sTemp);
		iLen=12;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->SN,sTemp);
		char *strTemp=(char*)tmpBuffer+iPos;
		

		Ident->DS1=0;iPos+=8;
		for(int i=0;i<7;i++)
		{
			Ident->DS1<<=8;
			Ident->DS1+=strTemp[i];
		}
		if(tmpBuffer[2]<109) return 1;
		
		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BVBootVersion,sTemp);
		
		iLen=20;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BVVersion,sTemp);

		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BCCPUBoot,sTemp);

		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BCCPUVersion,sTemp);

		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BCDispenserBoot,sTemp);

		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BCDispenserVersion,sTemp);

		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BCCS1Boot,sTemp);

		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BCCS2Boot,sTemp);

		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BCCS3Boot,sTemp);

		iLen=6;
		strncpy(sTemp,(char*)tmpBuffer+iPos,iLen);
		sTemp[iLen]=0;iPos+=iLen;
		strcpy(Ident->BCCSVersion,sTemp);
		return 1;
	}
	else 
	{
		return 0;
	}
}

/**	\brief	The CCCRSProtocol::CmdHold function sends HOLD command to the device

	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if exchange successfully completed

	
*/
int CC_CmdHold(unsigned char Addr)
{
	const unsigned char Data[256]={SYNC,0,6,HOLD,0,0};
        memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf, Addr);
	unsigned char ack;
	if(!Response)
	{
          ack = tmpBuffer[3];
	  if(ack != ACK)
            {
	      iLastError = (ack!=ST_INV_CMD) ? ER_NAK : ER_INVALID_CMD;
	      return 0;
	    }
	  else return 1;

	}
	else 
	{
	  return 0;
	}
}

/**	\brief	The CCCRSProtocol::CmdSetSecurity function sends SET SECURITY LEVELS command

	\param	wS	a parameter of type DWORD - a bitmap containing security levels to set
	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if exchange successfully completed

	
*/
int CC_CmdSetSecurity(unsigned long wS, unsigned char Addr)
{
	const unsigned char Data[]={SYNC,0,9,SET_SECURITY,0,0,0,0,0};
        memcpy(cc_buf, Data, sizeof(Data));
        cc_buf[4]=(unsigned char)(wS>>16);
        cc_buf[5]=(unsigned char)(wS>>8);
        cc_buf[6]=(unsigned char)(wS);
	int Response=Transmit(cc_buf, Addr);
	unsigned char ack;
	if(!Response)
	{
          ack = tmpBuffer[3];
          if(ack != ACK)
		{
			iLastError = (ack!=ST_INV_CMD)?ER_NAK:ER_INVALID_CMD;
			return 0;
		}
	  else return 1;

	}
	else 
	{
		return 0;
	}
}

/**	\brief	The CCCRSProtocol::CmdBillType function sends ENABLE BILL TYPE command

	\param	enBill	a parameter of type DWORD - a bitmap containing 1 in the positions corresponding to the enabled bill types
	\param	escBill	a parameter of type DWORD - a bitmap containing 1 in the positions corresponding to bill type processed with escrow
	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool- true if the command was acknowledged

	
*/
int CC_CmdBillType(unsigned long enBill, unsigned long escBill, unsigned char Addr)
{
	unsigned char Data[]={SYNC,0,12,BILL_TYPE,
					(unsigned char)(enBill>>16),(unsigned char)(enBill>>8),(unsigned char)enBill,
					(unsigned char)(escBill>>16),(unsigned char)(escBill>>8),(unsigned char)escBill,
					0,0};
        memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf, Addr);
	unsigned char ack;
	if (!Response)
	{
          ack = tmpBuffer[3];
	  if(ack!=ACK)
		{
			iLastError=(ack!=ST_INV_CMD)?ER_NAK:ER_INVALID_CMD;
			return 0;
		}
		else return 1;

	}
	else 
	{
		return 0;
	}
}

/**	\brief	The CCCRSProtocol::CmdPack function sends PACK command

	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if the command was acknowledged

	
*/
int CC_CmdPack(unsigned char Addr)
{
	const unsigned char Data[]={SYNC,0,6,PACK,0,0};
	memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf,Addr);
	unsigned char ack;
	if (!Response)
	{
          ack = tmpBuffer[3];
  	  if(ack!=ACK)
		{
			iLastError=(ack!=ST_INV_CMD)?ER_NAK:ER_INVALID_CMD;
			return 0;
		}
	  else return 1;

	}
	else 
	{
		return 0;
	}
}

/**	\brief	The CCCRSProtocol::CmdReturn function sends RETURN command

	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if the command was acknowledged

	
*/
int CC_CmdReturn(unsigned char Addr)
{
	const unsigned char Data[256]={SYNC,0,6,RETURN,0,0};
	memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf,Addr);
	unsigned char ack;
	if(!Response)
	{
           ack = tmpBuffer[3];
		if(ack!=ACK)
		{
			iLastError=(ack!=ST_INV_CMD)?ER_NAK:ER_INVALID_CMD;
			return 0;
		}
		else return 1;

	}
	else 
	{
		return 0;
	}
}


/**	\brief	The CCCRSProtocol::CmdGetBillTable function sends BILL TABLE request

	\param	BillTable	a parameter of type _BillRecord * containing pointer to the _BillRecord array receiving the bill table.
			Position in the array corresponds to the bill type and the structure at the position describes that bill type.
	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if the response was successfully received

	
*/
int CC_CmdGetBillTable(unsigned char Addr, TBillRecord *BillTable)
{
	const unsigned char Data[]={SYNC,0,6,GET_BILL_TABLE,0,0};
	memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf,Addr);
	if(!Response)
	{
                int i;
		if((tmpBuffer[3]==ST_INV_CMD)&&(tmpBuffer[2]==6))
		{
			iLastError=ER_INVALID_CMD;
			for(int i=0;i<24;i++)
			{
			BillTable[i].Denomination=0;
			strcpy(BillTable[i].sCountryCode,"");
			}
			return 0;
		}
		for(i=0;i<tmpBuffer[2]-5;i+=5)
		{
			BillTable[i/5].Denomination=tmpBuffer[i+3];
			char sTmp[5];
			strncpy(sTmp,(const char *)(tmpBuffer+i+4),3);
			sTmp[3]='\0';
			strcpy(BillTable[i/5].sCountryCode,sTmp);
			if(((tmpBuffer)[i+7])&0x80)
			{
				for(int j=0; j<((tmpBuffer[i+7])&0x7F);j++)
					BillTable[i/5].Denomination/=10;
			}
			else
			{
				for(int j=0; j<((tmpBuffer[i+7])&0x7F);j++)
					BillTable[i/5].Denomination*=10;
			};
		}
		for(;i<24*5;i+=5)
		{
			BillTable[i/5].Denomination=0;
			strcpy(BillTable[i/5].sCountryCode,"");
		} 
		return 1;
	
	}
	else 
	{
		return 0;
	}
}


/**	\brief	The CCCRSProtocol::CmdSetOptions function

	\param	dwOpt	a parameter of type DWORD containing bitmap with options to enable. refer to \link Options Options list \endlink
	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if the command was acknowledged

	
*/
int CC_CmdSetOptions(unsigned long dwOpt, unsigned char Addr)
{
	unsigned char Data[]={SYNC,0,10,SET_OPTIONS,
					(unsigned char)(dwOpt>>24),(unsigned char)(dwOpt>>16),(unsigned char)(dwOpt>>8),(unsigned char)dwOpt,
					0,0};
	memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf,Addr);
	if(!Response)
	{
          unsigned char ack = tmpBuffer[3];
		if(ack!=ACK)
		{
			iLastError=(ack!=ST_INV_CMD)?ER_NAK:ER_INVALID_CMD;
			return 0;
		}
		else return 1;

	}
	else 
	{
		return 0;
	}
}



/**	\brief	The CCCRSProtocol::CmdGetCRC32 function sends CRC32 request

	\param	dwCRC	a parameter of type DWORD & containing a reference to the variable receiving CRC32 of the firmware.
	\param	Addr	a parameter of type BYTE containing device address. Refer to \link Addr Device address list \endlink for the valid values

	\return	bool - true if the request was answered

	
*/
int CC_CmdGetCRC32(unsigned long *dwCRC, unsigned char Addr)
{
	unsigned char Data[]={SYNC,0,6,CRC32,0,0};
	memcpy(cc_buf, Data, sizeof(Data));
	int Response=Transmit(cc_buf,Addr);
	if(!Response)
	{
          
		if((tmpBuffer[3]==ST_INV_CMD)&&(tmpBuffer[2]==6))
		{
			iLastError=ER_INVALID_CMD;
			dwCRC=0;
			return 0;
		}
		*dwCRC=tmpBuffer[6]+((unsigned long)tmpBuffer[5]<<8)+((unsigned long)tmpBuffer[4]<<16)+((unsigned long)tmpBuffer[3]<<24);
		return 1;
	}
	else 
	{
		dwCRC=0;
		return 0;
	}
}







/** @} */