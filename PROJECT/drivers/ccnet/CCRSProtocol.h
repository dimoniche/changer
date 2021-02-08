#ifndef _CCRSPROTOCOL_H_
#define _CCRSPROTOCOL_H_

#define CC_TIME_OUT 300

#define SYNC        0x02	//!< synchronization byte 
#define ACK         0x00	//!< ACK code
#define NAK	    0xFF	//!< NAK code
#define ST_INV_CMD  0x30        //!< INVALID COMMAND response

/**	\defgroup Addr Device Addresses
* @{
*/
#define ADDR_BB		0x01 //!< Address for Bill-To-Bill units
#define ADDR_CHANGER    0x02 //!< Address for Coin Changer
#define ADDR_FL		0x03 //!< Address for Bill Validators
#define ADDR_CR		0x04 //!< Address for Smart Card Reader
/**@} */

/**	\defgroup Cmds Interface commands
* @{
*/
#define RESET		0x30 //!<REST command code
#define GET_STATUS	0x31 //!<STATUS REQUEST command code
#define SET_SECURITY	0x32 //!<SET SECURITY command code
#define POLL		0x33 //!<POLL command code
#define BILL_TYPE	0x34 //!<BILL TYPE command code
#define PACK		0x35 //!<PACK command code
#define RETURN		0x36 //!<RETURN command code
#define IDENTIFICATION  0x37 //!<IDENTIFICATION command code
#define IDENT_EXT	0x3E //!<EXTENDED IDENTIFICATION command code
#define HOLD		0x38 //!<HOLD command code
#define C_STATUS	0x3B //!<RECYCLING CASSETTE STATUS REQUEST command code
#define DISPENSE	0x3C //!<DISPENSE command code
#define UNLOAD		0x3D //!<UNLOAD command code
#define SET_CASSETES	0x40 //!<SET RECYCLING CASSETTE TYPE command code
#define GET_BILL_TABLE  0x41 //!<BILL TABLE REQUEST command code	
#define DOWNLOAD	0x50 //!<DOWNLOAD command code
#define CRC32		0x51 //!<CRC32 REQUEST command code
#define SET_TIME	0x62 //!<SET BB TIME command code
#define SET_BAR_PARAMS		0x39 //!<SET BARCODE PARAMETERS command code
#define EXTRACT_BAR_DATA	0x3A //!<EXTRACT BARCODE DATA command code
#define POWER_RECOVERY		0x66 //!<POWER RECOVERY command code
#define EMPTY_DISPENSER		0x67 //!<EMPTY DISPENSER command code
#define SET_OPTIONS		0x68 //!<SET OPTIONS command code
#define GET_OPTIONS		0x69 //!<GET OPTIONS command code
/**@} */

/**	\defgroup Options Options
* Describes options supported by CCNET (as a bitmap)
* @{
*/
//Options (bitmap)
#define OPT_LED_INHIBIT 0x80000000L //!< Turn OFF LEDs of the bezel in the DISABLED state
#define OPT_KEEP_BILL   0x40000000L //!< Hold bill after ejection on the input roller
#define OPT_LOOK_TAPE   0x20000000L //!< Use improved algorithm for tape detection
#define OPT_TURN_SWITCH 0x10000000L //!< Turn switch after packing a bill
/**@} */

// States
/**	\defgroup States CCNET states and events
* 
* @{
*/
#define ST_POWER_UP	        0x10//!< POWER UP state
#define ST_POWER_BILL_ESCROW	0x11//!< POWER UP WITH BILL IN ESCROW state
#define ST_POWER_BILL_STACKER	0x12//!< POWER UP WITH BILL IN STACKER state
#define ST_INITIALIZE		0x13//!< INITIALIZING state
#define ST_IDLING		0x14//!< IDLING state
#define ST_ACCEPTING		0x15//!< ACCEPTING state
#define ST_PACKING		0x17//!< STACKING/PACKING state
#define ST_RETURNING		0x18//!< RETURNING state
#define ST_DISABLED		0x19//!< UNIT DISABLED state
#define ST_HOLDING		0x1A//!< HOLDING state
#define	ST_BUSY			0x1B//!< Device is busy
#define ST_REJECTING		0x1C//!< REJECTING state. Followed by a rejection code
	
