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

//Queuing Code for ADFS

//#include "esnl_pub.h"
#include "CongestionControl.h"
#include "HW_LIB/RTC.h"
#include "FEAT_Queuing/Queuing.h"
#include "packet.h"
#include "FEAT_Scheduler/sch_basic_pub.h"

#ifdef FEAT_ENABLE_CC

//**********************
//Define Variables

uint32_t xdata cc_processing_timeout_;
uint32_t xdata cc_timeout_length_ = DEFAULT_CC_TIMEOUT;
uint32_t xdata cc_period_length_ ;
uint32_t xdata cc_last_period_tick_ ;

uint16_t xdata cc_generated_pkts_;
uint16_t xdata cc_sent_pkts_;
uint16_t xdata cc_recv_pkts_;
uint16_t xdata cc_drop_pkts_;

uint16_t xdata cc_timeout_divider_;

uint16_t xdata cc_throughput_;
uint16_t xdata cc_predicted_throughput_;
uint16_t xdata cc_previous_throughput_;
uint16_t xdata cc_droprate_;

float xdata cc_flow_alpha_;
float xdata cc_flow_gamma_;
float xdata cc_flow_error_;
float xdata cc_kv_queue_gain_;

uint8_t xdata cc_ideal_queue_;
int16_t xdata cc_queue_error_;

uint16_t xdata cc_recv_outflow_limit_;
uint16_t xdata cc_calculated_inflow_limit_;
uint16_t xdata cc_ownflow_limit_;
uint16_t xdata cc_target_outflow_;
uint16_t xdata cc_previous_target_outflow_ ;


float xdata cc_bo_alpha_;
float xdata cc_bo_sigma_;
float xdata cc_bo_outflow_error_;
float xdata cc_bo_kv_;

uint16_t xdata cc_bo_value_;

uint8_t xdata	cc_need_to_send_upstream_;

uint8_t xdata	cc_upstream_nodes_count_; // number of nodes in the below tables
uint16_t xdata	cc_upstream_nodes_[MAX_UPSTREAM_NODES]; // MAC address of upstraem nodes
uint8_t xdata	cc_upstream_nodes_status_[MAX_UPSTREAM_NODES]; // status (need to send limit, sending limit, done sending)
uint8_t xdata	cc_upstream_nodes_frame_id_[MAX_UPSTREAM_NODES];


// ##############################################################################
// ## Misc counters
// ##############################################################################
#ifdef ENABLE_CC_DEBUG_COUNTERS
uint16_t xdata cc_count_data_lost_;
uint16_t xdata cc_count_upstream_lost_=0;
uint16_t xdata cc_count_scan_lost_;
#endif


/// == sizeof(pkt_v2_PCCup_t)
#ifdef FEAT_ENABLE_CC
	char XDATA temp_p[20];
#endif
//**********************
/**
  *
  */
