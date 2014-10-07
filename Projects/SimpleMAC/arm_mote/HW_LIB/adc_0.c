/****************************************************************************
**
** Copyright (C) 2006-2005 Maciej Zawodniok, James W. Fonda. All rights reserved.
**
** This file is part of the documentation of the UMR Mote Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "adc_0.h"

#ifdef _ENABLE_ADC_MODULE_

#include "uart_0.h"
#include "common.h"
#include "Application.h"
#include "FEAT_Networking/tsp_common_pub.h"
#include "sys_func.h"


int xdata adc0_head[MAX_ADCS];
int xdata adc0_tail[MAX_ADCS];
unsigned int xdata adc0_buffer[ADC0_BUFFER_SIZE*MAX_ADCS];

// ##############################################
// ## Common configuration for all ADC channels
// ##############################################
//
uint8_t xdata adc_sample_rate_divider_;
// Averaging samples over N readings
uint8_t xdata adc_sample_averaging_; // 1 -> no averaging

// counter for sample rate divider
int xdata sample_rate_counter_;

//
unsigned char xdata send_adc_packet[MAX_ADCS];

unsigned char xdata current_channel_;
unsigned char xdata number_of_channels_;

// ##############################################
// ## ADC channel specific configuration
// ##############################################
//
int xdata sample_count[MAX_ADCS];
//
int xdata sample_averaging_counter_[MAX_ADCS];
// temporary sum for averaging
uint16_t xdata average_sum_[MAX_ADCS];
// What is the PIN address for ADC chanels
uint8_t xdata channel_mapping_[MAX_ADCS];


#define ADV0_CHANNEL_INDEX(channel, index) (channel*ADC0_BUFFER_SIZE + index)




#ifdef ADC_ENABLE_CYCLE_COUNT
uint8_t adc_cycle_count; // count cycles
//	rtc_t adc_cycle_timestamp;
uint8_t climbing;
uint16_t adc_cycle_peak;
uint16_t adc_cycle_min;
uint16_t adc_cycle_peak_max;
#endif // ADC_ENABLE_CYCLE_COUNT

// ##############################################
//unsigned char n_samples_per_packet_ = SAMPLES_PER_PACKET;	// samples per packet


// #############################################################
// ## Local declarations									  ##
// #############################################################

/**
  * adc0_need_to_send_packet() - returns id of channel that has data to be sent, or NO_CHANNEL if there is nothing to sent
  */
unsigned char adc0_need_to_send_packet();

/**
  * adc0_clear_buffer() - clears up buffer for all A/D channels
  */
void adc0_clear_buffer();



// #############################################################
// ## Functions' body										  ##
// #############################################################

/**
  * adc0_setup() - sets up buffer for all A/D channels
  */
void adc0_setup()
{
	unsigned char i, j;
//	number_of_channels_ = MAX_ADCS;
	number_of_channels_ = 1;

	adc_sample_rate_divider_ = DEF_DIVIDER / number_of_channels_;
	// Averaging samples over N readings
	adc_sample_averaging_ = DEF_N_AVERAGING; // 1 -> no averaging

	adc0_clear_buffer();

	for ( i = 0; i < MAX_ADCS; i++ )
		for ( j = 0; j < ADC0_BUFFER_SIZE; j++ )
		{
			adc0_buffer[ADV0_CHANNEL_INDEX( i, j )] = 0x10 + i;
		}

#ifdef ADC_ENABLE_CYCLE_COUNT
	adc_cycle_count = 0;
//	adc_cycle_timestamp = 0;
	climbing = 1;
	adc_cycle_peak = 0;
	adc_cycle_min = ADC_MAX_VALUE -  ADC_CYCLE_MIN_INCREASE;
	adc_cycle_peak_max = 0;

	adc_sample_rate_divider_ = 1;
	// Averaging samples over N readings
	adc_sample_averaging_ = 25; // 1 -> no averaging

#endif // ADC_ENABLE_CYCLE_COUNT

#ifdef _UMR_MOTE_F1_
	// This is hardcoded for testing
//	AMX0P = 0x13; // P1.1 for ADC
//	channel_mapping_[0] = 0x13;
    AMX0P     = 0x0A; // P3.7 - strain
    AMX0N     = 0x1F; // GND
	channel_mapping_[0] = 0x0A; 
#endif
#ifdef _ENABLE_ROBOTICS_04_
	channel_mapping_[0] = 1;
	channel_mapping_[1] = 0;
	channel_mapping_[2] = 2;
#endif // _ENABLE_ROBOTICS_04_
}




/**
  * adc0_need_to_send_packet() - returns id of channel that has data to be sent, or NO_CHANNEL if there is nothing to sent
  */
