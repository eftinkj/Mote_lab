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
//SPI handler

//#include <string.h>
//#include <stdio.h>
//#include <math.h>

#include "defs/esnl_pub.h"

#ifdef _ENABLE_SPI_MODULE_

#include "spi_pub.h"
#include "spi_prv.h"

#include "sys_func.h"

//#include "common.h"
//#include "API_frames.h"
//#include "Queuing.h"

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/
spi_buf_len_t SPI_IDX_MEM spi_curr_idx;
spi_buf_len_t SPI_IDX_MEM spi_tx_idx; // last byte to TX
spi_buf_len_t SPI_IDX_MEM spi_rx_idx; // first byte from RX (to be read by app)
spi_buf_t SPI_BUF_MEM spi_buffer[SPI_BUFFER_SIZE];

spi_state_t SPI_IDX_MEM spi_state;

bit spi_tx_tick_ = 0; // Watchdog for SPI interrupt

// Mutex for accessing
spi_app_t SPI_IDX_MEM spi_app_mutex;

spi_event_t *spi_event_flag;
spi_event_t spi_event_flag_mask;
rtc_tick_t spi_rtc_timeout;


spi_buf_t spi_preamble_char;
spi_event_t spi_old_event_mask;
uint8_t spi_max_retries;

uint8_t spi_count_intr_;

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/

/**
* spi_purge_task( app ) - stops current SPI task (operation)
*	RETURN: 1 (SPI_R_OK) - if request has been fulfilled (SPI is freed by app)
*			0 (SPI_R_FAIL) - if unsucceflul (e.g. used by another app or free)
*/
void spi_purge_task(spi_app_t app)
{
	spi_clear_buffer(app);
}


/**
  * spi_jump_start () - restarts the SPI transmission (Interrupt)
  */
void spi_jump_start ( void )
{
	HW_STOP_INTR();
	// If SPI is idle and it is safe to write to buffer than do it
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
	if ( ( 1 == TXBMT ) && ( 0 == ( SPI0CFG & SPIBSY_MASK ) ) )
	{
#ifdef _SPI_REVERSE_LOGIC_
//		unsigned char temp_spi_tx1 = 0xFF;
//		temp_spi_tx1 -= spi_buffer[spi_curr_idx & SPI_BUFFER_MASK];
//		SPI0DAT = temp_spi_tx1;
		SPI0DAT = 0xFF - spi_buffer[spi_curr_idx & SPI_BUFFER_MASK];
#else // _SPI_REVERSE_LOGIC_
		SPI0DAT = spi_buffer[spi_curr_idx & SPI_BUFFER_MASK];
#endif // _SPI_REVERSE_LOGIC_
	}
	else
	{
		// the transmission is ongoing (busy or something in TX buffer)
		// do not do enything
		// should not happen!!!!! unless streaming mode!!!
	}
	HW_START_INTR();
}

/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

/**
  * spi_init() - sets up SPI controller
  */
void spi_init_sw ( void )
{
	uint8_t i;
	spi_state = SPI_STATE_IDLE;
	spi_curr_idx = 0;
	spi_tx_idx = 0; // last byte to TX
	spi_rx_idx = 0; // first byte from RX (to be read by app)
	for ( i = 0; i < SPI_BUFFER_SIZE; i++ )
	{
		spi_buffer[i] = SPI_CLEAR_BUFFER_VALUE;
	}
	spi_app_mutex = SPI_APP_NONE;

	spi_preamble_char = SPI_PREAMBLE_OFF;
	spi_old_event_mask = SPI_EVENT_NONE;
	spi_max_retries = 0;


	spi_count_intr_ = 0;

#ifdef _SFRPAGE_EXIST_
	SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
	SPIF = 0;
	RXOVRN = 0;	//Clear Overrun flag
	MODF = 0;	//Clear Master-status-lost flag
	WCOL = 0;	//Clear Collision flag
#ifdef _F340_
//	IE &= (0xFF-0x40); // turn off the SPI0 interrupt (if no application claims it)
//	IE |= (0x40); // Enabel the SPI0 interrupt (but SPI0 should still be disabled)
#else
	#ifdef _F120_
		EIE1 |= 0x01;
	#endif // _F120_
#endif // _F340_
}


