#ifndef _MON_POWER_BASIC_PUB_H_
#define _MON_POWER_BASIC_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/


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
  * mon_power_init () - sets up MONitor of power module
  */
void mon_power_init ( void );



/**
* mon_power_test() - tests power monitoring chip
*/
uint8_t mon_power_test();


/**
  * mon_power_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void mon_power_loop( void );


/**
  * mon_power_execute_command (packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void mon_power_execute_command ( uint8_t *packet);

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################


#endif // _MON_POWER_BASIC_PUB_H_


