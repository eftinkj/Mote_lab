#include "defs/esnl_pub.h"

#ifdef _ENABLE_F1_MOTE_TEST_01_

#include "hw_f1_mote_basic_prv.h"
#include "hw_f1_mote_basic_pub.h"


//#include "pwm8_gen.h"
#include "FEAT_XBee_API/API_frames.h"
//#include "HW_LIB/adc_0.h"
#include "HW_LIB/uart_0.h"
#include "FEAT_Networking/tsp_common_pub.h"
#include "sys_func.h"
#include "Application.h"


//#define _HW_F1_MOTE_DEBUG_ENABLE_
#undef _HW_F1_MOTE_DEBUG_ENABLE_
//#define _HW_F1_MOTE_DEBUG_LEVEL_	100
#define _HW_F1_MOTE_DEBUG_LEVEL_	1

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

//uint8_t 	truck_state_test;
rtc_tick_t	xdata hw_f1_mote_timeout;
rtc_tick_t	xdata hw_f1_mote_read_power_timeout;
uint8_t		xdata hw_f1_mote_read_power_enabled;


#ifdef _ENABLE_SPI_MODULE_
	spi_event_t xdata hw_f1_mote_event_flag;
	spi_event_t xdata hw_f1_mote_even_mask;
#endif // _ENABLE_SPI_MODULE_


#ifdef _HW_F1_MOTE_DEBUG_ENABLE_
	// DEBUG 
	char	debug_buf[255];
	#include <string.h>
#endif // _HW_F1_MOTE_DEBUG_ENABLE_

sbit F1_LED = P3^6;

// STEPPER MOTOR QUICK SOLUTION
uint8_t hw_f1_stepper_state;
sbit SM1 = P1^0;
sbit SM2 = P0^6;
sbit SM3 = P0^7;
sbit SM4 = P1^1;
//sbit SM4	P1.2
//sbit SM4	P1.4

#define STEPPER_STOP	0
#define STEPPER_STEP1	1
#define STEPPER_STEP2	2
#define STEPPER_STEP3	3
#define STEPPER_STEP4	4

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/


// Support Functions
void delay(int t)
{
	int i, j;
	for(i=0;i<t;i++)
	{
		for(j=0;j<t;j++)
		{
			j = j+i;
			j = j-i;
		}
	}
}

/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/



/**
  * hw_f1_mote_init () - sets up MONitor of power module
  */
void hw_f1_mote_init ( void )
{
	F1_LED = HW_F1_LED_OFF;
	hw_f1_mote_timeout = 0;//RTC_OVERFLOW_TIMER_VALUE;

	hw_f1_mote_read_power_enabled = 1;
	hw_f1_mote_read_power_timeout = 0;

#ifdef _ENABLE_SPI_MODULE_
	hw_f1_mote_event_flag = SPI_EVENT_NONE;
	hw_f1_mote_even_mask =  SPI_EVENT_RELEASED | SPI_EVENT_PURGED | SPI_EVENT_DONE
			| SPI_EVENT_FAILED_RX_FILTERED;

	CS_MUX = HW_F1_MOTE_ADE_CS_DISABLE;	// Deselect the ADE chip
//	CS_MUX = HW_F1_MOTE_ADE_CS_ENABLE;	// Deselect the ADE chip
#endif // _ENABLE_SPI_MODULE_

#ifdef _ENABLE_XBEE_API_
//	api_send_packet16 ( "Power module is UP\r", 19, 0xFFFF );
#endif // _ENABLE_XBEE_API_

//	set_number_of_channels(HW_F1_MOTE_ADC_NO_CHANNELS);
//	adc_sample_averaging_ = HW_F1_MOTE_ADC_AVERAGER;
//	adc_sample_rate_divider_ = HW_F1_MOTE_ADC_DIVIDER;

//	sequence_no_ = 1;

//	app_resumeADC();
}