/**
  * spi_setup(baudrate) - sets up SPI speed
  */
uint8_t spi_setup ( uint16_t baudrate )
{
	// TODO: make dynamic setup
	// for now it is fixed for Chipcon2420 and F340
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
//	SPI0CFG   = 0x40;
//	SPI0CN    = 0x01;
//	SPI0CKR   = 0xEF;
	return SPI_R_OK;
}



/**
  * spi_send_char ( letter ) - sends a character via SPI
  */
//void spi_send_char ( char letter );




/**
  * spi_rx_read_byte(offset) - reads a byte at a given offset from the befining of read block (buffer tail)
  * returns value of byte at "offset" bytes from the begining of valid RX buffer for "port" UART
  * WARNING: assumes that there is "offset" bytes read!!!!!
  */
//unsigned char spi_rx_read_byte ( spi_buf_len_t offset );



/**
  * spi_check_buffer_indexes() - checks if the indexes re correct
  */
//void spi_check_buffer_indexes();


// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################
// #####################################################################







// ############################################################################
// ############################################################################
/**
* spi_request_dev( app, timeout, *event_flag, event_flag_mask) - handles
*		SPI reservation by an "app" for maximum of "timeout" msecs
*		events from SPI will be written to "event_flag" memory
*		events can be masked by "event_flag_mask"
*	RETURN: 1 (SPI_R_OK) - if request has been fulfilled (SPI is reserved)
*			0 (SPI_R_FAIL) - if unsucceflul (e.g. already in use)
*/
uint8_t spi_request_dev (
	spi_app_t app
	, spi_timeout_t timeout
	, spi_event_t *event_flag
	, spi_event_t event_flag_mask
)
{
	if ( SPI_APP_NONE == spi_app_mutex )
	{
		// device is free => can use it
		spi_app_mutex = app;
		spi_event_flag = event_flag;
		spi_event_flag_mask = event_flag_mask;
		spi_rtc_timeout = rtc_get_ticks() + timeout;

		spi_preamble_char = SPI_PREAMBLE_OFF;
		spi_old_event_mask = SPI_EVENT_NONE;
		spi_max_retries = 0;

		HW_STOP_INTR();
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
//		SPIEN = 1; // Enable SPI0

#ifdef _F340_
	//	IE &= (0xFF-0x40); // turn off the SPI0 interrupt (if no application claims it)
	//	IE |= (0x40); // Enabel the SPI0 interrupt (but SPI0 should still be disabled)
	ESPI0 = 1;
#else
	#ifdef _F120_
		EIE1 |= 0x01;
	#endif // _F120_
#endif // _F340_

//		spi_setup(1000);
		HW_START_INTR();

		return SPI_R_OK;
	}
	else
	{
		return SPI_R_FAIL;
	}
}


/**
* spi_release_dev( app ) - releases SPI reservation by an "app"
*	RETURN: 1 (SPI_R_OK) - if request has been fulfilled (SPI is freed by app)
*			0 (SPI_R_FAIL) - if unsucceflul (e.g. used by another app or free)
*/
uint8_t spi_release_dev (
	spi_app_t app
)
{
	if ( app == spi_app_mutex )
	{
		HW_STOP_INTR();
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
//		SPIEN = 0; // Disable SPI0
		HW_START_INTR();

		if ( SPI_STATE_IDLE != spi_state )
		{
			spi_purge_task(app);
		}
		// inform the app about succesfull release
		( *spi_event_flag ) |= ( spi_event_flag_mask & SPI_EVENT_RELEASED );
		// device is free => can use it
		spi_app_mutex = SPI_APP_NONE;
		spi_event_flag = NULL;
		spi_event_flag_mask = 0x00; // No events to report
		spi_rtc_timeout = 0;
		spi_state = SPI_STATE_IDLE;

		return SPI_R_OK;
	}
	else
	{
		return SPI_R_FAIL;
	}
}


