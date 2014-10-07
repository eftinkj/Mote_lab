#include "defs/esnl_pub.h"

#ifdef _ENABLE_NEM_UTILITIES_01_

#include "mon_power_basic_prv.h"
#include "mon_power_basic_pub.h"

#include "HW_ADE_common.h"

//#include "pwm8_gen.h"
#include "API_frames.h"
#include "adc_0.h"
#include "tsp_common_pub.h"
#include "sys_func.h"
#include "Application.h"


//#define _MON_POWER_DEBUG_ENABLE_
#undef _MON_POWER_DEBUG_ENABLE_
#define _MON_POWER_DEBUG_LEVEL_	0

#define MON_POWER_EXTENDED_REPORT
//#undef MON_POWER_EXTENDED_REPORT

#ifdef ADE_TEST_TRIGGER_ENABLED
	sbit TRIGGER=P1^0;
#endif // ADE_TEST_TRIGGER_ENABLED


/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

//uint8_t 	truck_state_test;
rtc_tick_t	mon_power_timeout;
rtc_tick_t	mon_power_read_power_timeout;
uint8_t		mon_power_read_power_enabled;

uint16_t mon_power_sampling_interval = MON_POWER_READ_POWER_DEFAULT_TIMEOUT;


spi_event_t mon_power_event_flag;
spi_event_t mon_power_even_mask;


#ifdef _MON_POWER_DEBUG_ENABLE_
	// DEBUG 
	char	debug_buf[255];
	#include <string.h>
#endif // _MON_POWER_DEBUG_ENABLE_

int16_t	mon_acc_watt_a;
int16_t	mon_acc_watt_b;
int16_t	mon_acc_watt_c;

int16_t	mon_acc_var_a;
int16_t	mon_acc_var_b;
int16_t	mon_acc_var_c;

uint16_t	mon_acc_va_a;
uint16_t	mon_acc_va_b;
uint16_t	mon_acc_va_c;


#ifdef MON_POWER_EXTENDED_REPORT
	uint32_t	v_a;
	uint32_t	v_b;
	uint32_t	v_c;

	uint32_t	i_a;
	uint32_t	i_b;
	uint32_t	i_c;

	uint16_t	ade_freq;
	sint8_t		ade_temp;
	uint32_t	ade_wave;
#endif


uint8_t	mon_power_init_ADE_reqested;

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
sbit	G_SELECT_1 = P2^0;
sbit	G_SELECT_2 = P2^1;
sbit	G_SLEEP_NOT = P2^2;

// Original direct connection between 8051 and ADE
#define MON_POWER_ADE_CS_DISABLE 1
#define MON_POWER_ADE_CS_ENABLE	0

// Reverse due to optoisolaters
//#define MON_POWER_ADE_CS_DISABLE 0
//#define MON_POWER_ADE_CS_ENABLE	1

/**
  * mon_power_init () - sets up MONitor of power module
  */
void mon_power_init ( void )
{
	mon_power_timeout = RTC_OVERFLOW_TIMER_VALUE;

	mon_power_read_power_enabled = 1;
	mon_power_read_power_timeout = 0;

	mon_power_event_flag = SPI_EVENT_NONE;
	mon_power_even_mask =  SPI_EVENT_RELEASED | SPI_EVENT_PURGED | SPI_EVENT_DONE
			| SPI_EVENT_FAILED_RX_FILTERED;

	CS_ADE = MON_POWER_ADE_CS_DISABLE;	// Deselect the ADE chip

	api_send_packet16 ( "Power module is UP\r", 19, 0xFFFF );

	G_SELECT_1 = 1;
	G_SELECT_2 = 1;
	G_SLEEP_NOT = 1;

	set_number_of_channels(MON_POWER_ADC_NO_CHANNELS);
	adc_sample_averaging_ = MON_POWER_ADC_AVERAGER;
	adc_sample_rate_divider_ = MON_POWER_ADC_DIVIDER;

	sequence_no_ = 1;
	app_resumeADC();

	mon_power_init_ADE_reqested = 1;

#ifdef ADE_TEST_TRIGGER_ENABLED
	TRIGGER = 0;
#endif // ADE_TEST_TRIGGER_ENABLED
}



/**
  * mon_power_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void mon_power_loop( void )
{
/*	if (mon_power_timeout < rtc_get_ticks())
	{
		mon_power_timeout = rtc_get_ticks() + MON_POWER_DEFAULT_DELAY;
		// Periodic opetrations/actions
	}
*/
	if (mon_power_read_power_enabled)
	{
		if (mon_power_read_power_timeout < rtc_get_ticks())
		{
			mon_power_read_power_timeout = rtc_get_ticks() + mon_power_sampling_interval ;
			mon_power_read_power_all();
			mon_power_create_report();
		}
	}
	// one time operation
	if ( mon_power_init_ADE_reqested )
	{
		mon_power_init_ade();
		mon_power_init_ADE_reqested = 0;
	}
}


