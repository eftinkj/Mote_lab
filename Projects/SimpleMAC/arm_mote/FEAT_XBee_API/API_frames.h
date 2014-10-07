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
#ifndef API_FRAMES_H
#define API_FRAMES_H

#include "defs/esnl_pub.h"

#ifdef _ENABLE_XBEE_API_

//#include "common_constants.h"


// length of the API header (start byte + length field)
#define API_HEADER_SIZE 3

// Min and Max value of the Length  field
#define API_MIN_LENGTH 2
#define API_MAX_LENGTH 107

// Offset to the length field in API frame
#define API_LENGTH_FIELD_OFFSET 1


// Value of the Start Byte in the API frame
#define API_FRAME_START_BYTE 0x7E

/*
// API frame identificator values - indicate type of data
#define XBEE_FRAME_API_ID_VALUE_STATUS 0x8A
#define XBEE_FRAME_API_ID_VALUE_AT_CMD 0x08
#define XBEE_FRAME_API_ID_VALUE_AT_RSP 0x88
#define XBEE_FRAME_API_ID_VALUE_TX_64  0x00
#define XBEE_FRAME_API_ID_VALUE_TX_16  0x01
#define XBEE_FRAME_API_ID_VALUE_TX_STA 0x89
#define XBEE_FRAME_API_ID_VALUE_RX_64  0x80
#define XBEE_FRAME_API_ID_VALUE_RX_16  0x81
*/

// TX specificfields
#define API_FRAME_TX16_OPTIONS 0


// refrs to the mandatory part of the frame including:
//	Start delimiter, length and checksum
#define AT_FRAME_SIZE_OF_FIXED 4

// the frame ID counter - used to uniquelly identify API frames
// cannot be "0"
extern unsigned char XDATA frame_id_;
#define INC_FRAME() frame_id_++; if (0==frame_id_) {frame_id_++;}


struct api_frame
{
	unsigned char api_start;
	unsigned int api_length;
	char api_data; // with crc at the end
};
typedef struct api_frame api_frame_t;


struct api_frame_at_cmd
{
	unsigned char api_start;
	unsigned int api_length;
	unsigned char api_id;
	unsigned char api_at_frame_id;
	char api_at_cmd_1;
	char api_at_cmd_2;
	union
	{
		//	query
		struct
		{
			unsigned char api_crc; //checksum of API frame
		}
		query;
		//  modify one-byte
		struct
		{
			unsigned char value; //
			unsigned char api_crc; //checksum of API frame
		}
		one;
		//  modify two-byte
		struct
		{
			unsigned int value; //
			unsigned char api_crc; //checksum of API frame
		}
		two;
		//  modify 4-byte
		struct
		{
			unsigned long int value; //
			unsigned char api_crc; //checksum of API frame
		}
		four;
	} value;
};
typedef struct api_frame_at_cmd api_frame_at_cmd_t;
#define API_AT_CMD_MIN_LENGTH 4
//#define API_AT_CMD_HEAD_LENGTH 7




struct api_frame_at_rsp
{
	unsigned char api_start;
	unsigned int api_length;
	unsigned char api_id;
	unsigned char api_at_frame_id;
	char api_at_cmd_1;
	char api_at_cmd_2;
	unsigned char api_at_status;
	union {
		//	finish
		struct
		{
			unsigned char api_crc; //checksum of API frame
		}
		zero;
		//  modify one-byte
		struct
		{
			unsigned char value; //
			unsigned char api_crc; //checksum of API frame
		}
		one;
		//  modify two-byte
		struct
		{
			unsigned int value; //
			unsigned char api_crc; //checksum of API frame
		}
		two;
		//  modify 4-byte
		struct
		{
			unsigned long int value; //
			unsigned char api_crc; //checksum of API frame
		}
		four;
	} value;
};
typedef struct api_frame_at_rsp api_frame_at_rsp_t;

struct api_frame_at_rsp_one
{
	unsigned char api_start;
	unsigned int api_length;
	unsigned char api_id;
	unsigned char api_at_frame_id;
	char api_at_cmd_1;
	char api_at_cmd_2;
	unsigned char api_at_status;
	unsigned char value; //
	unsigned char api_crc; //checksum of API frame
};
typedef struct api_frame_at_rsp_one api_frame_at_rsp_one_t;

struct api_frame_at_rsp_two
{
	unsigned char api_start;
	unsigned int api_length;
	unsigned char api_id;
	unsigned char api_at_frame_id;
	char api_at_cmd_1;
	char api_at_cmd_2;
	unsigned char api_at_status;
	unsigned int value; //
	unsigned char api_crc; //checksum of API frame
};
typedef struct api_frame_at_rsp_two api_frame_at_rsp_two_t;

