#ifndef _VEX_TANK_BASIC_CONTROL_PRV_H_
#define _VEX_TANK_BASIC_CONTROL_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/

//PWM Settings
#define VEX_TANK_PWM8_CONFIG_SIZE	2
#define VEX_TANK_MAX_PWMS	2

// ADC settings
#define VEX_TANK_ADC_NO_CHANNELS	3
#define VEX_TANK_ADC_AVERAGER		10
#define VEX_TANK_ADC_DIVIDER		10

#define VEX_TANK_LINE_SENSOR_LEFT	0
#define VEX_TANK_LINE_SENSOR_MIDDLE	1
#define VEX_TANK_LINE_SENSOR_RIGHT	2
#define VEX_TANK_ADC_THRESHOLD   1200



//LINE SENSOR STATES
//00000100
#define S_LEFT		  4

//%00000010
#define S_MIDDLE	  2

//%00000001
#define S_RIGHT		  1

//%00000110
#define S_LEFT_MID	  6

//%00000011
#define S_MID_RIGHT   3

//%00000111
#define S_L_MID_R	  7

//LINE POSITION STATES (LAST KNOWN LINE)
#define P_LOST	  0
#define P_LEFT	  1
#define P_MIDDLE  2
#define P_RIGHT	  3


#define TANK_LOST 20
#define TANK_LOST_GIVEUP 500


//TIME INTERVALS
#define VEX_TANK_DEFAULT_DELAY	2000
#define VEX_TANK_DEFAULT_LINE_FOLLOW_INTERVAL 5
#define VEX_TANK_CMD_TIMEOUT	65535 
#define VEX_TANK_DEFAULT_SEND_INTERVAL 10
#define VEX_TANK_TEST_DELAY 250

//PCA Modules
#define VEX_TANK_PWM_LEFT_MOTOR			0
#define VEX_TANK_PWM_RIGHT_MOTOR		1
#define VEX_TANK_PCA_ENCODER_LEFT_A		2
#define VEX_TANK_PCA_ENCODER_LEFT_B		3
#define VEX_TANK_PCA_ENCODER_RIGHT_A	4
#define VEX_TANK_PCA_ENCODER_RIGHT_B	5

//SPEED CONTROLS
#define VEX_TANK_IDLE		0xEA
#define VEX_TANK_MAX		0xFF

#define VEX_TANK_DEFAULT_SPEED 4

//TRIMS AUTOMATICALLY SET IN LINE FOLLOW MODE
//TRIMS ADDED VIA MATLAB IN MANUAL MODE
#define VEX_TANK_TRIM_LEFT 			2
#define VEX_TANK_TRIM_RIGHT 		0

#define VEX_TANK_VEER_SPEED_HARD	5
#define VEX_TANK_VEER_SPEED_LARGE	1
#define VEX_TANK_VEER_SPEED_SMALL	1




//Commands
#define VEX_TANK_CMD_STOP			0x00
#define VEX_TANK_SET_SPEEDS			0x01
#define VEX_TANK_SET_PWM_FREQ		0x02
#define VEX_TANK_START_LINE_FOLLOW	0x03
#define VEX_TANK_MANUAL_CNTRL 		0x04
#define VEX_TANK_TURN_180			0x05
#define VEX_TANK_LINE_FOLLOW_T180	0x06
#define VEX_TANK_SET_DEFAULT_SPEED	0x07


//request for space in module(data to be sent)
#define VEX_TANK_REPORT_V1_SIZE		3
#define VEX_TANK_REPORT_V2_SIZE		3
//Application
#define VEX_TANK_REPORT_V1			3
#define VEX_TANK_REPORT_V2			3

//used to chop up module into multiple messages
#define PKT_MODULE_LEN_VEX_TANK_REPORT_V1 3
#define PKT_MODULE_LEN_VEX_TANK_REPORT_V2 3


/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


extern rtc_tick_t	vex_tank_timeout;
extern rtc_tick_t	vex_tank_command_timeout;
extern rtc_tick_t	vex_tank_send_timeout;

extern uint16_t     vex_tank_send_interval;
extern uint8_t		vex_tank_position;
extern uint8_t		vex_tank_lost;
extern uint8_t		vex_tank_default_speed_left;
extern uint8_t		vex_tank_default_speed_right;
extern uint8_t		vex_tank_line_follow_mode;

//sbit VEX_TANK_HBRIDGE_ENABLE_LED=P3^3;
//sbit VEX_TANK_HBRIDGE_ENABLE=P3^0;
//0=Enable,  1=Disabled

/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/



/**
  * qrhelo_update_pwm() - Update pwm's for each rotor
  */

void vex_tank_update_pwm(uint8_t LM_speed, uint8_t RM_speed);

/**
  * vex_tank_veer_left() - steer left to find line
  *	
  */
void vex_tank_veer_left(uint8_t veer_size);

/**
  * vex_tank_veer_right() - steer right to find line
  *	
  */
void vex_tank_veer_right(uint8_t veer_size);

/**
  * vex_tank_line_follow(unsigned char line_status) - function for tracking a black line
  *	
  */
void vex_tank_line_follow(uint8_t line_status);

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _VEX_TANK_BASIC_CONTROL_PRV_H_