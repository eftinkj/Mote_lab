#ifndef _LGS_BASIC_PUB_H_
#define _LGS_BASIC_PUB_H_


/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"

#include "packet.h"


#ifdef _ENABLE_RFID_READER_DETECTION_

	struct lgs_rfidr_detect
	{
		uint8_t module_type;
		uint8_t module_length;

		uint8_t report_type;

		uint8_t	rfidr_id;
		uint8_t	rssi;
		uint8_t palette_id;

	} PACKED;

	typedef struct lgs_rfidr_detect lgs_rfidr_detect_packet_v2_t ;


void logistics_rfid_read(pkt_mod_header_t xdata *a_mod, uint8_t rssi );


#endif // _ENABLE_NEM_UTILITIES_01_




typedef	struct
		{
			uint8_t module_type;
			uint8_t module_length;

			uint8_t report_type;

			uint8_t	gate_id;
			uint8_t	reserved_1;
			uint8_t reserved_2;
			uint8_t reserved_3;
			uint8_t reserved_4;

		} PACKED lgs_gatemote_beacon_t;

typedef	struct
		{
			uint8_t module_type;
			uint8_t module_length;

			uint8_t report_type;

			uint8_t	node_id;
			uint8_t	node_type;
			uint8_t reserved_1;
			uint8_t reserved_2;
			uint8_t reserved_3;

		} PACKED lgs_mote_beacon_t;
#define PKT_MODULE_LEN_LGS_MOTE_BEACON	(sizeof(lgs_mote_beacon_t) - 2)
#define LGS_GATEMOTE_BEACON_V1	0x01
#define LGS_TRUCKMOTE_BEACON_V1	0x02

typedef	struct
		{
			uint8_t module_type;
			uint8_t module_length;

			uint8_t report_type;

			uint8_t node_id;		// reporting node id
			uint8_t	node_type;		// reporting node type
			uint8_t	assoc_l1;		// reporting node associacions at each level
			uint8_t	assoc_l2;
			uint8_t	assoc_l3;
			uint8_t	assoc_l4;
			uint8_t rssi;			// rssi corresponding to the event
			uint8_t	related_id;		// the node id that corresponds to the reported event
			uint8_t related_type;	// the node type that corresponds to the reported event

		} PACKED lgs_rfidr_mote_report_mod_t ;

#define PKT_MODULE_LEN_LGS_MOTE_REPORT	(sizeof(lgs_rfidr_mote_report_mod_t) - 2)

#define LGS_TRUCKMOTE_JOIN_DP		0x01
#define LGS_TRUCKMOTE_LEAVE_DP		0x02
#define LGS_PALETTEMOTE_JOIN_DP		0x03
#define LGS_PALETTEMOTE_LEAVE_DP	0x04
#define LGS_PALETTEMOTE_JOIN_TRUCK	0x05
#define LGS_PALETTEMOTE_LEAVE_TRUCK	0x06

#define LGS_PALETTEMOTE_NEW_TAG		0x07
#define LGS_PALETTEMOTE_INV_CHANGE	0x08

// When request has not been answared
#define LGS_ASSOC_ATTEMPT_FAILED	0x10
// When not heard for long time from an associated node
#define LGS_ASSOC_TIMEDOUT			0x11


#define LGS_ACK_REPORT			0xF1

typedef struct
{
	uint8_t	assoc_l1;
	uint8_t	assoc_l2;
	uint8_t	assoc_l3;
	uint8_t	assoc_l4;
	uint16_t	tag_type;
	uint32_t	tag_part1;
	uint32_t	tag_part2;
	uint32_t	tag_part3;
} PACKED lgs_tag_report_t;

#define LGS_TAG_REPORT_SIZE	(sizeof(lgs_tag_report_t))




typedef struct
{
	uint8_t	module_type;
	uint8_t module_length;
	uint8_t	node_id;
	uint8_t	node_type;
	uint8_t	inventory_count;
	uint8_t inventory_type;
	uint8_t invenotry; // begining of the list of asociated tags
} PACKED lgs_inventory_report_t;

