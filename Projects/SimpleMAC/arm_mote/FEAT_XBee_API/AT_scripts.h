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
#ifndef AT_SCRIPTS_H
#define AT_SCRIPTS_H


//extern unsigned char my_rf_channel_;
//extern unsigned char my_tx_power_;

//#include "feat_enable.h"
#include "defs/esnl_pub.h"

#include "HW_LIB/RTC.h"

extern uint8_t at_tx_timeout_enabled;
extern rtc_tick_t	at_tx_timeout;


void AT_GT_set(void);

void set_tx_timeout ( unsigned short ms );

#ifdef __KEIL__
	#ifdef FEAT_REPLICATOR
		extern void tx_timeout ( void )  TASK_FUNC;
	#else	
		extern void tx_timeout ( void );
	#endif
#endif
#ifdef __SDCC__
	#ifdef _ARM_
		extern void tx_timeout ( void );
	#else // _ARM_
	#if defined (_F340_) || defined (_F310_)
		extern void tx_timeout ( void );
	#else // _F340_
		extern void tx_timeout ( void ) interrupt HWM_INT_TIMER3;
	#endif
	#endif // else _ARM_
#endif

/**
  * void analyse_AT_response(rx_packet) - handles the responses to AT commands
  * 	1) response to Channes Scann
  */
void analyse_AT_response ( char *pkt );


#ifdef FEAT_ENABLE_XBEE_RECOVERY_CONFIG
void fix_XBee_baudrate();




#define CHECK_TIMEOUT(condition, delay, retries) \
		{ \
			unsigned int i = 0; \
			while (condition) \
			{ \
				unsigned char j = 0; while (j++<delay); \
				if (MAX_RETRIES < ++i) { break; } \
			} \
		} \
		// Macro end


#define AT_END() \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'A'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'T'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'C'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'N'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = '\r'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		// End of macro

#define AT_SET_115KBPS()  \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'A'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'T'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'B'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'D'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = '7'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = '\r'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		// End of macro

#define AT_SET_API()  \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'A'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'T'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'A'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'P'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = '1'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = '\r'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		// End of macro


#define AT_WRITE_API()  \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'A'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'T'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'W'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = 'R'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		i = 0; \
		SBUF0 = '\r'; CHECK_TIMEOUT((TI0 == 0), 100, MAX_RETRIES ); \
		TI0 = 0; \
		// End of macro


#define NO_ERROR	0
#define NO_RESPONSE 1
#define NO_OK		2

#ifdef _SFRPAGE_EXIST_

#define AT_CHECK_OK(result) \
		{ \
			result = NO_ERROR; \
			SFRPAGE = 0; \
			RI0 = 0; SSTA0 = 0;\
			CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES);  \
			if (RI0 == 0) { result = NO_RESPONSE;}  \
			else if ('O' != SBUF0)  { result = NO_OK;}  \
			SFRPAGE = 0; \
			RI0 = 0; \
			if (NO_ERROR == result) \
			{ \
				SFRPAGE = 0; \
				CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
				if (RI0 == 0) { result = NO_RESPONSE;} \
				else if ('K' != SBUF0)  { result = NO_OK;} \
				SFRPAGE = 0; \
				RI0 = 0; \
				if (NO_ERROR == result) \
				{ \
					CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
					if (RI0 == 0) { result = NO_RESPONSE;} \
					else if (0x0D != SBUF0)  { result = NO_OK;} \
					SFRPAGE = 0; \
					RI0 = 0; \
				} \
			} \
		} \
		// Macro end


#else // _SFRPAGE_EXIST_
#define AT_CHECK_OK(result) \
		{ \
			result = NO_ERROR; \
			RI0 = 0;\
			CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES);  \
			if (RI0 == 0) { result = NO_RESPONSE;}  \
			else if ('O' != SBUF0)  { result = NO_OK;}  \
			RI0 = 0; \
			if (NO_ERROR == result) \
			{ \
				CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
				if (RI0 == 0) { result = NO_RESPONSE;} \
				else if ('K' != SBUF0)  { result = NO_OK;} \
				RI0 = 0; \
				if (NO_ERROR == result) \
				{ \
					CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
					if (RI0 == 0) { result = NO_RESPONSE;} \
					else if (0x0D != SBUF0)  { result = NO_OK;} \
					RI0 = 0; \
				} \
			} \
		} \
		// Macro end
