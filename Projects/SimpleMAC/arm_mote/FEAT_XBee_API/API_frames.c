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

#include "API_frames.h"

#ifdef _ENABLE_XBEE_API_

#include <stdio.h>

#include "common.h"
#include "packet.h"
//#include "adc_0.h"
//#include "AT_scripts.h"
#include "HW_LIB/uart_0.h"

#include "FEAT_Queueing/Queuing.h"



unsigned char XDATA frame_id_ = 1;

// temporary buffer for creation of the API frame or frame header
//char frame[20];


/**
  * api_frame_calculate_sum( char* block) - calculates checksum of the data
  * 	returns sum
  */
unsigned char api_frame_calculate_sum ( unsigned char* block, unsigned char len )
{
	unsigned char i, sum = 0;
	for ( i = 0; i < len; i++ )
	{
		sum += block[i];
	}
	return sum;
}

/**
  * api_frame_calculate_sum( char* block) - calculates checksum of the data
  * 	returns sum
  */
unsigned char api_frame_calculate_sum_base ( unsigned int base, unsigned char len )
{
	unsigned char i, sum = 0;
	for ( i = 0; i < len; i++ )
	{
		sum += QBUFF_ACCESS ( base, i );
	}
	return sum;
}



/**
  * api_frame_set_checksum( char* frame) - calculates checksum of the frame
  * 	and sets the appropriate field
  * NOTE: requires all other fields to be filled-in
  */
void api_frame_set_checksum ( char* frame )
{
	int i;
	unsigned char checksum = 0;
	api_frame_t *af = ( api_frame_t * ) frame;
	uint8_t *body = (uint8_t *) & ( af->api_data );

	for ( i = 0; i < SWAP16(af->api_length); i++ )
	{
		checksum += body[i];
	}
	checksum = 0xFF - checksum;
	body[SWAP16(af->api_length)] = checksum;
	return;
}




/**
  * api_frame_set_checksum( char* frame) - calculates checksum of the frame
  * 	and sets the appropriate field
  * NOTE: requires all other fields to be filled-in
  */
uint8_t api_frame_check_checksum ( char* frame )
{
	int i;
	unsigned char checksum = 0;
	api_frame_t *af = ( api_frame_t * ) frame;
	uint8_t *body = (uint8_t *)& ( af->api_data );

	for ( i = 0; i <= SWAP16(af->api_length); i++ )
	{
		checksum += body[i];
	}

	if ( 0 == checksum )
	{
		return 1; // frame correct
	}
	return 0; // frame incorrect
}



/**
  * api_send_at( cmd, *param, p_len, wait, *result, *r_len) - sends frame with AT command
  * 	cmd - 2 character ID of the AT command
  * 	param - optional parameter (value: char/int/long) of the AT command
  * 	p_len - indicates length of parameter
  * 	wait - indicates if the procedure should: "0" -exit immediatly
  *			"1" - wait for the response, "2" - use delayed-response feature (NOT implemented yet!!)
  *		result - the buffer for storying result
  *		r_len -
  * NOTE: requires all other fields to be filled-in
  */
unsigned char api_send_at ( char cmd[2], char *param, unsigned char p_len )
{

	// =====================================================
	api_frame_at_cmd_t *af = ( api_frame_at_cmd_t * ) short_packet;

	af->api_start = API_FRAME_START_BYTE;
// 	af->api_length = API_ID_AT_CMD_MIN_LENGTH; // set later
	af->api_id = XBEE_FRAME_API_ID_VALUE_AT_CMD;
	af->api_at_frame_id = frame_id_;
	INC_FRAME();

	af->api_at_cmd_1 = cmd[0];
	af->api_at_cmd_2 = cmd[1];

	if ( 0 == p_len )
	{
		af->api_length = SWAP16((uint16_t)API_AT_CMD_MIN_LENGTH);
		// nothing to add
	}
	else if ( 1 == p_len )
	{
		af->api_length = SWAP16((uint16_t)API_AT_CMD_MIN_LENGTH + 1);
		af->value.one.value = * ( ( unsigned char* ) param );
	}
	else if ( 2 == p_len )
	{
		af->api_length = SWAP16((uint16_t)API_AT_CMD_MIN_LENGTH + 2);
		af->value.two.value = SWAP16(* ( ( unsigned int* ) param ));
	}
	else if ( 4 == p_len )
	{
		af->api_length = SWAP16((uint16_t)API_AT_CMD_MIN_LENGTH + 4);
		af->value.four.value = SWAP32(* ( ( unsigned long int* ) param ));
	}
	else
	{
		// wrong length of the parameter
		return 0;
	}

	api_frame_set_checksum ( short_packet );

	// =====================================================
	// Send frame
#ifdef _ENABLE_OLD_UART_
	com_block ( 0, short_packet, ( int ) SWAP16(af->api_length) + AT_FRAME_SIZE_OF_FIXED );
#endif // _ENABLE_OLD_UART_

	return 1;
}