//Rejection codes
/**	\defgroup RCodes Rejection codes
* 
* @{
*/
#define RJ_INSERTION		0x60 //!< Rejection because of insertion problem
#define RJ_MAGNETIC		0x61 //!< Rejection because of invalid magnetic pattern
#define RJ_REMAINING		0x62 //!< Rejection because of other bill remaining in the device
#define RJ_MULTIPLYING		0x63 //!< Rejection because of multiple check failures
#define RJ_CONVEYING		0x64 //!< Rejection because of conveying 
#define RJ_IDENT		0x65 //!< Rejection because of identification failure
#define RJ_VRFY			0x66 //!< Rejection because of verification failure
#define RJ_OPT			0x67 //!< Rejection because of optical pattern mismatch
#define RJ_INHIBIT		0x68 //!< Rejection because the denomination is inhibited
#define RJ_CAP			0x69 //!< Rejection because of capacity sensor pattern mismatch
#define RJ_OPERATION		0x6A //!< Rejection because of operation error
#define RJ_LNG			0x6C //!< Rejection because of invalid bill length
#define RJ_UV			0x6D //!< Rejection because of invalid UV pattern
#define RJ_BAR		        0x92 //!< Rejection because of unrecognized barcode
#define RJ_BAR_LNG	        0x93 //!< Rejection because of invalid barcode length
#define RJ_BAR_START            0x94 //!< Rejection because of invalid barcode start sequence
#define RJ_BAR_STOP	        0x95 //!< Rejection because of invalid barcode stop sequence
/**@} */

#define ST_DISPENSING		0x1D//!< DISPENSING state
#define ST_UNLOADING		0x1E//!< UNLOADING state 
#define ST_SETTING_CS_TYPE	0x21//!< SETTING RECYCLING CASSETTE TYPE state
#define ST_DISPENSED		0x25//!< DISPENSED event
#define ST_UNLOADED		0x26//!< UNLOADED event
#define ST_BILL_NUMBER_ERR	0x28//!< INVALID BILL NUMBER event
#define ST_CS_TYPE_SET		0x29//!< RECYCLING CASSETTE TYPE SET event
#define ST_ST_FULL		0x41//!< DROP CASSETTE IS FULL state
#define ST_BOX			0x42//!< DROP CASSETTE REMOVED state 
#define ST_BV_JAMMED		0x43//!< JAM IN VALIDATOR state
#define ST_ST_JAMMED		0x44//!< JAM IN STACKER state
#define ST_CHEATED		0x45//!< CHEATED event
#define ST_PAUSED		0x46//!< PAUSED state
#define ST_FAILURE		0x47//!< FAILURE state

	//Failure codes
/**	\defgroup FCodes Failure codes
* 
* @{
*/
#define FLR_STACKER		0x50 //!< Stacking mechanism failure
#define FLR_TR_SPEED		0x51 //!< Invalid speed of transport mechanism
#define FLR_TRANSPORT		0x52 //!< Transport mechanism failure
#define FLR_ALIGNING		0x53 //!< Aligning mechanism failure
#define FLR_INIT_CAS		0x54 //!< Initial cassette status failure
#define FLR_OPT			0x65 //!< Optical channel failure
#define FLR_MAG			0x66 //!< Inductive channel failure
#define FLR_CAP			0x67 //!< Capacity sensor failure
/**@} */

// Credit events
#define ST_PACKED		0x81	/**< A bill has been packed. 2nd byte - 0xXY:
					  \n X-bill type
					  \n Y-Packed into:
					  \n 0-BOX, else - Cassette Y;
					*/
#define ESCROW			0x80 //!< A bill is held in the escrow position	
#define RETURNED		0x82 //!< A bill was returned
/**@} */

// Cassetes status
/**	\defgroup CSStatus Possible cassette status codes
* 
* @{
*/
#define CS_OK			0	//!< Cassette is present and operational
#define CS_FULL			1       //!< Cassette is full
#define CS_NU			0xFE    //!< Cassette is not present
#define CS_MALFUNCTION	        0xFF    //!< Cassette is malfunctioning
#define CS_NA			0xFD    //!< Cassette is not assigned to any denomination
#define CS_ESCROW		0xFC    //!< Cassette is assigned to multi-escrow 
/**@} */


/**	\defgroup BTs Predefined bill type values
* 
* @{
*/
#define BT_ESCROW		24    //!< Bill type associated with the escrow cassette
#define BT_NO_TYPE		0x1f  //!< Invalid bill type
#define BT_BAR			23    //!< Bill type associated with barcode coupon
/**@} */


