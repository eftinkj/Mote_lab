#ifndef _VEX_TANK_BASIC_CONTROL_PUB_H_
#define _VEX_TANK_BASIC_CONTROL_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/

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
  * vex_tank_init() - sets up bot module
  */
void vex_tank_init ( void );


/**
  * vex_tank_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void vex_tank_loop ( void );


/**
  * vex_tank_execute_command(packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void vex_tank_execute_command ( uint8_t *packet);


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

} vex_tank_command_packet_t PACKED;


typedef struct 
{
	uint8_t version;
	uint8_t length;
	uint8_t command;
	uint8_t	LM_speed;
	uint8_t	RM_speed;
} vex_tank_command_packet_v1_t PACKED;


#endif // _VEX_TANK_BASIC_CONTROL_PUB_H_


