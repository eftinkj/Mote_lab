/****************************************************************************
**
** Copyright (C) 2009-2008 Maciej Zawodniok, Priya Kasirajan. All rights reserved.
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

#ifdef FEAT_ENABLE_MMCR

#include "routing.h"
#include "routing_MMCR_MPR_select.h"
#ifdef _ARM_
	#include "FEAT_STM32W/STM32W_Radio.h"
#endif // _ARM_

#include "network_hardware_ADFS.h"
#include "common.h"
#include "FEAT_Xbee_API/AT_scripts.h"
#include "FEAT_Queuing/Queuing.h"
#include "FEAT_Networking/tsp_common_pub.h"
#include "Application.h"
#include "FEAT_Networking/Phy_layer.h"
#include "packet.h"
#include "FEAT_Scheduler/sch_basic_pub.h"

///////////////////////////////////////////////
///////////////////////////////////////////////
//uint8_t	mmcr_pkt_id = 0xFF;

unsigned char XDATA mmcr_request_send_TC_ = 0 ; // request for sending Topology control
//unsigned char XDATA mmcr_request_send_HELLO_ = 0; // request for sending HELLO

// Variables to manage )periodic) timeout
//uint8_t mmcr_hello_timeout_enabled_ = 0;
//rtc_tick_t	mmcr_hello_timeout_ = RTC_OVERFLOW_TIMER_VALUE;
uint8_t mmcr_hello_tidx = SCH_NO_TIMEOUT_ID; // Clear timeout's ID
uint8_t mmcr_routeStart_tidx_ = SCH_NO_TIMEOUT_ID; // Clear timeout's ID

unsigned char mpr_idx[MMCR_MAX_MPRS];
//unsigned char mpr_idx[MMCR_MAX_MPRS];
uint8_t mpr_count_;


uint8_t mmcr_state_; // state of the routing agent (e.g. IDLE, routing in-progress)
unsigned char XDATA mmcr_Hello_countdown_;	// ON - periodically send HELLO
//int XDATA mmcr_route_search_BS_;	// address of the target node (BS)



//Counters for the Routing Energy Analysis
///unsigned char XDATA mmcr_Hello_counter;
///unsigned char XDATA mmcr_ACK_MMCR_counter;
///unsigned char XDATA mmcr_DAT_MMCR_counter;


uint16_t mmcr_len_sample;

//unsigned char XDATA mmcr_ch_counter;

//unsigned char XDATA mmcr_return_ch;

//uint8_t ch_sw_TIDX; /// Timer ID for channel switching request



nhood_t	Neighborhood;


char str_MMCR_sendACK[] = "MMCR_sendACK";
char str_MMCR_HELLO[] = "MMCR_HELLO";
char str_MMCR_TC[] = "MMCR_TC";
char str_MMCR_startRT[] = "MMCR_startRT";

///////////////////////////////////////////////
/**
  * routing_init - performs initial setup of routing
  */
void routing_init_MPR_select()
{
	mmcr_state_=MMCR_STATE_IDLE;

	//mmcr_request_send_HELLO_ = 1; // request for sending HELLO
	// Variables to manage (periodic) timeout
//	mmcr_hello_timeout_ = RTC_OVERFLOW_TIMER_VALUE;

	init_topology();
	// MZ: Why deleted? //	
	mpr_select();

	mmcr_startRouteSearch(0);
}

///////////////////////////////////////////////
/**
  * init_topology - performs initial setup of topology
  */
