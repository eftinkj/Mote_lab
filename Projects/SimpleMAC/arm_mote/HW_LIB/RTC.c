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
//Interupt for RTC
#include <RTC.h>
//#include "uart_0.h"

#ifndef _ARM_
	#ifdef __KEIL__
	//	sbit P2_4 = P2 ^ 4;
		sbit P2_2 = P2 ^ 2;
	#endif
#endif // not _ARM_

//unsigned int rtc_secs;


unsigned char XDATA rtc_hours = 0;
unsigned char XDATA rtc_minutes = 0;
unsigned char XDATA rtc_seconds = 0;
unsigned char XDATA rtc_tsec = 0;


rtc_tick_t XDATA rtc_ticker = 0;

//char rtc_disp = 0;


uint8_t XDATA rtc_adjust = RTC_ADJUST_RELOAD; // used to adjust time drift due to inacurate timer 4 overflow
uint8_t XDATA rtc_adjust_long = 0; // used to adjust time drift due to inacurate timer 4 overflow
uint8_t XDATA rtc_skip_tick = 0;
uint8_t XDATA rtc_zero_tickers_ = 0;


void rtc_init()
{
	rtc_ticker = 0;
	rtc_adjust = RTC_ADJUST_RELOAD; // used to adjust time drift due to inacurate timer 4 overflow
	rtc_adjust_long = 0; // used to adjust time drift due to inacurate timer 4 overflow
	rtc_skip_tick = 0;
	rtc_zero_tickers_ = 0;
}


#ifndef _ARM_

//**********************
#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void TIMER4_ISR() TASK_FUNC
#else
	// Case of the static interrupt on F120
	#ifdef _F340_
		void RTC_ISR()  interrupt HWM_INT_TIMER3 using HWM_INT_RTC_BANK
	#else
		void RTC_ISR() interrupt HWM_INT_TIMER4 using HWM_INT_RTC_BANK
	#endif // not F340
#endif
{
	// static int counter;
	//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = 2;
#endif // _SFRPAGE_EXIST_
#ifdef _F340_
	TMR3CN &= (0xFF-F340_TF3H);
#else // _F340_
	TF4 = 0;
#endif // else _F340
	//ADC0BUSY = 1;
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = 0;
#endif // _SFRPAGE_EXIST_

	//RTC Update Section
//	rtc_ticker = rtc_ticker + 1;
/*	if (1 == rtc_zero_tickers_)
	{
		rtc_ticker = 0;
		rtc_zero_tickers_ = 0;
	}
*/	if (!rtc_skip_tick)
	{
		rtc_ticker++;
	}
	else
	{
		rtc_skip_tick = 0;
	}
	rtc_adjust++;

//	P2_2 = 1;

	if (0 == (rtc_ticker%1000) )
	{
		P2_2 = FLIP_BIT(P2_2);
		//	P2_2 = 0;
	}
//	else if (1000 == rtc_ticker)
//	{
//		P2_2 = 1-P2_2;
//	}
} //End T4 ISR

#endif // #ifndef _ARM_

//**********************

//Functiont to Set the System Clock
void rtc_time_set ( uint8_t rtc_hours_s, uint8_t rtc_minutes_s, uint8_t rtc_seconds_s, uint16_t rtc_mseconds_s )
{
	uint32_t temp_t = (( uint32_t ) ( TICKERS_PER_SECOND )) * 60 * 60 * rtc_hours_s 
	+ ( uint32_t ) TICKERS_PER_SECOND * 60 * rtc_minutes_s 
	+ ( uint32_t ) TICKERS_PER_SECOND * rtc_seconds_s 
	+ ( uint32_t )rtc_mseconds_s;
#ifdef _ARM_
	uint32_t sys_ticks = halCommonGetInt32uMillisecondTick();
	rtc_ticker = sys_ticks - temp_t;
#else // _ARM_
	EA=0;
	EA=0;
	rtc_ticker = temp_t;
	EA=1;
#endif // else _ARM_
//	rtc_hours = rtc_hours_s;
//	rtc_minutes = rtc_minutes_s;
//	rtc_seconds = rtc_seconds_s;

}

//**********************

/*
//Function to Read the Current Temporal Coordinate
uint32_t rtc_time_read()
{
	unsigned long int xdata temp_lint = 0;
	char *time_vector = &temp_lint;
	time_vector[0] = rtc_hours;
	time_vector[1] = rtc_minutes;
	time_vector[2] = rtc_seconds;
	time_vector[3] = rtc_tsec;
	return temp_lint;

}
*/
//**********************

//Function to Read the Current Temporal Norm
//unsigned long int rtc_get_ticks()
//{
//	return rtc_ticker;
//}

//**********************

//Function ?????

