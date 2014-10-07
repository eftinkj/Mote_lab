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
#include "defs/esnl_pub.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "common.h"
#include "packet.h"
#include "AT_scripts.h"
#include "FEAT_Routing/routing.h"
#include "FEAT_Routing/routing_OEDSR.h"
#include "HW_LIB/uart_0.h"
#include "API_frames.h"
#include "FEAT_Networking/Phy_layer.h"
//#include "HW_LIB/RTC.h"

#include "sys_func.h"


#ifdef __SDCC__
//	#define RLED P2_4
//	#define GLED P2_5
// due to F1	#define YLED P2_4
	#define YLED P2_6
#endif

#ifdef __KEIL__
//	sbit RLED = P2 ^ 6;
//	sbit GLED = P2 ^ 5;
	sbit YLED = P2 ^ 4;

#endif




///////////////////////////////////////////////
///////////////////////////////////////////////
uint8_t at_tx_timeout_enabled = 0;
rtc_tick_t	at_tx_timeout = RTC_OVERFLOW_TIMER_VALUE;

#define AT_CMD_DELAY_TIME 1000
void at_cmd_delay()
{
	uint16_t i = 0;
	while ( i++ < AT_CMD_DELAY_TIME );
}

///////////////////////////////////////////////
void AT_GT_set()
{

//	int result;
	unsigned int uint_value;
//	float f = 0.222;


	uint_value = MY_ADDR;
#ifdef _ENABLE_XBEE_API_
	api_send_at ( "MY", ( char* ) ( &uint_value ), 2 );
	at_cmd_delay();

	uint_value = 0xffff;
	api_send_at ( "DL", ( char* ) ( &uint_value ), 2 );
	at_cmd_delay();

	// Set TX power level
	phy_set_power_level ( my_tx_power_ );

	// Set RF channel to be used
	phy_set_RF_channel ( my_rf_channel_ );


	api_send_at ( "RR", ( char* ) &my_RF_retries_, 1 );
	at_cmd_delay();

	uint_value = 0x0;
	api_send_at ( "ED", ( char* ) &uint_value, 1 );
	at_cmd_delay();

	phy_send_scan_request ( my_rf_channel_ );
	at_cmd_delay();

	packet[0] = 'P';
	packet[1] = 'R';
	packet[2] = 'O';
	packet[3] = 'B';
	packet[4] = 'A';
	packet[5] = MY_ADDR;
	packet[6] = '\r';

	api_send_packet16 ( packet , 17, 0xFFFF );
	at_cmd_delay();
#endif // _ENABLE_XBEE_API_

}







// timer 3 interupt - to handle timeouts for TX
#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void tx_timeout() TASK_FUNC
#else
	// Case of the static interrupt on F120
	#ifdef _F340_
	void tx_timeout()	
	#else
//	void tx_timeout() interrupt HWM_INT_TIMER3
	void tx_timeout()
	#endif
#endif
{
	// Make exceptions/additional operations depending on the NEXT TX state
#ifdef DEBUG_GEN
	request_debug_packet_ = 1;
#endif

#ifdef DEBUG_ROUTE
#endif

#ifndef _TEMP_ARM_
	#ifndef _F340_
		SFRPAGE   = TMR3_PAGE;
		//	TMR3CN = 0x05; // bit 2 - timer enable; bit 0 - capture mode
		TMR3CN = 0x01; // bit 2 - timer disabled(=0); bit 0 - capture mode
	#endif // _F340_ 
#endif // _TEMP_ARM_
}




/**
  * void analyse_AT_response(rx_packet) - handles the responses to AT commands
  * 	1) response to Channes Scann
  */
void analyse_AT_response ( char *pkt )
{
#ifdef _ENABLE_XBEE_API_
	api_frame_at_rsp_t* at_rsp = ( api_frame_at_rsp_t* ) pkt;

	// Received Channel Scan report
	if ( ( 'E' == at_rsp->api_at_cmd_1 ) && ( 'D' == at_rsp->api_at_cmd_2 ) )
	{
		phy_analyse_scan_response ( at_rsp );
	}
#endif // _ENABLE_XBEE_API_
}


#ifdef FEAT_ENABLE_XBEE_RECOVERY_CONFIG


#define MAX_RATE 5

/*
49MHz clock
0x60 - 9600 (9570)
0xB0 - 19200 (19140)
0xD8 - 38400 (38281)
0xE5 - 57600 (56712)
0xF2 - 115200 (109375)

98MHz clock
0xB0 - 9600 (sys/4)
0x60 - 19200 (sys)
0xB0 - 38400 (sys)
0xCB - 57600 (sys)
0xE4 - 115200 (109375)
*/
const unsigned char uart_rate_TH1[MAX_RATE] = { 0x60, 0xB0, 0xD8, 0xE5, 0xF2 }
//const unsigned char uart_rate_TH1[MAX_RATE] = { 0xB0, 0x60, 0xB0, 0xCB, 0xE4 }

		; //0xF2;


