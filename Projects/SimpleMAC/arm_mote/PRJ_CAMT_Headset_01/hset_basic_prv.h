#ifndef _HSET_BASIC_PRV_H_
#define _HSET_BASIC_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB\rtc.h"
//#include "spi_pub.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/


#define HSET_DEFAULT_DELAY	2000
#define HSET_CMD_TIMEOUT	1000

// Read power (Watt-Hour) every 250ms
//#define HSET_READ_POWER_DEFAULT_TIMEOUT	60000

#define HSET_SAMPLES_PER_PKT	80


// ADC settings
#define HSET_ADC_NO_CHANNELS	1
#define HSET_ADC_AVERAGER	10
#define HSET_ADC_DIVIDER	10

#define HSET_ADC_VOICE	0


//#define HSET_DEST	0x03


#define HSET_MAX_SPI_MESSAGE 2

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

extern rtc_tick_t	hset_timeout;
//extern rtc_tick_t	hset_command_timeout;
extern uint8_t		hset_read_enabled;
extern uint8_t 	i_max;
extern uint8_t packet_flag;

//extern spi_event_t vocoder_comm_event_flag;
//extern spi_event_t vocoder_comm_event_mask;


extern uint8_t hset_message[HSET_MAX_SPI_MESSAGE];
extern uint8_t msg_size;


/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/*******************************************************************************
* Function Name  : hset_processSerialInput
* Description    : It processes serial commands
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void hset_processSerialInput( void );



/**
* uint8_t hset_create_report () - creates and fills out the report packet
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t hset_create_report ();



void strip_packets();

//unsigned int noise();

//uint8_t pack_output(int16_t *temp_message);



/**
 * hset_sent_packet() - sends the current packet out
 */
void hset_sent_packet();

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################

#endif // _HSET_BASIC_PRV_H_
