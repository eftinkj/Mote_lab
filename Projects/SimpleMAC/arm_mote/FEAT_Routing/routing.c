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


#include "defs/esnl_pub.h"

#ifdef _ENABLE_ROUTING_

#include "FEAT_STM32W/STM32W_Radio.h"

#include "common.h"

#include "routing.h"

#include "routing_AODV.h"
#include "routing_OEDSR.h"
#include "routing_MMCR.h"

#include "FEAT_Queuing/Queuing.h"
#include "FEAT_Networking/Phy_layer.h"
#include "FEAT_Networking/tsp_common_pub.h"

#include "Application.h"
#ifdef FEAT_ENABLE_CC
	#include "congestionControl.h"
#endif


///////////////////////////////////////////////
unsigned char XDATA AODV_control_bit;
unsigned char XDATA OEDSR_control_bit;

uint8_t AODV_route_control;
uint8_t AODVcounter_update;

uint8_t data_forward_control_;

unsigned char XDATA RREQ_counter;
unsigned char XDATA RREP_counter;
unsigned char XDATA ACK_AODV_counter;
unsigned char XDATA DATA_AODV_counter;
///////////////////////////////////////////////
uint8_t can_start_on_beam = 1;  // node can start route discovery after the first (only) BEAM

///////////////////////////////////////////////
#ifndef _ARM_
	#ifdef __KEIL__
		sbit YLED = P2 ^ 4;
	#else
		#define YLED P2_4
	#endif
#endif // not _ARM_
///////////////////////////////////////////////


void recvStopData ( sint8_t XDATA *p )
{
	data_forward_control_ = 0;
}

///////////////////////////////////////////////
void routing_init()
{
	data_forward_control_ = 1;
	my_protocol_ = ROUTING_PROTOCOL_MMCR;//ROUTING_PROTOCOL_OEDSR;
	routing_set_routing_protocol();
}

///////////////////////////////////////////////

void routing_set_routing_protocol() // my_protocol_ has correct value
{
#ifdef FEAT_ENABLE_AODV
	if ( ROUTING_PROTOCOL_AODV == my_protocol_ )
	{
		AODV_control_bit = 1;
		OEDSR_control_bit = 0;
		routing_init_AODV();
	}
	else
#endif
#ifdef _ENABLE_OEDSR_
	if ( ROUTING_PROTOCOL_OEDSR == my_protocol_ )
	{
		AODV_control_bit = 0;
		OEDSR_control_bit = 1;
		routing_init_OEDSR();
	}
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_MMCR
	if ( ROUTING_PROTOCOL_MMCR == my_protocol_ )
	{
		AODV_control_bit = 0;
		OEDSR_control_bit = 0;
		//MMCR_control_bit = 1;
		routing_init_MPR_select();
		routing_init_MMCR();
	}
#endif // FEAT_ENABLE_MMCR
}

