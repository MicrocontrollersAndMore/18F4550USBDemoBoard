// main.c

// includes ///////////////////////////////////////////////////////////////////////////////////////
#include<p18f4550.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include"USBStuff.h"

// chip config ////////////////////////////////////////////////////////////////////////////////////
					// clock options, see 18F4550 data sheet figure 2-1 "clock diagram" for explanation
#pragma config PLLDIV = 5				// 20 MHz external clock / PLL prescaler value of 5 = 4 MHz required input to PLL circuit
#pragma config CPUDIV = OSC1_PLL2		// non-PLL postscale / 1 OR PLL postscale / 2 for CPU clock speed, depending on FOSC setting below
#pragma config USBDIV = 2				// USB clock source = 96 MHz PLL source / 2, (full-speed USB mode)

					/*	if desired, could change this line to "FOSC = HS" & "oscillator postscaler" gate would be used 
						(not the "PLL postscaler" gate), CPU speed would be 20MHz, USB circuitry would still receive 48Mhz clock */
#pragma config FOSC = HSPLL_HS			// use high-speed external osc crystal, & use PLL postscaler gate to feed CPU (CPU speed = 48 MHz)

					// now the other less confusing options . . .
#pragma config FCMEN = OFF				// fail-safe clock monitor disabled
#pragma config IESO = OFF				// internal / external osc switchover bit disabled
#pragma config PWRT = OFF				// power-up timer disabled
#pragma config BOR = OFF				// brown-out reset disabled in hardware & software
#pragma config BORV = 3					// brown-out reset voltage bits, does not matter since brown-out is disabled 
#pragma config VREGEN = ON				// USB voltage regulator enabled
#pragma config WDT = OFF				// watchdog timer disabled
#pragma config WDTPS = 32768			// watchdog timer postscale, does not matter since watchdog timer is disabled
#pragma config CCP2MX = ON				// use RC1 (pin #16) as CCP2 MUX (this is the default pin for CCP2 MUX)
#pragma config PBADEN = OFF				// RB0, RB1, RB2, RB3, & RB4 are configured as digital I/O on reset
#pragma config LPT1OSC = OFF			// disable low-power option for timer 1 (timer 1 in regular mode)
#pragma config MCLRE = OFF				// master clear disabled, pin #1 is for VPP and / or RE3 use
#pragma config STVREN = ON				// stack full/underflow will cause reset
#pragma config LVP = OFF				// single-supply ICSP disabled
#pragma config ICPRT = OFF				// in-circuit debug/programming port (ICPORT) disabled, this feature is not available on 40 pin DIP package
#pragma config XINST = OFF				// instruction set extension and indexed addressing mode disabled (this is the default setting)
#pragma config DEBUG = OFF				// background debugger disabled, RA6 & RB7 configured as general purpose I/O pins
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF			// code protection bits off
#pragma config CPB = OFF				// boot block code protection off
#pragma config CPD = OFF				// data EEPROM code protection off
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF		// write protection bits off
#pragma config WRTC = OFF				// config registers write protection off
#pragma config WRTB = OFF				// boot block is not write protected
#pragma config WRTD = OFF				// data EEPROM is not write protected
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF	// table read protection bits off
#pragma config EBTRB = OFF				// boot block table read protection off

// global variables ///////////////////////////////////////////////////////////////////////////////
#pragma udata
extern volatile BDT_ENTRY g_buffDescTable[4];
extern BYTE g_USBDeviceState;
extern BYTE g_fromHostToDeviceBuffer[65];
extern BYTE g_fromDeviceToHostBuffer[65];

// function prototypes ////////////////////////////////////////////////////////////////////////////
void YourHighPriorityISRCode();
void YourLowPriorityISRCode();

void MainInit(void);
void MainTasks(void);

#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS 	0x1008
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS		0x1018

