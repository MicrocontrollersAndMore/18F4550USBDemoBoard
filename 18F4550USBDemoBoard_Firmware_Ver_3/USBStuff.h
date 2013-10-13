// USBStuff.h

// #defines, typedefs, structs ////////////////////////////////////////////////////////////////////

		// pin #defines, see circuit diagram
#define LED0 PORTBbits.RB0
#define LED1 PORTBbits.RB1
#define LED2 PORTBbits.RB2
#define LED3 PORTBbits.RB3

#define LED4  PORTDbits.RD7
#define LED5  PORTDbits.RD6
#define LED6  PORTDbits.RD5
#define LED7  PORTDbits.RD4
#define LED8  PORTDbits.RD3
#define LED9  PORTDbits.RD2
#define LED10 PORTDbits.RD0
#define LED11 PORTDbits.RD1

#define SWITCH1 PORTAbits.RA1
#define SWITCH2 PORTAbits.RA2
#define SWITCH3 PORTAbits.RA3

#define TURN_LED3_OFF	0x80		// use with g_fromHostToDeviceBuffer[1]
#define TURN_LED3_ON	0x81		//

#define DO_NOT_USE_DEBUG_LEDS	0x00	// use with g_fromHostToDeviceBuffer[2]
#define USE_DEBUG_LEDS				0x01	//

#define SWITCH1_NOT_PRESSED		0x00	// use with g_fromDeviceToHostBuffer[1]
#define SWITCH1_PRESSED			0x01	//

#define SWITCH2_NOT_PRESSED		0x00	// use with g_fromDeviceToHostBuffer[2]
#define SWITCH2_PRESSED			0x01

#define SWITCH3_NOT_PRESSED		0x00	// use with g_fromDeviceToHostBuffer[3]
#define SWITCH3_PRESSED			0x01

		// USB device states - use with g_USBDeviceState (BYTE data type)
#define DETACHED_STATE		0x00
#define ATTACHED_STATE		0x01
#define POWERED_STATE		0x02
#define DEFAULT_STATE		0x04
#define ADR_PENDING_STATE	0x08
#define ADDRESS_STATE		0x10
#define CONFIGURED_STATE	0x20

		// for use with g_controlTransferState
#define CTRL_TRF_WAIT_SETUP		0
#define CTRL_TRF_TX				1
#define CTRL_TRF_RX				2

		// for use with g_shortPacketStatus
#define SHORT_PKT_NOT_USED	0
#define SHORT_PKT_PENDING	1
#define SHORT_PKT_SENT		2

		// for use with g_ctrlTrfSetupPkt.bDescriptorType
#define USB_DESCRIPTOR_DEVICE			0x01
#define USB_DESCRIPTOR_CONFIGURATION	0x02
#define USB_DESCRIPTOR_STRING			0x03
#define USB_DESCRIPTOR_INTERFACE		0x04
#define USB_DESCRIPTOR_ENDPOINT			0x05
#define USB_DESCRIPTOR_DEVICE_QUALIFIER 0x06
#define USB_DESCRIPTOR_OTHER_SPEED		0x07
#define USB_DESCRIPTOR_INTERFACE_POWER	0x08
#define USB_DESCRIPTOR_OTG				0x09

typedef enum _BOOL { FALSE = 0, TRUE = 1 } BOOL;

#define NULL 0

typedef unsigned char		BYTE;		// unsigned 8-bit int
typedef unsigned short		WORD;		// unsigned 16-bit int
typedef unsigned long		UINT32;		// unsigned 32-bit int
typedef unsigned long long	UINT64;		// unsigned 64-bit int

typedef union _WORD_VAL {		// 2 bytes
	WORD wordVal;
	struct {
		BYTE LB;
		BYTE HB;
	} byte;
} WORD_STRUCT;

