#include "defs/esnl_pub.h"

#ifdef _ENABLE_ROBOTICS_04_

#include "vex_tank_basic_control_pub.h"
#include "vex_tank_control_prv.h"
#include "HW_LIB/pwm8_gen.h"
#include "HW_LIB/adc_0.h"
//#include "API_frames.h"
//#include "tsp_common_pub.h"
#include "sys_func.h"
#include "Application.h"




/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


rtc_tick_t	vex_tank_timeout;
rtc_tick_t	vex_tank_command_timeout;
rtc_tick_t  vex_tank_send_timeout;
rtc_tick_t  vex_tank_line_follow_timeout;

uint16_t    vex_tank_send_interval;
uint8_t		vex_tank_position;
uint8_t		vex_tank_lost;
uint8_t		vex_tank_default_speed_left;
uint8_t		vex_tank_default_speed_right;
uint8_t		vex_tank_line_follow_mode;



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
  * vex_tank_init() - sets up bot module
  */
void vex_tank_init ( void )
{
	// setup PWM8
	uint8_t i;
	uint8_t	temp[VEX_TANK_PWM8_CONFIG_SIZE];
	pwm8_config_t *pwm_conf = (pwm8_config_t *)(temp);
	pwm8_duty_t *duty = (pwm8_duty_t *)(&(pwm_conf->idle));
//	Init_Device();
	pwm_conf->ver = PWM8_CONFIG_VER_01;
	pwm_conf->length = VEX_TANK_MAX_PWMS;
	for(i=0; i < VEX_TANK_MAX_PWMS; i++)
	{
//		duty[i] = PWM8_DUTY_OFF;	// for bot turn the PWM's off by default
		duty[i] = VEX_TANK_IDLE;	// for bot turn the PWM's off by default
	}


//	pwm8_init(pwm_conf);
	pwm8_init(NULL); // by default it sets all to "0"
	pwm8_make_all_idle();

	set_number_of_channels(VEX_TANK_ADC_NO_CHANNELS);
	adc_sample_averaging_ = VEX_TANK_ADC_AVERAGER;
	adc_sample_rate_divider_ = VEX_TANK_ADC_DIVIDER;

	sequence_no_ = 1;
	app_resumeADC();
	
	vex_tank_timeout = rtc_get_ticks() + VEX_TANK_DEFAULT_DELAY;
	vex_tank_command_timeout = RTC_OVERFLOW_TIMER_VALUE;
	vex_tank_send_timeout=VEX_TANK_DEFAULT_SEND_INTERVAL; //stores next send
	vex_tank_send_interval=VEX_TANK_DEFAULT_SEND_INTERVAL;//allows send rate to be modified
	vex_tank_line_follow_timeout=VEX_TANK_DEFAULT_LINE_FOLLOW_INTERVAL;//allows ADC READ RATA to be modified


	vex_tank_lost=0;
	vex_tank_line_follow_mode=0;  //start in manual mode
	vex_tank_position=0;
	
//	vex_tank_line_follow_mode=1;  //start in line follow mode
//	vex_tank_position=2;

	vex_tank_default_speed_left=VEX_TANK_IDLE+VEX_TANK_TRIM_LEFT+VEX_TANK_DEFAULT_SPEED;
	vex_tank_default_speed_right=VEX_TANK_IDLE-VEX_TANK_TRIM_RIGHT-VEX_TANK_DEFAULT_SPEED;
}



