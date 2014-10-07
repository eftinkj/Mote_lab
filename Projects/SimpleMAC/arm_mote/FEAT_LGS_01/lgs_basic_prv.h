#ifndef _LGS_BASIC_PRV_H_
#define _LGS_BASIC_PRV_H_


/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "lgs_basic_pub.h"

#include "HW_LIB\RTC.h"


#ifdef _ENABLE_RFID_READER_DETECTION_

#endif // _ENABLE_NEM_UTILITIES_01_



/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/

#define LGS_GATEMOTE_DEFAULT_TIMEOUT	3000
#define LGS_TRUCKMOTE_DEFAULT_TIMEOUT	4000

// Time to wait for ACK to Assoc Request
#define LGS_TRUCKMOTE_ASSOC_TIMEOUT		500
#define LGS_ASSOC_TIMEOUT				(5*LGS_TRUCKMOTE_DEFAULT_TIMEOUT)

//################### GATE - TRUCK MOTES ##############
#define MAX_RSSI	99
//#define LGS_GATE_RSSI_THRESHOLD	0x38
#define LGS_GATE_RSSI_THRESHOLD	75
#define LGS_GATE_RSSI_HIST		10

#define LGS_RFIDREADER_ASSOCIATE_THRESHOLD	60
#define LGS_RFIDREADER_ASSOCIATE_HIST		10

#define LGS_TRUCK_ASSOCIATE_THRESHOLD	60
#define LGS_TRUCK_ASSOCIATE_HIST		10

// Periodic inventory report - max count of RFID reader messages
#define LGS_PERIODIC_REPORT_COUNTER_MAX	10


// Truck States
#define LGS_TRUCK_IDLE			0x00
#define LGS_TRUCK_JOINING_DP	0x01
#define LGS_TRUCK_ASSOC			0x02


// Location (assoc)
#define LGS_MAIN_WEAREHOUSE	0x00
#define LGS_MAIN_DP1		0x100
#define LGS_MAIN_DP2		0x101


// Maximum size of memory for tags (in number of tags)
#define LGS_MAX_TAGS		5
// Maximum size of ID of the RFID tag (12 bytes - 96bits)
#define LGS_TAG_ID_SIZE		12
// Max tags reports per Invenotry report
#define LGS_MAX_TAGS_PER_INVENTORY_PKT	4


typedef uint8_t		lgs_tag_index_t;
typedef uint16_t	lgs_tag_type_t;

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

#ifdef _ENABLE_DLS_GATE_MOTE_
	extern rtc_tick_t lgs_gatemote_timeout;
#endif // _ENABLE_DLS_GATE_MOTE_


#ifdef _ENABLE_DLS_TRUCK_MOTE_
//	extern uint8_t lgs_truck_state_;
//	extern uint8_t lgs_truck_assoc_;
#endif // _ENABLE_DLS_TRUCK_MOTE_


extern uint8_t lgs_palette_periodic_report_couter;


extern uint8_t	rfids_[LGS_TAG_ID_SIZE * LGS_MAX_TAGS];
extern lgs_tag_type_t	lgs_tag_type_[LGS_MAX_TAGS];
extern lgs_tag_index_t	lgs_tag_count_;

extern uint8_t	lgs_assoc_l1[LGS_MAX_TAGS];
extern uint8_t	lgs_assoc_l2[LGS_MAX_TAGS];
extern uint8_t	lgs_assoc_l3[LGS_MAX_TAGS];
extern uint8_t	lgs_assoc_l4[LGS_MAX_TAGS];

extern uint8_t	my_assoc_l1_;
extern uint8_t	my_assoc_l2_;
extern uint8_t	my_assoc_l3_;
extern uint8_t	my_assoc_l4_;



extern uint8_t lgs_assoc_status_;
extern uint8_t lgs_pending_node_id_;
extern uint8_t lgs_pending_node_type_;
extern uint8_t lgs_pending_node_rssi_;
extern rtc_tick_t lgs_assoc_request_timeout_;
/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

#define LGS_UNRELIABLE_TX	0
#define LGS_RELIABLE_TX		1
#define LGS_RELIABLE_TX_CONTINUE	2

/**
  * lgs_send_gatemote_beacon() - sends Beacon message from GateMote
  */
void lgs_send_gatemote_beacon();


/**
  * lgs_clear_tag_memory() - cleares the inventory list of RFID tags
  */
void lgs_clear_tag_memory();



/**
  * lgs_send_assoc_req(up_node_id, up_node_type, req_type, rssi) - send Association Reques
  *		either to ASSOCIATE or DE-ASSOCIATE or ACK or NotACK
  */
void lgs_send_assoc_req( uint8_t up_node_id, uint8_t up_node_type, uint8_t req_type, uint8_t rssi );


/**
  * lgs_send_full_inventory(dst_id) - sends a full inventory list to a given node
  *		if needed the inventory is sent using multiple packets
  */
void lgs_send_full_inventory(uint8_t dst_id);

/**
  * lgs_send_inventory(dst_id, start_index) - sends a inventory list to a given node
  *		starts from "start_index" of own inventory
  *		RETURNS: LGS_TAG_INDEX_NONE if start index is incorrect, OR
  *				 (count of all record if finished), OR
  *				 index of next record to be sent
  */
lgs_tag_index_t lgs_send_inventory(uint8_t dst_id, lgs_tag_index_t start_index);



/**
* lgs_report_event(event_type, r_node_id, r_node_type, rssi) - sends an event report
* 	accepts event type, and info about relevant node
*/
void lgs_report_event(uint8_t event_type, uint8_t r_node_id, uint8_t r_node_type, uint8_t rssi, uint8_t enable_);


/**
* lgs_cancel_pending_report() - cancel pending report (that is do not retransmit it!!)
*/
void lgs_cancel_pending_report();


//#ifdef _ENABLE_DLS_TRUCK_MOTE_
/**
  * lgs_send_mote_beacon() - sends Beacon message from GateMote
  */
void lgs_send_mote_beacon();
//#endif // _ENABLE_DLS_TRUCK_MOTE_



/**
* lgs_add_assoc(a_node_id, a_node_type) - adds the association to a particular node
*	- handles both UP- and DOWN-association 
*		(including cascading affect DOWN -> get inventory from palette)
*/
void lgs_add_assoc(uint8_t a_node_id, uint8_t a_node_type);


/**
* lgs_del_assoc(a_node_id, a_node_type) - removes the association to a particular node
*	- handles both UP- and DOWN-association (including cascading affect UP and DOWN)
*/
void lgs_del_assoc(uint8_t a_node_id, uint8_t a_node_type);



/**
*	lgs_get_tag_index(tag_type, *id) - searches for a partiular tag in the inventory
*		RETURN: index to the found tag, OR 
*				LGS_TAG_INDEX_NONE is not found any
*/
lgs_tag_index_t lgs_get_tag_index(lgs_tag_type_t tag_type, uint8_t *id);

#endif // _LGS_BASIC_PRV_H_
