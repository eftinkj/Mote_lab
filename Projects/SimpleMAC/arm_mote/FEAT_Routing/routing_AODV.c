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

//AODV Routing Functions

#include "defs/esnl_pub.h"
#ifdef FEAT_ENABLE_AODV


//Files Included
///////////////////////////////////////////////
#include "packet_AODV.h"
#include "packet.h"
#include "Application.h"
//#include "AT_scripts.h"
//#include "uart_0.h"
#include "FEAT_XBee_API/API_frames.h"
#include "routing_AODV.h"
#include "common.h"
#include "routing.h"
#include "FEAT_Networking/Phy_layer.h"
#include "FEAT_Queuing/Queuing.h"
#include "FEAT_STM32W/STM32W_Radio.h"
///////////////////////////////////////////////

//AODV Global Variables
int16_t XDATA num_routes;
int8_t XDATA route_enable[ROUTES_MAX_AODV];
uint8_t XDATA route_table_destination[ROUTES_MAX_AODV];
uint16_t XDATA route_table_direction[ROUTES_MAX_AODV];
uint8_t XDATA route_table_cost[ROUTES_MAX_AODV];
uint16_t XDATA route_table_index[ROUTES_MAX_AODV];
//uint16_t route_table_source[ROUTES_MAX_AODV] //Adding in Source Memory

uint16_t XDATA my_AODV_index = 1;


uint16_t XDATA last_RREP_src;

#ifdef __SDCC__
//	#define YLED P2_4
//	#define YLED LED1
#endif

#ifdef __KEIL__
	sbit YLED = P2 ^ 4;
#endif

/// FROM MAIN.C

boolean AODV_reply_sent = 0;


///////////////////////////////////////////////
//Routing Functions
///////////////////////////////////////////////

///////////////////////////////////////////////
// Broadcast Request for Route to neighbors
void send_RREQ ( uint16_t index, uint8_t dest_id, uint8_t src_id )
{
	RREQpkt *rreq = (RREQpkt *)packet;
	//Assemble Routing Type Packet with the RREQ Flag
	rreq->start = START_BYTE;
	rreq->flags = ROUTEFLAG_RREQ;
	rreq->mac_dst = MAC_BROADCAST;
	rreq->mac_src = MY_ADDR;
	rreq->index = index;
	rreq->dst_id = dest_id;
	rreq->src_id = src_id;
	rreq->crc = STOP_BYTE;
	//	sendPriorityPacket ( ap->length, (char XDATA*) ap, MAC_BROADCAST );//ap->mac_dst );

	sendPriorityPacket ( RREQ_LEN, ( char* ) packet, rreq->mac_dst );
	if ( AODVcounter_update )
	{
		RREQ_counter++;
	}
	my_energy_ = my_energy_ - RREQ_LEN;

}
///////////////////////////////////////////////

///////////////////////////////////////////////
// Send a RREP in response to being the destination
void send_RREP_to_BS ( uint16_t index, uint8_t dest_id, uint8_t src_id )
{
	RREPpkt *rrep = (RREPpkt *)packet;
	//Assemble Routing Type Packet with the RREP Flag
	rrep->start = START_BYTE;
	rrep->flags = ROUTEFLAG_RREP;
	rrep->mac_dst = dest_id;
	rrep->mac_src = MY_ADDR;
	rrep->index = index;
	rrep->dst_id = dest_id;
	rrep->src_id = src_id;
	rrep->crc = STOP_BYTE;
	sendPriorityPacket ( RREP_LEN, ( char* ) packet, rrep->mac_dst );
}
///////////////////////////////////////////////
///////////////////////////////////////////////
// Send a RREP in response to being the destination
void send_RREP ( uint16_t index, uint8_t dest_id, uint8_t src_id )
{
	RREPpkt *rrep = (RREPpkt *)packet;
	//Assemble Routing Type Packet with the RREP Flag
	int16_t temp_index;
	temp_index = AODV_find_direction ( dest_id );
	if ( -1 == temp_index )
	{
		return;
	}
	rrep->start = START_BYTE;
	rrep->flags = ROUTEFLAG_RREP;
	rrep->mac_dst = route_table_direction[temp_index];
	rrep->mac_src = MY_ADDR;
	rrep->index = index;
	rrep->dst_id = dest_id;
	rrep->src_id = src_id;
	rrep->crc = STOP_BYTE;
	sendPriorityPacket ( RREP_LEN, ( char* ) packet, rrep->mac_dst );
	if ( AODVcounter_update )
	{
		RREP_counter++;
	}
	my_energy_ = my_energy_ -RREP_LEN;
	last_RREP_src = rrep->src_id;
	AODV_reply_sent = 1;
}
///////////////////////////////////////////////