/**
  * mon_power_execute_command (packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void mon_power_execute_command ( uint8_t *packet)
{	

	mon_power_cmd_packet_t *cmd = (mon_power_cmd_packet_t*)packet;
	switch (cmd->content)
	{
		case MON_POWER_COMMAND_V1:
			mon_power_cmd(packet);
			break;
		case MON_POWER_PARAM_REQUEST:
			mon_power_query_params(packet);
			break;
		case MON_POWER_PARAM_CHANGE:
			mon_power_change_params(packet);
			break;
	}
//	truck_command_timeout = rtc_get_ticks() + TRUCK_CMD_TIMEOUT;

//	truck_command_timeout = rtc_get_ticks() + TRUCK_CMD_TIMEOUT;
}



/**
*  mon_power_init_ade () - sets ADE chip params
*/
void mon_power_init_ade()
{
	// A1) Reserve SPI - it should be FREE !!!!!
	if (SPI_R_OK != spi_request_dev(SPI_APP_POWER, 10000, &mon_power_event_flag, mon_power_even_mask))
	{
#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		api_send_packet16 ( "ERROR while Read All Power\r", 20, 0xFFFF );
#endif
		while(1);
	}
#if (_MON_POWER_DEBUG_LEVEL_ > 10)
	api_send_packet16 ( "Modular SPI RESERVED\r", 21, 0xFFFF );
#endif
	// A2) Enable if not on SPI NSS pin
	CS_ADE = MON_POWER_ADE_CS_ENABLE;
	// 3) Transmit etc.
	{
		uint32_t value = 0;
		uint32_t rvalue = 0;
		uint8_t reg = 0;

		value = 0x78UL;
		value = value * 0x1000000;
		reg = LCYCMODE ;
//		mon_power_write_u32_reg(reg, value, 1);
//		rvalue = mon_power_read_u24_reg(reg);
		if (rvalue != value)
		{
			//errorr
//			api_send_packet16 ( "ERROR while setting ADE\r", 24, 0xFFFF );
		}
	}
	// A4) Disable if not on SPI NSS pin
	CS_ADE = MON_POWER_ADE_CS_DISABLE;	// Deselect the ADE chip

	// A5) Release SPI
	spi_release_dev( SPI_APP_POWER);
}


/**
* mon_power_test() - tests power monitoring chip
*/
uint8_t mon_power_test()
{
	uint8_t	result = 0;
	uint16_t i, j;
	// 1) Reserve SPI - it should be FREE !!!!!
	if (SPI_R_OK != spi_request_dev(SPI_APP_POWER, 10000, &mon_power_event_flag, mon_power_even_mask))
	{
#if (_MON_POWER_DEBUG_LEVEL_ > 10)
		api_send_packet16 ( "TEST of the monitor\r", 20, 0xFFFF );
#endif
		while(1);
	}
#if (_MON_POWER_DEBUG_LEVEL_ > 10)
	api_send_packet16 ( "Modular SPI RESERVED\r", 21, 0xFFFF );
#endif
	for (i=0; i<6500; i++) 
	{
//	for (j=0; j<65000; j++) 
//	for (i=0; i<6500;i++) 
//	for (i=0; i<10;i++) 
	{
#ifdef ADE_TEST_TRIGGER_ENABLED
		TRIGGER = 1;
#endif // ADE_TEST_TRIGGER_ENABLED
		delay(2);
		// 2) Enable if not on SPI NSS pin
		CS_ADE = MON_POWER_ADE_CS_ENABLE;
		// 3) Transmit etc.
#ifdef	ADE_TEST_VER
		if (1==mon_power_get_ver())
		{
			api_send_packet16 ( "Found response after\r", 21, 0xFFFF );
			api_send_packet16 ( &i, 2, 0xFFFF );
			break;
		}
#endif // ADE_TEST_VER

#ifdef	ADE_TEST_READ_WRITE
		{
			uint32_t value = 0;
			uint32_t rvalue = 0;
			uint8_t reg = 0;

			//	value = 0xAAAAAAAAUL;
			value = 0xAAAAAAAAUL;
			//	value = value * 0x1000000;
			reg = SAGCYC; //MASK;
			inter_spi_delay();
			mon_power_write_u32_reg(reg, value, 1);
			inter_spi_delay();
#ifdef ADE_TEST_TRIGGER_ENABLED
			TRIGGER = 0;
#endif // ADE_TEST_TRIGGER_ENABLED
			delay(2);
			rvalue = mon_power_read_u8_reg(reg);
			inter_spi_delay();
			if (rvalue == value)
			{
				//errorr
				//api_send_packet16 ( "ERROR while setting ADE\r", 24, 0xFFFF );
			}
		}
#endif // ADE_TEST_READ_WRITE

		//result = mon_power_read_u8_reg(VPINTLVL);
	// ... Any more activities/tests go here .... 

		// 4) Disable if not on SPI NSS pin
		CS_ADE = MON_POWER_ADE_CS_DISABLE;	// Deselect the ADE chip
//		delay(10);
	} // for j
	} // for i
	
	CS_ADE = MON_POWER_ADE_CS_DISABLE;	// Deselect the ADE chip
	// 5) Release SPI
	spi_release_dev( SPI_APP_POWER);

	return result ;

}

//#define MON_POWER_ADE_VER_REG	0x7F
//#define MON_POWER_ADE_VER	0x02

#define MON_POWER_ADE_VER_REG	0x1D
#define MON_POWER_ADE_VER	0xFF

//#define MON_POWER_ADE_VER_REG	0x13
//#define MON_POWER_ADE_VER	0x04

