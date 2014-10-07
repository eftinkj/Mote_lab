#ifndef _SRC_8BIT_ADC_PUB_H_
#define _SRC_8BIT_ADC_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"
#include "packet.h"

#ifdef _ENABLE_SRC_8BIT_ADC_

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

void src_8bit_powerup ();

/**
  * src_init () - sets up Signal Processing Module
  */
void src_8bit_adc_init ( void );



/**
 * src_enable() - enable the dummy source
 */
void src_8bit_adc_enable();


/**
 * src_disable() - disable the dummy source
 */
void src_8bit_adc_disable();


/**
  * src_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void src_8bit_adc_loop( void );


/**
 * src_set_rate(uint16_t rate) - set a sample rate in Hz (up to 1kHz)
 */
void src_8bit_adc_set_rate(uint16_t);


/**
 * src_attach ( uint8_t context, void code* receiver) - connects to a receiver module
 */
void src_8bit_adc_attach ( uint8_t context);// void code* receiver);

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################
#endif // _ENABLE_SRC_8BIT_ADC_


#endif // _SRC_8BIT_ADC_PUB_H_