// Error codes 
/**	\defgroup ErrCode CCNET Interface error codes
*
* @{
*/
/** \defgroup CErrs Communication error codes
	
	  The codes related to phisical data transmission and frame integrity

   @{
*/
#define RE_NONE			 0//!< No error happened
#define RE_TIMEOUT		-1//!< Communication timeout
#define RE_SYNC			-2//!< Synchronization error (invalid synchro byte)
#define RE_DATA			-3//!< Data reception error
/**@} */
#define RE_CRC			-4//!< CRC error

/** \defgroup LErrs Logical error codes

  The codes related to the interface logic

   @{
*/
#define ER_NAK			-5//!< NAK received
#define ER_INVALID_CMD	        -6//!< Invalid command response received 
#define ER_EXECUTION		-7//!< Execution error response received
#define ERR_INVALID_STATE	-8//!< Invalid state received
/**@} */
/**@} */
// Class Declaration
/**	\class CCCRSProtocol
	\brief	The CCCRSProtocol class providing low-level communication functions for the CCNET protocol

*/
/*
class CCCRSProtocol  
{
	CCommand cmdIn; //!< A variable to store current device responses
	CCommand cmdOut;//!< A variable to store controller commands
	CCOMPort COMPort; //!< A COM port to work with
private:
	WORD CalculateCRC(unsigned char*);
	CCommand Transmit(CCommand CMD, BYTE Addr=ADDR_BB);
	unsigned short crc16_ccitt(unsigned char, unsigned short);
	CCommand TransmitCMD(CCommand&, BYTE);
	int SendCommand(LPBYTE BufOut, LPBYTE BufIn);

public:
	int iCmdDelay;	//!< Delay between two consequtive commands
	int iTimeout;	//!< Communication timeout value
	int iLastError; //!< A variable storing error code generated during last serial I/O operation

public:	
	CCCRSProtocol();
	virtual ~CCCRSProtocol();


	
// Protocol commands	
	bool CmdEmptyDispenser();
	bool CmdPowerRecovery(BYTE&,LPBYTE,int&);
	bool CmdGetCRC32(DWORD&,BYTE);
	bool CmdGetOptions(DWORD&, BYTE);
	bool CmdSetOptions(DWORD, BYTE);
	bool CmdIdentExt(BYTE);
	bool CmdExtractBarData(LPSTR sBar, BYTE Addr);
	bool CmdSetBarParams(BYTE Format, BYTE Length, BYTE Addr);
	bool CmdBBTime(time_t&, bool bSet=false);
	bool CmdDispenseNew(LPBYTE, bool bTypes=true);
	bool CmdGetBillTable(_BillRecord*,BYTE Addr=ADDR_BB);
	bool CmdSetCasseteType(BYTE, BYTE,BYTE);
	bool CmdReset(BYTE Addr=ADDR_BB);
	bool CmdPoll(BYTE Addr=ADDR_BB);
	bool CmdStatus(BYTE Addr=ADDR_BB);
	bool CmdUnload(BYTE,BYTE);//Unload bills 
	bool CmdDispense(LPBYTE);//  
	bool CmdCsStatus();
	bool CmdReturn(BYTE Addr=ADDR_BB);
	bool CmdPack(BYTE Addr=ADDR_BB);
	bool CmdBillType(DWORD,DWORD,BYTE Addr=ADDR_BB);
	bool CmdSetSecurity(DWORD,BYTE Addr=ADDR_BB);
	bool CmdHold(BYTE Addr=ADDR_BB);
	bool CmdIdentification(BYTE Addr=ADDR_BB);
//////////////////////////
// COM port related functions
	BOOL InitCOM(int, int);
	DWORD PortState(int);
	CCOMPort* GetCOMPort();
	
*/

//Protocol structures

/**	\struct _BillStatus
	\brief	The _BillStatus struct describing response to the STATUS REQUEST command
	
*/
/*
	struct _BillStatus
	{
		DWORD Enabled; //!< A bitmap describing which bill types are enabled
		DWORD Security; //!< A bitmap describing which bill types are processed in High Security mode
		DWORD Routing; //!< A bitmap describing which denominations are routed to a recycling cassette. Is a valid value only for BB units
	}BillStatus;//!< Variable containing the most recent response to the STATUS REQUEST
*/
/**	\struct _Identification
	\brief	The _Identification struct contains identification of the device

*/
/*

	
*/
/**	\struct _PollResults
	\brief	The _PollResults struct containing 2 first bytes of the response to the POLL command

*/
/*


	_Cassete	Cassetes[16], //!< List of the cassettes 
				EscrCassete;  //!< Escrow cassette


};
*/

