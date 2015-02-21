#ifndef _SSN_BASIC_PUB_H_
#define _SSN_BASIC_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"
#include "packet.h"


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
//class ssn_basic {
//	ssn_basic() { ssn_powerup(); }
//	~ssn_basic() {}
	

/**
  * ssn_powerup () - sets up internal variables of Signal Processing Module
  */
void ssn_powerup ( void );

/**
  * ssn_init () - initializes and conencts the Signal Processing Module
  */
void ssn_init ( void );


/**
 * uint16_t ssn_start_burst(uint16_t len) - starts the burst transmission of LEN packets (e.g.)
 * RETURNS the number of scheduled packets or 0 is failed/already transmitting
 */
uint16_t ssn_start_burst(uint16_t len);

/**
 * uint16_t ssn_stop_burst() - stops the burst transmission
 * RETURNS 1 when successful (always??
 */
uint16_t ssn_stop_burst();

/**
* ssn_test() - tests Signal Processing operation (e.g.)
*/
uint8_t ssn_test();


//  public:
/**
  * ssn_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void ssn_loop( void );


/**
 * ssn_recv_sample(uint8_t context, uint16_t value) - receives a sample
 */
void ssn_recv_sample(uint8_t, uint16_t);

/**
  * fla_recv_cmd(module, rssi) - handles a command (packet's module)
  */
//void fla_recv_cmd(pkt_mod_header_t xdata *module, uint8_t rssi);

/**
 * ssn_execute_command(packet) - executes a command received in "packet"
 */
void ssn_execute_command ( uint8_t *packet);

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################

//}; // class ssn_basic


#endif // _SSN_BASIC_PUB_H_


