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
#include "routing_MMCR.h"
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
uint8_t	mmcr_pkt_id = 0xFF;

//uint8_t mmcr_state_; // state of the routing agent (e.g. IDLE, routing in-progress)

//Counters for the Routing Energy Analysis
//unsigned char XDATA mmcr_Hello_counter;
//unsigned char XDATA mmcr_ACK_MMCR_counter;
//unsigned char XDATA mmcr_DAT_MMCR_counter;


//uint16_t mmcr_len_sample;

unsigned char XDATA mmcr_ch_counter;

unsigned char XDATA mmcr_return_ch;

uint8_t ch_sw_TIDX; /// Timer ID for channel switching request

///////////////////////////////////////////////


IAR_PACKED struct RENTRY
{
	uint16_t ID;
	unsigned int NextHopID;
	uint32_t NextHopCost;
	unsigned int AltNextHopID;
	uint32_t AltNextHopCost;
};


IAR_PACKED struct
{
	char length;
	struct RENTRY rentry[MMCR_MAX_RTABLE_LEN];
} rtable;


extern void timer_cb_test1(uint8_t *x);
extern void timer_cb_test2(uint8_t *x);

char str_LED_BLINK_1[]="LED1";
char str_LED_BLINK_2[]="LED2";
char str_RTR_CH_SWITCH[] = "RTR_CH_SWITCH_MMCR";

char str_TEST[] = "TEST 1\n";

void timer_cb_test1(uint8_t *x)
{
	SET_LED(YLED);
	sch_create_timeout(rtc_get_ticks()+1000, timer_cb_test2, 0, str_LED_BLINK_2);
//	mmcr_age_tables(); // Increases the costs in neighbors and routing tables - to remove nodes over time
}
void timer_cb_test2(uint8_t *x)
{
	CLEAR_LED(YLED);
	//send_PONG(MAC_BROADCAST);
	//sendSerialData( strlen(str_TEST), str_TEST );
	sendPacketData( strlen(str_TEST), str_TEST, MAC_BROADCAST );
	sch_create_timeout(rtc_get_ticks()+1000, timer_cb_test1, 0, str_LED_BLINK_1);
}
///////////////////////////////////////////////
/**
  * routing_init - performs initial setup of routing
  */
void routing_init_MMCR()
{
//	mmcr_route_search_BS_ = MY_DEST;	// address of the target node (BS)
//	AODVcounter_update = 1;
//	mmcr_Hello_counter = 0;
//	mmcr_ACK_MMCR_counter = 0;
//	mmcr_DAT_MMCR_counter = 0;
//	mmcr_Hello_countdown_=0;

	mmcr_state_=MMCR_STATE_IDLE;
	
	mmcr_pkt_id = 0xFF;

	enableDataTx_ = 0;


	mmcr_init_rtable_from_topology();

	///sch_add_loop(( sch_loop_func_t )mmcr_loop );
	timer_cb_test1(0);
}

///////////////////////////////////////////////
/**
  * mmcr_init_rtable_from_topology - performs initial setup of topology
  */
void mmcr_init_rtable_from_topology()
{
	uint8_t x;
	/* init routing table */
	rtable.length = Neighborhood.TwoHopNodes;
	for (x = 0; x < rtable.length; x++)
	{
		rtable.rentry[x].ID = Neighborhood.TwoHop[x].ID;
		rtable.rentry[x].NextHopID = RTR_NO_ROUTE;
		rtable.rentry[x].NextHopCost = 0xFFFFFFFF;
		rtable.rentry[x].AltNextHopID = RTR_NO_ROUTE;
		rtable.rentry[x].AltNextHopCost = 0xFFFFFFFF;
	}
}

#define ROUTE_COST_AGE_STEP	10000

/**
*
*  Increases the costs in neighbors and routing tables - to remove nodes over time
*/
void mmcr_age_tables()
{
	for (uint8_t x = 0; x < rtable.length; x++)
	{
//		rtable.rentry[x].ID = Neighborhood.TwoHop[x].ID;
		if ( 0xFFFFFFFF - ROUTE_COST_AGE_STEP > rtable.rentry[x].NextHopCost )
		{
			rtable.rentry[x].NextHopCost += ROUTE_COST_AGE_STEP;
		}
		else
		{
			// rtable record spotted -> clear next hop
			rtable.rentry[x].NextHopID = rtable.rentry[x].AltNextHopID;
			rtable.rentry[x].NextHopCost = rtable.rentry[x].AltNextHopCost;
			rtable.rentry[x].AltNextHopID = RTR_NO_ROUTE;
			rtable.rentry[x].AltNextHopCost = 0xFFFFFFFF;			
		}
		if ( 0xFFFFFFFF - ROUTE_COST_AGE_STEP > rtable.rentry[x].AltNextHopCost )
		{
			rtable.rentry[x].AltNextHopCost += ROUTE_COST_AGE_STEP;
		}
		else
		{
			rtable.rentry[x].AltNextHopCost = 0xFFFFFFFF;
			rtable.rentry[x].AltNextHopID = RTR_NO_ROUTE;
		}
	}
}
///////////////////////////////////////////////