void init_topology()
{
	mmcr_neighbor_clear();
	
#ifdef NODE_1
	Neighborhood.OneHopNodes = 3;
	Neighborhood.TwoHopNodes = 1;
	Neighborhood.OneHop[0].ID = 0x000C;
	Neighborhood.OneHop[0].Links = 1;
	Neighborhood.OneHop[0].Link[0] = 0x00FE;
	Neighborhood.OneHop[1].ID = 0x000D;
	Neighborhood.OneHop[1].Links = 1;
	Neighborhood.OneHop[1].Link[0] = 0x00FE;
	Neighborhood.OneHop[2].ID = 0x000B;
	Neighborhood.OneHop[2].Links = 1;
	Neighborhood.OneHop[2].Link[0] = 0x00FE;
	Neighborhood.TwoHop[0].ID = 0x00FE;
#endif
#ifdef NODE_2
	Neighborhood.OneHopNodes = 2;
	Neighborhood.TwoHopNodes = 1;
	Neighborhood.OneHop[0].ID = 0x000A;
	Neighborhood.OneHop[0].Links = 1;
	Neighborhood.OneHop[0].Link[0] = 0x00AA;
	Neighborhood.OneHop[1].ID = 0x00FE;
	Neighborhood.OneHop[1].Links = 0;
	Neighborhood.TwoHop[0].ID = 0x00AA;
#endif
#ifdef NODE_3
	Neighborhood.OneHopNodes = 2; //3
	Neighborhood.TwoHopNodes = 0;
	Neighborhood.OneHop[0].ID = 0x000F;
	Neighborhood.OneHop[0].Links = 0;
//	Neighborhood.OneHop[1].ID = 0x000F;
//	Neighborhood.OneHop[1].Links = 0;
	Neighborhood.OneHop[1].ID = 0x00FE;
	Neighborhood.OneHop[1].Links = 0;
#endif
#ifdef NODE_4
	Neighborhood.OneHopNodes = 2;
	Neighborhood.TwoHopNodes = 0;
	Neighborhood.OneHop[0].ID = 0x000E;
	Neighborhood.OneHop[0].Links = 0;
	Neighborhood.OneHop[1].ID = 0x00FE;
	Neighborhood.OneHop[1].Links = 0;
#endif
#ifdef NODE_5
//	Neighborhood.OneHopNodes = 1;
//	Neighborhood.TwoHopNodes = 0;
//	Neighborhood.OneHop[0].ID = 0x0A;
//	Neighborhood.OneHop[0].Links = 0;
	Neighborhood.OneHopNodes = 3;
	Neighborhood.TwoHopNodes = 1;
	Neighborhood.OneHop[0].ID = 0x000C;
	Neighborhood.OneHop[0].Links = 1;
	Neighborhood.OneHop[0].Link[0] = 0x00FE;
	Neighborhood.OneHop[1].ID = 0x000D;
	Neighborhood.OneHop[1].Links = 1;
	Neighborhood.OneHop[1].Link[0] = 0x00FE;
	Neighborhood.OneHop[2].ID = 0x000B;
	Neighborhood.OneHop[2].Links = 1;
	Neighborhood.OneHop[2].Link[0] = 0x00FE;
	Neighborhood.TwoHop[0].ID = 0x00FE;
#endif


#ifdef NODE_6
//	Neighborhood.OneHopNodes = 1;
//	Neighborhood.TwoHopNodes = 0;
//	Neighborhood.OneHop[0].ID = 0x000F;
//	Neighborhood.OneHop[0].Links = 0;
	Neighborhood.OneHopNodes = 3;
	Neighborhood.TwoHopNodes = 1;
	Neighborhood.OneHop[0].ID = 0x000C;
	Neighborhood.OneHop[0].Links = 1;
	Neighborhood.OneHop[0].Link[0] = 0x00FE;
	Neighborhood.OneHop[1].ID = 0x000D;
	Neighborhood.OneHop[1].Links = 1;
	Neighborhood.OneHop[1].Link[0] = 0x00FE;
	Neighborhood.OneHop[2].ID = 0x000B;
	Neighborhood.OneHop[2].Links = 1;
	Neighborhood.OneHop[2].Link[0] = 0x00FE;
	Neighborhood.TwoHop[0].ID = 0x00FE;
#endif

#ifdef NODE_7
//	Neighborhood.OneHopNodes = 1;
//	Neighborhood.TwoHopNodes = 0;
//	Neighborhood.OneHop[0].ID = 0x000F;
//	Neighborhood.OneHop[0].Links = 0;
	Neighborhood.OneHopNodes = 3;
	Neighborhood.TwoHopNodes = 1;
	Neighborhood.OneHop[0].ID = 0x000C;
	Neighborhood.OneHop[0].Links = 1;
	Neighborhood.OneHop[0].Link[0] = 0x00FE;
	Neighborhood.OneHop[1].ID = 0x000D;
	Neighborhood.OneHop[1].Links = 1;
	Neighborhood.OneHop[1].Link[0] = 0x00FE;
	Neighborhood.OneHop[2].ID = 0x000B;
	Neighborhood.OneHop[2].Links = 1;
	Neighborhood.OneHop[2].Link[0] = 0x00FE;
	Neighborhood.TwoHop[0].ID = 0x00FE;
#endif

}

