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
#ifndef PACKET_H
#define PACKET_H


//#include "hw_specific.h"
#include "defs/esnl_pub.h"

#include "common_constants.h"

#ifdef QT4
	#define xdata
#endif

typedef uint8_t pkt_len_t;


//extern int16_t xdata my_time;
//extern int16_t xdata value;

#define MAX_VAL 100;
#define MAX_TIME 1000;

#define START_FIELD_OFFSET 0
#define FLAG_FIELD_OFFSET 1
#define HI_MAC_DEST_FIELD_OFFSET 2
#define LO_MAC_DEST_FIELD_OFFSET 3
#define HI_MAC_SRC_FIELD_OFFSET 4
#define LO_MAC_SRC_FIELD_OFFSET 5
#define LENGTH_FIELD_OFFSET 6
#define PKT_V2_MODULES_OFFSET 9
// #define MAX_NEIGHBOR_HOP 5

#define PAK_GET_TOTAL_LENGTH(idx) (PKT_HEADER_LENGTH + buffer0[ idx*SIZE_Q_BUFF + LENGTH_FIELD_OFFSET ] )

#define HEADER_LENGTH 7
#define PKT_HEADER_LENGTH 7


// -----------------------------------------------
// Values for packet fields
//#define RSSI_THRESH_DEFAULT 0x41
//#define RSSI_THRESH_DEFAULT 0x50
//#define RSSI_THRESH_DEFAULT 0x55
#define RSSI_THRESH_DEFAULT 0x64


//////////////////////////////////////////////////////////////////////////////
// To handle collecting XX samples and sending in one packet
#ifdef FEAT_ENABLE_SFQ
//	90-SIZE_OF_ADFS_EXTRA_HEADER
#define SAMPLES_PER_PACKET 85
#else // FEAT_ENABLE_SFQ
#define SAMPLES_PER_PACKET 90
#endif // else FEAT_ENABLE_SFQ
///////////////////////////////////////////////////////////////////////////////

// ====================================
IAR_PACKED struct packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field

#ifdef FEAT_ENABLE_SFQ
	uint8_t weight;
	uint32_t ticker;
#endif // FEAT_ENABLE_SFQ

	uint8_t sequ_num;
	uint8_t numhops;
	uint8_t hop_list;

} PACKED;
typedef struct packet_structure pkt_t;

// Extra data to handle SFQ parameters in a packet
#define SIZE_OF_ADFS_EXTRA_HEADER 5
// The 6th is data - unless the TEST MODE is used then it becomes a sequence number
// as the rest od data portion

// ====================================
IAR_PACKED struct packet_structure_v2
{
	uint8_t start;
	uint8_t flag;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field

	uint8_t data_modules;
} PACKED;
typedef IAR_PACKED struct packet_structure_v2 pkt_v2_t;

// Extra data to handle SFQ parameters in a packet
#define SIZE_OF_ADFS_EXTRA_HEADER 5
// The 6th is data - unless the TEST MODE is used then it becomes a sequence number
// as the rest od data portion


IAR_PACKED struct packet_v2_module_header
{
	uint8_t module_type;
	uint8_t module_length;
	uint8_t content;
} PACKED;
typedef struct packet_v2_module_header pkt_mod_header_t;


IAR_PACKED struct packet_v2_module_adfs
{
	uint8_t weight;
	uint32_t ticker;
} PACKED;
typedef IAR_PACKED struct packet_v2_module_adfs pkt_mod_adfs_t;


IAR_PACKED struct packet_v2_module_channel_switching
{
	uint16_t interval; /*! if zero then disable */
	uint8_t next_ch;
	uint8_t alternative_ch;
	uint32_t first_timeout;
} PACKED;
typedef IAR_PACKED struct packet_v2_module_channel_switching pkt_mod_cs_t;


IAR_PACKED struct packet_v2_module_raw_data
{
	uint8_t seq_num;
	uint8_t channel;
	uint8_t numsamples;
	uint8_t samples_list;
} PACKED;
typedef IAR_PACKED struct packet_v2_module_raw_data pkt_mod_raw_data_t;
#define MOD_RAW_DATA_OVERHEAD 3


