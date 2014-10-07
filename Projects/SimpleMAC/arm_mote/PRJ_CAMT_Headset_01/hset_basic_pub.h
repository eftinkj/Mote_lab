#ifndef _HSET_BASIC_PUB_H_
#define _HSET_BASIC_PUB_H_

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
//#define HSET_MAX_SPI_MESSAGE 2
//uint8_t hset_message[HSET_MAX_SPI_MESSAGE];

/**
  * hset_init () - sets up Headset module
  */
void hset_init ( void );


/**
  * hset_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void hset_loop( void );


/**
  * hset_execute_command (packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void hset_execute_command ( uint8_t *module);

/**
* hset_recv_uart(rx_buff) - called when received a new sample byte 
*   NEED TO BE QUICK!! VERY QUICK!!
*/
void hset_recv_uart(uint8_t rx_buff);


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################

typedef struct 
{
	uint8_t version;
	uint8_t length;
	uint8_t content;
} hset_cmd_packet_t PACKED;


typedef struct 
{
	uint8_t module_type;
	uint8_t module_length;

	uint8_t report_type;

	uint8_t content;

} hset_report_packet_v1_t PACKED;

#define HSET_REPORT_V1	01

#endif // _HSET_BASIC_PUB_H_

