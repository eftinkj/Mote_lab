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
#include "routing_OEDSR.h"

#ifdef _ENABLE_OEDSR_

#include "routing.h"
//#include "network_hardware.h"
#include "common.h"
#include "FEAT_Xbee_API/AT_scripts.h"
//#include "uart_0.h"
//#include "API_frames.h"
#include "FEAT_Queuing/Queuing.h"
#include "Application.h"
#include "FEAT_Networking/Phy_layer.h"
#include "FEAT_Scheduler/sch_basic_pub.h"

#include "FEAT_STM32W/STM32W_Radio.h"

// Variables to manage (periodic) timeout
uint8_t oedsr_hello_timeout_enabled_ = 0;
rtc_tick_t oedsr_hello_timeout_ = RTC_OVERFLOW_TIMER_VALUE;

#ifdef _ARM_
#else // _ARM_
	#ifdef __KEIL__
		#ifdef _ENABLE_F1_MOTE_TEST_01_
			sbit YLED = P2 ^ 5;
		#else // _ENABLE_F1_MOTE_TEST_01_
			sbit YLED = P2 ^ 2;
		#endif // else/not _ENABLE_F1_MOTE_TEST_01_
	#endif

	#ifdef __SDCC__
		#ifdef _ENABLE_F1_MOTE_TEST_01_
			#define YLED P2_5
		#else // _ENABLE_F1_MOTE_TEST_01_
			#define YLED P2_2
		#endif // else/not _ENABLE_F1_MOTE_TEST_01_
	#endif // __SDCC__
#endif // else _ARM_
///////////////////////////////////////////////
///////////////////////////////////////////////
uint16_t XDATA r_dst[MAX_ROUTES_OEDSR];
uint16_t XDATA r_next_hop[MAX_ROUTES_OEDSR];
uint8_t XDATA r_metric[MAX_ROUTES_OEDSR];
int8_t XDATA r_last_line;


uint8_t oedsr_state_; // state of the routing agent (e.g. IDLE, routing in-progress)
uint8_t XDATA oedsr_Hello_countdown_;	// ON - periodically send HELLO
int16_t XDATA route_search_BS_;	// address of the target node (BS)
//int16_t xdata route_search_count_;	// count down the interval between HELLO and period for ACK reception
//int8_t xdata route_search_timer_on_; //indicates if the timer should be set

///////////////////////////////////////////////
///////////////////////////////////////////////

uint16_t XDATA ack_temp_addr_[MAX_ACK_TEMP];
uint16_t XDATA ack_temp_link_cost_[MAX_ACK_TEMP];
uint8_t XDATA ack_temp_free_;	// the first free element (and count of used)

// Temp to handle dynamic packets
#define RTR_OEDSR_PKT_SIZE 30
uint8_t XDATA rtr_oedsr_pkt[RTR_OEDSR_PKT_SIZE];

///////////////////////////////////////////////

//Counters for the Routing Energy Analysis
uint8_t XDATA Select_counter;
uint8_t XDATA Hello_counter;
uint8_t XDATA ACK_OEDSR_counter;
uint8_t XDATA DAT_OEDSR_counter;

///////////////////////////////////////////////

void send_OEDSR_counters()
{

	CNTpkt_OEDSR *cnt = ( CNTpkt_OEDSR * ) packet;
	uint8_t temp_power = my_tx_power_;

	cnt->start = START_BYTE;
	cnt->flags = FLAG_COUNTERS;
	cnt->mac_dst = SWAP16((uint16_t)BS_ADDR);
	cnt->mac_src = SWAP16((uint16_t)MY_ADDR);
	cnt->Hellocnt = Hello_counter;
	cnt->Selcnt = Select_counter;
	cnt->ACKcnt = ACK_OEDSR_counter;
	cnt->DATcnt = DAT_OEDSR_counter;
	cnt->BEAMcnt = app_beam_count_;
	cnt->BEACONcnt = app_beacon_count_;
	cnt->crc = STOP_BYTE;

	phy_set_power_level ( MAX_POWER_LEVEL );
#ifdef _ENABLE_XBEE_API_
	api_send_packet16 ( ( int8_t* ) packet, COUNT_LEN_OEDSR+2, BS_ADDR );
#else
	sendPriorityPacket (  COUNT_LEN_OEDSR+2, ( sint8_t* ) packet, BS_ADDR );
#endif // _ENABLE_XBEE_API_

	phy_set_power_level ( temp_power );
}
///////////////////////////////////////////////


