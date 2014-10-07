
#include "defs/esnl_pub.h"

#ifdef _ENABLE_LOGISTICS_01_


/****************************************************************************
**	Includes (C file)
****************************************************************************/

#include "lgs_basic_prv.h"
//#include "lgs_basic_pub.h"

#include "common_constants.h"
#include "FEAT_Networking/tsp_common_pub.h"
#include "HW_LIB/RTC.h"
#include "FEAT_Scheduler/sch_basic_pub.h"

#include "packet.h"
#include "sys_func.h"


#include "common.h"



/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

#ifdef _ENABLE_DLS_GATE_MOTE_
rtc_tick_t lgs_gatemote_timeout;
#endif // _ENABLE_DLS_GATE_MOTE_


#ifdef _ENABLE_DLS_TRUCK_MOTE_
//uint8_t lgs_truck_state_;
//uint8_t lgs_truck_assoc_;
	rtc_tick_t lgs_truckmote_timeout_;
#endif // _ENABLE_DLS_TRUCK_MOTE_

// Periodic inventory report while palette arround RFID reader (count RFID reads)
uint8_t lgs_palette_periodic_report_couter;

uint8_t		rfids_[LGS_TAG_ID_SIZE * LGS_MAX_TAGS];
uint16_t	lgs_tag_type_[LGS_MAX_TAGS];

uint8_t	lgs_assoc_l1[LGS_MAX_TAGS];
uint8_t	lgs_assoc_l2[LGS_MAX_TAGS];
uint8_t	lgs_assoc_l3[LGS_MAX_TAGS];
uint8_t	lgs_assoc_l4[LGS_MAX_TAGS];

lgs_tag_index_t	lgs_tag_count_;

uint8_t	my_assoc_l1_;
uint8_t	my_assoc_l2_;
uint8_t	my_assoc_l3_;
uint8_t	my_assoc_l4_;

#define LGS_ASSOC_STATUS_IDLE		0x00
#define LGS_ASSOC_STATUS_PENDING	0x01
#define LGS_ASSOC_STATUS_ASSOC_L1	0x02
#define LGS_ASSOC_STATUS_ASSOC_L2	0x03

uint8_t lgs_assoc_status_;
uint8_t lgs_pending_node_id_;
uint8_t lgs_pending_node_type_;
uint8_t lgs_pending_node_rssi_;
rtc_tick_t lgs_assoc_request_timeout_;
rtc_tick_t lgs_assoc_timeout_;

// Handle retransmission of reports to BS
	uint8_t lgs_last_event_type;
	uint8_t lgs_last_r_node_id;
	uint8_t lgs_last_r_node_type;
	uint8_t lgs_last_rssi;
	uint8_t lgs_last_count_;
	rtc_tick_t	lgs_last_timeout_;

	#define LGS_LAST_TIMEOUT 5000
	#define LGS_LAST_COUNT_MAX 1

/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/


/**
  * lgs_init () - sets up Logistics Module
  */
void lgs_init( void )
{
#ifdef MZ_TEMP_TEST_SCH_HIO
	sch_add_loop(( sch_loop_func_t )lgs_loop );
#endif // MZ_TEMP_TEST_SCH_HIO


#ifdef _ENABLE_DLS_GATE_MOTE_
	lgs_gatemote_timeout = 0;
#endif // _ENABLE_DLS_GATE_MOTE_

#ifdef _ENABLE_DLS_TRUCK_MOTE_
//	lgs_truck_state_ = LGS_TRUCK_IDLE;
//	lgs_truck_assoc_ = LGS_MAIN_WEAREHOUSE;
//	lgs_assoc_request_timeout_ = RTC_OVERFLOW_TIMER_VALUE; // no associacion timeout
	lgs_truckmote_timeout_ = 0;
#endif // _ENABLE_DLS_TRUCK_MOTE_

	lgs_clear_tag_memory();

	lgs_palette_periodic_report_couter = 0;

	my_assoc_l1_ = LGS_DP_NONE;
	my_assoc_l2_ = LGS_TRUCK_NONE;
	my_assoc_l3_ = LGS_PALETTE_NONE;
	my_assoc_l4_ = LGS_ORDER_NONE;

	
	lgs_assoc_status_= LGS_ASSOC_STATUS_IDLE;
	lgs_pending_node_id_ = LGS_ASSOC_NONE;
	lgs_pending_node_type_ = LGS_ASSOC_NONE;
	lgs_pending_node_rssi_ = MAX_RSSI;
	lgs_assoc_request_timeout_ = RTC_OVERFLOW_TIMER_VALUE;
	lgs_assoc_timeout_ = RTC_OVERFLOW_TIMER_VALUE;

	lgs_last_event_type = 0;
	lgs_last_r_node_id = 0;
	lgs_last_r_node_type = 0;
	lgs_last_rssi = 0;
	lgs_last_count_ = 0; //0 disables the retransmissions
	lgs_last_timeout_ = RTC_OVERFLOW_TIMER_VALUE; 

}


