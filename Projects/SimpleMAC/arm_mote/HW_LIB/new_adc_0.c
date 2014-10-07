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

#include "new_adc_0_prv.h"

#ifdef _ENABLE_NEW_ADC_MODULE_

#include "common.h"
#include "sys_func.h"

#ifdef _ENABLE_SSN_BASIC_
	#include "FEAT_SigProc/ssn_basic_pub.h"
#endif // _ENABLE_SSN_BASIC_
// ##############################################
// ## Common configuration for all ADC channels
// ##############################################
//
uint8_t xdata adc_rate_divider_;
// counter for sample rate divider
int xdata adc_sample_rate_counter_;

uint8_t xdata adc_current_channel_;
uint8_t xdata adc_number_of_channels_;

// ##############################################
// ## ADC channel specific configuration
// ##############################################
//
// What is the PIN address for ADC chanels
uint8_t xdata adc_channel_mapping_[MAX_ADCS];
//p_func_t xdata channel_receiver_[MAX_ADCS];
uint8_t xdata channel_context_[MAX_ADCS];


// ##############################################
//unsigned char n_samples_per_packet_ = SAMPLES_PER_PACKET;	// samples per packet


// #############################################################
// ## Local declarations									  ##
// #############################################################

uint8_t xdata adc_rate_divider_;
unsigned char xdata adc_number_of_channels_;


// #############################################################
// ## Functions' body										  ##
// #############################################################

/**
  * adc0_setup() - sets up buffer for all A/D channels
  */
void adc0_setup()
{
	unsigned char i;
	adc_number_of_channels_ = 0;

	adc_rate_divider_ = DEF_DIVIDER;// / adc_number_of_channels_;

	for ( i = 0; i < MAX_ADCS; i++ )
	{
		// By default use all consecutive channels
		adc_channel_mapping_[i] = i;
//		channel_receiver_[i] = NULL; // no receiver assigned (function pointer -> RECEIVER(8,16))
		channel_context_[i] = 0; // no context initially -> dictated by receiver
	}
}

/**
 * adc0_attach(channel_mapping, channel_context, receiver) - activates and connects 
 *    an A/D channel to data processsing module
 */
uint8_t adc0_attach(uint8_t channel_mapping, uint8_t channel_context, p_func_t receiver)
{
	if (MAX_ADCS > adc_number_of_channels_)
	{
		adc_channel_mapping_[adc_number_of_channels_] = channel_mapping;
//		channel_receiver_[adc_number_of_channels_] = receiver; //  RECEIVER(8,16))
		channel_context_[adc_number_of_channels_] = channel_context; // 
		
		adc_number_of_channels_++;
		adc_rate_divider_ = DEF_DIVIDER / adc_number_of_channels_;
		return (adc_number_of_channels_ - 1);
	}
	return 0xFF; // Nothing added
}


/**
 * adc_set_rate_divider(divider) - sets the sampling rate divider (NOMINAL/DIVIDER)
 */
void adc_set_rate_divider(uint8_t divider)
{
	adc_rate_divider_ = divider;
}


void adc_suspendADC()
{
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = TMR2_PAGE;
#endif // _SFRPAGE_EXIST_
	//	TMR2CN = 0x05; // bit 2 - timer enable; bit 0 - capture mode
	TMR2CN &= 0xFB;//~0x04; // bit 2 - timer disabled(=0); bit 0 - capture mode
}

void adc_resumeADC()
{
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = TMR2_PAGE;
#endif // _SFRPAGE_EXIST_
	TMR2CN |= 0x04; // bit 2 - timer enable; bit 0 - capture mode
	//	TMR2CN = 0x01; // bit 2 - timer disabled(=0); bit 0 - capture mode
}


uint16_t seq;

/**
  * adc0_clear_buffer() - clears up buffer for all A/D channels
  */
void adc0_clear_buffer()
{
	unsigned char i;

	// counter for sample rate divider
	adc_sample_rate_counter_ = 0;

	for ( i = 0; i < adc_number_of_channels_; i++ )
	{
		// By default use all consecutive channels
		adc_channel_mapping_[i] = i;
//		channel_receiver_[i] = NULL; // no receiver assigned (function pointer -> RECEIVER(8,16))
		channel_context_[i] = 0; // no context initially -> dictated by receiver
	}
#ifdef _UMR_MOTE_F1_
	// This is hardcoded for testing
	//AMX0P = 0x13; // P1.1 for ADC
	adc_channel_mapping_[0] = 0x13;
#endif
#ifdef _ENABLE_ROBOTICS_04_
	adc_channel_mapping_[0] = 1;
	adc_channel_mapping_[1] = 0;
	adc_channel_mapping_[2] = 2;
#endif // _ENABLE_ROBOTICS_04_
	seq = 0;
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

	// PASS THE MEASURMENT TO RECEIVING FUNCTION
//	if ( NULL != channel_receiver_[adc_current_channel_])
//	{
//		(channel_receiver_[adc_current_channel_])( channel_context_[adc_current_channel_], value);
//	}
#ifndef _ENABLE_SRC_DUMMY_
	//value = seq++;
	ssn_recv_sample(channel_context_[adc_current_channel_], value);
#endif // NOT _ENABLE_SRC_DUMMY_
	
	adc_current_channel_++;
	if ( adc_current_channel_ >= adc_number_of_channels_ )
	{
		adc_current_channel_ = 0;
	}
	//set the channel to sample next
#ifdef _F310_
	AMX0P = adc_channel_mapping_[adc_current_channel_];
#else
#ifdef _F340_
	AMX0P = adc_channel_mapping_[adc_current_channel_];
#else 
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = ADC0_PAGE;
#endif // _SFRPAGE_EXIST_ 
	AMX0SL = adc_channel_mapping_[adc_current_channel_];  //Sets channel that will be sampled
#endif
#endif

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
	adc_sample_rate_counter_++;
	if ( adc_sample_rate_counter_ >= adc_rate_divider_ )
	{
		//for( i = 0; i < 500; i++ )
		//	{i++;
		//	};

		ADC0CN |= 0x10;  //Start ADC0 interupt flag
		adc_sample_rate_counter_ = 0;
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