/**
* spi_force_release_dev( ) - forces releases of SPI due to timeout
*		setes the application's event flag to PURGED state
*/
void spi_force_release_dev()
{
	if ( SPI_APP_NONE != spi_app_mutex )
	{
		// inform the app about succesfull release
		( *spi_event_flag ) |= ( spi_event_flag_mask & SPI_EVENT_RELEASED );
		( *spi_event_flag ) |= ( spi_event_flag_mask & SPI_EVENT_PURGED );
		spi_release_dev ( spi_app_mutex );
	}
}

/**
* spi_check_timeout () - functions executed within a main loop
*		to periodically check if timeout expired for current application
*		NOTE: should be moved to "TASK SCHEDULER" as time trigered event
*/
void spi_check_timeout()
{
	if ( SPI_APP_NONE != spi_app_mutex )
	{
		if ( rtc_get_ticks() > spi_rtc_timeout )
		{
			spi_force_release_dev();
		}
	}
}





// ############################################################################
// ############################################################################


/**
  * spi_clear_buffer(app) - empties whole RX buffer
  */
void spi_clear_buffer(spi_app_t app)
{
	if (app != spi_app_mutex)
	{
		return;
	}
	// set rx and curr index to the tx index to indicate EMPTY buffer
	spi_rx_idx = spi_curr_idx = spi_tx_idx;
}



/**
  * spi_send_block ( app, message, msg_size )- sends a message via SPI
  */
spi_buf_len_t spi_request_tx ( spi_app_t app, spi_buf_t *message, spi_buf_len_t msg_size )
{
	if ( app == spi_app_mutex )
	{
		spi_buf_len_t i, i_max;
		spi_state |= SPI_STATE_TX;
		spi_state &= (~SPI_STATE_RX);

		i_max = SPI_BUFFER_SIZE - ( ( spi_tx_idx - spi_rx_idx ) & SPI_BUFFER_MASK );
		if ( msg_size < i_max )	{ i_max = msg_size;	}
		for ( i = 0; i < i_max; i++ )
		{
			spi_buffer[(spi_tx_idx++)& SPI_BUFFER_MASK] = message[i];
		}
		spi_jump_start();
		return i_max;
	}
	else
	{
		sys_error ( SYS_ERROR_SPI_WRONG_APP );
		return 0;
	}
}


/**
  * spi_request_rxtx ( app, message, msg_size )- sends a message via SPI
  */
spi_buf_len_t spi_request_rxtx ( spi_app_t app, spi_buf_t *message, spi_buf_len_t msg_size )
{
	if ( app == spi_app_mutex )
	{
		spi_buf_len_t i, i_max;
		spi_state |= SPI_STATE_TX;
		spi_state |= SPI_STATE_RX;

		i_max = SPI_BUFFER_SIZE - ( ( spi_tx_idx - spi_rx_idx ) & SPI_BUFFER_MASK );
		if ( msg_size < i_max )	{ i_max = msg_size;	}
		for ( i = 0; i < i_max; i++ )
		{
			spi_buffer[(spi_tx_idx++)& SPI_BUFFER_MASK] = message[i];
		}
		spi_jump_start();
		return i_max;
	}
	else
	{
		sys_error ( SYS_ERROR_SPI_WRONG_APP );
		return 0;
	}
}


/**
  * spi_request_rx ( app, message, msg_size )- sends a message via SPI
  *		NOTE: only requests reception of "msg_size" bytes, 
  *			and exits befor the end of transmission
  *	RETURN: number of bufferd bytes out of the (maximum) requested "msg_size"
  */
