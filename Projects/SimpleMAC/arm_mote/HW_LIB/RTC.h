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
#ifndef _RTC_H_
#define _RTC_H_


#include "defs/esnl_pub.h"

//Header File for the RTC

//External Variables
//extern unsigned int rtc_secs;

// Temporal coordinates
extern unsigned char rtc_hours;
extern unsigned char rtc_minutes;
extern unsigned char rtc_seconds;
extern unsigned char rtc_tsec;
//extern char rtc_disp;

#define TICKERS_PER_SECOND 1000
// 1ms resolution -> 0xFFFC reload (??)
// 50ms resolution -> 0xFF34 reload (!!) stable and tested
//#define RTC_MAX_TIMER_VALUE (24*60*60*TICKERS_PER_SECOND-1)
#define RTC_MAX_TIMER_VALUE (86400*TICKERS_PER_SECOND-1)
#define RTC_OVERFLOW_TIMER_VALUE (86400*TICKERS_PER_SECOND)


typedef	uint32_t	rtc_tick_t;

extern rtc_tick_t rtc_ticker;
extern uint8_t rtc_adjust; // used to adjust time drift due to inacurate timer 4 overflow
extern uint8_t rtc_adjust_long; // used to adjust time drift due to inacurate timer 4 overflow
extern uint8_t rtc_skip_tick;
extern uint8_t rtc_zero_tickers_; // resets ticker after 24h overflow

//Function Prototypes
//void rtc_time_set ( unsigned char hours_s, unsigned char minutes_s, unsigned char seconds_s );
void rtc_time_set ( uint8_t rtc_hours_s, uint8_t rtc_minutes_s, uint8_t rtc_seconds_s, uint16_t rtc_mseconds_s );
//uint32_t rtc_time_read();

//Function to Read the Current Temporal Norm
//unsigned long int rtc_get_ticks();

#ifdef _ARM_
	#define rtc_get_ticks() (halCommonGetInt32uMillisecondTick())
	#define rtc_get_rticks() (halCommonGetInt32uMillisecondTick()-rtc_ticker)
#else // _ARM_
	#define rtc_get_ticks() rtc_ticker
	#define rtc_get_rticks() rtc_ticker
#endif // else _ARM_

#ifdef _F120_
	// Adjust every 100 tick (1% slowdown)
	#define RTC_ADJUST_RELOAD	0
	// Skip tick in interrupt every so often
	#define RTC_ADJUSTMENT_OVERFLOW 41
	// Every so often skip one more tick
	#define RTC_ADJUSTMENT_LONG 57
	// 57 - 0.06 s over 420 seconds
	// 60 - 0.038 s over 1886
	// 61 - 0.020 s over 800 (0.4 over 1400)
	// 63 - 0.040 s over 872
	// 53 - (-0.02) s over 575 s
	///////////////////////////////
	// 55 - (-0.02) over 750 s
	// 57 -  >0.001 over 900 s 	!! GOOD !!  (=0.0002)

	//#define RTC_ADJUSTMENT_LONG 23
#else	// _F120_
	// Assume F340
	#define RTC_ADJUST_RELOAD	0
	// Skip tick in interrupt every so often
	#define RTC_ADJUSTMENT_OVERFLOW 41
	// Every so often skip one more tick
	#define RTC_ADJUSTMENT_LONG 57
#endif // else _F120_

#define rtc_make_adjustment() { if (RTC_ADJUSTMENT_OVERFLOW < rtc_adjust) \
	{rtc_skip_tick=1; rtc_adjust= RTC_ADJUST_RELOAD; rtc_adjust_long++; \
		if (RTC_ADJUSTMENT_LONG < rtc_adjust_long) \
	{ rtc_skip_tick = 0; rtc_adjust_long = 0;} \
	} \
	if (RTC_MAX_TIMER_VALUE < rtc_get_ticks()) \
	{ rtc_zero_tickers_ = 1; } \
	}


void rtc_init(void);

#ifdef _ARM_

#else // _ARM_

#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void TIMER4_ISR() TASK_FUNC;
#else
	#ifndef __KEIL__
		// Case of the static interrupt on F120
		//void RTC_ISR() interrupt HWM_INT_TIMER4;
		// Case of the static interrupt on F120
		#ifdef _ARM_
			void RTC_ISR(void);// interrupt HWM_INT_TIMER3;
		#else // _ARM
			#ifdef _F340_
			void RTC_ISR() interrupt HWM_INT_TIMER3;
			#else
			void RTC_ISR() interrupt HWM_INT_TIMER4;
			#endif // not F340
		#endif // else _ARM_
	#endif // __KEIL__
#endif //_FEAT_REPLICATOR

//#endif // _TEMP_MZ_
#endif // else _ARM_


#endif // _RTC_H_

