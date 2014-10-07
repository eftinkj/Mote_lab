#include "defs/esnl_pub.h"

#ifdef _ENABLE_ROBOTICS_02_

#include "bot_basic_control_prv.h"
#include "bot_basic_control_pub.h"
#include "pwm8_gen.h"
#include "API_frames.h"
#include "tsp_common_pub.h"
#include "sys_func.h"
#include "Application.h"


/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

uint8_t 	bot_state_test;
rtc_tick_t	bot_timeout;
rtc_tick_t	bot_command_timeout;
rtc_tick_t  bot_send_timeout;
uint8_t		bot_test_enabled;
uint8_t		bot_cur_direction;
uint16_t    bot_send_interval;



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
  * bot_init() - sets up bot module
  */
void bot_init ( void )
{
	// setup PWM8
	uint8_t i;
	uint8_t	temp[BOT_PWM8_CONFIG_SIZE];
	pwm8_config_t *pwm_conf = (pwm8_config_t *)(temp);
	pwm8_duty_t *duty = (pwm8_duty_t *)(&(pwm_conf->idle));
//	Init_Device();
	pwm_conf->ver = PWM8_CONFIG_VER_01;
	pwm_conf->length = BOT_MAX_PWMS;
	for(i=0; i < BOT_MAX_PWMS; i++)
	{
//		duty[i] = PWM8_DUTY_OFF;	// for bot turn the PWM's off by default
		duty[i] = 0;	// for bot turn the PWM's off by default
	}
//	bot_set_direction(BOT_FORWARD);
	bot_set_direction(BOT_REVERSE);
	pwm8_init(pwm_conf);
	pwm8_make_all_idle();
	
	bot_test_enabled = true;
	bot_timeout = rtc_get_ticks() + BOT_DEFAULT_DELAY;
	bot_state_test = BOT_TEST_IDLE;
	bot_command_timeout = RTC_OVERFLOW_TIMER_VALUE;
	bot_send_timeout=BOT_DEFAULT_SEND_INTERVAL; //stores next send
	bot_send_interval=BOT_DEFAULT_SEND_INTERVAL;//allows send rate to be modified
}



/**
  * bot_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void bot_loop ( void )
{
	if (bot_timeout < rtc_get_ticks())
	{
		bot_timeout = rtc_get_ticks() + BOT_DEFAULT_DELAY;
		bot_test_execute();
	}

	if (bot_command_timeout < rtc_get_ticks())
	{
		// 
		pwm8_make_all_idle();
		bot_command_timeout = RTC_OVERFLOW_TIMER_VALUE;
	}

	//do everytime through;
//	bot_update_kinematics();//calculate where you are
//	bot_update_reference();//calculate where you think you should be
//	bot_update_control();//calculate appropriate control to get there

	if (bot_send_timeout < rtc_get_ticks())//send readings at specified interval
	{
		bot_send_timeout=rtc_get_ticks() + bot_send_interval;
		bot_send_packet();
	}
}


/**
  * bot_test_execute() - executes a test squence when appropriate flag is set
  *	
  */
void bot_test_execute()
{
	switch (bot_state_test)
			{
				case BOT_TEST_IDLE:
					pwm8_make_all_idle();
					if (0 != bot_test_enabled)
					{
						bot_state_test = BOT_TEST_TURN_LEFT ;
					}
					else
					{
						bot_state_test = BOT_TEST_ENDED ;
					}
					break;

				case BOT_TEST_TURN_LEFT:
					pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
					pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);
					bot_set_direction(BOT_LEFT_FOR_RIGHT_REV);

					pwm8_set_duty(BOT_PWM_LEFT_MOTOR, BOT_MOVE_HALF);
					pwm8_set_duty(BOT_PWM_RIGHT_MOTOR, BOT_MOVE_HALF);
				
					bot_timeout = rtc_get_ticks()+BOT_TEST_DELAY;

					bot_state_test = BOT_TEST_TURN_LEFT_AFTER_DELAY;
					break;

				case BOT_TEST_TURN_LEFT_AFTER_DELAY:

					pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
					pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);

					bot_state_test = BOT_TEST_TURN_RIGHT ;
					break;

				case BOT_TEST_TURN_RIGHT:
					pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
					pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);
					bot_set_direction(BOT_LEFT_REV_RIGHT_FOR);

					pwm8_set_duty(BOT_PWM_LEFT_MOTOR, BOT_MOVE_HALF);
					pwm8_set_duty(BOT_PWM_RIGHT_MOTOR, BOT_MOVE_HALF);
				
					bot_timeout = rtc_get_ticks()+BOT_TEST_DELAY;

					bot_state_test = BOT_TEST_TURN_RIGHT_AFTER_DELAY;
					break;

				case BOT_TEST_TURN_RIGHT_AFTER_DELAY:
					pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
					pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);

					bot_state_test = BOT_TEST_FORWARD;
					break;

				case BOT_TEST_FORWARD:
					pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
					pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);
					bot_set_direction(BOT_FORWARD);

					pwm8_set_duty(BOT_PWM_LEFT_MOTOR, BOT_MOVE_HALF);
					pwm8_set_duty(BOT_PWM_RIGHT_MOTOR, BOT_MOVE_HALF);
					bot_timeout = rtc_get_ticks()+BOT_TEST_DELAY;

					bot_state_test = BOT_TEST_FORWARD_AFTER_DELAY;
					break;
			
				case BOT_TEST_FORWARD_AFTER_DELAY:
					pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
					pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);
					bot_state_test = BOT_TEST_BACK;
					break;

				case BOT_TEST_BACK:
					pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
					pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);
					bot_set_direction(BOT_REVERSE);

					pwm8_set_duty(BOT_PWM_LEFT_MOTOR, BOT_MOVE_HALF);
					pwm8_set_duty(BOT_PWM_RIGHT_MOTOR, BOT_MOVE_HALF);

					bot_timeout = rtc_get_ticks()+BOT_TEST_DELAY;

					bot_state_test = BOT_TEST_BACK_AFTER_DELAY;
					break;

				case BOT_TEST_BACK_AFTER_DELAY:
					pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
					pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);
					bot_state_test = BOT_TEST_IDLE;
					break;

				case BOT_TEST_ENDED:
					pwm8_make_all_idle();
					bot_timeout = RTC_OVERFLOW_TIMER_VALUE;
					break;
				default:
					pwm8_make_all_idle();
					bot_state_test = BOT_TEST_IDLE;
			}	
}

