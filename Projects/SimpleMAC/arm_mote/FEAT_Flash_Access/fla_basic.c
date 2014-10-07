#include "defs/esnl_pub.h"

#ifdef _ENABLE_FLA_BASIC_

#include "fla_basic_prv.h"
#include "fla_basic_pub.h"


#include "sys_func.h"

#include "packet.h"
#include "common.h"


#include "FEAT_Networking/tsp_common_pub.h"
#include "FEAT_Scheduler/sch_basic_pub.h"

//#include "FEAT_Flash_Access/F12x_F13x/F120_FlashUtils.h"
#include "F12x_F13x/F120_FlashUtils.h"

//#define _FLA_DEBUG_ENABLE_
#undef _FLA_DEBUG_ENABLE_
#define _FLA_DEBUG_LEVEL_	0

//#define FLA_COMM_TEST
#undef FLA_COMM_TEST

//#define FLA_FAKE_MEM_ACCESS
#undef FLA_FAKE_MEM_ACCESS

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

#define FLA_FUNC_OFF	0
#define FLA_FUNC_ON		1


fla_mem_t xdata* fla_mem_p;
uint8_t	xdata fla_mem_raw[FLA_MEM_SIZE];


/**
  * fla_init () - sets up (tasks/system) Flash Access module
  */
void fla_init( )
{
	uint8_t i;
#ifdef _ENABLE_SCH_BASIC_
	sch_add_loop(( sch_loop_func_t )fla_loop );
#endif // _ENABLE_SCH_BASIC_

	for(i=0; i < FLA_MEM_SIZE; i++)
	{
		fla_mem_raw[i] = 0;
	}
	fla_mem_p = (fla_mem_t xdata* )fla_mem_raw;
	fla_mem_p->fla_img_seq_ = 255; // sequence invalid (alows to start new img load)
	fla_mem_p->fla_status_ = FLA_STATE_TEMP_CORRUPTED; // Initially the flash is considered "dirty"
	fla_mem_p->fla_delayed_hex_ack = 0;
	fla_mem_p ->buff = &(fla_mem_p->dynamic);

//	fla_test();
}


uint8_t xdata test_1[] = ":10010000214601360121470136007EFE09D2190140";
uint8_t xdata test_2[] = ":100110002146017EB7C20001FF5F16002148011988";
uint8_t xdata test_3[] = ":10012000194E79234623965778239EDA3F01B2CAA7";
uint8_t xdata test_4[] = ":100130003F0156702B5E712B722B732146013421C7";
uint8_t xdata test_5[] = ":00000001FF";

/**
* fla_test() - tests Flash Access operation (e.g. RTC correctness)
*/
uint8_t fla_test()
{
	uint8_t result=0xff;
	EA = 0;

	fla_erase_temp();

//	FLASH_PageErase(FLA_TEMP_MEM_OFFSET,false);
//	FLASH_PageErase(FLA_TEMP_MEM_OFFSET+FLASH_PAGESIZE,false);
//	FLASH_PageErase(FLA_TEMP_MEM_OFFSET+FLASH_PAGESIZE+FLASH_PAGESIZE,false);
//	FLASH_PageErase(FLA_TEMP_MEM_OFFSET+FLASH_PAGESIZE+FLASH_PAGESIZE+FLASH_PAGESIZE,false);

//	FLASH_Clear(( FLADDR ) FLA_TEMP_MEM_OFFSET, 0x0FFF, false );
//	FLASH_Copy(( FLADDR ) FLA_TEMP_MEM_OFFSET, false, ( FLADDR ) 0x0000, false, 0x0FFF );

//	FLASH_Clear(( FLADDR ) FLA_TEMP_MEM_OFFSET, 0x0FFF, false );

#ifdef _ENABLE_FLA_STRING_INTEL_HEX_
	result = fla_recv_Intel_hex(test_1);
	result = fla_recv_Intel_hex(test_2);
	result = fla_recv_Intel_hex(test_3);
	result = fla_recv_Intel_hex(test_4);
	result = fla_recv_Intel_hex(test_5);
#endif // _ENABLE_FLA_STRING_INTEL_HEX_

	EA = 1;

	return 0;
}



/**
  * fla_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void fla_loop( void )
{
	//
	if (1 == fla_mem_p->fla_delayed_hex_ack)
	{
		fla_send_msg( FLA_MSG_TYPE_HEX_ACK, fla_mem_p->fla_img_seq_ );
		fla_mem_p->fla_delayed_hex_ack = 0;
	}
}



/**
  * fla_recv_cmd(module, rssi) - handles a command (packet's module)
  */