//#define MON_POWER_ADE_VER_REG	0x16
//#define MON_POWER_ADE_VER	0x1c

/**
* mon_power_get_ver() - reads version register
*/
uint8_t mon_power_get_ver()
{
	spi_buf_len_t tx_count, rx_count0, rx_count1;
	uint8_t found_sync_byte = 0;
	uint8_t	txrx[2];

	// Synchronize SPI bus (reset SPI buffers/timers/registers in compass)

	// Send version request (PING)
//	txrx[0] = 0x80;/* reversed 0x7E */	txrx[1] = 0xAA;
//	txrx[0] = 0xE9;/* reversed 0x16 */	txrx[1] = 0xAA;
//	txrx[0] = 0x7F;	txrx[1] = 0xAA;
	txrx[0] = MON_POWER_ADE_VER_REG;
	txrx[1] = 0xAA;

	tx_count = spi_request_tx ( SPI_APP_POWER, txrx, 1);
	while (0 == (mon_power_event_flag & SPI_EVENT_DONE));
	mon_power_event_flag &= (~SPI_EVENT_DONE);


	delay(5);
	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx ( SPI_APP_POWER, 1);
	while (0 == (mon_power_event_flag & (SPI_EVENT_DONE | SPI_EVENT_PURGED)));
	if (0 == (mon_power_event_flag & SPI_EVENT_FAILED_RX_FILTERED) )
	{
		rx_count1 = spi_get_rx_block (SPI_APP_POWER, txrx, rx_count0);
		if (MON_POWER_ADE_VER == txrx[0])
			found_sync_byte = 1;

#ifdef _MON_POWER_DEBUG_ENABLE_
#if (_MON_POWER_DEBUG_LEVEL_ > 15)
		sprintf(debug_buf, "Rec(%bu,%bu,%bx)=%bx,%bx\r"
			, rx_count0, rx_count1, mon_power_event_flag, txrx[0], txrx[1]);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
#endif // _MON_POWER_DEBUG_ENABLE_
	}
	else
	{
		// ordinary read does not fail ...
		// ... but could hang due to SPI problem
//		com_string(0, "Failed SPI read - no SyncChar found\r");
	}
	mon_power_event_flag &= (~SPI_EVENT_DONE);
	mon_power_event_flag &= (~SPI_EVENT_FAILED_RX_FILTERED);

#ifdef _MON_POWER_DEBUG_ENABLE_
#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		sprintf(debug_buf, "VER_CHECK Rec(%bu,%bu,%bx)=%bx (state=%bu)\r", rx_count0
			, rx_count1, mon_power_event_flag, txrx[0], found_sync_byte);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
#endif // _MON_POWER_DEBUG_ENABLE_
	return found_sync_byte;
}