void cc_init ( bit full_reset )
{
	if ( full_reset )
	{
		cc_timeout_length_ = DEFAULT_CC_TIMEOUT;
	}
	cc_processing_timeout_ = rtc_get_ticks() + cc_timeout_length_;
	cc_generated_pkts_ = 0;
	cc_period_length_ = 1;
	cc_last_period_tick_ = rtc_get_ticks();

	cc_sent_pkts_ = 0;
	cc_recv_pkts_ = 0;
	cc_drop_pkts_ = 0;

	cc_timeout_divider_ = cc_timeout_length_ / DEFAULT_ONE_SECOND;
	cc_throughput_ = 0;
	cc_predicted_throughput_ = 0;
	cc_previous_throughput_ = 0;
	cc_droprate_ = 0;

	cc_flow_alpha_ = 1.0;
	cc_flow_gamma_ = 0.001;

	cc_kv_queue_gain_ = 0.9;
	cc_ideal_queue_ = CC_TARGET_QUEUE;

	cc_recv_outflow_limit_ = CC_MAX_OUT_FLOW;
	cc_calculated_inflow_limit_ = cc_ideal_queue_; // initially count in only the ideal queue level
	cc_ownflow_limit_ = CC_MAX_OUT_FLOW;

	cc_target_outflow_ = cc_calculated_inflow_limit_;
	cc_previous_target_outflow_ = cc_calculated_inflow_limit_;

	cc_bo_alpha_ = 1.0;
	cc_bo_sigma_ = 0.01;
	cc_bo_outflow_error_ = 0;
	cc_bo_kv_ = 0.1;

	cc_bo_value_ = PCC_DEFAULT_BO_VALUE;

	cc_need_to_send_upstream_ = PCC_JNL_NUMBER_OF_UPSTREAM;
	cc_upstream_nodes_count_ = PCC_JNL_NUMBER_OF_UPSTREAM; // number of nodes in the below tables
#ifdef SOURCE
	cc_upstream_nodes_count_--;
#endif

	{
		uint8_t i;
		for ( i = 0; i < MAX_UPSTREAM_NODES; i++ )
		{
			cc_upstream_nodes_[MAX_UPSTREAM_NODES] = 0x0F; // MAC address of upstraem nodes
			cc_upstream_nodes_status_[MAX_UPSTREAM_NODES] = 0x0F; // status (need to send limit, sending limit, done sending)
			cc_upstream_nodes_frame_id_[MAX_UPSTREAM_NODES] = 0x0F;
		}
	}

#ifdef NODE_5_PCC_RELAY
	cc_upstream_nodes_[0] = 0x01;
	cc_upstream_nodes_[1] = 0x02;
#endif
#ifdef NODE_7_PCC_SOURCE
	cc_upstream_nodes_[0] = 0x02;
#endif

	sch_add_loop((sch_loop_func_t)cc_loop);
}


/**
 *  cc_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
 */
void cc_loop( void )
{
#ifdef FEAT_ENABLE_CC
		// Check if need to do the periodic update of Congestion Control
		if ( ( cc_processing_timeout_ < rtc_get_ticks() ) )
		{
			cc_end_of_period ();
		}
#endif
#ifdef INCLUDE_PCC
		if ( ( 1 == DATA_waiting_4_BO_ ) && ( phy_backoff_timeout_ < rtc_get_ticks() ) )
		{
			DATA_waiting_4_BO_ = 0;
#ifdef _ENABLE_NETWORK_STACK_
			phy_transmit_packet();
#endif // _ENABLE_NETWORK_STACK_
		}
#endif

#if (0)
		//FEAT_ENABLE_CH_SWITCHING
		if ( (CS_ENABLED == cs_enabled_) && (cs_switching_timeout_ < rtc_get_ticks() ) )
		{
			cs_switching_timeout();
			cs_send_join ( 0xFE, 0x11 );

		}
#endif
}


/**
  *
  */
void cc_change_protocol()
{
	if (PHY_BACKOFF_NO_PCC != my_backoff_)
	{
		cc_need_to_send_upstream_ = PCC_JNL_NUMBER_OF_UPSTREAM;
		cc_upstream_nodes_count_ = PCC_JNL_NUMBER_OF_UPSTREAM; // number of nodes in the below tables
#ifdef SOURCE
		cc_upstream_nodes_count_--;
#endif
		cc_change_timeout ( DEFAULT_CC_TIMEOUT );
		cc_bo_value_ = PCC_DEFAULT_BO_VALUE-1;
	}
	else
	{
		cc_need_to_send_upstream_ = 0;
		cc_change_timeout ( DEFAULT_ONE_SECOND );
		cc_bo_value_ = PCC_DEFAULT_BO_VALUE;
	}
}


/**
  *
  */
void cc_change_timeout ( uint32_t timeout )
{
	if ( CC_TIMEOUT_MIN > timeout )
	{
		cc_timeout_length_ = CC_TIMEOUT_MIN;
	}
	else if ( CC_TIMEOUT_MAX < timeout )
	{
		cc_timeout_length_ = CC_TIMEOUT_MAX;
	}
	else
	{
		cc_timeout_length_ = timeout;
	}
}


/**
  *
  */
