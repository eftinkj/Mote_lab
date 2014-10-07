#include "defs/esnl_pub.h"

#if defined(_ENABLE_CAMT_HEADSET_01_) || defined(_ENABLE_CAMT_HEADSET_02_)

#include "hset_basic_prv.h"
#include "hset_basic_pub.h"


//#include "FEAT_Xbee_API/API_frames.h"
#include "FEAT_Networking/tsp_common_pub.h"
#include "sys_func.h"
#include "Application.h"
#include "HW_LIB/spi_pub.h"
//#include "FEAT_Queueing/Queuing.h"
#include "HW_LIB/uart_pc.h"
//#include "FEAT_Routing/routing_MMCR.h"
//#include "FEAT_Networking/Phy_layer.h"
#include "FEAT_adpcm/g72x.h" /* adpcm stuff */
#include "FEAT_Scheduler/sch_basic_pub.h"


#include "FEAT_Queueing/Queuing.h"


//#define _HSET_DEBUG_ENABLE_
#undef _HSET_DEBUG_ENABLE_
#define _HSET_DEBUG_LEVEL_	0

#define BUFFER_MAX_SIZE 80
/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

//uint8_t 	truck_state_test;
rtc_tick_t	hset_timeout;
rtc_tick_t	hset_read_timeout;

//Tim & Kelvin Start
rtc_tick_t	vocoder_comm_timeout;

uint8_t		hset_read_enabled;
uint8_t     packet_flag=0;

unsigned char buffer[]={ '\x20' , '\x37' , '\x40' , '\x37' , '\x20' , '\x17' , '\x0' , '\x17'};

//unsigned char buffer[8];

//spi_event_t vocoder_comm_event_flag = 0;
//spi_event_t vocoder_comm_event_mask;


//spi_buf_t hset_message[BUFFER_MAX_SIZE];
uint8_t msg_size;


uint8_t hset_pkt_id;
uint8_t packet_size;
uint8_t hset_pkt_max_size_;
uint8_t hset_samples_count_;
uint8_t XDATA* hset_samples_buff_;
uint8_t XDATA* hset_samples_buff_len_ptr_;
//uint8_t m=0;

uint8_t hset_waiting_packet;	// stores index to packet that arrived and is waiting to be processed

uint8_t XDATA *ptr_to_mod_data;
uint8_t ptr_to_packet_index;

//pkt_len_t package_size;

//sbit P0_5 = P0^5;
//sbit P0_6 = P0^6;
//Tim & Kelvin end

uint16_t timercounter = 0;
uint16_t sentpacketcounter = 0;
uint16_t receivedpacketcounter = 0;
uint16_t reportcounter = 0;
uint16_t modulecounter = 0;
uint16_t tick_max = 0;
int hello = 0;

uint8_t hset_seq = 0;
uint16_t avail_rx_bytes;

#ifdef _HSET_DEBUG_ENABLE_
	// DEBUG
	char	debug_buf[255];
	#include <string.h>
#endif // _HSET_DEBUG_ENABLE_


uint8_t	debug_spi[1000];
uint16_t debug_spi_i = 0;

uint8_t hset_vocoder_running;

uint16_t hset_dropped_due_to_queue_full;
uint16_t hset_NULL_error_count_;

//struct g72x_state enc_state_ptr; /* adpcm stuff */
//struct g72x_state dec_state_ptr; /* adpcm stuff */
//int16_t temp_message[] = {0xFF, 0xFF, 0xFF, 0xFF};

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/


/**
  * hset_init () - sets up headset for voice comm
  */
