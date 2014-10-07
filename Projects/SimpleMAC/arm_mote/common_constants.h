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

#ifndef _COMMON_CONSTANTS_H_
#define _COMMON_CONSTANTS_H_

#include "defs/esnl_pub.h"

// Field values for XBEE Frame
#define XBEE_FRAME_START_BYTE_VALUE 0x7e

//#define XBEE_FRAME_API_ID_VALUE_AT_CMD      0x08
//#define XBEE_FRAME_API_ID_VALUE_AT_RESPONSE 0x88
//#define XBEE_FRAME_API_ID_VALUE_TX16        0x01

//#define XBEE_FRAME_API_ID_VALUE_RX64        0x80
//#define XBEE_FRAME_API_ID_VALUE_RX16        0x81

#define XBEE_FRAME_API_ID_VALUE_STATUS 0x8A
#define XBEE_FRAME_API_ID_VALUE_AT_CMD 0x08
#define XBEE_FRAME_API_ID_VALUE_AT_RSP 0x88
#define XBEE_FRAME_API_ID_VALUE_TX_64  0x00
#define XBEE_FRAME_API_ID_VALUE_TX_16  0x01
#define XBEE_FRAME_API_ID_VALUE_TX_STA 0x89
#define XBEE_FRAME_API_ID_VALUE_RX_64  0x80
#define XBEE_FRAME_API_ID_VALUE_RX_16  0x81


// API TX Status responses
// Succesful transmission
#define TX_STATUS_OK 0x00
#define TX_STATUS_F_ACK 0x01
#define TX_STATUS_F_CCA 0x02
#define TX_STATUS_F_PURGED 0x03

// API AT cammnad responses
#define API_AT_STATUS_OK 0
#define API_AT_STATUS_ERROR 1


//Definition of Network Flags
//Packet Start and Stop Bytes
#define START_BYTE	0x42
#define STOP_BYTE	0x41
/////////////////////////////
#define FLAG_PING			0x01
#define FLAG_PONG			0x02
#define FLAG_RESET_SW		0x07
#define FLAG_CONFIG_NODE	0x08
#define FLAG_NEW_CONFIG_NODE	0x11
#define FLAG_CONFIG_RESP	0x10
#define FLAG_STOP_DATA		0x09
#define FLAG_GOODBYE		0x77

/////////////////////////////
#define FLAG_BEACON 0x13
#define FLAG_JOIN	0x14
#define FLAG_START_SEARCH_CHANNEL 0x15
#define FLAG_CHANNEL_SWITCHING_V2 0x16
/////////////////////////////
#define FLAG_BEAM	0x0A
#define FLAG_HELLO	0x0B
#define FLAG_ACK	0x0C
#define FLAG_STARTD	0x0D
#define FLAG_SELECT 0x0E
/////////////////////////////
// MMCR
//hindu start
#define FLAG_HELLO_MMCR	0x6A
#define FLAG_ACK_MMCR	0x6B
#define FLAG_TC_MMCR	0x6C
#define FLAG_CHSW_MMCR 0x6D
#define FLAG_SELECT_MMCR 0x6E
//hindu end
/////////////////////////////
#define FLAG_DATA			0x0F
#define FLAG_DATA_ARB		0x73
#define FLAG_DATA_MODULAR	0x74
/////////////////////////////
#define FLAG_ENABLE_NODE	0x67
#define FLAG_DISABLE_NODE	0x68
#define FLAG_STOP_COUNTERS	0x71

#define FLAG_DATA_TEST		0x72
#define FLAG_DATA_TEST_V2	0x75
#define FLAG_DATA_TEST_EMPTY	0x76

#define FLAG_SEND_COUNTERS	0x78
#define FLAG_COUNTERS		0x79
/////////////////////////////
#define FLAG_ACK_DEBUG 0x80
/////////////////////////////
#define FLAG_PROBA  0x52
#define FLAG_PCC_V2			0x81
/////////////////////////////
/////////////////////////////
#define ROUTEFLAG_RREQ	0xB0
#define ROUTEFLAG_RREP	0xB1
#define ROUTEFLAG_ACK	0xB2
#define ROUTEFLAG_RERR	0xB3
/////////////////////////////

