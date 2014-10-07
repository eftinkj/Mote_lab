#ifndef _QRHELO_BASIC_CONTROL_PUB_H_
#define _QRHELO_BASIC_CONTROL_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

// Command field of the bot control module
#define QRHELO_CMD_STOP		0
#define QRHELO_CMD_TRIM		1




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
  * qrhelo_init() - sets up bot module
  */
void qrhelo_init ( void );


/**
  * qrhelo_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void qrhelo_loop ( void );


/**
  * qrhelo_execute_command(packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void qrhelo_execute_command ( uint8_t *packet, uint8_t rssi);


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

} qrhelo_command_packet_t PACKED;


typedef struct 
{
	uint8_t version;
	uint8_t length;
	uint8_t command;
	uint8_t	R1_speed;
	uint8_t	R2_speed;
	uint8_t	R3_speed;
	uint8_t	R4_speed;

} qrhelo_command_packet_v1_t PACKED;


#endif // _QRHELO_BASIC_CONTROL_PUB_H_