IAR_PACKED struct packet_v2_module_hoplist
{
	uint8_t sequ_num;
	uint8_t hoplist_type;
	uint8_t numhops;
	uint8_t hop_list;
} PACKED;
typedef IAR_PACKED struct packet_v2_module_hoplist pkt_mod_hoplist_t;


IAR_PACKED struct packet_v2_module_hoplist_cc_node_info
{
	uint8_t node_id;
	uint8_t rssi;
	uint8_t tx_power;
	uint8_t throughput;
	uint8_t predicted_throughput;
	uint8_t calculated_inflow;
	uint8_t droprate;
	uint8_t queue;
} PACKED;
typedef IAR_PACKED struct packet_v2_module_hoplist_cc_node_info pkt_mod_hoplist_cc_t;

// NEW_PONG has 9B header + 7 ADFS module + 1 end byte (EMPTY MODULE)
#define NEW_PONG_LEN 17

// ====================================
IAR_PACKED struct hello_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field
	uint32_t distance;
	uint32_t energy;
	uint8_t crc;
} PACKED;
typedef IAR_PACKED struct hello_packet_structure hpkt_t;

#define HELLO_LENGTH 18

// ====================================
IAR_PACKED struct ack_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field
	uint32_t distance;
	uint32_t energy;
	uint8_t crc;
} PACKED;
typedef IAR_PACKED struct ack_packet_structure apkt_t;

#define ACK_LENGTH 18

// ====================================
IAR_PACKED struct beam_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field

	uint8_t year; //  year counted from from 2000 (e.g. 2006->06)
	uint8_t month; // month number
	uint8_t day; // a day of the month
	uint8_t hour; // hour (24-hour format)
	uint8_t minute; // minute
	uint8_t second; // second
	uint16_t msecond; // milisecond
	uint8_t hops_from_bs;
	uint8_t seq;

	uint8_t crc;
} PACKED;
typedef struct beam_packet_structure bpkt_t;
#define BEAM_PACKET_LENGTH sizeof(struct beam_packet_structure)

// ====================================
IAR_PACKED struct packet_structure_v2_PCC_upstream
{
	uint8_t start;
	uint8_t flag;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field

	uint8_t module_type;
	uint8_t module_len;
	uint8_t flow_limit;
	uint8_t misc;
	uint8_t next_module_type;
} PACKED;
typedef struct packet_structure_v2_PCC_upstream pkt_v2_PCCup_t;

// Extra data to handle SFQ parameters in a packet
#define SIZE_OF_PCC_UPSTREAM_EXTRA_HEADER 5
#define PCC_UPSTREAM_PKT_LEN		7
#define SIZE_OF_PCC_UPSTREAM_PKT	14

// ====================================
IAR_PACKED struct beacon_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field

	uint8_t channel; //  channel used by the network

	uint8_t crc;
} PACKED;
typedef struct beacon_packet_structure bcpkt_t;

// ====================================
IAR_PACKED struct switch_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field

	uint8_t channel; //  channel to be used as a starting point16_t
	uint8_t search_mode; // search algorithm

	uint8_t crc;
} PACKED;
typedef struct switch_packet_structure switchpkt_t;

// ====================================
IAR_PACKED struct join_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field

	uint8_t channel; //  channel to be used as a starting point16_t
	uint8_t search_mode; // search algorithm
	uint32_t duration; // search algorithm

	uint8_t crc;
} PACKED;
typedef struct join_packet_structure jpkt_t;

#define JOIN_PKT_LENGTH	9
#define JOIN_PKT_SIZE	JOIN_PKT_LENGTH+PKT_HEADER_LENGTH

// ====================================
IAR_PACKED struct select_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field
//	uint8_t timestamp[6]; // a real time and data delivered from the BS
	uint8_t crc;
} PACKED;
typedef struct select_packet_structure spkt_t;