#define LGS_INVENTORY_REPORT_OVERHEAD	(sizeof(lgs_inventory_report_t)-3)


typedef struct
{
	uint8_t	module_type;
	uint8_t module_length;
	uint8_t	msg_type;	// report, partial/full inventory
	uint8_t	node_id;	// Requesting Mote ID
	uint8_t	node_type;	// Requesting Mote TYPE
	uint8_t	filter_assoc_level; // lowest association level to be reported
	uint8_t filter_node_type;	// filter only specific node types
} PACKED lgs_query_t;

#define LGS_QUERY_MSG_REPORT	0x01
#define LGS_QUERY_MSG_FULL_INV	0x02

#define PKT_MODULE_LEN_LGS_MOTE_QUERY	(sizeof(lgs_query_t)-2)

typedef struct
{
	uint8_t	module_type;
	uint8_t module_length;
	uint8_t	msg_type;	// de-/assoc request, acknowledgement
	uint8_t	node_id;
	uint8_t	node_type;
	uint8_t	assoc_l1; // associated DP/Gate
	uint8_t assoc_l2; // Associated Truck
	uint8_t assoc_l3;
} PACKED lgs_mote_assoc_req_t;
#define LGS_REQ_ASSOC		0x10
#define LGS_REQ_DEASSOC		0x11
#define LGS_REQ_ASSOC_ACK	0x12
#define LGS_REQ_ASSOC_NACK	0x13
#define LGS_REQ_DEASSOC_ACK	0x14

#define PKT_MODULE_LEN_LGS_MOTE_ASSOC_REQ	(sizeof(lgs_mote_assoc_req_t)-2)
/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

#define LGS_ASSOC_LIST_SIZE	4
#define LGS_TAG_TYPE_SIZE 	sizeof(lgs_tag_type_t)
#define LGS_INVENTORY_TAG_REPOR_SIZE	(LGS_TAG_ID_SIZE + LGS_TAG_TYPE_SIZE + LGS_ASSOC_LIST_SIZE)

#define LGS_RFID_OFFSET(i)	(i*LGS_TAG_ID_SIZE)
#define LGS_TAG_INDEX_NONE	0xFFFF



#define LGS_TAG_TYPE_EMPTY	0x00
//#define LGS_TAG_TYPE_EPC	0x8300
//#define LGS_TAG_TYPE_ISO	0x8100


#define LGS_ASSOC_NONE	0x00

#define LGS_DP_NONE			0x00
#define LGS_DP_TRANSIT		100
#define LGS_TRUCK_NONE		0x00
#define LGS_PALETTE_NONE	0x00
#define LGS_ORDER_NONE		0x00


// Simple inventory with 3-tier associations (palette(L3) - truck(L2) - warehouse(L1))
#define LGS_INVENTORY_REPORT_V1	0x01


/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/

/**
  * lgs_init () - sets up Logistics Module
  */
void lgs_init ( void );


/**
  * lgs_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void lgs_loop();

/**
  * lgs_recv_command(module, rssi) - receive a misc LGS packet (e.g. REPORT)
  */
void lgs_recv_command( pkt_mod_header_t xdata *module, uint8_t rssi );


#ifdef _ENABLE_DLS_TRUCK_MOTE_
/**
  * lgs_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void logistics_gate_read(pkt_mod_header_t xdata *module, uint8_t rssi);
#endif // _ENABLE_DLS_TRUCK_MOTE_


//#ifdef _ENABLE_RFID_READER_DETECTION_
/**
  * lgs_recv_mote_beacon() - when received beacon from a mote (truck) (do I join/associate??)
  */
void lgs_recv_mote_beacon(pkt_mod_header_t xdata *module, uint8_t rssi);
//#endif // _ENABLE_RFID_READER_DETECTION_


/**
  * lgs_recv_assoc_req() - when received assoc req. related massgae
  */
void lgs_recv_assoc_req(pkt_mod_header_t xdata *module, uint8_t rssi);


#endif // _LGS_BASIC_PUB_H_