void fla_recv_cmd(pkt_mod_header_t xdata * module, uint8_t rssi)
{
	//PKT_MODULE_TYPE_FLA_BASIC
	fla_cmd_packet_v1_t xdata *cmd = (fla_cmd_packet_v1_t xdata *)module;

	uint8_t result;

	switch (cmd->message_type)
	{
		case FLA_MSG_TYPE_BINARY_INTEL_HEX:
//			fla_recv_binary_Intel_hex();
			if (fla_mem_p->fla_status_ == FLA_STATE_TEMP_RECEIVING)
			{
				fla_ihex_t xdata *ihex = (fla_ihex_t xdata *)&(cmd->message);
				if (fla_mem_p->fla_img_seq_ == ihex->sequence)
				{
#ifndef FLA_COMM_TEST
					result = fla_recv_binary_Intel_hex(&(ihex->hex_record));
#else
					//fla_mem_p->fla_img_seq_++;
					result = FLA_RECORD_CONTINUE;
#endif // else/not FLA_COMM_TEST
					if ( FLA_MEM_CHECKSUM_FAILED == result)
					{
						fla_mem_p->fla_status_ = FLA_STATE_TEMP_CORRUPTED;
						fla_send_msg(FLA_MSG_TYPE_TEMP_FLASH_CORRUPTED, fla_mem_p->fla_img_seq_);
						fla_send_msg(FLA_MSG_TYPE_ACK, fla_mem_p->fla_status_);
					} 
					else if (FLA_RECORD_END_OF_FILE == result)
					{
						fla_mem_p->fla_status_ = FLA_STATE_TEMP_READY;
						fla_mem_p->fla_delayed_hex_ack = 0; // Finished no need for more HEX ACKs
						fla_send_msg(FLA_MSG_TYPE_ACK, fla_mem_p->fla_status_);
					}
					else if (FLA_RECORD_CONTINUE == result)
					{
						fla_mem_p->fla_img_seq_++;
						fla_mem_p->fla_delayed_hex_ack = 1;
					}
					else if ( FLA_CODE_ADDRESS_OUT_OF_RANGE == result)
					{
						fla_mem_p->fla_status_ = FLA_STATE_TEMP_CORRUPTED;
						fla_send_msg(FLA_MSG_TYPE_TEMP_FLASH_FAILURE, result);
						fla_send_msg(FLA_MSG_TYPE_ACK, fla_mem_p->fla_status_);						
					}
				}
				else
				{
					// out_of_sync
					fla_send_msg(FLA_MSG_TYPE_RESYNC_SEQ, fla_mem_p->fla_img_seq_);
				}
			}
			else
			{
				fla_send_msg(FLA_MSG_TYPE_BAD_STATUS, fla_mem_p->fla_status_);
			}
			break;
#ifdef _ENABLE_FLA_STRING_INTEL_HEX_
		case FLA_MSG_TYPE_INTEL_HEX:
			if (fla_mem_p->fla_status_ == FLA_STATE_TEMP_RECEIVING)
			{
				fla_ihex_t *ihex = (fla_ihex_t *)&(cmd->message);
				if (fla_mem_p->fla_img_seq_ == ihex->sequence)
				{
#ifndef FLA_COMM_TEST
	#ifdef _ENABLE_FLA_STRING_INTEL_HEX_
					result = fla_recv_Intel_hex(&(ihex->hex_record));
	#endif // _ENABLE_FLA_STRING_INTEL_HEX_
#else
					//fla_mem_p->fla_img_seq_++;
					/*
					if (fla_mem_p->fla_img_seq_ == 4)
					{
						fla_mem_p->fla_img_seq_ -= 2;
					}
					*/
					//fla_mem_p->fla_img_seq_ = 250;
					//result = FLA_RECORD_END_OF_FILE;
					result = FLA_RECORD_CONTINUE;
#endif // else/not FLA_COMM_TEST
					if ( FLA_MEM_CHECKSUM_FAILED == result)
					{
						fla_mem_p->fla_status_ = FLA_STATE_TEMP_CORRUPTED;
						fla_send_msg(FLA_MSG_TYPE_TEMP_FLASH_CORRUPTED, fla_mem_p->fla_img_seq_);
						fla_send_msg(FLA_MSG_TYPE_ACK, fla_mem_p->fla_status_);
					} 
					else if (FLA_RECORD_END_OF_FILE == result)
					{
						fla_mem_p->fla_status_ = FLA_STATE_TEMP_READY;
						fla_send_msg(FLA_MSG_TYPE_ACK, fla_mem_p->fla_status_);
					}
					else if (FLA_RECORD_CONTINUE == result)
					{
						fla_mem_p->fla_img_seq_++;
						fla_mem_p->fla_delayed_hex_ack = 1;
					}
					else if ( FLA_CODE_ADDRESS_OUT_OF_RANGE == result)
					{
						fla_mem_p->fla_status_ = FLA_STATE_TEMP_CORRUPTED;
						fla_send_msg(FLA_MSG_TYPE_TEMP_FLASH_FAILURE, result);
						fla_send_msg(FLA_MSG_TYPE_ACK, fla_mem_p->fla_status_);						
					}
				}
				else
				{
					// out_of_sync
					fla_send_msg(FLA_MSG_TYPE_RESYNC_SEQ, fla_mem_p->fla_img_seq_);
				}
			}
			else
			{
				fla_send_msg(FLA_MSG_TYPE_BAD_STATUS, fla_mem_p->fla_status_);
			}
			break;
#endif // _ENABLE_FLA_STRING_INTEL_HEX_
		case FLA_MSG_TYPE_ERASE_TEMP_FLASH:
#ifndef FLA_COMM_TEST
			fla_erase_temp();
#endif //FLA_COMM_TEST
			fla_mem_p->fla_status_ = FLA_STATE_TEMP_EMPTY;
			fla_send_msg(FLA_MSG_TYPE_ACK, FLA_STATE_TEMP_EMPTY);
			break;
		case FLA_MSG_TYPE_START_FLASH_TEMP:
			if (fla_mem_p->fla_status_ == FLA_STATE_TEMP_EMPTY)
			{
				fla_mem_p->fla_status_ = FLA_STATE_TEMP_RECEIVING;
				fla_mem_p->fla_img_seq_ = 0; // reset sequence 
				fla_send_msg(FLA_MSG_TYPE_HEX_ACK, fla_mem_p->fla_img_seq_);
			}
			else
			{
				fla_send_msg(FLA_MSG_TYPE_BAD_STATUS, fla_mem_p->fla_status_);
			}
			break;
		case FLA_MSG_TYPE_START_CODE_SWAP:
			// actual flashing of the main (common) bank with the loaded code
			//fla_copy_temp_to_main(1);
			fla_copy_temp_to_main(FLA_COPY_ALL_PAGES);
			// TODO:
			break;
		case FLA_MSG_TYPE_GET_STATUS:
			fla_send_msg(FLA_MSG_TYPE_ACK, fla_mem_p->fla_status_);
			break;
	}
}

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/

