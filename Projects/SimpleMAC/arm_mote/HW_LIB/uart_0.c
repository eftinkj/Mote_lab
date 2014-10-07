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
//uart0 handler



#include "uart_0.h"

#ifdef _ENABLE_OLD_UART_

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "common.h"
#ifdef _ENABLE_XBEE_API_
	#include "FEAT_Xbee_API/API_frames.h"
#endif // _ENABLE_XBEE_API_

#ifdef _ENABLE_QUEUING_
	#include "FEAT_Queueing/Queuing.h"
#endif // _ENABLE_QUEUING_

buf_idx_t xdata com0_tx_tail;
buf_idx_t xdata com0_tx_head;
char xdata com0_tx_buffer[COM0_TX_BUFFER_SIZE];
char xdata com0_running;

buf_idx_t xdata com0_rx_tail;
buf_idx_t xdata com0_rx_head;
char xdata com0_rx_buffer[COM0_RX_BUFFER_SIZE];


bit com0_tx_tick_ = 0; // Watchdog for UART0 interrupt

unsigned int xdata count_com0_ticks = 0;

int8_t com0_count_intr_tx;
int8_t com0_count_intr_rx;



void com0_setup()
{
	buf_idx_t i;
	com0_tx_tail = 0;
	com0_tx_head = 0;
	com0_running = 0;

	com0_rx_tail = COM0_RX_BUFFER_SIZE - 1;
	com0_rx_head = 0;

	com0_tx_tick_ = 0;
	count_com0_ticks = 0;

	com0_count_intr_tx = 0;
	com0_count_intr_rx = 0;

	for ( i = 0;i < COM0_RX_BUFFER_SIZE; i++ )
	{
		com0_rx_buffer[i] = 'A';
	}
	for ( i = 0;i < COM0_TX_BUFFER_SIZE; i++ )
	{
		com0_tx_buffer[i] = 'B';
	}
}


#ifdef EXTENDED_UART

void com_string ( unsigned char my_port, char *msg )
{
	if ( 0 == my_port )
	{
		com0_string ( msg );
	}
}
#endif


//send a string out uart0
void com_block ( unsigned char  com_port, char *message, unsigned char msg_size )
{
	buf_idx_t  i;
	if ( 0 == com_port )
	{
		for ( i = 0; i < msg_size; i++ )
		{
			com0_char ( message[i] );
		}
	}
}

//send a string out uart0
void com_block_x ( unsigned char  com_port, char xdata *message, unsigned char msg_size )
{
	buf_idx_t  i;
	if ( 0 == com_port )
	{
		for ( i = 0; i < msg_size; i++ )
		{
			com0_char ( message[i] );
		}
	}
}


#ifdef _ENABLE_QUEUING_

void com_block_base ( unsigned char com_port, unsigned char idx, unsigned char msg_size )
{
	buf_idx_t i;
//	unsigned int base = QBUFF_BASE(idx);
	unsigned int base = idx * 0x64;
	if ( 0 == com_port )
	{
		for ( i = 0; i < msg_size; i++ )
		{
			com0_char ( QBUFF_ACCESS ( base, i ) );
		}
	}
}
#endif // _ENABLE_QUEUING_









#ifdef EXTENDED_UART

//send a number as ascii out uart1
void com0_int ( int number )
{
	char xdata temp_buf[11];

	sprintf ( temp_buf, "%d", number );
	com0_string ( temp_buf );

	return;
}




void com0_ulong ( unsigned long number )
{
	char xdata temp_buf[11];

	sprintf ( temp_buf, "%lx", number );
	com0_string ( temp_buf );

	return;
}


void com0_uint ( unsigned int number )
{
	char xdata temp_buf[6];

	sprintf ( temp_buf, "%x", number );
	com0_string ( temp_buf );

	return;
}

//send a string out uart0
void com0_string ( char *message )
{
	int i;

	for ( i = 0; i < strlen ( message ); i++ )
	{
		com0_char ( message[i] );
	}
}
#endif


//send a character out uart0
void com0_char ( char letter )
{
	com0_transmit ( letter );
}

