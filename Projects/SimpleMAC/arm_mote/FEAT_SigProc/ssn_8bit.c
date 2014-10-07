#include "defs/esnl_pub.h"

#ifdef _ENABLE_SSN_8BIT_

#include "ssn_8bit_prv.h"
#include "ssn_8bit_pub.h"

#include "FEAT_Queuing/Queuing.h"
//#include "sys_func.h"

//#include "packet.h"
//#include "common.h"


#include "FEAT_Networking/tsp_common_pub.h"
#include "FEAT_Scheduler/sch_basic_pub.h"
//#include "HW_LIB/new_adc_0_pub.h"

#include "sys_func.h"
#include "common.h"

#ifdef _ENABLE_SRC_DUMMY_
	#define _BRIDGE_EXP_
#endif // _ENABLE_SRC_DUMMY_

// generate 255 packets in a row
//#define SSN_TEST
//#undef SSN_TEST

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/



ssn_mem_t XDATA* ssn_mem_p;

uint8_t	XDATA ssn_mem_raw[SSN_MEM_STRUCT_SIZE];
SSN_BUFF_ITEM_T	XDATA ssn_buffer_raw[SSN_BUFF_SIZE];


/**
 * ssn_init () - sets up Signal Processing Module
 */
void ssn_init ( void )
{
//	float XDATA * temp_coeff;
	int i;
	for( i=0; i < SSN_BUFF_SIZE; i++) { ssn_buffer_raw[i]=i; }

	ssn_mem_p = (ssn_mem_t XDATA* )ssn_mem_raw;

	ssn_mem_p-> head_ = 0;
	ssn_mem_p-> tail_ = 0;
	ssn_mem_p-> buff = ssn_buffer_raw;
	ssn_mem_p-> rate_ = 400; // rate in Hz
	// TODO: setup NEW ADC to use this rate

	ssn_mem_p-> next_pkt_ = SSN_8BIT_SPP;
	ssn_mem_p-> start_time_ = 0; // start index for the Packet
	ssn_mem_p-> start_index_ = 0; // start index for the Packet
	ssn_mem_p-> burst_enabled_ = 0; // send a burst of a given size

	ssn_mem_p-> pause_ = 0; // send a burst of a given size
	ssn_mem_p-> pause_timeout_ = 0; // send a burst of a given size
	
	ssn_mem_p->burst_timeout_ = 0;

	#ifdef SSN_TEST
		ssn_mem_p-> burst_enabled_ = 10000;//255; // send a burst of a given size	
	#endif // SSN_TEST
	sch_add_loop( (sch_loop_func_t) ssn_loop);

	ssn_mem_p-> seq_no_ = 0;
	
#ifdef _BRIDGE_EXP_
	// (adc_channel, context, receiver)
	//adc0_attach(0, 0x22, NULL);
	//adc_resumeADC();
#endif // _BRIDGE_EXP_
}


/**
 * uint16_t ssn_start_burst(uint16_t len) - starts the burst transmission of LEN packets (e.g.)
 * RETURNS the number of scheduled packets or 0 is failed/already transmitting
 */
uint16_t ssn_start_burst(uint16_t len)
{
	if (0 == ssn_mem_p -> burst_enabled_)
	{	// Reset buffer (if this is what we want????)
		ssn_mem_p-> seq_no_ = 0;
		ssn_mem_p-> head_ = 0;
		ssn_mem_p-> tail_ = 0;
		ssn_mem_p->next_pkt_ =  ssn_mem_p->head_ + SSN_8BIT_SPP;
		return ssn_mem_p-> burst_enabled_ = len; // send a burst of a given size	
	}
	else
	{
		return 0;
	}
}	
	
/**
 * uint16_t ssn_stop_burst() - stops the burst transmission
 * RETURNS 1 when successful (always??
 */