// ##############################################################################
// ## ISN Config definitions BEGIN
// ##############################################################################
#define ISN_APP_CONFIG_FLAG_END		0x41
#define ISN_APP_CONFIG_FLAG_SAMPLING	0x01
#define ISN_APP_CONFIG_FLAG_SAMPLING_MAP	0x02
#define ISN_APP_CONFIG_FLAG_RF		0x03
#define ISN_APP_CONFIG_FLAG_OEDSR	0x04
#define ISN_APP_CONFIG_FLAG_MMCR	0x08
#define ISN_APP_CONFIG_FLAG_PROTOCOLS	0x05
#define ISN_APP_CONFIG_FLAG_SCHEDULING	0x06
#define ISN_APP_CONFIG_FLAG_CLUSTERING	0x07


#define ISN_APP_NEW_CONFIG_OFFSET_FROM_CUSTOM_HEADER 9

#define ISN_APP_CONFIG_OFFSET_FROM_CUSTOM_HEADER 4

// Length of the common header length
#define ISN_APP_CONFIG_LEN_OVERHEAD 0x02

// Sampling Config Option
#define ISN_APP_CONFIG_RAW_LEN_SAMPLING 0x05
#define ISN_APP_CONFIG_LEN_SAMPLING     ISN_APP_CONFIG_RAW_LEN_SAMPLING + ISN_APP_CONFIG_LEN_OVERHEAD

// Sampling map has variable length (add number of channels to this one)
#define ISN_APP_CONFIG_RAW_LEN_SAMPLING_MAP 0x01
#define ISN_APP_CONFIG_LEN_SAMPLING_MAP     ISN_APP_CONFIG_RAW_LEN_SAMPLING_MAP + ISN_APP_CONFIG_LEN_OVERHEAD
// RF config fields (4 1-byte parameters)
#define ISN_APP_CONFIG_RAW_LEN_RF       0x04
#define ISN_APP_CONFIG_LEN_RF           ISN_APP_CONFIG_RAW_LEN_RF + ISN_APP_CONFIG_LEN_OVERHEAD

// OEDSR config has two "unsigned long int" + 1 byte -> 2x4 bytes
#define ISN_APP_CONFIG_RAW_LEN_OEDSR    0x09
#define ISN_APP_CONFIG_LEN_OEDSR        ISN_APP_CONFIG_RAW_LEN_OEDSR + ISN_APP_CONFIG_LEN_OVERHEAD

// MMCR config has one "unsigned long int" + 1 byte
#define ISN_APP_CONFIG_RAW_LEN_MMCR    0x05
#define ISN_APP_CONFIG_LEN_MMCR        ISN_APP_CONFIG_RAW_LEN_MMCR + ISN_APP_CONFIG_LEN_OVERHEAD

// Protocols Selection config
#define ISN_APP_CONFIG_RAW_LEN_PROTOCOLS 0x02
#define ISN_APP_CONFIG_LEN_PROTOCOLS    ISN_APP_CONFIG_RAW_LEN_PROTOCOLS + ISN_APP_CONFIG_LEN_OVERHEAD

// scheduling configuration -> weights (source and node's default)
#define ISN_APP_CONFIG_LEN_SCHEDULING 0x08 + ISN_APP_CONFIG_LEN_OVERHEAD
// Clustering ("self-organization" configuration) -> CH id (or itself)
#define ISN_APP_CONFIG_LEN_CLUSTERING 0x05 + ISN_APP_CONFIG_LEN_OVERHEAD


// Channel searching application constants

#define SEARCH_OFF					0x00
#define SEARCH_BEACON_BS_SWITCHING 	0x01
#define SEARCH_BEACON_NODE_SWITCHING 0x02
#define SEARCH_OEDSR_HELLO 			0x03
#define SEARCH_MMCR_HELLO 			0x04

// beacon interval in ticks (ms)
//#define SEARCH_BEACON_INTERVAL			500
//#define SEARCH_BEACON_INTERVAL_GUARD	50
//#define SEARCH_BEACON_SWITCH_INTERVAL	SEARCH_BEACON_INTERVAL + SEARCH_BEACON_INTERVAL_GUARD
//
//#define SEARCH_OEDSR_HELLO_SWITCH_INTERVAL 2