struct api_frame_at_rsp_four
{
	unsigned char api_start;
	unsigned int api_length;
	unsigned char api_id;
	unsigned char api_at_frame_id;
	char api_at_cmd_1;
	char api_at_cmd_2;
	unsigned char api_at_status;
	unsigned long value; //
	unsigned char api_crc; //checksum of API frame
};
typedef struct api_frame_at_rsp_four api_frame_at_rsp_four_t;


struct api_frame_tx16
{
	unsigned char api_start;
	unsigned int api_length;
	unsigned char api_id;
	unsigned char api_frame_id;
	unsigned int api_destination;
	unsigned char api_options;
	char api_data; // with crc at the end
};
typedef struct api_frame_tx16 api_frame_tx16_t;
#define API_TX16_MIN_LENGTH 5
#define API_TX16_HEAD_LENGTH 5



struct api_frame_tx_status
{
	unsigned char api_start;
	unsigned int api_length;
	unsigned char api_id;
	unsigned char api_frame_id;
	unsigned char api_tx_status; // status of the TX
	unsigned char crc; // crc at the end
};
typedef struct api_frame_tx_status api_frame_tx_status_t;
//#define API_TX16_MIN_LENGTH 5
//#define API_TX16_HEAD_LENGTH 5



struct api_frame_rx16
{
	unsigned char api_start;
	unsigned int api_length;
	unsigned char api_id;
	unsigned int api_source;
	unsigned char api_rssi;
	unsigned char api_options;
	char api_data; // with crc at the end
};
typedef struct api_frame_rx16 api_frame_rx16_t;
#define API_RX16_MIN_LENGTH 5
#define API_RX16_HEAD_LENGTH 5

/**
  * api_frame_calculate_sum( char* block, unsigned char len)
  * 	- calculates sum of the data
  * 	- returns sum
  */
unsigned char api_frame_calculate_sum ( unsigned char* block, unsigned char len );

/**
  * api_frame_calculate_sum( char* block) - calculates checksum of the data
  * 	returns sum
  */
unsigned char api_frame_calculate_sum_base ( unsigned int base, unsigned char len );

/**
  * api_frame_set_checksum( char* frame) - calculates checksum of the frame
  * 	and sets the appropriate field
  * NOTE: requires all other fields to be filled-in
  */
void api_frame_set_checksum ( char* frame );



/**
  * api_frame_set_checksum( char* frame) - calculates checksum of the frame
  * 	and sets the appropriate field
  * NOTE: requires all other fields to be filled-in
  */
uint8_t api_frame_check_checksum ( char* frame );




/**
  * api_send_at( cmd, *param, p_len) - sends frame with AT command
  * 	cmd - 2 character ID of the AT command
  * 	param - optional parameter (value: char/int/long) of the AT command
  * 	p_len - indicates length of parameter
  */
unsigned char api_send_at ( char cmd[2], char *param, unsigned char p_len );



/**
  * api_send_packet16( char *pkt, unsigned char length, unsigned int dst)
  * 	- sends frame with packet (pkt) of length (length) to a destination (dst)
  * 	cmd - 2 character ID of the AT command
  * 	puts the API frame together and copies it to the UART buffer
  * 	returns "0" is unsuccesful or the used (frame_if_)
  *
  */
unsigned char api_send_packet16 ( char *pkt, unsigned char len, unsigned int dst );


/**
  * api_send_packet16( char *pkt, unsigned char length, unsigned int dst)
  * 	- sends frame with packet (pkt) of length (length) to a destination (dst)
  * 	cmd - 2 character ID of the AT command
  * 	puts the API frame together and copies it to the UART buffer
  * 	returns "0" is unsuccesful or the used (frame_if_)
  *
  */
unsigned char api_send_packet16_base ( unsigned char idx, unsigned char len, unsigned int dst );


/**
  * api_send_packet16( char *pkt, unsigned char length, unsigned int dst)
  * 	- sends frame with packet (pkt) of length (length) to a destination (dst)
  * 	cmd - 2 character ID of the AT command
  * 	puts the API frame together and copies it to the UART buffer
  * 	returns "0" is unsuccesful or the used (frame_if_)
  *
  */
//unsigned char api_recv_packet16( char *pkt, unsigned char *len, unsigned int dst);


#endif // _ENABLE_XBEE_API_

#endif // API_FRAMES_H


















