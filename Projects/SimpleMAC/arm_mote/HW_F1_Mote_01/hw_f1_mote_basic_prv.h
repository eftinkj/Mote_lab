#ifndef _HW_F1_MOTE_BASIC_PRV_H_
#define _HW_F1_MOTE_BASIC_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"
#include "HW_LIB/spi_pub.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/


#define HW_F1_MOTE_DEFAULT_DELAY	2000
#define HW_F1_MOTE_CMD_TIMEOUT	1000

// Read power (Watt-Hour) every 250ms
#define HW_F1_MOTE_READ_POWER_DEFAULT_TIMEOUT	5000


#define	MON_POWER_REPORT_V1_SIZE	30



#define HW_F1_LED_OFF	1
#define HW_F1_LED_ON	0


// Original direct connection between 8051 and ADE
#define HW_F1_MOTE_ADE_CS_DISABLE 1
#define HW_F1_MOTE_ADE_CS_ENABLE	0


/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

//extern uint8_t		truck_state_test;
//extern rtc_tick_t	mon_power_timeout;
//extern rtc_tick_t	mon_power_read_power_timeout;
//extern rtc_tick_t	truck_command_timeout;
extern uint8_t		mon_power_read_power_enabled;

#ifdef _ENABLE_SPI_MODULE_
	extern spi_event_t xdata hw_f1_mote_event_flag;
	extern spi_event_t xdata hw_f1_mote_even_mask;
#endif // _ENABLE_SPI_MODULE_


#ifdef __KEIL__
	sbit CS_MUX	= P0^3;
#else // __KEIL__
//	#define CS_MUX	P0_3
	#define CS_MUX	NSSMD0
#endif // else/not __KEIL__


/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/**
  * truck_move_break() - apply break (stop but keep direction)
  */
//void truck_move_break();

/**
* hw_f1_mote_test_ver() - reads version register
*/
bit hw_f1_mote_test_ver();


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _HW_F1_MOTE_BASIC_PRV_H_
