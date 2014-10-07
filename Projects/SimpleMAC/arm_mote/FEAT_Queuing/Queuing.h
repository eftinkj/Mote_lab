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
#ifndef _QUEUING_H_
#define _QUEUING_H_

//Constants File for Queuing
#include "defs/esnl_pub.h"

#ifdef _F310_
	#define NUMQBUFFS 1
#else // _F310_
	#ifdef _F340_
		#define NUMQBUFFS 2
	#else // _F340_
		#define NUMQBUFFS 8
	#endif // else _F340_
#endif // else _F310_

#define QSIZE NUMQBUFFS+1
#define SIZE_Q_BUFF 100
//#define numbuffstats 3 // full/head/tail/age

#ifdef _ENABLE_QUEUING_


extern uint8_t	que_buffer_empty_slots_end_;
extern uint8_t	XDATA buffer_empty_slots[NUMQBUFFS];
extern int8_t	XDATA buffer_wp_table[NUMQBUFFS];
extern uint32_t	XDATA buffer_age_table[NUMQBUFFS];


//**********************

//Define where the Q-Buffers reside
//#define DATA_BEGIN	0x0100
/*
#define Data_Begin0 0x0000
#define Data_Begin1 0x0064
#define Data_Begin2 0x00C8
#define Data_Begin3 0x012C
#define Data_Begin4 0x0190
#define Data_Begin5 0x01F4
#define Data_Begin6 0x0258
#define Data_Begin7 0x02BC
#define Data_Begin8 0x0320
#define Data_Begin9 0x0384
*/

//**********************

//Define Q-Buffers
extern int8_t XDATA buffer0[SIZE_Q_BUFF * NUMQBUFFS];// _at_ Data_Begin0;
//extern char buffer0[SIZE_Q_BUFF * NUMQBUFFS];

//**********************

#ifdef __KEIL__
//#define PKT_INDEX_BEGIN		(DATA_BEGIN+(SIZE_Q_BUFF * NUMQBUFFS))
#define PKT_INDEX_BEGIN		(buffer0+(SIZE_Q_BUFF * NUMQBUFFS))
#endif

#ifdef __SDCC__
#define PKT_INDEX_BEGIN		(buffer0+(SIZE_Q_BUFF * NUMQBUFFS))
#endif



#include "common.h"
#include "Q_Droptail.h"
#ifdef FEAT_ENABLE_SFQ
	#include "Q_SFQ.h"
#endif // FEAT_ENABLE_SFQ

// the way to access the buffer directly
#define QBUFF_ACCESS(base, offset) buffer0[base + offset]
//#define QBUFF_BASE(idx) (buffer0 + idx*SIZE_Q_BUFF)
#define QBUFF_BASE(idx) ( idx*SIZE_Q_BUFF)

// Packet indexes (nopacket/empty, ??)
#define NO_PACKET_INDEX 0xFF

// Queueing schemes
#define QUEUE_DROPTAIL 0
#define QUEUE_SFQ 1

extern uint8_t my_queue_; // Type of queuing scheme

//Function Prototypes

extern void que_init(void);

extern uint8_t que_get_empty_buffer_location(void);

extern void que_make_empty_buffer ( uint8_t index );

//extern char* que_get_packet_pointer(unsigned char index);

// Switch to a particular queuing scheme
void que_switch_protocol ( uint8_t new_protocol );



uint8_t que_enQpacket ( uint8_t index );


uint8_t que_deQpackets(void);


#endif // _ENABLE_QUEUING_
//**********************

#endif