/**
* uint8_t mon_power_create_report () - creates and fills out the report packet
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t mon_power_create_report ()
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
#ifdef MON_POWER_EXTENDED_REPORT
	uint8_t size = tsp_reserve_packet(MON_POWER_REPORT_V2_SIZE, &pkt_id);
#else // MON_POWER_EXTENDED_REPORT
	uint8_t size = tsp_reserve_packet(MON_POWER_REPORT_V1_SIZE, &pkt_id);
#endif // else MON_POWER_EXTENDED_REPORT
	uint8_t original_size = 0;
	if (0 == size)
	{
		// Error
		sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	{
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy(pkt_id);
	// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
	//    .... (Possibly defered, but not for too long)
#ifdef MON_POWER_EXTENDED_REPORT
		mon_power_report_packet_v2_t *ph = ( mon_power_report_packet_v2_t * )mod ;
		int16_t adc_len = 0;
		//original_size = ph->module_length;
		ph->module_type = PKT_MODULE_TYPE_MON_POWER_CMD_V1;
		ph->module_length = PKT_MODULE_LEN_MON_POWER_REPORT_V2;
		ph->report_type = MON_POWER_REPORT_V2;
#else //MON_POWER_EXTENDED_REPORT
		mon_power_report_packet_v1_t *ph = ( mon_power_report_packet_v1_t * )mod ;
		int16_t adc_len = 0;
		//original_size = ph->module_length;
		ph->module_type = PKT_MODULE_TYPE_MON_POWER_CMD_V1;
		ph->module_length = PKT_MODULE_LEN_MON_POWER_REPORT_V1;
		ph->report_type = MON_POWER_REPORT_V1;
#endif // MON_POWER_EXTENDED_REPORT
		// !!!! Fill out the header of END module (after the APP-specific one)
		mod[ph->module_length + PKT_MODULE_LEN_HEADER] = 
			PKT_MODULE_TYPE_END;
		mod[ph->module_length + PKT_MODULE_LEN_HEADER + 1] = 
			original_size - ph->module_length + PKT_MODULE_LEN_HEADER;
	// 4) Fill the content of the APP-specific module
	//    .... (Possibly defered, but not for too long)
 		adc_len = adc0_not_empty (MON_POWER_ADC_TEMP);
		if (0 == adc_len)
		{
			ph->aver_temp = ph->aver_g_x = ph->aver_g_y = ph->aver_g_z = 0xFF;
		}
		else
		{
			ph->aver_temp = adc0_get_average(MON_POWER_ADC_TEMP, adc_len);
			ph->aver_g_x = adc0_get_max(MON_POWER_ADC_G_X, adc_len);
			ph->aver_g_y = adc0_get_max(MON_POWER_ADC_G_Y, adc_len);
			ph->aver_g_z = adc0_get_max(MON_POWER_ADC_G_Z, adc_len);
			//ph->aver_temp = ph->aver_g_x = ph->aver_g_y = ph->aver_g_z = 0x0005;
		}

//		ph->acc_watt_a = ph->acc_watt_b = ph->acc_watt_c = 0; 
		ph->acc_watt_a = mon_acc_watt_a;
		ph->acc_watt_b = mon_acc_watt_b;
		ph->acc_watt_c = mon_acc_watt_c;

//		ph->acc_var_a = ph->acc_var_b = ph->acc_var_c = -1; 
		ph->acc_var_a = mon_acc_var_a;
		ph->acc_var_b = mon_acc_var_b;
		ph->acc_var_c = mon_acc_var_c;

//		ph->acc_va_a = ph->acc_va_b = ph->acc_va_c = 1; 
		ph->acc_va_a = mon_acc_va_a;
		ph->acc_va_b = mon_acc_va_b;
		ph->acc_va_c = mon_acc_va_c;

#ifdef MON_POWER_EXTENDED_REPORT
		ph->v_a = v_a;
		ph->v_b = v_b;
		ph->v_c = v_c;

		ph->i_a = i_a;
		ph->i_b = i_b;
		ph->i_c = i_c;

		ph->freq = ade_freq;
		ph->temp = ade_temp;
		ph->wave = ade_wave;
#endif
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	return tsp_send_from_modules(pkt_id);
}



/**
* mon_power_read_power_all() - reads all Accum. registers (Watt-Hour, VAR, VA)
*/
uint8_t mon_power_read_power_all()
{
	uint8_t	result = 0;
	// 1) Reserve SPI - it should be FREE !!!!!
	if (SPI_R_OK != spi_request_dev(SPI_APP_POWER, 10000, &mon_power_event_flag, mon_power_even_mask))
	{
#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		api_send_packet16 ( "ERROR while Read All Power\r", 20, 0xFFFF );
#endif
		while(1);
	}
#if (_MON_POWER_DEBUG_LEVEL_ > 10)
	api_send_packet16 ( "Modular SPI RESERVED\r", 21, 0xFFFF );
#endif
	// 2) Enable if not on SPI NSS pin
	CS_ADE = MON_POWER_ADE_CS_ENABLE;
	// 3) Transmit etc.

//	mon_acc_watt_a = mon_power_read_s16_reg(MON_POWER_ADE_WATT_HOUR_A);
//	mon_acc_watt_b = mon_power_read_s16_reg(MON_POWER_ADE_WATT_HOUR_B);
//	mon_acc_watt_c = mon_power_read_s16_reg(MON_POWER_ADE_WATT_HOUR_C);

//	mon_acc_var_a = mon_power_read_s16_reg(MON_POWER_ADE_VAR_HOUR_A);
//	mon_acc_var_b = mon_power_read_s16_reg(MON_POWER_ADE_VAR_HOUR_B);
//	mon_acc_var_c = mon_power_read_s16_reg(MON_POWER_ADE_VAR_HOUR_C);

//	v_a = mon_power_read_u24_reg(AVRMS);

	mon_acc_watt_a = (int16_t)mon_power_read_u16_reg(MON_POWER_ADE_WATT_HOUR_A);
	inter_spi_delay();
	mon_acc_watt_b = (int16_t)mon_power_read_u16_reg(MON_POWER_ADE_WATT_HOUR_B);
	inter_spi_delay();
	mon_acc_watt_c = (int16_t)mon_power_read_u16_reg(MON_POWER_ADE_WATT_HOUR_C);
	inter_spi_delay();

	mon_acc_var_a = (int16_t)mon_power_read_u16_reg(MON_POWER_ADE_VAR_HOUR_A);
	inter_spi_delay();
	mon_acc_var_b = (int16_t)mon_power_read_u16_reg(MON_POWER_ADE_VAR_HOUR_B);
	inter_spi_delay();
	mon_acc_var_c = (int16_t)mon_power_read_u16_reg(MON_POWER_ADE_VAR_HOUR_C);
	inter_spi_delay();

	mon_acc_va_a = mon_power_read_u16_reg(MON_POWER_ADE_VA_HOUR_A);
	inter_spi_delay();
	mon_acc_va_b = mon_power_read_u16_reg(MON_POWER_ADE_VA_HOUR_B);
	inter_spi_delay();
	mon_acc_va_c = mon_power_read_u16_reg(MON_POWER_ADE_VA_HOUR_C);
	inter_spi_delay();

#ifdef MON_POWER_EXTENDED_REPORT
	v_a = mon_power_read_u24_reg(AVRMS);
	inter_spi_delay();
	v_b = mon_power_read_u24_reg(BVRMS);
	inter_spi_delay();
	v_c = mon_power_read_u24_reg(CVRMS);
	inter_spi_delay();

	i_a = mon_power_read_u24_reg(AIRMS);
	inter_spi_delay();
	i_b = mon_power_read_u24_reg(BIRMS);
	inter_spi_delay();
	i_c = mon_power_read_u24_reg(CIRMS);
	inter_spi_delay();

	ade_freq = mon_power_read_u16_reg( FREQ );
	inter_spi_delay();
	ade_temp = mon_power_read_u8_reg ( TEMP );
	inter_spi_delay();
	ade_wave = mon_power_read_u24_reg( WFORM );
	inter_spi_delay()

#endif

	// ... Any more activities/tests go here .... 

	// 4) Disable if not on SPI NSS pin
	CS_ADE = MON_POWER_ADE_CS_DISABLE;	// Deselect the ADE chip
	
	// 5) Release SPI
	spi_release_dev( SPI_APP_POWER);

#if (_MON_POWER_DEBUG_LEVEL_ > 1)
		sprintf(debug_buf, "All Power Read (WH)A=%bd, B=%bd, C=%bd\r (VAR)A=%bd, B=%bd, C=%bd\r (VA)A=%bu, B=%bu, C=%bu\r"
			, mon_acc_watt_a, mon_acc_watt_b, mon_acc_watt_c
			, mon_acc_var_a, mon_acc_var_b, mon_acc_var_c
			, mon_acc_va_a, mon_acc_va_b, mon_acc_va_c
);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif

	return result ;

}