/**
  * bot_execute_command(packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void bot_execute_command ( uint8_t *packet)
{	
	bot_command_packet_v1_t *cmd = (bot_command_packet_v1_t*)packet;

		switch (cmd->command)
		{

			case BOT_CMD_STOP:
				pwm8_make_all_idle();
			break;

			case BOT_CMD_G0:

			break;

			case BOT_CMD_UPDATE:

			break;

			case BOT_CMD_UPDATE_REF:

			break;

			case BOT_CMD_SPEED_SET:
				bot_set_speed(cmd->L_speed, cmd->R_speed,cmd->direction);
				break;
//			case BOT_CMD_FREQ_SET:
//				pwm8_set_freq(cmd->direction);
//				return;
//			break;

			case BOT_CMD_TEST:
				bot_test_enabled=1;
				bot_state_test = BOT_TEST_IDLE;

			break;

			case BOT_CMD_SEND:
				bot_send_timeout = rtc_get_ticks();
			break;
		}

	bot_command_timeout = rtc_get_ticks() + BOT_CMD_TIMEOUT;

}




/**
  * bot_move_break() - apply break (stop but keep direction)
  */
void bot_move_break()
{
	pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);
	pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
}


/**
  * 	bot_set_speed(L_speed, R_speed) updates new motor speeds
  */
void bot_set_speed(uint8_t L_speed, uint8_t R_speed, uint8_t direction)
{
	pwm8_make_idle(BOT_PWM_LEFT_MOTOR);
	pwm8_make_idle(BOT_PWM_RIGHT_MOTOR);
	bot_set_direction(direction);
	pwm8_set_duty(BOT_PWM_LEFT_MOTOR, L_speed);
	pwm8_set_duty(BOT_PWM_RIGHT_MOTOR, R_speed);
}


/**
  * 	bot_set_direction(uint8_t right_dir) updates direction of motors
  */
void bot_set_direction(uint8_t direction)
{
	bot_cur_direction=direction;
	switch(direction&0x88)
	{
		case BOT_FORWARD:
			LEFT_DIR=0;
			RIGHT_DIR=0;

		break;

		case BOT_LEFT_FOR_RIGHT_REV:
			LEFT_DIR=0;
			RIGHT_DIR=1;
		break;

		case BOT_LEFT_REV_RIGHT_FOR:
			LEFT_DIR=1;
			RIGHT_DIR=0;
		break;

		case BOT_REVERSE:
			LEFT_DIR=1;
			RIGHT_DIR=1;
		break;

		default://should never occur

		break;
	}
}

/*
* uint8_t bot_send_packet() creates a packet of current sensor data
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t bot_send_packet()
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
	uint8_t size = tsp_reserve_packet(BOT_REPORT_V1_SIZE, &pkt_id);
	uint8_t original_size = 0;
	if (0 == size)
	{
		// Error
		//sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	{
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy(pkt_id);
	// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
	//    .... (Possibly defered, but not for too long)
		bot_command_packet_v1_t *ph = ( bot_command_packet_v1_t * )mod ;

		ph->version = PKT_MODULE_TYPE_BOT_CMD_V1;
		ph->length = PKT_MODULE_LEN_BOT_REPORT_V1;
		ph->command= BOT_REPORT_V1;
		// !!!! Fill out the header of END module (after the APP-specific one)
		mod[PKT_MODULE_LEN_BOT_REPORT_V1 + PKT_MODULE_LEN_HEADER] = 
			PKT_MODULE_TYPE_END;
		mod[PKT_MODULE_LEN_BOT_REPORT_V1 + PKT_MODULE_LEN_HEADER + 1] = 
			original_size - PKT_MODULE_LEN_BOT_REPORT_V1 + PKT_MODULE_LEN_HEADER;
	// 4) Fill the content of the APP-specific module
	//    .... (Possibly defered, but not for too long)
 
		ph->L_speed=pwm8_get_PCA0CPHx(BOT_LEFT_ENCODER);
		ph->R_speed=pwm8_get_PCA0CPHx(BOT_RIGHT_ENCODER);
		ph->direction=bot_cur_direction;
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	return tsp_send_from_modules(pkt_id);
}



// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################
#endif  //_ENABLE_ROBOTICS_02_