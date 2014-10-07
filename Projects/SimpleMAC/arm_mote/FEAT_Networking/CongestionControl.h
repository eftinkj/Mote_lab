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
#ifndef _CONGESTION_CONTROL_H_
#define _CONGESTION_CONTROL_H_

//#include "feat_enable.h"
#include "defs/esnl_pub.h"
#include "packet.h"

#ifdef FEAT_ENABLE_CC


//**********************
#define DEFAULT_ONE_SECOND TICKERS_PER_SECOND

#define DEFAULT_CC_TIMEOUT_SECONDS 1
#ifdef SOURCE
	// for source allow longer timeout to accomodate for delayed reception of UPSTREAM update
	#define DEFAULT_CC_TIMEOUT DEFAULT_ONE_SECOND*(DEFAULT_CC_TIMEOUT_SECONDS+1)
#else
	#define DEFAULT_CC_TIMEOUT DEFAULT_ONE_SECOND*DEFAULT_CC_TIMEOUT_SECONDS
#endif

// Minimum time between UPSTREAM updates (to avoid duplicates)
#define CC_MIN_PERIOD DEFAULT_ONE_SECOND / 2

//#define CC_TIMEOUT_MIN 1
//#define CC_TIMEOUT_MAX 200
#define CC_TIMEOUT_MIN TICKERS_PER_SECOND / 20
#define CC_TIMEOUT_MAX TICKERS_PER_SECOND * 2

#define MAX_UPSTREAM_NODES 4
// Allow as much as possible - unless restriced by down flow
#define CC_MAX_OUT_FLOW 0x40

#define MIN_FLOW_ALPHA 0
#define MAX_FLOW_ALPHA 10

//#define PCC_DEFAULT_BO_VALUE	2
//#define PCC_MIN_BO_VALUE	1
//#define PCC_MAX_BO_VALUE	10
#define PCC_DEFAULT_BO_VALUE	TICKERS_PER_SECOND/10
#define PCC_MIN_BO_VALUE	TICKERS_PER_SECOND/20
#define PCC_MAX_BO_VALUE	TICKERS_PER_SECOND/2


#define PCC_MIN_BO_ALPHA	-0.9
#define PCC_MAX_BO_ALPHA	0.9


#define CC_TARGET_QUEUE_UTILIZATION_FRACTION 0.8
#define CC_TARGET_QUEUE NUMQBUFFS * CC_TARGET_QUEUE_UTILIZATION_FRACTION


#define PCC_UPSTREAM_NODE_STATUS_IDLE	 0x00
#define PCC_UPSTREAM_NODE_STATUS_SENDING 0x01

#define CC_MAX_UPSTREAM_RETRIES 2
//**********************
extern uint32_t xdata cc_processing_timeout_;
extern uint32_t xdata cc_timeout_length_;
extern uint32_t xdata cc_period_length_ ;
extern uint32_t xdata cc_last_period_tick_ ;

extern uint16_t xdata cc_generated_pkts_;
extern uint16_t xdata cc_sent_pkts_;
extern uint16_t xdata cc_recv_pkts_;
extern uint16_t xdata cc_drop_pkts_;

extern uint16_t xdata cc_timeout_divider_;

extern uint16_t xdata cc_throughput_;
extern uint16_t xdata cc_predicted_throughput_;
extern uint16_t xdata cc_droprate_;

extern float xdata cc_flow_alpha_;
extern float xdata cc_flow_gamma_;
extern float xdata cc_flow_error_;



extern float xdata cc_kv_queue_gain_;
extern uint8_t xdata cc_ideal_queue_;
extern int16_t xdata cc_queue_error_;

extern uint16_t xdata cc_recv_outflow_limit_;
extern uint16_t xdata cc_calculated_inflow_limit_;
extern uint16_t xdata cc_ownflow_limit_;
extern uint16_t xdata cc_target_outflow_;
extern uint16_t xdata cc_previous_target_outflow_ ;

extern float xdata cc_bo_alpha_;
extern float xdata cc_bo_sigma_;
extern float xdata cc_bo_outflow_error_;
extern float xdata cc_bo_kv_;

extern uint16_t xdata cc_bo_value_;

extern uint8_t xdata	cc_need_to_send_upstream_;

extern uint8_t xdata	cc_upstream_nodes_count_; // number of nodes in the below tables
extern uint16_t xdata	cc_upstream_nodes_[MAX_UPSTREAM_NODES]; // MAC address of upstraem nodes
extern uint8_t xdata	cc_upstream_nodes_status_[MAX_UPSTREAM_NODES]; // status (need to send limit, sending limit, done sending)
extern uint8_t xdata	cc_upstream_nodes_frame_id_[MAX_UPSTREAM_NODES];


// ##############################################################################
// ## Misc counters
// ##############################################################################
#ifdef ENABLE_CC_DEBUG_COUNTERS
extern uint16_t xdata cc_count_data_lost_;
extern uint16_t xdata cc_count_upstream_lost_;
extern uint16_t xdata cc_count_scan_lost_;
#endif

// ##############################################################################
// ## Function Section
// ##############################################################################

/**
  * TEMPLATE
  */

/**
  *
  */
void cc_init ( bit full_reset );

/**
 *  cc_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
 */
void cc_loop( void );

/**
  *
  */
void cc_change_protocol();


/**
  *
  */
void cc_change_timeout ( uint32_t timeout );

/**
  *
  */
void cc_end_of_period ();

/**
  * cc_calculate_allowed_inflow() - Calculate the allowed incomming traffic
  */
void cc_calculate_allowed_inflow();


/**
  * cc_apply_cc_to_own_flows() - Restric own flows
  */
void cc_apply_cc_to_own_flows();


/**
  * cc_prepare_cc_messages_for_inflows() - Prepare the ConCon updates for all incomming/own flows
  */
void cc_prepare_cc_messages_for_inflows();

/**
  * cc_recv_API_ack(frame_id, status) - handle reception of ACK when sending
  *		PCC upstream commands,
  *		returns 1 if found PCC message, or 0 if this belongs to another packet
  */
uint8_t cc_recv_API_ack ( uint8_t frame_id, uint8_t status );

/**
  * sendPCCupstream(pkt_buffer) - prepare PCC upstream message
  */
unsigned int sendPCCupstream ( char * );



/**
  *
  */
void cc_recv_PCC_upstream ( char *p );


/**
  *
  */
int8_t cc_checkAllowedSourceFlow();


/**
  * cc_recalculate_BO_params() - recalculates backoff params (e.g. backoff window)
  */
void cc_recalculate_BO_params();


#endif

#endif