#define ROUTE_COST_AGE_STEP	10000


///////////////////////////////////////////////
/**
  * mpr_select - performs MPR selection
  */
void mpr_select()
{
	int x = 0;
	int y = 0;
	int z = 0;
	unsigned char count[10];

//	boolean uncovered;
	int overcoverage;

	for(x=0;x<MMCR_MAX_MPRS;x++)
	{
		count[x]=0;
		mpr_idx[x]=NO_NODE_IDX;
	}
	for (x=0;x<MMCR_MAX_TWO_HOPS;x++)
	{
		Neighborhood.TwoHop[x].Covered=0;
	}
	/** MPR selection **/
	// count coverage
	for(x=0;x<Neighborhood.OneHopNodes;x++)
	{
		for(y=0;y<Neighborhood.OneHop[x].Links;y++)
		{
			for(z=0;z<Neighborhood.TwoHopNodes;z++)
			{
				if(Neighborhood.OneHop[x].Link[y] == Neighborhood.TwoHop[z].ID)
					count[x]++;
			}
		}		
	}

	// 1. select singular neighbors
	for(x=0;x<Neighborhood.OneHopNodes;x++)
	{
		if(count[x] == 0)
		{
			Neighborhood.OneHop[x].flags.MPR = true;
			//TODO: search for single link
			Neighborhood.TwoHop[find_two_hop_node(Neighborhood.OneHop[x].Link[0])].Covered ++;
		}
	}

	// 2. selects as MPR neighbors with the largest count of uncovered twohop nodes
	x=0;
//	uncovered = true;
	while(x<Neighborhood.OneHopNodes /*&& uncovered*/)
	{
		//TODO: sort high-low
		if(count[x]>0)
		{
			if(~Neighborhood.TwoHop[find_two_hop_node(Neighborhood.OneHop[x].Link[0])].Covered)
			{
				Neighborhood.OneHop[x].flags.MPR = true;
				Neighborhood.TwoHop[find_two_hop_node(Neighborhood.OneHop[x].Link[0])].Covered ++;
			}
		}
		x++;
	}

	// 3. remove redundant nodes
	for(x=0;x<Neighborhood.OneHopNodes;x++)
	{
		overcoverage = 0;
		for(y=0;y<Neighborhood.OneHop[x].Links;y++)
		{
			//for(z=0;z<10;z++)
			{
				if(Neighborhood.TwoHop[find_two_hop_node(Neighborhood.OneHop[x].Link[y])].Covered > 1)
				{
					overcoverage ++;
				}
			}
		}
		if(overcoverage == Neighborhood.OneHop[x].Links)
		{
			Neighborhood.OneHop[x].flags.MPR = false;
			//TODO: reduce Covered
			for(y=0;y<Neighborhood.OneHop[x].Links;y++)
			{
				Neighborhood.TwoHop[find_two_hop_node(Neighborhood.OneHop[x].Link[y])].Covered--;
			}
		}
	}
    mpr_count_ = 0;
	for(x=0;x<Neighborhood.OneHopNodes;x++)
	{
		if(Neighborhood.OneHop[x].flags.MPR)
		{
			mpr_idx[mpr_count_++] = x; //Neighborhood.OneHop[x].ID;
		}
	}
}


/**
  * neighbor_clear - setups up neighbor table
  */
void mmcr_neighbor_clear()
{
	int x,y;
	Neighborhood.OneHopNodes = 0;
	Neighborhood.TwoHopNodes = 0;
	for (x=0;x<MMCR_MAX_ONE_HOPS;x++)
	{
		Neighborhood.OneHop[x].ID=0;
		Neighborhood.OneHop[x].flags_u8 = 0; // MPR=0; IAM_MPR=0; ...
		Neighborhood.OneHop[x].Links=0;
		Neighborhood.OneHop[x].LinkCost=0xFFFFFFFF;
		for (y=0;y<MMCR_MAX_OH_LINKS;y++)
			Neighborhood.OneHop[x].Link[y]=0;
	}
	for (x=0;x<MMCR_MAX_TWO_HOPS;x++)
	{
		Neighborhood.TwoHop[x].ID=0;
		Neighborhood.TwoHop[x].Covered=0;
	}
}


