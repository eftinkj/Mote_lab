/*
	uart_pc.h
	UART_PC provids the transport functions handler for both UART0 and UART1
	Users can access the incoming and outgoing data by calling the sets of functions
	and specify which port to use.

	Must run com_setup(bit port) to setup either the ports.

	Main functions for transmitting
	*	com_string(bit port, char *message)
	*	com_char(bit port, char letter)
	*	com_int(bit port, int number)
	*	com_uint(bit port, unsigned int number)

	Main functions for receiving
	*	com_rx_read(int port)
	*	com_get_rx_block()
	*	com_get_rx_block(int port, char *buffer, int len)

*/

#ifndef UART_PC_H
#define UART_PC_H

/*
	Standard Includes
*/

#include "defs/esnl_pub.h"


#ifdef _ENABLE_UART_PC_

//#include <c8051F120.h>
//#include "const_def_nov_21.h"

/*
	Define buffer size for rx and tx
*/

#define com0_tx_buffer_size 200
#define com0_rx_buffer_size 200

#define com1_tx_buffer_size 20
#define com1_rx_buffer_size 20

/*
	com_setup(bit port)
	Initialize Com Port
	pre: port must be either 0 or 1 -> 0 for UART0, 1 for UART1
*/

void uart_setup(bit port);

/*
	com0_jump_start(void)
	Jump start the process if the the port is not already transmitting
	No need to call this function
*/
#ifdef _ENABLE_UART0_
	void com0_jump_start(void);
#endif // _ENABLE_UART0_
#ifdef _ENABLE_UART1_
	void com1_jump_start(void);
#endif // _ENABLE_UART1_


//send a block from XDATA
void uart_send_block_x ( bit port, uint8_t xdata *message, unsigned char msg_size );

void uart_send_text(bit port, const char message[], int len);


/*
	com_transmit(bit port, char letter)
	Transmit a byte, put the byte into the tx_buffer, and set TI = 1
	UART interrupt service routine then will be called
	pre: port must be either 0 or 1 -> 0 for UART0, 1 for UART1
		 letter must be character
*/
void com_transmit(bit port, char letter);

/*
	com_tx_size(bit port)
	Return the length of untransmitted bytes in the tx_buffer
	pre: port must be either 0 or 1 -> 0 for UART0, 1 for UART1
*/

int com_tx_size(bit port);

/*
	com_rx_clear_buffer(bit port)
    Clears the entier RX buffer
	pre: port must be either 0 or 1 -> 0 for UART0, 1 for UART1
*/
//void com_rx_clear_buffer(bit port);

/*
   	com_rx_skip_bytes(port, len)
   	Skips first "len" of bytes from RX buffer (drops them)
	pre: port must be either 0 or 1 -> 0 for UART0, 1 for UART1
	WARNING: assumes that there is "len" bytes read!!!!!
*/
//void com_rx_skip_bytes(bit port, int len);


/*
   	com_rx_read(int port)
	Returns the number of received but unread bytes
	This function can be used by main or other objects to determine
	if there is new data avaliable.
	pre: port must be either 0 or 1 -> 0 for UART0, 1 for UART1
*/
//int com_rx_read(int port);


/*
   	com_get_rx_block(int port, char *buffer, int len)
	Get "len" number of bytes from rx_buffer and put into buffer
	"len" is usually determined by com_rx_read(int port)
	pre: port must be either 0 or 1 -> 0 for UART0, 1 for UART1
		 buffer is for storing the data from rx_buffer
		 len is the length of bytes to be read
	WARNING: assumes that there is "len" bytes to read!!!!!
*/
//void com_get_rx_block(int port, char *buffer, int len);


/*
	Special Function for reading a valid command for the Autonomous Vehicle
	Read from 0xFE and 0xFDm and store in to buffer, returns the actual
	number of bytes read.
*/
//int com_get_rx_valid_command(int port, char *buffer, int len);

/*
	com_rx_read_byte(port, offset)
	Reads a byte at a given offset from the befining of read block
	(buffer tail) returns value of byte at "offset" bytes from the
	begining of valid RX buffer for "port" UART
	pre: port must be either 0 or 1 -> 0 for UART0, 1 for UART1
	WARNING: assumes that there is "offset" bytes read!!!!!
*/
//unsigned char com_rx_read_byte(int port, int offset);

/*
	Interrupt Service routine which handles the rx and tx of data
	Take data from tx_buffer and send to terminal
	Get data from terminal and store to rx_buffer
*/
#ifdef _ENABLE_UART0_
	void UART0_ISR(void);
#endif // _ENABLE_UART0_
#ifdef _ENABLE_UART1_
	void UART1_ISR(void);
#endif // _ENABLE_UART1_

#ifdef _ENABLE_UART0_
	extern int com0_tx_tail;	//Pointer to tail of tx
	extern int com0_tx_head;	//Pointer to head of tx
	extern char com0_running;	//com0 is running

	extern int com0_rx_tail;	//Pointer to tail of rx
	extern int com0_rx_head;	//Pointer to head of rx

	extern unsigned char com0_tx_buffer[com0_tx_buffer_size];	//tx_buffer to store outgoing data
	extern unsigned char com0_rx_buffer[com0_rx_buffer_size];	//rx_buffer to store incoming data
#endif // _ENABLE_UART0_

#ifdef _ENABLE_UART1_
	extern int com1_tx_tail;	//Pointer to tail of tx
	extern int com1_tx_head;	//Pointer to head of tx
	extern char com1_running;	//com0 is running

//	extern int com1_rx_tail;	//Pointer to tail of rx
//	extern int com1_rx_head;	//Pointer to head of rx

	extern unsigned char com1_tx_buffer[com1_tx_buffer_size];	//tx_buffer to store outgoing data
//	extern unsigned char com1_rx_buffer[com1_rx_buffer_size];	//rx_buffer to store incoming data

#endif // _ENABLE_UART1_


#endif // _ENABLE_UART_PC_


#endif // UART_PC_H
