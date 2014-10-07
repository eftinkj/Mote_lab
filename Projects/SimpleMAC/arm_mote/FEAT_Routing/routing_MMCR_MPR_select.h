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
#ifndef ROUTING_MMCR_MPR_SELECT_H
#define ROUTING_MMCR_MPR_SELECT_H

#include "defs/esnl_pub.h"

#ifdef FEAT_ENABLE_MMCR

#include "packet.h"
///////////////////////////////////////////////

///extern unsigned char XDATA mmcr_request_send_HELLO_; // request for sending HELLO
extern uint8_t XDATA request_send_TC_ ; // request for sending Topology control

#define MAX_ROUTES_MMCR 2
#define MMCR_MAX_MPRS	10
#define CHANNEL_SWITCH_DELAY 2000

	

#define MMCR_BANDWIDTH_FACTOR 1
#define MMCR_SWITCH_CHANNEL(c) (c>=MAX_RF_CHANNEL)?MIN_RF_CHANNEL:c+1


#define MMCR_MAX_ONE_HOPS	10
#define MMCR_MAX_TWO_HOPS	10
#define MMCR_MAX_OH_LINKS	10

#define MMCR_MAX_RTABLE_LEN	10


//0

extern void send_MMCR_counters();

//extern unsigned int XDATA r_dst[MAX_ROUTES_MMCR];
//extern unsigned int XDATA r_next_hop[MAX_ROUTES_MMCR];
//extern unsigned char XDATA r_metric[MAX_ROUTES_MMCR];
//extern char XDATA r_last_line;

extern uint8_t mmcr_state_; // state of the routing agent (e.g. IDLE, routing in-progress)
#define MMCR_STATE_IDLE	0
#define MMCR_STATE_ROUTE_DISCOVERY	1


// Neighbor list index defines:
#define NO_NODE_IDX	0xff

#define	MAX_MPRS_PER_PKT	40

// Max number of repetitions of HELLO (without ACKs) before stopping the routing functionality
#define MMCR_DEFAULT_ROUTE_SEARCH_ON_REPETITIONS 3
extern uint16_t XDATA mmcr_route_search_BS_;	// address of the target node (BS)
extern uint8_t XDATA mmcr_Hello_countdown_;	// ON - periodically send HELLO

// Count down between HELLO packets (x (25ms) timer interrupt)
#define MMCR_ROUTE_SEARCH_HELLO_INTERVAL 500  //Edit this to tune the MMCR......
#define MMCR_LONG_HELLO_INTERVAL	60000	// Every 1min=60000ms ...
//#define MMCR_ROUTE_SEARCH_COUNT_HELLO 50  //Edit this to tune the MMCR......
// Count down for collecting all ACK packets (x (25ms) timer interrupt)
//#define MMCR_ROUTE_SEARCH_COUNT_ACK 20
//extern unsigned char XDATA mmcr_DAT_MMCR_counter;


extern uint8_t XDATA mmcr_ch_counter;

extern uint8_t XDATA mmcr_return_ch;


// -----------------------------------------
// Temporary routing info about neighbours


//#define MMCR_MAX_NEIGHBOR 10
//#define MMCR_OH_MAX_NEIGHBOR 10
//#define RTR_MMCR_HELLO_PKT_SIZE	18
//#define RTR_MMCR_HELLO_TC_SIZE	14
//#define MAX_BUFFER 80
#define MMCR_CHANNEL_SWITCH_REQ_INTERVAL	5

//extern uint8_t hello_flag;
//extern uint8_t tc_flag;
extern uint8_t mmcr_data_flag;
//extern uint8_t load;
//extern uint8_t hset_buffer[MAX_BUFFER];

//extern uint16_t XDATA mmcr_ack_addr_[MMCR_MAX_NEIGHBOR];
//extern uint16_t XDATA mmcr_neighbor_addr_[MMCR_MAX_NEIGHBOR];
//extern unsigned int XDATA mmcr_neighbor_link_cost_[MMCR_MAX_NEIGHBOR];
//extern unsigned int XDATA mmcr_neighbor_distance_[MMCR_MAX_NEIGHBOR];

