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
#include <stdio.h>

#include "defs/esnl_pub.h"

#include "packet.h"

#include "FEAT_Queuing/Queuing.h"


// Sequence number used for data packets (initially for TEST MODE only)
uint8_t sequence_no_=0;

/**
  * get_dst_mac_base(base) - returns the MAC address of destination
  * 	for a given packet ('base' offset in the queue buffer)
  * returns the MAC addrss of dest node
  */
uint16_t get_dst_mac_base ( uint16_t base )
{
	// SDCC:
	pkt_t * XDATA pkt = ( pkt_t* ) ( & ( buffer0[base] ) ); //&(QBUFF_ACCESS(base,0));
	return ( ENDIAN16(pkt->mac_dst) );

}