uint16_t ssn_stop_burst()
{
	if (0 != ssn_mem_p -> burst_enabled_)
	{	// Disable and reset buffer (??)
		ssn_mem_p -> burst_enabled_ = 0;
		ssn_mem_p-> head_ = 0;
		ssn_mem_p-> tail_ = 0;
		ssn_mem_p->next_pkt_ =  ssn_mem_p->head_ + SSN_8BIT_SPP; // not needed ??
		return 1;
	}
	else
	{
		return 1; // already disabled
	}
}	
	
	
	/**
	 * ssn_test() - tests Signal Processing operation (e.g.)
	 */
	uint8_t ssn_test();
	
	
/**
 * ssn_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
 */
void ssn_loop( void )
{
	//	ssn_mem_p->sample_interval_ = (ssn_mem_p->last_tick_ - ssn_mem_p->tail_tick_);
	if ( 1 == ssn_mem_p->pause_)
	{
		if ( ssn_mem_p->pause_timeout_ < rtc_get_ticks())
		{
			ssn_mem_p->pause_ = 0;
		}
	}
	else if ( 0 < ssn_mem_p->burst_enabled_)
	{
		if ( ( 0 < (ssn_mem_p->head_ - ssn_mem_p->next_pkt_) )
			&& ( 0xFF00 > (ssn_mem_p->head_ - ssn_mem_p->next_pkt_) ) )
		{
			if (0 == ssn_send_pkt(SSN_8BIT_SPP))
			{
				ssn_mem_p->pause_ = 1;
				ssn_mem_p->pause_timeout_ = rtc_get_ticks() + SSN_PAUSE_DELAY;
			}
			else
			{
				ssn_mem_p->burst_enabled_--;
				ssn_mem_p->next_pkt_ += SSN_8BIT_SPP;
				ssn_mem_p->start_index_ = ssn_mem_p->next_pkt_ - SSN_8BIT_SPP ;
				//ssn_mem_p->start_time_ = rtc_get_ticks() - (SSN_SPP*1000/ssn_mem_p->rate_);
				ssn_mem_p->start_time_ = ssn_mem_p->start_time_ + (1000UL*SSN_8BIT_SPP/(uint32_t)ssn_mem_p->rate_);
			}
		}
	}
	else
	{
		/* // do nothing if disabled
		if ( ssn_mem_p->burst_timeout_ > rtc_get_ticks())
		{
			ssn_mem_p->burst_enabled_ = 10;
		}
		*/
//		adc_suspendADC();
	}
	// If packet is filled then sent out
}
	
	
	/**
	 * ssn_recv_sample(uint8_t context, uint16_t value) - receives a sample
	 */
void ssn_recv_sample(uint8_t context, uint16_t value)
{
	(ssn_mem_p->buff)[ssn_mem_p->head_ & SSN_BUFF_MASK] = value;
	ssn_mem_p->head_++;
	if (0 == ((ssn_mem_p->head_ - ssn_mem_p->tail_) & SSN_BUFF_MASK))
	{
		// eat your own tail
		ssn_mem_p->tail_++;
	}
}


/**
 * ssn_execute_command(packet) - executes a command received in "packet"
 */
void ssn_execute_command ( uint8_t *packet)
{	
	ssn_command_mod_v1_t *cmd = (ssn_command_mod_v1_t*)packet;
	switch (cmd->command)
	{
		case SSN_CMD_SET:
			// set interval and other params
			return;
			//break;
		case SSN_CMD_REQ:
//			ssn_mem_p->start_time_ = cmd->ticks;
//			ssn_mem_p->start_index_ = ssn_mem_p->head_ - ssn_time_offset(cmd->ticks);
			//adc_set_rate_divider((uint8_t)((uint16_t)20000 / cmd->rate));
			ssn_mem_p->rate_ = cmd->rate;
			ssn_mem_p->start_time_ = rtc_get_ticks();
			ssn_mem_p->start_index_ = ssn_mem_p->head_;
			ssn_mem_p->burst_timeout_ = rtc_get_ticks() + 1000UL * cmd->mod_data;
			ssn_mem_p->next_pkt_ =  ssn_mem_p->start_index_ + SSN_8BIT_SPP;
//			ssn_send_pkt(cmd->mod_data); // sample count to send
			break;
	}
}


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/**
 * ssn_time_offset(start_ticks) -
 *	
 */
