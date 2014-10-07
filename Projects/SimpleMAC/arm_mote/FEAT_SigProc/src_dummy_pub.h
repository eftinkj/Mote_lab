#ifndef _SRC_DUMMY_PUB_H_
#define _SRC_DUMMY_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"
#include "packet.h"

#ifdef _ENABLE_SRC_DUMMY_

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/


// Message module types
//#define FLA_MSG_TYPE_INTEL_HEX			0x01
//#define FLA_MSG_TYPE_ERASE_TEMP_FLASH	0x02


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
  * src_init () - sets up Signal Processing Module
  */
void src_init ( void );



/**
 * src_enable() - enable the dummy source
 */
void src_enable();


/**
 * src_disable() - disable the dummy source
 */
void src_disable();


/**
  * src_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void src_loop( void );


/**
 * src_set_rate(uint16_t rate) - set a sample rate in Hz (up to 1kHz)
 */
void src_set_rate(uint16_t);


/**
 * src_attach ( uint8_t context, void code* receiver) - connects to a receiver module 
 */
void src_attach ( uint8_t context);// void code* receiver);

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################
#endif // _ENABLE_SRC_DUMMY_


#endif // _SRC_DUMMY_PUB_H_