/**
  * lgs_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void lgs_loop()
{
	if (0 < lgs_last_count_)
	{
		if ( rtc_get_ticks() > lgs_last_timeout_ )
		{
			lgs_report_event(lgs_last_event_type, lgs_last_r_node_id, lgs_last_r_node_type, lgs_last_rssi, LGS_RELIABLE_TX_CONTINUE);
		}
	}

#ifdef _ENABLE_DLS_GATE_MOTE_
	if ( rtc_get_ticks() > lgs_gatemote_timeout )
	{
		lgs_gatemote_timeout += LGS_GATEMOTE_DEFAULT_TIMEOUT;
		lgs_send_mote_beacon();
	}
#endif // _ENABLE_DLS_GATE_MOTE_

#ifdef _ENABLE_DLS_TRUCK_MOTE_
	if ( rtc_get_ticks() > lgs_truckmote_timeout_ )
	{
		lgs_truckmote_timeout_ += LGS_TRUCKMOTE_DEFAULT_TIMEOUT;
		lgs_send_mote_beacon();
/*
uint8_t lgs_bcast_assoc_intercount_ = 0;
#define LGS_BCAST_ASSOC_MAX_COUNT	5;
		if (lgs_bcast_assoc_intercount_++ > LGS_BCAST_ASSOC_MAX_COUNT)
		{
			
		}
*/
	}
	if (( LGS_ASSOC_STATUS_PENDING == lgs_assoc_status_ ) 
		&& ( rtc_get_ticks() > lgs_assoc_request_timeout_ ) )
	{
		// Timed out the request for association
		// Temp: clear associacion for now
			lgs_assoc_status_= LGS_ASSOC_STATUS_IDLE;
			lgs_pending_node_id_ = LGS_ASSOC_NONE;
			lgs_pending_node_type_ = LGS_ASSOC_NONE;
			lgs_pending_node_rssi_ = MAX_RSSI; 
			lgs_report_event( LGS_ASSOC_ATTEMPT_FAILED, lgs_pending_node_id_, lgs_pending_node_type_, 0, LGS_RELIABLE_TX);
	}
	else if (( LGS_ASSOC_STATUS_ASSOC_L1 == lgs_assoc_status_ ) 
		|| (LGS_ASSOC_STATUS_ASSOC_L2 == lgs_assoc_status_ ) )
	{
		if (rtc_get_ticks () > lgs_assoc_timeout_)
		{
			lgs_send_assoc_req( lgs_pending_node_id_, lgs_pending_node_type_, LGS_REQ_DEASSOC, MAX_RSSI);
			lgs_report_event( LGS_ASSOC_TIMEDOUT, lgs_pending_node_id_, lgs_pending_node_type_, 0, LGS_RELIABLE_TX);
		}
	}

#endif // _ENABLE_DLS_TRUCK_MOTE_

}


/**
  * lgs_recv_command(module, rssi) - receive a misc LGS packet (e.g. REPORT)
  */
void lgs_recv_command( pkt_mod_header_t xdata *module, uint8_t rssi )
{
	if (PKT_MODULE_TYPE_LGS_MOTE_REPORT == module->module_type)
	{
		if (LGS_ACK_REPORT == module->content ) // report_type
		{
			lgs_cancel_pending_report();
		}
	}
}


//#ifdef _ENABLE_RFID_READER_DETECTION_
/**
  * lgs_recv_mote_beacon() - when received beacon from a mote (truck) (do I join/associate??)
  */