#define SELECT_LENGTH 10

// Sequence number used for data packets (initially for TEST MODE only)
extern uint8_t sequence_no_;


#define GET_DST_ID( pkt )	(((pkt_t *)pkt)->dst_id)


/**
  * get_dst_mac_base(base) - returns the MAC address of destination
  * 	for a given packet ('base' offset in the queue buffer)
  * returns the MAC addrss of dest node
  */
uint16_t get_dst_mac_base ( uint16_t base );


//int16_t create_data_packet(int8_t *p, int16_t channel);

//int16_t create_count_packet(int8_t *p, int16_t data_size);


#ifndef QT4
	pkt_len_t create_data_packet_b ( uint16_t base, int16_t channel ) REENTRANT;
	pkt_len_t create_count_packet_b ( uint16_t base, uint8_t data_size, uint8_t channel, uint8_t mode );

	pkt_len_t create_packet ( int8_t *p );
	//uint8_t pak_get_packet_total_length ( int8_t *pkt );

	#define GET_PKT_LEN(p_pkt) (PKT_HEADER_LENGTH + ((pkt_t*)p_pkt)->length)



	// Vreate test packet V2
	pkt_len_t create_count_packet_v2_b ( uint16_t base, int16_t data_size, uint8_t channel, uint8_t mode );

	pkt_len_t create_data_packet_v2_b ( uint16_t base, int16_t data_size, uint8_t channel, uint8_t mode );
#endif // QT4



////////////////////////////////////////////////////////
IAR_PACKED struct config_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id;
	uint8_t src_id;
	//9 bytes
	//Section for Sampling Settings
	uint8_t data_mode;
	uint8_t divider;
	uint8_t averager;
	uint8_t data_size;
	uint8_t num_adc_chan;
	//5 bytes
	//Section for RF Settings
	uint8_t current_RF_chan;
	uint8_t power_control_scheme;
	uint8_t power_level;
	uint8_t RSSI_thresh;
	//4 bytes
	//Settings for OEDSR
	uint32_t energy;
	uint32_t distance;
	uint8_t RF_TX_retries;
	//9 bytes
	//Settings for Protocol Selection
	uint8_t protocol;
	uint8_t scheduling_protocol;
	//2 bytes
	//Section for Scheduling Settings
	uint16_t src_weight;
	uint16_t node_weight;
	//4 bytes
	//Clustering Settings
	uint8_t current_CH;
	//1 bytes
	//For Recieved Config packet RSSI Feedback
	uint8_t rssi;
	// if length > 0x0B
	//1 bytes
	//uint8_t pkt->cost_function;
	uint8_t crc;
	//1 bytes
} PACKED;
typedef struct config_packet_structure conpkt_t;
#define CONFIG_LEN 36
//#define CONFIG_LEN sizeof(conpkt_t)
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
IAR_PACKED struct enable_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint32_t energy;
	uint8_t crc;
} PACKED;
typedef struct enable_packet_structure enpkt_t;
//#define CONFIG_LEN 11
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
IAR_PACKED struct goodbye_packet_structure
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t crc;
} PACKED;
typedef struct goodbye_packet_structure gbpkt_t;
#define GOODBYE_LEN 7
////////////////////////////////////////////////////////



///////////////////////////////////////////////
//Counter Report Packet Structure
#define COUNT_LEN_OEDSR 11
IAR_PACKED struct counter_packet_structure_OEDSR
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t Hellocnt;
	uint8_t Selcnt;
	uint8_t ACKcnt;
	uint8_t DATcnt;
	uint8_t BEAMcnt;
	uint8_t BEACONcnt;
	uint8_t crc; // part of DATA field
} PACKED;
typedef struct counter_packet_structure_OEDSR CNTpkt_OEDSR;
///////////////////////////////////////////////



typedef IAR_PACKED struct
{
	uint8_t module_type;
	uint8_t module_length;
	uint8_t report_ver;
	uint8_t	cycle_count;
	uint16_t	max_value;
} PACKED cycle_report_packet_v1_t;



