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

#include "Phy_layer.h"

#include "FEAT_XBee_API/API_frames.h"
#include "packet.h"
#include "common.h"
#include "HW_LIB/RTC.h"
#include "Application.h"
//#include "HW_LIB/uart_0.h"
#include "FEAT_Queuing/Queuing.h"
#include "FEAT_XBee_API/AT_scripts.h"
#include "CongestionControl.h"

#ifdef _ARM_
	#include "FEAT_STM32W/STM32W_Radio.h"
#endif // _ARM_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char XDATA phy_busyThreshold_;

unsigned int XDATA phy_count_free;
unsigned int XDATA phy_count_busy;

unsigned long int XDATA phy_scanTimestamp_;

unsigned int XDATA phy_backoff_counter_;
unsigned int XDATA phy_cw_;

unsigned char XDATA phy_current_channel_;
unsigned char XDATA my_rf_channel_;
unsigned char XDATA my_tx_power_; // configured power level (default for orodinary communication
unsigned char XDATA current_tx_power_; // current power level set on the radio

unsigned long int XDATA phy_sent_timeout_; // timeout is not used until a packet is sent to Radio module


#ifdef FEAT_ENABLE_CC
uint32_t XDATA phy_backoff_timeout_;
uint8_t XDATA DATA_waiting_4_BO_ = 0;
#endif

///////////////////////////////////////////////



/**
  * phy_init_scan_params() -
  */
void phy_init_scan_params()
{
#ifdef _ENABLE_XBEE_API_
	unsigned int XDATA uint_value;

	// Set channel to listen too
	uint_value = 1 << ( DEFAULT_RF_CHANNEL - CHANNEL_MASK_SHIFT );
	api_send_at ( "SC", ( char* ) &uint_value, 2 );

	// Set scan duration parameter
	uint_value = DEFAULT_SCAN_TIME;
	api_send_at ( "SD", ( char* ) &uint_value, 2 );
#endif // _ENABLE_XBEE_API_

	phy_busyThreshold_ = DEFAULT_BUSY_THRESHOLD;
	phy_count_free = 0;
	phy_count_busy = 0;
	phy_scanTimestamp_ = 0;
	phy_backoff_counter_ = 1;
	phy_cw_ = PHY_DEFAULT_CW;

	phy_current_channel_ = DEFAULT_RF_CHANNEL;
	my_rf_channel_ = DEFAULT_RF_CHANNEL;
	my_tx_power_ = DEFAULT_TX_POWER; // configured power level (default for orodinary communication
	current_tx_power_ = DEFAULT_TX_POWER;
	phy_sent_timeout_ = 0;
}



/**
  * phy_change_backoff(mode) - switch backoff scheme
  */
void phy_change_backoff ( unsigned char new_backoff )
{
	switch ( new_backoff )
	{
		case PHY_BACKOFF_DISABLE:
			my_backoff_ = new_backoff;
			break;
		case PHY_BACKOFF_EXPONENTIAL:
			my_backoff_ = new_backoff;
			break;
#ifdef FEAT_ENABLE_SFQ
		case PHY_BACKOFF_ADFS:
			my_backoff_ = new_backoff;
			break;
#endif // FEAT_ENABLE_SFQ
#ifdef INCLUDE_PCC
		case PHY_BACKOFF_PCC:
			my_backoff_ = new_backoff;
			cc_change_protocol();
			break;
		case PHY_BACKOFF_NO_PCC:
			my_backoff_ = new_backoff;
			cc_change_protocol();
			break;
#endif

		default:
			my_backoff_ = PHY_BACKOFF_DISABLE;
			break;
	}
}


/**
  * phy_send_scan_request(channel) -
  */
void phy_send_scan_request ( unsigned char a_channel )
{
#ifdef _ENABLE_XBEE_API_
	unsigned int XDATA uint_value;

	// Change channel only if the currently used mask is different than requested
	if ( phy_current_channel_ != a_channel )
	{
		// Set channel to listen too
		uint_value = 1 << ( a_channel - CHANNEL_MASK_SHIFT );
		api_send_at ( "SC", ( char* ) &uint_value, 2 );
		phy_current_channel_ = a_channel;
	}
	// Start scan
	uint_value = 0x0;
	api_send_at ( "ED", ( char* ) &uint_value, 1 );
#endif // _ENABLE_XBEE_API_
}