extern void _startup(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code REMAPPED_RESET_VECTOR = 0x1000
void _reset(void) {
	_asm goto _startup _endasm
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR(void) {
	_asm goto YourHighPriorityISRCode _endasm
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void Remapped_Low_ISR(void) {
	_asm goto YourLowPriorityISRCode _endasm
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code HIGH_INTERRUPT_VECTOR = 0x08
void High_ISR(void) {
	_asm goto REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS _endasm
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code LOW_INTERRUPT_VECTOR = 0x18
void Low_ISR(void) {
	_asm goto REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS _endasm
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt YourHighPriorityISRCode
void YourHighPriorityISRCode() {
	// check which int flag is set
	// service int
	// clear flag
	// etc.
} // return will be a "retfie fast"

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma interruptlow YourLowPriorityISRCode
void YourLowPriorityISRCode() {
	// check which int flag is set
	// service int
	// clear int flag
	// etc.
} // return will be a "retfie"


#pragma code
///////////////////////////////////////////////////////////////////////////////////////////////////
void main(void) {
	USBInit();				// in USBStuff.c
	MainInit();				// in main.c
	while(1) {
		USBTasks();			// in USBStuff.c
		MainTasks();			// in main.c
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainInit(void) {
	ADCON1 = 0x0F;			// set AN0 - AN12 to digital
	TRISB = 0x00;			// port B all output (LEDs)
	TRISD = 0x00;			// port D all output (LEDs)
	
	PORTB = 0x00;			// init all LEDs to off
	PORTD = 0x00;			//
	
	TRISAbits.TRISA1 = 1;			// set RA1 to input (switch 1)
	TRISAbits.TRISA2 = 1;			// set RA2 to input (switch 2)
	TRISAbits.TRISA3 = 1;			// set RA3 to input (switch 3)
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainTasks(void) {
	if(UCONbits.SUSPND == 1) {							// if in suspend mode
		LED2 = 0;										// show binary 1 on LEDs
		LED1 = 0;
		LED0 = 1;
	} else if(g_USBDeviceState == DETACHED_STATE) {		// if in detached state
		LED2 = 0;										// show binary 2 on LEDs
		LED1 = 1;
		LED0 = 0;
	} else if(g_USBDeviceState == ATTACHED_STATE) {		// if in attached state
		LED2 = 0;										// show binary 3 on LEDs
		LED1 = 1;
		LED0 = 1;
	} else if(g_USBDeviceState == POWERED_STATE) {		// if in powered state
		LED2 = 1;										// show binary 4 on LEDs
		LED1 = 0;
		LED0 = 0;
	} else if(g_USBDeviceState == DEFAULT_STATE) {		// if in default state
		LED2 = 1;										// show binary 5 on LEDs
		LED1 = 0;
		LED0 = 1;
	} else if(g_USBDeviceState == ADDRESS_STATE) {		// if in address state
		LED2 = 1;										// show binary 6 on LEDs
		LED1 = 1;
		LED0 = 0;
	} else if(g_USBDeviceState == CONFIGURED_STATE) {	// if in configured state
		LED2 = 1;										// show binary 7 on LEDs
		LED1 = 1;
		LED0 = 1;
		
		while(UIRbits.TRNIF != 1) { }						// wait here until any pending transactions are complete
		
		if(g_buffDescTable[2].STAT.UOWN == 0) {					// verify USB circuitry does not own from host USB buffer
			transferFromHostToDeviceViaEP1((BYTE*)&g_fromHostToDeviceBuffer[1], 64);		// get packet from host
			
			if(g_fromHostToDeviceBuffer[1] == TURN_LED3_ON) {							// if received 0x80
				LED3 = 1;																// turn LED3 on
			} else if(g_fromHostToDeviceBuffer[1] == TURN_LED3_OFF) {					// else if received 0x81
				LED3 = 0;																// turn LED3 off
			} else {
				// should never get here
			}
			
			if(g_fromHostToDeviceBuffer[2] == USE_DEBUG_LEDS) {			// if check box indicating to show drop down combo box value on LEDs is checked . . .
				showByteOnLEDs(g_fromHostToDeviceBuffer[3]);			// then show value from combo box on debug LEDs
			}
		}
		
		while(UIRbits.TRNIF != 1) { }						// wait here until any pending transactions are complete
		
		if(g_buffDescTable[3].STAT.UOWN == 0) {					// verify USB circuitry does not own to host USB buffer
			
			if(SWITCH1 == 1) {											// if switch 1 is not pressed
				g_fromDeviceToHostBuffer[1] = SWITCH1_NOT_PRESSED;	// set to host buffer to 0x01 (logic low)
			} else {														// else switch 1 must not be pressed so
				g_fromDeviceToHostBuffer[1] = SWITCH1_PRESSED;			// set to host buffer to 0x01 (logic low)
			}
			
			if(SWITCH2 == 1) {											// if switch 1 is not pressed
				g_fromDeviceToHostBuffer[2] = SWITCH2_NOT_PRESSED;	// set to host buffer to 0x01 (logic low)
			} else {														// else switch 1 must not be pressed so
				g_fromDeviceToHostBuffer[2] = SWITCH2_PRESSED;			// set to host buffer to 0x01 (logic low)
			}
			
			if(SWITCH3 == 1) {											// if switch 1 is not pressed
				g_fromDeviceToHostBuffer[3] = SWITCH3_NOT_PRESSED;	// set to host buffer to 0x01 (logic low)
			} else {														// else switch 1 must not be pressed so
				g_fromDeviceToHostBuffer[3] = SWITCH3_PRESSED;			// set to host buffer to 0x01 (logic low)
			}
			
			transferFromDeviceToHostViaEP1((BYTE*)&g_fromDeviceToHostBuffer[1], 64);		// send packet to host
		}
		
	}
}