/**
  * vex_tank_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void vex_tank_loop ( void )
{
	int16_t adc_len = 0;

	uint8_t line_sensor_status=0;

/*	if (bot_timeout < rtc_get_ticks())
	{
		bot_timeout = rtc_get_ticks() + BOT_DEFAULT_DELAY;
		bot_test_execute();
	}*/

	if ((vex_tank_command_timeout < rtc_get_ticks())&&(!vex_tank_line_follow_mode))
	{
		// 
		pwm8_make_all_idle();
		vex_tank_command_timeout = RTC_OVERFLOW_TIMER_VALUE;
	}
	
	if ((vex_tank_line_follow_timeout < rtc_get_ticks())&&(P_LOST!=vex_tank_position))
	{
 		adc_len = adc0_not_empty (VEX_TANK_LINE_SENSOR_LEFT);
		if (0 == adc_len)
		{
			line_sensor_status=255;
		}
		else
		{
			line_sensor_status=0;
			line_sensor_status=adc0_get_value_threshed(VEX_TANK_LINE_SENSOR_LEFT,VEX_TANK_ADC_THRESHOLD);
			line_sensor_status=line_sensor_status<<1;
			line_sensor_status|=adc0_get_value_threshed(VEX_TANK_LINE_SENSOR_MIDDLE,VEX_TANK_ADC_THRESHOLD);
			line_sensor_status=line_sensor_status<<1;
			line_sensor_status|=adc0_get_value_threshed(VEX_TANK_LINE_SENSOR_RIGHT,VEX_TANK_ADC_THRESHOLD);

			vex_tank_line_follow(line_sensor_status);
		}



	}//end if line_follow_timeout

	else if ((P_LOST==vex_tank_position)&&(vex_tank_line_follow_mode))
	{
		pwm8_make_all_idle();
	} //end else

/*	if (bot_send_timeout < rtc_get_ticks())//send readings at specified interval
	{
		bot_send_timeout=rtc_get_ticks() + bot_send_interval;
		bot_send_packet();
	}*/
}// end vex_tank_loop ( void )

/**
  * vex_tank_line_follow(unsigned char line_status) - function for tracking a black line
  *	
  */
void vex_tank_line_follow(uint8_t line_status)
{
	switch (line_status)
	{

		case S_RIGHT:
			vex_tank_position=3;
			vex_tank_lost=0;

			vex_tank_veer_right(VEX_TANK_VEER_SPEED_LARGE);
		break;

		case S_MIDDLE:
			vex_tank_position=2;
			vex_tank_lost=0;

			vex_tank_update_pwm(vex_tank_default_speed_left , vex_tank_default_speed_right );
		break;

		case S_MID_RIGHT:
			vex_tank_position=3;
			vex_tank_lost=0;

			vex_tank_veer_right(VEX_TANK_VEER_SPEED_SMALL);
//			vex_tank_update_pwm(vex_tank_default_speed_left , vex_tank_default_speed_right );

		break;

		case S_LEFT:
			vex_tank_position=1;
			vex_tank_lost=0;

			vex_tank_veer_left(VEX_TANK_VEER_SPEED_LARGE);
		break;

		case S_LEFT_MID:
			vex_tank_position=1;
			vex_tank_lost=0;

			vex_tank_veer_right(VEX_TANK_VEER_SPEED_SMALL);
//			vex_tank_update_pwm(vex_tank_default_speed_left, vex_tank_default_speed_right);
	
		break;

		case S_L_MID_R:
			vex_tank_lost=0;

//			vex_tank_update_pwm(vex_tank_default_speed_left, vex_tank_default_speed_right);
//			vex_tank_line_follow_180();
			//vex_tank_position=3;
		break;

		default:
			vex_tank_lost++;
		break;

		}
		
		if(vex_tank_lost>TANK_LOST)
		{
			switch(vex_tank_position)
			{
				case P_LEFT:
					vex_tank_veer_left(VEX_TANK_VEER_SPEED_HARD);
				break;

				case P_MIDDLE:
					pwm8_make_all_idle();
					vex_tank_update_pwm(vex_tank_default_speed_left, vex_tank_default_speed_right);
				break;

				case P_RIGHT:
					vex_tank_veer_right(VEX_TANK_VEER_SPEED_HARD);
				break;
			}
			
		}
		if(vex_tank_lost>TANK_LOST_GIVEUP)	
			vex_tank_position=0;	
}

/**
  * vex_tank_veer_left(uint8 veer_size) - steer left to find line
  *	
  */
