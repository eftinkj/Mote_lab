#ifndef _FLA_BASIC_PUB_H_
#define _FLA_BASIC_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"
#include "packet.h"


/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/
#define FLA_NO_TIMEOUT_ID	255
#define FLA_NO_FUNC_ID	0


#define	FLA_RECORD_CONTINUE		0x00
#define	FLA_RECORD_END_OF_FILE	0x01

#define	FLA_MEM_CHECKSUM_FAILED	0x80
#define FLA_RECORD_BAD_TYPE		0x81
#define FLA_CODE_ADDRESS_OUT_OF_RANGE	0x82



// Message module types
#define FLA_MSG_TYPE_INTEL_HEX			0x01
#define FLA_MSG_TYPE_ERASE_TEMP_FLASH	0x02
// Start 
#define FLA_MSG_TYPE_START_FLASH_TEMP	0x03
// Initiate flashing of the code from TEMP area to the common (apply the new code)
#define FLA_MSG_TYPE_START_CODE_SWAP	0x04
// Received message out of sync (sequence no. does not match)
#define FLA_MSG_TYPE_RESYNC_SEQ			0x05
// Flash Manager has incorrect status for the request 
//		(e.g. corrupted temp flash when trying to write new code)
#define FLA_MSG_TYPE_BAD_STATUS			0x06
// Flashing of a code to TEMP area has failed
#define FLA_MSG_TYPE_TEMP_FLASH_FAILURE	0x07

#define FLA_MSG_TYPE_GET_STATUS			0x08

#define FLA_MSG_TYPE_TEMP_FLASH_CORRUPTED	0x09

#define FLA_MSG_TYPE_BINARY_INTEL_HEX	0x0a

// Acknowledge message
#define FLA_MSG_TYPE_ACK	0x10
// Acknowledge message
#define FLA_MSG_TYPE_HEX_ACK	0x11


// loop function
//typedef void (code *fla_loop_func_t)(void);


/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/



/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

/**
  * fla_init () - sets up Flash Access module
  */
void fla_init ( void );



/**
* fla_test() - tests Flash Access  operation (e.g.)
*/
uint8_t fla_test();


/**
  * fla_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void fla_loop( void );


/**
  * fla_recv_cmd(module, rssi) - handles a command (packet's module)
  */
void fla_recv_cmd(pkt_mod_header_t xdata *module, uint8_t rssi);



// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################


#endif // _FLA_BASIC_PUB_H_