void lgs_recv_mote_beacon( pkt_mod_header_t xdata *module, uint8_t rssi )
{
	lgs_mote_beacon_t *ph = ( lgs_mote_beacon_t * )module ;

	// should be: (ph->report_type == LGS_MOTE_BEACON_V1)
	uint8_t mid = ph->node_id;
	uint8_t mtype = ph->node_type;

	if ( mtype >= LGS_NODE_TYPE ) {return;}	// if the same or lower level (do not care)

	if ((LGS_ASSOC_STATUS_ASSOC_L1 == lgs_assoc_status_) 
		|| (LGS_ASSOC_STATUS_ASSOC_L2 == lgs_assoc_status_))
	{
		if (( mid == lgs_pending_node_id_) && (mtype == lgs_pending_node_type_))
		{
			// already associated with that node -> update timeout 
			// 		(it will NOT conflict with DEASSOC case of RSSI)
			lgs_assoc_timeout_ = rtc_get_ticks() + LGS_ASSOC_TIMEOUT;
		}
	}
	if ( LGS_MOTE_TYPE_TRUCK == mtype )
	{
		if ( rssi < LGS_TRUCK_ASSOCIATE_THRESHOLD )
		{
			if (( LGS_TRUCK_NONE != my_assoc_l2_ )&& (mid != my_assoc_l2_))
			{
				lgs_send_assoc_req( my_assoc_l2_, LGS_MOTE_TYPE_TRUCK, LGS_REQ_DEASSOC, rssi );
			}
//			if ((mid == my_assoc_l2_) && (
			lgs_send_assoc_req( mid, mtype, LGS_REQ_ASSOC, rssi );
			lgs_assoc_request_timeout_ = rtc_get_ticks() + LGS_TRUCKMOTE_ASSOC_TIMEOUT;
		}
		else
		{
			if ( rssi > LGS_TRUCK_ASSOCIATE_THRESHOLD + LGS_TRUCK_ASSOCIATE_HIST )
			{
				// close to the threshold !!
				if ( mid == my_assoc_l2_ )
				{
					lgs_send_assoc_req( mid, mtype, LGS_REQ_DEASSOC, rssi );
					lgs_report_event( LGS_PALETTEMOTE_LEAVE_TRUCK, my_assoc_l2_, LGS_MOTE_TYPE_TRUCK, rssi, LGS_RELIABLE_TX );

					my_assoc_l2_ = LGS_TRUCK_NONE;
				}
			}
		}
	}
	else if ( LGS_MOTE_TYPE_DP == mtype )
	{
		if (( LGS_MOTE_TYPE_PALETTE == LGS_NODE_TYPE ) && ( LGS_TRUCK_NONE != my_assoc_l2_ ) )
		{
			return; // if palette is asociated with truck than no need to assoc with gate/DP
		}
		if ( rssi < LGS_GATE_RSSI_THRESHOLD )
		{
			if (( LGS_DP_NONE != my_assoc_l1_ ) && (mid != my_assoc_l1_))
			{
				lgs_send_assoc_req( my_assoc_l1_, LGS_MOTE_TYPE_DP, LGS_REQ_DEASSOC, rssi );
			}
			lgs_send_assoc_req( mid, mtype, LGS_REQ_ASSOC, rssi );
			// close to the threshold !!
			my_assoc_l1_ = mid;
			lgs_assoc_request_timeout_ = rtc_get_ticks() + LGS_TRUCKMOTE_ASSOC_TIMEOUT;
		}
		else
		{
			if ( rssi > LGS_GATE_RSSI_THRESHOLD + LGS_GATE_RSSI_HIST )
			{
				// close to the threshold !!
				if ((LGS_ASSOC_STATUS_IDLE != lgs_assoc_status_)
					&&( mid == lgs_pending_node_id_ ))
				{
					lgs_send_assoc_req( mid, mtype, LGS_REQ_DEASSOC, rssi );
					
					if (LGS_MOTE_TYPE_PALETTE == LGS_NODE_TYPE )
						lgs_report_event( LGS_PALETTEMOTE_LEAVE_DP, my_assoc_l1_, LGS_MOTE_TYPE_DP, rssi, LGS_RELIABLE_TX );
					else
						lgs_report_event( LGS_TRUCKMOTE_LEAVE_DP, my_assoc_l1_, LGS_MOTE_TYPE_DP, rssi, LGS_RELIABLE_TX );

					my_assoc_l1_ = LGS_DP_NONE;
				}
			}
		}

	}

	{
		//lgs_report_event(LGS_TRUCKMOTE_JOIN_DP, my_assoc_l1_, LGS_MOTE_TYPE_DP, rssi);
	} // send join message
}

//#endif // _ENABLE_RFID_READER_DETECTION_



/**
  * lgs_recv_assoc_req() - when received assoc req. related massgae
  */
