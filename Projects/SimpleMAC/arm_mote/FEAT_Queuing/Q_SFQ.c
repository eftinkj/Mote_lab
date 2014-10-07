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
//SFQ source code

#include "common.h"

#ifdef FEAT_ENABLE_SFQ



#include "RTC.h"
#include "Q_SFQ.h"
#include "Queuing.h"
#include "packet.h"

//Decleration Variables
table_index_t	XDATA SFQ_head[NUMFLOWS];
table_index_t	XDATA SFQ_tail[NUMFLOWS];
table_size_t	XDATA SFQsize[NUMFLOWS];
v_time_t	virt_tag = 0;
v_time_t	XDATA Ftime_old[NUMFLOWS];
v_time_t	XDATA Ftime[NUMFLOWS][NUMQBUFFS];
v_time_t	XDATA Stime[NUMFLOWS][NUMQBUFFS];
//pkt_index_t 	xdata pktindex[NUMFLOWS * NUMQBUFFS] _at_ PKT_INDEX_BEGIN;
pkt_index_t 	XDATA pktindex[NUMFLOWS * NUMQBUFFS];
//table_index_t Nextpkt[NUMFLOWS][NUMQBUFFS];
weight_t XDATA flow_weights[NUMFLOWS];
weight_t XDATA my_updated_weight_;
uint8_t XDATA q_sfq_version_;

//**********************

#define PKTINDEX_ACCESS(f,i) pktindex[f*NUMQBUFFS+i]

//SFQ Initialization
void Q_SFQ_init()
{
	int k1, k2;
	//Initialize the SFQ ID Table
	for ( k1 = 0; k1 < NUMFLOWS; k1++ )
	{
		for ( k2 = 0; k2 < NUMQBUFFS; k2++ )
		{
			Ftime[k1][k2] = k1;
			Stime[k1][k2] = k1;
//		 	Nextpkt[k1][k2] = 0xFF;
			PKTINDEX_ACCESS ( k1, k2 ) = 0xDD;

		}
		Ftime_old[k1] = 0;
		SFQ_tail[k1] = 0;
		SFQ_head[k1] = 0;
		SFQsize[k1] = 0;

		flow_weights[k1] = 0;
	}
	my_updated_weight_ = 0; // the original weight with update is empty
	q_sfq_version_ = SFQ_WEIGHT_UPDATE_ADFS;
}
//**********************

//Function to deQ an SFQ Packet
pkt_index_t Q_SFQ_dequeue()
{

	pkt_index_t index_dequeu; // the result place
	table_index_t f;
	v_time_t S_temp = 0xFFFFFFFF; // the max value 0xFFFFFF
	table_index_t f_temp = 0xFF;	

	if ( 0 < Q_SFQ_utilization() )
	{
		
		//find the queue with lowest Start Tag
		for ( f = 0; f < NUMFLOWS; f++ )
		{
			if ( 0 < SFQsize[f] )
			{
				if ( S_temp > Stime[f][SFQ_tail[f]] )
				{
					S_temp = Stime[f][SFQ_tail[f]];
					f_temp = f;
				}
			}
			else
			{
				continue;
			}
		}
		if ( 0xFF == f_temp )
		{
			return NO_PACKET_INDEX; // ERROR -> size not 0, but not found any packet)
		}
		// Take out the packet from the found queue
		//index = pktindex[f_temp][SFQ_tail[f_temp]];
		index_dequeu = PKTINDEX_ACCESS ( f_temp, SFQ_tail[f_temp] );
		SFQ_tail[f_temp]++;
		if ( NUMQBUFFS <= SFQ_tail[f_temp] )
			SFQ_tail[f_temp] = 0;
		SFQsize[f_temp]--;
// MZ_DFS
		// Update total weight
		if (SFQ_WEIGHT_UPDATE_ADFS == q_sfq_version_)
		{
			pkt_t* xdata temp = ( pkt_t* ) & ( buffer0[QBUFF_BASE ( index_dequeu ) ] );
			flow_id_t temp_f = temp->src_id - FLOW_ID_SHIFT;
			weight_t temp_w = temp->weight;
			my_updated_weight_ = my_updated_weight_ - flow_weights[temp_f] + temp_w;
			flow_weights[temp_f] = temp_w;
		}
// MZ_DFS
		return index_dequeu;
	}
	else
	{
		// no packets in the queue
		return NO_PACKET_INDEX;
	}
}
//**********************

/**
  * Q_SFQ_enqueue_internal (int index,int flownum,int weight) - enqueues a packet in one of SFQ sub-queues
  *     //Function to enQ a SFQ packet
  */

bit Q_SFQ_enqueue_internal ( pkt_index_t index, flow_id_t flownum, weight_t weight )
{
	table_index_t relindex;
	if ( SFQsize[flownum] < NUMQBUFFS )
	{
		relindex = SFQ_head[flownum];
		// Increase size and move head forward
		SFQsize[flownum] = SFQsize[flownum] + 1;
		SFQ_head[flownum] = SFQ_head[flownum] + 1;
		if ( NUMQBUFFS <= SFQ_head[flownum] )
		{
			SFQ_head[flownum] = 0;
		}
		Stime[flownum][relindex] = START_TAG ( rtc_get_ticks(), Ftime_old[flownum] ); // start_tag(ticker, Ftime_old[flownum]);
		Ftime[flownum][relindex] = FINISH_TAG ( Stime[flownum][relindex], weight ); //finish_tag(Stime[flownum][relindex],weight);
		PKTINDEX_ACCESS ( flownum, relindex ) = index;
		// update the flow info
		Ftime_old[flownum] = Ftime[flownum][relindex];

		return 1;
	}
	else
	{
		// Error due to buffer overflow
		return 0;
	}
}