unsigned char adc0_need_to_send_packet()
{
	unsigned char i;
	for ( i = 0; i < number_of_channels_; i++ )
	{
		if ( 0 != send_adc_packet[i] )
		{
			return i;
		}
	}
	return NO_CHANNEL;

}


/**
  * adc0_clear_buffer() - clears up buffer for all A/D channels
  */
void adc0_clear_buffer()
{
	unsigned char i;

	// counter for sample rate divider
	sample_rate_counter_ = 0;

	for ( i = 0; i < number_of_channels_; i++ )
	{
		send_adc_packet[i] = 0; // no packet to be sent
		// count samples ready to be sent
		sample_count[i] = 0;
		//
		//
		sample_averaging_counter_[i] = 0;
		// temporary sum for averaging
		average_sum_[i] = 0;


		adc0_head[i] = 0;
		adc0_tail[i] = 0;

		// By default use all consecutive channels
		channel_mapping_[i] = i;
	}
#ifdef _UMR_MOTE_F1_
	// This is hardcoded for testing
//	AMX0P = 0x13; // P1.1 for ADC
//	channel_mapping_[0] = 0x13;
    AMX0P     = 0x0A; // P3.7 - strain
    AMX0N     = 0x1F; // GND
	channel_mapping_[0] = 0x0A; 
#endif
#ifdef _ENABLE_ROBOTICS_04_
	channel_mapping_[0] = 1;
	channel_mapping_[1] = 0;
	channel_mapping_[2] = 2;
#endif // _ENABLE_ROBOTICS_04_
}


/**
  * set_number_of_channels(num_of_channels) - sets the number of active channels
  *		is restricted from top by MAX_ADCS
  */
void set_number_of_channels( unsigned char num )
{
	if ( num > MAX_ADCS )
	{
		number_of_channels_ = MAX_ADCS;
	}
	else
	{
		number_of_channels_ = num;
	}

	adc0_clear_buffer();

	current_channel_ = number_of_channels_ -1; // when the interrupt will be executed the current_channell_ will be rolled-over to ZERO
}


/**
  * adc0_not_empty(channel) - checks the size of waiting samples (in buffer)
  * returns number of ready samples (0-MAX_BUF), or '-1' if channel number was incorrect
  */
int adc0_not_empty( unsigned char channel )
{
	int ret;

	if ( MAX_ADCS > channel )
	{
		ret = ( int ) adc0_head[channel] - ( int ) adc0_tail[channel];
		if ( ret < 0 )
			ret = ADC0_BUFFER_SIZE + ret;

		return ret;
	}
	// wrong channel number
	return ( -1 );
}


/**
  * int adc0_get_value(channel) - returns value of the first sample in the buffer (longest waiting)
  * returns sample value (0-256)8-bit or (0-1024)10-bit; or '-1' if incorrect channel number
  */
unsigned int adc0_get_value( unsigned char channel )
{
	unsigned int ret;

	if ( MAX_ADCS > channel )
	{
		if ( adc0_not_empty( channel ) != 0 )
		{
			ret = adc0_buffer[ADV0_CHANNEL_INDEX( channel, adc0_tail[channel] )];
			adc0_tail[channel] ++;
			if ( adc0_tail[channel] >= ADC0_BUFFER_SIZE )
				adc0_tail[channel] = 0;
		}
		else
			ret = 0;

		return ret;
	}
	// incorrect channel number
	return ( 0 );
}

uint16_t adc_test[3];
/**
  * int adc0_get_value_threshed(channel) - returns value of '1' if the last sample in the buffer (shortest waiting)
  * is greater than the specified threshold, '0' else; or '-1' if incorrect channel number
  */
unsigned int adc0_get_value_threshed ( uint8_t channel, unsigned int threshold )
{
	uint16_t adc_val=0;
	uint8_t  ret=0;
	
	if ( ( channel >= 0 ) && ( MAX_ADCS > channel ) )
	{
		if ( adc0_not_empty ( channel ) != 0 )
		{
			adc_val = adc0_buffer[ADV0_CHANNEL_INDEX ( channel, adc0_tail[channel] ) ];
			adc_test[channel] = adc_val;
			adc0_tail[channel] ++;
			if ( adc0_tail[channel] >= ADC0_BUFFER_SIZE )
				adc0_tail[channel] = 0;
		}
		else
			adc_val = 0;

		if (adc_val>threshold)
			ret=1;

		return ret;
	}
	// incorrect channel number
	return ( 255 );
}