/**
  * hw_f1_mote_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void hw_f1_mote_loop( void )
{
//	com0_string ( "F1_Loop_1\r" );
	if (hw_f1_mote_timeout < rtc_get_ticks())
	{
		hw_f1_mote_timeout = rtc_get_ticks() + HW_F1_MOTE_DEFAULT_DELAY;
		// Periodic opetrations/actions
		
		switch (hw_f1_stepper_state)
		{
			case STEPPER_STOP:
				SM1 = 0;	SM2 = 0;	SM3 = 0;	SM4 = 0;
				break;
			case STEPPER_STEP1:
				SM1 = 0;	SM2 = 0;	SM3 = 0;	SM4 = 0;
				hw_f1_stepper_state = STEPPER_STEP2;
				break;
			case STEPPER_STEP2:
				SM1 = 0;	SM2 = 0;	SM3 = 0;	SM4 = 0;
				hw_f1_stepper_state = STEPPER_STEP3;
				break;
			case STEPPER_STEP3:
				SM1 = 0;	SM2 = 0;	SM3 = 0;	SM4 = 0;
				hw_f1_stepper_state = STEPPER_STEP4;
				break;
			case STEPPER_STEP4:
				SM1 = 0;	SM2 = 0;	SM3 = 0;	SM4 = 0;
				hw_f1_stepper_state = STEPPER_STEP1;
				break;

		}
//		com0_string ( "F1_Loop_2\r" );
//		while(1){
//		F1_LED = hw_f1_mote_test();
//		}
//		F1_LED = ~F1_LED;
	}

	if (hw_f1_mote_read_power_enabled)
	{
		if (hw_f1_mote_read_power_timeout < rtc_get_ticks())
		{
			hw_f1_mote_read_power_timeout = rtc_get_ticks() + HW_F1_MOTE_READ_POWER_DEFAULT_TIMEOUT;
//			hw_f1_mote_read_power_all();
//			hw_f1_mote_create_report();
		}
	}
}


/**
  * hw_f1_mote_execute_command (packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void hw_f1_mote_execute_command ( uint8_t *packet)
{	
/*
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
*/
}




/**
* hw_f1_mote_test() - tests power monitoring chip
*/
uint8_t hw_f1_mote_test()
{
#ifdef _ENABLE_SPI_MODULE_
	uint8_t	result = 0;
	uint16_t i;
	// 1) Reserve SPI - it should be FREE !!!!!
	if (SPI_R_OK != spi_request_dev(SPI_APP_MUX0, 10000, &hw_f1_mote_event_flag, hw_f1_mote_even_mask))
	{
#if (_HW_F1_MOTE_DEBUG_LEVEL_ > 10)
//		api_send_packet16 ( "TEST of the monitor\r", 20, 0xFFFF );
		com0_string ( "TEST of the monitor\r" );
#endif
		while(1);
	}
#if (_HW_F1_MOTE_DEBUG_LEVEL_ > 10)
//	api_send_packet16 ( "Modular SPI RESERVED\r", 21, 0xFFFF );
	com0_string ( "Modular SPI RESERVED\r" );
#endif
//	for (i=0; i<65000;i++) 
	{
	// 2) Enable if not on SPI NSS pin
	CS_MUX = HW_F1_MOTE_ADE_CS_ENABLE;
	// 3) Transmit etc.
		if (1==hw_f1_mote_test_ver())
		{
//			break;
		}
	// ... Any more activities/tests go here .... 
	CS_MUX = HW_F1_MOTE_ADE_CS_DISABLE;	// Deselect the ADE chip

	// 4) Disable if not on SPI NSS pin
	delay(5);
	}
	
	CS_MUX = HW_F1_MOTE_ADE_CS_DISABLE;	// Deselect the ADE chip

	// 5) Release SPI
	spi_release_dev( SPI_APP_MUX0);

	return result;
#else // _ENABLE_SPI_MODULE_
	return 0;
#endif // else/not _ENABLE_SPI_MODULE_
}


#define HW_F1_MOTE_MUX_PATTERN	0x55