void cc_end_of_period ()
{
	float throughput;
	float alpha_delta ;
	// Check if received duplicates of the period request
	if ( rtc_get_ticks() - cc_last_period_tick_ < CC_MIN_PERIOD )
	{
		return;
	}
	cc_period_length_ = rtc_get_ticks() - cc_last_period_tick_;
	cc_last_period_tick_ = rtc_get_ticks();
//	throughput = cc_sent_pkts_ / cc_timeout_divider_;
	throughput = (float)(cc_sent_pkts_ * DEFAULT_ONE_SECOND) / (float)cc_period_length_;

	cc_flow_error_ = throughput - ( float ) cc_predicted_throughput_;
	alpha_delta = cc_flow_gamma_ * throughput * cc_flow_error_;
//	alpha_delta = cc_flow_gamma_ * cc_throughput_ * cc_flow_error_;

	cc_flow_alpha_ += alpha_delta;
	cc_flow_alpha_ = MAX ( cc_flow_alpha_, MIN_FLOW_ALPHA );
	cc_flow_alpha_ = MIN ( cc_flow_alpha_, MAX_FLOW_ALPHA );

	cc_predicted_throughput_ = ( uint16_t ) ( throughput * cc_flow_alpha_ );


	cc_previous_throughput_ = cc_throughput_;
	cc_throughput_ = ( uint16_t ) throughput;
	cc_droprate_ = cc_drop_pkts_ * DEFAULT_ONE_SECOND / cc_period_length_;
	cc_processing_timeout_ = rtc_get_ticks() + cc_timeout_length_;
	cc_generated_pkts_ = 0;
	cc_sent_pkts_ = 0;
	cc_recv_pkts_ = 0;
	cc_drop_pkts_ = 0;
	// Delayed setting of the timeoutr divider to accomodate for
	// change of timeout in the middle of timeout period
	cc_timeout_divider_ = cc_timeout_length_ / DEFAULT_ONE_SECOND;

/////// !!!!!! how to enable case when the PCC upstrema is comming a bit earlier then own PERIOD end
//	cc_recv_outflow_limit_ = CC_MAX_OUT_FLOW; // reset limit -> if will receive the update then keep it

	// Calculate and send the info only when the PCC backoff is used
	if (PHY_BACKOFF_PCC == my_backoff_)
	{
		// Calculate the allowed incomming traffic
		cc_calculate_allowed_inflow();

		// Prepare the ConCon updates for all incomming/own flows
		cc_apply_cc_to_own_flows();
		cc_prepare_cc_messages_for_inflows();

		cc_recalculate_BO_params();
	}
}


// ##############################################################################
// ##
// ##############################################################################


// ##############################################################################
// ## Function Section
// ##############################################################################

//**********************

/**
  * cc_calculate_allowed_inflow() - Calculate the allowed incomming traffic
  */
void cc_calculate_allowed_inflow()
{
	int16_t temp_i;
	// Implement buffer management from the paper
	// queue utilization error = difference between actual and ideal queue level
	cc_queue_error_ = ( int16_t ) ( NUMQBUFFS - que_buffer_empty_slots_end_ ) - ( int16_t ) cc_ideal_queue_;
	temp_i = ( int16_t ) ( cc_kv_queue_gain_ * cc_queue_error_ );

//	cc_target_outflow_ = MIN(cc_calculated_inflow_limit_, cc_recv_outflow_limit_);

	cc_previous_target_outflow_ = cc_target_outflow_;
#ifdef SOURCE
	cc_target_outflow_ = cc_recv_outflow_limit_;
#else
	cc_target_outflow_ = cc_predicted_throughput_;
#endif

//	cc_target_outflow_ = MIN(cc_predicted_throughput_, cc_recv_outflow_limit_);
	cc_target_outflow_ = MAX ( cc_target_outflow_ , 1 );

	if ( 0 > temp_i )
	{
		cc_calculated_inflow_limit_ = cc_target_outflow_ - temp_i;
	}
	else if ( ( uint16_t ) temp_i > cc_target_outflow_ )
	{
		cc_calculated_inflow_limit_ = 0;
	}
	else
	{
		cc_calculated_inflow_limit_ = cc_target_outflow_ - ( uint16_t ) temp_i;
	}
}


/**
  * cc_apply_cc_to_own_flows() - Restric own flows
  */