/**
  * uint16_t adc0_get_average ( channel, count );- returns average value
  *		for "count" first samples of "channel"
  * returns 0xFFFF when failed, or the calculated value otherwise
  * NOTE: Assumes that there are the "count" samples
  */
uint16_t adc0_get_average( uint8_t channel, uint8_t count )
{
	uint16_t result = 0;
	uint8_t	i;
	if ( ADC0_BUFFER_SIZE < count )	return 0xFFFF;
	for ( i = 0; i < count; i++ )
	{
		if ( adc0_not_empty( channel ) != 0 )
		{
			result += adc0_buffer[ADV0_CHANNEL_INDEX( channel, adc0_tail[channel] )];
			adc0_tail[channel] ++;
			if ( adc0_tail[channel] >= ADC0_BUFFER_SIZE )
				adc0_tail[channel] = 0;
		}
		else
			result += 0;
	}
	return ( result / count );
}



/**
  * uint16_t adc0_get_average ( channel, count );- returns average value
  *		for "count" first samples of "channel"
  * returns 0xFFFF when failed, or the calculated value otherwise
  * NOTE: Assumes that there are the "count" samples
  */
uint16_t adc0_get_max( uint8_t channel, uint8_t count )
{
	uint16_t result = 0;
	uint16_t temp_val = 0;
	uint8_t	i;
	if ( ADC0_BUFFER_SIZE < count )	return 0xFFFF;
	for ( i = 0; i < count; i++ )
	{
		if ( adc0_not_empty( channel ) != 0 )
		{
			temp_val = adc0_buffer[ADV0_CHANNEL_INDEX( channel, adc0_tail[channel] )];
			if ( temp_val > result )
				result = temp_val;
			adc0_tail[channel] ++;
			if ( adc0_tail[channel] >= ADC0_BUFFER_SIZE )
				adc0_tail[channel] = 0;
		}
		else
			result = 0;
	}
	return result;
}


//Called when Conversion Finishes

#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void ADC0_ISR() TASK_FUNC
#else
	// Case of the static interrupt on F120
	void ADC0_ISR() interrupt HWM_INT_ADC0_EOC //15
#endif

{
	unsigned int value;

	//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = ADC0_PAGE;
#endif // _SFRPAGE_EXIST_
	AD0INT = 0; //Clear Conversion Interupt
	value = ADC0H;
	value = value << 8;  //Bit shift Left
	value += ADC0L;	//Append the value with the lower 8 bits

	// sum up the read value with temporary value
	average_sum_[current_channel_] += value;

	// check if enough data read to average
	sample_averaging_counter_[current_channel_]++;
	if ( sample_averaging_counter_[current_channel_] >= adc_sample_averaging_ )
	{
		// got the last sample -> average the result
		adc0_buffer[ADV0_CHANNEL_INDEX( current_channel_, adc0_head[current_channel_] )] 
			= average_sum_[current_channel_] / adc_sample_averaging_;


#ifdef ADC_ENABLE_CYCLE_COUNT
		{
			uint16_t taver = average_sum_[current_channel_] / adc_sample_averaging_;
			if ( climbing )
			{
				if ( taver > adc_cycle_peak )
				{
					// still climbing
					adc_cycle_peak = taver;
//					adc_cycle_timestamp = rtc_get_ticks();
				}
				else if ( taver + ADC_CYCLE_MIN_DECREASE < adc_cycle_peak )
				{
					// reached the max -> count the cycle, store max value, and reset
					adc_cycle_count++;
					if ( adc_cycle_peak > adc_cycle_peak_max )
					{
						adc_cycle_peak_max = adc_cycle_peak;
					}
					adc_cycle_peak = 0;
					climbing = 0;
				}
			}
			else // not climbing
			{
				if ( taver < adc_cycle_min )
				{
					// still climbing
					adc_cycle_min = taver;
				}
				else if ( taver > ADC_CYCLE_MIN_INCREASE + adc_cycle_min )
				{
					// reached the min -> reset
					adc_cycle_min = ADC_MAX_VALUE - ADC_CYCLE_MIN_INCREASE;
					climbing = 1;
				}
			}
		}
#endif // ADC_ENABLE_CYCLE_COUNT

		// move HEAD since stored a sample
		adc0_head[current_channel_] ++;
		if ( adc0_head[current_channel_] >= ADC0_BUFFER_SIZE )
		{
			adc0_head[current_channel_] = 0;
		}

		// If head is cought up with tail -> eat it!!! (== is OK since we added one)
		if ( adc0_head[current_channel_] == adc0_tail[current_channel_] )
		{
			adc0_tail[current_channel_]++;
			if ( ADC0_BUFFER_SIZE <= adc0_tail[current_channel_] )
			{
				adc0_tail[current_channel_] = 0;
			}
		}

		sample_count[current_channel_]++;
		if ( n_samples_per_packet_ <= sample_count[current_channel_] )
		{
			send_adc_packet[current_channel_] = 1;
			sample_count[current_channel_] = 0;
		}
		// reset averaging counter and temp. sum
		sample_averaging_counter_[current_channel_] = 0;
		average_sum_[current_channel_] = 0;
	}
	else
	{
		// continue reading samples for averaging
	}

	current_channel_++;
	if ( current_channel_ >= number_of_channels_ )
	{
		current_channel_ = 0;
	}
	//set the channel to sample next
#ifdef _F310_
	AMX0P = channel_mapping_[current_channel_];
#else
#ifdef _F340_
	AMX0P = channel_mapping_[current_channel_];
#else
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = ADC0_PAGE;
#endif // _SFRPAGE_EXIST_
	AMX0SL = channel_mapping_[current_channel_];  //Sets channel that will be sampled
#endif
#endif

}




