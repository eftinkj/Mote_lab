
/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "FEAT_STM32W/STM32W_Radio.h"

sint8_t XDATA packet[115]; // packet to be filled before transmission

#ifdef ENABLE_HIO_XBEE

#include "hio_xbee_prv.h"
#include "hio_xbee_pub.h"

#include "FEAT_Xbee_API/API_frames.h"
#include "FEAT_Xbee_API/AT_scripts.h"
//#include "packet.h"
#include "FEAT_Networking/Phy_layer.h"
//#include "HW_LIB/uart_0.h"
#include "sys_func.h"

#include "common.h"

#include "FEAT_Scheduler/sch_basic_pub.h"


#ifdef FEAT_ENABLE_CH_SWITCHING
	#include "FEAT_Networking/ChannelSwitching.h"
#endif

//#include "CongestionControl.h"

//#include "FEAT_Routing/routing_OEDSR.h"
#include "FEAT_Routing/routing.h"
#ifdef FEAT_ENABLE_AODV
	#include "FEAT_Routing/routing_AODV.h"
#endif //FEAT_ENABLE_AODV

#include "Application.h"

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

#ifndef _ARM_

  #ifdef __KEIL__
	///////////////////////////////////////////////
	//Reset Resources
	sbit XBEE_RESET_NOT = P2 ^ 5;
	sbit XBEE_DTR_NOT = P2 ^ 6;
	///////////////////////////////////////////////
	sbit YLED = P2 ^ 4;
  #endif

  #ifdef __SDCC__
	///////////////////////////////////////////////
	//Reset Resources
	#define XBEE_RESET_NOT P2_5
	#define XBEE_DTR_NOT P2_6
	///////////////////////////////////////////////
	//LED Resources
// due to F1	#define YLED P2_4
	#define YLED P2_6
  #endif
#endif // no _ARM_

int8_t XDATA rx_packet[115];
int8_t XDATA rx_state = RX_IDLE;

int16_t XDATA rx_expected_length; // expected length of the packet being received

uint8_t XDATA tmp = 0;

uint8_t request_debug_packet_; // request for sending DEBUG packet
uint16_t  XDATA request_debug_packet_dst_;

rtc_tick_t XDATA hio_state_timeout;

uint32_t count_received_ = 0;
uint32_t count_received_xbee_ack_ = 0;

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/



/**
*  hio_xbee_init() - Initi the XBee interface
*/
void hio_xbee_init()
{
	uint8_t i;
	rx_state = RX_IDLE;
	rx_expected_length = 0;
	for ( i = 0;i < 100;i++ )
	{
		rx_packet[i] = 0x45;
	}
	for ( i = 0;i < 100;i++ )
	{
		packet[i] = 0x44;
	}

	sch_add_loop((sch_loop_func_t)hio_xbee_loop);

	// send initial PING to everybody
	request_debug_packet_ = 1;
	request_debug_packet_dst_ = MAC_BROADCAST; //pkt->dst_id;

}


/**
*  hio_xbee_loop() - Main loop of the XBee High-level IO interface
*/
void hio_xbee_loop()
{
	if (TRUE == packetReceived)
	{
		// Update Routing Table ??
		if (rxPacket[11] != 0x03)
		{
#ifdef MMCR_ENABLE_TRUE_NEIGHBORS
			uint16_t src = rxPacket[9]; src = src << 8; src += rxPacket[8];
			rtr_add_neighbor(src, src, 1);
#endif // MMCR_ENABLE_TRUE_NEIGHBORS
			count_received_ ++;
			// Copy packet??
			hio_recv_packet  ( (sint8_t*)& (rxPacket[12]), rxPacketRSSI ); // data ref and RSSI
		}
		else
		{
			// Received XBee ACK????
			count_received_xbee_ack_++;
		}
		// Acknowledge release of RX buffer
		packetReceived = FALSE;
	}

///////////////////////////////////////////////
	// Send "DEBUG" / "ping-pong" packet
	if ( 1 == request_debug_packet_ )
	{
		send_PONG ( request_debug_packet_dst_ );
		request_debug_packet_ = 0;
	}
///////////////////////////////////////////////

}



