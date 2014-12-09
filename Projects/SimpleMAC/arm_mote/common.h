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

#ifndef _COMMON_H_
#define _COMMON_H_


#include "defs/esnl_pub.h"

// #############################################################
// ## Common defines										  ##
// #############################################################



// Debugging of OEDSR ACKs (node that send HELLO will report up to 3 AKC responses)
//#define DEBUG_OEDSR_ACKS
#undef DEBUG_OEDSR_ACKS

// routing debuging
#define DEBUG_ROUTE
//#undef DEBUG_ROUTE

// Defines if the node is a SOURCE of ADC data packets
//#define SOURCE
//#undef SOURCE

// Define what is a default operation mode - if defined then use test-mode code
// e.g. send sequence number instead of real ADC data
//#define TEST_MODE
#undef TEST_MODE

// Defines if debug mode should be used, where the extra info is sent via radio
#undef DEBUG_MODE
//#define DEBUG_MODE

// #############################################################
// ## ADFS and BACK-OFF implementation
// #############################################################


//#define DEFAULT_SOURCE_WEIGHT 200

// #############################################################
// ## Common defines for the whole network
// #############################################################
// Defines how many attempts to send data there sould be before
// declaring the current route as inactive -> and start new route discovery
#define MAX_TX_FAILED 3

// 10 = 0xA
#define BS_ADDR 0xFE

#define MAC_BROADCAST 0xFFFF

#define LOG_ADDR 0xB
//#define APPEND_LOG(s,l) api_send_packet16(s, l, LOG_ADDR)
#define APPEND_LOG(s,l) // NO LOGING


#define serialReadByte __io_getcharNonBlocking


// #############################################################
// #############################################################
extern unsigned char XDATA energy_depleted_control_;
extern unsigned char XDATA my_rssi_threshold_;
extern unsigned char XDATA my_protocol_;
extern unsigned char XDATA my_scheduling_;
extern unsigned char XDATA my_backoff_;
//extern uint16_t XDATA my_weight_;
//extern uint16_t XDATA my_source_weight_;
extern uint16_t XDATA my_CH_;
extern uint32_t XDATA my_tx_timeout_; // Timeout for receiveing confirmation of packet TX (expires if no reponse from Radio)

// RF retries apply to Maxstream radio modules -> 0=3 retries, 1=>6 retries etc
#define DEFAULT_RF_RETRIES 2
#define MAX_RF_RETRIES 6
#define NO_CHANGE_RF_RETRIES 0xF
extern unsigned char XDATA my_RF_retries_;



// #############################################################


extern uint8_t XDATA request_send_HELLO_;
extern uint8_t XDATA sent_DATA_;

extern uint8_t request_debug_packet_;

void send_PONG ( uint16_t mac_dst );

extern uint8_t XDATA last_successful_frame_id_;

extern int8_t XDATA rx_needs_to_send; // when the RX buffer have to be sent via TX



extern uint8_t XDATA pkt_to_sent_id;
extern uint8_t XDATA pkt_to_sent_len;
//extern char * xdata pkt_to_sent;

extern uint16_t XDATA mac_dest;

#define SHORT_PACKET_SIZE 20
extern sint8_t XDATA packet[115]; // packet to be filled before transmission
extern int8_t XDATA short_packet[SHORT_PACKET_SIZE];
extern int8_t XDATA rx_packet[115]; // packet to be filled with received packet




extern char XDATA enableDataTx_;

// OEDSR constants

// typical delay is taken as 0.1 ms
//#define TYPICAL_DELAY_FOR_LINK 1000
#define TYPICAL_DELAY_FOR_LINK 1
// delay if expressed in micro seconds
//#define DELAY_SCALING 1000000
#define DELAY_SCALING 100

extern unsigned long int XDATA my_energy_;
extern unsigned long int XDATA my_delay_;
extern unsigned long int XDATA my_distance_;


// Prototype for "config.h"
void Init_Device ( void );

// Prototype Power Up initialization (early stages -> tasking, etc)
void main_power_up();
	
/**
  * release_pkt_in_tx() - releases packet that is being transmited
  *		this includes buffer freeing and reseting "pkt_to_sent_*" variables
  */
void release_pkt_in_tx(void);




// #################################################################3
// ## comon MACROS
// #################################################################3


// Standard MIN/MAX macros
#define MIN(a,b) (a>b)?b:a
#define MAX(a,b) (a>b)?a:b

#endif
