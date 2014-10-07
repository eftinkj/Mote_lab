#ifndef _BOT_BASIC_CONTROL_PRV_H_
#define _BOT_BASIC_CONTROL_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "rtc.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/
#define BOT_PWM8_CONFIG_SIZE	7
#define BOT_MAX_PWMS	4

#define BOT_DEFAULT_DELAY	2000
//#define BOT_CMD_TIMEOUT	500
//unit in msec
#define BOT_CMD_TIMEOUT	1000 
#define BOT_DEFAULT_SEND_INTERVAL 10
#define BOT_TEST_DELAY 250


#define BOT_PWM_RIGHT_MOTOR	0
#define BOT_PWM_LEFT_MOTOR	1
#define BOT_RIGHT_ENCODER	4
#define BOT_LEFT_ENCODER    5


//Assumes 6 Volt max accross motors
#define BOT_MOVE_MAX	0x96   
#define BOT_MOVE_HALF	0x4B

#define BOT_TEST_IDLE					0
#define BOT_TEST_TURN_LEFT				1
#define BOT_TEST_TURN_LEFT_AFTER_DELAY 	2
#define BOT_TEST_TURN_RIGHT				3
#define BOT_TEST_TURN_RIGHT_AFTER_DELAY	4
#define BOT_TEST_FORWARD				5
#define BOT_TEST_FORWARD_AFTER_DELAY	6
#define BOT_TEST_BACK					7
#define BOT_TEST_BACK_AFTER_DELAY		8
#define BOT_TEST_ENDED					255

#define BOT_FORWARD				0x00
#define BOT_REVERSE				0x88
#define BOT_LEFT_REV_RIGHT_FOR  0x80
#define BOT_LEFT_FOR_RIGHT_REV  0x08

//request for space in module(data to be sent)
#define BOT_REPORT_V1_SIZE		3
#define BOT_REPORT_V2_SIZE		3
//Application
#define BOT_REPORT_V1			2
#define BOT_REPORT_V2			3

//used to chop up module into multiple messages
#define PKT_MODULE_LEN_BOT_REPORT_V1 3
#define PKT_MODULE_LEN_BOT_REPORT_V2 3


/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

extern uint8_t		bot_state_test;
extern rtc_tick_t	bot_timeout;
extern rtc_tick_t	bot_command_timeout;
extern rtc_tick_t	bot_bot_send_timeout;
extern uint8_t		bot_test_enabled;
extern uint8_t		bot_cur_direction;
extern uint16_t     bot_send_interval;

//LEFT MOT0R-Forward_bar/Reverse, F=0, R=1
//RIGHT MOTOR-Forward_bar/Reverse, F=0, R=1
sbit LEFT_DIR=P3^3;
sbit RIGHT_DIR=P3^2;
//#define LEFT_DIR P3_3	
//#define RIGHT_DIR P3_2	
/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/**
  * bot_move_break() - apply break (stop but keep direction)
  */
void bot_move_break();

/**
  * bot_update_kinmeatics() - Update position and velocity based on encoder readins
  */
void bot_update_kinematics();

/**
  * bot_update_reference() - Update position and velocity of reference cart
  */
void bot_update_reference();


/**
  * bot_test_execute() - executes a test squence when appropriate flag is set
  *	
  */
void bot_test_execute();


/**
  * 	bot_set_speed(L_speed, R_speed) updates new motor speeds
  */
void bot_set_speed(uint8_t L_speed, uint8_t R_speed, uint8_t direction);


/**
  * 	bot_set_direction(uint8_t right_dir) updates direction of motors
  */
void bot_set_direction(uint8_t direction);


/*
* uint8_t bot_send_packet_v1(uint8_t BOT_PACKET_TYPE) creates a packet of current sensor data
*		which is queued for transmission in the end, BOT_PACKET_TYPE specifies packet type
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t bot_send_packet();




// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _BOT_BASIC_CONTROL_PRV_H_