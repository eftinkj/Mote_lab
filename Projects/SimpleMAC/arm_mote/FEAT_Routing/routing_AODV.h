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
#ifndef _ROUTING_AODV_H_
#define _ROUTING_AODV_H_



//AODV Routing Header File

#define ROUTES_MAX_AODV 5

//AODV Global Variables
extern int16_t XDATA num_routes;
extern int8_t XDATA route_enable[ROUTES_MAX_AODV];
extern uint8_t XDATA route_table_destination[ROUTES_MAX_AODV];
extern uint16_t XDATA route_table_direction[ROUTES_MAX_AODV];
extern uint8_t XDATA route_table_cost[ROUTES_MAX_AODV];
extern uint16_t XDATA route_table_index[ROUTES_MAX_AODV];
extern uint16_t XDATA my_AODV_index;

extern uint8_t AODV_reply_sent;
extern uint16_t XDATA last_RREP_src;


//AODV Routing Functions
void send_RREQ ( uint16_t index, uint8_t dest_id, uint8_t src_id );
void send_RREP ( uint16_t index, uint8_t dest_id, uint8_t src_id );
void ACK_packet ( uint16_t index, uint8_t dest_id, uint8_t src_id, uint16_t dest );
int16_t AODV_find_direction ( uint8_t dest );
uint8_t route_table_update_AODV ( uint8_t dest, uint16_t index, uint16_t direction, uint8_t cost );
void start_routing_aodv ( uint8_t dest_id );
void routing_init_AODV(void );
void Routing_AODV ( int8_t *p, uint8_t rssi );
void send_AODV_counters(void);

void send_RERR ( uint16_t hop_id  );
void send_RERR2 ( uint8_t dest_id );

uint8_t AODV_send_DATA_base ( uint16_t base );


///////////////////////////////////////////////
///////////////////////////////////////////////
/**
  * aodv_dropped_link(??) - the link failed (after few retransmissions??) - update routing
  *    and optionally restart route discovery
  */
void aodv_dropped_link(uint16_t hop_id);



#endif
