
#include "sys_func.h"

error_t sys_last_error;


///////////////////////////////////////////////
//Decleration of Variables
///////////////////////////////////////////////
#ifndef _ARM_
  unsigned char XDATA SWRSEF = 0x10;	// software reset


#ifdef __KEIL__
	///////////////////////////////////////////////
	//Reset Resources
	sbit XBEE_RESET_NOT = P2 ^ 5;
	sbit XBEE_DTR_NOT = P2 ^ 6;
	///////////////////////////////////////////////

	//LED Resources
	//sbit RLED = P2 ^ 2;
	//	sbit GLED = P2 ^ 3;
	sbit YLED = P2 ^ 4;
#endif

#ifdef __SDCC__
	///////////////////////////////////////////////
	//Reset Resources
	#define XBEE_RESET_NOT P2_5
	#define XBEE_DTR_NOT P2_6
	///////////////////////////////////////////////
	//	#define RLED P2_2
	//	#define GLED P2_3
	#define YLED P2_6
#endif

#endif // _ARM_


/**
* sys_init(config) - handles initialization of system functions and variables
*/
void sys_init(void *config)
{
	if (NULL == config)
	{
		// Missing configuration -> use the default settings
	}
	sys_last_error = SYS_ERROR_NONE;
}




/**
* sys_reset() - resets the mote
*/
void sys_reset() 
{ 
	/* Reset XBee */
	/*XBEE_RESET_NOT = 0;*/

#ifndef _ARM_
	// reset 8051
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = 0;
#endif // _SFRPAGE_EXIST_
	RSTSRC |= SWRSEF;	
#endif // _ARM_
}



/**
* sys_error(e) - handles error anouncing
*/
void sys_error(error_t e)
{
	sys_last_error = e;
#ifdef _SYS_ERROR_LOCKING_LED_
	sys_error_led(e);
#endif // _SYS_ERROR_LOCKING_LED_
}



#define LED_LONG_DELAY_TIME 40000

void sys_led_delay()
{
	uint16_t i = 0;
	while ( i++ < LED_LONG_DELAY_TIME );
}

/**
* sys_error_led(repeat) - emits "repeat" number of LED blink series 
*    (with decreasing delay between blinks)
*		NOTE: blocks CPU until finished (except for interrupts)
*/
void sys_error_led ( uint8_t repeat )
{
	uint8_t j = 0;
	for ( j = 0; j < repeat; j++ )
	{
#ifndef _ARM_
		YLED = 0;
#endif // _ARM_
		sys_led_delay();
		sys_led_delay();
		sys_led_delay();
		sys_led_delay();
#ifndef _ARM_
		YLED = 1;
#endif // _ARM_
		sys_led_delay();
		sys_led_delay();
		sys_led_delay();
		sys_led_delay();
#ifndef _ARM_
		YLED = 0;
#endif // _ARM_
		sys_led_delay();
		sys_led_delay();
		sys_led_delay();
#ifndef _ARM_
		YLED = 1;
#endif // _ARM_
		sys_led_delay();
		sys_led_delay();
		sys_led_delay();
#ifndef _ARM_
		YLED = 0;
#endif // _ARM_
		sys_led_delay();
		sys_led_delay();
#ifndef _ARM_
		YLED = 1;
#endif // _ARM_
		sys_led_delay();
		sys_led_delay();
#ifndef _ARM_
		YLED = 0;
#endif // _ARM_
		sys_led_delay();
#ifndef _ARM_
		YLED = 1;
#endif // _ARM_
		sys_led_delay();
#ifndef _ARM_
		YLED = 0;
#endif // _ARM_
		sys_led_delay();
	}
}
