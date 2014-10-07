
//uart0 handler
#include "uart_pc.h"

#ifdef _ENABLE_UART_PC_

#ifdef _ENABLE_CAMT_HEADSET_01_
	#include "PRJ_CAMT_Headset_01/hset_basic_pub.h"
#endif // _ENABLE_CAMT_HEADSET_01_

#include <string.h>

#ifdef _ENABLE_UART0_
	int com0_tx_tail;
	int com0_tx_head;
	char com0_running;

	int com0_rx_tail;
	int com0_rx_head;

	unsigned char com0_tx_buffer[com0_tx_buffer_size];
	unsigned char com0_rx_buffer[com0_rx_buffer_size];
#endif // _ENABLE_UART0_

#ifdef _ENABLE_UART1_
	int com1_tx_tail;
	int com1_tx_head;
	char com1_running;

//	int com1_rx_tail;
//	int com1_rx_head;

	unsigned char com1_tx_buffer[com1_tx_buffer_size];
//	unsigned char com1_rx_buffer[com1_rx_buffer_size];
#endif // _ENABLE_UART1_

#define _CLEAN_BUFFERS_
//#undef _CLEAN_BUFFERS_




void uart_setup(bit port)
{
	if(!port){
#ifdef _ENABLE_UART0_
		com0_tx_tail = 0;
		com0_tx_head = 0;
		com0_running = 0;

		com0_rx_tail = com0_rx_buffer_size-1;
		com0_rx_head = 0;
#ifdef _CLEAN_BUFFERS_
		{	int i;
			for( i = 0; i < com0_rx_buffer_size; i++ )
			{	com0_rx_buffer[i] = com0_tx_buffer[i] = 0xAA; }
		}
#endif
#endif // _ENABLE_UART0_
	}
#ifdef _ENABLE_UART1_
	else{
		com1_tx_tail = 0;
		com1_tx_head = 0;
		com1_running = 0;

//		com1_rx_tail = com1_rx_buffer_size-1;
//		com1_rx_head = 0;
#ifdef _CLEAN_BUFFERS_
		{	int i;
			for( i = 0; i < com1_rx_buffer_size; i++ )
			{	/*com1_rx_buffer[i] = */ com1_tx_buffer[i] = 0xAA; }
		}
#endif
	}
#endif // _ENABLE_UART1_
}


int com_tx_size(bit port)
{
	int ret;
	if(!port){
#ifdef _ENABLE_UART0_
		ret = com0_tx_head - com0_tx_tail;

		if(ret < 0)
			ret = com0_tx_buffer_size + ret;
#endif // _ENABLE_UART0_
	}
#ifdef _ENABLE_UART1_
	else{
		ret = com1_tx_head - com1_tx_tail;

		if(ret < 0)
			ret = com1_tx_buffer_size + ret;
	}
#endif // _ENABLE_UART1_
	return ret;
}

//send a block from XDATA
void uart_send_block_x ( bit port, uint8_t xdata *message, unsigned char msg_size )
{
	unsigned char  i;
	for ( i = 0; i < msg_size; i++ )
	{
		com_transmit(port, message[i]);
	}
}


void uart_send_text(bit port, const char message[], int len)
{
	int i;

	for(i=0; i<len; i++)
	{
		com_transmit(port, message[i]);
		//com_int(port, i);
	}
}

//actually copy things into the uart1 tx buffer
//and check the running state of uart1
void com_transmit(bit port, char letter)
{
	int i;
	if(!port){
#ifdef _ENABLE_UART0_
		com0_tx_buffer[com0_tx_head] = letter;
		com0_tx_head ++;
		if(com0_tx_head >= com0_tx_buffer_size)
		{
			com0_tx_head = 0;
		}
		if(com0_tx_head == com0_tx_tail)
		{
			//buffer overrun occured
			i++;
		}
		com0_jump_start();
#endif // _ENABLE_UART0_
	}
#ifdef _ENABLE_UART1_
	else{
		com1_tx_buffer[com1_tx_head] = letter;
		com1_tx_head ++;
		if(com1_tx_head >= com1_tx_buffer_size)
		{
		com1_tx_head = 0;
		}
		if(com1_tx_head == com1_tx_tail)
		{
		//buffer overrun occured
		i++;
		}
		com1_jump_start();
	}
#endif // _ENABLE_UART1_
}

#ifdef _ENABLE_UART0_
//if uart1 is not currently transmitting then we need
//to jumpstart the interrupt process
void com0_jump_start()
{
	if(com0_running == 0)
	{
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
		TI0 = 1;
		com0_running = 1;
	}
}
#endif // _ENABLE_UART0_

#ifdef _ENABLE_UART1_
void com1_jump_start()
{
	if(com1_running == 0)
	{
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART1_PAGE;
#endif // _SFRPAGE_EXIST_
		TI1 = 1;
		com1_running = 1;
	}
}
#endif // _ENABLE_UART1_




#ifdef _ENABLE_UART0_
//routine that handles the transmit and receive
//interrupts from uart0
void UART0_ISR() interrupt HWM_INT_UART0
{
	//Need another character to transmitt
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
	if(TI0 == 1)
	{
		if(com0_tx_tail != com0_tx_head)
		{
			SBUF0 = com0_tx_buffer[com0_tx_tail];
			com0_tx_tail ++;
			if(com0_tx_tail >= com0_tx_buffer_size)
			{
				com0_tx_tail = 0;
			}
			//com0_running = 1;
		}
		else
		{
			com0_running = 0;
		}
		//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
		TI0 = 0;
	}

	//We have received a character
	else if(RI0 == 1)
	{
		if(com0_rx_tail != com0_rx_head)
		{
			com0_rx_buffer[com0_rx_head] = SBUF0;
			com0_rx_head ++;
			if(com0_rx_head >= com0_rx_buffer_size)
			{
				com0_rx_head = 0;
			}
		}
		//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART0_PAGE;
#endif // _SFRPAGE_EXIST_
		RI0 = 0;
	}
}
#endif // _ENABLE_UART0_


#ifdef _ENABLE_UART1_
//routine that handles the transmit and receive
//interrupts from uart0
void UART1_ISR() interrupt HWM_INT_UART1
{
	//Need another character to transmitt
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART1_PAGE;
#endif // _SFRPAGE_EXIST_
	if(TI1 == 1)
	{
		if(com1_tx_tail != com1_tx_head)
		{
			SBUF1 = com1_tx_buffer[com1_tx_tail];
			com1_tx_tail ++;
			if(com1_tx_tail >= com1_tx_buffer_size)
			{
				com1_tx_tail = 0;
			}
			//com1_running = 1;
		}
		else
		{
			com1_running = 0;
		}
		//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART1_PAGE;
#endif // _SFRPAGE_EXIST_
		TI1 = 0;
	}

	//We have received a character
	else if(RI1 == 1)
	{
		uint8_t rx_buff = SBUF1;
		// TODO: dynamic call of a receive handler
#ifdef _ENABLE_CAMT_HEADSET_01_
		hset_recv_uart(rx_buff);
#endif // _ENABLE_CAMT_HEADSET_01_
		//clear interrupt flag
#ifdef _SFRPAGE_EXIST_
		SFRPAGE = UART1_PAGE;
#endif // _SFRPAGE_EXIST_
		RI1 = 0;
	}
}

#endif // _ENABLE_UART1_

#endif // _ENABLE_UART0_