///////////////////////////////////////////////
/*void set_distance(uint8_t rssi)
{
	if(0==rssi)
		{
		my_distance_ = DEFAULT_DISTANCE;
		}
	else
		{
		my_distance_ = ALPHA*rssi+(1-ALPHA)*my_distance_;
		}

}*/
///////////////////////////////////////////////



///////////////////////////////////////////////
/**
  * ack_temp_clear - setups up temporary ACKs table
  */
void ack_temp_clear()
{
	ack_temp_free_ = 0;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * ack_temp_add - adds new ACK to the temporary ACKs table
  */
int8_t ack_temp_add ( apkt_t *ap, uint16_t delay )
{
	unsigned long int link_factor;
	uint16_t mac_d;
	// saturate energy level
	uint32_t energy = ap->energy;
	if (ENERGY_MAX_VALUE < energy)
		energy = ENERGY_MAX_VALUE;
	link_factor = ( energy * DELAY_SCALING ) / ( TYPICAL_DELAY_FOR_LINK * ap->distance );
	if ( MAX_ACK_TEMP > ack_temp_free_ )
	{
		mac_d = SWAP16(ap->mac_src);
		ack_temp_addr_[ack_temp_free_] = mac_d;
		ack_temp_link_cost_[ack_temp_free_] = link_factor;
		ack_temp_free_++;
		return 1;
	}
	return 0;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * ack_temp_analyse - performs analysis of the temporary ACKs table
		and selects the best node
  */
uint16_t ack_temp_analyse()
{
	uint8_t i = 0;
	uint16_t best_val = 0;	// any node with equal or higher value is better
	uint16_t result = 0xFFFF;	// by default FFFF indicating lack of route

	while ( ( i < ack_temp_free_ ) && ( best_val <= ack_temp_link_cost_[i] ) )
	{
		best_val = ack_temp_link_cost_[i];
		result = ack_temp_addr_[i];
		i++;
	}
#ifdef DEBUG_OEDSR_ACKS
	{
		//int8_t xdata tmp[35];
		uint8_t* tmp = rtr_oedsr_pkt;
		pkt_t *p = tmp;
		uint16_t *list = & ( p->hop_list );
		p->start = START_BYTE;
		p->flags = FLAG_ACK_DEBUG;
		p->mac_dst = SWAP16(BS_ADDR);
		p->mac_src = SWAP16(MY_ADDR);
		p->length = 17; // of the packet'd data
		p->dst_id = BS_ADDR; // part of DATA field (dest_id) - actual length depends on "length"
		p->src_id = MY_ADDR; // part of DATA field

		p->sequ_num = ack_temp_free_;
		p->numhops = 0xFF;

		for ( i = 0; i < ack_temp_free_; i++ )
		{
			*list = ack_temp_addr_[i];
			list++;
			*list = ack_temp_link_cost_[i];
			list++;
		}
		*list = 0x0000;
		list++;
		*list = 0x0000;
		list++;
#ifdef _ENABLE_XBEE_API_
		api_send_packet16 ( tmp, 27, BS_ADDR );
#else
		sendPriorityPacket ( 27, tmp, BS_ADDR );
#endif
	}
#endif

	return result;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * ack_temp_get_cost - get cost for the selected node
  */
uint16_t ack_temp_get_cost ( uint16_t addr )
{
	uint8_t i = 0;
	while ( ( i < ack_temp_free_ ) && ( addr != ack_temp_addr_[i] ) )
	{
		i++;
	}

	// if not found -> return cost = 0
	if ( ack_temp_free_ == i )
	{
		return 0;
	}

	return ack_temp_link_cost_[i];
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * routing_init - performs initial setup of routing
  */
void routing_init_OEDSR()
{
	r_last_line = -1;
	oedsr_state_ = OEDSR_STATE_IDLE;
	oedsr_Hello_countdown_ = 0;		// (>0) - periodically send HELLO
	route_search_BS_ = 0xFE;	// address of the target node (BS)
//	route_search_count_ = 0;	// count down the interval between HELLO
	// ... and waiting period for ACK reception
	ack_temp_clear();
	AODVcounter_update = 1;
	Select_counter = 0;
	Hello_counter = 0;
	ACK_OEDSR_counter = 0;
	DAT_OEDSR_counter = 0;


	sch_add_loop(( sch_loop_func_t )oedsr_loop );
}

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void oedsr_loop( void )
{
	if (1 == oedsr_hello_timeout_enabled_)
		if ( oedsr_hello_timeout_ < rtc_get_ticks())
		{
			oedsr_hello_timeout_enabled_ = 0;
			oedsr_hello_timeout_ = RTC_OVERFLOW_TIMER_VALUE;
			oedsr_hello_timoeout();
		}

}
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_set_route - adds or updates a route to "dst_id"
  * returns '0' if failed to do so (no free space) or '1' when successful
  */
int16_t oedsr_set_route ( uint16_t dst_id, uint16_t next_hop, uint8_t metric )
{
	int16_t index;
	uint16_t mac_next_hop = next_hop;
// Workaround for small demo-setups
//  -> allows only specific nodes to transmit directlt to the BS
#ifndef BS_NEIGHBOR
	if ( BS_ADDR == dst_id)
	{
		if (BS_ADDR == next_hop)			return 0;
	}
#endif // BS_NEIGHBOR
	if ( 0xFF == dst_id )
	{
		mac_next_hop = 0xffff;
	}

	index = find_route ( dst_id );
	if ( -1 == index )
	{
		r_last_line++;
		index = r_last_line;
		if ( MAX_ROUTES_OEDSR == r_last_line )
		{
			r_last_line--;
			return 0;
		}
	}
	r_dst[index] = dst_id;
	r_next_hop[index] = mac_next_hop;
	r_metric[index] = metric;

	return 1; // successfully added/updated route
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * find_route - searches for a route to a given destination
  * returns index to the route record or '-1' if not found
  */
int16_t find_route ( const uint16_t dst_id )
{
	int16_t i;
	for ( i = 0; i <= r_last_line; i++ )
	{
		if ( dst_id == r_dst[i] )
		{
			return i;
		}
	}
	return -1; // record not found
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * get_next_hop - searches for a route to a given destination
  * returns MAC address of next hop on this route
  * or '0xFFFF' if not found any route
  */
uint16_t get_next_hop ( uint16_t dst_id )
{
	int16_t index = find_route ( dst_id );
	if ( -1 == index )
	{
		// TODO: no route => set the dest_id to be searched, and packet to wait

		return 0xFFFF; // Not found route - try Broadcast ???
	}
	return r_next_hop[index];
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * startRouteSearch - starts the route discovery procedure
  *
  *
  */
void startRouteSearch ( uint16_t dst )
{
	if ( ( 0 < oedsr_Hello_countdown_ ) || ( OEDSR_STATE_IDLE != oedsr_state_ ) )
	{
		// there is one route discovery in progress
		// skip the new one
		// TODO: remember the new request for later
		return;
	}

	// ###############################
	// start route Discovery toward BS
	route_search_BS_ = dst;
	// Clear temporary table for ACK responses
	ack_temp_clear();
	sendHELLO();
	oedsr_Hello_countdown_ = DEFAULT_ROUTE_SEARCH_ON_REPETITIONS;
	oedsr_state_ = OEDSR_STATE_ROUTE_DISCOVERY;
	oedsr_set_tx_timeout ( ROUTE_SEARCH_HELLO_INTERVAL );
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_start_routing - re-starts the route discovery for DST_ID
  *
  *
  */
void oedsr_start_routing( uint16_t dst_id )
{
	if ( OEDSR_STATE_ROUTE_DISCOVERY == oedsr_state_ )
	{
		return;
	}
//	oedsr_set_route ( BS_ADDR, MAC_BROADCAST, 100 );
	oedsr_set_route ( dst_id, MAC_BROADCAST, 100 );
	// stop data forwarding
	enableDataTx_ = 0;
	// start route discovery
	startRouteSearch ( BS_ADDR );
}

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendHELLO - handles a sending of HELLO packet
  * collects all info for packet
  * prepares timestamp
  */
uint8_t sendHELLO ()
{
//	uint8_t XDATA packet[RTR_OEDSR_HELLO_PKT_SIZE];
//	uint8_t XDATA *packet = rtr_oedsr_pkt;
	hpkt_t XDATA *hpkt = ( hpkt_t * ) packet;

	if (0 != (CLUSTERING_I_AM_CH - my_CH_) ) return 0;

	hpkt->start = START_BYTE;
	hpkt->flags = FLAG_HELLO;  // HELLO packet
	hpkt->mac_dst = SWAP16((uint16_t)MAC_BROADCAST); // Broadcast
	hpkt->mac_src = SWAP16((uint16_t)MY_ADDR);
	hpkt->length = RTR_OEDSR_HELLO_PKT_SIZE - PKT_HEADER_LENGTH; // of the packet'd data
	// FILL END-TO-END ADDRESSING
	hpkt->dst_id = route_search_BS_; //BS_ADDR;
	hpkt->src_id = MY_ADDR;
	// FILL THE OEDSR field
	hpkt->distance = my_distance_;
	hpkt->energy = my_energy_;
	// FILL the CHECKSUM
	// TODO: calculate checksum
	hpkt->crc = STOP_BYTE;


	// TRANSMIT the HELLO packet out
#ifdef _ENABLE_XBEE_API_
	//TX_PACKET ( pkt_to_sent, len )
	api_send_packet16( (int8_t*)packet, RTR_OEDSR_HELLO_PKT_SIZE, 0xFFFF);
#else
	sendPriorityPacket ( RTR_OEDSR_HELLO_PKT_SIZE, ( sint8_t* ) packet, 0xFFFF );
#endif // _ENABLE_XBEE_API_
	if ( AODVcounter_update )
	{
		Hello_counter++;
	}
	my_energy_ = my_energy_ -HELLO_LENGTH;
	return 1;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * revcHELLO - handles a received HELLO packet
  * should identify if this node is potential relay node
  * if yes, then send response (ACK?)
  */
uint16_t recvHELLO ( hpkt_t *hp )
{
	// find BS address and find correcponding distace
	if (( hp->dst_id == BS_ADDR) && ( hp->distance <= my_distance_ ))
	{
		return 0;
	}
	// check if closer then sending node

	// prepare response

	// sendACK packet
//	com0_string("HELLO->ACK");
	sendACK ( hp );
	return 1;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendACK - handles a sending of ACK packet
  * collects all info for packet
  * prepares timestamp
  */
uint16_t sendACK ( hpkt_t *hp )
{
	apkt_t *ap = (apkt_t *)packet;
	ap->start = START_BYTE;
	ap->flags = FLAG_ACK;
	ap->mac_dst = hp->mac_src;	// MAC DST
	ap->mac_src = SWAP16((uint16_t)MY_ADDR); 		// MAC SRC

	ap->length = 11;		// 2 x id(1B), 2 x u_long int(4B), 1 x crc (1B)

	ap->dst_id = hp->src_id;
	ap->src_id = MY_ADDR;


	ap->distance = my_distance_;
	ap->energy = my_energy_;
	ap->crc = STOP_BYTE;

//	com0_string("Send ACK");
	if ( AODVcounter_update )
	{
		ACK_OEDSR_counter++;
	}
	my_energy_ = my_energy_ -ACK_LENGTH;
//	return api_send_packet16 ( packet, ACK_LENGTH, 0xFFFF );// hp->mac_src );
#ifdef _ENABLE_XBEE_API_
	return api_send_packet16 ( packet, ACK_LENGTH, SWAP16((uint16_t)hp->mac_src) );
#else // _ENABLE_XBEE_API_
	sendPriorityPacket ( ACK_LENGTH, ( sint8_t* ) packet, SWAP16((uint16_t)hp->mac_src) );
        return 0;
#endif // else _ENABLE_XBEE_API_

}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * revcACK - handles a received ACK packet
  * should identify if this node is potential relay node
  * if yes, then send response (ACK?)
  */
uint16_t recvACK ( sint8_t *pkt )
{
	apkt_t *p = ( apkt_t * ) pkt;
	// check delay from timeout
	// TODO:
	// For now it will be constant
	long int delay = TYPICAL_DELAY_FOR_LINK;

	// analyse ACK packet
	if ((0 < oedsr_Hello_countdown_)&&(SWAP16((uint16_t)(p->mac_src)) == route_search_BS_))
	{
#ifndef BS_NEIGHBOR
		if (BS_ADDR == SWAP16((uint16_t)(p->mac_src)) )
		{
			// Force to "not-hear" BS
			return 0;
		}
#endif

		// found the destination - ?? should it be for "src_id" instead "mac_src"??
		oedsr_set_route ( SWAP16((uint16_t)(p->mac_src)), SWAP16((uint16_t)p->mac_src), 1 ); //metric not important
		oedsr_Hello_countdown_ = 0; 	// Stop since already found final destination
		oedsr_state_ = OEDSR_STATE_IDLE;
		ack_temp_clear();
		enableDataTx_ = 1;
		return 0xff;
	}

#ifndef BS_NEIGHBOR
	if (BS_ADDR == SWAP16((uint16_t)(p->mac_src)) )  { return 0; }
#endif
	oedsr_set_route ( p->src_id, SWAP16((uint16_t)p->mac_src), 1 ); //metric not important

	// store routing info in the temporarly table
	return ack_temp_add ( ( apkt_t * ) pkt, delay );
}
///////////////////////////////////////////////
#ifdef NO_QUEUE
///////////////////////////////////////////////
/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
uint16_t sendDATA ( int8_t *pkt, uint8_t rssi )
{
	uint16_t mac_d;
	pkt_t *pkt2 = ( pkt_t * ) pkt;
	//enableDataTx_=1;

	if ( enableDataTx_ )
	{
		// find destination -- only when taken out of the main loop
		mac_d = get_next_hop ( GET_NEXT_HOP( pkt ) );
		if ( 0xFFFF == mac_d )
		{
			// if the route search returned 0xFFFF then there is no route!!!
			// then start new one and drop the packet
			startRouteSearch ( BS_ADDR );
			return 0;
		}
		// Set MAC header (addresses)
		pkt->mac_dst = SWAP16((uint16_t)mac_d);
		pkt->mac_src = SWAP16((uint16_t)MY_ADDR);

		// send the packet
		if ( AODVcounter_update )
		{
			DAT_OEDSR_counter++;
		}
		my_energy_ = my_energy_ - ( ( pkt_t* ) pkt )->length + 7;



		if ( FLAG_DATA_TEST == pkt2->flags )
		{
			uint8_t *hop_list;

#ifdef CH_START_ROUTE
			pkt2->numhops = 0;

			//uint8_t sequ_num;
			//uint8_t numhops;
			//uint8_t hop_list;
#endif
			hop_list = & ( pkt2->hop_list );
			hop_list[ ( pkt2->numhops ) ++] = MY_ADDR;
			hop_list[ ( pkt2->numhops ) ++] = rssi;
			hop_list[ ( pkt2->numhops ) ++] = my_tx_power_;
		}
#ifdef _ENABLE_XBEE_API_
		api_send_packet16 ( pkt, ( ( pkt_t* ) pkt )->length + 7, mac_d );
#else // _ENABLE_XBEE_API_
	sendPriorityPacket ( ( ( pkt_t* ) pkt )->length + 7, pkt, mac_d );
#endif
	}
	else
	{
		APPEND_LOG ( NODE_ID_STR, NODE_ID_STR_LEN );
		APPEND_LOG ( "DROP DATA\r", 10 );
		// drop the packet
		return 0;
	}
	return 1;

}
///////////////////////////////////////////////
#endif

///////////////////////////////////////////////
/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
int8_t OEDSR_send_DATA_base ( uint16_t base )
{
	uint16_t mac_d;
	// SDCC:
	pkt_t * XDATA pkt = ( pkt_t* ) ( & ( buffer0[ base] ) ); //&(QBUFF_ACCESS(base,0));

//	if (enableDataTx_)
	if ( 1 )
	{
		if ( ((uint8_t)MAC_BROADCAST) == pkt->dst_id )
		{
			// broadcast - set BCAST Mac address
			mac_d = MAC_BROADCAST;
		}
		else
		{
			// find destination -- only when taken out of the main loop
			mac_d = get_next_hop ( pkt->dst_id );
//			mac_d = NEXT_HOP;
			if ( ( 0xFFFF == mac_d ) || ( 0 == enableDataTx_ ) )
			{
				// if the route search returned 0xFFFF then there is no route!!!
				// then start new one and drop the packet
				startRouteSearch ( pkt->dst_id );
				enableDataTx_ = 0; // do not pass date anymore
				// return the packet to queue
//				app_drop_pkt ( base, MODULE_RTR, REASON_NOROUTE, EVENT_DSEND );
				return ROUTING_BEGAN_ROUTE_DISCOVERY;
			}
		}
			// Set MAC header (addresses)
		pkt->mac_dst = SWAP16((uint16_t)mac_d);
		pkt->mac_src = SWAP16((uint16_t)MY_ADDR);

			// send the packet
		if ( AODVcounter_update )
		{
			DAT_OEDSR_counter++;
		}
		// ENERGY USAGE
		{
			unsigned long int energy_delta = ( ( ( pkt_t* ) pkt )->length + 7 );
			my_energy_ = my_energy_ - energy_delta ;
		}
	}
	else
	{
		APPEND_LOG ( NODE_ID_STR, NODE_ID_STR_LEN );
		APPEND_LOG ( "DROP DATA\r", 10 );
		// drop the packet
		return 0;
	}
	return 1;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * recvAcceptData - handles a received ACCEPT DATA packet from BS
  * 1) enables data transmission
  * 2)
  */
uint16_t recvAcceptData ( int8_t *pkt )
{
	pkt_t *p = ( pkt_t * ) pkt;

//	int16_t i;
//	float f = 0.0033;

	// analyse ACK packet
//	if (p->src_id == BS_ADDR)
	if ( 0xFFFF == SWAP16((uint16_t)p->mac_dst) )
	{
		// enable DATA transmission
		enableDataTx_ = 1;


		APPEND_LOG ( NODE_ID_STR, NODE_ID_STR_LEN );
		APPEND_LOG ( "DATA - OK\r\r\r", 12 );

		return 1;
	}

//	api_send_packet16(pkt, ((pkt_t*)pkt)->length + 7, 0x11);

	// wrong trasmitter
	return 0;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendSelect - tells a selected node that it will be a relay node
  * 1)
  * 2)
  */
uint16_t sendSelect ( uint16_t relay, uint8_t bs )
{
	spkt_t *sp = 0;

	// enable DATA formwarding since we found the route
	enableDataTx_ = 1;

	// Create and fill in the SELECT packet
	sp = ( spkt_t * ) packet;

	sp->start = START_BYTE;
	sp->flags = FLAG_SELECT;
	sp->mac_dst = SWAP16((uint16_t)relay);	// MAC DST
	sp->mac_src = SWAP16((uint16_t)MY_ADDR); 		// MAC SRC

	sp->length = 3;		// 2 x id(1B), 2 x int(2B), 1 x crc (1B)

	sp->dst_id = bs;
	sp->src_id = MY_ADDR;

	sp->crc = STOP_BYTE;

	// Send the SELECT packet and log the activity if necessary
	APPEND_LOG ( NODE_ID_STR, NODE_ID_STR_LEN );
	APPEND_LOG ( ( int8_t* ) ( packet[3] ), 1 );
	APPEND_LOG ( "Select sent\r", 12 );
	if ( AODVcounter_update )
	{
		Select_counter++;
	}
	my_energy_ = my_energy_ -SELECT_LENGTH;

#ifdef _ENABLE_XBEE_API_
	if ( relay == 0xFFFF )
	{
		return api_send_packet16 ( packet, SELECT_LENGTH, BS_ADDR );
	}

	return api_send_packet16 ( packet, SELECT_LENGTH, relay );
#else // _ENABLE_XBEE_API_
	if ( relay == 0xFFFF )
	{
		sendPriorityPacket ( SELECT_LENGTH, ( sint8_t* ) packet, BS_ADDR );
		return 0;
	}
	sendPriorityPacket ( SELECT_LENGTH, ( sint8_t* ) packet, relay );
	return 0;
#endif // else _ENABLE_XBEE_API_
	//Count the Select Packets sent
//	return api_send_packet16(packet, SELECT_LENGTH, 0xFFFF);
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * recvSelect - tells this node that it has been selected as a relay node
  * 1) enables data transmission
  * 2)
  */
uint16_t recvSelect ( sint8_t *pkt )
{
	uint8_t dst = (( spkt_t * ) pkt)->dst_id;
	if ((uint8_t)MY_ADDR == dst)
	{
		return 1;
	}
//	startRouteSearch ( BS_ADDR );
	startRouteSearch ( dst );
	return 1;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_dropped_link(??) - the link failed (after few retransmissions??) - update routing
  *    and optionally restart route discovery
  */
void oedsr_dropped_link()
{
	if ( CLUSTERING_I_AM_CH == my_CH_ )
	{
		// delete route
		oedsr_set_route ( BS_ADDR, MAC_BROADCAST, 100 );
		// stop data forwarding
		enableDataTx_ = 0;
		// start route discovery
		startRouteSearch ( BS_ADDR );
	}
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/////////////////////////////////
/**
  * oedsr_hello_timoeout(??) - runs the procedure of periodic sending of Hello packets
  */
void oedsr_hello_timoeout()
{
	// Timeout reached -> is there a route found? or should I retransmit HELLO?
	uint16_t mac_addr = ack_temp_analyse();
	if ( MAC_BROADCAST != mac_addr )
	{
		// finalize routing determination
		// initially analyze ROUTE_TEMP_TABLE ad select the best
		int16_t link_cost = ack_temp_get_cost ( mac_addr );
		oedsr_set_route ( route_search_BS_, mac_addr, link_cost );
		ack_temp_clear();
		sendSelect ( mac_addr, route_search_BS_ );
#ifndef _TEMP_ARM_
		YLED = 0;
#endif // _TEMP_ARM_
		oedsr_state_ = OEDSR_STATE_IDLE;
		oedsr_Hello_countdown_ = 0;
	}
	else
	{
		if ( 0 < oedsr_Hello_countdown_ )
		{
			oedsr_Hello_countdown_ --; // cound down HELLO retransmissions
			// number of retransmission not reached -> resent HELLO
#ifndef _TEMP_ARM_
			YLED = FLIP_BIT(YLED);
#endif // _TEMP_ARM_
			oedsr_set_tx_timeout(ROUTE_SEARCH_HELLO_INTERVAL);
			sendHELLO();
#ifndef _TEMP_ARM_
			YLED = FLIP_BIT(YLED);
#endif // _TEMP_ARM_
		}
		else
		{
			// all Hello messages has been without response -> STOP
			oedsr_Hello_countdown_ = 0; // stop
			oedsr_state_ = OEDSR_STATE_IDLE;
		}
	}
}


///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_set_tx_timeout(??) - set one-time timeout for MMCR hello/tc packets
  */
void oedsr_set_tx_timeout ( uint16_t ms )
{
	oedsr_hello_timeout_ = rtc_get_ticks() + ms;
	oedsr_hello_timeout_enabled_ = 1;
}
///////////////////////////////////////////////
///////////////////////////////////////////////

#endif // _ENABLE_OEDSR_