void mon_power_cmd(char* packet)
{
	mon_power_command_packet_v1_t * pkt = (mon_power_command_packet_v1_t*)packet;
	switch (pkt->command_param)
	{
		case MON_POWER_CMD_STOP:
			mon_power_read_power_enabled = 0;
//			mon_power_read_power_timeout = 0;
			break;
		case MON_POWER_CMD_START:
			mon_power_read_power_enabled = 1;
			mon_power_read_power_timeout = rtc_get_ticks() + mon_power_sampling_interval ;
			break;
		case MON_POWER_CMD_INTERVAL:
			mon_power_sampling_interval = pkt->command_wparam;
			mon_power_read_power_timeout = rtc_get_ticks() + (rtc_tick_t)mon_power_sampling_interval ;
			break;
	}
}



/**
*  mon_power_change_params (packet) - changes ADE chip params
*/
void mon_power_change_params(char* packet)
{
	mon_power_request_packet_v1_t* pkt = (mon_power_request_packet_v1_t*)packet;
	char*	param_list = (char*)(&(pkt->params_list));
	uint8_t param_count = pkt->count_params;

	uint8_t	i = 0;
	// A1) Reserve SPI - it should be FREE !!!!!
	if (SPI_R_OK != spi_request_dev(SPI_APP_POWER, 10000, &mon_power_event_flag, mon_power_even_mask))
	{
#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		api_send_packet16 ( "ERROR while Read All Power\r", 20, 0xFFFF );
#endif
		while(1);
	}
#if (_MON_POWER_DEBUG_LEVEL_ > 10)
	api_send_packet16 ( "Modular SPI RESERVED\r", 21, 0xFFFF );
#endif
	// A2) Enable if not on SPI NSS pin
	CS_ADE = MON_POWER_ADE_CS_ENABLE;
	// 3) Transmit etc.

	for(i=0; i < param_count; i++)
	{
		mon_power_param_report_t* p_block = (mon_power_param_report_t*)(param_list + i * MON_POWER_PARAM_REPORT_LEN);
		uint32_t val = p_block->value;
//		value = value << (32 - p_block->size);
		switch (p_block->size)
		{
			case 7:
			case 8:
			case 12:
			case 16:
			case 24:
				mon_power_write_u32_reg(p_block->addr, val, 1 + (p_block->size-1) / BYTE_SIZE_IN_BITS  );
				break;
/*			case 12:
			case 16:
				mon_power_write_u24_reg(p_block->addr, val);
				break;
			case 24:
				mon_power_write_u24_reg(p_block->addr, val);
				break;
*/
		}
		inter_spi_delay();
	}
	// A4) Disable if not on SPI NSS pin
	CS_ADE = MON_POWER_ADE_CS_DISABLE;	// Deselect the ADE chip

	// A5) Release SPI
	spi_release_dev( SPI_APP_POWER);
}


