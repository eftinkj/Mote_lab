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
#ifndef ADC0_H
#define ADC0_H


#include "defs/esnl_pub.h"



#include "common.h"


#ifdef _F120_
	#define ADC_BIT_RESOLUTION()	12
#endif
#ifdef _F340_
	#define ADC_BIT_RESOLUTION()	10
#endif


#define MAX_ADCS 4

//#define DEF_DIVIDER 100
//#define DEF_N_AVERAGING 8

#define DEF_DIVIDER 10
#define DEF_N_AVERAGING 10


#define ADC0_BUFFER_SIZE 100


#define ADC_MAX_VALUE 255

//extern int adc0_head;
//extern int adc0_tail;
//extern unsigned int adc0_buffer[ADC0_BUFFER_SIZE];
//extern int adc0_channel;
//#define ADC0_CHANNEL 0

#define NO_CHANNEL 0xFF


#ifdef _ENABLE_ADC_MODULE_


#ifdef ADC_ENABLE_CYCLE_COUNT
	extern uint8_t adc_cycle_count; // count cycles
//	extern rtc_t adc_cycle_timestamp;
	extern uint8_t climbing;
	extern uint16_t adc_cycle_peak;
	extern uint16_t adc_cycle_min;
	extern uint16_t adc_cycle_peak_max;

	#define ADC_CYCLE_MIN_DECREASE	600
	#define ADC_CYCLE_MIN_INCREASE	600
	#define ADC_CYCLE_SENDING_TRIGGER_LEVEL 10

	void adc_cycle_loop();
#endif // ADC_ENABLE_CYCLE_COUNT

// ##############################################
// ## Common configuration for all ADC channels
//
extern uint8_t XDATA adc_sample_rate_divider_;
// Averaging samples over N readings
extern uint8_t XDATA adc_sample_averaging_; // 1 -> no averaging
extern unsigned char XDATA number_of_channels_;


/**
  * adc0_need_to_send_packet() - returns id of channel that has data to be sent, or NO_CHANNEL if there is nothing to sent
  */
unsigned char adc0_need_to_send_packet(void);

/**
  * adc0_setup() - sets up buffer for all A/D channels
  */
void adc0_setup(void);


void set_number_of_channels ( unsigned char num );


/**
  * adc0_not_empty(channel) - checks the size of waiting samples (in buffer)
  * returns number of ready samples (0-MAX_BUF), or '-1' if channel number was incorrect
  */
int adc0_not_empty ( unsigned char channel );

/**
  * int adc0_get_value(channel) - returns value of the first sample in the buffer (longest waiting)
  * returns sample value (0-256)8-bit or (0-1024)10-bit; or '-1' if incorrect channel number
  */
unsigned int adc0_get_value ( unsigned char channel );

/**
  * int adc0_get_value_threshed(channel) - returns value of '1' if the last sample in the buffer (shortest waiting)
  * is greater than the specified threshold, '0' else; or '-1' if incorrect channel number
  */
unsigned int adc0_get_value_threshed ( uint8_t channel, unsigned int threshold );

/**
  * uint16_t adc0_get_average ( channel, count );- returns average value 
  *		for "count" first samples of "channel" 
  * returns 0xFFFF when failed, or the calculated value otherwise
  */
uint16_t adc0_get_average ( uint8_t channel, uint8_t count );

/**
  * uint16_t adc0_get_max ( channel, count );- returns average value 
  *		for "count" first samples of "channel" 
  * returns 0xFFFF when failed, or the calculated value otherwise
  */
uint16_t adc0_get_max ( uint8_t channel, uint8_t count );

/**
  * bit adc0_skip_samples(channel, skip_n) - skips "skip_n" samples in the buffer for "channel"
  * returns "1" if successful or "0" if there was not enough samples to skip
  */
uint8_t adc0_skip_samples ( unsigned char channel, unsigned char skip_n );

//Called when Conversion Finishes


#ifdef __KEIL__
	#ifdef FEAT_REPLICATOR
		// Case of custom interrupt handler
		void ADC0_ISR() TASK_FUNC;
	#else
		// Case of the static interrupt on F120
		//void ADC0_ISR() interrupt HWM_INT_ADC0_EOC;
	#endif
#endif

#ifdef __SDCC__
	#ifdef _ARM_
    	void ADC0_ISR(void); // interrupt HWM_INT_ADC0_EOC;
	#else // _ARM_
    	void ADC0_ISR() interrupt HWM_INT_ADC0_EOC;
	#endif // _ARM_
#endif



#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void TIMER2_ISR() TASK_FUNC;
#else
	#ifndef __KEIL__
		// Case of the static interrupt on F120
		#ifdef _ARM_
			void TIMER2_ISR(void); // interrupt HWM_INT_TIMER2;
		#else // _ARM_
			void TIMER2_ISR() interrupt HWM_INT_TIMER2;
		#endif // else _ARM_
	#endif // __KEIL__
#endif // FEAT_REPLICATOR


#endif // _ENABLE_ADC_MODULE_

#endif
