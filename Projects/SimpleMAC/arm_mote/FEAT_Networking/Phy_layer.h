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
#ifndef _PHY_LAYER_H_
#define _PHY_LAYER_H_


#include "defs/esnl_pub.h"
#include "FEAT_XBee_API/API_frames.h"


// ###############################################
// ## Constants' definitions
// ###############################################

// Clear chanel threshold = -75dBm (<-> 0x4B)
#define DEFAULT_BUSY_THRESHOLD 0x4B

#define MY_PAN_ID	0x3332
//#define MY_PAN_ID	0x1604

//#define DEFAULT_RF_CHANNEL 0x0F //0x14
#define DEFAULT_RF_CHANNEL 0x14
// used to shift the mask=1 by (X - SHIFT) bits to the left
#define CHANNEL_MASK_SHIFT 0xB

#define DEFAULT_TX_POWER 0x04

// Defines mantisa with base 2 of the time duration
// [ = 2^(mantisa) * ~15ms * #_of_channels ]
#define DEFAULT_SCAN_TIME 0

// Maximum tixks between scan requests in one TX operation
// (chain of scans during backoff)
//#define MAX_TICKS_BETWEEN_SCANS 4
#define MAX_TICKS_BETWEEN_SCANS		TICKERS_PER_SECOND/10


// Default CW (contention window value in timeslots)
#define PHY_DEFAULT_CW 0x20



// PHY_BACKOFF_SF is Scalling Factor multiplied by 1000 to accomodate for weight reprezentation
// (e.g. for 32 it is actually 0.032)
#define PHY_BACKOFF_SF 32

#define RND_DIFF 4

#define MAX_BACKOFF 50

// Timeout for TX acknowledgement (negative or positive) in TICKS
//#define DEFAULT_TX_TIMEOUT 4
#define DEFAULT_TX_TIMEOUT TICKERS_PER_SECOND/4
// ###############################################
// ## Global variables declaration
// ###############################################
extern unsigned char XDATA phy_busyThreshold_;

extern unsigned int XDATA phy_count_free;

extern unsigned int XDATA phy_count_busy;

extern unsigned long int XDATA phy_scanTimestamp_;

extern unsigned int XDATA phy_backoff_counter_;

extern unsigned int XDATA phy_cw_;

extern unsigned long int XDATA phy_sent_timeout_;

#ifdef FEAT_ENABLE_CC
extern uint32_t XDATA phy_backoff_timeout_;
extern uint8_t XDATA DATA_waiting_4_BO_;
#endif

// ###############################################
// ## Function prototypes
// ###############################################
/**
  * phy_init_scan_params() - sets the RF Scan variables and params
  *  including radio module (e.g. scan mask, scan diration, etc)
  */
void phy_init_scan_params(void);


/**
  * phy_change_backoff(mode) - switch backoff scheme
  */
void phy_change_backoff ( unsigned char new_backoff );


/**
  * phy_send_scan_request(channel) - send a scan request for a particular channel
  */
void phy_send_scan_request ( unsigned char a_channel );


/**
  * phy_send_scan_report() - sends the scan report on BCAST
  */
void phy_send_scan_report(void);


/**
  * phy_start_tx_process() - starts TX process including channel sensing and backoff
  */
void phy_start_tx_process(void);


#ifdef _ENABLE_XBEE_API_
/**
  * phy_analyse_scan_response(api_frame_with_at_response) - analyse received report
  * 	from RF scan
  */
void phy_analyse_scan_response ( api_frame_at_rsp_t* at_rsp );
#endif // _ENABLE_XBEE_API_


/**
  * phy_set_RF_channel(channel) - switch an RF channel the node is using
  */
void phy_transmit_packet(void);


// ###############################################
// ## Power control mechanizm
// ###############################################
// New API Frames Functions


extern unsigned char XDATA my_rf_channel_;
extern unsigned char XDATA my_tx_power_;

#define MAX_POWER_LEVEL 0x04

#define MAX_RF_CHANNEL 0x1A
#define MIN_RF_CHANNEL 0x0B

///////////////////////////////////////////////

void phy_set_power_level ( unsigned char my_power_level );


/**
  * phy_set_RF_channel(channel) - switch an RF channel the node is using
  */
void phy_set_RF_channel ( unsigned char a_channel );

#endif

