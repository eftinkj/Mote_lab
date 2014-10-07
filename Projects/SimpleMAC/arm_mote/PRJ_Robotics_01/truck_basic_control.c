
#include "defs/esnl_pub.h"

#ifdef _ENABLE_ROBOTICS_01_

#include "truck_basic_control_prv.h"
#include "truck_basic_control_pub.h"


#include "HW_LIB/pwm8_gen.h"

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

uint8_t 	truck_state_test;
rtc_tick_t	truck_timeout;
rtc_tick_t	truck_command_timeout;
uint8_t		truck_test_enabled;

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/



/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

/**
  * truck_init() - sets up truck module
  */
void truck_init ( void )
{
	// setup PWM8
	uint8_t i;
	uint8_t	temp[TRUCK_PWM8_CONFIG_SIZE];
	pwm8_config_t *pwm_conf = (pwm8_config_t *)(temp);
	pwm8_duty_t *duty = (pwm8_duty_t *)(&(pwm_conf->idle));

	pwm_conf->ver = PWM8_CONFIG_VER_01;
	pwm_conf->length = TRUCK_MAX_PWMS;
	for(i=0; i < TRUCK_MAX_PWMS; i++)
	{
//		duty[i] = PWM8_DUTY_OFF;	// for truck turn the PWM's off by default
		duty[i] = 0;	// for truck turn the PWM's off by default
	}

	pwm8_init(pwm_conf);
	
	truck_test_enabled = false;
	truck_timeout = rtc_get_ticks() + TRUCK_DEFAULT_DELAY;
	truck_state_test = TRUCK_TEST_IDLE;
	truck_command_timeout = RTC_OVERFLOW_TIMER_VALUE;
}



/**
  * truck_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void truck_loop ( void )
{
	if (truck_timeout < rtc_get_ticks())
	{
		truck_timeout = rtc_get_ticks() + TRUCK_DEFAULT_DELAY;
		switch (truck_state_test)
		{
			case TRUCK_TEST_IDLE:
				pwm8_make_all_idle();
				if (0 != truck_test_enabled)
				{
					truck_state_test = TRUCK_TEST_TURN_LEFT ;
				}
				else
				{
					truck_state_test = TRUCK_TEST_ENDED ;
				}
				break;
			case TRUCK_TEST_TURN_RIGHT:
				pwm8_make_idle(TRUCK_PWM_LEFT);
				pwm8_set_duty(TRUCK_PWM_RIGHT, TRUCK_TURN_MAX);
				truck_state_test = TRUCK_TEST_FORWARD;
				break;

			case TRUCK_TEST_TURN_LEFT:
				pwm8_make_idle(TRUCK_PWM_RIGHT);
				pwm8_set_duty(TRUCK_PWM_LEFT, TRUCK_TURN_MAX);
				truck_state_test = TRUCK_TEST_TURN_RIGHT ;
				break;

			case TRUCK_TEST_FORWARD:
				pwm8_make_idle(TRUCK_PWM_BACK);
				pwm8_make_idle(TRUCK_PWM_TURBO);
				pwm8_set_duty(TRUCK_PWM_FORWARD, TRUCK_MOVE_HALF);
				truck_state_test = TRUCK_TEST_TURBO_FORWARD;
				break;

			case TRUCK_TEST_TURBO_FORWARD:
				pwm8_make_idle(TRUCK_PWM_BACK);
				pwm8_set_duty(TRUCK_PWM_FORWARD, TRUCK_MOVE_MAX);
//				pwm8_set_duty(TRUCK_PWM_TURBO, TRUCK_MOVE_MAX);
				truck_state_test = TRUCK_TEST_IDLE;
				break;
			case TRUCK_TEST_ENDED:
				pwm8_make_all_idle();
				truck_timeout = RTC_OVERFLOW_TIMER_VALUE;
				break;
			default:
				pwm8_make_all_idle();
				truck_state_test = TRUCK_TEST_IDLE;
		}	
	}

	if (truck_command_timeout < rtc_get_ticks())
	{
		// 
		pwm8_make_all_idle();
		truck_command_timeout = RTC_OVERFLOW_TIMER_VALUE;
	}
}


/**
  * truck_execute_command(packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void truck_execute_command ( uint8_t *packet)
{	
	truck_command_packet_v1_t *cmd = (truck_command_packet_v1_t*)packet;
	switch (cmd->command)
	{
		case TRUCK_CMD_KILLSWITCH:
			pwm8_make_all_idle();
			return;
			break;
		case TRUCK_CMD_STOP:
			pwm8_make_all_idle();
			break;
		case TRUCK_CMD_MOVE_FORWARD:
			truck_move_forward(cmd->speed, cmd->turbo);
			break;
		case TRUCK_CMD_MOVE_BACK:
			truck_move_back(cmd->speed);
			break;
		case TRUCK_CMD_FREQ_SET:
			pwm8_set_freq(cmd->speed);
			return;
	}
	truck_command_timeout = rtc_get_ticks() + TRUCK_CMD_TIMEOUT;

	if (TRUCK_DIR_LEFT == cmd->direction)
	{
		truck_steer_left( TRUCK_TURN_MAX );			
	}
	else if (TRUCK_DIR_RIGHT == cmd->direction)
	{
		truck_steer_right( TRUCK_TURN_MAX );			
	}
	else
	{
		truck_steer_stright();			
	}
//	truck_command_timeout = rtc_get_ticks() + TRUCK_CMD_TIMEOUT;
}




/**
  * truck_move_break() - apply break (stop but keep direction)
  */
void truck_move_break()
{
	pwm8_make_idle(TRUCK_PWM_BACK);
	pwm8_make_idle(TRUCK_PWM_FORWARD);
	pwm8_make_idle(TRUCK_PWM_TURBO);
}


/**
  * truck_move_forward(speed, turbo) - starts movement forward 
  */
void truck_move_forward(uint8_t speed, uint8_t turbo)
{
	pwm8_make_idle(TRUCK_PWM_BACK);
	pwm8_set_duty(TRUCK_PWM_FORWARD, speed);
	pwm8_set_duty(TRUCK_PWM_TURBO, turbo);
}

/**
  * truck_move_back(speed) - starts movement backward
  */
void truck_move_back(uint8_t speed)
{
	pwm8_make_idle(TRUCK_PWM_FORWARD);
	pwm8_make_idle(TRUCK_PWM_TURBO);
	pwm8_set_duty(TRUCK_PWM_BACK, speed);
}

/**
  * truck_steer_stright() - turns weels stright 
  */
void truck_steer_stright()
{
	pwm8_make_idle(TRUCK_PWM_RIGHT);			
	pwm8_make_idle(TRUCK_PWM_LEFT);			
}

/**
  * truck_steer_left(uint8_t strength) - turns left with a given strength 
  */
void truck_steer_left(uint8_t strength)
{
	pwm8_make_idle(TRUCK_PWM_RIGHT);			
	pwm8_set_duty(TRUCK_PWM_LEFT, strength);			
}

/**
  * truck_steer_right(uint8_t strength) - turns right with a given strength 
  */
void truck_steer_right(uint8_t strength)
{
	pwm8_make_idle( TRUCK_PWM_LEFT );			
	pwm8_set_duty( TRUCK_PWM_RIGHT, strength);			
}

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################

#endif // _ENABLE_ROBOTICS_01_