/**
*  mon_power_query_params (packet) - creates and fills out the report packet
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t mon_power_query_params(char* packet)
{
	mon_power_request_packet_v1_t* rpkt = (mon_power_request_packet_v1_t*)packet;
	char*	param_list = (char*)(&(rpkt->params_list));
	uint8_t param_count = rpkt->count_params;

	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
	uint8_t size = tsp_reserve_packet(MON_POWER_PARAM_REPORT_SIZE, &pkt_id);
	uint8_t original_size = 0;
	if (0 == size)
	{
		// Error
		sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	{
		uint8_t i;
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy(pkt_id);
	// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
	//    .... (Possibly defered, but not for too long)
		mon_power_request_packet_v1_t *ph = ( mon_power_request_packet_v1_t * )mod ;
		int16_t adc_len = 0;
		//original_size = ph->module_length;
		ph->module_type = PKT_MODULE_TYPE_MON_POWER_CMD_V1;
		ph->module_length = MON_POWER_PARAM_REPORT_SIZE;
		ph->request_type = MON_POWER_PARAM_REPORT;
		// !!!! Fill out the header of END module (after the APP-specific one)
		mod[MON_POWER_PARAM_REPORT_SIZE + PKT_MODULE_LEN_HEADER] = 
			PKT_MODULE_TYPE_END;
		mod[ph->module_length + PKT_MODULE_LEN_HEADER + 1] = 
			original_size - ph->module_length + PKT_MODULE_LEN_HEADER;
	// 4) Fill the content of the APP-specific module
	//    .... (Possibly defered, but not for too long)
		{
			mon_power_param_report_t *param_report = (mon_power_param_report_t *)&(ph->params_list);
			uint8_t	result = 0;
			// A1) Reserve SPI - it should be FREE !!!!!
			if (SPI_R_OK != spi_request_dev(SPI_APP_POWER, 10000, &mon_power_event_flag, mon_power_even_mask))
			{
#if (_MON_POWER_DEBUG_LEVEL_ > 5)
				api_send_packet16 ( "ERROR while Read All Power\r", 20, 0xFFFF );
#endif
				while(1);
			}
#if (_MON_POWER_DEBUG_LEVEL_ > 10)
			api_send_packet16 ( "Modular SPI RESERVED\r", 21, 0xFFFF );
#endif
			// A2) Enable if not on SPI NSS pin
			CS_ADE = MON_POWER_ADE_CS_ENABLE;
			// 3) Transmit etc.
//			delay(50);

			ph->count_params = param_count;
			for(i=0; i < param_count; i++)
			{
				uint8_t p_id = param_list[i*2];
				uint8_t p_len = param_list[i*2+1];
				uint32_t val = 0;

				switch (p_len)
				{
					case 7:
					case 8:
						val = mon_power_read_u8_reg(p_id);
						break;
					case 12:
					case 16:
						val = mon_power_read_u16_reg(p_id);
						break;
					case 24:
						val = mon_power_read_u24_reg(p_id);
						break;
					default:
						val = ADE_ILLEGAL_REG_32;
				}
				inter_spi_delay();

				param_report[i].addr = p_id;
				param_report[i].size = p_len;
				param_report[i].value = val;
			}
			// A4) Disable if not on SPI NSS pin
			CS_ADE = MON_POWER_ADE_CS_DISABLE;	// Deselect the ADE chip
	
			// A5) Release SPI
			spi_release_dev( SPI_APP_POWER);
		}
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	return tsp_send_from_modules(pkt_id);
}

// ############################################################################
// ############################################################################
/**
* mon_power_write_u32_reg( reg, val, write size) - 
* 		writes a value "val" to register "reg" of ADE sending the "write_size"
*		most significant bytes (octets) of the "val" 32-bit parameter
*/
void mon_power_write_u32_reg(uint8_t reg, uint32_t val, uint8_t write_size)
{
	spi_buf_len_t tx_count;
	char	txrx[4];
	uint32_t	value = val;
	char*	val_ch = (char*)(&value);

	// Synchronize SPI bus (reset SPI buffers/timers/registers in compass)

	// Send version request (PING)
	txrx[0] = reg | ADE_WRITE_FLAG;	
	txrx[1] = 0xAA;
	tx_count = spi_request_tx ( SPI_APP_POWER, txrx, 1);
	while (0 == (mon_power_event_flag & SPI_EVENT_DONE));
	mon_power_event_flag &= (~SPI_EVENT_DONE);

//	delay(5);

	// Read response until found "SyncChar" or retries exceeded threshold
	tx_count = spi_request_tx ( SPI_APP_POWER, val_ch, write_size);
	while (0 == (mon_power_event_flag & SPI_EVENT_DONE));
	mon_power_event_flag &= (~SPI_EVENT_DONE);

	mon_power_event_flag &= (~SPI_EVENT_FAILED_RX_FILTERED);

#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		sprintf(debug_buf, "SET_REG_U24 Set(%bu,%bu,%bu) (state=%bu)\r"
			, val_ch[0], val_ch[1], val_ch[2], mon_power_event_flag);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
	return;
}


// ############################################################################
// ############################################################################



/**
* mon_power_read_u24_reg() - reads a 24-bit (unsigned) register (returns 32 bit)
*/
uint32_t mon_power_read_u24_reg(uint8_t reg)
{
	spi_buf_len_t tx_count, rx_count0, rx_count1;
	bit found_sync_byte = 0;
	spi_buf_t	txrx[4];
	uint32_t	result = 0;

	// Synchronize SPI bus (reset SPI buffers/timers/registers in compass)

	// Send version request (PING)
	txrx[0] = reg;	txrx[1] = 0xAA;
	tx_count = spi_request_tx ( SPI_APP_POWER, txrx, 1);
	while (0 == (mon_power_event_flag & SPI_EVENT_DONE));
	mon_power_event_flag &= (~SPI_EVENT_DONE);

	delay(MIN_DELAY_ADE_REG_READ);

	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx ( SPI_APP_POWER, 3);
	while (0 == (mon_power_event_flag & (SPI_EVENT_DONE | SPI_EVENT_PURGED)));
	if (0 == (mon_power_event_flag & SPI_EVENT_FAILED_RX_FILTERED) )
	{
		found_sync_byte = 1;
		rx_count1 = spi_get_rx_block (SPI_APP_POWER, (spi_buf_t*)(&result), rx_count0);
//		result = ((uint16_t*)txrx)[0];

#ifdef _MON_POWER_DEBUG_ENABLE_
#if (_MON_POWER_DEBUG_LEVEL_ > 15)
		sprintf(debug_buf, "Rec(%bu,%bu,%bx)=%bx,%bx\r"
			, rx_count0, rx_count1, mon_power_event_flag
			, txrx[0], txrx[1]);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
#endif // _MON_POWER_DEBUG_ENABLE_
	}
	else
	{
		// ordinary read does not fail ...
		// ... but could hang due to SPI problem
//		com_string(0, "Failed SPI read - no SyncChar found\r");
	}
	mon_power_event_flag &= (~SPI_EVENT_DONE);
	mon_power_event_flag &= (~SPI_EVENT_FAILED_RX_FILTERED);

#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		sprintf(debug_buf, "GET_REG_S16 Rec(%bu,%bu,%bx)=%bu (state=%bu)\r", rx_count0
			, rx_count1, mon_power_event_flag, result, found_sync_byte);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
	return result;
}