///////////////////////////////////////////////
///////////////////////////////////////////////



///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * oedsr_start_routing - re-starts the route discovery for DST_ID
  *
  *
  */
void mmcr_start_routing( uint16_t dst_id )
{
	// MMCR is proactive routing protocol thus it is "always" running
	return;
}

///////////////////////////////////////////////
///////////////////////////////////////////////

/**
*	mmcr_recv_routing_msg()
*/
uint8_t	mmcr_recv_routing_msg(pkt_t XDATA *pkt)
{
	switch (pkt->flags)
	{
		case FLAG_HELLO_MMCR:
 			mmcr_recvHELLO ( ( hpkt_mmcr_t XDATA * ) pkt );
			break;
		case FLAG_ACK_MMCR:
//			if ( ( 0 < route_search_on_ ) || ( 0 < route_search_count_ ) )
			{
				mmcr_recvACK ( (apkt_t_mmcr*)pkt );				
			}
			break;
		case FLAG_TC_MMCR:
			mmcr_recvTC ( (tpkt_t_mmcr*)pkt );
			break;
			
		case FLAG_CHSW_MMCR:
			mmcr_recvCHSW ( (tpkt_t_mmcr*)pkt );
			break;

		case FLAG_SELECT_MMCR:
			//mmcr_recvSelect ( pkt );
			break;
		///////////////////////////////////////////////
		// Packet unknown -> was not processed by the routing implementation
		default:
			return RTR_NOT_DONE;
	}
	// Channel switch
/* // Causes to return to original RF channel too quickly
	if (mmcr_ch_counter == 15)
	{
		phy_set_RF_channel(mmcr_return_ch);
		routing_init_MMCR();
		mmcr_ch_counter = 0; //01/17/2011

	}
*/
	return RTR_DONE;
}

///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_rf_ch_sw - send channel switch message
  *
  */

void mmcr_rf_ch_sw ( uint8_t *channel_id )
{

//	int len = PKT_HEADER_LENGTH; // size of the packet header (start + flag + dst(2) + src(2) + len)

	tpkt_t_mmcr XDATA *tpkt = ( ( tpkt_t_mmcr XDATA * ) packet);

	tpkt->start = 0xAA;
	tpkt->flags = FLAG_CHSW_MMCR;  // TC packet
	tpkt->mac_dst = MAC_BROADCAST; // Broadcast
	tpkt->mac_src = ENDIAN16(MY_ADDR);


	tpkt->length = RTR_MMCR_TC_PKT_SIZE - PKT_HEADER_LENGTH; // of the packet'd data

	// FILL END-TO-END ADDRESSING
	tpkt->dst_id = 0xFF;
	tpkt->src_id = ENDIAN16(MY_ADDR); // ENDIAN MAY RESULT IN WRONG PART USED (high byte)

	tpkt->energy=ENDIAN32(my_energy_);
	tpkt->channel=MMCR_SWITCH_CHANNEL(DEFAULT_RF_CHANNEL);
	// FILL the CHECKSUM
	// TODO: calculate checksum
	tpkt->mpr_count = 0; //no extra info included
	tpkt->mpr_list = 0x4141; //last byte

///////////////////////////////// 12-22-2010 For Channel switching
//	mmcr_ch_counter = 0;
	if (mmcr_ch_counter<15)
	{
		sendPriorityPacket ( tpkt->length+PKT_HEADER_LENGTH, (sint8_t XDATA*) tpkt, MAC_BROADCAST );//ap->mac_dst );
	 	mmcr_ch_counter++;
		// Schedule next
		ch_sw_TIDX = sch_create_timeout(rtc_get_ticks()+MMCR_CHANNEL_SWITCH_REQ_INTERVAL, mmcr_rf_ch_sw, channel_id, str_RTR_CH_SWITCH);
	}
	else
	{	// after last request - switch myself
		phy_set_RF_channel(tpkt->channel);
		(*channel_id) = tpkt->channel;
	}
}

/////////////////////////////////
/**
  * mmcr_recvCHSW - switch channel
  *
  */
