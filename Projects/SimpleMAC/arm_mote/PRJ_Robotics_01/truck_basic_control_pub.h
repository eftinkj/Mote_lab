#ifndef _TRUCK_BASIC_CONTROL_PUB_H_
#define _TRUCK_BASIC_CONTROL_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

// Command field of the truck control module
#define TRUCK_CMD_STOP	0x00
#define TRUCK_CMD_MOVE_FORWARD	0x01
#define TRUCK_CMD_MOVE_BACK		0x02
#define TRUCK_CMD_FREQ_SET		0xF0
#define TRUCK_CMD_KILLSWITCH	0xFF

#define TRUCK_DIR_STRIGHT	0x00
#define TRUCK_DIR_LEFT	0x01
#define TRUCK_DIR_RIGHT	0x02

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
  * truck_init() - sets up truck module
  */
void truck_init ( void );


/**
  * truck_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void truck_loop ( void );


/**
  * truck_execute_command(packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void truck_execute_command ( uint8_t *packet);


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
} truck_command_packet_t PACKED;


typedef struct 
{
	uint8_t version;
	uint8_t length;
	uint8_t command;
	uint8_t	speed;
	uint8_t	turbo;
	uint8_t	direction;
} truck_command_packet_v1_t PACKED;


#endif // _TRUCK_BASIC_CONTROL_PUB_H_