///////////////////////////////////////////////
// Send a RERR in response to being the destination
void send_RERR ( uint16_t hop_id )
{
	//RERRpkt *pkt = (RERRpkt *)p;

	RERRpkt *rerr = (RERRpkt *)packet;
	//Assemble Routing Type Packet with the RREP Flag
	int16_t temp_index;
	temp_index = AODV_find_direction ( hop_id );
	if ( -1 == temp_index )
	{
		return;
	}
	rerr->start = START_BYTE;
	rerr->flags = ROUTEFLAG_RERR;
	rerr->mac_dst = route_table_direction[temp_index];
	rerr->mac_src = MY_ADDR;
	//rerr.index=index;
	rerr->dst_id = hop_id;
	rerr->src_id = MY_ADDR;
	rerr->crc = STOP_BYTE;
	sendPriorityPacket ( RERR_LEN, ( char* ) packet, rerr->mac_dst );
	if ( AODVcounter_update )
	{
		RREP_counter++;
	}
	my_energy_ = my_energy_ - RERR_LEN;

}
///////////////////////////////////////////////

#ifndef CH_START_ROUTE
	///////////////////////////////////////////////
	// Send a RERR in response to being the destination
	void send_RERR2 ( uint8_t dest_id )
	{

		RERRpkt *rerr = (RERRpkt *)packet;
		//Assemble Routing Type Packet with the RREP Flag
		int16_t temp_index;
		temp_index = AODV_find_direction ( dest_id );
		if ( -1 == temp_index )
		{
			return;
		}
		rerr->start = START_BYTE;
		rerr->flags = ROUTEFLAG_RERR;
		rerr->mac_dst = route_table_direction[temp_index];
		rerr->mac_src = MY_ADDR;
		//rerr.index=index;
		rerr->dst_id = dest_id;
		rerr->src_id = MY_ADDR;
		rerr->crc = STOP_BYTE;
		sendPriorityPacket ( RERR_LEN, ( char* ) packet, rerr->mac_dst );
		if ( AODVcounter_update )
		{
			RREP_counter++;
		}
		my_energy_ = my_energy_ - RERR_LEN;

	}
	///////////////////////////////////////////////
#endif

///////////////////////////////////////////////
// Send ACK Packet
void ACK_packet ( uint16_t index, uint8_t dest_id, uint8_t src_id, uint16_t dest )
{
	ACKpkt *ack = (ACKpkt *)packet;
	//Assemble Routing Type Packet with the ACK Flag
	ack->start = START_BYTE;
	ack->flags = ROUTEFLAG_ACK;
	ack->mac_dst = dest;
	ack->mac_src = MY_ADDR;
	ack->index = index;
	ack->dst_id = dest_id;
	ack->src_id = src_id;
	ack->crc = STOP_BYTE;
	sendPriorityPacket ( ACK_LEN, ( char* ) packet, ack->mac_dst );
	if ( AODVcounter_update )
	{
		ACK_AODV_counter++;
	}
	my_energy_ = my_energy_ - ACK_LEN;
}
///////////////////////////////////////////////

///////////////////////////////////////////////

void send_aodv_counters()
{

	CNTpkt *cnt = (CNTpkt *) packet;
	uint8_t temp_power = my_tx_power_;

	cnt->start = START_BYTE;
	cnt->flags = FLAG_COUNTERS;
	cnt->mac_dst = BS_ADDR;
	cnt->mac_src = MY_ADDR;
	cnt->RREQcnt = RREQ_counter;
	cnt->RREPcnt = RREP_counter;
	cnt->ACKcnt = ACK_AODV_counter;
	cnt->DATcnt = DATA_AODV_counter;
	cnt->crc = STOP_BYTE;

	phy_set_power_level ( MAX_POWER_LEVEL );
	sendPriorityPacket ( COUNT_LEN_AODV, ( char* ) packet, BS_ADDR );
	phy_set_power_level ( temp_power );
}
///////////////////////////////////////////////


