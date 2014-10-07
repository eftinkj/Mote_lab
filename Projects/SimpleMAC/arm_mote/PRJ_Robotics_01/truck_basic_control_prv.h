#ifndef _TRUCK_BASIC_CONTROL_PRV_H_
#define _TRUCK_BASIC_CONTROL_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"

#ifdef _ENABLE_ROBOTICS_01_

#include "HW_LIB/rtc.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/
#define TRUCK_PWM8_CONFIG_SIZE	7
#define TRUCK_MAX_PWMS	5

#define TRUCK_DEFAULT_DELAY	2000
//#define TRUCK_CMD_TIMEOUT	500
#define TRUCK_CMD_TIMEOUT	1000

#ifdef _OLD_TRUCK_
	#define TRUCK_PWM_RIGHT	0
	#define TRUCK_PWM_LEFT	1
	#define TRUCK_PWM_TURBO	2
	#define TRUCK_PWM_FORWARD	3
	#define TRUCK_PWM_BACK	4
#else
	#define TRUCK_PWM_RIGHT	4
	#define TRUCK_PWM_LEFT	3
	#define TRUCK_PWM_TURBO	2
	#define TRUCK_PWM_FORWARD	0
	#define TRUCK_PWM_BACK	1
#endif // _OLD_TRUCK_

#define TRUCK_TURN_MAX	0xFF
#define TRUCK_MOVE_MAX	0xFF
#define TRUCK_MOVE_HALF	0x80

#define TRUCK_TEST_IDLE			0
#define TRUCK_TEST_TURN_LEFT	1
#define TRUCK_TEST_TURN_RIGHT	2
#define TRUCK_TEST_FORWARD		3
#define TRUCK_TEST_TURBO_FORWARD	4
#define TRUCK_TEST_BACK			5
#define TRUCK_TEST_FORWARD_TURN_RIGHT	6
#define TRUCK_TEST_ENDED		255





/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

extern uint8_t		truck_state_test;
extern rtc_tick_t	truck_timeout;
extern rtc_tick_t	truck_command_timeout;
extern uint8_t		truck_test_enabled;

/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/**
  * truck_move_break() - apply break (stop but keep direction)
  */
void truck_move_break();


/**
  * truck_move_forward(speed, turbo) - starts movement forward 
  */
void truck_move_forward(uint8_t speed, uint8_t turbo);


/**
  * truck_move_back(speed) - starts movement backward
  */
void truck_move_back(uint8_t speed);


/**
  * truck_steer_stright() - turns weels stright 
  */
void truck_steer_stright();


/**
  * truck_steer_left(uint8_t strength) - turns left with a given strength 
  */
void truck_steer_left(uint8_t strength);


/**
  * truck_steer_right(uint8_t strength) - turns right with a given strength 
  */
void truck_steer_right(uint8_t strength);


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################


#endif // _ENABLE_ROBOTICS_01_


#endif // _TRUCK_BASIC_CONTROL_PRV_H_