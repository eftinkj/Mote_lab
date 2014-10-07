#ifndef _SSN_8BIT_PRV_H_
#define _SSN_8BIT_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
//#include "rtc.h"
#include "ssn_8bit_pub.h"


#define SIG_FILTER_TYPE_BUTTER

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/



 typedef IAR_PACKED struct
{
//	float sample_interval;
	uint16_t head_;
	uint16_t tail_;
	uint16_t next_pkt_; // index at which to sent next packet
	uint16_t XDATA *buff;
	uint16_t rate_; // rate in Hz
	uint16_t start_index_; // start index for the Packet
	uint32_t start_time_; // timestamp corresponding to start_index
	uint16_t burst_enabled_; // used to count down packets in a burst
	uint8_t pause_; // when queue/pkt mem is full then pause
	uint32_t pause_timeout_;
	uint32_t burst_timeout_; // timeout for a burst
	uint8_t seq_no_;
}  PACKED ssn_mem_t;



#define SSN_CMD_SET	1
#define SSN_CMD_REQ	2
#define SSN_DATA_V1	3

// 10 ms delay if queue is full
#define SSN_PAUSE_DELAY	1

// SSN Samples Per Packet (35 x 16bit values)
//#define SSN_8BIT_SPP 79
#define SSN_8BIT_SPP 20


//#define FLA_MEM_STRUCT_OFFSET_BUFF	sizeof(void xdata*)
#define SSN_MEM_STRUCT_SIZE			sizeof(ssn_mem_t)

//#define SIG_FILTER_MEM_SIZE	(SIG_FILTER_MEM_STRUCT_SIZE - 1 + 4 * SIG_FILTER_Z_BUFF_SIZE)


// SSN_BUFF_SIZE has to be = 2^n
#define SSN_BUFF_SIZE	(uint16_t)128
#define SSN_BUFF_MASK	(uint16_t)(SSN_BUFF_SIZE-1)
#define SSN_BUFF_ITEM_T	uint16_t

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/



/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/**
* sig_filter_new_data(input) -
*	
*/
//void sig_filter_reset(sig_mem_t xdata *filter);

/**
* sig_filter_new_config(module) -
*	
*/
//void sig_filter_new_config(sig_filter_mem_t xdata *filter, uint8_t * module);

/**
* sig_filter_new_data(input) -
*	
*/
//float sig_filter_new_data( sig_filter_mem_t xdata *filter, float input );


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/

/**
 * ssn_time_offset(start_ticks) -
 *	
 */
uint16_t ssn_time_offset(uint32_t start_ticks);

/**
 * ssn_send_pkt(cmd->pkt_len)
 *	
 */
uint8_t ssn_send_pkt(uint8_t sample_count);

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _FLA_BASIC_PRV_H_