//Routing Functions

///////////////////////////////////////////////
//
//Function to use the dst_id input to search for a valid direction for a destination

//TODO: Make function return first open routing slot
int16_t AODV_find_direction ( uint8_t dest )
{
	int16_t index;
	for ( index = 0; index < ROUTES_MAX_AODV; index++ )
	{
		//Find the Index of the Destination
		if ( 1 == route_enable[index] )
		{
			if ( dest == route_table_destination[index] )
			{
				return index; //Return the known route index
			}
		}
	}
	return ( int16_t ) - 1;
}
///////////////////////////////////////////////
void routing_init_AODV()
{

	int16_t index;
	RREQ_counter = 0;
	RREP_counter = 0;
	ACK_AODV_counter = 0;
	DATA_AODV_counter = 0;
	AODVcounter_update = 1;
	for ( index = 0; index < ROUTES_MAX_AODV ; index++ )
	{
		route_enable[index] = 0;
		//route_table_source[index]=0;
		route_table_destination[index] = 0;
		route_table_direction  [index] = 0;
		route_table_cost       [index] = 0;
		route_table_index      [index] = 0;
	}
	num_routes = 0;
	my_AODV_index = 1;
	AODV_route_control = 1;

}
///////////////////////////////////////////////

boolean route_table_update_AODV ( uint8_t dest, uint16_t index, uint16_t direction, uint8_t cost )
{

	int16_t index2, index_temp;
	index_temp = AODV_find_direction ( dest );

	if ( num_routes < ROUTES_MAX_AODV )
	{

		if ( ( int16_t ) - 1 == index_temp )
		{

			//Define a new known route
			for ( index2 = 0; index2 < ROUTES_MAX_AODV; index2++ )
			{
				if ( route_enable[index2] == 0 )
				{
					route_enable[index2] = 1;
					route_table_destination[index2] = dest;
					route_table_direction  [index2] = direction;
					route_table_cost       [index2] = cost;
					route_table_index      [index2] = index;
					num_routes++;
					return 1;
				}
			}
			return 0;
		}
		else
		{
			if ( index > route_table_index[index_temp] )
			{
				route_table_destination[index_temp] = dest;
				route_table_direction  [index_temp] = direction;
				route_table_cost       [index_temp] = cost;
				route_table_index      [index_temp] = index;
				return 1;
			}
			else
			{
				return 0;
			}
		}

	}
	return 0;
}
///////////////////////////////////////////////

void start_routing_aodv ( uint8_t dest_id )
{
	send_RREQ ( my_AODV_index++, dest_id, MY_ADDR );
//	route_search_on_=1;
}
///////////////////////////////////////////////




