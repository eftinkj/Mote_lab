#ifndef _RFID_BASIC_PUB_H_
#define _RFID_BASIC_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"


/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/


#define RFID_READ_BUFFER_SIZE 32

// ######## BEGIN ########
// PACKET MODULE RFID READER V1 - Reports/Responses Types

// Simple report of RFID Tag <TYPE> and <ID> -> fixed size of 15 (0x0F)
#define RFID_REPORT_TYPY_TAG_INVENTORY 0x01
#define RFID_REPORT_TYPY_TAG_INVENTORY_LEN 0x0F

// ######## END ########

// FOR MESSAGE HEADER
#define MSG_LEN 2
#define FLAGS 2
#define CMD 2
#define RID 4
#define TAG_TYPE 2
#define TID_LEN 1
#define TID 16
#define AFI 1
#define SESSION 1
#define ADD 2
#define NUM_BLOCKS 2
#define DATA_LEN 2
#define DATA1 1000
#define CRC 2

// For Flag bytes

#define Loop_f 0x01
#define Inventory_f 0x02
#define Lock_f 0x03
#define RF_f 0x04
#define AFI_f 0x05
#define CRC_f 0x06
#define TID_f 0x07
#define RID_f 0x08
#define Encryption_f 0x09
#define HMAC_f 0x0A
#define Session_f 0x0B
#define Data_f 0x0C

typedef struct
{
	uint16_t msg_len;
	uint16_t flags;
	uint16_t cmd;
	uint32_t rid;
	uint16_t tag_type;
	uint8_t tid_len;
	uint8_t	tid;
//[8];
//	uint8_t afi;
//	uint8_t session;
//	uint16_t add;
//	uint16_t num_blocks;
//	uint16_t data_len;
//	uint16_t data1[250];
//	uint16_t crc;
} rfid_frame_head;



typedef struct
{
	uint8_t	start;	// start byte
//	uint16_t	start;	// start byte
	uint16_t msg_len;
	uint16_t flags;
	uint16_t cmd;
	uint16_t tag_type;
	uint16_t crc;
} rfid_cmd_frame;


typedef struct
{
	uint8_t	start;	// start byte
	uint16_t msg_len;
	uint16_t flags;
	uint16_t cmd;
	uint16_t tag_type;
	uint16_t tag_len;
	uint8_t tag_id;
} rfid_taglist_frame;



typedef struct
{
	uint8_t	start;	// start byte
//	uint16_t	start;	// start byte
	uint16_t msg_len;
	uint16_t flags;
	uint16_t cmd;
	uint16_t addr;
	uint16_t param_len;
	uint16_t crc;
} rfid_sysparam_frame;

//spi_event_t mon_power_event_flag;


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
  * rfid_init () - sets up RFID Reader module
  */
void rfid_init ( void );


/**
  * rfid_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void rfid_loop( void );


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################


#endif // _RFID_BASIC_PUB_H_