void lgs_recv_assoc_req( pkt_mod_header_t xdata *module, uint8_t rssi )
{
	lgs_mote_assoc_req_t *ph = ( lgs_mote_assoc_req_t * )module ;

	switch ( ph->msg_type )
	{
		case LGS_REQ_ASSOC:
			// Received from palette
			lgs_add_assoc( ph->node_id, ph->node_type );
			lgs_send_assoc_req( ph->node_id, ph->node_type, LGS_REQ_ASSOC_ACK, rssi );
			// ADD to my list of associated nodes??
			break;
/// ###########################################
		case LGS_REQ_DEASSOC:
			lgs_send_assoc_req( ph->node_id, ph->node_type, LGS_REQ_DEASSOC_ACK, rssi );
			// Received from palette
			break;
/// ###########################################
		case LGS_REQ_ASSOC_ACK:
			// Received from DP/Gate
			if ( LGS_MOTE_TYPE_DP == LGS_NODE_TYPE ) break;
			if ( LGS_MOTE_TYPE_TRUCK == LGS_NODE_TYPE )
			{
				my_assoc_l1_ = ph->node_id;
				lgs_report_event( LGS_TRUCKMOTE_JOIN_DP, ph->node_id, ph->node_type, rssi, LGS_RELIABLE_TX );
				lgs_assoc_status_= LGS_ASSOC_STATUS_ASSOC_L1;
			}
			if ( LGS_MOTE_TYPE_PALETTE == LGS_NODE_TYPE )
			{
				if ( LGS_MOTE_TYPE_TRUCK == ph->node_type )
				{
					my_assoc_l1_ = ph->assoc_l1;
					my_assoc_l2_ = ph->node_id;
					lgs_report_event( LGS_PALETTEMOTE_JOIN_TRUCK, ph->node_id, ph->node_type, rssi, LGS_RELIABLE_TX );
					lgs_assoc_status_= LGS_ASSOC_STATUS_ASSOC_L2;
				}
				else
				{
					my_assoc_l1_ = ph->node_id;
					my_assoc_l2_ = LGS_ASSOC_NONE;
					lgs_report_event( LGS_PALETTEMOTE_JOIN_DP, ph->node_id, ph->node_type, rssi, LGS_RELIABLE_TX );
					lgs_assoc_status_= LGS_ASSOC_STATUS_ASSOC_L1;
				}
			}
			lgs_assoc_request_timeout_ = RTC_OVERFLOW_TIMER_VALUE;
			lgs_assoc_timeout_ = rtc_get_ticks() + LGS_ASSOC_TIMEOUT;

			break;
/// ###########################################
		case LGS_REQ_ASSOC_NACK:
			// received from DP/Gate
			break;
/// ###########################################
		case LGS_REQ_DEASSOC_ACK:
			// received from DP/Gate
#ifdef _ENABLE_DLS_TRUCK_MOTE_
//			lgs_truck_state_ = LGS_TRUCK_IDLE;
//			lgs_truck_assoc_ = LGS_DP_TRANSIT;
#endif // _ENABLE_DLS_TRUCK_MOTE_
			my_assoc_l1_ = LGS_DP_TRANSIT;

			break;
/// ###########################################
	} // SWITCH

//	if (LGS_TRUCK_JOINING_DP == lgs_truck_state_) // when to send info about joining the DP
	{
//		lgs_report_event( LGS_TRUCKMOTE_JOIN_DP, ph->node_id, ph->node_type, rssi);
		/*		LGS_TRUCKMOTE_JOIN_DP		0x01
				LGS_TRUCKMOTE_LEAVE_DP		0x02
				LGS_PALETTEMOTE_JOIN_DP		0x03
				LGS_PALETTEMOTE_LEAVE_DP	0x04
				LGS_PALETTEMOTE_JOIN_TRUCK	0x05
				LGS_PALETTEMOTE_LEAVE_TRUCK	0x06
		*/
	} // send join message
}



/**
  * lgs_send_gatemote_beacon() - sends Beacon message from GateMote
  */
void lgs_send_gatemote_beacon()
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
	uint8_t size = tsp_reserve_packet( PKT_MODULE_LEN_LGS_GATEMOTE_BEACON, &pkt_id, MAC_BROADCAST );
	uint8_t original_size = 0;

	if ( 0 == size ) {/* Error */	sys_error( SYS_ERROR_MEM_FULL );	return;	}
	else
	{
		// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy( pkt_id );
		// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
		//    .... (Possibly defered, but not for too long)
		lgs_gatemote_beacon_t *ph = ( lgs_gatemote_beacon_t * )mod ;
		//original_size = ph->module_length;
		ph->module_type = PKT_MODULE_TYPE_LGS_GATEMOTE_BEACON;
		ph->module_length = PKT_MODULE_LEN_LGS_GATEMOTE_BEACON;
		ph->report_type = LGS_GATEMOTE_BEACON_V1;
		ph->gate_id = LGS_NODE_ID;
		//ph->palette_id = MY_ADDR;
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	tsp_send_from_modules( pkt_id );

}


//#ifdef _ENABLE_DLS_TRUCK_MOTE_
/**
  * lgs_send_mote_beacon() - sends Beacon message from GateMote
  */
void lgs_send_mote_beacon()
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
	uint8_t size = tsp_reserve_packet( PKT_MODULE_LEN_LGS_MOTE_BEACON, &pkt_id, MAC_BROADCAST );
	uint8_t original_size = 0;

	if ( 0 == size ) {/* Error */	sys_error( SYS_ERROR_MEM_FULL );	return;	}
	else
	{
		// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy( pkt_id );
		// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
		//    .... (Possibly defered, but not for too long)
		lgs_mote_beacon_t *ph = ( lgs_mote_beacon_t * )mod ;
		//original_size = ph->module_length;
		ph->module_type = PKT_MODULE_TYPE_LGS_MOTE_BEACON;
		ph->module_length = PKT_MODULE_LEN_LGS_MOTE_BEACON;
		if ( LGS_MOTE_TYPE_TRUCK == LGS_NODE_TYPE )
			{ ph->report_type = LGS_TRUCKMOTE_BEACON_V1;}
		else { ph->report_type = LGS_GATEMOTE_BEACON_V1; }
		ph->node_id = LGS_NODE_ID;
		ph->node_type = LGS_NODE_TYPE;
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	tsp_send_from_modules( pkt_id );

}
//#endif // _ENABLE_DLS_TRUCK_MOTE_




/**
  * lgs_send_assoc_req(up_node_id, up_node_type, req_type, rssi) - send Association Reques
  *		either to ASSOCIATE or DE-ASSOCIATE or ACK or NotACK
  */