//actually copy things into the uart0 tx buffer
//and check the running state of uart0
void com0_transmit ( char letter )
{
	com0_tx_buffer[com0_tx_head] = letter;
	com0_tx_head ++;
	if ( com0_tx_head >= COM0_TX_BUFFER_SIZE )
	{
		com0_tx_head = 0;
	}
	com0_jump_start();
}



void com0_send_byte()
{
	//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	TI0 = 0;
	//	SFRPAGE = 0;
	if ( com0_tx_tail != com0_tx_head )
	{
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
		SBUF0 = com0_tx_buffer[com0_tx_tail];
		EA=0;
		EA=0;
		com0_tx_tail ++;
		if ( com0_tx_tail >= COM0_TX_BUFFER_SIZE )
		{
			com0_tx_tail = 0;
		}
		EA=1;
	}
	else
	{
		com0_running = 0;
	}

}



//if uart0 is not currently transmitting then we need
//to jumpstart the interrupt process
void com0_jump_start()
{
	if ( com0_running == 0 )
	{
//		SFRPAGE = 0;
//		TI0 = 1;
		com0_running = 1;
		com0_send_byte();
	}
}


//routine that handles the transmit and receive
//interrupts from uart0
#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void UART0_ISR()  TASK_FUNC
#else
	// Case of the static interrupt on F120
	void UART0_ISR()  interrupt HWM_INT_UART0 using HWM_INT_UART0_BANK
#endif
{
	//Need another character to transmitt
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	if ( TI0 == 1 )
	{
		com0_count_intr_tx++;

	//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	TI0 = 0;
	com0_tx_tick_ = 0;
	if ( com0_tx_tail != com0_tx_head )
		{
#ifdef _SFRPAGE_EXIST_
			SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
			SBUF0 = com0_tx_buffer[com0_tx_tail];
			com0_tx_tail ++;
			if ( com0_tx_tail >= COM0_TX_BUFFER_SIZE )
			{
				com0_tx_tail = 0;
			}
		}
		else
		{
			com0_running = 0;
		}

	}
	// We have received a character
#ifdef _SFRPAGE_EXIST_
	SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	if ( RI0 == 1 )
	{
		com0_count_intr_rx++;
	if ( com0_rx_tail != com0_rx_head )
		{
			com0_rx_buffer[com0_rx_head] = SBUF0;
			com0_rx_head ++;
			if ( com0_rx_head >= COM0_RX_BUFFER_SIZE )
			{
				com0_rx_head = 0;
			}
			// start RX process if rx is IDLE
			//			if (RX_IDLE == rx_state)
			//			{
			//				rx_state = RX_RECEIVING_HEADER;
			//			}
		}
		else
		{
			// DROP Byte and make notice
			//com0_rx_bytes_dropped = true;
		}
		//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
		RI0 = 0;
	}
}


/**
  * com0_compare_head(pattern) - checks if the last received bytes match the "pattern" string
  * returns '1' if pattern found or '0' otherwise
  */
/*
bit com0_compare_head(char* pattern)
{
	int rx;
	int i;
	bit eq = 1;
	int len = strlen(pattern);
	// Check if there was enough read
	int read = (com0_rx_head > com0_rx_tail)?(com0_rx_head - com0_rx_tail):(com0_rx_head + COM0_RX_BUFFER_SIZE - com0_rx_tail);
	read--; // to fix the size+1 due to tail being 1 char behind
	if (read<len)
	{
		return 0;
	}
	// compare from beginig of the suspected string
//	rx = (com0_rx_head+1 < len)?(COM0_RX_BUFFER_SIZE + com0_rx_head - len):(com0_rx_head - len);
	rx = (com0_rx_tail + 1) % COM0_RX_BUFFER_SIZE;
	for(i = 0; i< len; i++, rx++, rx%=COM0_RX_BUFFER_SIZE)
	{
		if(pattern[i]!=com0_rx_buffer[rx]) {
			eq=0;
			break;
		}
	}
	return eq;
}
*/


/**
  * com_clear_buffer(port) - empties whole RX buffer
  */
/*
void com_clear_buffer(int port)
{
	if (0 == port) {
		// set tail behind the head to indicate EMPTY buffer
		com0_rx_tail = (com0_rx_head==0)?(COM0_RX_BUFFER_SIZE - 1):(com0_rx_head - 1);
	}
}
*/

