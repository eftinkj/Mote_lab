#ifndef _FLA_BASIC_PRV_H_
#define _FLA_BASIC_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
//#include "rtc.h"
#include "fla_basic_pub.h"


/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/

//#define MAX_LOOPS		10

//#define MAX_TIMEOUTS	10

// State of the memory
#define FLA_STATE_TEMP_CORRUPTED	0x00
#define FLA_STATE_TEMP_EMPTY		0x01
#define FLA_STATE_TEMP_RECEIVING	0x02
#define FLA_STATE_TEMP_READY		0x03



// size of FLA specific Module with response info (short - 1b type + 1b info)
#define FLA_MSG_RESPONSE_SIZE 2


#define	FLA_TEMP_MEM_OFFSET	0x8000UL
#define FLA_TEMP_MEM_PAGE_COUNT	32
#define FLA_COPY_ALL_PAGES	30
#define FLA_DEST_CODE_ADDR	0x0000UL
#define FLA_MAX_CODE_ADDR	0x7FFFUL
 
//#define toint(a) ( ( (a % 87) % 55) % 48)
#define toint(a) ( (a>58)?(a-55):(a-48) )

#define HEX2CHAR(byteH,byteL) (toint( byteH ) * 16 +  toint( byteL ) )


typedef struct
{
	uint8_t fla_img_seq_;
	uint8_t fla_status_;
	uint8_t fla_delayed_hex_ack;
	uint8_t xdata *buff;
	uint8_t dynamic; // pointer to the rest of memory
}  fla_mem_t;


//#define FLA_MEM_STRUCT_OFFSET_BUFF	sizeof(void xdata*)
#define FLA_MEM_STRUCT_SIZE			sizeof(fla_mem_t)
// Length of the buffer for Intel HEX format (one string line)
#define FLA_MEM_BUFF_SIZE			0x10

#define FLA_MEM_SIZE	(FLA_MEM_STRUCT_SIZE+FLA_MEM_BUFF_SIZE)

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

#ifdef _ENABLE_FLA_STRING_INTEL_HEX_
/**
* fla_recv_Intel_hex() - reads the received code piece (in Intel HEX) 
*	and writes it into temp flash memory
*/
uint8_t fla_recv_Intel_hex( uint8_t xdata *in );
#endif // _ENABLE_FLA_STRING_INTEL_HEX_

/**
* fla_recv_binary_Intel_hex() - reads the received code piece (in binary Intel HEX) 
*	and writes it into temp flash memory
*/
uint8_t fla_recv_binary_Intel_hex( uint8_t xdata *in );

/**
  * fla_erase_temp() - erases the temporary FLASH memory area
  */
void fla_erase_temp();

/**
  * fla_send_msg(msg_type, msg_info) - sends a packet with specific info
  */
uint8_t fla_send_msg(uint8_t msg_type, uint8_t msg_info);


/**
  * fla_copy_temp_to_main( no_pages ) - copy code from TEMP area to main code area
  *		and restart the proc (to use the new code)
  */
void fla_copy_temp_to_main(uint8_t no_pages) ;//reentrant;

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/



// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _FLA_BASIC_PRV_H_