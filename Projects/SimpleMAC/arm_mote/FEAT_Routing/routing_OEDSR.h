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
#ifndef ROUTING_OEDSR_H
#define ROUTING_OEDSR_H

#include "defs/esnl_pub.h"

#ifdef _ENABLE_OEDSR_

#include "packet.h"
///////////////////////////////////////////////

//#define MAX_ROUTES_OEDSR 10
#define MAX_ROUTES_OEDSR 20

extern void send_OEDSR_counters(void);


extern uint16_t XDATA r_dst[MAX_ROUTES_OEDSR];
extern uint16_t XDATA r_next_hop[MAX_ROUTES_OEDSR];
extern uint8_t XDATA r_metric[MAX_ROUTES_OEDSR];
extern int8_t XDATA r_last_line;

extern uint8_t oedsr_state_; // state of the routing agent (e.g. IDLE, routing in-progress)
#define OEDSR_STATE_IDLE	0
#define OEDSR_STATE_ROUTE_DISCOVERY	1

// Max number of repetitions of HELLO (without ACKs) before stopping the routing functionality
#define DEFAULT_ROUTE_SEARCH_ON_REPETITIONS 10
extern uint8_t XDATA oedsr_Hello_countdown_;	// ON - periodically send HELLO
extern int16_t XDATA route_search_BS_;	// address of the target node (BS)

// Interval between HELLO packets (in ms) timer interrupt)
#define ROUTE_SEARCH_HELLO_INTERVAL 1250  //Edit this to tune the OEDSR......
// Count down for collecting all ACK packets (x (25ms) timer interrupt)
//#define ROUTE_SEARCH_COUNT_ACK 40

// -----------------------------------------
// Temporary routing info about neighbours


#define MAX_ACK_TEMP 15
#define RTR_OEDSR_HELLO_PKT_SIZE	18


extern uint16_t XDATA ack_temp_addr_[MAX_ACK_TEMP];
extern uint16_t XDATA ack_temp_link_cost_[MAX_ACK_TEMP];

extern uint8_t XDATA ack_temp_free_;	// the first free element (and count of used)

//Recent Additions to add the other abilities to OEDSR
//Counters for the Routing Energy Analysis
extern uint8_t XDATA Select_counter;
extern uint8_t XDATA Hello_counter;
extern uint8_t XDATA ACK_OEDSR_counter;
extern uint8_t XDATA DAT_OEDSR_counter;
void send_aodv_counters(void);


//extern void set_distance(uint8_t);
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * ack_temp_clear - setups up temporary ACKs table
  */
void ack_temp_clear(void);
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * ack_temp_add - adds new ACK to the temporary ACKs table
  */
int8_t ack_temp_add ( apkt_t *p, uint16_t delay );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * ack_temp_analyse - performs analysis of the temporary ACKs table
		and selects the best node
  */
uint16_t ack_temp_analyse(void);
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * ack_temp_get_cost - get cost for the selected node
  */
uint16_t ack_temp_get_cost ( uint16_t addr );

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * routing_init - performs initial setup of routing
  */
void routing_init_OEDSR(void);

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void oedsr_loop( void );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_set_route - adds or updates a route to "dst_id"
  * returns '0' if failed to do so (no free space) or '1' when successful
  */
int16_t oedsr_set_route ( uint16_t dst_id, uint16_t next_hop, uint8_t metric );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * find_route - searches for a route to a given destination
  * returns index to the route record or '-1' if not found
  */
int16_t find_route ( const uint16_t dst_id );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * get_next_hop - searches for a route to a given destination
  * returns MAC address of next hop on this route
  * or '0xFFFF' if not found any route
  */
uint16_t get_next_hop ( uint16_t dst_id );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * startRouteSearch - starts the route discovery procedure
  *
  *
  */
void startRouteSearch ( uint16_t dst );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_start_routing - re-starts the route discovery for DST_ID
  *
  *
  */
void oedsr_start_routing( uint16_t dst_id );

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendHELLO - handles a sending of HELLO packet
  * collects all info for packet
  * prepares timestamp and sents out
  */
uint8_t sendHELLO ( void );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * revcHELLO - handles a received HELLO packet
  * should identify if this node is potential relay node
  * if yes, then send response (ACK?)
  */
uint16_t recvHELLO ( hpkt_t *pkt );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendACK - handles a sending of ACK packet
  * collects all info for packet
  * prepares timestamp
  */
uint16_t sendACK ( hpkt_t *pkt );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * revcACK - handles a received ACK packet
  * should identify if this node is potential relay node
  * if yes, then send response (ACK?)
  */
uint16_t recvACK ( sint8_t *pkt );
///////////////////////////////////////////////

#ifdef NO_QUEUE
///////////////////////////////////////////////
/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
uint16_t sendDATA ( int8_t *pkt, uint8_t rssi );
///////////////////////////////////////////////
#endif

///////////////////////////////////////////////
/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
int8_t OEDSR_send_DATA_base ( uint16_t base );
///////////////////////////////////////////////

///////////////////////////////////////////////
/**
  * recvAcceptData - handles a received ACCEPT DATA packet from BS
  * 1) enables data transmission
  * 2)
  */
uint16_t recvAcceptData ( int8_t *pkt );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendSelect - tells a selected node that it will be a relay node
  * 1)
  * 2)
  */
uint16_t sendSelect ( uint16_t relay, uint8_t bs );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * recvSelect - tells this node that it has been selected as a relay node
  * 1) enables data transmission
  * 2)
  */
uint16_t recvSelect ( sint8_t *pkt );
///////////////////////////////////////////////
///////////////////////////////////////////////

/**
  * oedsr_dropped_link(??) - the link failed (after few retransmissions??) - update routing
  *    and optionally restart route discovery
  */
void oedsr_dropped_link(void);

///////////////////////////////////////////////
///////////////////////////////////////////////

/**
  * oedsr_hello_timoeout(??) - runs the procedure of periodic sending of Hello packets
  */
void oedsr_hello_timoeout(void);
///////////////////////////////////////////////
///////////////////////////////////////////////


/**
  * oedsr_set_tx_timeout(??) - set one-time timeout for MMCR hello/tc packets
  */
void oedsr_set_tx_timeout ( uint16_t ms );

///////////////////////////////////////////////
///////////////////////////////////////////////

#endif // _ENABLE_OEDSR_

#endif // ROUTING_OEDSR_H