/**
  * phy_send_scan_report() -
  */
void phy_send_scan_report()
{
	unsigned char XDATA len = 7; // size of the packet header (start + flag + dst(2) + src(2) + len)
	unsigned char XDATA msg_i;
	pkt_t *pkt = ( pkt_t* ) packet;
	uint8_t *msg;
//	char *tmp;
	unsigned char n_samples;
//	unsigned int sample_value;

	pkt->start = 66;
	pkt->flags = 0x0F;
	pkt->mac_dst = 0xFFFF;
	pkt->mac_src = MY_ADDR;

	msg = & ( pkt->dst_id );

	msg_i = 0;

	// FILL END-TO-END ADDRESSING
	msg[msg_i] = BS_ADDR;
	msg[msg_i+1] = MY_ADDR;

	len = len + 2;
	msg_i = msg_i + 2;


// FILL THE REST OF 100byte packet (90 bytes of samples)
	sprintf ( (char*)& ( msg[msg_i] ), "F=%d, B=%d, BO=%d\n", phy_count_free, phy_count_busy, phy_backoff_counter_ );
	n_samples = strlen ( (char*)& ( msg[msg_i] ) );
	msg_i += n_samples;
	len += n_samples;

	// FILL the CHECKSUM
	// TODO: calculate checksum
	msg[msg_i] = 65;

	len++;
	msg_i++;

	pkt->length = msg_i; // of the packet'd data

#ifdef _ENABLE_XBEE_API_
	// send the packet
	api_send_packet16 ( packet, len, 0xFFFF );
#endif // _ENABLE_XBEE_API_
}


/**
  * phy_start_tx_process() -
  */
void phy_start_tx_process()
{
	switch ( my_backoff_ )
	{
		case PHY_BACKOFF_DISABLE:
			phy_transmit_packet();
			break;
#ifdef FEAT_ENABLE_SFQ
		case PHY_BACKOFF_ADFS:
			{
				// TODO: initiate Backoff timer and start scanning
				unsigned int r = ( rand() % RND_DIFF );
				// MZ_DFS begin
				unsigned int temp_weight = Q_SFQ_get_total_weight();				
				phy_backoff_counter_ =  ( PHY_BACKOFF_SF * 100 / temp_weight ) + r - ( RND_DIFF / 2 );
				// MZ_DFS end
				if ( ( 0 >= phy_backoff_counter_ ) || ( 0xF0 < phy_backoff_counter_ ) )
					phy_backoff_counter_ = 1;
				if ( MAX_BACKOFF <= phy_backoff_counter_ )
					phy_backoff_counter_ = MAX_BACKOFF;
				phy_send_scan_request ( DEFAULT_RF_CHANNEL );
			}
			break;
#endif // FEAT_ENABLE_SFQ
#ifdef INCLUDE_PCC
		case PHY_BACKOFF_PCC:
			{
				// TODO: initiate Backoff timer and start scanning
				/*				unsigned int r = ( rand() % RND_DIFF );
								phy_backoff_counter_ =  ( PHY_BACKOFF_SF * 100 / my_weight_ ) + r - ( RND_DIFF / 2 );
								if ( ( 0 >= phy_backoff_counter_ ) || ( 0xF0 < phy_backoff_counter_ ) )
									phy_backoff_counter_ = 1;
								if ( MAX_BACKOFF <= phy_backoff_counter_ )
									phy_backoff_counter_ = MAX_BACKOFF;
				*/
//				phy_backoff_counter_ =  cc_bo_value_;
				phy_backoff_timeout_ = rtc_get_ticks() + cc_bo_value_;
				DATA_waiting_4_BO_ = 1;
//				phy_send_scan_request ( DEFAULT_RF_CHANNEL );
			}
			break;
		case PHY_BACKOFF_NO_PCC:
			{
				phy_backoff_timeout_ = rtc_get_ticks() + PCC_DEFAULT_BO_VALUE;
				DATA_waiting_4_BO_ = 1;
			}
			break;
#endif
		case PHY_BACKOFF_EXPONENTIAL:
			// TODO: initiate Backoff timer and start scanning
			phy_backoff_counter_ =  1 + rand() % phy_cw_;
			phy_send_scan_request ( DEFAULT_RF_CHANNEL );
			if ( ( 0 >= phy_backoff_counter_ ) || ( 0xF0 < phy_backoff_counter_ ) )
				phy_backoff_counter_ = 1;
			if ( MAX_BACKOFF <= phy_backoff_counter_ )
				phy_backoff_counter_ = MAX_BACKOFF;
			break;
		default:
			// ERROR
			phy_transmit_packet();
			break;
	}
#ifdef DEBUG_MODE
	phy_send_scan_report();
#endif
}