spi_buf_len_t spi_request_rx ( spi_app_t app, spi_buf_len_t msg_size )
{
	if ( app == spi_app_mutex )
	{
		spi_buf_len_t i, i_max;
		spi_state |= SPI_STATE_RX;
		spi_state &= (~SPI_STATE_TX);
		i_max = SPI_BUFFER_SIZE - ( ( spi_tx_idx - spi_rx_idx ) & SPI_BUFFER_MASK );
		if ( msg_size < i_max )	{ i_max = msg_size;	}
//		spi_tx_idx += i_max;	// schedule i_max byte transmission-receptions (SPI specific)
		for (i=0; i<i_max;i++)
		{
			spi_buffer[(spi_tx_idx++)& SPI_BUFFER_MASK] = 0x00;
		}
		spi_jump_start();
		return i_max;
	}
	else
	{
		sys_error ( SYS_ERROR_SPI_WRONG_APP );
		return 0;
	}
}



/**
  * spi_request_rx_filtered  ( app, message, msg_size, preamble, retries ) 
  *		- requests a message read via SPI with filtering of input
  *		The event will be reported "finished" when the "msg_size" bytes are read
  *		starting from first occurence of "preamble" character.
  *		number of retries indicate the maximum received bytes before giving up
  *
  *		NOTE: only requests reception of "msg_size" bytes, 
  *			and exits befor the end of transmission
  *	RETURN: number of bufferd bytes out of the (maximum) requested "msg_size"
  */
spi_buf_len_t spi_request_rx_filtered ( 
	spi_app_t app
	, spi_buf_len_t msg_size
	, spi_buf_t preamble
	, uint8_t retries
)
{
	if ( app == spi_app_mutex )
	{
		spi_buf_len_t i, i_max;
		spi_state |= SPI_STATE_RX;
		spi_state &= (~SPI_STATE_TX);
		i_max = SPI_BUFFER_SIZE - ( ( spi_tx_idx - spi_rx_idx ) & SPI_BUFFER_MASK );
		if ( msg_size < i_max )	{ i_max = msg_size;	}
//		spi_tx_idx += i_max;	// schedule i_max byte transmission-receptions (SPI specific)
		for (i=0; i<i_max;i++)
		{
			spi_buffer[(spi_tx_idx++)& SPI_BUFFER_MASK] = 0x00;
		}
		// Sets the filtering configuration
		spi_preamble_char = preamble;
		spi_old_event_mask = spi_event_flag_mask;
		spi_event_flag_mask |= SPI_EVENT_PER_BYTE;
		spi_max_retries = retries;

		spi_jump_start();
		return i_max;
	}
	else
	{
		sys_error ( SYS_ERROR_SPI_WRONG_APP );
		return 0;
	}
}



/**
  * com_rx_skip_bytes(app, len) - skips first "len" of bytes from RX buffer (drops them)
  * WARNING: assumes that there is "len" bytes read!!!!!
  */
void spi_rx_skip_bytes ( spi_app_t app, spi_buf_len_t len )
{
	if ( app != spi_app_mutex )
	{
		return;
	}
	// set tail after "len" bytes that will be dropped
	if ( spi_curr_idx - spi_rx_idx < len )
	{
		spi_rx_idx = spi_curr_idx; // set to curent position (nothing to read)
		// TODO: verify that we can make it IDLE (e.g. if streaming than do not)
		spi_state = SPI_STATE_IDLE;
	}
	else
	{
		spi_rx_idx -= len;
	}
}


/**
  * spi_rx_find_preamble (app, preamble) - skip bytes from RX buffer, 
  *		unless you detect "preamble"
  *	RETURN: returns number of available bytes
  */
spi_buf_len_t spi_rx_find_preamble ( spi_app_t app, spi_buf_t preamble )
{
	spi_buf_len_t i, i_max;
	if (app != spi_app_mutex)
	{
		return 0;
	}
	i_max = spi_curr_idx - spi_rx_idx ; 
	// Break when found preamble ...
	for (i=0; i<i_max;i++)
	{
		if (preamble == spi_buffer[spi_rx_idx & SPI_BUFFER_MASK])
		{
			return (spi_curr_idx - spi_rx_idx);
		}
		spi_rx_idx++;
	}
	// If not found than end the search
	spi_rx_idx = spi_curr_idx; // set to curent position (nothing to read)
	// TODO: verify that we can make it IDLE (e.g. if streaming than do not)
	spi_state = SPI_STATE_IDLE;
	return 0;
}