void vex_tank_veer_left(uint8_t veer_size)
{
	uint8_t speed_left=0;
	uint8_t speed_right=0;

	if (VEX_TANK_VEER_SPEED_HARD==veer_size)
	{
		speed_left=VEX_TANK_IDLE+VEX_TANK_TRIM_LEFT+veer_size;
		speed_right=VEX_TANK_IDLE-VEX_TANK_TRIM_RIGHT+veer_size;
		
	}

	else
	{
		speed_left=VEX_TANK_IDLE+VEX_TANK_DEFAULT_SPEED+VEX_TANK_TRIM_LEFT+veer_size;
		speed_right=VEX_TANK_IDLE-VEX_TANK_DEFAULT_SPEED-VEX_TANK_TRIM_RIGHT+veer_size;
		
	}
	vex_tank_update_pwm(speed_left,speed_right);

}

/**
  * vex_tank_veer_right() - steer right to find line
  *	
  */
void vex_tank_veer_right(uint8_t veer_size)
{
	uint8_t speed_left=0;
	uint8_t speed_right=0;

	if (VEX_TANK_VEER_SPEED_HARD==veer_size)
	{
		speed_left=VEX_TANK_IDLE+VEX_TANK_TRIM_LEFT-veer_size;
		speed_right=VEX_TANK_IDLE+VEX_TANK_TRIM_RIGHT-veer_size;
		
	}

	else
	{
		speed_left=VEX_TANK_IDLE+VEX_TANK_DEFAULT_SPEED+VEX_TANK_TRIM_LEFT-veer_size;
		speed_right=VEX_TANK_IDLE-VEX_TANK_DEFAULT_SPEED-VEX_TANK_TRIM_RIGHT-veer_size;
		
	}
	vex_tank_update_pwm(speed_left,speed_right);
}

/**
  * vex_tank_execute_command() - executes a recieved command
  *	
  */

void vex_tank_execute_command ( uint8_t *packet)
{	
	vex_tank_command_packet_v1_t *cmd = (vex_tank_command_packet_v1_t*)packet;

		switch (cmd->command)
		{

			case VEX_TANK_CMD_STOP:
				pwm8_make_all_idle();
				vex_tank_position=0;
				vex_tank_line_follow_mode=0;
			break;

			case VEX_TANK_START_LINE_FOLLOW:
				vex_tank_position=2;
				vex_tank_line_follow_mode=1;
				pwm8_make_all_idle();
				vex_tank_update_pwm(vex_tank_default_speed_left, vex_tank_default_speed_right);
			break;

			case VEX_TANK_MANUAL_CNTRL:
				vex_tank_position=0;
				vex_tank_line_follow_mode=0;
				pwm8_make_all_idle();
			break;

			case VEX_TANK_SET_SPEEDS:
				vex_tank_update_pwm(cmd->LM_speed, cmd->RM_speed);		
			break;

			case VEX_TANK_SET_PWM_FREQ:
				pwm8_set_freq(cmd->LM_speed);
			break;

			case VEX_TANK_TURN_180:
			
			break;

			case VEX_TANK_LINE_FOLLOW_T180:
			
			break;

			case VEX_TANK_SET_DEFAULT_SPEED:
				//vex_tank_default_speed_left=vex_tank_default_speed_left+cmd->LM_speed;
				//vex_tank_default_speed_right=vex_tank_default_speed_left-cmd->RM_speed;
			break;

			default:

			break;

		}

	vex_tank_command_timeout = rtc_get_ticks() + VEX_TANK_CMD_TIMEOUT;

}




/**
  * 	vex_tank_update_pwm(L_speed, R_speed) updates new motor speeds
  */
void vex_tank_update_pwm(uint8_t LM_speed, uint8_t RM_speed)
{
	pwm8_make_idle(VEX_TANK_PWM_LEFT_MOTOR);
	pwm8_set_duty(VEX_TANK_PWM_LEFT_MOTOR, LM_speed);
	
	pwm8_make_idle(VEX_TANK_PWM_RIGHT_MOTOR);
	pwm8_set_duty(VEX_TANK_PWM_RIGHT_MOTOR, RM_speed);

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
#endif  //_ENABLE_ROBOTICS_04_