//extern uint16_t XDATA mmcr_hopcount[MMCR_MAX_NEIGHBOR];

//extern uint16_t XDATA mmcr_oh_neighbor_addr_[MMCR_OH_MAX_NEIGHBOR];
//extern unsigned int XDATA mmcr_oh_neighbor_distance_[MMCR_OH_MAX_NEIGHBOR];
//extern unsigned int XDATA mmcr_oh_neighbor_link_cost_[MMCR_OH_MAX_NEIGHBOR];
//extern unsigned char XDATA mmcr_neighbor_element_;	// the first free element (and count of used)
//extern unsigned char XDATA mmcr_oh_hopcount[MMCR_MAX_NEIGHBOR];
//extern unsigned char mmcr_neighbor_flag;
//extern unsigned char mmcr_neighbor_enable;


//extern unsigned int XDATA diff_distance[MMCR_MAX_NEIGHBOR];
//extern unsigned int XDATA mmcr_neighbor_two_link_cost_[MMCR_MAX_NEIGHBOR];
//extern unsigned char XDATA mmcr_neighbor_two_free_;	// the first free element (and count of used)
//extern unsigned char XDATA mmcr_hopcount_two[MMCR_MAX_NEIGHBOR];
//extern unsigned char mmcr_neighbor_two_enable[MMCR_MAX_NEIGHBOR];

//extern unsigned char XDATA mmcr_neighbor_free_;	// the first free element (and count of used)
//extern unsigned char XDATA mmcr_ack_free_;
//extern unsigned char XDATA mmcr_ack_enable_;
//extern unsigned char XDATA ack_enable_;

//Recent Additions to add the other abilities to MMCR
//Counters for the Routing Energy Analysis
//extern unsigned char XDATA mmcr_Select_counter;
//extern unsigned char XDATA mmcr_Hello_counter;
//extern unsigned char XDATA mmcr_ACK_MMCR_counter;
//extern unsigned char XDATA mmcr_DAT_MMCR_counter;
//void mmcr_send_aodv_counters();
//void mmcr_route_table_clear();

//extern void set_distance(unsigned char);
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
typedef IAR_PACKED struct ONEHOP_FLAGS
{
  uint8_t	MPR		:1;
  uint8_t	IAM_MPR	:1;
} PACKED hop_flags_t;

IAR_PACKED struct ONEHOP
{
	uint16_t 	ID;
	union {hop_flags_t flags; 	uint8_t flags_u8;};	// flags for one hop node
	uint16_t	Link[MMCR_MAX_OH_LINKS]; // point to index (ID) of the second hop node
	char	Links;
	uint32_t LinkCost;
};

IAR_PACKED struct TWOHOP
{
	uint16_t 	ID;
	char	Covered;
};
typedef IAR_PACKED struct NEIGHBORHOOD
{
	struct 	ONEHOP OneHop[MMCR_MAX_ONE_HOPS];
	struct 	TWOHOP TwoHop[MMCR_MAX_TWO_HOPS];
	int		OneHopNodes;
	int		TwoHopNodes;
} PACKED nhood_t;

extern nhood_t	Neighborhood;

///////////////////////////////////////////////
///////////////////////////////////////////////


///////////////////////////////////////////////
///////////////////////////////////////////////

/**
  * neighbor_clear - setups up neighbor table
  */
void mmcr_neighbor_clear();
///////////////////////////////////////////////
///////////////////////////////////////////////

/**
  * mmcr_neighborhood_age - age the neighbor tables to remove stale ones
  */
void mmcr_neighborhood_age();

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * neighbor_add - adds new neighbor to the neighbor table
  */
char mmcr_neighbor_add ( apkt_t_mmcr *hp, unsigned int delay );
//char neighbor_add (uint32_t ap_energy, uint16_t ap_mac_src, uint8_t ap_src_id, unsigned int delay );