/**
  * bit adc0_skip_samples(channel, skip_n) - skips "skip_n" samples in the buffer for "channel"
  * returns "1" if successful or "0" if there was not enough samples to skip
  */
bit adc0_skip_samples( unsigned char channel, unsigned char skip_n )
{
	int available = adc0_not_empty( channel );
	if ( skip_n > available )
	{
		// Delete anyway??
		return 0;
	}
	adc0_tail[channel] += skip_n;
	if ( adc0_tail[channel] >= ( int ) ADC0_BUFFER_SIZE )
	{
		adc0_tail[channel] -= ( int ) ADC0_BUFFER_SIZE;
	}
	// reset flag if new size is less than no of samples per packet
	available -= skip_n;
	if ( available < n_samples_per_packet_ )
	{
		send_adc_packet[channel] = 0;
	}

	return 1;
}


#ifdef FEAT_REPLICATOR
// Case of custom interrupt handler
void TIMER2_ISR() TASK_FUNC
#else
// Case of the static interrupt on F120/F340 (Timer 2)
void TIMER2_ISR() interrupt HWM_INT_TIMER2
#endif

{
	//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = TMR2_PAGE;
#endif // _SFRPAGE_EXIST_
#ifdef _F340_
	TF2H = 0;
#else // _F340_
	TF2 = 0;
#endif // else _F340_
	//	TF2H = 0;

	// only read sample every "sample_rate_divider_" interrupts
	sample_rate_counter_++;
	if ( sample_rate_counter_ >= adc_sample_rate_divider_ )
	{
		//for( i = 0; i < 500; i++ )
		//	{i++;
		//	};

		ADC0CN |= 0x10;  //Start ADC0 interupt flag
		sample_rate_counter_ = 0;
	}
}




#ifdef ADC_ENABLE_CYCLE_COUNT




void adc_send_cycle_packet()
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
	uint8_t size = tsp_reserve_packet( PKT_MODULE_LEN_ADC_CYCLE_REPORT_V1+2, &pkt_id, BS_ADDR );
	uint8_t original_size = 0;
	if ( 0 == size ) // Error
	{
		sys_error( SYS_ERROR_MEM_FULL );
		return;
	}

	{
		// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy( pkt_id );
		// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
		//    .... (Possibly defered, but not for too long)
		cycle_report_packet_v1_t *ph = ( cycle_report_packet_v1_t* )mod;
		ph->module_type = PKT_MODULE_TYPE_ADC_CYCLE_REPORT_V1;
		ph->module_length = PKT_MODULE_LEN_ADC_CYCLE_REPORT_V1;

		// !!!! Fill out the header of END module (after the APP-specific one)
		mod[ph->module_length + PKT_MODULE_LEN_HEADER] =
			PKT_MODULE_TYPE_END;
		mod[ph->module_length + PKT_MODULE_LEN_HEADER + 1] =
			original_size - ph->module_length + PKT_MODULE_LEN_HEADER;
		// 4) Fill the content of the APP-specific module
		//    .... (Possibly defered, but not for too long)
		ph->report_ver = ADC_CYCLE_REPORT_V1;

		ph->cycle_count = adc_cycle_count;
		adc_cycle_count = 0;

		ph->max_value = adc_cycle_peak_max;
		adc_cycle_peak_max = 0;
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	tsp_send_from_modules( pkt_id );
}



void adc_cycle_loop()
{
	if (adc_cycle_count > ADC_CYCLE_SENDING_TRIGGER_LEVEL)
	{
		adc_send_cycle_packet();
	}

}

#endif // ADC_ENABLE_CYCLE_COUNT

#endif // _ENABLE_ADC_MODULE_