#ifdef _ENABLE_XBEE_API_
/**
  * phy_analyse_scan_response(pkt) -
  */
void phy_analyse_scan_response ( api_frame_at_rsp_t* at_rsp )
{
	api_frame_at_rsp_one_t * at_rsp_one = ( api_frame_at_rsp_one_t* ) at_rsp;
	if ( API_AT_STATUS_OK != at_rsp->api_at_status )
	{
		// failed to execute command -> ERROR
		// TODO: handle AT command error
	}

	// DEBUG: print the packet via UART
	//		api_send_packet16( pkt, 9, 0xFFFF);

	// Check if channel is Free or Busy
	if ( phy_busyThreshold_ > ( at_rsp_one->value ) )
	{
		// Channel is BUSY!!!! because the values are
		//    absolut values of negative dBm signal strength
		// TODO: handle busy channel
		phy_count_busy++;
	}
	else
	{
		// Channel is FREE!!!! because the values are
		//    absolut values of negative dBm signal strength
		// TODO: handle free channel
		phy_count_free++;

		if ( 0 < phy_backoff_counter_ )
		{
			phy_backoff_counter_--;
			if ( 0 == phy_backoff_counter_ )
			{
				// backoff elapsed -> transmit packet
				phy_transmit_packet();
			}
//		} else {
			// phy_backoff_counter_ less or equal to '0'
//			phy_transmit_packet();
		}
	}

//#ifdef DEBUG_MODE
//	if (((phy_count_free + phy_count_busy) % 12) > 10)
//		phy_send_scan_report();
//#endif

	// Send next SCAN request and timestamp this event
	if ( 0 < phy_backoff_counter_ )
	{
		phy_send_scan_request ( DEFAULT_RF_CHANNEL );
		phy_scanTimestamp_ = rtc_get_ticks();
	}
}
#endif // _ENABLE_XBEE_API_


/**
  * phy_transmit_packet() -
  */
void phy_transmit_packet()
{
	if ( ( 0 != pkt_to_sent_len ) && ( 0xFF != pkt_to_sent_id ) )
	{
//		if (1 == app_send_data)
		{
#ifdef _ENABLE_XBEE_API_
			unsigned int base = QBUFF_BASE ( pkt_to_sent_id );
			unsigned int mac_d = get_dst_mac_base ( base );
			if ( /*(MAC_BROADCAST == mac_d) || */(BS_ADDR == mac_d) )
			{
				mac_d = BS_ADDR;
			}
			api_send_packet16_base ( pkt_to_sent_id, pkt_to_sent_len, mac_d );
//			api_send_packet16_base ( pkt_to_sent_id, pkt_to_sent_len, 0xFFFF);
			sent_DATA_ = 1;
			phy_sent_timeout_ = rtc_get_ticks() + my_tx_timeout_;
#endif // _ENABLE_XBEE_API_
		}
	}
	// Make sure that all elements are cleaned!!
//	pkt_to_sent_id = 0xFF;
//	pkt_to_sent_len = 0;
}



int8_t actual_power = 100;
StStatus result=0;
///////////////////////////////////////////////
/**
  * phy_set_power_level(power) - switch an RF TX power the node is using
  */
void phy_set_power_level ( unsigned char new_power_level )
{
	//Set the Power on the Radio Module
	current_tx_power_ = new_power_level;  // This line is bad....
	stradio_power_ = -37+(10*new_power_level);
	result = ST_RadioSetPower(stradio_power_);
	actual_power = ST_RadioGetPower();

}

///////////////////////////////////////////////

/**
  * phy_set_RF_channel(channel) - switch an RF channel the node is using
  */
void phy_set_RF_channel ( unsigned char a_channel )
{
	//Set the Channel on the Radio Module -- reset to DEFAULT_RF_CHANNEL
	my_rf_channel_ = a_channel;
	ST_RadioSetChannel(my_rf_channel_);
}