#ifdef _ENABLE_FLA_STRING_INTEL_HEX_
/**
* fla_recv_Intel_hex() - reads the received code piece (in Intel HEX) 
*	and writes it into temp flash memory
*/
uint8_t fla_recv_Intel_hex( uint8_t xdata *in )
{
	uint8_t i = 0;
	uint8_t j ;
	uint8_t len = 0;
	uint16_t offset = 0;
	uint8_t	record_type;
	uint8_t checksum;
	uint8_t flash_checksum;
	char code * pread;
	uint8_t hb=0;
	uint8_t lb=0;
	uint8_t tb=0;

	while ( in[i++] != ':' );

	// get the length
	hb = toint(in[i]); i++; lb = toint(in[i]); i++;
	tb = hb*16 + lb;
	len = tb;
//	len = HEX2CHAR(in[i++],in[i++]);

	// get the offset
	hb = toint(in[i]); i++; lb = toint(in[i]); i++;
	tb = toint(in[i-2])*16 + toint(in[i-1]);
	offset = hb*16 + lb;
//	offset = HEX2CHAR(in[i++], in[i++]);
	offset <<= 8;
	hb = toint(in[i]); i++; lb = toint(in[i]); i++;
	tb = toint(in[i-2])*16 + toint(in[i-1]);
	offset |= hb*16 + lb;


	// get the record type
	hb = toint(in[i]); i++; lb = toint(in[i]); i++;
	tb = toint(in[i-2])*16 + toint(in[i-1]);
	record_type = hb*16 + lb; //tb; //HEX2CHAR(in[i++], in[i++]);
	if ( record_type != 0 && record_type != 1 )
	{
//		printf( "\n*** Cannot decode HEX file.\n" );
		return FLA_RECORD_BAD_TYPE;
	}

	if (FLA_MAX_CODE_ADDR < offset)
	{
		return FLA_CODE_ADDRESS_OUT_OF_RANGE;
	}

	for ( j = 0; j < len; j++ )
	{
		hb = toint(in[i]); i++; lb = toint(in[i]); i++;
		tb = toint(in[i-2])*16 + toint(in[i-1]);
		fla_mem_p->buff[j] = hb*16 + lb;
	}
#ifdef FLA_FAKE_MEM_ACCESS
	return record_type;
#else // FLA_FAKE_MEM_ACCESS

	FLASH_Write ( FLA_TEMP_MEM_OFFSET + (FLADDR) offset, &(fla_mem_p->buff[0]), (uint16_t) len, false);

	// verify the checksum
	pread = ( char code* ) ((uint16_t)FLA_TEMP_MEM_OFFSET + offset);      // initialize the read pointer
	hb = toint(in[i]); i++; lb = toint(in[i]); i++;
	checksum =  hb*16 + lb;              // get the HEX record checksum field
	flash_checksum = 0;                 // set the flash_checksum to zero

	// add the data field stored in FLASH to the checksum
	for ( j = 0; j < len; j++ )
	{
		flash_checksum += *pread++;
	}

	// add the remaining fields
	flash_checksum += len;
	flash_checksum += ( char )( offset >> 8 );
	flash_checksum += ( char )( offset & 0x00FF );
	flash_checksum += record_type;
	flash_checksum += checksum;

	// verify the checksum (the flash_checksum should equal zero)
	if ( flash_checksum != 0 )
	{
		//printf( "*** Checksum failed, try again." );
		return FLA_MEM_CHECKSUM_FAILED;
	}

	return record_type;
#endif // else/not FLA_FAKE_MEM_ACCESS
}
#endif // _ENABLE_FLA_STRING_INTEL_HEX_