void hset_init ( void )
{
//	hset_timeout = RTC_OVERFLOW_TIMER_VALUE;	
	hset_pkt_id = 0xFF;
	packet_size = 0;
	hset_waiting_packet = 0xFF;

	hset_pkt_max_size_ = HSET_SAMPLES_PER_PKT;
	hset_samples_count_ = HSET_SAMPLES_PER_PKT; // FIRST TIME -> request a buffering packet
	hset_samples_buff_ = NULL;

//	api_send_packet16 ( "Headset module is UP\r", 21, 0xFFFF );

	modulecounter++;

	sequence_no_ = 1;
	hset_dropped_due_to_queue_full = 0;
	hset_NULL_error_count_ = 0;
//	g72x_init_state(&enc_state_ptr); /* adpcm stuff */
//	g72x_init_state(&dec_state_ptr); /* adpcm stuff */

	// Debugging -> sent directly to destination:
//	rtr_add_neighbor(HSET_DST, HSET_DST, 1);

#ifndef _ARM_
	uart_setup(1); // Needed for F120/F340/... but not ARM - for now
#endif // _ARM_

	sch_add_loop((sch_loop_func_t)hset_loop);

}

/**
 *  hset_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
 */
void hset_loop( void )
{
	if ((hset_samples_count_ >= hset_pkt_max_size_))
	{
		hset_samples_buff_ = NULL;
		// TODO: sent packet out
		hset_sent_packet();
	}
	//First we have to check if the packet is there
//	uint8_t n;
	if (0 == packet_size) //If we don't have a packet, create or RESERVE ONE
	{
		if (0 == hset_create_report())
		{
			// error - no packet reserved - skip transmission
			packet_flag = 0;
			hset_dropped_due_to_queue_full++;
			return;
		}
	}
	hset_processSerialInput();
}

/*******************************************************************************
* Function Name  : hset_processSerialInput
* Description    : It processes serial commands
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void hset_processSerialInput( void )
{
	static u8 buff_char;

	if (serialReadByte(&buff_char))
	{
		hset_recv_uart(buff_char);
	}
}/* end hset_processSerialInput() */



/**
 * hset_sent_packet() - sends the current packet out
 */
void hset_sent_packet()
{		
	//If sending out, get new packet from buffer or reserve a new one
	if (0xFF != hset_pkt_id)
	{
		tsp_send_from_modules(hset_pkt_id);
		sentpacketcounter++;

		//packet_flag = 0;
		hset_pkt_id = 0xFF;
		packet_size = 0;
	}
	hset_create_report();
}

// IAR Standard library hook for serial output
extern size_t __write(int handle, const unsigned char * buffer, size_t size);

/**
  * hset_execute_command (packet) - executes a command received in "packet"
  *		sets the controller and prepares timeout
  */
void hset_execute_command ( uint8_t *module)
{	

	hset_report_packet_v1_t *ph= ( hset_report_packet_v1_t *)module;
	uint8_t XDATA*pkt_buff = &(ph->content);
	uint8_t original_size = ph->module_length - 1;
//	uint8_t i;
	if (PKT_MODULE_TYPE_HSET_SAMPLES_V1 == ph->module_type)
	{
//		switch (ph->report_type)
		{
//			case HSET_REPORT_V1:
				{
#ifndef _ARM_
					uart_send_block_x (1, pkt_buff, original_size);
#else // not _ARM_
					__write(_LLIO_STDOUT, (unsigned char *)pkt_buff, original_size);
//					for(int i=0; i< original_size; i++)
//					{
//						PUTCHAR_PROTOTYPE(pkt_buff[i]);
//					}
#endif // else not _ARM_
					return;
				}
//				break;
		}
	}
/*	if (PKT_MODULE_TYPE_VEST == ph->module_type)
	{
		
	}
*/
}