/**
  * spi_rx_available_bytes() - checks for number of read bytes (in RX buffer)
  * returns - # of valid/received bytes in the rx buffer
  */
spi_buf_len_t spi_rx_available_bytes ( spi_app_t app )
{
	if (app != spi_app_mutex) { return 0;}
	return ( spi_curr_idx - spi_rx_idx );
}


/**
  * spi_get_rx_block( app, buf, len) - copies "len" bytes from RX buffer 
  *		to a "buf" buffer
  * WARNING: assumes that there is "len" bytes to read!!!!!
  */
spi_buf_len_t spi_get_rx_block ( spi_app_t app, char *buffer, spi_buf_len_t len )
{
	spi_buf_len_t i, i_max;
	if (app != spi_app_mutex)
	{
		return 0;
	}
	if ( len > ( spi_curr_idx - spi_rx_idx ) )
	{
		i_max = spi_curr_idx - spi_rx_idx;
	}
	else
	{
		i_max = len;
	}
	for ( i = 0; i < i_max; i++ )
	{
		// copy byte (remember about "TAIL" pointing 1 byte behind the first valid byte)
		buffer[i] = spi_buffer[ (spi_rx_idx++) & SPI_BUFFER_MASK];
		if (spi_rx_idx == spi_curr_idx)
		{
			spi_state = SPI_STATE_IDLE;
		}
	}
	return i_max;
}


//#include "uart_pc.h"

// ############################################################################
// ############################################################################
/**
  * SPI_ISR() - SPI interrupt handler (4 flags avaialable)
*/
#ifdef FEAT_REPLICATOR
// Case of custom interrupt handler
	void SPI_ISR()  TASK_FUNC
#else
// Case of the static interrupt on F120
	void SPI_ISR()  interrupt HWM_INT_SPI