/**
  * api_send_packet16( char *pkt, unsigned char length, unsigned int dst)
  * 	- sends frame with packet (pkt) of length (length) to a destination (dst)
  * 	cmd - 2 character ID of the AT command
  * 	puts the API frame together and copies it to the UART buffer
  * 	returns "0" is unsuccesful or the used (frame_if_)
  *
  */
unsigned char api_send_packet16 ( char *pkt, unsigned char len, unsigned int dst )
{
	unsigned char sum = 0;
	api_frame_tx16_t XDATA *tf = ( api_frame_tx16_t XDATA * ) short_packet;

	tf->api_start = API_FRAME_START_BYTE;
	tf->api_length = SWAP16(API_TX16_MIN_LENGTH + len);

	tf->api_id = XBEE_FRAME_API_ID_VALUE_TX_16;
	tf->api_frame_id = frame_id_;
	INC_FRAME();

	tf->api_destination = SWAP16(dst);
	tf->api_options = API_FRAME_TX16_OPTIONS;

	sum += api_frame_calculate_sum ( (uint8_t *)pkt, len );
	sum += api_frame_calculate_sum ( & ( tf->api_id ), API_TX16_HEAD_LENGTH );
//	sum += api_frame_calculate_sum( pkt, len);
	sum = 0xFF - sum;

#ifdef _ENABLE_OLD_UART_
	// Send frame
	com_block_x ( 0, (char *)short_packet, 8 );
	com_block ( 0, (char *)pkt, len );
	com_block ( 0, (char *)&sum, 1 );
#endif // _ENABLE_OLD_UART_
	return tf->api_frame_id;
}



/**
  * api_send_packet16( char *pkt, unsigned char length, unsigned int dst)
  * 	- sends frame with packet (pkt) of length (length) to a destination (dst)
  * 	cmd - 2 character ID of the AT command
  * 	puts the API frame together and copies it to the UART buffer
  * 	returns "0" is unsuccesful or the used (frame_if_)
  *
  */
unsigned char api_send_packet16_base ( unsigned char idx, unsigned char len, unsigned int dst )
{
	unsigned char sum = 0;
	api_frame_tx16_t * XDATA tf = ( api_frame_tx16_t* ) ( short_packet );

	tf->api_start = API_FRAME_START_BYTE;
	tf->api_length = SWAP16((uint16_t)API_TX16_MIN_LENGTH + len);

	tf->api_id = XBEE_FRAME_API_ID_VALUE_TX_16;
	tf->api_frame_id = frame_id_;
	INC_FRAME();

	tf->api_destination = SWAP16(dst);
	tf->api_options = API_FRAME_TX16_OPTIONS;

	sum += api_frame_calculate_sum_base ( ( QBUFF_BASE ( idx ) ), len );
	sum += api_frame_calculate_sum ( & ( tf->api_id ), API_TX16_HEAD_LENGTH );
	sum = 0xFF - sum;

#ifdef _ENABLE_OLD_UART_
	// Send frame
	com_block ( 0, short_packet, 8 );
	com_block_base ( 0, idx, len );
	com_block ( 0, (char *)&sum, 1 );
#endif // _ENABLE_OLD_UART_
	return tf->api_frame_id;
}


#endif // _ENABLE_XBEE_API_