char mmcr_neighbor_two_add(apkt_t_mmcr *ap, unsigned int delay);
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * neighbor_analyse - performs analysis of the neighbor table
		and selects the best MPR nodes
  */
uint16_t mmcr_neighbor_analyse(uint16_t dst_id);
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * neighbor_get_cost - get cost for the selected node
  */
uint32_t mmcr_neighbor_get_cost ( uint16_t addr );

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * routing_init_MPR_select - performs initial setup of routing
  */
void routing_init_MPR_select();
///////////////////////////////////////////////



///////////////////////////////////////////////
///////////////////////////////////////////////
/**
*	mmcr_recv_routing_msg()
*/
//uint8_t	mmcr_recv_routing_msg(pkt_t XDATA *pkt);

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * startRouteSearch - starts the route discovery procedure
  *
  *
  */
void mmcr_startRouteSearch (uint8_t *dst );
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendHELLO - handles a sending of HELLO packet
  * collects all info for packet
  * prepares timestamp
  */
unsigned int mmcr_sendHELLO ( hpkt_mmcr_t XDATA *pkt );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_recvHELLO - handles a received HELLO packet
  * should identify if this node is potential relay node
  * if yes, then send response (ACK?)
  */
unsigned int mmcr_recvHELLO ( hpkt_mmcr_t XDATA*pkt );
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * sendACK - handles a sending of ACK packet
  * collects all info for packet
  * prepares timestamp
  */
void mmcr_sendACK ( uint8_t XDATA *p);//hpkt_mmcr_t XDATA*pkt);
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * revcACK - handles a received ACK packet
  * should identify if this node is potential relay node
  * if yes, then send response (ACK?)
  */
unsigned int mmcr_recvACK ( apkt_t_mmcr *pkt );
///////////////////////////////////////////////

void mmcr_sendTC ( uint8_t *context );
///////////////////////////////////////////////

unsigned int mmcr_recvTC ( tpkt_t_mmcr *pkt );
///////////////////////////////////////////////




///////////////////////////////////////////////
//unsigned int mmcr_sendSwitch ( tpkt_t_mmcr XDATA*pkt );
///////////////////////////////////////////////
/**
  * mmcr_rf_ch_sw - send channel switch message
  *
  */
//void mmcr_rf_ch_sw ( uint8_t *tidx );
///////////////////////////////////////////////
/**
  * mmcr_recvCHSW - switch channel
  *
  */
//uint8_t mmcr_recvCHSW ( tpkt_t_mmcr *pkt );
///////////////////////////////////////////////



///////////////////////////////////////////////

///////////////////////////////////////////////
/**
  * recvAcceptData - handles a received ACCEPT DATA packet from BS
  * 1) enables data transmission
  * 2)
  */
//uint8_t mmcr_recvAcceptData ( sint8_t *pkt );
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * rtr_MMCR_Hello_Phase() - handles the routing phase of transmitting HELLO pkts
  */
void rtr_MMCR_Hello_Phase();
void rtr_MMCR_TC_Phase();
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////

/**
  * mmcr_hello_timeout(??) - runs the procedure of periodic sending of Hello packets
  */
void mmcr_hello_timeout();
///////////////////////////////////////////////
///////////////////////////////////////////////


/**
  * mmcr_set_tx_timeout(??) - set one-time timeout for MMCR hello/tc packets
  */
void mmcr_set_tx_timeout ( uint16_t ms );

///////////////////////////////////////////////
///////////////////////////////////////////////

#endif // FEAT_ENABLE_MMCR

/* new code begins */
void init_topology(void);
void mpr_select(void);
uint8_t find_one_hop_node(uint16_t ID);
uint8_t find_two_hop_node(uint16_t ID);
uint8_t add_one_hop_node(uint8_t ID, uint16_t link_factor);
uint8_t add_two_hop_node(uint8_t one_hop_idx, uint8_t two_hop_id);
uint8_t find_link(uint8_t one_idx, uint8_t two_idx);

#endif // ROUTING_MMCR_MPR_SELECT_H
