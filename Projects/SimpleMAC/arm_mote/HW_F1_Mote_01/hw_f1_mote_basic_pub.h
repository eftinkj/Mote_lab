#ifndef _HW_F1_MOTE_BASIC_PUB_H_
#define _HW_F1_MOTE_BASIC_PUB_H_

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
  * hw_f1_mote_init () - sets up MONitor of power module
  */
void hw_f1_mote_init ( void );



/**
* hw_f1_mote_test() - tests power monitoring chip
*/
uint8_t hw_f1_mote_test( void );


/**
  * hw_f1_mote_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void hw_f1_mote_loop( void );


/**
  * hw_f1_mote_execute_command (packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void hw_f1_mote_execute_command ( uint8_t *packet);

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
} hw_f1_mote_cmd_packet_t PACKED;


typedef struct 
{
	uint8_t module_type;
	uint8_t module_length;

	uint8_t report_type;

	uint16_t	aver_temp;
	uint16_t	aver_g_x;
	uint16_t	aver_g_y;
	uint16_t	aver_g_z;

	int16_t	acc_watt_a;
	int16_t	acc_watt_b;
	int16_t	acc_watt_c;

	int16_t	acc_var_a;
	int16_t	acc_var_b;
	int16_t	acc_var_c;

	uint16_t	acc_va_a;
	uint16_t	acc_va_b;
	uint16_t	acc_va_c;

} hw_f1_mote_report_packet_v1_t PACKED;

#define hw_f1_mote_REPORT_V1	01

#endif // _HW_F1_MOTE_BASIC_PUB_H_