void lgs_send_assoc_req( uint8_t up_node_id, uint8_t up_node_type, uint8_t req_type, uint8_t rssi )
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
	uint8_t size = tsp_reserve_packet( PKT_MODULE_LEN_LGS_MOTE_ASSOC_REQ, &pkt_id, LGS_TO_NET_ID( up_node_id, up_node_type ) );
	uint8_t original_size = 0;

	if ( 0 == size ) {/* Error */	sys_error( SYS_ERROR_MEM_FULL );	return;	}
	else
	{
		// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy( pkt_id );
		// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
		//    .... (Possibly defered, but not for too long)
		lgs_mote_assoc_req_t *ph = ( lgs_mote_assoc_req_t * )mod ;
		//original_size = ph->module_length;
		ph->module_type = PKT_MODULE_TYPE_LGS_MOTE_ASSOC_REQ;
		ph->module_length = PKT_MODULE_LEN_LGS_MOTE_ASSOC_REQ;

		ph->msg_type = req_type;	// de-/assoc request, acknowledgement
		ph->node_id = LGS_NODE_ID;
		ph->node_type = LGS_NODE_TYPE;
		ph->assoc_l1 = my_assoc_l1_; // associated DP/Gate
		ph->assoc_l2 = my_assoc_l2_; // Associated Truck
		ph->assoc_l3 = my_assoc_l3_;

		if (req_type == LGS_REQ_ASSOC)
		{
			lgs_assoc_status_= LGS_ASSOC_STATUS_PENDING;
			lgs_pending_node_id_ = up_node_id;
			lgs_pending_node_type_ = up_node_type;
			lgs_pending_node_rssi_ = rssi;
		}
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	tsp_send_from_modules( pkt_id );
}





#ifdef _ENABLE_RFID_READER_DETECTION_

void logistics_rfid_read( pkt_mod_header_t xdata *a_mod, uint8_t rssi )
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
	uint8_t size = 0;
	uint8_t original_size = 0;
	uint8_t *src_p;
	uint8_t i;

	// Delete all tags when restarting a new update (set flag after last received)
	// Clear when a new one received (should allow for correction if missed something in one round)

	// TODO: Decide if we should associate the tags with this palette
	// TODO: Check if already associated (if in load mode than
	if ( LGS_RFIDREADER_ASSOCIATE_THRESHOLD > rssi )
	{
//		rfid_report_packet_v1_t *mod = (rfid_report_packet_v1_t *)&(pp->data_modules);
		rfid_report_packet_v1_t *mod = ( rfid_report_packet_v1_t * )a_mod;
		uint8_t * id = &( mod->tag_part1 );
		lgs_tag_index_t tag_index = lgs_get_tag_index( mod->tag_type, id );
		if ( tag_index != ( lgs_tag_index_t )LGS_TAG_INDEX_NONE )
		{
			// tag is already in the database
			// periodically send the full inventory (but rather rarelly)
			lgs_palette_periodic_report_couter++;
			if ( LGS_PERIODIC_REPORT_COUNTER_MAX < lgs_palette_periodic_report_couter)
			{
				lgs_send_full_inventory( BS_ADDR );
			}
		}
		else if ( lgs_tag_count_ + 1 < LGS_MAX_TAGS )
		{
			// add new tag
			lgs_tag_type_[lgs_tag_count_] = mod->tag_type;
			lgs_assoc_l1[lgs_tag_count_] = my_assoc_l1_; // DP/Gate info
			lgs_assoc_l2[lgs_tag_count_] = my_assoc_l2_; // Trcuk info
			lgs_assoc_l3[lgs_tag_count_] = LGS_NODE_ID; // Palette info
			lgs_assoc_l4[lgs_tag_count_] = LGS_ORDER_NONE; // ORDER ??? from where
//			for (i=0; i < mod->modelu_length; i++)
			for ( i = 0; i < LGS_TAG_ID_SIZE; i++ )
			{
				rfids_[i+LGS_RFID_OFFSET( lgs_tag_count_ )] = id[i];
			}
			lgs_tag_count_++; //
			// When added a new tag -> send new inventory
			lgs_send_full_inventory( BS_ADDR );
		}
		else
		{
			sys_error( SYS_ERROR_LGS_TAG_MEM_FULL );
		}
	}
	// Send the report to BS about the detection AND (???) associacion if performed
	src_p = (( uint8_t* )a_mod );
	src_p--;
	lgs_report_event( LGS_PALETTEMOTE_NEW_TAG, src_p[0], LGS_MOTE_TYPE_RFID, rssi, LGS_RELIABLE_TX );

}

#endif // _ENABLE_RFID_READER_DETECTION_







/**
  * lgs_clear_tag_memory() - cleares the inventory list of RFID tags
  */
void lgs_clear_tag_memory()
{
	uint16_t i;
	for ( i = 0; i < LGS_MAX_TAGS; i++ )
	{
		lgs_tag_type_[i] = LGS_TAG_TYPE_EMPTY;
		lgs_assoc_l1[i] = LGS_ASSOC_NONE;
		lgs_assoc_l2[i] = LGS_ASSOC_NONE;
		lgs_assoc_l3[i] = LGS_ASSOC_NONE;
		lgs_assoc_l4[i] = LGS_ASSOC_NONE;
	}
	for ( i = 0; i < ( LGS_TAG_ID_SIZE * LGS_MAX_TAGS ); i++ )
	{
		rfids_[i] = 0xBB;
	}
	lgs_tag_count_ = 0;
}


