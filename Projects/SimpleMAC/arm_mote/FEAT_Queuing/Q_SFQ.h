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
#ifndef _SFQ_H_
#define _SFQ_H_

#include "defs/esnl_pub.h"

#ifdef FEAT_ENABLE_SFQ


#include "common.h"
#include "Queuing.h"

//Header File for SFQ Implementation


//#define QUEUE_SPECIFIC_INIT() SFQ_init()

//JWF
//
#define MIN_WEIGHT 1
//#define MAX_WEIGHT 255
// The shift between source id and flow id
#define FLOW_ID_SHIFT 0x00
//


typedef unsigned char pkt_index_t;	// index of a packet in the BIG buffer
typedef unsigned char flow_id_t;	// id of a flow
typedef unsigned long int v_time_t;	// Virtual time used by the SFQ buffers
typedef unsigned long int r_time_t; // Real-time timer ticks
typedef int	weight_t;	// weight type (integer version expressed in # of 1000ths)
typedef unsigned char table_index_t;	// index in a queue tables
typedef unsigned char table_size_t;	// variable to store size of a table/buffer/queue


#define NUMFLOWS 30
#define PACKLEN 100
#define ALPHA .4
#define BETA .6
//#define EXPDELAY 20
#define EXPDELAY 1*TICKERS_PER_SECOND
#define EXPQ 5

//**********************

//Define External Variables

extern table_index_t xdata SFQ_head[NUMFLOWS];
extern table_index_t xdata SFQ_tail[NUMFLOWS];
extern table_size_t xdata SFQsize[NUMFLOWS];
extern v_time_t	xdata Ftime_old[NUMFLOWS];
extern v_time_t xdata Ftime[NUMFLOWS][NUMQBUFFS];
extern v_time_t xdata Stime[NUMFLOWS][NUMQBUFFS];
extern pkt_index_t xdata pktindex[NUMFLOWS * NUMQBUFFS];
//extern table_index_t Nextpkt[NUMFLOWS][NUMQBUFFS];

extern uint8_t xdata q_sfq_version_;

//**********************

//Function Prototypes

/**
  * Q_SFQ_init() - initializes all queue related variables
  */
void Q_SFQ_init();

pkt_index_t Q_SFQ_dequeue();
//#define que_deQpackets() deSFQ()

/**
  * Q_SFQ_enqueue_internal (int index,int flownum,int weight) - enqueues a packet in one of SFQ sub-queues
  */
bit Q_SFQ_enqueue_internal ( pkt_index_t index, flow_id_t flownum, weight_t weight );
//Function to enQ a packet from index number
bit Q_SFQ_enqueue ( unsigned char index );

//Function to check Q utilization
pkt_index_t Q_SFQ_utilization();

/**
  * finish_tag(v_time_t start_tag, int weight) - calculates a FINISH TAG for a new packet
  */
//v_time_t finish_tag(v_time_t start_tag, weight_t weight);
#define FINISH_TAG(start_tag, weight) (start_tag + (PACKLEN/weight))

/**
  * start_tag(v_time_t virt_time,v_time_t finish_tag) - calculates a START TAG for a new packet
  */
//v_time_t start_tag(v_time_t virt_time, v_time_t old_finish_tag);
#define START_TAG(virt_time, old_finish_tag) ( (virt_time > old_finish_tag)?virt_tag:old_finish_tag )

/**
  * Q_SFQ_weight_update(int index) - updates weight of a packed in BIG buffer at position "index"
  */
weight_t Q_SFQ_weight_update ( pkt_index_t index );


/**
  * Q_SFQ_get_total_weight() - returns total weight ofor the node
  */
weight_t Q_SFQ_get_total_weight();


/**
  * Q_SFQ_set_version() - sets version of queueing/scheduling (ADFS/DFS)
  */
void Q_SFQ_set_version(uint8_t version);

/**
  * Q_SFQ_set_weights() - sets weights for the node
  */
void Q_SFQ_set_weights(weight_t total_weight, weight_t flow_weight);


#endif // FEAT_ENABLE_SFQ

#endif

