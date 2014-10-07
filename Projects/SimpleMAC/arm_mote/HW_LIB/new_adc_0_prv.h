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
#ifndef NEW_ADC_0_PRV_H
#define NEW_ADC_0_PRV_H


#include "defs/esnl_pub.h"
#include "new_adc_0_pub.h"


#include "common.h"


#ifdef _F120_
	#define ADC_BIT_RESOLUTION()	12
#endif
#ifdef _F340_
	#define ADC_BIT_RESOLUTION()	10
#endif


#define MAX_ADCS 4

//#define DEF_DIVIDER 10
// 50 -> 400Hz sampling
// 100 -> 200Hz sampling
#define DEF_DIVIDER 100

#define NO_CHANNEL 0xFF

#ifdef _ENABLE_NEW_ADC_MODULE_



// ##############################################
// ## Common configuration for all ADC channels
//

// ##############################################
// ## ADC channel specific configuration
// ##############################################
//
// What is the PIN address for ADC chanels
extern uint8_t xdata adc_channel_mapping_[MAX_ADCS];
//extern p_func_t xdata channel_receiver_[MAX_ADCS];
extern uint8_t xdata channel_context_[MAX_ADCS];

extern uint8_t xdata adc_rate_divider_;
extern int xdata adc_sample_rate_counter_;

extern uint8_t xdata adc_number_of_channels_;
extern uint8_t xdata adc_current_channel_;




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
    void ADC0_ISR() interrupt HWM_INT_ADC0_EOC;
#endif



#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void TIMER2_ISR() TASK_FUNC;
#else
	#ifndef __KEIL__
		// Case of the static interrupt on F120
		void TIMER2_ISR() interrupt HWM_INT_TIMER2;
	#endif // __KEIL__
#endif // FEAT_REPLICATOR


#endif // _ENABLE_NEW_ADC_MODULE_

#endif // NEW_ADC_0_PRV_H