// The _PollResults struct containing 2 first bytes of the response to the POLL command
typedef struct{
  unsigned char Z1; //!< State
  unsigned char Z2; //!< State extension or substate
}TPollResults; //!< A variable keeping last POLL result

typedef struct{
  unsigned long Enabled; //!< A bitmap describing which bill types are enabled
  unsigned long Security; //!< A bitmap describing which bill types are processed in High Security mode
  unsigned long Routing; //!< A bitmap describing which denominations are routed to a recycling cassette. Is a valid value only for BB units
}TBillStatus;//!< Variable containing the most recent response to the STATUS REQUEST
        
        
typedef struct{
		// Identification command fields
		char PartNumber[16];//!< Firmware part number 
		char SN[13];//!< Device's serial number
		unsigned long long DS1;//!< Device's asset number
		// Extended identification command fiels
		char BVBootVersion[7];//!< Boot version of the validating head (is reported in response to EXTENDED IDENTIFICATION command)
		char BVVersion[21];//!< Firmware version of the validating head (is reported in response to EXTENDED IDENTIFICATION command)
		
		char BCCPUBoot[7];//!< Boot version of the central controller (is reported in response to EXTENDED IDENTIFICATION command)
		char BCCPUVersion[7];//!<Firmware version of the central controller (is reported in response to EXTENDED IDENTIFICATION command)

		char BCDispenserBoot[7];//!< Boot version of the dispenser (is reported in response to EXTENDED IDENTIFICATION command)
		char BCDispenserVersion[7];//!< Firmware version of the dispenser (is reported in response to EXTENDED IDENTIFICATION command)

		char BCCS1Boot[7];//!< Boot version of the cassette 1 (is reported in response to EXTENDED IDENTIFICATION command)
		char BCCS2Boot[7];//!< Boot version of the cassette 2 (is reported in response to EXTENDED IDENTIFICATION command)
		char BCCS3Boot[7];//!< Boot version of the cassette 3 (is reported in response to EXTENDED IDENTIFICATION command)
		char BCCSVersion[7];//!< Firmware version of the cassettes (is reported in response to EXTENDED IDENTIFICATION command)
}TIdent;//!< A variable containing current device identification
        
/**	\struct _BillRecord
	\brief	The _BillRecord struct represents a record in the bill table.

  The structure describes denomination of the bill, country or currency code and whether
  the bill is forwarded to the cassette. The bill table usually is an array of _BillRecord
  structures, where the position is representing a billtype.

*/
typedef struct{
		float Denomination;		//!< Denomination of the bill
		char sCountryCode[4];	//!< Country or currency code
		unsigned char bRouted;			//!< A bool variable specifiying whether the bill is forwarded to a cassette
}TBillRecord;


extern int CC_CmdReset(unsigned char Addr);
extern int CC_CmdPoll(unsigned char Addr, TPollResults *PollResults);
extern int CC_CmdStatus(unsigned char Addr, TBillStatus* BillStatus);
extern int CC_CmdIdentification(unsigned char Addr, TIdent* Ident);
extern int CC_CmdHold(unsigned char Addr);
extern int CC_CmdSetSecurity(unsigned long wS, unsigned char Addr);
extern int CC_CmdBillType(unsigned long enBill, unsigned long escBill, unsigned char Addr);
extern int CC_CmdPack(unsigned char Addr);
extern int CC_CmdReturn(unsigned char Addr);
extern int CC_CmdGetBillTable(unsigned char Addr, TBillRecord *BillTable);
extern int CC_CmdSetOptions(unsigned long dwOpt, unsigned char Addr);
extern int CC_CmdGetCRC32(unsigned long *dwCRC, unsigned char Addr);


#define PurgeComm             Uart1_Purge
#define COMPort_Send          Uart1_Send
#define COMPort_Recieve(x,y)  Uart1_Receive(x,y,CC_TIME_OUT)
#define Sleep(x)              OSTimeDly(x)


#endif //#ifndef _CCRSPROTOCOL_H_

