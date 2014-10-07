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
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "defs/esnl_pub.h"

#include "HW_LIB/RTC.h"

#include "common.h"
#include "packet.h"



// ##############################################################################
// ## Application related constants
// ##############################################################################

//#define SAMPLES_PER_PACKET 90

// values for "send_data"
#define ENABLE_SENDING_SAMPLE_DATA 1
#define DISABLE_SENDING_SAMPLE_DATA 0

// values for "tx_data_mode_"
#define NORMAL_TX_DATA_MODE 0
#define TEST_TX_DATA_MODE 1

//#define SWRSF (unsigned char)0x10



#ifdef TEST_MODE
// test data mode (1)
#define APP_DEFAULT_TX_DATA_MODE APP_TX_DATA_MODE_DATA_V2
#else
// normal data mode (0)
#define APP_DEFAULT_TX_DATA_MODE APP_TX_DATA_MODE_ADC
#endif

// Wait 10 seconds before "panicing" about lost BS BEAM (direct connection)
#define APP_MAX_BS_IDLE	10000

// Allow for large window ahead (but prevent from sending beam
//  that is only slightly behind - or in other words very recently used sequence)
#define APP_BEAM_WINDOW	230

// ##############################################################################
// ## ISN Config generic header (type+length)
// ##############################################################################
IAR_PACKED struct app_config_generic_structure
{
	unsigned char config_flag;
	unsigned char length;
	unsigned char config_data;
} PACKED ;
typedef struct app_config_generic_structure config_generic_t;

// ##############################################################################
// ## ISN Config SAMPLING option (ADC configuration)
// ##############################################################################
IAR_PACKED struct app_config_sampling_structure
{
	// tx_data mode (0 - real data, 1 - test data)
	unsigned char tx_data_mode;
	// samples rate divider
	unsigned char adc_sample_rate_divider;
	// sample averaging over "N"
	unsigned char adc_sample_averaging;
	// No. of samples per packet
	unsigned char n_samples_per_packet;
	// Number of ADC channels
	unsigned char num_of_channels;
} PACKED ;
typedef struct app_config_sampling_structure config_sampling_t;


// ##############################################################################
// ## ISN Config SAMPLING MAP option (ADC multiplexing)
// ##############################################################################
IAR_PACKED struct app_config_sampling_map_structure
{
	// num _of_channels - number of active ADC channel
	unsigned char num_of_channels;
	// list of Pin ID for each channel
	unsigned char pin_id;
} PACKED ;
typedef struct app_config_sampling_map_structure config_sampling_map_t;



// ##############################################################################
// ## ISN Config RF option (RF channel, TX power, etc )
// ##############################################################################
#define APP_CONFIG_RF_PC_NONE 0x00

IAR_PACKED struct app_config_rf_structure
{
	// radio channel to be used (0 - 0xB, 1 - 0xC, ... 15 - 0x1A)
	unsigned char channel;
	// power control scheme
	unsigned char power_control_scheme;
	// default TX power level to be used
	unsigned char default_power_level;
	// rssi threshold for reception of protocol info
	//(not for debuging/testing/special messages)
	unsigned char rssi_threshold;
} PACKED ;
typedef struct app_config_rf_structure config_rf_t;



// ##############################################################################
// ## Config MMCR derived options (energy, distance, CH address)
// ##############################################################################
IAR_PACKED struct app_config_mmcr_structure
{
	// set battery energy
	unsigned long int energy;
	// RF retries
	unsigned char rf_retries;
} PACKED ;
typedef struct app_config_mmcr_structure config_mmcr_t;

// ##############################################################################
// ## ISN Config OEDSR derived options (energy, distance, CH address)
// ##############################################################################
IAR_PACKED struct app_config_oedsr_structure
{
	// set battery energy
	unsigned long int energy;
	// distance to BS
	unsigned long int distance;
	// RF retries
	unsigned char rf_retries;
} PACKED ;
typedef struct app_config_oedsr_structure config_oedsr_t;




// ##############################################################################
// ## ISN Config PROTOCOLS option
// ##   (routing: OEDSR, AODV,
// ##       later also scheduling: ADFS)
// ##############################################################################
#define PROTOCOL_ROUTING_OEDSR 0x00
#define PROTOCOL_ROUTING_AODV 0x01
#define PROTOCOL_SCHEDULING_NONE	0x00
#define PROTOCOL_SCHEDULING_ADFS	0x01
#define PROTOCOL_SCHEDULING_NONE_BO	0x02
#define PROTOCOL_SCHEDULING_ADFS_BO	0x03
#define PROTOCOL_SCHEDULING_PCC_BO	0x04
#define PROTOCOL_SCHEDULING_PCC_SFQ_BO	0x05
#define PROTOCOL_SCHEDULING_NO_PCC_BO	0x06

IAR_PACKED struct app_config_protocols_structure
{
	// routing protocol
	unsigned char routing;
	// scheduling protocol
	unsigned char scheduling;
} PACKED ;
typedef struct app_config_protocols_structure config_protocols_t;