uint8_t mmcr_recvCHSW ( tpkt_t_mmcr* pkt )
{
//	tp->channel=MMCR_SWITCH_CHANNEL(DEFAULT_RF_CHANNEL);
//	tp->channel=MMCR_SWITCH_CHANNEL(DEFAULT_RF_CHANNEL); // Workaround for compiler issue
//	tp->channel=MMCR_SWITCH_CHANNEL(DEFAULT_RF_CHANNEL);
	phy_set_RF_channel(pkt->channel);
	return 1;
}
///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * mmcr_set_route - adds or updates a route to "dst_id"
  */
int mmcr_set_route ( uint16_t dst_id, uint16_t next_hop, uint32_t metric )
{
	int i;
#ifndef BS_NEIGHBOR
	if (BS_ADDR == next_hop) // Prevent node from adding BS as next hop (neighbor) if it is not supposed to be one
	{
		return 0;
	}
#endif
	//mpr_select();
	for (i = 0; i < rtable.length; i++)
	{
		if (rtable.rentry[i].ID == dst_id)
		{
			// rtable record spotted -> set next hop
			if (rtable.rentry[i].NextHopID == RTR_NO_ROUTE)
			{
				rtable.rentry[i].NextHopID = next_hop;
				rtable.rentry[i].NextHopCost = metric;
				return 1; 	//successfully added route
			}
			else if (rtable.rentry[i].NextHopID == next_hop)
			{
				rtable.rentry[i].NextHopCost = metric;
				return 1; 	//successfully updated route
			}
			else if (metric < rtable.rentry[i].NextHopCost) // If new is better
			{
				rtable.rentry[i].AltNextHopID = rtable.rentry[i].NextHopID;
				rtable.rentry[i].AltNextHopCost = rtable.rentry[i].NextHopCost;
				rtable.rentry[i].NextHopID = next_hop;
				rtable.rentry[i].NextHopCost = metric;
				return 1; 	//successfully added route			
			}
			// if not better then First choice
			else if (rtable.rentry[i].AltNextHopID == RTR_NO_ROUTE)
			{
				rtable.rentry[i].AltNextHopID = next_hop;
				rtable.rentry[i].AltNextHopCost = metric;
				return 1; 	//successfully added route
			}
			else if (rtable.rentry[i].AltNextHopID == next_hop)
			{
				rtable.rentry[i].AltNextHopCost = metric;
				return 1; 	//successfully updated route
			}
			else if (metric < rtable.rentry[i].AltNextHopCost) // If new is better
			{
				rtable.rentry[i].AltNextHopID = next_hop;
				rtable.rentry[i].AltNextHopCost = metric;			
				return 1; 	//successfully added route
			}
			// Found record but nothing has been updated - existing routes are better
			return 0;
		}
	}
	// Have not found entry - is the routing table full?
	if (MMCR_MAX_RTABLE_LEN > rtable.length)
	{
		rtable.rentry[rtable.length].ID = dst_id;
		rtable.rentry[rtable.length].NextHopID = next_hop;
		rtable.rentry[rtable.length].NextHopCost = metric;
		rtable.rentry[rtable.length].AltNextHopID = RTR_NO_ROUTE;
		rtable.rentry[rtable.length].AltNextHopCost = 0xFFFFFFFF;
		rtable.length++;
		return 1; // Successfully added new routing entry
	}
	else
	{
		// Define how to remove stale ones
		return 0;
	}
	//return 0; // two hop id not found!!!
}
///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * neighbor_analyse - performs analysis of the neighbor table
		and selects the best node
  */
uint16_t mmcr_neighbor_analyse(uint16_t dst_id)
{
	unsigned int i, result = RTR_NO_ROUTE;	// by default FFFF indicating lack of route
	
	/* check if dst_id is a one hop neighbor */
	for (i = 0; i < Neighborhood.OneHopNodes; i++)
	{
		if (Neighborhood.OneHop[i].ID == dst_id)
			result = Neighborhood.OneHop[i].ID;
	}
	/* check if dst_id is a two hop neighbor */
	for (i = 0; i < rtable.length; i++)
	{
		if (rtable.rentry[i].ID == dst_id)
		{
			if ((rtable.rentry[i].NextHopID == RTR_NO_ROUTE) && (rtable.rentry[i].AltNextHopID == RTR_NO_ROUTE))
				result = RTR_NO_ROUTE;
			else if ((rtable.rentry[i].NextHopCost <= rtable.rentry[i].AltNextHopCost) && (rtable.rentry[i].NextHopID != RTR_NO_ROUTE))
				result = rtable.rentry[i].NextHopID;
			else if (rtable.rentry[i].AltNextHopID != RTR_NO_ROUTE)
				result = rtable.rentry[i].AltNextHopID;
		}
	}
	return result;
}

