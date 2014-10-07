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
#ifndef UART0_H
#define UART0_H

//#include "hw_specific.h"
//#include "feat_enable.h"
#include "defs/esnl_pub.h"


// Define type for buffer index for buffered UART module (typically below 255 -> hence uint8_t sufficient)
typedef unsigned char	buf_idx_t;


#ifdef _ENABLE_OLD_UART_

//#define COM0_TX_BUFFER_SIZE 250
//#define COM0_RX_BUFFER_SIZE 250

#define COM0_TX_BUFFER_SIZE 150
#define COM0_RX_BUFFER_SIZE 150

//extern int com0_tx_tail;
//extern int com0_tx_head;
//extern char com0_tx_buffer[COM0_TX_BUFFER_SIZE];
extern char XDATA com0_running;
extern uint8_t com0_tx_tick_; // Watchdog for UART0 interrupt

extern unsigned int XDATA count_com0_ticks;

//extern int com0_rx_tail;
//extern int com0_rx_head;
//extern char com0_rx_buffer[COM0_RX_BUFFER_SIZE];



// function prototypes
void com_block ( unsigned char  com_port, char *message, buf_idx_t msg_size );
//send a string out uart0
void com_block_x ( unsigned char  com_port, char XDATA *message, buf_idx_t msg_size );
void com_block_base ( unsigned char  com_port, unsigned char idx, buf_idx_t msg_size );

void com0_jump_start ( void );
void com0_setup ( void );
void com0_char ( char letter );
void com0_transmit ( char letter );

#ifdef EXTENDED_UART
void com_string ( unsigned char  com, char *msg );
void com0_string ( char *message );
void com0_int ( int number );
void com0_uint ( unsigned int number );
void com0_ulong ( unsigned long number );
#endif
//bit com0_compare_head(char *pattern);




/**
  * com_clear_buffer(port) - empties whole RX buffer
  */
//void com_clear_buffer(int port);

/**
  * com_rx_skip_bytes(port, len) - skips first "len" of bytes from RX buffer (drops them)
  * WARNING: assumes that there is "len" bytes read!!!!!
  */
void com_rx_skip_bytes ( unsigned char my_port, buf_idx_t len );





/**
  * com_rx_read(port) - checks for number of read bytes (in RX buffer)
  * returns - # of valid/received bytes in the rx buffer
  */
int com_rx_read ( unsigned char com_port );

/**
  * com_get_rx_block(port, buf, len) - copies "len" bytes from RX buffer of "port" to a "buf" buffer
  * WARNING: assumes that there is "len" bytes to read!!!!!
  */
void com_get_rx_block ( unsigned char com_port, char *buffer, buf_idx_t len );

/**
  * com_rx_read_byte(port, offset) - reads a byte at a given offset from the befining of read block (buffer tail)
  * returns value of byte at "offset" bytes from the begining of valid RX buffer for "port" UART
  * WARNING: assumes that there is "offset" bytes read!!!!!
  */
unsigned char com_rx_read_byte ( unsigned char port, buf_idx_t offset );


/**
  * com_check_checksum(port, length) - checks the checksum of an API frame
  * 	of the "length" size (total)
  * 	returns "1" if OK, or "0" if checksum failed
  * WARNING: assumes that there is "length" bytes read!!!!!
  */
uint8_t com_check_checksum ( unsigned char port, buf_idx_t length );


void uart_check_buffer_indexes(void);


#ifdef FEAT_REPLICATOR
	// Case of custom interrupt handler
	void UART0_ISR()  TASK_FUNC;
#else
	#ifndef __KEIL__
		// Case of the static interrupt on F120
	#ifdef _ARM_
		void UART0_ISR(void); //  interrupt HWM_INT_UART0 using HWM_INT_UART0_BANK;
	#else
		void UART0_ISR()  interrupt HWM_INT_UART0 using HWM_INT_UART0_BANK;
	#endif // _ARM_
	#endif // __KEIL__
#endif


#endif // _ENABLE_OLD_UART_

#endif
