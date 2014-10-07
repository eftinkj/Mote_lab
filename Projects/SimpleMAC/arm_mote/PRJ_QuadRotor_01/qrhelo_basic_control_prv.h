#ifndef _QRHELO_BASIC_CONTROL_PRV_H_
#define _QRHELO_BASIC_CONTROL_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/
#define QRHELO_PWM8_CONFIG_SIZE	7
#define QRHELO_MAX_PWMS	6

#define QRHELO_DEFAULT_DELAY	2000

#define QRHELO_CMD_TIMEOUT	65535 
#define QRHELO_DEFAULT_SEND_INTERVAL 10
#define QRHELO_TEST_DELAY 250


#define QRHELO_PWM_ROTOR_1	2
#define QRHELO_PWM_ROTOR_2	3
#define QRHELO_PWM_ROTOR_3	4
#define QRHELO_PWM_ROTOR_4  5


#define QRHELO_STOP				0x00
#define QRHELO_SET_SPEEDS		0x01

//request for space in module(data to be sent)
#define QRHELO_REPORT_V1_SIZE		5
#define QRHELO_REPORT_V2_SIZE		5
//Application
#define QRHELO_REPORT_V1			5
#define QRHELO_REPORT_V2			5

//used to chop up module into multiple messages
#define PKT_MODULE_LEN_QRHELO_REPORT_V1 5
#define PKT_MODULE_LEN_QRHELO_REPORT_V2 5


/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


extern rtc_tick_t	qrhelo_timeout;
extern rtc_tick_t	qrhelo_command_timeout;
extern rtc_tick_t	qrhelo_send_timeout;
extern uint16_t     qrhelo_send_interval;

sbit QRHELO_HBRIDGE_ENABLE_LED=P3^3;
sbit QRHELO_HBRIDGE_ENABLE=P3^0;

/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/



/**
  * qrhelo_update_pwm() - Update pwm's for each rotor
  */

void qrhelo_update_pwm(uint8_t R1_speed, uint8_t R2_speed, uint8_t R3_speed, uint8_t R4_speed);





// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _QRHELO_BASIC_CONTROL_PRV_H_