// ##############################################################################
// ## Config SHEDULING options
// ##   (source weight and defauilf node (all flows) weight
// ##
// ##############################################################################
#define SCHEDULING_WEIGHT_NO_CHANGE 0xFFFF

IAR_PACKED struct app_config_scheduling_structure
{
	// Source weight (if the node is the source) - initial packet weight
	unsigned int source_weight;
	// Initial node's weight
	unsigned int node_weight;
	// MISC - for future use
	unsigned long int misc;
} PACKED ;
typedef struct app_config_scheduling_structure config_scheduling_t;


// ##############################################################################
// ## Config CLUSTERING option
// ##   (who is the cluster head)
// ##
// ##############################################################################
#define CLUSTERING_I_AM_CH 0xFD
#define CLUSTERING_BS 0xFE
#define CLUSTERING_NO_CHANGE 0xFF

IAR_PACKED struct app_config_clustering_structure
{
	// CH address
	unsigned char CH;
	// MISC for future use
	unsigned long int misc;
} PACKED ;
typedef struct app_config_clustering_structure config_clustering_t;






////////////////////////////////////////////////////////
typedef enum {EVENT_UNKNOWN = 0, EVENT_DSEND, EVENT_DGEN, EVENT_DRECV} event_t;
typedef enum {REASON_UNKNOWN = 0, REASON_NOROUTE, REASON_NOACK, REASON_RADIO_FAILURE, REASON_QUEUE_FULL, REASON_MEMORY_FULL } reason_t;
typedef enum {MODULE_UNKNOWN = 0, MODULE_RTR, MODULE_PHY, MODULE_QUEUE, MODULE_APP} module_t;

// ##############################################################################
// ## ISN Application related variables and functions
// ##############################################################################

// indicates if the data packet should be actually sent
//extern bit app_send_data;
extern uint8_t app_generate_pkt_;


extern unsigned char XDATA n_samples_per_packet_;	// samples per packet

extern unsigned char XDATA app_tx_data_mode_; // normal data mode (0)

extern unsigned char XDATA n_samples_per_packet_;	// samples per packet

extern unsigned char XDATA last_config_rssi_rcv_;


extern unsigned int XDATA app_beam_count_; // Count received beams
extern unsigned int XDATA app_beacon_count_;	// Received beacon messages

extern rtc_tick_t app_last_BS_contact_;

/**
  * app_init() - resets app counters and variables
  */
void app_init(void);

/**
 *  app_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
 */
void app_loop( void );


/**
  * void app_recvStopData(pkt_t *p) - stops data transmission (after reception of packet form BS)
  */
void app_recvStopData ( pkt_t *p );

/**
  * app_recvStartData(pkt_t *p) - starts data transmission (after reception of packet form BS)
  */
void app_recvStartData ( pkt_t *p );

/**
  * app_recvConfigSensor(pkt_t *p) - analyses Config packet and sets the configuration
  */
void app_recvConfigSensor ( pkt_t *p , unsigned char rssi );

#ifdef _ENABLE_ADC_MODULE_
/**
  * app_make_and_enqueue(unsigned char channel) - create packet and put into a queue
  */
void app_make_and_enqueue ( unsigned char channel );
#endif // _ENABLE_ADC_MODULE_

/**
  * app_recvBEAM(char *pkt) - called after reception of BEAM message from BS (APP related handling)
  */
void app_recvBEAM ( sint8_t *pkt );

#if defined (_ENABLE_ADC_MODULE_) || defined (_ENABLE_NEW_ADC_MODULE_)
/**
  * app_suspendADC() - suspends ADC conversion
  */
void app_suspendADC(void);

/**
  * app_resumeADC() - resumes ADC conversion (and generation of packets, though not sending of them)
  */
void app_resumeADC(void);

#endif // #if defined (_ENABLE_ADC_MODULE_) || defined (_ENABLE_NEW_ADC_MODULE_)





/**
  * enable_node(char *p)
  */
void enable_node ( char *p );

/**
  * disable_node(unsigned char address);
  */
void disable_node ( unsigned char address );

/**
  * NodeConfigure(char *p)
  */
void NodeConfigure ( char *p );

/**
  * higher_function_packet(char *p, unsigned char rssi)
  */
void higher_function_packet ( char *p, unsigned char rssi );




/**
  * app_drop_pkt(base,module_name, reason, event) - handles packet dropping
  *     possible actions are - counting errors, rescheduling packet for retransmission
  */
void app_drop_pkt ( unsigned int base, module_t module_name, reason_t reason, event_t event );





extern unsigned char XDATA network_search_mode_;
extern unsigned long XDATA switch_timeout_;
extern unsigned long XDATA switch_start_time_;


void app_start_search_channel ( pkt_t *p );
void app_search_switch_channel(void);
void app_received_beacon ( pkt_t * p );
void app_send_join ( unsigned char dest_id, unsigned char channel, unsigned char mode );

#endif

