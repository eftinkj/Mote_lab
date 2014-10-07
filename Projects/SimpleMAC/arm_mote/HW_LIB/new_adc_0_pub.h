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
#ifndef NEW_ADC_0_PUB_H
#define NEW_ADC_0_PUB_H


#include "defs/esnl_pub.h"

#include "common.h"


#ifdef _ENABLE_NEW_ADC_MODULE_

//typedef void (code *p_func_t)(uint8_t, uint16_t);

#ifdef __SDCC__
typedef code void ( * xdata p_func_t)(uint8_t, uint16_t) TASK_FUNC;
#else
typedef void ( * code p_func_t)(uint8_t, uint16_t) TASK_FUNC;
#endif // not _SDCC_


// ##############################################
// ## Common configuration for all ADC channels
//

/**
  * adc0_setup() - sets up buffer for all A/D channels
  */
void adc0_setup();


/**
 * adc0_attach(channel_mapping, channel_context, receiver) - activates and connects 
 *    an A/D channel to data processsing module
 */
uint8_t adc0_attach(uint8_t channel_mapping, uint8_t channel_context, p_func_t receiver);


/**
 * adc_set_rate_divider_(divider) - sets the sampling rate divider (NOMINAL/DIVIDER)
 */
void adc_set_rate_divider(uint8_t divider);


/**
 * adc_suspendADC() - Stops the ADC timouts
 */
void adc_suspendADC();
/**
 * adc_resumeADC() - Starts thr ADC timeout (reading)
 */
void adc_resumeADC();

#endif // _ENABLE_NEW_ADC_MODULE_

#endif // NEW_ADC_0_PUB_H
