#include "defs/esnl_pub.h"



#ifdef _ENABLE_ROBOTICS_03_

#include "qrhelo_basic_control_prv.h"
#include "qrhelo_basic_control_pub.h"
#include "HW_LIB/pwm8_gen.h"
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


rtc_tick_t	qrhelo_timeout;
rtc_tick_t	qrhelo_command_timeout;
rtc_tick_t  qrhelo_send_timeout;
uint16_t    qrhelo_send_interval;




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
  * qrhelo_init() - sets up bot module
  */
void qrhelo_init ( void )
{
	// setup PWM8
	uint8_t i;
	uint8_t	temp[QRHELO_PWM8_CONFIG_SIZE];
	pwm8_config_t *pwm_conf = (pwm8_config_t *)(temp);
	pwm8_duty_t *duty = (pwm8_duty_t *)(&(pwm_conf->idle));
//	Init_Device();
	pwm_conf->ver = PWM8_CONFIG_VER_01;
	pwm_conf->length = QRHELO_MAX_PWMS;
	for(i=0; i < QRHELO_MAX_PWMS; i++)
	{
//		duty[i] = PWM8_DUTY_OFF;	// for bot turn the PWM's off by default
		duty[i] = 0;	// for bot turn the PWM's off by default
	}

	//DISABLE H-BRIDGE
	QRHELO_HBRIDGE_ENABLE_LED=0;
	QRHELO_HBRIDGE_ENABLE=0;

//	pwm8_init(pwm_conf);
	pwm8_init(NULL); // by default it sets all to "0"
	pwm8_make_all_idle();
	
	qrhelo_timeout = rtc_get_ticks() + QRHELO_DEFAULT_DELAY;
	qrhelo_command_timeout = RTC_OVERFLOW_TIMER_VALUE;
	qrhelo_send_timeout=QRHELO_DEFAULT_SEND_INTERVAL; //stores next send
	qrhelo_send_interval=QRHELO_DEFAULT_SEND_INTERVAL;//allows send rate to be modified
}



/**
  * qrhelo_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void qrhelo_loop ( void )
{
/*	if (bot_timeout < rtc_get_ticks())
	{
		bot_timeout = rtc_get_ticks() + BOT_DEFAULT_DELAY;
		bot_test_execute();
	}*/

	if (qrhelo_command_timeout < rtc_get_ticks())
	{
		// 
		pwm8_make_all_idle();
		qrhelo_command_timeout = RTC_OVERFLOW_TIMER_VALUE;
	}

	//do everytime through;
//	bot_update_kinematics();//calculate where you are
//	bot_update_reference();//calculate where you think you should be
//	bot_update_control();//calculate appropriate control to get there

/*	if (bot_send_timeout < rtc_get_ticks())//send readings at specified interval
	{
		bot_send_timeout=rtc_get_ticks() + bot_send_interval;
		bot_send_packet();
	}*/
}


/**
  * qrhelo_execute_command() - executes a recieved command
  *	
  */

void qrhelo_execute_command ( uint8_t *packet, uint8_t rssi)
{	
	qrhelo_command_packet_v1_t *cmd = (qrhelo_command_packet_v1_t*)packet;

		switch (cmd->command)
		{

			case QRHELO_CMD_STOP:
				//DISABLE H-BRIDGE
				QRHELO_HBRIDGE_ENABLE_LED=0;
				QRHELO_HBRIDGE_ENABLE=0;
				pwm8_make_all_idle();
			break;

			case QRHELO_SET_SPEEDS:
				
				qrhelo_update_pwm(cmd->R1_speed, cmd->R2_speed,cmd->R3_speed,cmd->R4_speed);
				//ENABLE H-BRIDGE
				QRHELO_HBRIDGE_ENABLE_LED=1;
				QRHELO_HBRIDGE_ENABLE=1;
			break;

		}

	qrhelo_command_timeout = rtc_get_ticks() + QRHELO_CMD_TIMEOUT;

}




/**
  * 	qrhelo_update_pwm(L_speed, R_speed) updates new motor speeds
  */
void qrhelo_update_pwm(uint8_t R1_speed, uint8_t R2_speed, uint8_t R3_speed, uint8_t R4_speed)
{
	pwm8_make_idle(QRHELO_PWM_ROTOR_1);
	pwm8_set_duty(QRHELO_PWM_ROTOR_1, R1_speed);
	
	pwm8_make_idle(QRHELO_PWM_ROTOR_2);
	pwm8_set_duty(QRHELO_PWM_ROTOR_2, R2_speed);

	pwm8_make_idle(QRHELO_PWM_ROTOR_3);
	pwm8_set_duty(QRHELO_PWM_ROTOR_3, R3_speed);

	pwm8_make_idle(QRHELO_PWM_ROTOR_4);
	pwm8_set_duty(QRHELO_PWM_ROTOR_4, R4_speed);

}



/*
* uint8_t bot_send_packet() creates a packet of current sensor data
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
/*
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
*/


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################
#endif  //_ENABLE_ROBOTICS_03_