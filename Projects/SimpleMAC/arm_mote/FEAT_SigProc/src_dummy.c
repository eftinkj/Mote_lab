#include "defs/esnl_pub.h"

#ifdef _ENABLE_SRC_DUMMY_

#include "src_dummy_prv.h"
#include "src_dummy_pub.h"


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

#define SRC_DUMMY_DEBUG

/**
 * ssn_init () - sets up Signal Processing Module
 */
void src_init ( void )
{
	src_d_mem_p = (src_d_mem_t XDATA*)src_d_mem_raw;
	src_d_mem_p -> enabled_ = 0;
	src_d_mem_p -> timeout_ = 0;
	src_d_mem_p -> rate_ = 500;
	src_d_mem_p -> seq_ = 0;
	src_d_mem_p -> context_ = 0xFF;
//	src_d_mem_p -> receiver = (recv_func_t *)NULL;

	sch_add_loop( (sch_loop_func_t) src_loop);

#ifdef SRC_DUMMY_DEBUG
	src_enable();
#endif // SRC_DUMMY_DEBUG
}




/**
 * src_enable() - enable the dummy source
 */
void src_enable()
{
	src_d_mem_p -> timeout_ = rtc_get_ticks() + 1000 / src_d_mem_p -> rate_;
	src_d_mem_p -> enabled_ = SRC_D_ENABLED;
}



/**
 * src_disable() - disable the dummy source
 */
void src_disable()
{
//	src_d_mem_p -> timeout_ = rtc_get_ticks() + 1000 / src_d_mem_p -> rate_;
	src_d_mem_p -> enabled_ = SRC_D_DISABLED;
}


	
	
#include "ssn_basic_pub.h"
	
/**
 * ssn_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
 */
void src_loop( void )
{
	if ( SRC_D_ENABLED == src_d_mem_p -> enabled_)
	{
		if (rtc_get_ticks() > src_d_mem_p -> timeout_)
		{
			//(src_d_mem_p -> receiver)(src_d_mem_p->context_, src_d_mem_p->seq_++);
			ssn_recv_sample(src_d_mem_p->context_, src_d_mem_p->seq_++);
			src_d_mem_p -> timeout_ = rtc_get_ticks() + 1000 / src_d_mem_p -> rate_;
		}
	}
}
	

/**
 * src_set_rate(uint16_t rate) - set a sample rate in Hz (up to 1kHz)
 */
void src_set_rate(uint16_t rate)
{
	src_d_mem_p->rate_ = rate;
}


/**
 * src_attach ( uint8_t context, void code* receiver) - connects to a receiver module 
 */
void src_attach ( uint8_t context)// void code* receiver)
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

