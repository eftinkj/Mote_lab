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

//Constants File for Queuing
#include "defs/esnl_pub.h"

#include "Queuing.h"

//#include "common.h"
#include "HW_LIB/RTC.h"

#ifdef _ENABLE_QUEUING_
//**********************
//Define Variables

uint8_t que_buffer_empty_slots_end_;
uint8_t XDATA buffer_empty_slots[NUMQBUFFS];
int8_t XDATA buffer_wp_table[NUMQBUFFS];
uint32_t XDATA buffer_age_table[NUMQBUFFS];


//Define Q-Buffers
#ifdef __KEIL__
//char xdata buffer0[SIZE_Q_BUFF * NUMQBUFFS] _at_ DATA_BEGIN;
int8_t XDATA buffer0[SIZE_Q_BUFF * NUMQBUFFS];
#endif

#ifdef __SDCC__
int8_t XDATA buffer0[SIZE_Q_BUFF * NUMQBUFFS];
#endif

//char buffer0[SIZE_Q_BUFF * NUMQBUFFS];


//**********************

//**********************

// ##############################################################################
// ##
// ##############################################################################

uint8_t my_queue_ = QUEUE_DROPTAIL; // Type of queuing scheme



// ##############################################################################
// ## Function Section
// ##############################################################################

//Function to Init the Buffer Pointer Table
void que_init()
{
	uint8_t k1;
	for ( k1 = 0; k1 < NUMQBUFFS; k1++ )
	{
		buffer_empty_slots[k1] = k1;
		buffer_wp_table[k1] = 0;
		buffer_age_table[k1] = 0;
	}
	que_buffer_empty_slots_end_ = NUMQBUFFS;

	switch ( my_queue_ )
	{
		case QUEUE_DROPTAIL:
			que_Droptail_init();
			break;
#ifdef FEAT_ENABLE_SFQ
		case QUEUE_SFQ:
			Q_SFQ_init();
			break;
#endif // FEAT_ENABLE_SFQ
		default:
			// ERROR
			my_queue_ = QUEUE_DROPTAIL;
			que_Droptail_init();
			break;
	}
}
//**********************

//Function to give the next empty buffer slot
uint8_t que_get_empty_buffer_location()
{
	uint8_t ret;
	if ( que_buffer_empty_slots_end_ == 0 )
	{
		return 0xFF;
	}
	que_buffer_empty_slots_end_--;
	ret = buffer_empty_slots[que_buffer_empty_slots_end_];
	buffer_wp_table[ret] = 1;
	buffer_age_table[ret] = rtc_get_ticks();
	return ret;
}
//**********************

uint16_t wrong_make_empty = 0;
//Function to Mark Space as empty buffer
void que_make_empty_buffer ( uint8_t index )
{
	if ( index >= NUMQBUFFS )
	{
		wrong_make_empty++;
	}
	else
	{
		buffer_empty_slots[que_buffer_empty_slots_end_] = index;
		que_buffer_empty_slots_end_++;
	}
}
//**********************


// ##############################################################################
// ##
// ##############################################################################

void que_switch_protocol ( uint8_t new_protocol )
{
	my_queue_ = new_protocol;
	que_init();
}


uint8_t que_enQpacket ( uint8_t index )
{
	switch ( my_queue_ )
	{
		case QUEUE_DROPTAIL:
			return que_Droptail_enqueue ( index );
			break;
#ifdef FEAT_ENABLE_SFQ
		case QUEUE_SFQ:
			return Q_SFQ_enqueue ( index );
			break;
#endif // FEAT_ENABLE_SFQ
		default:
			// ERROR
			break;
	}
	return 0; // FAILED!!
}


uint8_t que_deQpackets()
{
	switch ( my_queue_ )
	{
		case QUEUE_DROPTAIL:
			return que_Droptail_dequeue();
			break;
#ifdef FEAT_ENABLE_SFQ
		case QUEUE_SFQ:
			return Q_SFQ_dequeue();
			break;
#endif // FEAT_ENABLE_SFQ
		default:
			// ERROR
			break;
	}
	return NO_PACKET_INDEX;
}



#endif // _ENABLE_QUEUING_
