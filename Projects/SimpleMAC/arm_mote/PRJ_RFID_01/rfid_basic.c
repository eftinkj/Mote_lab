#include "defs/esnl_pub.h"

//#ifdef _ENABLE_RFID_01_
#ifdef _ENABLE_RFID_02_ 


#include "rfid_basic_prv.h"
#include "rfid_basic_pub.h"


#include "sys_func.h"

#ifdef _ENABLE_RFID_02_
#include "uart_0.h"
#endif // _ENABLE_RFID_02_



			sbit LED = P1^6;


/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

spi_event_t rfid_spi_event_flag;

spi_event_t rfid_spi_event_mask;

rfid_state_t rfid_state;

sbit TRIG = P2^0;

uint8_t rfid_result[RFID_READ_BUFFER_SIZE];

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/
// Support Functions
void rfid_delay(int t)
{
	int i, j;
	for(i=0;i<t;i++)
	{
		for(j=0;j<t;j++)
		{
			j = j+i;
			j = j-i;
		}
	}
}

/**
* uint8_t rfid_wait_for_stx(uint8_t timeout) - reads UART waiting for 0x02 (STX)
*	timeout is a maximum number of retries
*	RETURNS: RFID_OK if STX is found
*/
uint8_t rfid_wait_for_stx(uint8_t timeout)
{
	uint8_t c = 0;
	uint32_t tt =0;
	rfid_result[0] = 0x00;
	while ( ( 0x02 != rfid_result[0] ) && (c < timeout) )
	{
		tt = 0;
		while ( ( 1 > com_rx_read( RFID_UART_PORT ) ) && (0xF0000000 > tt) ) { tt++; }
		com_get_rx_block( RFID_UART_PORT, ( spi_buf_t* )( &rfid_result ), 1 );
	}
	return (0x02 == rfid_result[0])?RFID_OK:RFID_ERROR;
}

/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/


/**
  * rfid_init () - sets up RFID Reader module
  */
void rfid_init( void )
{
	// TOOD
#ifdef _ENABLE_RFID_01_
	rfid_spi_event_flag = SPI_EVENT_NONE;

	rfid_spi_event_mask = SPI_EVENT_RELEASED | SPI_EVENT_PURGED | SPI_EVENT_DONE
			| SPI_EVENT_FAILED_RX_FILTERED;
#endif // _ENABLE_RFID_01_

	rfid_state = RFID_STATE_DISCONNECTED;
	LED = 1;
}


//void rfi_chckeer

#define TEST_DELAY	3
/**
  * rfid_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void rfid_loop( void )
{
/*
#ifdef _ENABLE_RFID_01_
	if (RFID_OK == rfid_req_spi())
#endif // _ENABLE_RFID_01_
	{
//		rfid_wait_for_reader();
		while (1 != LED)
		{
			rfid_delay(TEST_DELAY);
			rfid_wait_for_reader();
			// LED = 1;
		}
		rfid_request_inventory();
//		rfid_request_sysparam();
		rfid_delay(TEST_DELAY+30);
		rfid_read_inventory();
		rfid_delay(TEST_DELAY+500);
	
#ifdef _ENABLE_RFID_01_
		rfid_free_spi();
		rfid_delay(TEST_DELAY);
#endif // _ENABLE_RFID_01_
	}
return;
*/
	switch (rfid_state)
	{
		case RFID_STATE_HALT:
			// DO NOTHING
			break;
		case RFID_STATE_DISCONNECTED:
			while (1 != LED)
			{
				rfid_delay(TEST_DELAY);
				rfid_wait_for_reader();
				// LED = 1;
			}
			rfid_state = RFID_STATE_IDLE;				
			break;
		case RFID_STATE_REQUEST_DISCONNECT:
//			rfid_free_spi();
			rfid_state = RFID_STATE_HALT;
			break;
		case RFID_STATE_IDLE:
			// ADD DELAY ???
			rfid_request_inventory();
			rfid_delay(TEST_DELAY+30);
			rfid_state = RFID_STATE_INVENTRY_READ;
			break;
		case RFID_STATE_INVENTRY_READ:
			rfid_read_inventory();
			break;
		case RFID_STATE_INVENTORY_DONE:
			// process?? count down repetitions??
			rfid_delay(TEST_DELAY+600);
			rfid_state = RFID_STATE_IDLE;
			break;
		case RFID_STATE_ERROR:
			break;
		case RFID_STATE_SPI_ERROR:
			break;
		default:
			sys_error(SYS_ERROR_RFID_UNKNOWN_STATE);
	}
	// TODO: NO LOOPS!!!!!!
}





// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################


/**
  * rfid_req_spi() - activates SPI and reserves it for RFID reader (also enables CS line)
*/
uint8_t rfid_req_spi()
{
	rfid_spi_event_flag = SPI_EVENT_NONE;

	if (SPI_R_OK != spi_request_dev(SPI_APP_RFID, 10000, &rfid_spi_event_flag, rfid_spi_event_mask))
	{
		sys_error(SYS_ERROR_RFID_SPI);
		return RFID_ERROR;
	}
	CS_RFID = RFID_CS_ENABLE;	// Select the ADE chip
//	CS_RFID = RFID_CS_DISABLE;	// Deselect the ADE chip
//	CS_RFID = RFID_CS_ENABLE;	// Select the ADE chip

//	CS_RFID = 0;	// Select the ADE chip
//	CS_RFID = 1;	// Deselect the ADE chip
//	CS_RFID = 0;	// Select the ADE chip

//SPI0CN |= 0x04;
//SPI0CN &= 0xFB;

	return RFID_OK;
}


/**
  * rfid_free_spi() - releases SPI and also disables CS line
  */
void rfid_free_spi()
{
	CS_RFID = RFID_CS_DISABLE;	// Deselect the ADE chip
	// 5) Release SPI
	spi_release_dev( SPI_APP_RFID );
}



/**
  * rfid_request_sysparam() - sends request for system parameter
  */
void rfid_request_sysparam()
{
//	uint16_t tx_count;//, rx_count0, rx_count1, rx_count2, rx_count3;

	rfid_sysparam_frame tx_frame;

	uint16_t msg_act_len = 0x0D; // +3 (=+2 for message length itself +1 for start byte)

	tx_frame.start = 0x02;
	tx_frame.msg_len = msg_act_len-3;
	tx_frame.flags = 0x0020;
	tx_frame.cmd = 0x1201;
	tx_frame.addr = 0x0000;
	tx_frame.param_len = 0x0004;
	tx_frame.crc = 0x8035;


//	tx_frame.addr = 0x0003;
//	tx_frame.param_len = 0x0002;
//	tx_frame.crc = 0x0A67;

//	msg_act_len = 9;
//	tx_frame.msg_len = msg_act_len - 3;
//	tx_frame.cmd = 0x1102;
//	tx_frame.addr = 0x97F8;

//msg_act_len ++; // for extra byte at the begining

#ifdef _ENABLE_RFID_01_
// existing spi interface code
	tx_count = spi_request_tx( SPI_APP_RFID, (spi_buf_t*)&tx_frame, msg_act_len);

	while ( 0 == ( rfid_spi_event_flag & SPI_EVENT_DONE ) );
	rfid_spi_event_flag &= ( ~SPI_EVENT_DONE );
#endif

// UART0 version
#ifdef _ENABLE_RFID_02_
	com_block(RFID_UART_PORT, (spi_buf_t*)&tx_frame, msg_act_len);
	while (com0_running);
#endif // _ENABLE_RFID_02_

	rfid_state = RFID_STATE_INVENTRY_READ;
}


/**
  * rfid_request_inventory() - sends request for inventory list of RFID tags
  */
void rfid_request_inventory()
{
	uint16_t tx_count;//, rx_count0, rx_count1, rx_count2, rx_count3;

	rfid_cmd_frame tx_frame;

	uint16_t msg_act_len = 0x0B; // +2 for message length itself +1 for start byte

	tx_frame.start = 0x02;
	tx_frame.msg_len = msg_act_len-3;
	tx_frame.flags = 0x22;
	tx_frame.cmd = 0x0101;
	tx_frame.tag_type = 0x0000;
	tx_frame.crc = 0xEE92;

#ifdef _ENABLE_RFID_01_
// existing spi interface code
	tx_count = spi_request_tx( SPI_APP_RFID, (spi_buf_t*)&tx_frame, msg_act_len );

	while ( 0 == ( rfid_spi_event_flag & SPI_EVENT_DONE ) );
	rfid_spi_event_flag &= ( ~SPI_EVENT_DONE );
#endif

// UART0 version
#ifdef _ENABLE_RFID_02_
	com_block(RFID_UART_PORT, (spi_buf_t*)&tx_frame, msg_act_len);
	while (com0_running);
#endif // _ENABLE_RFID_02_

	rfid_state = RFID_STATE_INVENTRY_READ;
}



/**
  * rfid_read_inventory() - reads inventory list of RFID tags
  */