#define ONEHOP_COST_AGE_STEP	0x10000000
/**
  * mmcr_neighborhood_age - age the neighbor tables to remove stale ones
  */
void mmcr_neighborhood_age()
{
	int x,y;
//	Neighborhood.OneHopNodes = 0;
//	Neighborhood.TwoHopNodes = 0;
	for (x=0; x<Neighborhood.OneHopNodes; x++)
	{
		if ( 0xFFFFFFFF - ONEHOP_COST_AGE_STEP > Neighborhood.OneHop[x].LinkCost )
		{
			Neighborhood.OneHop[x].LinkCost += ONEHOP_COST_AGE_STEP;
		}
		else
		{
			//Neighborhood.OneHop[x].ID=0;
			Neighborhood.OneHop[x].flags_u8 = 0; // MPR=0; IAM_MPR=0; ...
			Neighborhood.OneHop[x].Links=0;
			Neighborhood.OneHop[x].LinkCost=0xFFFFFFFF;
			for (y=0;y<10;y++)
				Neighborhood.OneHop[x].Link[y]=0;
		}
	}
//	for (x=0;x<MMCR_MAX_TWO_HOPS;x++)
//	{
//		Neighborhood.TwoHop[x].ID=0;
//		Neighborhood.TwoHop[x].Covered=0;
//	}
}


uint8_t find_one_hop_node(uint16_t ID)
{
	uint8_t x;
	for (x=0;x<Neighborhood.OneHopNodes;x++)
	{
		if(Neighborhood.OneHop[x].ID == ID)
			return x;
	}
	return NO_NODE_IDX;
}

uint8_t find_two_hop_node(uint16_t ID)
{
	uint8_t x;
	for (x=0;x<Neighborhood.TwoHopNodes;x++)
	{
		if(Neighborhood.TwoHop[x].ID == ID)
			return x;
	}
	return NO_NODE_IDX;
}

uint8_t add_one_hop_node(uint8_t ID, uint16_t link_factor)
{
	uint8_t y;
	if ( MMCR_MAX_ONE_HOPS == Neighborhood.OneHopNodes)
	{
		return NO_NODE_IDX;
	}
	Neighborhood.OneHop[Neighborhood.OneHopNodes].ID = ID;
	Neighborhood.OneHop[Neighborhood.OneHopNodes].flags.MPR = false;
	Neighborhood.OneHop[Neighborhood.OneHopNodes].Links = 0;
	Neighborhood.OneHop[Neighborhood.OneHopNodes].LinkCost = link_factor;
	// Redundant - the count (Links) is sufficient
	for (y=0;y<MMCR_MAX_TWO_HOPS;y++)
	{
		Neighborhood.OneHop[Neighborhood.OneHopNodes].Link[y] = NO_NODE_IDX;
	}
	// Return current index and then increment neighbor count
	return Neighborhood.OneHopNodes++;
}

// Return: two hop node index
uint8_t add_two_hop_node(uint8_t one_hop_idx, uint8_t two_hop_id)
{
	uint8_t lidx = NO_NODE_IDX;
	uint8_t x = find_two_hop_node(two_hop_id);
	if (NO_NODE_IDX == x)
	{
		if (MMCR_MAX_TWO_HOPS == Neighborhood.TwoHopNodes)
		{
			return NO_NODE_IDX;
		}
		Neighborhood.TwoHop[Neighborhood.TwoHopNodes].ID = two_hop_id;
		Neighborhood.TwoHop[Neighborhood.TwoHopNodes].Covered = 0;
		x = Neighborhood.TwoHopNodes++;
	}
	// Add two hop link
	lidx = find_link(one_hop_idx, x);
	if (NO_NODE_IDX == lidx)
	{
		if (MMCR_MAX_TWO_HOPS == Neighborhood.OneHop[one_hop_idx].Links)
		{
			return NO_NODE_IDX; // no space left
		}
		Neighborhood.OneHop[one_hop_idx].Link[Neighborhood.OneHop[one_hop_idx].Links] = x;
		lidx = Neighborhood.OneHop[one_hop_idx].Links++;
	}
	else
	{
		Neighborhood.OneHop[one_hop_idx].Link[lidx] = x;
	}
	return x;
}
//uint8_t add_one_hop_node(uint8_t ID, uint16_t link_factor)
//uint8_t add_two_hop_node(uint8_t one_hop_idx, uint8_t two_hop_id)