void cc_apply_cc_to_own_flows()
{
#ifdef SOURCE
	// Set limits on own flows
	//  - either simply restric packet creation rate
	//  - or modify datacollection method
	// Use OWN SOURCE WEIGTH to calculate number of allowed packets
	if (PHY_BACKOFF_NO_PCC != my_backoff_)
	{
		cc_ownflow_limit_ = cc_calculated_inflow_limit_ / PCC_JNL_NUMBER_OF_UPSTREAM;
		// cc_ownflow_limit_ = cc_target_outflow_ / PCC_JNL_NUMBER_OF_UPSTREAM;
	}
#endif
}


/**
  * cc_prepare_cc_messages_for_inflows() - Prepare the ConCon updates for all incomming/own flows
  */
void cc_prepare_cc_messages_for_inflows()
{
	// Check if there are upstream nodes (be aware of own source)
#ifdef SOURCE
	if ( 2 > cc_upstream_nodes_count_ )
		return;
#else
	if ( 1 > cc_upstream_nodes_count_ )
		return;
#endif
	{
		pkt_v2_PCCup_t *pcc_pkt = ( pkt_v2_PCCup_t * ) temp_p;
		uint8_t i;
		// Fill the packet in
		uint8_t len = sendPCCupstream ( temp_p );

		// for each upastream node update DST fields
//		for ( i = 0; i < cc_upstream_nodes_count_; i++ )
		for ( i = 0; i < 1; i++ )
		{
			pcc_pkt->mac_dst = cc_upstream_nodes_[i]; // iterate through list to set correct value
			pcc_pkt->dst_id = cc_upstream_nodes_[i]; //

#ifdef PCC_DEBUG_UPSTREAM_MSG
			cc_upstream_nodes_frame_id_[i] = api_send_packet16 ( temp_p, len, 0xffff );
#else
			cc_upstream_nodes_frame_id_[i] = api_send_packet16 ( temp_p, len, cc_upstream_nodes_[i] );
#endif
			cc_upstream_nodes_status_[i] = PCC_UPSTREAM_NODE_STATUS_SENDING;
		}
		cc_need_to_send_upstream_ = cc_upstream_nodes_count_; // Need to send messages -> check if received OK
	}
}



/**
  * cc_recv_API_ack(frame_id, status) - handle reception of ACK when sending
  *		PCC upstream commands,
  *		returns 1 if found PCC message, or 0 if this belongs to another packet
  */
uint8_t cc_recv_API_ack ( uint8_t frame_id, uint8_t status )
{
	uint8_t i;
	// for each upastream node update DST fields
	for ( i = 0; i < cc_upstream_nodes_count_; i++ )
	{
		if ( frame_id == cc_upstream_nodes_frame_id_[i] )
		{
			if ( TX_STATUS_OK == status )
			{
				// cc_upstream_nodes_frame_id_[i];
				cc_upstream_nodes_status_[i] = PCC_UPSTREAM_NODE_STATUS_IDLE;
				cc_need_to_send_upstream_--;
			}
			else
			{
				pkt_v2_PCCup_t *pcc_pkt = ( pkt_v2_PCCup_t * ) temp_p;
				uint8_t len;
#ifdef ENABLE_CC_DEBUG_COUNTERS
				cc_count_upstream_lost_++;
#endif
				// If exceeded retransmission limit, then stop
				if ( CC_MAX_UPSTREAM_RETRIES < cc_upstream_nodes_status_[i] )
				{
					cc_upstream_nodes_status_[i] = PCC_UPSTREAM_NODE_STATUS_IDLE;
					cc_need_to_send_upstream_--;
					return 1;
				}
				// Fill the packet in
				len = sendPCCupstream ( temp_p );
				pcc_pkt->mac_dst = cc_upstream_nodes_[i]; // iterate through list to set correct value
				pcc_pkt->dst_id = cc_upstream_nodes_[i]; //
#ifdef PCC_DEBUG_UPSTREAM_MSG
				cc_upstream_nodes_frame_id_[i] = api_send_packet16 ( temp_p, len, 0xffff );
#else
				cc_upstream_nodes_frame_id_[i] = api_send_packet16 ( temp_p, len, cc_upstream_nodes_[i] );
#endif
				cc_upstream_nodes_status_[i] ++; // count retransmissions
			}
			return 1; // since found it is done
		}
	}
	return 0; // not found frame_id -> maybe the ACK is for another messages
}