typedef IAR_PACKED struct
{
	uint8_t module_type;
	uint8_t module_length;
	uint8_t reserved; // ????
	uint16_t tag_type;
	uint32_t	tag_part1;
	uint32_t	tag_part2;
	uint32_t	tag_part3;
} PACKED rfid_report_packet_v1_t;



typedef IAR_PACKED struct
{
	uint8_t module_type;
	uint8_t module_length;
	uint8_t message_type;
	uint8_t	message;
} PACKED fla_cmd_packet_v1_t;


typedef IAR_PACKED struct
{
	uint8_t sequence;
	uint8_t hex_record;
} PACKED fla_ihex_t;


/////////////////////////////////////////
// MMCR structures
IAR_PACKED struct packet_structure_MMCR
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field

	uint8_t pkt_data;
	uint8_t crc;
} PACKED;
typedef struct packet_structure_MMCR pkt_t_mmcr;



IAR_PACKED struct hello_packet_structure_MMCR
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;

	uint8_t length;
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field
	uint32_t energy;
	uint8_t crc;

} PACKED;
typedef struct hello_packet_structure_MMCR hpkt_mmcr_t;

#define HELLO_LENGTH_MMCR 14
//#define RTR_MMCR_HELLO_PKT_SIZE	18


IAR_PACKED struct ack_packet_structure_MMCR
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field
	uint32_t energy;


	uint8_t neighbor_count;
	uint16_t neighbor_addr_first; // this will be first in the list -> next accessible via (pointer[i])


} PACKED;
typedef struct ack_packet_structure_MMCR apkt_t_mmcr;

//#define ACK_LENGTH_MMCR 25

// ====================================

IAR_PACKED struct tc_packet_structure_MMCR
{
    uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t length; // of the packet'd data
	uint8_t dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	uint8_t src_id; // part of DATA field
	
	uint32_t energy;
	uint8_t channel;
    //uint16_t channel_switch_countdown_ms;
	uint8_t mpr_count;
	uint16_t mpr_list;
	//uint8_t crc;	

} PACKED;
typedef struct tc_packet_structure_MMCR tpkt_t_mmcr;

//#define TC_LENGTH_MMCR 14
//#define CHSW_LENGTH_MMCR 15

// Basic size with empty MPR list (mpr_count=0)
#define RTR_MMCR_TC_PKT_SIZE	16


// ====================================
#define COUNT_LEN_MMCR 11
IAR_PACKED struct counter_packet_structure_MMCR
{
	uint8_t start;
	uint8_t flags;
	uint16_t mac_dst;
	uint16_t mac_src;
	uint8_t Hellocnt;
	uint8_t Selcnt;
	uint8_t ACKcnt;
	uint8_t DATcnt;
	uint8_t BEAMcnt;
	uint8_t BEACONcnt;
	uint8_t crc; // part of DATA field
} PACKED;
typedef struct counter_packet_structure_MMCR CNTpkt_MMCR;




typedef IAR_PACKED struct
{
	uint8_t module_type;
	uint8_t module_length;
	uint8_t command;
	uint32_t ticks;
	uint16_t rate; // in Hz
	uint8_t	channel;
	uint8_t seq_num;
	uint8_t mod_data; // begining of the data portion
}  PACKED ssn_command_mod_v1_t;


// without any module data (e.g. sensor measurments)
#define SSN_SENSOR_DATA_V1_SIZE	(sizeof(ssn_command_mod_v1_t)-3)



#ifdef _ENABLE_RFID_READER_DETECTION_

IAR_PACKED struct lgs_rfidr_detect
{
	uint8_t module_type;
	uint8_t module_length;

	uint8_t report_type;

	uint8_t	rfidr_id;
	uint8_t	rssi;

} PACKED;
typedef struct lgs_rfidr_detect lgs_rfidr_detect_packet_v2_t ;s

#endif // _ENABLE_RFID_READER_DETECTION_


#endif