void rfid_read_inventory()
{
	uint16_t rx_count0, rx_count1, rx_count2, rx_count3;

	rfid_taglist_frame *rx_frame = NULL;

	TRIG = 1;

	if (RFID_STATE_INVENTRY_READ != rfid_state)
	{ 	// should not be here
		sys_error(SYS_ERROR_RFID_WRONG_STATE);
	}

// SPI version
#ifdef _ENABLE_RFID_01_
	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx( SPI_APP_RFID, 14 ); // 8 for header

	while ( 0 == ( rfid_spi_event_flag & ( SPI_EVENT_DONE | SPI_EVENT_PURGED ) ) );
	if ( 0 == ( rfid_spi_event_flag & SPI_EVENT_FAILED_RX_FILTERED ) )
	{
		rfid_spi_event_flag &= ( ~SPI_EVENT_DONE );

		rx_count1 = spi_get_rx_block( SPI_APP_RFID, ( spi_buf_t* )( &rfid_result ), rx_count0 );
		rx_frame = (rfid_taglist_frame *) rfid_result;
		// rx_frame->msg_len == 04 -> end of inventory
		if (4 < rx_frame->msg_len)
		{
			//read the rest
			// depends on already read and message length
			uint8_t to_read = rx_frame->msg_len - rx_count0; // should NOT exceed 8-bit number range!!
	
			// Read response until found "SyncChar" or retries exceeded threshold
			rx_count2 = spi_request_rx( SPI_APP_RFID, to_read );
			while ( 0 == ( rfid_spi_event_flag & ( SPI_EVENT_DONE | SPI_EVENT_PURGED ) ) );
			rfid_spi_event_flag &= ( ~SPI_EVENT_DONE );
			rx_count3 = spi_get_rx_block( SPI_APP_RFID, ( spi_buf_t* )( &rfid_result[rx_count1] ), rx_count2 );
			// Read tag from rfid_result
		}
		else
		{
			// inventory read ended!!!!
			rfid_state = RFID_STATE_INVENTORY_DONE;
		}
	}
	else
	{
		// error
		rfid_state = RFID_STATE_ERROR;
	}
#endif // _ENABLE_RFID_01_

#define TIMEOUT_UART_READ 10000000

// UART0 version
#ifdef _ENABLE_RFID_02_
	rfid_wait_for_stx( TIMEOUT_UART_READ );

	while (2 > (rx_count0 = com_rx_read( RFID_UART_PORT ) ));
	{
		uint8_t to_read;
		uint32_t tt;
		rx_count1 = MIN(rx_count0, RFID_READ_BUFFER_SIZE);
		com_get_rx_block( RFID_UART_PORT, ( spi_buf_t* )( &rfid_result[1] ), rx_count1 );
		rx_frame = (rfid_taglist_frame *) rfid_result;

		//read the rest
		// depends on already read and message length
		to_read = rx_frame->msg_len - rx_count1 + 2; // should NOT exceed 8-bit number range!!
		tt = 0;
		while ( to_read > com_rx_read( RFID_UART_PORT ) )
		{
			tt++;
			if (TIMEOUT_UART_READ < tt) break;
		}
		rx_count2 = MIN(to_read, RFID_READ_BUFFER_SIZE);
		com_get_rx_block( RFID_UART_PORT, ( spi_buf_t* )( &(rfid_result[rx_count1+1]) ), to_read);
		if (4 < rx_frame->msg_len)
		{
			// read TAG info
			rfid_send_tag_info();
		}
		else
		{
			// inventory read ended!!!!
			rfid_state = RFID_STATE_INVENTORY_DONE;
		}
	}
#endif // _ENABLE_RFID_02_

	TRIG = 0;

}



/**
  * rfid_wait_for_reader() - waits for RFID reader to switch to TTL/serial
  */