void Routing_AODV ( int8_t *p, uint8_t rssi )
{


	//uint16_t index_temp, dir_index;
	boolean update_bit;
	int8_t tempdst;
	int16_t tempdest_index;



	RREQpkt *pkt = (RREQpkt *)p; // NOTE: this is OK since all routing packets have the same format!!!



	switch ( pkt->flags )
	{
			///////////////////////////////////////////////
		case ROUTEFLAG_RERR:
			tempdst = ( ( pkt_t* ) p )->dst_id;
			if ( tempdst == MY_ADDR )
			{
#ifdef CH_START_ROUTE
				start_routing_aodv ( BS_ADDR );
#endif
				break;
				//recieve_pkt(); //Need to make this function
			}

			tempdest_index = AODV_find_direction ( tempdst );

			if ( tempdest_index == -1 )
			{}
			else
			{
				send_RERR ( tempdst );
			}
			break;
			////////////////////////////////



		case ROUTEFLAG_RREP:


			////////////////////////////////
			//if(MY_ADDR < pkt->mac_src){ //Provide Backward Propogation to the RREPs

			ACK_packet ( pkt->index, pkt->dst_id, pkt->src_id, pkt->mac_src ); //ACK the recipt of this packet

			update_bit = route_table_update_AODV ( pkt->src_id, pkt->index, pkt->mac_src, 1 );

			if ( 0 == update_bit )
			{
				break;
			}
			if ( MY_ADDR == pkt->dst_id )
			{
				//Route is enabled in update above, wait for command to Tx ;
#ifdef CH_START_ROUTE
				send_RREP_to_BS(pkt->index,BS_ADDR,pkt->src_id);
#endif
				break;
			}
			else
			{
				//if(MY_ADDR < pkt->mac_src){
				send_RREP ( pkt->index, pkt->dst_id, pkt->src_id );
				//}


			}
			//}

			break;

			///////////////////////////////////////////////



		case ROUTEFLAG_RREQ:
			ACK_packet ( pkt->index, pkt->dst_id, pkt->src_id, pkt->mac_src );

			//if(MY_ADDR > pkt->mac_src){ //Provide Forward propogation to the RREQs

			update_bit = route_table_update_AODV ( pkt->src_id, pkt->index, pkt->mac_src, 1 );
			//}

			if ( 0 == update_bit )
			{
				break;
			}
			if ( MY_ADDR == pkt->dst_id )
			{
				my_AODV_index = my_AODV_index + 1;
				send_RREP ( my_AODV_index, pkt->src_id, pkt->dst_id );
			}
			else
			{
				if(MY_ADDR != pkt->src_id){
					send_RREQ ( pkt->index, pkt->dst_id, pkt->src_id );
				}
			}


			break;

			///////////////////////////////////////////////




			///////////////////////////////////////////////
			/*
				case ROUTEFLAG_ACK:
					//Used to check for my local neighbors that I can reach
					//Use RSSI value for determining the network neighbors
					//Update the routing table

					if(pkt->mac_src == last_RREP_dest){
						AODV_reply_sent=0;
					}

					break;
			*/
			///////////////////////////////////////////////


			///////////////////////////////////////////////
		case FLAG_DATA:
		case FLAG_DATA_TEST:
			// THERE SHOULD BE NO DATA PACKETS HERE -> handled earlier

			break;
	}
}
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
boolean AODV_send_DATA_base ( uint16_t base )
{
	uint16_t mac_d;
	// SDCC:
	pkt_t * XDATA pkt = ( pkt_t* ) ( &(buffer0[base]) ); //&(QBUFF_ACCESS(base,0));

	uint8_t tempdst = pkt->dst_id;
	//Check if I am the destination, then if not forward towards the destination node
	if ( tempdst == MY_ADDR )
	{
		//recieve_pkt(); //Need to make this function
		return 0;
	}
	else
	{
		//Relay Packet
//		uint8_t *hop_list;
		int16_t tempdest_index;


		TOGGLE_LED(YLED);// = ~YLED;
		tempdest_index = AODV_find_direction ( tempdst );

		if ( ( tempdest_index == -1 ) && ( CLUSTERING_I_AM_CH == my_CH_ ) )
		{
			start_routing_aodv ( BS_ADDR );
			return 0; // unsuccesful
		}
		else
		{
			pkt->mac_dst = route_table_direction[tempdest_index];
			pkt->mac_src = MY_ADDR;
//			sendPriorityPacket ( pkt, pkt->length, route_table_direction[tempdest_index] );
			if ( AODVcounter_update )
			{
				DATA_AODV_counter++;
			}
			my_energy_ = my_energy_ - ( pkt->length + 7 );
			TOGGLE_LED(YLED);// = ~YLED;

		}
	}
	return 1;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * aodv_dropped_link(??) - the link failed (after few retransmissions??) - update routing
  *    and optionally restart route discovery
  */
void aodv_dropped_link(uint16_t hop_id)
{
	if ( CLUSTERING_I_AM_CH == my_CH_ )
	{
		// stop data forwarding
		enableDataTx_ = 0;
#ifndef CH_START_ROUTE
		//start_routing_aodv(BS_ADDR);
		send_RERR ( hop_id );
#else
		// Restart route
		start_routing_aodv ( BS_ADDR );
#endif
	}
}
///////////////////////////////////////////////
///////////////////////////////////////////////
#endif