/**
  * sendPCCupstream - prepare PCC upstream message
  */
unsigned int sendPCCupstream ( char*p )
{
	pkt_v2_PCCup_t *pcc_pkt = ( pkt_v2_PCCup_t * ) p;

	pcc_pkt->start = START_BYTE;
	pcc_pkt->flag = FLAG_PCC_V2;
	pcc_pkt->mac_dst = 0xFFFF; // iterate through list to set correct value
	pcc_pkt->mac_src = MY_ADDR;
	pcc_pkt->length = SIZE_OF_PCC_UPSTREAM_PKT; // of the packet'd data
	pcc_pkt->dst_id = 0xFF; //
	pcc_pkt->src_id = MY_ADDR; // part of DATA field

	pcc_pkt->module_type = PKT_MODULE_TYPE_PCC_UPSTREAM;
	pcc_pkt->module_len = PKT_MODULE_LEN_PCC_UPSTREAM;
	pcc_pkt->flow_limit = cc_calculated_inflow_limit_ / cc_upstream_nodes_count_;
	pcc_pkt->misc = 0xAA;
	pcc_pkt->next_module_type = PKT_MODULE_TYPE_END;

	return SIZE_OF_PCC_UPSTREAM_PKT;
}





/**
  * cc_recv_PCC_upstream(pkt) - handle reception of PCC upstream message
  *		apply to the BO and
  */
void cc_recv_PCC_upstream ( char *p )
{
	pkt_v2_PCCup_t *pcc_pkt = ( pkt_v2_PCCup_t * ) p;
	cc_recv_outflow_limit_ = pcc_pkt->flow_limit;

	cc_end_of_period();
}




int8_t cc_checkAllowedSourceFlow()
{
	if (PHY_BACKOFF_NO_PCC != my_backoff_)
	{
		// add one always to allow at least one packet ??? DO WE NEED THIS? ???
		uint16_t allowed = 0;
		if ( ( 0 == cc_ownflow_limit_ ) && ( 0 == cc_generated_pkts_ ) )
			allowed = 1; // case when there is no flow allowed
		else if ( cc_ownflow_limit_ > cc_generated_pkts_ )
			allowed = cc_ownflow_limit_ - cc_generated_pkts_;
		if ( allowed > 0xFE )
			allowed = 0xFE;
		return 	allowed;
	}
	return 1; // always allow for NO PCC case
}

// ##############################################################################
// ##
// ##############################################################################



/**
  * cc_recalculate_BO_params() - recalculates backoff params (e.g. backoff window)
  */
void cc_recalculate_BO_params()
{
// Update only when this is a source node - for relay do not change (fixed BO)
#ifdef SOURCE
	float alpha_delta;
	float b, v;
	cc_bo_outflow_error_ = cc_throughput_ - cc_previous_target_outflow_;
	alpha_delta = cc_bo_sigma_ * cc_previous_throughput_ * cc_bo_outflow_error_;
	cc_bo_alpha_ += alpha_delta;

	cc_bo_alpha_ = MAX ( cc_bo_alpha_, PCC_MIN_BO_ALPHA );
	cc_bo_alpha_ = MIN ( cc_bo_alpha_, PCC_MAX_BO_ALPHA );

	b = cc_bo_value_ * cc_throughput_;
	v = cc_target_outflow_ - cc_bo_alpha_ * cc_throughput_ + cc_bo_kv_ * cc_bo_outflow_error_;
	cc_bo_value_ = b / v;

	cc_bo_value_  = MAX ( cc_bo_value_ , PCC_MIN_BO_VALUE );
	cc_bo_value_  = MIN ( cc_bo_value_ , PCC_MAX_BO_VALUE );
#endif

// cc_previous_throughput_
// cc_bo_alpha_;
// cc_bo_sigma_;
// cc_bo_outflow_error_;
// cc_bo_kv_;


//	cc_bo_alpha_
//	cc_flow_error_
//	cc_target_outflow_
}

#endif



