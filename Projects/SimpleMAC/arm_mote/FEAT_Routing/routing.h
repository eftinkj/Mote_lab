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
#ifndef ROUTING_H
#define ROUTING_H


#include "defs/esnl_pub.h"


#define ROUTING_PROTOCOL_OEDSR 0
#define ROUTING_PROTOCOL_AODV 1
#define ROUTING_PROTOCOL_MMCR 2

#define RTR_DONE		1
#define RTR_NOT_DONE	0

// Indicate there is no next hop/route does not exist
#define RTR_NO_ROUTE	0xFFFF

// Maximum length of hop list in the packet (limited by buffer size per packet)
#define MAX_HOPLIST_INDEX 80

#define ROUTING_BEGAN_ROUTE_DISCOVERY	0x03

#ifdef _ENABLE_ROUTING_


// Routing protocols flags
extern unsigned char XDATA AODV_control_bit;
extern unsigned char XDATA OEDSR_control_bit;
//extern unsigned char xdata MMCR_control_bit;

extern uint8_t AODV_route_control;
//Packet Counters
extern uint8_t AODVcounter_update;
extern unsigned char XDATA RREQ_counter;
extern unsigned char XDATA RREP_counter;
extern unsigned char XDATA ACK_AODV_counter;
extern unsigned char XDATA DATA_AODV_counter;

extern uint8_t data_forward_control_;

//extern uint8_t hello_flag;

void recvStopData ( sint8_t XDATA *p );


void routing_init(void);

void routing_set_routing_protocol(void); // my_protocol_ has correct value

void routing_receive_packet ( sint8_t XDATA *p, sint8_t rssi );

void routing_data_request ( sint8_t XDATA *p );

int8_t routing_send_DATA_base ( uint16_t base );

///////////////////////////////////////////////
/**
  * recvDATA - handles a received DATA packet
  * 1) check if final destination
  * 2) passes pakcet to sendDATA
  */
uint8_t recvDATA ( sint8_t XDATA *pkt, sint8_t rssi );
///////////////////////////////////////////////

void routing_send_counters(void);

/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
int8_t routing_to_CH_base ( uint16_t base );


/**
  * update_hoplist_module_recv(hoplist, rssi) - updates hoplist
  *  when receiving a packet (node + rssi)
  */
void update_hoplist_module_recv ( uint8_t XDATA *hoplist, uint8_t rssi );

/**
  * update_hoplist_module_send(hoplist) - updates hoplist
  *  when sending a packet (tx power + statistics)
  */
void update_hoplist_module_send ( uint8_t XDATA *hoplist );


/**
  * update_pkt_send(base) - updates packet content when sending a packet
  *  (hoplist and othe modules)
  */
void update_pkt_send ( uint16_t base );


/**
  * rtr_dropped_link(??) - the link failed (after few retransmissions??) - update routing
  *    and optionally restart route discovery
  */
void rtr_dropped_link(uint16_t hop_id);


/**
  * rtr_add_neighbor(DST, NEXT_HOP, METRIC) - update routing for one-hop neighbor
  */
void rtr_add_neighbor(uint16_t dst_id, uint16_t next_hop, uint8_t metric);

/**
  * rtr_start_routing(DST_ID) - start a route discovery for "DST_ID"
  */
void rtr_start_routing(uint16_t dst_id);


#endif // _ENABLE_ROUTING_

#endif
