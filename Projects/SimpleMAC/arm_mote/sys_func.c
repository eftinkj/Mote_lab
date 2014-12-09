
#include "sys_func.h"

error_t sys_last_error;

#define SYS_MAX_MODULES	20

#define SYS_MOD_FREE	0
#define SYS_MOD_BUSY	1
#define SYS_MOD_POWERED	2
#define SYS_MOD_INIT	3
#define SYS_MOD_FAILED	255

typedef CODE void ( * XDATA sys_power_up_func_t)(uint8_t *context);
typedef CODE void ( * XDATA sys_init_func_t)(uint8_t* context);

typedef struct {
	uint8_t status;
	const char *name;
	sys_power_up_func_t	mod_power_up;
	sys_init_func_t		mod_init;
	uint8_t *sys_context;
} sys_module_t;

sys_module_t sys_modules[SYS_MAX_MODULES]={{SYS_MOD_FREE,},};
uint8_t sys_module_count_ = 0;

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

#define SYS_MODULE_SLOT_ERROR	0xFF
uint8_t sys_get_free_module_slot()
{
	assert( SYS_MOD_FREE == sys_modules[sys_module_count_].status);
	if ( sys_module_count_ < SYS_MAX_MODULES )
		return sys_module_count_++; // increment after returning
	else
		return SYS_MODULE_SLOT_ERROR;
}


int8_t sys_add_module( const char *name 
	, sys_power_up_func_t	mod_power_up
	, sys_init_func_t		mod_init
	, uint8_t *sys_context
		)
{
	uint8_t i = sys_get_free_module_slot();
	if( SYS_MODULE_SLOT_ERROR == i) return i;
	
	sys_modules[i].status = SYS_MOD_BUSY;
	sys_modules[i].name = name;
	sys_modules[i].mod_power_up = mod_power_up;
	sys_modules[i].mod_init = mod_init;
	sys_modules[i].sys_context = sys_context;
	return i;
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