/**
  * com_rx_skip_bytes(port, len) - skips first "len" of bytes from RX buffer (drops them)
  * WARNING: assumes that there is "len" bytes read!!!!!
  */
void com_rx_skip_bytes ( unsigned char my_port, buf_idx_t len )
{
	if ( 0 == my_port )
	{
		// set tail after "len" bytes that will be dropped
		com0_rx_tail = ( com0_rx_tail + len ) % COM0_RX_BUFFER_SIZE;
		if (com0_rx_tail > COM0_RX_BUFFER_SIZE)
			com0_rx_tail = com0_rx_tail % COM0_RX_BUFFER_SIZE;
		while (0 > com0_rx_tail)
			com0_rx_tail = com0_rx_tail + COM0_RX_BUFFER_SIZE;
	}
}




/**
  * com_rx_read(port) - checks for number of read bytes (in RX buffer)
  * returns - # of valid/received bytes in the rx buffer
  */
int com_rx_read ( unsigned char com_port )
{
	buf_idx_t n_read;
	if ( 0 == com_port )
	{
		n_read = ( com0_rx_head > com0_rx_tail ) ? ( com0_rx_head - com0_rx_tail ) : ( com0_rx_head + COM0_RX_BUFFER_SIZE - com0_rx_tail );
		n_read--; // to fix the size+1 due to tail being 1 char behind

		return n_read;
	}

	return 0;
}


/**
  * com_get_rx_block(port, buf, len) - copies "len" bytes from RX buffer of "port" to a "buf" buffer
  * WARNING: assumes that there is "len" bytes to read!!!!!
  */
void com_get_rx_block ( unsigned char com_port, char *buffer, buf_idx_t len )
{
	buf_idx_t i;
	if ( 0 == com_port )
	{
		for ( i = 0; i < len; i++ )
		{
			// copy byte (remember about "TAIL" pointing 1 byte behind the first valid byte)
			buffer[i] = com0_rx_buffer[ ( com0_rx_tail + 1 + i ) % COM0_RX_BUFFER_SIZE];
		}
		// Release the copied part of RX buffer
		com0_rx_tail = ( com0_rx_tail + len ) % COM0_RX_BUFFER_SIZE;
	}
}


/**
  * com_rx_read_byte(port, offset) - reads a byte at a given offset from the befining of read block (buffer tail)
  * returns value of byte at "offset" bytes from the begining of valid RX buffer for "port" UART
  * WARNING: assumes that there is "offset" bytes read!!!!!
  */
unsigned char com_rx_read_byte ( unsigned char com_port, buf_idx_t offset )
{
	if ( 0 == com_port )
	{
		return com0_rx_buffer[ ( com0_rx_tail + 1 + offset ) % COM0_RX_BUFFER_SIZE];
	}
	return 0xFF;
}



#ifdef _ENABLE_XBEE_API_
/**
  * com_check_checksum(port, length) - checks the checksum of an API frame
  * 	of the "length" size (total)
  * 	returns "1" if OK, or "0" if checksum failed
  * WARNING: assumes that there is "length" bytes read!!!!!
  */
bit com_check_checksum ( unsigned char port, unsigned char length )
{
	buf_idx_t start;
	buf_idx_t stop;
	unsigned char sum = 0;

	start = ( ( com0_rx_tail + 1 ) + API_HEADER_SIZE ) % COM0_RX_BUFFER_SIZE;
	stop = ( com0_rx_tail + length ) % COM0_RX_BUFFER_SIZE;

	if ( start < stop )
	{
		// one block
		sum += api_frame_calculate_sum ( & ( com0_rx_buffer[start] ), ( stop - start + 1 ) );
	}
	else
	{
		// two vlocks => wrap-around
		sum += api_frame_calculate_sum ( & ( com0_rx_buffer[start] ), ( COM0_RX_BUFFER_SIZE - start ) );
		sum += api_frame_calculate_sum ( & ( com0_rx_buffer[0] ), ( stop + 1 ) );
	}

	return ( sum == 0xFF ) ? 1 : 0;
}
#endif // _ENABLE_XBEE_API_


void uart_check_buffer_indexes()
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

#endif // _ENABLE_OLD_UART_