/**
* fla_recv_binary_Intel_hex() - reads the received code piece (in binary Intel HEX) 
*	and writes it into temp flash memory
*/
uint8_t fla_recv_binary_Intel_hex( uint8_t xdata *in )
{
	uint8_t i = 0;
	uint8_t j ;
	uint8_t len = 0;
	uint16_t offset = 0;
	uint8_t	record_type;
	uint8_t checksum;
	uint8_t flash_checksum;
	char code * pread;
	uint8_t hb=0;
	uint8_t lb=0;
	uint8_t tb=0;

	// get the length
	len = in[i]; i++; 

	// get the offset
	offset = in[i]; i++; 
	offset <<= 8;
	offset |= in[i]; i++; 


	// get the record type
	record_type = in[i]; i++;
	if ( record_type != 0 && record_type != 1 )
	{
//		printf( "\n*** Cannot decode HEX file.\n" );
		return FLA_RECORD_BAD_TYPE;
	}

	if (FLA_MAX_CODE_ADDR < offset)
	{
		return FLA_CODE_ADDRESS_OUT_OF_RANGE;
	}

#ifdef FLA_FAKE_MEM_ACCESS
	return record_type;
#else // FLA_FAKE_MEM_ACCESS

	FLASH_Write ( FLA_TEMP_MEM_OFFSET + (FLADDR) offset, &(in[i]), (uint16_t) len, false);
	i += len;

	// verify the checksum
	pread = ( char code* ) ((uint16_t)FLA_TEMP_MEM_OFFSET + offset);      // initialize the read pointer
	checksum =  in[i];              // get the HEX record checksum field
	flash_checksum = 0;                 // set the flash_checksum to zero

	// add the data field stored in FLASH to the checksum
	for ( j = 0; j < len; j++ )
	{
		flash_checksum += *pread++;
	}

	// add the remaining fields
	flash_checksum += len;
	flash_checksum += ( char )( offset >> 8 );
	flash_checksum += ( char )( offset & 0x00FF );
	flash_checksum += record_type;
	flash_checksum += checksum;

	// verify the checksum (the flash_checksum should equal zero)
	if ( flash_checksum != 0 )
	{
		//printf( "*** Checksum failed, try again." );
		return FLA_MEM_CHECKSUM_FAILED;
	}

	return record_type;
#endif // else/not FLA_FAKE_MEM_ACCESS
}


// ############################################################################
// ############################################################################
/**
  * fla_erase_temp() - erases the temporary FLASH memory area
  */