///////////////////////////////////////////////
/**
  * sendDATA - handles a sending of DATA packet
  * 1) check if buffer ready then passes packet
  * 2) else temporarly stores
  */
char MMCR_send_DATA_base ( uint16_t base )
{
	unsigned int mac_d;
	pkt_t * XDATA pkt = ( pkt_t* ) ( & ( buffer0[ base] ) ); //&(QBUFF_ACCESS(base,0));


//	if (enableDataTx_)
	//if (1)
	{
	  if (0xFF == pkt->dst_id)
	  {
		mac_d = MAC_BROADCAST;
	  }
	  else
	  {
		mac_d = mmcr_neighbor_analyse ( pkt->dst_id );
		if ( ( RTR_NO_ROUTE == mac_d ))// || ( 0 == enableDataTx_ ) )
		{
			// if the route search returned 0xFFFF then there is no route!!!
			// then start new one and drop the packet
			mmcr_startRouteSearch(&(pkt->dst_id));// MY_DEST); //2010-12-13 Reset network after losting link
			//MZ: for multiple destinations that makes no sense:
			//enableDataTx_ = 0; // do not pass date anymore
			// return the packet to queue
//			app_drop_pkt ( base, MODULE_RTR, REASON_NOROUTE, EVENT_DSEND );
			return ROUTING_BEGAN_ROUTE_DISCOVERY;
		}
	  }
		pkt->mac_dst = ENDIAN16(mac_d);
		pkt->mac_src = ENDIAN16(MY_ADDR);

	}
//	else
//	{
//		APPEND_LOG ( NODE_ID_STR, NODE_ID_STR_LEN );
//		APPEND_LOG ( "DROP DATA\r", 10 );
		// drop the packet
//		return 0;
//	}
	return 1;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * recvAcceptData - handles a received ACCEPT DATA packet from BS
  * 1) enables data transmission
  * 2)
  */
uint8_t mmcr_recvAcceptData ( sint8_t *pkt )
{
	pkt_t XDATA *p = ( ( pkt_t XDATA * ) pkt);

	// analyse ACK packet
	if ( 0xFFFF == ENDIAN16(p->mac_dst) )
	{
		// enable DATA transmission
		enableDataTx_ = 1;


		APPEND_LOG ( NODE_ID_STR, NODE_ID_STR_LEN );
		APPEND_LOG ( "DATA - OK\r\r\r", 12 );

		return 1;
	}


	// wrong trasmitter
	return 0;
}

/////////////////////////////////
/**
  * mmcr_dropped_link(??) - the link failed (after few retransmissions??) - update routing
  *    and optionally restart route discovery
  */
void mmcr_dropped_link(uint16_t hop_id)
{
	int i;
	if (MAC_BROADCAST == hop_id)
		return;
	for (i = 0; i < rtable.length; i++)
	{
		if (rtable.rentry[i].ID == hop_id)
		{
			// rtable record spotted -> clear next hop
			rtable.rentry[i].NextHopID = rtable.rentry[i].AltNextHopID;
			rtable.rentry[i].NextHopCost = rtable.rentry[i].AltNextHopCost;
			rtable.rentry[i].AltNextHopID = RTR_NO_ROUTE;
			rtable.rentry[i].AltNextHopCost = 0xFFFFFFFF;
			if ( RTR_NO_ROUTE == rtable.rentry[i].NextHopID )
				mmcr_startRouteSearch(0);
		}
		if (rtable.rentry[i].NextHopID == hop_id)
		{
			// rtable record spotted -> clear next hop
			rtable.rentry[i].NextHopID = rtable.rentry[i].AltNextHopID;
			rtable.rentry[i].NextHopCost = rtable.rentry[i].AltNextHopCost;
			rtable.rentry[i].AltNextHopID = RTR_NO_ROUTE;
			rtable.rentry[i].AltNextHopCost = 0xFFFFFFFF;
		}
		if (rtable.rentry[i].AltNextHopID == hop_id)
		{
			// rtable record spotted -> clear alternative next hop
			//rtable.rentry[i].NextHopID = rtable.rentry[i].AltNextHopID;
			//rtable.rentry[i].NextHopCost = rtable.rentry[i].AltNextHopCost;
			rtable.rentry[i].AltNextHopID = RTR_NO_ROUTE;
			rtable.rentry[i].AltNextHopCost = 0xFFFFFFFF;
		}
	}
}




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