#endif
{
	spi_count_intr_++;

	if ( SPI_STATE_IDLE == spi_state )
	{
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
		SPIF = 0;
		WCOL = 0;
		MODF = 0;
		RXOVRN = 0;
//		sys_error ( SYS_ERROR_SPI_INCORRECT_STATE );
		return;
	}
	//Need another character to transmitt
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = SPI0_PAGE;
#endif // _SFRPAGE_EXIST_
	// SPIF - byte transfer ended
	// WCOL - collision on SFR-TO-BUFFER detected
	// MODF - when "I was stripped off of master status"
	// RXOVRN - overflow in SPI buffer (BUS-TO-BUFFER)

	if ( 1 == SPIF )
	{
#ifdef _SPI_REVERSE_LOGIC_
//		unsigned char temp_spi = 0xFF;
//		temp_spi = SPI0DAT;
//		spi_buffer[spi_curr_idx & SPI_BUFFER_MASK] = temp_spi; // read the RX byte
//		spi_buffer[spi_curr_idx & SPI_BUFFER_MASK] = 0xFF ^ SPI0DAT; // read the RX byte
//		spi_buffer[spi_curr_idx & SPI_BUFFER_MASK] = SPI0DAT ;
		spi_buffer[spi_curr_idx & SPI_BUFFER_MASK] = 0xFF - SPI0DAT ;
#else // _SPI_REVERSE_LOGIC_
		spi_buffer[spi_curr_idx & SPI_BUFFER_MASK] = SPI0DAT; // read the RX byte
#endif // else _SPI_REVERSE_LOGIC_
//		com_char(0, SPI0DAT);
		spi_curr_idx++;
		SPIF = 0;
		// report event if needed
		if ( 0 != ( spi_event_flag_mask & SPI_EVENT_PER_BYTE ) )
		{
			if (SPI_PREAMBLE_OFF != spi_preamble_char)
			{
				if ( spi_preamble_char == spi_buffer[(spi_curr_idx-1) & SPI_BUFFER_MASK])
				{	// Found char -> read the requested number of bytes and later finish
					// end filtered operation
					spi_preamble_char = SPI_PREAMBLE_OFF;
					spi_event_flag_mask = spi_old_event_mask;
					spi_max_retries = 0;
				}
				else 
				{	// Character still not found - retry
					if ( --spi_max_retries > 0)
					{	// If not reached maximum retries -> shift indexes
						spi_rx_idx++; // skip last byte
						spi_buffer[(spi_tx_idx++)& SPI_BUFFER_MASK] = 0x00; // add one more retry
					}
					else
					{ 	// failed due to max retransmissions -> stop activity NOW
						spi_clear_buffer(spi_app_mutex);
						spi_preamble_char = SPI_PREAMBLE_OFF;
						spi_event_flag_mask = spi_old_event_mask;
						spi_max_retries = 0;
						( *spi_event_flag ) |= SPI_EVENT_FAILED_RX_FILTERED;
					}
				}
			}
			else // no filtering of preamble
			{
				( *spi_event_flag ) |= SPI_EVENT_PER_BYTE;
			}
		}
		if ( spi_curr_idx != spi_tx_idx )
		{
#ifdef _SPI_REVERSE_LOGIC_
//		{
//			unsigned char temp_spi = 0xFF;
//			temp_spi ^= spi_buffer[spi_curr_idx & SPI_BUFFER_MASK];
//			SPI0DAT = temp_spi;
//		}
		SPI0DAT = 0xFF - spi_buffer[spi_curr_idx & SPI_BUFFER_MASK];
#else // _SPI_REVERSE_LOGIC_
			SPI0DAT = spi_buffer[spi_curr_idx & SPI_BUFFER_MASK];
#endif // else _SPI_REVERSE_LOGIC_
		}
		else
		{
			// finished -> indicate state and app event flag
			spi_state |= SPI_STATE_DONE;
			( *spi_event_flag ) |= ( spi_event_flag_mask & SPI_EVENT_DONE );
			// if not receiving than discard the received bytes
			if ( 0 == (spi_state & SPI_STATE_RX) )
			{
				spi_rx_idx = spi_curr_idx;
			}
		}
	}

	// ################################################
	// ################################################
	if ( 1 == WCOL )
	{
	sys_error ( SYS_ERROR_SPI_COLLISION );
		WCOL = 0;	//Clear Collision flag
	}
	// ################################################
	// ################################################
	if ( 1 == MODF )
	{
		MODF = 0;	//Clear Master-status-lost flag
	}	// ################################################
	// ################################################
	if ( 1 == RXOVRN )
	{
	sys_error ( SYS_ERROR_SPI_OVERRUN );
		RXOVRN = 0;	//Clear Overrun flag
	}
}





//##############################################################################
//##############################################################################
//##############################################################################
//##############################################################################
// ### SCRAPBOOK for old/unused/abandoned code ###

/**
  * com_rx_read_byte(port, offset) - reads a byte at a given offset from the befining of read block (buffer tail)
  * returns value of byte at "offset" bytes from the begining of valid RX buffer for "port" UART
  * WARNING: assumes that there is "offset" bytes read!!!!!
  */
/*
unsigned char com_rx_read_byte ( unsigned char com_port, int offset )
{
	if ( 0 == com_port )
	{
		return com0_rx_buffer[ ( com0_rx_tail + 1 + offset ) % COM0_RX_BUFFER_SIZE];
	}
	return 0xFF;
}



void check_buffer_indexes()
{
	if (( com0_rx_head >= COM0_RX_BUFFER_SIZE )||( 0 > com0_rx_head ))
	{
		com0_rx_head = 0;
	}
	if (( com0_rx_tail >= COM0_RX_BUFFER_SIZE )||( -1 > com0_rx_tail ))
	{
		com0_rx_tail = com0_rx_head-1;
	}
}
*/


#endif // _ENABLE_SPI_MODULE_