void fla_erase_temp()
{
	uint8_t i;
	for(i=0; i< FLA_TEMP_MEM_PAGE_COUNT; i++)
	{
#ifndef FLA_FAKE_MEM_ACCESS
		FLASH_PageErase( FLA_TEMP_MEM_OFFSET + i * FLASH_PAGESIZE, false );
#endif // not FLA_FAKE_MEM_ACCESS
	}
}


// ############################################################################
// ############################################################################


/**
  * fla_send_msg(msg_type, msg_info) - sends a packet with specific info
  */
uint8_t fla_send_msg(uint8_t msg_type, uint8_t msg_info)
{
	uint8_t pkt_id = 0xFF;
	uint8_t size = tsp_reserve_packet(FLA_MSG_RESPONSE_SIZE, &pkt_id, BS_ADDR);
	if (0 == size)
	{	// Error
		sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	{
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
		uint8_t *mod = tsp_get_dummy(pkt_id);
	// 3) Fill the headers of the APP-specific module and the END module (REQUIRED!!)
	//    .... (Possibly defered, but not for too long)
		fla_cmd_packet_v1_t *ph = ( fla_cmd_packet_v1_t * )mod ;
		ph->module_type = PKT_MODULE_TYPE_FLA_BASIC;
		ph->module_length = FLA_MSG_RESPONSE_SIZE; // Actually it already is set
	// 4) Fill the content of the APP-specific module
		ph->message_type = msg_type;
		ph->message = msg_info;
	}
	// 5) Send the Packet out via TRANSPORT layer (by default put into the queue)
	return tsp_send_from_modules(pkt_id);

}

// ############################################################################
// ############################################################################

#endif // _ENABLE_FLA_BASIC_


void fla_copy_temp_to_main(uint8_t no_pages) //reentrant
{
// Leave empty if it is not F120 proc (due to lack of implementation)
#ifdef _ENABLE_FLA_BASIC_
	char xdata * data pwrite;           // FLASH write pointer
 	char code * data pread;             // FLASH read pointer
 	uint8_t data byte;
	uint16_t no_bytes = (uint16_t)no_pages * (uint16_t)FLASH_PAGESIZE;
	uint16_t i;

	FLADDR addr = FLA_TEMP_MEM_OFFSET;
	
	SFRPAGE = LEGACY_PAGE;             // restore SFRPAGE
	EA = 0;                       // restore interrupts
// set banks
	   SFRPAGE = LEGACY_PAGE;

	if (addr < 0x10000) {               // 64K linear address
		pread = (char code *) (unsigned int) addr;
		PSBANK &= ~0x30;                 // COBANK = 0x2
		PSBANK |=  0x10;
	} else if (addr < 0x18000) {        // BANK 2
      addr |= 0x8000;
      pread = (char code *) (unsigned int) addr;
      PSBANK &= ~0x30;                 // COBANK = 0x2
      PSBANK |=  0x20;
	} else {                            // BANK 3
      pread = (char code *) (unsigned int) addr;
      PSBANK &= ~0x30;                 // COBANK = 0x3
      PSBANK |=  0x30;
	}

	pwrite = FLA_DEST_CODE_ADDR;

// erase main memory (but THIS FUNCTION CODE)
	for(i = 0; i < no_pages; i++)
	{
		FLSCL |= 0x01;                      // enable FLASH writes/erases
		PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1

		RSTSRC = 0x02;                      // enable VDDMON as reset source
		*pwrite = 0;                        // initiate page erase

		PSCTL &= ~0x03;                     // PSWE = 0; PSEE = 0
		FLSCL &= ~0x01;                     // disable FLASH writes/erases

		pwrite += FLASH_PAGESIZE;
	}
// copy bytes from temp to the main
//	return;

	pwrite = FLA_DEST_CODE_ADDR;

	for(i = 0; i < no_bytes; i++)
	{
		// READ BYTE
		byte = *pread;	

		// WRITE BYTE
		FLSCL |= 0x01;                      // enable FLASH writes/erases
		PSCTL |= 0x01;                      // PSWE = 1

		RSTSRC = 0x02;                      // enable VDDMON as reset source
		*pwrite = byte;                     // write the byte

		PSCTL &= ~0x01;                     // PSWE = 0
		FLSCL &= ~0x01;                     // disable FLASH writes/erases

		// MOVE to NEXT BYTE
		pwrite++;
		pread++;
	}

#ifdef _SFRPAGE_EXIST_
	SFRPAGE = 0;
#endif // _SFRPAGE_EXIST_
	RSTSRC |= SWRSEF;

#endif // _ENABLE_FLA_BASIC_

}

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################



