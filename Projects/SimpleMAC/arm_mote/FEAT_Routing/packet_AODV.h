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
//AODV Packet Definitions
///////////////////////////////////////////////
//Includes
#include "common.h"
//#include "definitions.h"
///////////////////////////////////////////////

///////////////////////////////////////////////
//RREQ Packet Structure
#define RREQ_LEN 11
struct RREQ_packet_structure
{
	unsigned char start;
	unsigned char flags;
	unsigned int mac_dst; //Local Hop
	unsigned int mac_src; //Local Hop
	unsigned int index;
	unsigned char dst_id; // Original Destination
	unsigned char src_id; // Original Source
	unsigned char crc; // part of DATA field
};
typedef struct RREQ_packet_structure RREQpkt;
///////////////////////////////////////////////

///////////////////////////////////////////////
//RREP Packet Structure
#define RREP_LEN 11
struct RREP_packet_structure
{
	unsigned char start;
	unsigned char flags;
	unsigned int mac_dst;
	unsigned int mac_src;
	unsigned int index;
	unsigned char dst_id;
	unsigned char src_id;
	unsigned char crc; // part of DATA field
};
typedef struct RREP_packet_structure RREPpkt;
///////////////////////////////////////////////

///////////////////////////////////////////////
//RERR Packet Structure
#define RERR_LEN 9
struct RERR_packet_structure
{
	unsigned char start;
	unsigned char flags;
	unsigned int mac_dst;
	unsigned int mac_src;
	//unsigned int index;
	unsigned char dst_id;
	unsigned char src_id;
	unsigned char crc; // part of DATA field
};
typedef struct RERR_packet_structure RERRpkt;
///////////////////////////////////////////////

///////////////////////////////////////////////
//ACK Packet Structure
#define ACK_LEN 11
struct ACK_packet_structure
{
	unsigned char start;
	unsigned char flags;
	unsigned int mac_dst;
	unsigned int mac_src;
	unsigned int index;
	unsigned char dst_id;
	unsigned char src_id;
	unsigned char crc; // part of DATA field
};
typedef struct ACK_packet_structure ACKpkt;
///////////////////////////////////////////////

///////////////////////////////////////////////
//Counter Report Packet Structure
#define COUNT_LEN_AODV 11
struct counter_packet_structure
{
	unsigned char start;
	unsigned char flags;
	unsigned int mac_dst;
	unsigned int mac_src;
	unsigned char RREQcnt;
	unsigned char RREPcnt;
	unsigned char ACKcnt;
	unsigned char DATcnt;
	unsigned char crc; // part of DATA field
};
typedef struct counter_packet_structure CNTpkt;
///////////////////////////////////////////////

///////////////////////////////////////////////
/*
struct packet_structure {
	unsigned char start;
	unsigned char flags;
	unsigned int mac_dst;
	unsigned int mac_src;
	unsigned char length; // of the packet'd data
	unsigned char message; // part of DATA field (dest_id) - actual length depends on "length"
	unsigned char src_id; // part of DATA field
};
typedef struct packet_structure pkt_t;
*/
///////////////////////////////////////////////
