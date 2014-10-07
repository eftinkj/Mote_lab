#ifndef _RFID_BASIC_PRV_H_
#define _RFID_BASIC_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "rfid_basic_pub.h"

#include "rtc.h"
#include "spi_pub.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/

sbit CS_RFID = P0^3; // SPECIFIC
//sbit CS_RFID = P2^0; // SPECIFIC

#define RFID_CS_DISABLE	1
#define RFID_CS_ENABLE	0


#define RFID_STATE_HALT	0
#define RFID_STATE_DISCONNECTED	1
#define RFID_STATE_REQUEST_DISCONNECT 2

#define RFID_STATE_IDLE	3
#define RFID_STATE_INVENTRY_READ	4
#define RFID_STATE_INVENTORY_DONE	5

#define RFID_STATE_ERROR	200
#define RFID_STATE_SPI_ERROR	201



#define RFID_OK		1
#define RFID_ERROR	0

typedef uint8_t	rfid_state_t;


/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

extern spi_event_t rfid_spi_event_flag;

extern spi_event_t rfid_spi_event_mask;

extern rfid_state_t rfid_state;

extern uint8_t rfid_result[RFID_READ_BUFFER_SIZE];

/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

#define RFID_SKIP_UNTIL_STX() \
{\
	rfid_result[0] = 0x00; \
	while ( 0x00 == rfid_result[0] ) \
	{ \
		uint32_t tt = 0; \
		while ( ( 1 > com_rx_read( RFID_UART_PORT ) ) && (0xF0000000 > tt) ) { tt++; } \
		com_get_rx_block( RFID_UART_PORT, ( spi_buf_t* )( &rfid_result ), 1 ); \
	} \
}

/**
* uint8_t rfid_wait_for_stx(uint8_t timeout) - reads UART waiting for 0x02 (STX)
*	timeout is a maximum number of retries
*	RETURNS: RFID_OK if STX is found
*/
uint8_t rfid_wait_for_stx(uint8_t timeout);


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/

/**
  * rfid_req_spi() - activates SPI and reserves it for RFID reader (also enables CS line)
*/
uint8_t rfid_req_spi();


/**
  * rfid_free_spi() - releases SPI and also disables CS line
  */
void rfid_free_spi();


/**
  * rfid_request_sysparam() - sends request for system parameter
  */
void rfid_request_sysparam();

/**
  * rfid_request_inventory() - sends request for inventory list of RFID tags
  */
void rfid_request_inventory();

/**
  * rfid_read_inventory() - reads inventory list of RFID tags
  */
void rfid_read_inventory();


/**
  * rfid_wait_for_reader() - waits for RFID reader to switch to TTL/serial
  */
void rfid_wait_for_reader();

/**
* void rfid_send_tag_info() - sends the TAG info in Missouri S&T packet format
*/
void rfid_send_tag_info();

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _RFID_BASIC_PRV_H_