/**
* mon_power_read_s16_reg() - reads a 16-bit (signed) register
*/
int16_t mon_power_read_s16_reg(uint8_t reg)
{
	spi_buf_len_t tx_count, rx_count0, rx_count1;
	bit found_sync_byte = 0;
	char	txrx[2];
	int16_t	result = 0;

	// Synchronize SPI bus (reset SPI buffers/timers/registers in compass)

	// Send version request (PING)
	txrx[0] = reg;	txrx[1] = 0xAA;
	tx_count = spi_request_tx ( SPI_APP_POWER, txrx, 1);
	while (0 == (mon_power_event_flag & SPI_EVENT_DONE));
	mon_power_event_flag &= (~SPI_EVENT_DONE);

	delay(MIN_DELAY_ADE_REG_READ);

	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx ( SPI_APP_POWER, 2);
	while (0 == (mon_power_event_flag & (SPI_EVENT_DONE | SPI_EVENT_PURGED)));
	if (0 == (mon_power_event_flag & SPI_EVENT_FAILED_RX_FILTERED) )
	{
		found_sync_byte = 1;
		rx_count1 = spi_get_rx_block (SPI_APP_POWER, txrx, rx_count0);
		result = ((int16_t*)txrx)[0];

#ifdef _MON_POWER_DEBUG_ENABLE_
#if (_MON_POWER_DEBUG_LEVEL_ > 15)
		sprintf(debug_buf, "Rec(%bu,%bu,%bx)=%bx,%bx\r"
			, rx_count0, rx_count1, mon_power_event_flag
			, txrx[0], txrx[1]);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
#endif // _MON_POWER_DEBUG_ENABLE_
	}
	else
	{
		// ordinary read does not fail ...
		// ... but could hang due to SPI problem
//		com_string(0, "Failed SPI read - no SyncChar found\r");
	}
	mon_power_event_flag &= (~SPI_EVENT_DONE);
	mon_power_event_flag &= (~SPI_EVENT_FAILED_RX_FILTERED);

#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		sprintf(debug_buf, "GET_REG_S16 Rec(%bu,%bu,%bx)=%bd (state=%bu)\r", rx_count0
			, rx_count1, mon_power_event_flag, result, found_sync_byte);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
	return result;
}



/**
* mon_power_read_u16_reg() - reads a 16-bit (unsigned) register
*/
uint16_t mon_power_read_u16_reg(uint8_t reg)
{
	spi_buf_len_t tx_count, rx_count0, rx_count1;
	bit found_sync_byte = 0;
	char	txrx[2];
	uint16_t	result = 0;

	// Synchronize SPI bus (reset SPI buffers/timers/registers in compass)

	// Send version request (PING)
	txrx[0] = reg;	txrx[1] = 0xAA;
	tx_count = spi_request_tx ( SPI_APP_POWER, txrx, 1);
	while (0 == (mon_power_event_flag & SPI_EVENT_DONE));
	mon_power_event_flag &= (~SPI_EVENT_DONE);

	delay(MIN_DELAY_ADE_REG_READ);

	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx ( SPI_APP_POWER, 2);
	while (0 == (mon_power_event_flag & (SPI_EVENT_DONE | SPI_EVENT_PURGED)));
	if (0 == (mon_power_event_flag & SPI_EVENT_FAILED_RX_FILTERED) )
	{
		found_sync_byte = 1;
		rx_count1 = spi_get_rx_block (SPI_APP_POWER, txrx, rx_count0);
		result = *((uint16_t*)&(txrx[0]));

#ifdef _MON_POWER_DEBUG_ENABLE_
#if (_MON_POWER_DEBUG_LEVEL_ > 15)
		sprintf(debug_buf, "Rec(%bu,%bu,%bx)=%bx,%bx\r"
			, rx_count0, rx_count1, mon_power_event_flag
			, txrx[0], txrx[1]);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
#endif // _MON_POWER_DEBUG_ENABLE_
	}
	else
	{
		// ordinary read does not fail ...
		// ... but could hang due to SPI problem
//		com_string(0, "Failed SPI read - no SyncChar found\r");
	}
	mon_power_event_flag &= (~SPI_EVENT_DONE);
	mon_power_event_flag &= (~SPI_EVENT_FAILED_RX_FILTERED);

#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		sprintf(debug_buf, "GET_REG_S16 Rec(%bu,%bu,%bx)=%bu (state=%bu)\r", rx_count0
			, rx_count1, mon_power_event_flag, result, found_sync_byte);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
	return result;
}

