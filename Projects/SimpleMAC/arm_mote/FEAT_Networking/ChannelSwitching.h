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
#ifndef _CHANNEL_SWITCHING_H_
#define _CHANNEL_SWITCHING_H_

//#include "feat_enable.h"
#include "defs/esnl_pub.h"
#include "packet.h"

#ifdef FEAT_ENABLE_CH_SWITCHING

//**********************
// ##############################################################################
// ## Misc constants
// ##############################################################################
#define DEFAULT_ONE_SECOND TICKERS_PER_SECOND
#define CS_DISABLED	0x0
#define CS_ENABLED	0x1


#define CS_REQ_REPEAT_TIMEOUT	100

// ##############################################################################
// ## Misc variables
// ##############################################################################
extern uint8_t cs_enabled_;//xdata cs_enabled_;

extern uint32_t cs_switching_timeout_;//xdata cs_switching_timeout_;
extern uint16_t cs_switching_interval_;//xdata cs_switching_interval_;
extern uint8_t cs_next_channel_;//xdata cs_next_channel_;
extern uint8_t cs_alternative_ch_;//xdata cs_alternative_ch_;

extern char str_CS_SWITCH[];
extern char str_CS_SWITCH_END[];
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
void cs_power_up ( );


/**
 * void cs_init ( ) - initialize Channel Switching Module
 */
void cs_init ( );


/**
 * void cs_end_switch ( uint8_t *context ) - finishes the Channel switching
 *			(execute switch to the target channel)
 */
void cs_end_switch ( uint8_t *context );

/**
 * cs_send_req_Switch ( uint8_t *context ) - sends a channel switch request (BCAST)
 */
void cs_send_req_Switch ( uint8_t *context );

#undef CS_ENABLE_DATA_ACCESS_FUNCT
#ifdef CS_ENABLE_DATA_ACCESS_FUNCT
/**
  *
  */
void cs_activate( bit activate);


/**
  *
  */
void cs_change_timeout ( uint32_t timeout );

/**
  *
  */
void cs_change_switching_interval ( uint16_t interval );

#endif

/**
  *
  */
void cs_switching_timeout ();

/**
  * cs_recv_switching_request(pkt) - handle channel switching request
  *	
  */
void cs_recv_switching_request( char *module );


/**
  * cs_send_join(dst_id, mode) - sends JOIN packet to agiven node (dst_id)
  *     additionally it stops the channel search and sets the particular channel
  */
void cs_send_join ( unsigned char dst_id, unsigned char mode );

#endif


#endif

