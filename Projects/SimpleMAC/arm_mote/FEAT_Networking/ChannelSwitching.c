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

#include "defs/esnl_pub.h"

#ifdef FEAT_ENABLE_CH_SWITCHING

#include "common.h"
#include "ChannelSwitching.h"
#include "HW_LIB/RTC.h"
#include "FEAT_Queuing/Queuing.h"
//#include "packet.h"
//#include "API_frames.h"
#include "Phy_layer.h"
#include "FEAT_Scheduler/sch_basic_pub.h"
#include "sys_func.h"



uint8_t cs_enabled_;//xdata cs_enabled_;

uint32_t cs_switching_timeout_;//xdata cs_switching_timeout_;
uint16_t cs_switching_interval_;//xdata cs_switching_interval_;
uint8_t cs_next_channel_;//xdata cs_next_channel_;
uint8_t cs_alternative_ch_;//xdata cs_alternative_ch_;
uint8_t cs_current_channel_;//xdata cs_current_channel_;


uint8_t cs_pkt_id;// = NO_PACKET_INDEX;
uint32_t cs_switch_time_;// = CS_DISABLE_TIMEOUT;

uint8_t	cs_bcast_tidx_;
uint8_t	cs_end_tidx_;
	
extern char str_CS_SWITCH[] = "SC_SWITCH";
extern char str_CS_SWITCH_END[] = "CS_SWITCH_END";

pkt_mod_cs_t *mod_cs;
// ##############################################################################
// ## Misc counters
// ##############################################################################

// ##############################################################################
// ## Function Section
// ##############################################################################

/**
  * TEMPLATE
  */

/**
 * void cs_power_up ( ) - set local variables of the Channel Switching module
 */
void cs_power_up ( )
{
	cs_pkt_id = NO_PACKET_INDEX;
	cs_switch_time_ = CS_DISABLE_TIMEOUT;
	cs_bcast_tidx_ = SCH_NO_TIMEOUT_ID;
	cs_end_tidx_ = SCH_NO_TIMEOUT_ID;

    cs_next_channel_ = DEFAULT_RF_CHANNEL;
    //cs_switch_count_down_ = 0;
    cs_switching_timeout_ = CS_DISABLE_TIMEOUT ; // not important since disabled

//	cs_enabled_ = CS_DISABLED;
//	cs_switching_interval_ = CS_DEFAULT_SWITCHING_INTERVAL;
//	cs_next_channel_ = DEFAULT_RF_CHANNEL; /*! Default channel == no switching */
//	cs_alternative_ch_ = DEFAULT_RF_CHANNEL;
//	cs_current_channel_ = DEFAULT_RF_CHANNEL;
	mod_cs = ( pkt_mod_cs_t* )NULL;
}

/**
 * void cs_init ( ) - initialize Channel Switching Module
 */
void cs_init ( )
{
    // Nothing to do -> switch request will initiate the process
}


/**
 *
 */
void cs_req_switch ( uint8_t new_ch, uint16_t timeout)
{
	cs_next_channel_ = new_ch;
	cs_switch_time_ = rtc_get_ticks() + timeout;
	cs_bcast_tidx_ = sch_create_timeout(rtc_get_ticks()+CS_REQ_REPEAT_TIMEOUT, cs_send_req_Switch, 0, str_CS_SWITCH);
	cs_end_tidx_ = sch_create_timeout(cs_switch_time_, cs_end_switch, 0, str_CS_SWITCH_END);
}


/**
 * void cs_end_switch ( uint8_t *context ) - finishes the Channel switching
 *			(execute switch to the target channel)
 */
void cs_end_switch ( uint8_t *context )
{
	if (SCH_NO_TIMEOUT_ID != cs_bcast_tidx_)
	{
		sch_remove_timeout(cs_bcast_tidx_, str_CS_SWITCH);
		cs_bcast_tidx_ = SCH_NO_TIMEOUT_ID;
	}
	phy_set_RF_channel(cs_next_channel_);
	routing_init_MMCR(); /// ?? Is it enough
	if ( NULL != mod_cs )
	{
		tsp_release_packet(cs_pkt_id);
		mod_cs = NULL;
	}

}



/**
 * cs_send_req_Switch ( uint8_t *context ) - sends a channel switch request (BCAST)
 */
void cs_send_req_Switch ( uint8_t *context )
{
	uint32_t timeout = 0;
	uint8_t module_size = PMOD_CS_V1_LENGTH;
	if ( NULL == mod_cs )
	{
		mod_cs = ( pkt_mod_cs_t* ) tsp_new_module( PKT_MODULE_TYPE_CS_V1,
									module_size, &cs_pkt_id, MAC_BROADCAST);
	}
	if (NULL == mod_cs)
	{
		// Error
		cs_pkt_id = NO_PACKET_INDEX;
		sys_error(SYS_ERROR_MEM_FULL);
		return;
	}
	mod_cs -> interval = ENDIAN16(0);	// NOT USED FOR NOW
	mod_cs -> next_ch = cs_next_channel_;
	mod_cs -> alternative_ch = cs_next_channel_; // NOT USED FOR NOW
	timeout =  (cs_switch_time_ > rtc_get_ticks()) ? (cs_switch_time_ - rtc_get_ticks()) : 0 ;
	mod_cs -> first_timeout = ENDIAN32 ( timeout );
	
	//tsp_send_from_modules(cs_pkt_id);
	unsigned int base = QBUFF_BASE ( cs_pkt_id );
	uint8_t len = PAK_GET_TOTAL_LENGTH ( cs_pkt_id );
	sendPriorityPacket(len, (sint8_t*)&(QBUFF_ACCESS(base, 0)) , MAC_BROADCAST);

	cs_bcast_tidx_ = sch_create_timeout(rtc_get_ticks()+CS_REQ_REPEAT_TIMEOUT, cs_send_req_Switch, 0, str_CS_SWITCH);
}

