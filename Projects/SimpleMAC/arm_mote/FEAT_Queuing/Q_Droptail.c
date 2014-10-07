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
#ifndef _Q_DROPTAIL_C_
#define _Q_DROPTAIL_C_


//Queuing Code for ADFS

#include "common.h"
#include "HW_LIB/RTC.h"
#include "Queuing.h"
//#include "Q_Droptail.h"


//**********************
//Define Variables


uint8_t XDATA Q_table[QSIZE];
uint8_t XDATA Q_head;
uint8_t XDATA Q_tail;



//Define Q-Buffers
//char xdata buffer0[SIZE_Q_BUFF * NUMQBUFFS] _at_ Data_Begin0;
//char buffer0[SIZE_Q_BUFF * NUMQBUFFS];

//**********************

//Function Section

//Function to Init the Buffer Pointer Table
void que_Droptail_init( void )
{
	uint8_t k1;
	Q_tail = 0;
	Q_head = 0;
	for ( k1 = 0; k1 < QSIZE; k1++ )
	{
		Q_table[k1] = 0xFF;
	}
}
//**********************



//**********************

//Function to check Q utilization
int que_Droptail_utilization( void )
{
	int16_t ret;
	ret = ( int ) Q_head - Q_tail;
	if ( ret < 0 )
	{
		ret = QSIZE + ret;
	}
	return ret;
}
//**********************

//Function to enQ a packet from index number
uint8_t que_Droptail_enqueue ( uint8_t index )
{
	if ( que_Droptail_utilization() < QSIZE - 1 )
	{
		Q_table[Q_head] = index;
		Q_head++;
		if ( QSIZE <= Q_head )
		{
			Q_head = 0;
		}
		return 1;
	}
	else
		return 0;
}
//**********************

//Function to deQ a packet that has been sent
unsigned char que_Droptail_dequeue()
{
	uint8_t index;
	if ( 0 < que_Droptail_utilization() )
	{
		index = Q_table[Q_tail];
		Q_tail++;
		if ( QSIZE <= Q_tail )
			Q_tail = 0;
		return index;
	}
	else
		return NO_PACKET_INDEX;
}
//**********************



#endif