/**
* mon_power_read_s8_reg() - reads a 8-bit (signed) register
*/
int8_t mon_power_read_s8_reg(uint8_t reg)
{
	spi_buf_len_t tx_count, rx_count0, rx_count1;
	bit found_sync_byte = 0;
	uint8_t	txrx[2];
	int8_t	result = 0;

	// Synchronize SPI bus (reset SPI buffers/timers/registers in compass)

	// Send version request (PING)
	txrx[0] = reg;	txrx[1] = 0xAA;
	tx_count = spi_request_tx ( SPI_APP_POWER, txrx, 1);
	while (0 == (mon_power_event_flag & SPI_EVENT_DONE));
	mon_power_event_flag &= (~SPI_EVENT_DONE);

	delay(MIN_DELAY_ADE_REG_READ);

	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx ( SPI_APP_POWER, 1);
	while (0 == (mon_power_event_flag & (SPI_EVENT_DONE | SPI_EVENT_PURGED)));
	if (0 == (mon_power_event_flag & SPI_EVENT_FAILED_RX_FILTERED) )
	{
		found_sync_byte = 1;
		rx_count1 = spi_get_rx_block (SPI_APP_POWER, txrx, rx_count0);
		result = txrx[0];

#ifdef _MON_POWER_DEBUG_ENABLE_
#if (_MON_POWER_DEBUG_LEVEL_ > 15)
		sprintf(debug_buf, "Rec(%bu,%bu,%bx)=%bx,%bx\r"
			, rx_count0, rx_count1, mon_power_event_flag
			, txrx[0], txrx[1]);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
#endif // _MON_POWER_DEBUG_ENABLE_
	}
	else
	{
		// ordinary read does not fail ...
		// ... but could hang due to SPI problem
//		com_string(0, "Failed SPI read - no SyncChar found\r");
	}
	mon_power_event_flag &= (~SPI_EVENT_DONE);
	mon_power_event_flag &= (~SPI_EVENT_FAILED_RX_FILTERED);

#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		sprintf(debug_buf, "GET_REG_S16 Rec(%bu,%bu,%bx)=%bd (state=%bu)\r", rx_count0
			, rx_count1, mon_power_event_flag, result, found_sync_byte);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
	return result;
}


/**
* mon_power_read_u8_reg() - reads a 8-bit (unsigned) register
*/
uint8_t mon_power_read_u8_reg(uint8_t reg)
{
	spi_buf_len_t tx_count, rx_count0, rx_count1;
	bit found_sync_byte = 0;
	uint8_t	txrx[2];
	uint8_t	result = 0;

	// Synchronize SPI bus (reset SPI buffers/timers/registers in compass)

	// Send version request (PING)
	txrx[0] = reg;	txrx[1] = 0xAA;
	tx_count = spi_request_tx ( SPI_APP_POWER, txrx, 1);
	while (0 == (mon_power_event_flag & SPI_EVENT_DONE));
	mon_power_event_flag &= (~SPI_EVENT_DONE);

	
#ifdef _SFRPAGE_EXIST_
//	SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
//	SPI0CFG   = 0x50;
//	delay(MIN_DELAY_ADE_REG_READ);

	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx ( SPI_APP_POWER, 1);
	while (0 == (mon_power_event_flag & (SPI_EVENT_DONE | SPI_EVENT_PURGED)));
	if (0 == (mon_power_event_flag & SPI_EVENT_FAILED_RX_FILTERED) )
	{
		found_sync_byte = 1;
		rx_count1 = spi_get_rx_block (SPI_APP_POWER, txrx, rx_count0);
		result = txrx[0];

#ifdef _MON_POWER_DEBUG_ENABLE_
#if (_MON_POWER_DEBUG_LEVEL_ > 15)
		sprintf(debug_buf, "Rec(%bu,%bu,%bx)=%bx,%bx\r"
			, rx_count0, rx_count1, mon_power_event_flag
			, txrx[0], txrx[1]);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
#endif // _MON_POWER_DEBUG_ENABLE_
	}
	else
	{
		// ordinary read does not fail ...
		// ... but could hang due to SPI problem
//		com_string(0, "Failed SPI read - no SyncChar found\r");
	}
	mon_power_event_flag &= (~SPI_EVENT_DONE);
	mon_power_event_flag &= (~SPI_EVENT_FAILED_RX_FILTERED);

#ifdef _SFRPAGE_EXIST_
	SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
//	SPI0CFG   = 0x70;

#if (_MON_POWER_DEBUG_LEVEL_ > 5)
		sprintf(debug_buf, "GET_REG_S16 Rec(%bu,%bu,%bx)=%bu (state=%bu)\r", rx_count0
			, rx_count1, mon_power_event_flag, result, found_sync_byte);
		api_send_packet16 ( debug_buf, strlen(debug_buf), 0xFFFF );
#endif
	return result;
}


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################



#endif // _ENABLE_NEM_UTILITIES_01_

