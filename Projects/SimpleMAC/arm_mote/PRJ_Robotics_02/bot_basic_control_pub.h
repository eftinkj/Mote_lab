#ifndef _BOT_BASIC_CONTROL_PUB_H_
#define _BOT_BASIC_CONTROL_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

// Command field of the bot control module
#define BOT_CMD_STOP		0
#define BOT_CMD_G0			1
#define BOT_CMD_TEST		2
#define BOT_CMD_UPDATE		3
#define BOT_CMD_UPDATE_REF  4
#define BOT_CMD_SPEED_SET	5
#define BOT_CMD_SEND		6

//#define BOT_CMD_FREQ_SET	6
//#define BOT_CMD_KILLSWITCH	0xFF
//#define BOT_CMD_SEND_COUNT		5

#define BOT_DIR_STRIGHT	0x00
#define BOT_DIR_LEFT	0x01
#define BOT_DIR_RIGHT	0x02

//#define BOT_ID			0x01
//#define BOT_ID			0x02
//#define BOT_ID			0x03
//#define BOT_ID			0x04
//#define BOT_ID			0x05

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
  * bot_init() - sets up bot module
  */
void bot_init ( void );


/**
  * bot_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void bot_loop ( void );


/**
  * bot_execute_command(packet) - executes a command received in "packet" 
  *		sets the controller and prepares timeout
  */
void bot_execute_command ( uint8_t *packet);


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
} bot_command_packet_t PACKED;


typedef struct 
{
	uint8_t version;
	uint8_t length;
	uint8_t command;
	uint8_t	L_speed;
	uint8_t	R_speed;
	uint8_t lin_vel;
	uint8_t ang_vel;
	uint8_t theta;
	uint8_t pos_x;
	uint8_t pos_y;
	uint8_t ref_x;
	uint8_t ref_y;
	uint8_t ref_theta;
	uint8_t sampling_T;
	uint8_t encoder_cal;
	uint8_t	direction;//can also used to set frequency of PWM


} bot_command_packet_v1_t PACKED;


#endif // _BOT_BASIC_CONTROL_PUB_H_