#endif // _SFRPAGE_EXIST_




#ifdef _SFRPAGE_EXIST_
#define CHECK_HW_RESET(result) \
		{ \
			result = NO_ERROR; \
			if (0x7e != SBUF0)  { result = NO_OK;}  \
			SFRPAGE = 0; \
			RI0 = 0; \
			if (NO_ERROR == result) \
			{ \
				SFRPAGE = 0; \
				CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
				if (RI0 == 0) { result = NO_RESPONSE;} \
				else if (0x00 != SBUF0)  { result = NO_OK;} \
				SFRPAGE = 0; \
				RI0 = 0; \
				if (NO_ERROR == result) \
				{ \
					CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
					if (RI0 == 0) { result = NO_RESPONSE;} \
					else if (0x02 != SBUF0)  { result = NO_OK;} \
					SFRPAGE = 0; \
					RI0 = 0; \
					if (NO_ERROR == result) \
					{ \
						SFRPAGE = 0; \
						CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
						if (RI0 == 0) { result = NO_RESPONSE;} \
						else if (0x8a != SBUF0)  { result = NO_OK;} \
						SFRPAGE = 0; \
						RI0 = 0; \
						if (NO_ERROR == result) \
						{ \
							CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
							if (RI0 == 0) { result = NO_RESPONSE;} \
							else if (0x00 != SBUF0)  { result = NO_OK;} \
							SFRPAGE = 0; \
							RI0 = 0; \
							if (NO_ERROR == result) \
							{ \
								CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
								if (RI0 == 0) { result = NO_RESPONSE;} \
								else if (0x75 != SBUF0)  { result = NO_OK;} \
								SFRPAGE = 0; \
								RI0 = 0; \
							} \
						} \
					} \
				} \
			} \
		} \
		// Macro end


#else // _SFRPAGE_EXIST_

#define CHECK_HW_RESET(result) \
		{ \
			result = NO_ERROR; \
			if (0x7e != SBUF0)  { result = NO_OK;}  \
			RI0 = 0; \
			if (NO_ERROR == result) \
			{ \
				CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
				if (RI0 == 0) { result = NO_RESPONSE;} \
				else if (0x00 != SBUF0)  { result = NO_OK;} \
				RI0 = 0; \
				if (NO_ERROR == result) \
				{ \
					CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
					if (RI0 == 0) { result = NO_RESPONSE;} \
					else if (0x02 != SBUF0)  { result = NO_OK;} \
					RI0 = 0; \
					if (NO_ERROR == result) \
					{ \
						CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
						if (RI0 == 0) { result = NO_RESPONSE;} \
						else if (0x8a != SBUF0)  { result = NO_OK;} \
						RI0 = 0; \
						if (NO_ERROR == result) \
						{ \
							CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
							if (RI0 == 0) { result = NO_RESPONSE;} \
							else if (0x00 != SBUF0)  { result = NO_OK;} \
							RI0 = 0; \
							if (NO_ERROR == result) \
							{ \
								CHECK_TIMEOUT(RI0==0, 100, MAX_RETRIES); \
								if (RI0 == 0) { result = NO_RESPONSE;} \
								else if (0x75 != SBUF0)  { result = NO_OK;} \
								RI0 = 0; \
							} \
						} \
					} \
				} \
			} \
		} \
		// Macro end
#endif // else _SFRPAGE_EXIST_


#endif // FEAT_ENABLE_XBEE_RECOVERY_CONFIG



#define check_at_scripts_tx_timeout() \
{\
	if (at_tx_timeout_enabled) \
	{\
		if (at_tx_timeout < rtc_get_ticks())\
		{\
			at_tx_timeout = RTC_OVERFLOW_TIMER_VALUE;\
			tx_timeout();\
		}\
	}\
}


#endif
