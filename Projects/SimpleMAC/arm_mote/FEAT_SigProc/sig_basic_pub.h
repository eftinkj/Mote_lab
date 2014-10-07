#ifndef _SIG_BASIC_PUB_H_
#define _SIG_BASIC_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"
#include "packet.h"


/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/


#define	FILTER_TYPE_BUTTER		0x01



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
  * sig_init () - sets up Signal Processing Module
  */
void sig_init ( void );



/**
* sig_test() - tests Signal Processing operation (e.g.)
*/
uint8_t sig_test();


/**
  * sig_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void sig_loop( void );


/**
  * fla_recv_cmd(module, rssi) - handles a command (packet's module)
  */
//void fla_recv_cmd(pkt_mod_header_t xdata *module, uint8_t rssi);



// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################


#endif // _FLA_BASIC_PUB_H_