/**
  * lgs_send_full_inventory(dst_id) - sends a full inventory list to a given node
  *		if needed the inventory is sent using multiple packets
  */
void lgs_send_full_inventory( uint8_t dst_id )
{
	lgs_tag_index_t i = 0;
	do
	{
		i = lgs_send_inventory( dst_id, i );
	}
	while ( i < lgs_tag_count_ );
}



/**
  * lgs_send_inventory(dst_id, start_index) - sends a inventory list to a given node
  *		starts from "start_index" of own inventory
  *		RETURNS: LGS_TAG_INDEX_NONE if start index is incorrect, OR
  *				 (count of all record if finished), OR
  *				 index of next record to be sent
  */
lgs_tag_index_t lgs_send_inventory( uint8_t dst_id, lgs_tag_index_t start_index )
{
	if ( start_index < lgs_tag_count_ )
	{
		uint8_t pkt_id = 0xFF;
		uint8_t needed_size = 0;
		uint8_t pkt_size = 0;
		lgs_tag_index_t temp = lgs_tag_count_ - start_index;
		if ( temp > LGS_MAX_TAGS_PER_INVENTORY_PKT )
		{
			temp = LGS_MAX_TAGS_PER_INVENTORY_PKT;
		}
		needed_size = LGS_INVENTORY_REPORT_OVERHEAD + temp * LGS_TAG_REPORT_SIZE;
		pkt_size = tsp_reserve_packet( needed_size, &pkt_id, dst_id );

		if ( 0 == pkt_size )
		{
			/* Error */
			sys_error( SYS_ERROR_MEM_FULL );
			return LGS_TAG_INDEX_NONE;
		}
		else
		{
			uint8_t i, j;
			uint8_t *inv_p = NULL;
			lgs_tag_report_t *inv_tag = NULL;
			uint8_t *mod = tsp_get_dummy( pkt_id );
			lgs_inventory_report_t *ph = ( lgs_inventory_report_t* )mod ;
			ph->module_type = PKT_MODULE_TYPE_LGS_INVENTORY_REPORT;
			ph->module_length = needed_size;
			ph->node_id = LGS_NODE_ID;
			ph->node_type = LGS_NODE_TYPE;
			ph->inventory_count = temp;
			ph->inventory_type = LGS_INVENTORY_REPORT_V1;

			{
				uint8_t *inv_p = ( uint8_t* ) & ( ph->invenotry );
				uint8_t *id_p = NULL;
				inv_tag = ( lgs_tag_report_t* ) & ( ph->invenotry );
				for ( i = start_index; i < ( start_index + temp ); i++ )
				{
//				inv_tag ->assoc_l1 = warehouse;
//				inv_tag ->assoc_l2 = truck;
//				inv_tag ->assoc_l3 = palette;
//				inv_tag ->assoc_l4 = order;
					inv_tag ->tag_type = lgs_tag_type_[i];
					id_p = ( uint8_t* ) & ( inv_tag ->tag_part1 );
					for ( j = 0; j < LGS_TAG_ID_SIZE; j++ )
					{
						id_p[j] = rfids_[j+LGS_RFID_OFFSET( i )];
					}
					// Move to next tag
					inv_p += LGS_TAG_REPORT_SIZE;
					inv_tag = ( lgs_tag_report_t* )inv_p;
				}
			}
		}
		// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
		tsp_send_from_modules( pkt_id );
		return ( start_index + temp );
	}
	else
	{
		// start index too high
		sys_error( SYS_ERROR_LGS_WRONG_TAG_IDX );
	}
	return lgs_tag_count_;
}