void uart_switch_rate ( unsigned char rate )
{
	if ( rate >= MAX_RATE )
	{
		rate = MAX_RATE - 1;
	}
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = TIMER01_PAGE;
#endif // _SFRPAGE_EXIST_
//    TCON      = uart_rate_tcon[rate]; //0x40;
//    TMOD      = uart_rate_tmod[rate]; //0x20;
//    CKCON     = uart_rate_ckcon[rate]; //0x10;
	TH1       = uart_rate_TH1[rate]; //0xF2;
}

#define MAX_RETRIES 50000

void fix_XBee_baudrate()
{
	unsigned int i = 0;
	unsigned char j = 0;
	unsigned char at_error = 0;
	unsigned char rate = 0;
	// check if XBee works (e.g. AT command with simple response)
	// disable interrupts
	EA = 0;
	EA = 0;

//	while ((RI0 == 0) && (i++<15000));
//	{ 	unsigned int i = 0, j=0; while (i++<15000) {j=0;while(j++<1);} }
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	CHECK_TIMEOUT ( RI0 == 0, 100, 100 );
	// Check if RESET frame received in API mode (possibly)
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	if ( 1 == RI0 )
	{
		CHECK_HW_RESET ( at_error );
		if ( 0 == at_error )
		{
#ifdef _SFRPAGE_EXIST_
			SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
			RI0 = 0;
			EA = 1;
			return;
		}
	}
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	RI0 = 0;

	for ( rate = MAX_RATE + 1; rate > 0 ; )
	{
		rate--;
		at_error = 0; // reset befor each try

		uart_switch_rate ( rate );

		// send AT command
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
		TI0 = 0;
		SBUF0 = '+';
		i = 0;
		while ( ( TI0 == 0 ) && ( i++ < 1000 ) );
		if ( TI0 == 0 )
		{
			sys_error ( SYS_ERROR_XBEE_INIT );
			sys_reset();
		}

		TI0 = 0;
		SBUF0 = '+';
		i = 0;
		while ( ( TI0 == 0 ) && ( i++ < 1000 ) );
		if ( TI0 == 0 )
		{
			sys_error ( SYS_ERROR_XBEE_INIT );
			sys_reset();
		}

		TI0 = 0;
		SBUF0 = '+';
		i = 0;
		while ( ( TI0 == 0 ) && ( i++ < 1000 ) );
	if ( TI0 == 0 ) { sys_error ( SYS_ERROR_XBEE_INIT ); sys_reset(); }
		TI0 = 0;

		RI0 = 0;
		// check response

		AT_CHECK_OK ( at_error );
		if ( 0 == at_error )
		{
			break; // succesfully found good baudrate
		}
		if ( 2 == at_error )
		{
			unsigned char temp1, temp2, temp3, temp4;
#ifdef _SFRPAGE_EXIST_
			SFRPAGE = 0;
#endif // _SFRPAGE_EXIST_
			RI0 = 0; 
#ifndef _F340_
			SSTA0 = 0;
#endif // not _F340_
			CHECK_TIMEOUT ( RI0 == 0, 100, 100 );
			temp1 = SBUF0;

#ifdef _SFRPAGE_EXIST_
			SFRPAGE = 0;
#endif // _SFRPAGE_EXIST_
			RI0 = 0; 
#ifndef _F340_
			SSTA0 = 0;
#endif // _F340_
			CHECK_TIMEOUT ( RI0 == 0, 100, 100 );
			temp2 = SBUF0;

#ifdef _SFRPAGE_EXIST_
			SFRPAGE = 0;
#endif // _SFRPAGE_EXIST_
			RI0 = 0; 
#ifndef _F340_
			SSTA0 = 0;
#endif // not _F340_
			CHECK_TIMEOUT ( RI0 == 0, 100, 100 );
			temp3 = SBUF0;

#ifdef _SFRPAGE_EXIST_
			SFRPAGE = 0;
#endif // _SFRPAGE_EXIST_
			RI0 = 0;
#ifndef _F340_
			SSTA0 = 0;
#endif // _F340_
			CHECK_TIMEOUT ( RI0 == 0, 100, 100 );
			temp4 = SBUF0;
		}
		sys_error ( SYS_ERROR_XBEE_RX ); // Indicate failure of current baudrate
	}

	if ( 1 == at_error )
	{
		sys_error ( SYS_ERROR_XBEE_INIT_TOTAL );
	}
	else
	{
		AT_SET_115KBPS();
		AT_CHECK_OK ( at_error );
		AT_SET_API();
		AT_CHECK_OK ( at_error );
		AT_WRITE_API();
		AT_CHECK_OK ( at_error );
	}

	AT_END();
	AT_CHECK_OK ( at_error );

	// Search for correct speed ('+++' -> 'OK')

	// set the new values ( baudrate, API)

	// store the values in XBee (WR)

	// ???

	// reset the UART settings
#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = TIMER01_PAGE;
#endif // _SFRPAGE_EXIST_
	TCON      = 0x40;
	TMOD      = 0x20;
	CKCON     = 0x10;
	TH1       = 0xF2;

#ifdef _SFRPAGE_EXIST_
	SFRPAGE   = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	SCON0     = 0x50;
	TI0 = 0;
	RI0 = 0;
#ifndef _F340_
	SSTA0 = 0;
#endif // not _F340_
	// enable interrupts
	EA = 1;
}

#endif // FEAT_ENABLE_XBEE_RECOVERY_CONFIG