void rfid_wait_for_reader()
{
	uint16_t rx_count0, rx_count1, rx_count2, rx_count3;

	rfid_taglist_frame *rx_frame = NULL;

	TRIG = 1;

// SPI version
#ifdef _ENABLE_RFID_01_
	// Read response until found "SyncChar" or retries exceeded threshold
	rx_count0 = spi_request_rx( SPI_APP_RFID, 14 ); // 8 for header

	while ( 0 == ( rfid_spi_event_flag & ( SPI_EVENT_DONE | SPI_EVENT_PURGED ) ) );
	if ( 0 == ( rfid_spi_event_flag & SPI_EVENT_FAILED_RX_FILTERED ) )
	{
		rfid_spi_event_flag &= ( ~SPI_EVENT_DONE );

		rx_count1 = spi_get_rx_block( SPI_APP_RFID, ( spi_buf_t* )( &rfid_result ), rx_count0 );
		rx_frame = (rfid_taglist_frame *) rfid_result;
		// rx_frame->msg_len == 04 -> end of inventory
		if (4 < rx_frame->msg_len)
		{
			//read the rest
			// depends on already read and message length
			uint8_t to_read = rx_frame->msg_len - rx_count0; // should NOT exceed 8-bit number range!!
	
			// Read response until found "SyncChar" or retries exceeded threshold
			rx_count2 = spi_request_rx( SPI_APP_RFID, to_read );
			while ( 0 == ( rfid_spi_event_flag & ( SPI_EVENT_DONE | SPI_EVENT_PURGED ) ) );
			rfid_spi_event_flag &= ( ~SPI_EVENT_DONE );
			rx_count3 = spi_get_rx_block( SPI_APP_RFID, ( spi_buf_t* )( &rfid_result[rx_count1] ), rx_count2 );
			// Read tag from rfid_result
		}
		else
		{
			// inventory read ended!!!!
			rfid_state = RFID_STATE_INVENTORY_DONE;
		}
	}
	else
	{
		// error
		rfid_state = RFID_STATE_ERROR;
	}
#endif // _ENABLE_RFID_01_

//#define TIMEOUT_UART_READ 100

// UART0 version
#ifdef _ENABLE_RFID_02_
	{
	uint32_t tt = 0;
	// Skip the first byte == 0x00 (when nothing is put on TX line -> LOW -> results in 0x00)
	rfid_result[0] = 0x00;
	while ( 0x00 == rfid_result[0] )
	{
		tt = 0;
		while ( ( 1 > com_rx_read( RFID_UART_PORT ) ) && (0xF0000000 > tt) )
		{
			tt++;
		}
		com_get_rx_block( RFID_UART_PORT, ( spi_buf_t* )( &rfid_result ), 1 );
		LED = ~LED;
	}

	tt = 0;
	while ( ( 7 > com_rx_read( RFID_UART_PORT ) ) && (100000 > tt) )
	{
		tt++;
	}
	}
	rx_count0 = com_rx_read( RFID_UART_PORT );
//	if ( 8 <=  rx_count0 )
	{
		rx_count1 = MIN(rx_count0, RFID_READ_BUFFER_SIZE);
		com_get_rx_block( RFID_UART_PORT, ( spi_buf_t* )( &rfid_result[1] ), rx_count1 );
		rx_frame = (rfid_taglist_frame *) rfid_result;
		if (0x1202 == rx_frame->flags)
		{
			LED = 1;
		}
	}
#endif // _ENABLE_RFID_02_

	TRIG = 0;

}



uint8_t bout[30];

/**
* void rfid_send_tag_info() - sends the TAG info in Missouri S&T packet format
*/
void rfid_send_tag_info()
{
	bout[0] = 0x42;
	bout[1] = 0x75; // FLAG_DATA_MODULAR	
	bout[2] = 0xFF; // DST MAC
	bout[3] = 0xFF;
	bout[4] = 0x00; // SRC MAC
	bout[5] = 0xAB;
	bout[6] = 20; // length
	bout[7] = 0xFE; // DST ID
	bout[8] = 0xAB; // SRC ID
	bout[9] = 0xA0;	// PKT_MODULE_TYPE_RFID_READER_V1
	bout[10] = RFID_REPORT_TYPY_TAG_INVENTORY_LEN; // PKT_MODULE Length
	bout[11] = RFID_REPORT_TYPY_TAG_INVENTORY;
	bout[12] = rfid_result[5]; // TAG TYPE
	bout[13] = rfid_result[6];
	bout[14] = rfid_result[9]; // TAG ID
	bout[15] = rfid_result[10];
	bout[16] = rfid_result[11];
	bout[17] = rfid_result[12];
	bout[18] = rfid_result[13];
	bout[19] = rfid_result[14];
	bout[20] = rfid_result[15];
	bout[21] = rfid_result[16];
	bout[22] = (0x0c==rfid_result[8])?rfid_result[17]:0x00;
	bout[23] = (0x0c==rfid_result[8])?rfid_result[28]:0x00;
	bout[24] = (0x0c==rfid_result[8])?rfid_result[19]:0x00;
	bout[25] = (0x0c==rfid_result[8])?rfid_result[20]:0x00;
	bout[26] = 0x41;

	// Send to all neighbours
	bout[7] = 0xFF; // DST ID
	com_block( XBEE_UART_PORT, bout, 27);

	// Send to BS (0xFE)	Q
	bout[7] = 0xFE; // DST ID
	com_block( XBEE_UART_PORT, bout, 27);
}


#endif // _ENABLE_RFID_01_
