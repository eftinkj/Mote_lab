#ifndef _HIO_XBEE_PRV_H_
#define _HIO_XBEE_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"

#include "HW_LIB/RTC.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/

enum
{
	RX_IDLE, RX_RECEIVING_HEADER, RX_RECEIVING_PACKET, RX_FULL
};

#define HIO_STATE_TIMEOUT_RECEIVING_HEADER	2
#define HIO_STATE_TIMEOUT_RECEIVING_PKT		10


/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


extern sint8_t XDATA packet[115]; // packet to be filled before transmission

extern int8_t XDATA rx_packet[115];
extern int8_t XDATA rx_state;

extern int16_t XDATA rx_expected_length; // expected length of the packet being received

extern uint8_t  XDATA tmp;


extern uint8_t request_debug_packet_; // request for sending DEBUG packet
extern uint16_t XDATA request_debug_packet_dst_;

extern rtc_tick_t XDATA hio_state_timeout;
/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/


//unsigned int sendPacket(char *p, unsigned len);
void hio_recv_packet ( sint8_t XDATA *p, sint8_t rssi );


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/



/**
*  hio_xbee_init() - Initi the XBee interface
*/
//void hio_xbee_init();




// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################



#endif // _HIO_XBEE_PRV_H_
