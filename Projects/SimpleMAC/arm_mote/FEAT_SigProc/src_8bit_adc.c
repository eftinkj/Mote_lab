#include "defs/esnl_pub.h"

#ifdef _ENABLE_SRC_8BIT_ADC_

#include "src_8bit_adc_prv.h"
#include "src_8bit_adc_pub.h"


#include "FEAT_Scheduler/sch_basic_pub.h"
#include "sys_func.h"
//#include "common.h"


/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/



src_d_mem_t XDATA* src_d_mem_p;

uint8_t	XDATA src_d_mem_raw[SRC_D_MEM_STRUCT_SIZE];

sint16_t adc_value;

#define SRC_8BIT_ADC_DEBUG

void src_8bit_powerup ()
{
	//int i;
	//init variables
	// init hardware config (e.g. GPIOs? ADCs?)
	//ADC0 input PB5
//	GPIO_PBCFGH[7:4] = 0;
//	GPIO_PBCFGH = GPIO_PBCFGH & 0x11111101;
	//ADC1 input PB6
//	GPIO_PBCFGH[11:8] = 00;
//	GPIO_PBCFGH = GPIO_PBCFGH & 0xFFFFF0FF;
	//ADC2 input PB7
//	GPIO_PBCFGH[15:12] = 0;
//	GPIO_PBCFGH = GPIO_PBCFGH & 0xFFFF0FFF;
	//ADC3 input PC1
//	GPIO_PCCFGH[7:4] = 0;
	//ADC4 input PA4
//	GPIO_PACFGH[3:0] = 0;
	//ADC5 input PA5 -  PACFGH [7:4]
//	((gpia_pacfgh_t)GPIO_PACFGH).PA5 = 0;
	GPIO_PACFGH = GPIO_PACFGH & 0xFFFFFF0F;	// PA5/ADC5
	GPIO_PACFGH = GPIO_PACFGH & 0xFFFFFFF0; // PA4/ADC4
	
}

/**
 * ssn_init () - sets up Signal Processing Module
 */
void src_8bit_adc_init ( void )
{
	src_d_mem_p = (src_d_mem_t XDATA*)src_d_mem_raw;
	src_d_mem_p -> enabled_ = 0;
	src_d_mem_p -> timeout_ = 0;
	src_d_mem_p -> rate_ = SRC_8BIT_DEFAULT_RATE;
	src_d_mem_p -> seq_ = 0;
	src_d_mem_p -> context_ = 0xFF;
//	src_d_mem_p -> receiver = (recv_func_t *)NULL;

	sch_add_loop( (sch_loop_func_t) src_8bit_adc_loop);

#ifdef SRC_8BIT_ADC_DEBUG
	src_8bit_adc_enable();
#endif // SRC_8BIT_ADC_DEBUG
	halAdcCalibrate(ADC_USER_APP);
}




/**
 * src_enable() - enable the dummy source
 */
void src_8bit_adc_enable()
{
	src_d_mem_p -> timeout_ = rtc_get_ticks() + 1000 / src_d_mem_p -> rate_;
	src_d_mem_p -> enabled_ = SRC_D_ENABLED;
}



/**
 * src_disable() - disable the dummy source
 */
void src_8bit_adc_disable()
{
//	src_d_mem_p -> timeout_ = rtc_get_ticks() + 1000 / src_d_mem_p -> rate_;
	src_d_mem_p -> enabled_ = SRC_D_DISABLED;
}


	
	
#include "ssn_basic_pub.h"


/**
 * ssn_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
 */
void src_8bit_adc_loop( void )
{
	uint16_t temp = 0;
	volatile StStatus result = 0;
	if ( SRC_D_ENABLED == src_d_mem_p -> enabled_)
	{
		if (rtc_get_ticks() > src_d_mem_p -> timeout_)
		{
			//(src_d_mem_p -> receiver)(src_d_mem_p->context_, src_d_mem_p->seq_++);
			result = 1;
			if ( ST_ADC_CONVERSION_DONE == halRequestAdcData(ADC_USER_APP, &adc_value))
			{
				//stat != ST_ADC_CONVERSION_DONE
				temp = (adc_value<0)?0:adc_value;
			}
			else
			{
				temp = src_d_mem_p->seq_++;
			}
			ssn_recv_sample(src_d_mem_p->context_, temp);
			src_d_mem_p -> timeout_ = rtc_get_ticks() + 1000 / src_d_mem_p -> rate_;

			result = halStartAdcConversion(ADC_USER_APP, ADC_REF_INT,
										   ADC_SOURCE_ADC4_VREF2, ADC_CONVERSION_TIME_US_1024);
			if (ST_ERR_FATAL != result)
			{
				sys_error(3);
			}
		}
	}
}
	

/**
 * src_set_rate(uint16_t rate) - set a sample rate in Hz (up to 1kHz)
 */
void src_8bit_adc_set_rate(uint16_t rate)
{
	src_d_mem_p->rate_ = rate;
}


/**
 * src_attach ( uint8_t context, void code* receiver) - connects to a receiver module
 */
void src_8bit_adc_attach ( uint8_t context)// void code* receiver)
{
	src_d_mem_p->context_ = context;
}


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/



// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################



#endif // _ENABLE_SIG_BASIC_