/**
 * cs_recv_switching_request(pkt) - handle channel switching request
 *	
 */
void cs_recv_switching_request( char *module )
{
	pkt_mod_cs_t *cs_module = ( pkt_mod_cs_t * ) &(module[2]); // map the content
	cs_next_channel_ = cs_module->next_ch;
	if (SCH_NO_TIMEOUT_ID != cs_end_tidx_)
	{
		//sch_remove_timeout(cs_end_tidx_);
		// = SCH_NO_TIMEOUT_ID;
		return;
	}
	cs_switching_timeout_ = rtc_get_ticks() + ENDIAN32(cs_module -> first_timeout);
	cs_end_tidx_ = sch_create_timeout( cs_switching_timeout_,
									  cs_end_switch, 0, str_CS_SWITCH_END);
//	cs_switching_interval_ = cs_module->interval;	
//	cs_alternative_ch_ = cs_module->alternative_ch;
//	cs_switching_timeout_ = cs_module->first_timeout;
}

#if (0)

#ifdef CS_ENABLE_DATA_ACCESS_FUNCT
/**
  *
  */
void cs_activate( bit activate)
{
	if (activate)
	{
		if ( (CS_DISABLE_TIMEOUT == cs_switching_timeout_ )
			|| (cs_switching_timeout_ < rtc_get_ticks() ) )
		{
			// If not set or STALE then set based on current time
			cs_switching_timeout_ = rtc_get_ticks() + cs_switching_interval_ ;
			if (cs_switching_timeout_ > RTC_MAX_TIMER_VALUE)
			{
				cs_switching_timeout_ -= RTC_MAX_TIMER_VALUE;
			}
		}
		cs_enabled_= CS_ENABLED;
	}
	else
	{
		cs_enabled_ = CS_DISABLED;
	}
}

/**
  *
  */
void cs_change_timeout ( uint32_t timeout )
{
	cs_switching_timeout_ = timeout;
}


/**
  *
  */
void cs_change_switching_interval ( uint16_t interval)
{
	cs_switching_interval_ = interval;
	if ( CS_DISABLE_TIMEOUT == cs_switching_timeout_ )
	{
		cs_switching_timeout_ = rtc_get_ticks() + cs_switching_interval_; // not important since disabled
	}
}

/**
  *
  */
void cs_set_next_channel ( uint8_t ch )
{
	cs_next_channel_ = ch;
}


/**
  *
  */
void cs_set_alternative_channel ( uint8_t ch )
{
	cs_alternative_ch_ = ch;
}

#endif

/**
  *
  */
void cs_switching_timeout ()
{
	// currently simply switch and schedule next timeout
	uint8_t temp_ch = cs_current_channel_;
	phy_set_RF_channel(cs_next_channel_);
	cs_current_channel_ = cs_next_channel_;
	cs_next_channel_ = temp_ch;

	if (CS_MIN_CHANNEL_SWITCHING_INTERVAL > cs_switching_interval_ )
	{
		cs_enabled_ = CS_DISABLED;
	}
	else
	{
		cs_switching_timeout_ += cs_switching_interval_;
		if (cs_switching_timeout_ < rtc_get_ticks())
		{
			cs_switching_timeout_ = rtc_get_ticks() + cs_switching_interval_;
		}
	}
}

//**********************





/**
  * cs_send_join(dst_id, mode) - sends JOIN packet to agiven node (dst_id)
  *     additionally it stops the channel search and sets the particular channel
  */
void cs_send_join ( unsigned char dst_id, unsigned char mode )
{
	unsigned char temp_power = my_tx_power_;
	char temp_pkt[JOIN_PKT_SIZE];
	jpkt_t *pkt = ( jpkt_t* ) temp_pkt;
	// Turn off the channel search
	//network_search_mode_ = SEARCH_OFF;
	// Make the JOIN packet
	pkt->start = START_BYTE;
	pkt->flags = FLAG_JOIN;
	pkt->mac_dst = ( unsigned int ) dst_id;
	pkt->mac_src = MY_ADDR;
	pkt->length = JOIN_PKT_LENGTH; // of the packet'd data
	pkt->dst_id = dst_id; // part of DATA field (dest_id) - actual length depends on "length"
	pkt->src_id = MY_ADDR; // part of DATA field
	pkt->channel = cs_current_channel_; //  channel to be used as a starting point
	pkt->search_mode = mode; // search algorithm
	pkt->duration = rtc_get_rticks(); // ????  search algorithm
	pkt->crc = STOP_BYTE;

	// Send the JOIN packet using MAX power
	phy_set_power_level ( MAX_POWER_LEVEL );
	api_send_packet16 ( ( char* ) temp_pkt, JOIN_PKT_SIZE, ( unsigned int ) dst_id );
	phy_set_power_level ( temp_power );
}

#endif // (0)


#endif