///////////////////////////////////////////////
//Function for Reception and Sorting of Packet Types
///////////////////////////////////////////////
void hio_recv_packet  ( sint8_t XDATA *p, sint8_t rssi )
{
	pkt_t XDATA *pkt = ( pkt_t XDATA * ) p;
//	YLED = 0;
///////////////////////////////////////////////
	// TODO: for flag in ROUTING RANGE -> call routing handling function
	switch ( pkt->flags )
	{

			///////////////////////////////////////////////
			//Process STARTData Packet
		case FLAG_STARTD:
			routing_data_request ( p );
			app_recvStartData ( ( pkt_t* ) p );
			// PING packet handling -> respond with PONG
			request_debug_packet_ = 1;
			request_debug_packet_dst_ = BS_ADDR;
			break;
			///////////////////////////////////////////////
		case FLAG_STOP_DATA:
			// TODO: Handling of STOP DATA GENERATION/SENDING
			recvStopData ( p );
			app_recvStopData ( ( pkt_t * ) p );
			//	app_suspendADC();
			break;
			///////////////////////////////////////////////
		case FLAG_CONFIG_NODE:
			// TODO: Handling of CONFIGURATION message:
			//		  - read and apply config (e.g. set routing protocol to use next time)
			//		  - forward the config info (multihop case)
//			recvConfigSensor(p);
			NodeConfigure ( p );
			break;
			///////////////////////////////////////////////
			///////////////////////////////////////////////
		case FLAG_NEW_CONFIG_NODE:
			// TODO: Handling of NEW CONFIGURATION message:
			//		  - read and apply config (e.g. set routing protocol to use next time)
			//		  - forward the config info (multihop case)
			app_recvConfigSensor ( ( pkt_t * ) p , rssi );
			break;
			///////////////////////////////////////////////
		case FLAG_START_SEARCH_CHANNEL:
			// TODO: Handling of NEW CONFIGURATION message:
			//		  - read and apply config (e.g. set routing protocol to use next time)
			//		  - forward the config info (multihop case)
			app_start_search_channel ( ( pkt_t * ) p );
			break;
			///////////////////////////////////////////////
#ifdef FEAT_ENABLE_CH_SWITCHING
		case FLAG_CHANNEL_SWITCHING_V2:
			// TODO: Handling of NEW CONFIGURATION message:
			//		  -
			//		  -
			{
				pkt_v2_t * p2 = ( pkt_v2_t * ) p;
				pkt_mod_header_t * mh = (pkt_mod_header_t *)(&( p2->data_modules ));
				cs_recv_switching_request ( &(mh->content) );
			}
			break;
#endif // FEAT_ENABLE_CH_SWITCHING
			///////////////////////////////////////////////
		case FLAG_BEAM:
			if ( SEARCH_OFF != network_search_mode_ )
			{
				app_recvBEAM ( ( char * ) p );
				routing_send_counters();
			}
			else
			{
				app_recvBEAM ( ( char * ) p );
				routing_send_counters();
			}
			rtr_start_routing(BS_ADDR);
			routing_receive_packet( p, rssi);
			break;
			///////////////////////////////////////////////
		case FLAG_BEACON:
			if ( SEARCH_OFF != network_search_mode_ )
			{
				app_received_beacon ( ( pkt_t * ) p );
			}
			else
			{
				app_received_beacon ( ( pkt_t * ) p );
			}
			break;
			///////////////////////////////////////////////
		case FLAG_RESET_SW:
			if ( ( 0xFFFF == ENDIAN16(pkt->mac_dst) ) && ( 0x5253 == ENDIAN16(pkt->mac_src) ) && ( 0x54 == pkt->length ) )
			{
				sys_reset();

// IS IT NEEDED OR USED AT ALL?? since after reset the node reinitializes everything
				rtc_ticker = 0;
				sequence_no_ = 0;
//				RLED = 1;
//				GLED = 1;
				SET_LED(YLED);// = 1;
				//app_suspendADC();
			}
			break;
			///////////////////////////////////////////////
		case FLAG_PING:
			// PING packet handling -> respond with PONG
			request_debug_packet_ = 1;
			request_debug_packet_dst_ = BS_ADDR; //pkt->dst_id;
			SET_LED(YLED);// = 1;
			break;
			///////////////////////////////////////////////
		case FLAG_SEND_COUNTERS:
			//Packet to Start the Relay of the Counter Information
			routing_send_counters();
			break;
			///////////////////////////////////////////////
		case FLAG_STOP_COUNTERS:
			//Disable Packet Counters
			AODVcounter_update = 0;
			break;
			///////////////////////////////////////////////
		case FLAG_DISABLE_NODE:
			disable_node ( 0x99 );
			break;
			///////////////////////////////////////////////
		case FLAG_ENABLE_NODE:
			enable_node ( p );
			break;

#ifdef FEAT_ENABLE_CC
		case FLAG_PCC_V2:
			if (PHY_BACKOFF_NO_PCC != my_backoff_)
			{
				cc_recv_PCC_upstream ( p );
			}
			break;
#endif
			///////////////////////////////////////////////
		default:
			if ( ( SEARCH_OEDSR_HELLO == network_search_mode_ ) && ( FLAG_ACK == pkt->flags ) )
			{
				// received ACK in the
				app_send_join ( BS_ADDR, my_rf_channel_, network_search_mode_ );
			}
			if ( 0 == energy_depleted_control_ )
			{
				routing_receive_packet ( p, rssi );
			}
			break;
	}
}



void send_PONG ( uint16_t mac_dst )
{
	pkt_v2_t *p = (pkt_v2_t *)packet;
	pkt_mod_header_t *pm = (pkt_mod_header_t *)&(p->data_modules);
	pkt_mod_adfs_t *p_adfs = ( pkt_mod_adfs_t* ) &(pm->content );
	p->start = START_BYTE;
	p->flag = FLAG_DATA_TEST_V2;
	p->mac_dst = ENDIAN16((uint16_t)mac_dst);
	p->mac_src = ENDIAN16(MY_ADDR);
	p->length = NEW_PONG_LEN - PKT_HEADER_LENGTH; // of the packet'd data
	p->dst_id = mac_dst; // part of DATA field (dest_id) - actual length depends on "length"
	p->src_id = MY_ADDR; // part of DATA field

	pm->module_type = PKT_MODULE_TYPE_PONG; // The same as ADFS
	pm->module_length = PKT_MODULE_LEN_ADFS;

	p_adfs->weight = 0xFF;
	p_adfs ->ticker = ENDIAN32(rtc_get_rticks());

	packet[NEW_PONG_LEN-1] = PKT_MODULE_TYPE_END;
#ifndef NO_UART_OUT
	printf("Sending PONG packet to %d (0x%x)\r\n", mac_dst, mac_dst);
#endif
#ifndef _ARM_
//	TX_PACKET( packet, NEW_PONG_LEN);
//	api_send_packet16( p, l, mac_dest);
	api_send_packet16( packet, NEW_PONG_LEN, MAC_BROADCAST);
#else // not _ARM_
	sendPriorityPacket( NEW_PONG_LEN, (sint8_t*)packet, MAC_BROADCAST);
#endif // else not _ARM_
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#endif // ENABLE_HIO_XBEE