uint8_t find_link(uint8_t one_idx, uint8_t two_idx)
{
	uint8_t i;
	for( i=0; i<Neighborhood.OneHop[one_idx].Links; i++)
	{
		if (two_idx == Neighborhood.OneHop[one_idx].Link[i])
		{
			return i;
		}
	}
	return NO_NODE_IDX;
}

///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * startRouteSearch - starts the route discovery procedure
  *
  *
  */
void mmcr_startRouteSearch ( uint8_t *dst )
{
	if ( ( 0 < mmcr_Hello_countdown_ ) || ( MMCR_STATE_IDLE != mmcr_state_ ) )
	{
			return;
	}

	// ###############################
	// start route Discovery toward BS
	//mmcr_route_search_BS_ = *dst;
	
	// OR only age the tables/costs
	//mmcr_neighbor_clear();
	//mmcr_neighborhood_age();
		
	rtr_MMCR_Hello_Phase();

	mmcr_Hello_countdown_ = MMCR_DEFAULT_ROUTE_SEARCH_ON_REPETITIONS;
	mmcr_state_ = MMCR_STATE_ROUTE_DISCOVERY;
	
	mmcr_set_tx_timeout ( 10 );
	

}
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * rtr_MMCR_Hello_Phase() - handles the routing phase of transmitting HELLO pkts
  */
void rtr_MMCR_Hello_Phase()
{
	//int a;
//	if ( ROUTING_PROTOCOL_MMCR == my_protocol_ )
	{
		// Send HELLO routing packet
// MZ: Why??		if ( 1 == mmcr_request_send_HELLO_ )
		{
//			uint8_t packet[RTR_MMCR_HELLO_PKT_SIZE];
			uint16_t len = mmcr_sendHELLO ( (hpkt_mmcr_t XDATA*)packet );

			/*a = */ sendPriorityPacket ( len, packet, MAC_BROADCAST );
			my_energy_ = my_energy_ - HELLO_LENGTH;

//			mmcr_request_send_HELLO_ = 0;
		}
	}
}



///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_sendHELLO - broadcasts a HELLO packet
  */
