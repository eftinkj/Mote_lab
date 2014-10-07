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
#ifndef _SPI_PUB_H_
#define _SPI_PUB_H_

//#include "hw_specific.h"
//#include "feat_enable.h"
#include "defs/esnl_pub.h"

#ifdef _ENABLE_SPI_MODULE_

#include "rtc.h"

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

typedef uint8_t spi_buf_len_t;	// type for buffer length and indexes
typedef uint8_t spi_buf_t;		// type for buffer memory
typedef uint8_t spi_state_t;	// type for state of SPI interface
typedef uint8_t spi_app_t;		// type for application using/blocking the SPI
typedef uint8_t spi_timeout_t;
typedef uint8_t	spi_event_t;


#ifdef _ARM_
	#define SPI_IDX_MEM	
#else // _ARM_
	#define SPI_IDX_MEM	idata
#endif // else _ARM_

extern spi_buf_len_t SPI_IDX_MEM spi_curr_idx;
extern spi_buf_len_t SPI_IDX_MEM spi_tx_idx; // last byte to TX
extern spi_buf_len_t SPI_IDX_MEM spi_rx_idx; // first byte from RX (to be read by app)

// bit encoding:
// bit 0 -> RX
// bit 1 -> TX
// bit 2 -> FINISHED OPERATION
// bit 7 -> ERROR
#define SPI_STATE_IDLE	0
#define SPI_STATE_RX	1
#define SPI_STATE_TX	2
#define SPI_STATE_RXTX	3
#define SPI_STATE_DONE	4
#define	SPI_STATE_DONE_RX	5
#define	SPI_STATE_DONE_TX	6
#define	SPI_STATE_DONE_RXTX	7

#define SPI_STATE_ERROR		0x80
#define	SPI_STATE_ERROR_RX	0x81
#define	SPI_STATE_ERROR_TX	0x82
#define	SPI_STATE_ERROR_RXTX	0x83


#define SPI_EVENT_NONE	0
#define SPI_EVENT_PER_BYTE	0x01
#define SPI_EVENT_RELEASED	0x02
#define SPI_EVENT_PURGED	0x04
#define SPI_EVENT_DONE	0x08
#define SPI_EVENT_FAILED_RX_FILTERED	0x10
#define SPI_EVENT_6	0x20
#define SPI_EVENT_7	0x40
#define SPI_EVENT_8	0x80



#define SPI_APP_NONE	0
#define SPI_APP_DMA		1
#define SPI_APP_SD		2
#define SPI_APP_CC2420		3

#define SPI_APP_COMPASS		4

// Boeing Utility Monitoring - power monitoring chip on baseboard
#define SPI_APP_POWER		5
// F1 - Matrix Switches (MUX)
#define SPI_APP_MUX0		6
#define SPI_APP_MUX1		7

// HEADSET SPI application (for connection
#define SPI_APP_HEADSET		10


#define SPI_R_OK	1
#define SPI_R_FAIL	0






// function prototypes

/**
  * spi_init() - sets up SPI controller
  */
void spi_init_sw ( void );


/**
  * spi_setup(baudrate) - sets up SPI speed
  */
uint8_t spi_setup ( uint16_t baudrate );



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
);

/**
* spi_release_dev( app ) - releases SPI reservation by an "app"
*	RETURN: 1 (SPI_R_OK) - if request has been fulfilled (SPI is freed by app)
*			0 (SPI_R_FAIL) - if unsucceflul (e.g. used by another app or free)
*/
uint8_t spi_release_dev (
	spi_app_t app
);


/**
* spi_force_release_dev( ) - forces releases of SPI due to timeout
*		setes the application's event flag to PURGED state
*/
void spi_force_release_dev(void);


/**
* spi_check_timeout () - functions executed within a main loop
*		to periodically check if timeout expired for current application
*		NOTE: should be moved to "TASK SCHEDULER" as time trigered event
*/
void spi_check_timeout(void);



// ############################################################################
// ############################################################################


/**
  * spi_clear_buffer(app) - empties whole RX/TX buffer
  */
void spi_clear_buffer(spi_app_t app);


/**
  * spi_request_tx ( app, message, msg_size )- sends a message via SPI
  */
spi_buf_len_t spi_request_tx ( spi_app_t app, spi_buf_t *message, spi_buf_len_t msg_size );


/**
  * spi_request_rxtx ( app, message, msg_size )- sends a message via SPI
  */
spi_buf_len_t spi_request_rxtx ( spi_app_t app, spi_buf_t *message, spi_buf_len_t msg_size );


/**
  * spi_send_block ( app, message, msg_size )- sends a message via SPI
  */
spi_buf_len_t spi_request_rx ( spi_app_t app, spi_buf_len_t msg_size );


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
	, uint8_t reties
);



/**
  * com_rx_skip_bytes(len) - skips first "len" of bytes from RX buffer (drops them)
  * WARNING: assumes that there is "len" bytes read!!!!!
  */
void spi_rx_skip_bytes ( spi_app_t app, spi_buf_len_t len );


/**
  * spi_rx_find_preamble (app, preamble) - skip bytes from RX buffer, 
  *		unless you detect "preamble"
  *	RETURN: returns number of available bytes
  */
spi_buf_len_t spi_rx_find_preamble ( spi_app_t app, spi_buf_t preamble );


/**
  * spi_rx_available_bytes(app) - checks for number of read bytes (in RX buffer)
  * returns - # of valid/received bytes in the rx buffer
  */
spi_buf_len_t spi_rx_available_bytes ( spi_app_t app );

/**
  * spi_get_rx_block( app, buf, len) - copies "len" bytes from RX buffer to a "buf" buffer
  * WARNING: assumes that there is "len" bytes to read!!!!!
  */
spi_buf_len_t spi_get_rx_block ( spi_app_t app, char *buffer, spi_buf_len_t len );



/**
  * SPI_ISR() - SPI interrupt handler (4 flags avaialable)
  */
#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void SPI_ISR()  TASK_FUNC;
#else
	// Case of the static interrupt on F120
		#ifdef __SDCC__
			#ifdef _ARM_
			void SPI_ISR(void); //  interrupt HWM_INT_SPI;
			#else // _ARM_
			void SPI_ISR()  interrupt HWM_INT_SPI;
			#endif // else _ARM_
		#endif // __SDCC__
#endif


#endif // _ENABLE_SPI_MODULE_

#endif