#define SEARCH_BEACON_INTERVAL			TICKERS_PER_SECOND/2
#define SEARCH_BEACON_INTERVAL_GUARD	TICKERS_PER_SECOND/20
#define SEARCH_BEACON_SWITCH_INTERVAL	TICKERS_PER_SECOND + SEARCH_BEACON_INTERVAL_GUARD

#define SEARCH_OEDSR_HELLO_SWITCH_INTERVAL TICKERS_PER_SECOND/10


// ==========================================
// Application level constanst
// ==========================================

// Packet (data) generation mode (type of packet)
// Data generation modes (real ADC or test)
#define APP_TX_DATA_MODE_ADC 0
#define APP_TX_DATA_MODE_TEST 1
#define APP_TX_DATA_MODE_TEST_V2	2
#define APP_TX_DATA_MODE_DATA_V2	3

// ==========================================
// Test packet constanst
// ==========================================
// Different Backoff schemes
#define PHY_BACKOFF_DISABLE		0x00
#define PHY_BACKOFF_EXPONENTIAL	0x01
#define PHY_BACKOFF_ADFS 		0x02
#define PHY_BACKOFF_PCC			0x03
#define PHY_BACKOFF_NO_PCC		0x04


// ==========================================
// Test packet constanst
// ==========================================

#define PKT_MODULE_TYPE_ADFS 0x10
#define PKT_MODULE_TYPE_PONG 0x11
#define PKT_MODULE_TYPE_HOPLIST 0x20
#define PKT_MODULE_TYPE_PCC_UPSTREAM 0x30

#define PKT_MODULE_TYPE_RTLS_REPORT	0x44
#define PKT_MODULE_TYPE_RTLS_CMD	0x45
#define PKT_MODULE_TYPE_RTLS_BEACON	0x46

#define PKT_MODULE_TYPE_CS_V1	0x50
#define PKT_MODULE_TYPE_RAW_CHANNEL		0x60
#define PKT_MODULE_TYPE_RAW_CHANNEL_V1	0x61
#define PKT_MODULE_TYPE_RAW_CHANNEL_V2	0x62
#define PKT_MODULE_TYPE_RAW_CHANNEL_V3	0x63
#define PKT_MODULE_TYPE_SSN_V1			0x66

#define PKT_MODULE_TYPE_RS_RSSI_REPORT	0x6A

#define PKT_MODULE_TYPE_TRUCK_CMD_V1	0x70
#define PKT_MODULE_TYPE_BOT_CMD_V1		0x71

#define PKT_MODULE_TYPE_QRHELO_CMD_V1	0x72

#define PKT_MODULE_TYPE_VEX_TANK_CMD_V1	0x73

#define PKT_MODULE_TYPE_MON_POWER_CMD_V1	0x80
#define PKT_MODULE_TYPE_MON_POWER_REPORT_V1	0x81

#define PKT_MODULE_TYPE_HSET_REPORT_V1	0x90
#define PKT_MODULE_TYPE_HSET_SAMPLES_V1	0x91

#define PKT_MODULE_TYPE_RFID_READER_V1	0xA0

#define PKT_MODULE_TYPE_LGS_RFIDR_DETECT_REPORT 0xA5
#define PKT_MODULE_TYPE_LGS_GATEMOTE_BEACON		0xA6
#define PKT_MODULE_TYPE_LGS_MOTE_REPORT			0xA7
#define PKT_MODULE_TYPE_LGS_INVENTORY_REPORT	0xA8
#define PKT_MODULE_TYPE_LGS_MOTE_BEACON			0xA9
#define PKT_MODULE_TYPE_LGS_MOTE_ASSOC_REQ		0xAA
#define PKT_MODULE_TYPE_LGS_MOTE_QUERY			0xAB


#define PKT_MODULE_TYPE_ADC_CYCLE_REPORT_V1	0xB0

#define PKT_MODULE_TYPE_FLA_BASIC	0xE0

#define PKT_MODULE_TYPE_END		0x41

#define PKT_MODULE_TYPE_DUMMY 0xF0


// Maximum bytes used by hoplist!!
#define MAX_HOPLIST_COUNT 30

#define PKT_MODULE_LEN_HEADER 2
#define PKT_MODULE_LEN_ADFS 5
// HOPLIST HAS VARIABLE LENGTH
//#define PKT_MODULE_LEN_HOPLIST 2
//#define PKT_MODULE_LEN_HOPLIST 2+MAX_HOPLIST_COUNT

