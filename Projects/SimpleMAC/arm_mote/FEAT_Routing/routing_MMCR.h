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
#ifndef ROUTING_MMCR_H
#define ROUTING_MMCR_H

#include "defs/esnl_pub.h"

#ifdef FEAT_ENABLE_MMCR

#include "packet.h"
///////////////////////////////////////////////

///extern unsigned char XDATA mmcr_request_send_HELLO_; // request for sending HELLO
extern uint8_t XDATA request_send_TC_ ; // request for sending Topology control


#define MMCR_MAX_RTABLE_LEN	10


extern uint8_t mmcr_state_; // state of the routing agent (e.g. IDLE, routing in-progress)
#define MMCR_STATE_IDLE	0
#define MMCR_STATE_ROUTE_DISCOVERY	1


// Neighbor list index defines:
#define NO_NODE_IDX	0xff


// -----------------------------------------
// Temporary routing info about neighbours


void mmcr_route_table_clear();


///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * neighbor_analyse - performs analysis of the neighbor table
		and selects the best MPR nodes
  */
uint16_t mmcr_neighbor_analyse(uint16_t dst_id);
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * routing_init - performs initial setup of routing
  */
void routing_init_MMCR();
///////////////////////////////////////////////

/**
  * mmcr_init_rtable_from_topology - performs initial setup of routing table
  *										based on MPR/Neighbor topology
  */
void mmcr_init_rtable_from_topology();


/**
*
*  Increases the costs in neighbors and routing tables - to remove nodes over time
*/
void mmcr_age_tables();

///////////////////////////////////////////////
/**
  * mmcr_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
//void mmcr_loop( void );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_set_route - adds or updates a route to "dst_id"
  * returns '0' if failed to do so (no free space) or '1' when successful
  */
int mmcr_set_route ( uint16_t dst_id, uint16_t next_hop, uint32_t metric );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * find_route - searches for a route to a given destination
  * returns index to the route record or '-1' if not found
  */
int mmcr_find_route ( const uint16_t dst_id );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * get_next_hop - searches for a route to a given destination
  * returns MAC address of next hop on this route
  * or '0xFFFF' if not found any route
  */
unsigned int mmcr_get_next_hop ( uint16_t dst_id );

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
*	mmcr_recv_routing_msg()
*/
uint8_t	mmcr_recv_routing_msg(pkt_t XDATA *pkt);

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * startRouteSearch - starts the route discovery procedure
  *
  *
  */
//void mmcr_startRouteSearch (uint8_t *dst );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_start_routing - re-starts the route discovery for DST_ID
  *
  *
  */
void mmcr_start_routing( uint16_t dst_id );

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

unsigned int mmcr_sendSwitch ( tpkt_t_mmcr XDATA*pkt );

/**
  * mmcr_rf_ch_sw - send channel switch message
  *
  */
void mmcr_rf_ch_sw ( uint8_t *tidx );
///////////////////////////////////////////////

/**
  * mmcr_recvCHSW - switch channel
  *
  */
uint8_t mmcr_recvCHSW ( tpkt_t_mmcr *pkt );
///////////////////////////////////////////////



//unsigned int recv_DATA( unsigned char *pkt,int data_message[], unsigned char rssi);
unsigned int mmcr_recv_DATA( sint8_t XDATA *pkt, uint8_t rssi);

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
char MMCR_send_DATA_base ( uint16_t base );

///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////

/**
  * mmcr_dropped_link(??) - the link failed (after few retransmissions??) - update routing
  *    and optionally restart route discovery
  */
void mmcr_dropped_link(uint16_t hop_id);
///////////////////////////////////////////////
///////////////////////////////////////////////


///////////////////////////////////////////////
///////////////////////////////////////////////

#endif // FEAT_ENABLE_MMCR

#endif // ROUTING_MMCR_H