typedef struct _USB_DEVICE_DESCRIPTOR {
	BYTE bLength;
	BYTE bDescriptorType;
	WORD bcdUSB;
	BYTE bDeviceClass;
	BYTE bDeviceSubClass;
	BYTE bDeviceProtocol;
	BYTE bMaxPacketSize0;
	WORD idVendor;
	WORD idProduct;
	WORD bcdDevice;
	BYTE iManufacturer;
	BYTE iProduct;
	BYTE iSerialNumber;
	BYTE bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

typedef struct _USB_HID_DSC {
	BYTE bLength;
	BYTE bDescriptorType;
	WORD bcdHID;
	BYTE bCountryCode;
	BYTE bNumDsc;
} USB_HID_DSC;

typedef struct {				// 4 bytes
	union {							// BDnSTAT imitation
		struct {					// CPU mode (UOWN == 0)
			unsigned BC8:1;
			unsigned BC9:1;
			unsigned BSTALL:1;
			unsigned DTSEN:1;
			unsigned INCDIS:1;
			unsigned KEN:1;
			unsigned DTS:1;
			unsigned UOWN:1;
		};
		struct {
			unsigned BC8:1;			// SIE mode (UOWN == 1)
			unsigned BC9:1;
			unsigned PID:4;
			unsigned :1;
			unsigned UOWN:1;
		};
		BYTE STATVal;
	} STAT;
	BYTE CNT;						// size of buffer in bytes
	BYTE *ADR;
} BDT_ENTRY;

#define USB_EP0_BUFF_SIZE		8

#define HID_INTERFACE_ID		0x00

#define HID_NUM_OF_DSC			1		// for use with config descriptor only

#define HID_RPT01_SIZE			29		// HID report 1 size in bytes, used with descriptors only

#define _EP_IN		0x80		// for use with descriptor declarations only
#define _EP_OUT		0x00		//

		// configuration attributes
#define _DEFAULT	(0x01 << 7)		// default value, bit 7 is set			used with descriptor declarations only
#define _SELF		(0x01 << 6)		// self-powered (supports if set)		why are these done as bit shifts ??!!
#define _RWU		(0x01 << 5)		// remote wakeup (supports if set)

		// endpoint transfer type, for use with config descriptor only
#define _CTRL	0x00		// control transfer
#define _ISO	0x01		// isochronous transfer
#define _BULK	0x02		// bulk transfer
#define _INT	0x03		// interrupt transfer

		// HID interface class code, for use with config descriptor only
#define HID_INTF		0x03

// every setup packet has 8 bytes, this structure allows direct access to the various members of the control transfer
typedef union _CTRL_TRF_SETUP {				// 8 bytes
	struct {
		BYTE bmRequestType;		// bit map request type
		BYTE bRequest;
		WORD wValue;
		WORD wIndex;
		WORD wLength;
	};
	struct {
		unsigned Recipient:5;
		unsigned RequestType:2;
		unsigned DataDir:1;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
	};
	struct {
		unsigned :8;
		unsigned :8;
		BYTE bDscIndex;
		BYTE bDescriptorType;
		WORD wLangID;
		unsigned :8;
		unsigned :8;
	};
	struct {
		unsigned :8;
		unsigned :8;
		BYTE bDevADR;
		BYTE bDevADRH;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
	};
	struct {
		unsigned :8;
		unsigned :8;
		BYTE bConfigurationValue;
		BYTE bCfgRSD;
		unsigned :8;
		unsigned :8;
		unsigned :8;
		unsigned :8;
	};
} CTRL_TRF_SETUP;

		// pipe structure, used to keep track of data that is sent out of the stack automatically
typedef struct {		// 7 bytes
	rom BYTE *bRom;
	rom WORD *wRom;
	BYTE busy;
	WORD wCount;
} IN_PIPE;

typedef struct {		// 5 bytes
	BYTE *bRam;	
	BYTE busy;
	WORD wCount;
} OUT_PIPE;

			// for use with g_crtlTrfSetupPkt.bRequest, see USB 2.0 spec. table 9-3
#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR		6
#define SET_DESCRIPTOR		7
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11
#define SYNCH_FRAME			12

			// for use with g_buffDescTable.STAT.PID, note that number is 4-bit
#define SETUP_TOKEN		0b1101	// 13 base 10
#define OUT_TOKEN		0b0001	//  1 base 10
#define IN_TOKEN		0b1001	//  9 base 10

			// for use with g_ctrlTrfSetupPkt.DataDir
#define HOST_TO_DEV		0
#define DEV_TO_HOST		1

			// for use with CTRL_TRF_SETUP.RequestType
#define STANDARD	0x00
#define CLASS		0x01
#define VENDOR		0x02

			// for use with g_ctrlTrfSetupPkt.Recipient
#define RECIPIENT_DEVICE		0
#define RECIPIENT_INTERFACE		1
#define RECIPIENT_ENDPOINT		2
#define RECIPIENT_OTHER			3

			// for use with g_ctrlTrfSetupPkt.bRequest
#define GET_REPORT		0x01
#define GET_IDLE		0x02
#define GET_PROTOCOL	0x03
#define SET_REPORT		0x09
#define SET_IDLE		0x0A
#define SET_PROTOCOL	0x0B

			// for use with g_ctrlTrfSetupPkt.bDescriptorType, and also in descriptor declarations
#define HID_DESCRIPTOR		0x21
#define REPORT_DESCRIPTOR	0x22
#define PHY_DESCRIPTOR		0x23

#define CLK_FREQ 48000000

// function prototypes ////////////////////////////////////////////////////////////////////////////
void USBInit(void);
void USBTasks(void);
void USBWakeFromSuspend(void);
void USBSuspend(void);
void USBStallHandler(void);
void USBEP0ControlTransfer(void);
void USBSetupControlTransfer(void);
void USBOutControlTransfer(void);
void USBInControlTransfer(void);
void USBCheckStandardRequest(void);
void USBCheckHIDRequest(void);
void USBFinishControlTransferStuff(void);
void USBControlTransferReceive(void);
void USBControlTransferTransmit(void);
void USBPrepareForNextSetupTransfer(void);
void transferFromHostToDeviceViaEP1(BYTE* pPacketData, BYTE packetLength);
void transferFromDeviceToHostViaEP1(BYTE* pPacketData, BYTE packetLength);
void showByteOnLEDs(BYTE byteToShow);
void delay(void);