/*
lgs_tag_index_t lgs_send_inventory(uint8_t dst_id, lgs_tag_index_t start_index)
{
	if (start_index < lgs_tag_count_)
	{
		uint8_t pkt_id = 0xFF;
		uint8_t needed_size = 0;
		uint8_t pkt_size =0;
		lgs_tag_index_t temp = lgs_tag_count_ - start_index;
		if (temp > LGS_MAX_TAGS_PER_INVENTORY_PKT)
		{
			temp = LGS_MAX_TAGS_PER_INVENTORY_PKT;
		}
		needed_size = LGS_INVENTORY_REPORT_OVERHEAD + temp * LGS_TAG_REPORT_SIZE;
		pkt_size = tsp_reserve_packet ( needed_size, &pkt_id, dst_id );

		if (0 == pkt_size)
		{ // Error
			sys_error(SYS_ERROR_MEM_FULL);
			return LGS_TAG_INDEX_NONE;
		}
		else
		{
			uint8_t i,j;
			uint8_t *inv_p = NULL;
			lgs_tag_report_t *inv_tag = NULL;
			uint8_t *mod = tsp_get_dummy(pkt_id);
			lgs_inventory_report_t *ph = ( lgs_inventory_report_t* )mod ;
			ph->module_type = PKT_MODULE_TYPE_LGS_INVENTORY_REPORT;
			ph->module_length = needed_size;
			ph->node_id = LGS_NODE_ID;
			ph->node_type = LGS_NODE_TYPE;
			ph->inventory_count = temp;
			ph->inventory_type = LGS_INVENTORY_REPORT_V1;

			{
			uint8_t *inv_p = (uint8_t*)&(ph->invenotry);
			uint8_t *id_p = NULL;
			inv_tag = (lgs_tag_report_t*)&(ph->invenotry);
			for(i=start_index; i < (start_index+temp); i++)
			{
//				inv_tag ->assoc_l1 = warehouse;
//				inv_tag ->assoc_l2 = truck;
//				inv_tag ->assoc_l3 = palette;
//				inv_tag ->assoc_l4 = order;
				inv_tag ->tag_type = tag_type[i];
				id_p = (uint8_t*)&(inv_tag ->tag_part1);
				for(j=0; j< LGS_TAG_ID_SIZE; j++)
				{
					id_p[j] = rfids_[j+LGS_RFID_OFFSET(i)];
				}
				// Move to next tag
				inv_p += LGS_TAG_REPORT_SIZE;
				inv_tag = (lgs_tag_report_t*)inv_p;
			}
			}
		}
		// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
		tsp_send_from_modules(pkt_id);
		return (start_index + temp);
	}
	else
	{
		// start index too high
		sys_error(SYS_ERROR_LGS_WRONG_TAG_IDX);
	}
	return lgs_tag_count_;
}
*/

/**
*	lgs_get_tag_index(tag_type, *id) - searches for a partiular tag in the inventory
*		RETURN: index to the found tag, OR
*				LGS_TAG_INDEX_NONE is not found any
*/
lgs_tag_index_t lgs_get_tag_index( lgs_tag_type_t tag_type, uint8_t *id )
{
	lgs_tag_index_t i = 0;
	uint8_t	j = 0; //
	uint16_t offset = 0;
	for ( i = 0; i < lgs_tag_count_; i++ )
	{
		if ( tag_type == lgs_tag_type_[i] )
		{
			offset = LGS_RFID_OFFSET( i );
			for ( j = 0; j < LGS_TAG_ID_SIZE; j++ )
			{
				if ( id[j] != rfids_[offset+j] )
				{
					break;
				}
			}
			if ( LGS_TAG_ID_SIZE == j )
			{
				//found
				return i;
			}
		}
	}
	return LGS_TAG_INDEX_NONE;
}


/**
* lgs_report_event(event_type, r_node_id, r_node_type, rssi) - sends an event report
* 	accepts event type, and info about relevant node
*/
void lgs_report_event( uint8_t event_type, uint8_t r_node_id, uint8_t r_node_type, uint8_t rssi, uint8_t reliability )
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	uint8_t pkt_id = 0xFF;
	uint8_t size = tsp_reserve_packet( PKT_MODULE_LEN_LGS_MOTE_REPORT, &pkt_id, BS_ADDR );

	if ( 0 == size ) {/* Error */	sys_error( SYS_ERROR_MEM_FULL );	return;	}
	else
	{
		// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy( pkt_id );
		// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
		//    .... (Possibly defered, but not for too long)
		lgs_rfidr_mote_report_mod_t *ph = ( lgs_rfidr_mote_report_mod_t  * )mod ;
		//original_size = ph->module_length;
		ph->module_type = PKT_MODULE_TYPE_LGS_MOTE_REPORT;
		ph->module_length = PKT_MODULE_LEN_LGS_MOTE_REPORT;
		ph->report_type = event_type;
		ph->node_id = LGS_NODE_ID;		// reporting node id
		ph->node_type = LGS_NODE_TYPE;		// reporting node type
		ph->assoc_l1 = my_assoc_l1_;		// reporting node associacions at each level
		ph->assoc_l2 = my_assoc_l2_;
		ph->assoc_l3 = my_assoc_l3_;
		ph->assoc_l4 = my_assoc_l4_;
		ph->rssi = rssi;			// rssi corresponding to the event
		ph->related_id = r_node_id;		// the node id that corresponds to the reported event
		ph->related_type = r_node_type ;	// the node type that corresponds to the reported event
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	tsp_send_from_modules( pkt_id );

	// Enable repetition of event report until ACK is received
	if ( LGS_RELIABLE_TX == reliability )
	{
		lgs_last_timeout_ = rtc_get_ticks() + LGS_LAST_TIMEOUT;
		lgs_last_event_type = event_type;
		lgs_last_r_node_id = event_type;
		lgs_last_r_node_type = event_type;
		lgs_last_rssi = event_type;
		lgs_last_count_ = 1; // Enable retransmissions
		lgs_cancel_pending_report();
	}
	else if ( LGS_RELIABLE_TX_CONTINUE == reliability) 
	{
		lgs_last_timeout_ = rtc_get_ticks() + LGS_LAST_TIMEOUT;
		lgs_last_count_++;
		if ( LGS_LAST_COUNT_MAX < lgs_last_count_)	{ lgs_last_count_ = 0; }
	}
	else
	{
		lgs_last_count_ = 0; // may cut short previous reliable transmissions (but will shut it for now)
	}
}