// shift by 10 positions (11->1, ... , 19->9)
// NO SHIFT -> SOURCES HAVE TO USE IDs BETWEEN flow_id_shift AND numflows
#define FLOW_ID_SHIFT 0x00

//Function to enQ a packet from index number
bit Q_SFQ_enqueue ( unsigned char index )
{
	weight_t temp_w;
	pkt_t* xdata temp = ( pkt_t* ) & ( buffer0[QBUFF_BASE ( index ) ] );
	flow_id_t temp_f = temp->src_id - FLOW_ID_SHIFT;
	if (SFQ_FIXED_WEIGHT_DFS != q_sfq_version_)
	{
		Q_SFQ_weight_update ( index );
	}
	temp_w = temp->weight;
	// Update total weight
	//my_updated_weight_ = my_updated_weight_ - flow_weights[temp_f] + temp_w;
	//flow_weights[temp_f] = temp_w;

	// Store packet
	return Q_SFQ_enqueue_internal ( index, temp_f, temp_w );
}



//**********************

//Function to check Q utilization
pkt_index_t Q_SFQ_utilization()
{
	pkt_index_t ret = 0;
	table_index_t i;
	for ( i = 0; i < NUMFLOWS; i++ )
	{
		ret += SFQsize[i];
	}
	return ret;
}
//**********************



//**********************
/**
  * finish_tag(v_time_t start_tag, int weight) - calculates a FINISH TAG for a new packet
  *     //Function to Calculate the Finish Tag of a packet
  */
/*
v_time_t finish_tag(v_time_t start_tag, weight_t weight) {
	return (start_tag + (PACKLEN/weight));
}
*/
//**********************

/**
  * start_tag(v_time_t virt_time,v_time_t finish_tag) - calculates a START TAG for a new packet
  */
/*
v_time_t start_tag(v_time_t virt_time, v_time_t old_finish_tag) {
	if (virt_time > old_finish_tag){
		return virt_time;
	}
	else{
		return old_finish_tag;
	}
}
*/
//**********************

#define QUEUE_ERROR_SCALING 10
//#define DELAY_ERROR_SCALING 200
#define DELAY_ERROR_SCALING 400

/**
  * Q_SFQ_weight_update(int index) - updates weight of a packed in BIG buffer at position "index"
  *      //Function to Update the packet weight for an indexed packet
  */
weight_t Q_SFQ_weight_update ( pkt_index_t index )
{
	int ret;
	weight_t phi_last;
	int E;
	//int error_delay; //This was original
	unsigned long int error_delay;

	unsigned long int ticker_old;
	unsigned long int delay;
	//unsigned long int errorQ;  //This was original
	int errorQ;

	pkt_t* xdata temp = ( pkt_t* ) & ( buffer0[QBUFF_BASE ( index ) ] );//Fetch Packet pointer
	ticker_old = temp -> ticker; //Point to Ticker value in packet
	phi_last = temp-> weight;
	delay = rtc_get_ticks() - ticker_old;
	// @TODO: Verify that new resolution will not mess up the calculations
	// Previously it used 1 tick = 50 ms, NEW: 1tick=1ms
	errorQ = Q_SFQ_utilization() - EXPQ;
//	error_delay = delay - EXPDELAY;
	error_delay = EXPDELAY - delay;
	//E = errorQ + ( 1 / error_delay );
	E = QUEUE_ERROR_SCALING*errorQ + ( DELAY_ERROR_SCALING / error_delay );
	ret = ALPHA * phi_last + BETA * E;

	// store the new weight
	temp-> weight = ret;

	return ret;
}
//**********************



/**
  * Q_SFQ_get_total_weight() - returns total weight ofor the node
  */
weight_t Q_SFQ_get_total_weight()
{
	if (SFQ_FIXED_WEIGHT_DFS == q_sfq_version_)
	{
		return my_weight_;
	}
	else
	{
		// ADFS
		if ( my_updated_weight_ < MIN_WEIGHT )
		{ return MIN_WEIGHT; }
		else { return my_updated_weight_; }
	}
}


//**********************
/**
  * Q_SFQ_get_total_weight() - returns total weight ofor the node
  */
void Q_SFQ_set_version(uint8_t version)
{
	
	switch (version)
	{
		case SFQ_FIXED_WEIGHT_DFS:
			q_sfq_version_ = SFQ_FIXED_WEIGHT_DFS;
			break;
		case SFQ_WEIGHT_UPDATE_ADFS:
			q_sfq_version_ = SFQ_WEIGHT_UPDATE_ADFS;
			break;
		default:
			q_sfq_version_ = SFQ_WEIGHT_UPDATE_ADFS;
	}
}


/**
  * Q_SFQ_set_weights() - sets weights for the node
  */
void Q_SFQ_set_weights(weight_t total_weight, weight_t flow_weight)
{
	uint8_t k1;
	for ( k1 = 0; k1 < NUMFLOWS; k1++ )
	{
		flow_weights[k1] = flow_weight;
	}
	my_updated_weight_ = total_weight;
}


#endif