unsigned int mmcr_sendHELLO ( hpkt_mmcr_t XDATA*hello_pkt )
{
//	int len = PKT_HEADER_LENGTH; // size of the packet header (start + flag + dst(2) + src(2) + len)

	hpkt_mmcr_t XDATA *hpkt = ( ( hpkt_mmcr_t XDATA * ) hello_pkt);

	hpkt->start = START_BYTE;
	hpkt->flags = FLAG_HELLO_MMCR;  // HELLO_MMCR packet
	hpkt->mac_dst = ENDIAN16(MAC_BROADCAST); // Broadcast
	hpkt->mac_src = ENDIAN16(MY_ADDR);

	// FILL END-TO-END ADDRESSING
	hpkt->dst_id = 0xFF;
	hpkt->src_id = MY_ADDR;
	hpkt->length = HELLO_LENGTH_MMCR - PKT_HEADER_LENGTH;;

	// FILL THE MMCR field
	hpkt->energy=ENDIAN32(my_energy_);

	// FILL the CHECKSUM
	// TODO: calculate checksum
	hpkt->crc = 65;

	return HELLO_LENGTH_MMCR;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_recvHELLO - handles a received HELLO packet
  * should identify if this node is potential relay node
  * if yes, then send response (ACK?)
  */
#ifdef _ARM_
#pragma optimize=none
#endif // _ARM_
unsigned int mmcr_recvHELLO ( hpkt_mmcr_t XDATA *hp )
{

	unsigned long int link_factor, i=0;
	//uint16_t	mac_src = ENDIAN16(hp->mac_src);
	//uint16_t t = 0x0000;
//	uint16_t hop_mac = Neighborhood.OneHop[0].ID;
//	uint8_t j;
	uint32_t energy = ENDIAN32(hp->energy);
	long int delay = TYPICAL_DELAY_FOR_LINK;
//	int channel_i;

	if (ENDIAN16(hp->mac_src) == MY_ADDR)
	{
	  	cs_req_switch (MMCR_SWITCH_CHANNEL(DEFAULT_RF_CHANNEL), CHANNEL_SWITCH_DELAY);
		//mmcr_ch_counter = 0;
		//mmcr_rf_ch_sw(&mmcr_return_ch);
		//channel_i = mmcr_sendSwitch((tpkt_t_mmcr*)hp); // Workaround to compiler issues
		//channel_i = mmcr_sendSwitch((tpkt_t_mmcr*)hp);
		//mmcr_return_ch = channel_i;
//		phy_set_RF_channel(channel_i);
//        routing_init_MMCR();
		return 1;
	}

////////////////////////////////////////////////////////
//if ((hp->mac_src == 0x000A)||(hp->mac_src == 0x000F))
 //return 1;

///////////////////////////////////////////////////////// For block hello packet from source A

	if (ENERGY_MAX_VALUE < energy)
		energy = ENERGY_MAX_VALUE;
	link_factor = ( energy * MMCR_BANDWIDTH_FACTOR * DELAY_SCALING ) / (delay);

	i = find_one_hop_node(ENDIAN16(hp->mac_src));
	if ( NO_NODE_IDX != i )
	{
		Neighborhood.OneHop[i].LinkCost = link_factor;
	}
	else
	{
		i = add_one_hop_node(ENDIAN16(hp->mac_src), link_factor);
	}
// NOT PRESENT IN HELLO
	/*
	// For all one-hop neighbors of the sender add to Two hop neighberhood
	for (j=0; j< hello_node_count; j++)
	{
		add_two_hop_node(i, two_hop_id);
	}
*/
	sch_create_timeout(rtc_get_ticks()+5, mmcr_sendACK, (uint8_t XDATA*)hp, str_MMCR_sendACK);
	//mmcr_sendACK( (hpkt_mmcr_t XDATA *) hp);
	return 1;

}


///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_sendACK - handles a sending of ACK packet
  * collects all info for packet
  * prepares timestamp
  */
void mmcr_sendACK ( uint8_t XDATA * p)
{
	uint8_t i;
	hpkt_mmcr_t XDATA* hp = (hpkt_mmcr_t XDATA*)p;
//	long int delay = TYPICAL_DELAY_FOR_LINK;

	apkt_t_mmcr *ap = ( (apkt_t_mmcr *) packet );
	IAR_PACKED	uint16_t *pkt_neighbor_addr = (uint16_t *)&(ap->neighbor_addr_first);
	uint32_t *pkt_neighbor_cost = NULL;
	uint8_t *pkt_crc;

	ap->start = START_BYTE;
	ap->flags = FLAG_ACK_MMCR;
	ap->mac_dst = hp->mac_src;	 //MAC DST
	ap->mac_src = ENDIAN16(MY_ADDR); 		// MAC SRC

	ap->length = 15 + (Neighborhood.OneHopNodes*6);		

	ap->dst_id = hp->src_id;
	ap->src_id = MY_ADDR;

	ap->energy = ENDIAN32(my_energy_);

	ap->neighbor_count=Neighborhood.OneHopNodes;

	for(i=0;i<Neighborhood.OneHopNodes;i++)
	{
		*pkt_neighbor_addr = ENDIAN16(Neighborhood.OneHop[i].ID);
		pkt_neighbor_addr++;
		pkt_neighbor_cost = (uint32_t *)pkt_neighbor_addr;
		*pkt_neighbor_cost = ENDIAN32(Neighborhood.OneHop[i].LinkCost);
		pkt_neighbor_addr +=2;
	}

	pkt_crc = (uint8_t *)pkt_neighbor_addr;
	*pkt_crc = STOP_BYTE;

	my_energy_ = my_energy_ - ap->length;


	sendPriorityPacket ( ap->length, (char XDATA*) ap, MAC_BROADCAST );//ap->mac_dst );
	//mmcr_count_ack_tx_++;
	return;
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_revcACK - handles a received ACK packet
  * should identify if this node is potential relay node
  * if yes, then send response (ACK?)
  */
unsigned int mmcr_recvACK (apkt_t_mmcr *apkt )
{
	apkt_t_mmcr XDATA *ap = (( apkt_t_mmcr XDATA * ) apkt);
	long int delay = TYPICAL_DELAY_FOR_LINK;
	unsigned long int link_factor;
	uint32_t energy = ENDIAN32(ap->energy);
	IAR_PACKED uint16_t *pkt_neighbor_addr = (uint16_t *)&(ap->neighbor_addr_first);
	uint32_t *pkt_neighbor_cost = NULL;
	uint8_t i,j;
	uint8_t two_hop_id;
	//if (0 == mmcr_Hello_countdown_) return 1; // do not receive ACK if I have not sent HELLO
	
	if (ENERGY_MAX_VALUE < energy)
		energy = ENERGY_MAX_VALUE;
	link_factor = ( energy * MMCR_BANDWIDTH_FACTOR * DELAY_SCALING ) / (delay);

	i = find_one_hop_node(ap->src_id);
	if ( NO_NODE_IDX != i )
	{
		Neighborhood.OneHop[i].LinkCost = link_factor;
	}
	else
	{
		i = add_one_hop_node(ap->src_id, link_factor);
	}
// For all one-hop neighbors of the sender add to Two hop neighberhood
	for (j=0; j< ap->neighbor_count; j++)
	{
		two_hop_id = ENDIAN16(*pkt_neighbor_addr);
		if (MY_ADDR != two_hop_id)
		{
			pkt_neighbor_cost = (uint32_t *)(pkt_neighbor_addr+1);
			add_two_hop_node(i, two_hop_id);
			// Is it the right way/place to do it? - Maybe after MPR selection?
			mmcr_set_route(two_hop_id, ENDIAN16(ap->mac_src), link_factor + ENDIAN32(*pkt_neighbor_cost));
		}
		pkt_neighbor_addr +=3;
	}
	return 1;
}	


/////////////////////////////////
/**
  * mmcr_sendTC - send channel switch message
  *
  */
void mmcr_sendTC ( uint8_t *context )
{
	int i;
	tpkt_t_mmcr XDATA*tpkt = (tpkt_t_mmcr XDATA*) packet;
	uint16_t IAR_PACKED	*mpr_list = (uint16_t*) &(tpkt->mpr_list); //
		
	mpr_select();

	tpkt->start = START_BYTE;
	tpkt->flags = FLAG_TC_MMCR;  // TC packet
	tpkt->mac_dst = MAC_BROADCAST; // Broadcast
	tpkt->mac_src = ENDIAN16(MY_ADDR);

	tpkt->length = RTR_MMCR_TC_PKT_SIZE - PKT_HEADER_LENGTH + (mpr_count_<<1); // of the packet'd data

	// FILL END-TO-END ADDRESSING
	tpkt->dst_id = MAC_BROADCAST;
	tpkt->src_id = MY_ADDR;//ENDIAN16(MY_ADDR); //01/17/2011

	tpkt->energy=ENDIAN32(my_energy_);
	tpkt->channel=my_rf_channel_;

	if (MAX_MPRS_PER_PKT <= mpr_count_)
	{
		mpr_count_ = MAX_MPRS_PER_PKT; // need to split the TC list into multiple TC packets
	}
	tpkt->mpr_count = mpr_count_;
	for(i=0; i < mpr_count_; i++)
	{
		mpr_list[i] = ENDIAN16(Neighborhood.OneHop[mpr_idx[i]].ID);
	}
	
	*((uint8_t*)(&mpr_list[mpr_count_])) = PKT_MODULE_TYPE_END;
	
	sendPriorityPacket ( tpkt->length+PKT_HEADER_LENGTH, (char XDATA*) tpkt, MAC_BROADCAST );//ap->mac_dst );
}

// 				ch_sw_TIDX = sch_create_timeout(rtc_get_ticks()+MMCR_CHANNEL_SWITCH_REQ_INTERVAL, mmcr_rf_ch_sw, &ch_sw_TIDX);


/////////////////////////////////
/**
  * mmcr_recvTC - Topology Control packet (MPR info)
  *
  */
unsigned int mmcr_recvTC ( tpkt_t_mmcr *tp )
{
	// Check if I'm MP for the sender -> update neighbor list to indicate it
//	tp->channel=MMCR_SWITCH_CHANNEL(DEFAULT_RF_CHANNEL);
//	tp->channel=MMCR_SWITCH_CHANNEL(DEFAULT_RF_CHANNEL); // Workaround for compiler issue
//	tp->channel=MMCR_SWITCH_CHANNEL(DEFAULT_RF_CHANNEL);
//	phy_set_RF_channel(tp->channel);
	uint8_t i, idx;
	uint16_t IAR_PACKED *mpr_list = &(tp->mpr_list);
	uint16_t src = ENDIAN16(tp->mac_src);

    if (tp->channel != my_rf_channel_)
    {
        //channel_switch_countdown_ms
        phy_set_RF_channel(tp->channel);
    }
	for( i=0; i< tp->mpr_count; i++)
	{
		if (MY_ADDR == ENDIAN16(mpr_list[i]))
		{
			idx = find_one_hop_node(src);
			if (NO_NODE_IDX == idx)
			{
				// the node is not my one-hop neighbor ??!!! -> Fatal Error
			}
			else
			{
				// set myself as MPR for the one hop neighbor
				Neighborhood.OneHop[idx].flags.IAM_MPR = 1;
			}
		}
	}
	return 1;
}
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_set_tx_timeout(??) - set one-time timeout for MMCR hello/tc packets
  */
void mmcr_set_tx_timeout ( uint16_t ms )
{
	if (SCH_NO_TIMEOUT_ID != mmcr_hello_tidx)
	{
		sch_remove_timeout(mmcr_hello_tidx, str_MMCR_HELLO);
		mmcr_hello_tidx = SCH_NO_TIMEOUT_ID;
	}
	mmcr_hello_tidx = sch_create_timeout( rtc_get_ticks() + ms, mmcr_hello_timeout, 0, str_MMCR_HELLO);
//	mmcr_hello_timeout_ = rtc_get_ticks() + ms;
//	mmcr_hello_timeout_enabled_ = 1;
}

/////////////////////////////////
/**
  * mmcr_hello_timeout(??) - runs the procedure of periodic sending of Hello packets
  */
void mmcr_hello_timeout()
{
	// Workaround - remove long Hello interval timeout - since we are already here
	if (SCH_NO_TIMEOUT_ID != mmcr_routeStart_tidx_)
	{
		sch_remove_timeout(mmcr_routeStart_tidx_, str_MMCR_startRT);
		mmcr_routeStart_tidx_ = SCH_NO_TIMEOUT_ID;
	}
	// Timeout reached -> is there a route found? or should I retransmit HELLO?
	mmcr_hello_tidx = SCH_NO_TIMEOUT_ID; // Clear THIS timeout's ID - it already fired
//	unsigned int mac_addr = mmcr_neighbor_analyse(MY_DEST);
	if ( 0 < mmcr_Hello_countdown_ )
	{			mmcr_Hello_countdown_ --; // count down HELLO retransmissions
		// number of retransmission not reached -> resent HELLO
		TOGGLE_LED(YLED);//YLED = ~YLED;
				
		//mmcr_request_send_HELLO_ = 1;
		//mmcr_request_send_TC_= 1;
		mmcr_set_tx_timeout(MMCR_ROUTE_SEARCH_HELLO_INTERVAL);
		rtr_MMCR_Hello_Phase();
		TOGGLE_LED(YLED);//YLED = ~YLED;
	}
	else
	{
		// all Hello messages has been without response -> STOP
		mmcr_Hello_countdown_=0;
		//mmcr_request_send_HELLO_ = 0; //1;
		//mmcr_request_send_TC_ = 0;
		mmcr_state_ = MMCR_STATE_IDLE;
		sch_create_timeout(rtc_get_ticks()+10, mmcr_sendTC, (uint8_t*)packet, str_MMCR_TC);
		mmcr_routeStart_tidx_ = sch_create_timeout(rtc_get_ticks()+MMCR_LONG_HELLO_INTERVAL, mmcr_startRouteSearch, 0, str_MMCR_startRT);

		enableDataTx_ = 1;
	}
}	
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////



/////////////////////////////////




/*
void copyOneHop(uint8_t idx_src, idx_dst)
{
	Neighborhood		uint16_t 	ID;
	union {hop_flags_t flags; 	uint8_t flags_u8;};	// flags for one hop node
	uint16_t	Link[10]; // point to index (ID) of the second hop node
	char	Links;
	uint32_t LinkCost;
}

void removeOneHop(uint16_t id)
{
}
*/

#endif /* FEAT_ENABLE_MMCR */