/**
* uint8_t hset_create_report () - creates and fills out the report packet
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t hset_create_report ()
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
	//uint8_t hset_pkt_id = 0xFF;
	uint8_t *mod = NULL;
	
	packet_size = tsp_reserve_packet(PKT_MODULE_LEN_HSET_SAMPLES_V1 + HSET_SAMPLES_PER_PKT
									, &hset_pkt_id, HSET_DST);//DECLARE THIS GLOBAL
	if (0 == packet_size)
	{
		// Error
		hset_pkt_id = 0xFF;
		sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	else
	{
		hset_report_packet_v1_t *ph;
		mod = tsp_get_dummy(hset_pkt_id);
		// SET DESTINATION ADDRESS FOR THE OTHER END:
	// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
	//    .... (Possibly defered, but not for too long)
		ph = ( hset_report_packet_v1_t * )mod ;
		//original_size = ph->module_length;
		ph->module_type = PKT_MODULE_TYPE_HSET_SAMPLES_V1;
		hset_samples_buff_len_ptr_ = &(ph->module_length);
		hset_samples_buff_len_ptr_[0] = PKT_MODULE_LEN_HSET_SAMPLES_V1 + HSET_SAMPLES_PER_PKT;
		ph->report_type = sequence_no_++;//HSET_REPORT_V1;

		hset_pkt_max_size_ = HSET_SAMPLES_PER_PKT;//ph->module_length - PKT_MODULE_LEN_HSET_SAMPLES_V1;
		hset_samples_buff_ = &(ph->content);
		hset_samples_count_ = 0;
		//ptr_to_mod_data = &(ph->content);
		//ptr_to_packet_index = 0;
	}
	reportcounter++;
	return 1;
}

void hset_crop_packet(uint8_t data_size)
{
	hset_samples_buff_len_ptr_[0] = PKT_MODULE_LEN_HSET_SAMPLES_V1 + data_size;
	hset_samples_buff_[data_size] = PKT_MODULE_TYPE_END;
	// it will still sent entire packet but the receiver will skip the "padding"
}


#define STR_CC_U16	0x4343
#define STR_CV_U16	0x5643
#define STR_TR_U16	0x5254

void vest_resolve_dst()
{
	u8 dst_id = 0xff;
#ifdef _HSET_FIXED_DST_
	dst_id = HSET_DST;
#else
	union{ char dst_str[2]; u16 dst_u16;};
	dst_str[0] = hset_samples_buff_[0];
	dst_str[1] = hset_samples_buff_[1];
	
	switch (dst_u16)
	{
	  case STR_CC_U16:
		dst_id = VEST_CC_ADDR;
		break;
	  case STR_CV_U16:
		dst_id = VEST_CV_ADDR;
		break;
	  case STR_TR_U16:
		dst_id = VEST_TR_ADDR;
		break;
	  default:
		sys_error(5);
		break;
	}
#endif
	{
		uint16_t	pkt_base;
		pkt_v2_t * XDATA pkt = NULL;
		pkt_base = ( u16 ) QBUFF_BASE ( (hset_pkt_id) );
		pkt = ( pkt_v2_t* ) ( & ( buffer0[pkt_base] ) ); //&(QBUFF_ACCESS(base,0));
		pkt->dst_id = dst_id;
	}
}

/**
* hset_recv_uart(rx_buff) - called when received a new sample byte
*   NEED TO BE QUICK!! VERY QUICK!!
*/
void hset_recv_uart(uint8_t rx_buff)
{
	if (hset_samples_count_ < hset_pkt_max_size_)
	{
		//TODO: store the rx_buff in the packet
		if (NULL != hset_samples_buff_)
		{
			hset_samples_buff_[hset_samples_count_] = rx_buff;
			hset_samples_count_++;
#ifdef HSET_ASCI_UART_ENABLE_CR_FLUSH
			if ('\r' == rx_buff)
			{
				//initiate forwarding of data via 802.15.4
				vest_resolve_dst();
				hset_crop_packet( hset_samples_count_ );
				hset_pkt_max_size_ = hset_samples_count_;
			}
#endif //HSET_ASCI_UART_ENABLE_CR_FLUSH
		}
		else
		{
			hset_NULL_error_count_++;
		}
	}
	else
	{
		//TODO: count dropped bytes
	}
}



// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################



#endif // _ENABLE_CAMT_HEADSET_01_