/**
* hw_f1_mote_test_ver() - reads version register
*/
bit hw_f1_mote_test_ver()
{
#ifdef _ENABLE_SPI_MODULE_
	spi_buf_len_t tx_count, rx_count0, rx_count1;
	bit found_sync_byte = 0;
	uint8_t	txrx[4];

	// Synchronize SPI bus (reset SPI buffers/timers/registers in compass)

	// Send version request (PING)
	txrx[0] = HW_F1_MOTE_MUX_PATTERN;		txrx[1] = HW_F1_MOTE_MUX_PATTERN;
	txrx[2] = HW_F1_MOTE_MUX_PATTERN;		txrx[3] = HW_F1_MOTE_MUX_PATTERN;

	rx_count0 = spi_request_rxtx(SPI_APP_MUX0, txrx, 4);
	while (0 == (hw_f1_mote_event_flag & SPI_EVENT_DONE));

	rx_count1 = spi_get_rx_block (SPI_APP_MUX0, txrx, rx_count0);
	if (0xAA == txrx[0])
		found_sync_byte = 1;

#ifdef _HW_F1_MOTE_DEBUG_ENABLE_
#if (_HW_F1_MOTE_DEBUG_LEVEL_ > 15)
		sprintf(debug_buf, "Rec(%bu,%bu,%bx)=%bx,%bx\r"
			, rx_count0, rx_count1, hw_f1_mote_event_flag, txrx[0], txrx[1]);
//		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
		com0_string ( debug_buf );
#endif
#endif // _HW_F1_MOTE_DEBUG_ENABLE_
	hw_f1_mote_event_flag &= (~SPI_EVENT_DONE);

/*

	tx_count = spi_request_tx ( SPI_APP_POWER, txrx, 2);
	while (0 == (hw_f1_mote_event_flag & SPI_EVENT_DONE));
	hw_f1_mote_event_flag &= (~SPI_EVENT_DONE);


//	delay(800);
	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx ( SPI_APP_MUX0, 2);
	while (0 == (hw_f1_mote_event_flag & (SPI_EVENT_DONE | SPI_EVENT_PURGED)));
	if (0 == (hw_f1_mote_event_flag & SPI_EVENT_FAILED_RX_FILTERED) )
	{
		rx_count1 = spi_get_rx_block (SPI_APP_MUX0, txrx, rx_count0);
		if (HW_F1_MOTE_ADE_VER == txrx[0])
			found_sync_byte = 1;

#ifdef _HW_F1_MOTE_DEBUG_ENABLE_
#if (_HW_F1_MOTE_DEBUG_LEVEL_ > 15)
		sprintf(debug_buf, "Rec(%bu,%bu,%bx)=%bx,%bx\r"
			, rx_count0, rx_count1, hw_f1_mote_event_flag, txrx[0], txrx[1]);
//		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
		com0_string ( debug_buf );
#endif
#endif // _HW_F1_MOTE_DEBUG_ENABLE_
	}
	else
	{
		// ordinary read does not fail ...
		// ... but could hang due to SPI problem
//		com_string(0, "Failed SPI read - no SyncChar found\r");
	}
	hw_f1_mote_event_flag &= (~SPI_EVENT_DONE);
	hw_f1_mote_event_flag &= (~SPI_EVENT_FAILED_RX_FILTERED);
*/

#ifdef _HW_F1_MOTE_DEBUG_ENABLE_
#if (_HW_F1_MOTE_DEBUG_LEVEL_ > 5)
		sprintf(debug_buf, "VER_CHECK Rec(%bu,%bu,%bx)=%bx (state=%bu)\r", rx_count0
			, rx_count1, hw_f1_mote_event_flag, txrx[0], found_sync_byte);
//		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
		com0_string ( debug_buf );
#endif
#endif // _HW_F1_MOTE_DEBUG_ENABLE_
	return found_sync_byte;
#else // _ENABLE_SPI_MODULE_
	return 0;
#endif // else/not _ENABLE_SPI_MODULE_
}





// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################



#endif // _ENABLE_F1_MOTE_TEST_01_