#define PKT_MODULE_LEN_PCC_UPSTREAM 2

#define PKT_MODULE_LEN_DUMMY 0
#define MAX_PKT_SIZE 99

// Old with node id, rssi and power level
#define HOPLIST_TYPE_V1 0x01
// Congestion control version ("V1" + queue level + throughput)
#define HOPLIST_TYPE_CC 0x02
#define HOPLIST_TYPE_FUTURE 0x03

// MZ_DFS
#define SFQ_FIXED_WEIGHT_DFS	0x00
#define SFQ_WEIGHT_UPDATE_ADFS	0x01

#define PROTOCOL_SCHEDULING_DFS 0x04
#define PROTOCOL_SCHEDULING_DFS_BO 0x05
//#define HOPLIST_TYPE_CC_NODE_LIST_SIZE	7
#define HOPLIST_TYPE_CC_NODE_STRUCTURE_SIZE	8
#define HOPLIST_TYPE_CC_OVERHEAD_SIZE	3
#define PKT_MODULE_LEN_HOPLIST	MAX_HOPLIST_COUNT+HOPLIST_TYPE_CC_OVERHEAD_SIZE


#define PMOD_CS_V1_LENGTH	8

/*! Second has by default 1000 ticks (miliseconds)*/
#ifdef QT4
	#define DEFAULT_ONE_SECOND 1000
#endif

/*!
* CHANNEL SWITCHING CONSTANTS
*/
/*! Default interval between switching */
#define CS_DEFAULT_SWITCHING_INTERVAL 1*DEFAULT_ONE_SECOND
/*! Minimal interval between switching */
#define CS_MIN_CHANNEL_SWITCHING_INTERVAL	0.05*DEFAULT_ONE_SECOND
/*! Maximal interval between switching */
#define CS_MAX_CHANNEL_SWITCHING_INTERVAL	0xFFFF
/*! Interval value that disables switching */
#define CS_DISABLE_INTERVAL	0

#define CS_DISABLE_TIMEOUT 0xFFFFFFFF


#define NO_CHANNEL_CHANGE 0xFF


/*!
* MESSAGE-RELATED CONSTANTS (e.g. lengths)
*/

#define BEAM_CUSTOM_LENGTH	11
#define BEAM_TOTAL_LENGTH	11+7


#define CS_PKT_V2_CUSTOM_LENGTH 14
#define CS_PKT_V2_TOTAL_LENGTH 14+7


// ###############3 POWER MONITORING_ MESSAGE INFO ###############################
#define PKT_MODULE_LEN_MON_POWER_REPORT_V1	27
#define PKT_MODULE_LEN_MON_POWER_REPORT_V2	68

// ############### CAMT HEADSET MESSAGE INFO ###############################
#define PKT_MODULE_LEN_HSET_REPORT_V1	20
#define PKT_MODULE_LEN_HSET_SAMPLES_V1	1

// ###############3 POWER MONITORING_ MESSAGE INFO ###############################
#define PKT_MODULE_LEN_ADC_CYCLE_REPORT_V1	4

#define ADC_CYCLE_REPORT_V1	1

// ############### LOGISTICS REPORT MESSAGE INFO ###############################
#define PKT_MODULE_LEN_LGS_RFIDR_DETECT_REPORT	4
#define LGS_RFIDR_REPORT_V1	0x01

#define PKT_MODULE_LEN_LGS_GATEMOTE_BEACON	5







// ##############################################################################
// ## ENERGY SIMULATION RELATED CONSTANTS
// ##############################################################################

/*! This is a maximum value of energy that should be taken into account
* for routing discovery (ranges are important to be in a "sweat spot")
* The larger value will saturate the OEDSR link (cost) preference.
*/
#define ENERGY_MAX_VALUE 0x2AB98
/*! This is a value that indicates that the energy value should not be updated
* usefull for the configuration packet
*/
#define ENERGY_IGNORE_VALUE 0xFFFFFFFF
/*! This is a value that disables the energy counting down
* for now it will be couted but since it is A LOT it will not matter
*/
#define ENERGY_DISABLE_VALUE 0xFFFFFFFE


#endif