//////////////////////////////////////////////////////////////////////////////////////////////
void routing_receive_packet ( sint8_t XDATA *p, sint8_t rssi )
{

	pkt_t XDATA *pkt = ( pkt_t XDATA * ) p;

	// ######################################################3
	// ### Receive packet to myself or broadcast
	// ######################################################3
	if ((MY_ADDR == pkt->dst_id)||(0xFF == pkt->dst_id))
	{
		if (TSP_HANDLED_PACKET == tsp_receive( (uint8_t*)p, rssi ))
		{
			return;
		}
	}

	// ######################################################3
	// ### Start routing when received BEAM message
	// ######################################################3
	if ( FLAG_BEAM == pkt->flags )
	{
		app_recvBEAM ( p );

#ifdef CH_START_ROUTE

	#ifdef _ENABLE_OEDSR_
		/*
		if (ROUTING_PROTOCOL_OEDSR == my_protocol_)
		{
			if (dist_approx_on_){ set_distance(rssi); }
			else { set_distance(0); }
		}*/
	#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_AODV
		// for AODV which has already the route -> skip
		if (( ROUTING_PROTOCOL_AODV == my_protocol_)
			&& ( -1 != AODV_find_direction ( BS_ADDR ) ) )
		{ // DO NOTHING
		}
		else
#endif // FEAT_ENABLE_AODV
		{
			//  FOR ANYTHING ELSE start route Discovery toward BS
			if ( CLUSTERING_I_AM_CH == my_CH_ )
				rtr_start_routing ( BS_ADDR );
		}
#endif // CH_START_ROUTE
	} // if FLAG_BEAM

	// ######################################################
	// ### Receive packet if above minimal threshold
	// ######################################################
	// FILTERING of messages below certain RSSI threshold (!! values are absolute)
	if ( rssi >= my_rssi_threshold_ )
	{
		return;
	}
	else
	{
		// ######################################################
		// ### Handle routing packets and forwarding
		// ######################################################
		switch ( pkt->flags )
		{
				///////////////////////////////////////////////
#ifdef _ENABLE_OEDSR_
			case FLAG_HELLO:
				if ( ROUTING_PROTOCOL_OEDSR == my_protocol_ )
				{
					recvHELLO ( ( hpkt_t XDATA * ) p );
				}
				break;
#endif // _ENABLE_OEDSR_
				///////////////////////////////////////////////
#ifdef _ENABLE_OEDSR_
			case FLAG_ACK:
				if ( ROUTING_PROTOCOL_OEDSR == my_protocol_ )
				{
					// check if should accept ACK
					if ( OEDSR_STATE_IDLE != oedsr_state_ )
					{
						if ( 0xFF == recvACK ( p ) )
						{	// if BS found as next hop
//							RLED = 0;
						}
					}
				}
				break;
#endif // _ENABLE_OEDSR_
				///////////////////////////////////////////////
//#ifdef _ENABLE_OEDSR_
			case FLAG_SELECT:
#ifdef _ENABLE_OEDSR_
				if ( ROUTING_PROTOCOL_OEDSR == my_protocol_ )
				{
					recvSelect ( p );
				}
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_MMCR
				if ( ROUTING_PROTOCOL_MMCR == my_protocol_ )
				{
//					mmcr_recvSelect ( p );
				}
#endif // FEAT_ENABLE_MMCR
				break;

// THE BLOCK BELOW SHOULD NOT BE THERE!!!
#ifdef FEAT_ENABLE_MMCR
			case FLAG_HELLO_MMCR:
			case FLAG_ACK_MMCR:
			case FLAG_TC_MMCR:
			case FLAG_CHSW_MMCR:
				if ( ROUTING_PROTOCOL_MMCR == my_protocol_ )
				{
					mmcr_recv_routing_msg ( (pkt_t XDATA *)p );
				}
				break;
#endif // FEAT_ENABLE_MMCR
				///////////////////////////////////////////////
				
				///////////////////////////////////////////////
				//Process Data Packet

			case FLAG_DATA:
			case FLAG_DATA_TEST:
			case FLAG_DATA_ARB:
			case FLAG_DATA_TEST_V2:
			case FLAG_DATA_MODULAR:
#ifdef FEAT_ENABLE_CC
				cc_recv_pkts_ ++; // Count received packets
#endif
#ifdef _ENABLE_OEDSR_
				if ( data_forward_control_ )
				{
//					YLED = ( ( ~YLED ) && recvDATA ( p, rssi ) );
					recvDATA ( p, rssi ) ;
				}
#endif // _ENABLE_OEDSR_
				break;

				///////////////////////////////////////////////
#ifdef FEAT_ENABLE_AODV
			default:
				if ( 1 == AODV_control_bit )
				{
					//if(AODV_route_control){
					Routing_AODV ( p, rssi );
					//}
				}
				break;
#endif

		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////


// ##############################################################
void routing_data_request ( sint8_t XDATA *p )
{
	data_forward_control_ = 1;

	switch ( my_protocol_ )
	{
#ifdef _ENABLE_OEDSR_
		case ROUTING_PROTOCOL_OEDSR:
			// TODO: Handling of START DATA GENERATION and/or FORWARDING message:
			//		  - start data generation
			//		  - forward the message (multihop case)
			recvAcceptData ( p );
			break;
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_AODV
		case ROUTING_PROTOCOL_AODV:
			AODV_route_control = 0;
			break;
#endif

#ifdef FEAT_ENABLE_MMCR
		case ROUTING_PROTOCOL_MMCR:
			// TODO: Handling of START DATA GENERATION and/or FORWARDING message:
			//		  - start data generation
			//		  - forward the message (multihop case)
			mmcr_recvAcceptData ( p );
			break;
#endif // FEAT_ENABLE_MMCR
		default:
			// ERROR
			break;
	}
}

// ##############################################################

int8_t  routing_send_DATA_base ( uint16_t base )
{
	int8_t temp = 0xEE;
	if ( CLUSTERING_I_AM_CH != my_CH_ )
	{
		return routing_to_CH_base ( base );
	}

	update_pkt_send ( base );

	switch ( my_protocol_ )
	{
#ifdef _ENABLE_OEDSR_
		case ROUTING_PROTOCOL_OEDSR:
			temp = OEDSR_send_DATA_base ( base );
			break;
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_AODV
		case ROUTING_PROTOCOL_AODV:
			temp = AODV_send_DATA_base ( base );
			break;
#endif // FEAT_ENABLE_AODV
#ifdef FEAT_ENABLE_MMCR
		case ROUTING_PROTOCOL_MMCR:
			temp = MMCR_send_DATA_base ( base );
			break;
#endif // FEAT_ENABLE_MMCR
		default:
			// ERROR
		  	temp = 0;
			break;
	}
	return temp;
	//return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * recvDATA - handles a received DATA packet
  * 1) check if final destination
  * 2) passes pakcet to sendDATA
  */
uint8_t recvDATA ( sint8_t XDATA *pkt, sint8_t rssi )
{
//	return sendDATA(pkt,rssi);
//	return 1;

	uint8_t packet_id;
	uint8_t packet_len;

	packet_id = que_get_empty_buffer_location();
	if ( 0xFF == packet_id )
	{
		// Error with packet buffer allocation!!
		// TODO: Handle the lack of space for new packet
		app_drop_pkt ( 0xFFFF, MODULE_RTR, REASON_MEMORY_FULL, EVENT_DRECV );
		return 0;
	}
	else
	{
		uint8_t i;
		pkt_t *pkt_temp = ( pkt_t* ) pkt;
		// Fill the TEST PACKET with hop information
		if ( FLAG_DATA_TEST == pkt_temp->flags )
		{
			uint8_t XDATA *hop_list;

#ifdef CH_START_ROUTE
#ifndef SOURCE
			pkt_temp->numhops = 0;
#endif
			//unsigned char sequ_num;
			//unsigned char numhops;
			//unsigned char hop_list;
#endif
			hop_list = (uint8_t XDATA *)& ( pkt_temp->hop_list );
			if ( MAX_HOPLIST_INDEX > pkt_temp->numhops + 3 )
			{
				hop_list[ ( pkt_temp->numhops ) ++] = MY_ADDR;
				hop_list[ ( pkt_temp->numhops ) ++] = rssi;
				hop_list[ ( pkt_temp->numhops ) ++] = my_tx_power_;
			}
		}

		else if ( FLAG_DATA_TEST_V2 == pkt_temp->flags )
		{
			pkt_v2_t XDATA *pkt_v2 = ( pkt_v2_t XDATA * ) pkt;
			pkt_mod_header_t XDATA *ph = ( pkt_mod_header_t XDATA * ) ( & ( pkt_v2->data_modules ) );
			//ph->module_type = PKT_MODULE_TYPE_HOPLIST;
			uint8_t mod_len;
			while ( PKT_MODULE_TYPE_END != ph->module_type )
			{
				mod_len = ph->module_length;
				switch ( ph->module_type )
				{
					case PKT_MODULE_TYPE_ADFS:
#ifdef FEAT_ENABLE_SFQ
						{
							/*							pkt_mod_header_t *ph = (pkt_mod_header_t *)(&(msg[msg_i]));
														pkt_mod_adfs_t *p_adfs = (pkt_mod_adfs_t*)(&msg[msg_i + PKT_MODULE_LEN_HEADER]);

														ph->module_type = PKT_MODULE_TYPE_ADFS;
														ph->module_length = PKT_MODULE_LEN_ADFS;

														p_adfs->weight = my_source_weight_;
														p_adfs ->ticker = rtc_get_rticks();

														len = len + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_ADFS;
														msg_i = msg_i + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_ADFS;
							*/
						}
#endif // FEAT_ENABLE_SFQ
						break;
					case PKT_MODULE_TYPE_HOPLIST:
						update_hoplist_module_recv ( & ( ph->content ), rssi );
						break;
					case PKT_MODULE_TYPE_DUMMY:
						break;
				}
				ph = ( ( pkt_mod_header_t XDATA * ) ( ( ( char XDATA * ) ( ph ) ) + mod_len + PKT_MODULE_LEN_HEADER ) );
			} //while
		}



		// copy packet ...
		packet_len = GET_PKT_LEN ( pkt );
//		strncpy(&(buffer0[QBUFF_BASE(packet_id)]), pkt, packet_len);
		for ( i = 0 ; i < packet_len; i++ )
			buffer0[ ( packet_id * SIZE_Q_BUFF ) +i] = pkt[i];

		// Enqueue packet
		if ( 0 == que_enQpacket ( packet_id ) )
		{
			// Failed -> free the packet
			app_drop_pkt ( packet_id, MODULE_RTR, REASON_QUEUE_FULL, EVENT_DRECV );
			que_make_empty_buffer ( packet_id );
			// Handle the lack of space in queue
			return 0;
		}
	}
	return 1;
}
///////////////////////////////////////////////

// ##############################################################

void routing_send_counters()
{
	switch ( my_protocol_ )
	{
#ifdef _ENABLE_OEDSR_
		case ROUTING_PROTOCOL_OEDSR:
			//send_OEDSR_counters();
			break;
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_AODV
		case ROUTING_PROTOCOL_AODV:
			send_aodv_counters();
			break;
#endif

#ifdef FEAT_ENABLE_MMCR
		case ROUTING_PROTOCOL_MMCR:
//			send_MMCR_counters();
			break;
#endif // FEAT_ENABLE_MMCR


		default:
			// ERROR
			break;
	}
}

// ##############################################################


///////////////////////////////////////////////
/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
int8_t routing_to_CH_base ( uint16_t base )
{
	// SDCC:
	pkt_t * XDATA pkt = ( pkt_t XDATA * ) ( & ( buffer0[ base] ) ); //&(QBUFF_ACCESS(base,0));

//	if (enableDataTx_)
	if ( 1 )
	{
		// find destination -- only when taken out of the main loop
		pkt->mac_dst = SWAP16((uint16_t)my_CH_);
		pkt->mac_src = SWAP16((uint16_t)MY_ADDR);

		// send the packet
		if ( AODVcounter_update )
		{
			DATA_AODV_counter++;
#ifdef _ENABLE_OEDSR_
			DAT_OEDSR_counter++;
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_MMCR
			///mmcr_DAT_MMCR_counter++;
#endif // FEAT_ENABLE_MMCR
		}
		// ENERGY USAGE
		{
			uint32_t energy_delta = ( ( ( pkt_t* ) pkt )->length + 7 );
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

/**
  * update_hoplist_module_recv(hoplist, rssi) - updates hoplist
  *  when receiving a packet (node + rssi)
  */
void update_hoplist_module_recv ( uint8_t XDATA *hoplist, uint8_t rssi )
{
	pkt_mod_hoplist_t XDATA *p_hop = ( pkt_mod_hoplist_t XDATA * ) ( hoplist );

	switch ( p_hop->hoplist_type )
	{
		case HOPLIST_TYPE_V1:
#ifdef GENERATE_HOPLIST_VER_1
			if ( HOPLIST_TYPE_V1 == )
			{
				unsigned char *hop_list;
				unsigned char &seq = msg[msg_i+PKT_MODULE_LEN_HEADER];
				unsigned char &type = msg[msg_i+PKT_MODULE_LEN_HEADER+1];
				unsigned char &numhops = msg[msg_i+PKT_MODULE_LEN_HEADER+2];
				hop_list = msg[msg_i+PKT_MODULE_LEN_HEADER+3];
				seq = sequence_no_;
				type =
					if ( MAX_HOPLIST_INDEX > numhops + 3 )
					{
						hop_list[ numhops ++] = MY_ADDR;
						hop_list[ numhops ++] = 20;
						hop_list[ numhops ++] = my_tx_power_;
					}
					len = len + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_HOPLIST;
				msg_i = msg_i + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_HOPLIST;
			}
			else // == HOPLIST_TYPE_CC
#endif
				break;
		case HOPLIST_TYPE_CC:
#ifdef FEAT_ENABLE_CC
			{
				unsigned char *hop_list;
				hop_list = & ( p_hop->hop_list );
				if ( MAX_HOPLIST_COUNT >= p_hop->numhops + HOPLIST_TYPE_CC_NODE_STRUCTURE_SIZE )
				{
					hop_list[ ( p_hop->numhops ) ] = MY_ADDR;
					hop_list[ ( p_hop->numhops ) +1] = rssi;
//					hop_list[ (p_hop->numhops) +2] = my_tx_power_;
//					hop_list[ (p_hop->numhops) +3] = cc_throughput_;
//					hop_list[ (p_hop->numhops) +4] = cc_throughput_;
//					hop_list[ (p_hop->numhops) +5] = cc_inflow_limit_;
//					hop_list[ (p_hop->numhops) +6] = cc_droprate_;
//					hop_list[ (p_hop->numhops) +7] = que_buffer_empty_slots_end_;
					p_hop->numhops += HOPLIST_TYPE_CC_NODE_STRUCTURE_SIZE;
				}
			}
#endif
			break;
		default:
			break;
	}
}



/**
  * update_hoplist_module_send(hoplist) - updates hoplist
  *  when sending a packet (tx power + statistics)
  */
void update_hoplist_module_send ( uint8_t XDATA *hoplist )
{
	pkt_mod_hoplist_t XDATA *p_hop = ( pkt_mod_hoplist_t XDATA * ) ( hoplist );
	switch ( p_hop->hoplist_type )
	{
		case HOPLIST_TYPE_V1:
#ifdef GENERATE_HOPLIST_VER_1
			if ( HOPLIST_TYPE_V1 == )
			{
				unsigned char XDATA *hop_list;
				unsigned char &seq = msg[msg_i+PKT_MODULE_LEN_HEADER];
				unsigned char &type = msg[msg_i+PKT_MODULE_LEN_HEADER+1];
				unsigned char &numhops = msg[msg_i+PKT_MODULE_LEN_HEADER+2];
				hop_list = msg[msg_i+PKT_MODULE_LEN_HEADER+3];
				seq = sequence_no_;
				type =
					if ( MAX_HOPLIST_INDEX > numhops + 3 )
					{
						hop_list[ numhops ++] = MY_ADDR;
						hop_list[ numhops ++] = 20;
						hop_list[ numhops ++] = my_tx_power_;
					}
					len = len + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_HOPLIST;
				msg_i = msg_i + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_HOPLIST;
			}
			else // == HOPLIST_TYPE_CC
#endif
				break;
		case HOPLIST_TYPE_CC:
#ifdef FEAT_ENABLE_CC
			{
				uint8_t XDATA *hop_list;
				hop_list = & ( p_hop->hop_list );
//				hop_list[ (p_hop->numhops) -8] = MY_ADDR;
//				hop_list[ (p_hop->numhops) -7] = 20;
				hop_list[ ( p_hop->numhops ) -6] = my_tx_power_;
				hop_list[ ( p_hop->numhops ) -5] = cc_throughput_;
				hop_list[ ( p_hop->numhops ) -4] = cc_predicted_throughput_;

//				hop_list[ (p_hop->numhops) -3] = (int8_t)(10*cc_flow_alpha_);
				hop_list[ ( p_hop->numhops ) -3] = cc_calculated_inflow_limit_;

				hop_list[ ( p_hop->numhops ) -2] = cc_droprate_;

				hop_list[ ( p_hop->numhops ) -1] = NUMQBUFFS - que_buffer_empty_slots_end_;
//				hop_list[ (p_hop->numhops) -1] = que_buffer_empty_slots_end_;
//				hop_list[ (p_hop->numhops) -1] = (uint8_t)(0x30+cc_flow_error_);

				hop_list[ ( p_hop->numhops ) +1] = cc_upstream_nodes_status_[0];
				hop_list[ ( p_hop->numhops ) +2] = cc_upstream_nodes_status_[1];
			}
#endif
			break;
		default:
			break;
	}
}






/**
  * update_pkt_send(base) - updates packet content when sending a packet
  *  (hoplist and othe modules)
  */
void update_pkt_send ( uint16_t base )
{
	pkt_v2_t XDATA *pkt = ( pkt_v2_t XDATA * ) ( & ( buffer0[base] ) ); //&(QBUFF_ACCESS(base,0));

	if ( FLAG_DATA_TEST_V2 == pkt->flag )
	{
		pkt_mod_header_t XDATA *ph = ( pkt_mod_header_t XDATA * ) ( & ( pkt->data_modules ) );
		//ph->module_type = PKT_MODULE_TYPE_HOPLIST;
		uint8_t mod_len = ph->module_length ;
		while ( PKT_MODULE_TYPE_END != ph->module_type )
		{
			switch ( ph->module_type )
			{
				case PKT_MODULE_TYPE_ADFS:
#ifdef FEAT_ENABLE_SFQ
					{
						/*							pkt_mod_header_t *ph = (pkt_mod_header_t *)(&(msg[msg_i]));
												pkt_mod_adfs_t *p_adfs = (pkt_mod_adfs_t*)(&msg[msg_i + PKT_MODULE_LEN_HEADER]);

												ph->module_type = PKT_MODULE_TYPE_ADFS;
												ph->module_length = PKT_MODULE_LEN_ADFS;

												p_adfs->weight = my_source_weight_;
												p_adfs ->ticker = rtc_get_rticks();

												len = len + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_ADFS;
												msg_i = msg_i + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_ADFS;
						*/
					}
#endif // FEAT_ENABLE_SFQ
					break;
				case PKT_MODULE_TYPE_HOPLIST:
					update_hoplist_module_send ( & ( ph->content ) );
					break;
				case PKT_MODULE_TYPE_DUMMY:
					break;
			}
			ph = ( ( pkt_mod_header_t XDATA * ) ( ( ( char XDATA * ) ( ph ) ) + mod_len + PKT_MODULE_LEN_HEADER ) );
		} //while
	}
}


/**
  * rtr_dropped_link(??) - the link failed (after few retransmissions??) - update routing
  *    and optionally restart route discovery
  */
void rtr_dropped_link(uint16_t hop_id)
{
	switch ( my_protocol_ )
	{
#ifdef _ENABLE_OEDSR_
		case ROUTING_PROTOCOL_OEDSR:
			oedsr_dropped_link();
			break;
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_AODV
		case ROUTING_PROTOCOL_AODV:
			aodv_dropped_link(hop_id);
			break;
#endif
#ifdef FEAT_ENABLE_MMCR
		case ROUTING_PROTOCOL_MMCR:
			mmcr_dropped_link(hop_id);
			break;
#endif
		default:
			// ERROR
			break;
	}
}


/**
  * rtr_add_neighbor(DST, NEXT_HOP, METRIC) - update routing for one-hop neighbor
  */
void rtr_add_neighbor(uint16_t dst_id, uint16_t next_hop, uint8_t metric)
{
#ifndef BS_NEIGHBOR
	if (BS_ADDR == next_hop)
	{
		return; // skip if not a neigbor of BS
	}
#endif //
	switch ( my_protocol_ )
	{
#ifdef _ENABLE_OEDSR_
		case ROUTING_PROTOCOL_OEDSR:
			oedsr_set_route(dst_id, next_hop, metric);
			break;
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_AODV
		case ROUTING_PROTOCOL_AODV:
#warning AODV - Route update has incorrect index (age of the routing info)
			route_table_update_AODV(dst_id, 0, next_hop, metric);
			break;
#endif
#ifdef FEAT_ENABLE_MMCR
		case ROUTING_PROTOCOL_MMCR:
			mmcr_set_route(dst_id, next_hop, metric);
			break;
#endif
		default:
			// ERROR
			break;
	}
}

/**
  * rtr_start_routing(DST_ID) - start a route discovery for "DST_ID"
  */
void rtr_start_routing(uint16_t dst_id)
{
	switch ( my_protocol_ )
	{
#ifdef _ENABLE_OEDSR_
		case ROUTING_PROTOCOL_OEDSR:
			oedsr_start_routing(dst_id);
			break;
#endif // _ENABLE_OEDSR_
#ifdef FEAT_ENABLE_AODV
		case ROUTING_PROTOCOL_AODV:
//			aodv_start_routing(dst_id);
			break;
#endif
#ifdef FEAT_ENABLE_MMCR
		case ROUTING_PROTOCOL_MMCR:
//			mmcr_start_routing(dst_id);
			break;
#endif
		default:
			// ERROR
			break;
	}
}

#endif // _ENABLE_ROUTING_