/**
* lgs_cancel_pending_report() - cancel pending report (that is do not retransmit it!!)
*/
void lgs_cancel_pending_report()
{
	lgs_last_timeout_ = rtc_get_ticks() + LGS_LAST_TIMEOUT;
/*	lgs_last_event_type = 0;
	lgs_last_r_node_id = 0;
	lgs_last_r_node_type = 0;
	lgs_last_rssi = 0;
*/	lgs_last_count_ = 0; // End retransmissions

}


/**
* lgs_add_assoc(a_node_id, a_node_type) - adds the association to a particular node
*	- handles both UP- and DOWN-association
*		(including cascading affect DOWN -> get inventory from palette)
*/
void lgs_add_assoc( uint8_t a_node_id, uint8_t a_node_type )
{
	switch ( a_node_type )
	{
		case LGS_MOTE_TYPE_DP:
			if ( a_node_id == my_assoc_l1_ ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_DP ) break;
			if (( LGS_NODE_TYPE == LGS_MOTE_TYPE_PALETTE )
					&& ( my_assoc_l2_ != LGS_TRUCK_NONE ) ) break;
			// Check if already associated
			if ( LGS_DP_NONE != my_assoc_l1_ )
			{
				lgs_send_assoc_req( my_assoc_l1_, LGS_MOTE_TYPE_DP, LGS_REQ_DEASSOC, MAX_RSSI );
			}
			my_assoc_l1_ = a_node_id;
			break;
		case LGS_MOTE_TYPE_TRUCK:
			if ( a_node_id == my_assoc_l2_ ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_TRUCK ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_DP )
			{
				// do special association (add to list??)
				// request detailed Inventory ??
				break;
			}
			// Check if already associated
			if ( LGS_TRUCK_NONE != my_assoc_l2_ )
			{
				lgs_send_assoc_req( my_assoc_l2_, LGS_MOTE_TYPE_TRUCK, LGS_REQ_DEASSOC, MAX_RSSI );
			}
			my_assoc_l2_ = a_node_id;
			break;
		case LGS_MOTE_TYPE_PALETTE:
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_DP ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_TRUCK ) break;
			if ( a_node_id == my_assoc_l3_ ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_DP )
			{
				// do special association (add to list??)
				// request detailed Inventory ??
				break;
			}
			// Check if already associated
			if ( LGS_PALETTE_NONE != my_assoc_l3_ )
			{
				lgs_send_assoc_req( my_assoc_l3_, LGS_MOTE_TYPE_TRUCK, LGS_REQ_DEASSOC, MAX_RSSI );
			}
			my_assoc_l3_ = a_node_id;
			break;
		case LGS_MOTE_TYPE_RFID:
			break;
	}
}


/**
* lgs_del_assoc(a_node_id, a_node_type) - removes the association to a particular node
*	- handles both UP- and DOWN-association (including cascading affect UP and DOWN)
*/
void lgs_del_assoc( uint8_t a_node_id, uint8_t a_node_type )
{
	switch ( a_node_type )
	{
		case LGS_MOTE_TYPE_DP:
			if ( a_node_id == my_assoc_l1_ ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_DP ) break;
			if (( LGS_NODE_TYPE == LGS_MOTE_TYPE_PALETTE )
					&& ( my_assoc_l2_ != LGS_TRUCK_NONE ) ) break;
			// Check if already associated
			if ( LGS_DP_NONE != my_assoc_l1_ )
			{
				lgs_send_assoc_req( my_assoc_l1_, LGS_MOTE_TYPE_DP, LGS_REQ_DEASSOC, MAX_RSSI );
			}
			my_assoc_l1_ = a_node_id;
			break;
		case LGS_MOTE_TYPE_TRUCK:
			if ( a_node_id == my_assoc_l2_ ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_TRUCK ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_DP )
			{
				// do special association (add to list??)
				// request detailed Inventory ??
				break;
			}
			// Check if already associated
			if ( LGS_TRUCK_NONE != my_assoc_l2_ )
			{
				lgs_send_assoc_req( my_assoc_l2_, LGS_MOTE_TYPE_TRUCK, LGS_REQ_DEASSOC, MAX_RSSI );
			}
			my_assoc_l2_ = a_node_id;
			break;
		case LGS_MOTE_TYPE_PALETTE:
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_DP ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_TRUCK ) break;
			if ( a_node_id == my_assoc_l3_ ) break;
			if ( LGS_NODE_TYPE == LGS_MOTE_TYPE_DP )
			{
				// do special association (add to list??)
				// request detailed Inventory ??
				break;
			}
			// Check if already associated
			if ( LGS_PALETTE_NONE != my_assoc_l3_ )
			{
				lgs_send_assoc_req( my_assoc_l3_, LGS_MOTE_TYPE_TRUCK, LGS_REQ_DEASSOC, MAX_RSSI );
			}
			my_assoc_l3_ = a_node_id;
			break;
		case LGS_MOTE_TYPE_RFID:
			break;
	}
}

#endif // _ENABLE_LOGISTICS_01_