uint16_t ssn_time_offset(uint32_t start_ticks)
{
	uint16_t off;
	uint32_t d = rtc_get_ticks() - start_ticks;
	d = d * ssn_mem_p->rate_ / 1000;
	off = (uint16_t)d;
	return off;
}

/**
 * ssn_send_pkt(sample_count)
 *	
 */
uint8_t ssn_send_pkt(uint8_t sample_count)
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = NO_PACKET_INDEX;
#ifdef SSN_WITH_RAW_FORMAT
	uint8_t mod_len = sample_count + MOD_RAW_DATA_OVERHEAD;
#else // SSN_WITH_RAW_FORMAT
	uint8_t mod_len = SSN_SENSOR_DATA_V1_SIZE + sample_count;//sizeof(SSN_BUFF_ITEM_T);
#endif // else SSN_WITH_RAW_FORMAT
	if (0 == tsp_reserve_packet( mod_len, &pkt_id, BS_ADDR))
	{
		// Error
		sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	{
		IAR_PACKED uint8_t XDATA*pkt_samples = NULL;
		uint16_t i, temp;
		// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t XDATA*mod = (uint8_t XDATA*)tsp_get_dummy(pkt_id);
		// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
		//    .... (Possibly defered, but not for too long)
		ssn_command_mod_v1_t XDATA*ph = ( ssn_command_mod_v1_t XDATA* )mod ;
#ifdef SSN_WITH_RAW_FORMAT
		pkt_mod_raw_data_t *raw = (pkt_mod_raw_data_t *)(&(ph->command));
		ph->module_type = PKT_MODULE_TYPE_RAW_CHANNEL;
		ph->module_length = mod_len;
		// 4) Fill the content of the APP-specific module
		//    .... (Possibly defered, but not for too long)
		raw -> seq_num = ssn_mem_p-> seq_no_++;
		raw -> channel = MY_ADC_CHANNEL;
		raw -> numsamples = sample_count;
		pkt_samples = (uint8_t XDATA*)(&(raw->samples_list));
#else
		ph ->module_type = PKT_MODULE_TYPE_SSN_V1;
		ph ->module_length = mod_len;
		// 4) Fill the content of the APP-specific module
		//    .... (Possibly defered, but not for too long)
		ph ->command = SSN_DATA_V1;
		ph -> ticks = ENDIAN32(ssn_mem_p->start_time_);
		ph -> rate = ENDIAN16(ssn_mem_p->rate_);
		ph -> channel = MY_ADC_CHANNEL;
		ph -> seq_num = ssn_mem_p->seq_no_++;
		pkt_samples = (uint8_t XDATA*)(&(ph->mod_data));
#endif // not SSN_WITH_RAW_FORMAT
		for( i = 0; i < sample_count; i++)
		{
			temp = (ssn_mem_p->buff)[(i+ssn_mem_p->start_index_)&SSN_BUFF_MASK];
			pkt_samples[i] = (uint8_t)(temp/100); // Divide by 100 to get value in 0.01V units (10mV)
			//(ssn_mem_p->buff)[(i+ssn_mem_p->start_index_)&SSN_BUFF_MASK];
		}		
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	return tsp_send_from_modules(pkt_id);
}

/**
* sig_filter_new_data(input) -
*	
*/
//void sig_filter_reset(sig_filter_mem_t XDATA *filter);

/**
* sig_filter_new_config(module) -
*	
*/
//void sig_filter_new_config(sig_filter_mem_t XDATA *filter, uint8_t * module)


/**
* sig_filter_new_data(input) -
*	
*/
/*
float sig_filter_new_data( sig_filter_mem_t XDATA *filter, float input )
{
	float new_y = B[1] * input;
	X[idx%] = input;
    for(i=2; i<3; i++)
	{
		new_y = new_y + X(idx-i+1)* B(i) - Y(idx-i+1)*A(i);
	}
	Y[idx] = new_y;
}
*/

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################



#endif // _ENABLE_SSN_BASIC_

