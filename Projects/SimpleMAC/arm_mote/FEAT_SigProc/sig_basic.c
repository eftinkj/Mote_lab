#include "defs/esnl_pub.h"

#ifdef _ENABLE_SIG_BASIC_

#include "sig_basic_prv.h"
#include "sig_basic_pub.h"


//#include "sys_func.h"

//#include "packet.h"
//#include "common.h"


#include "FEAT_Networking/tsp_common_pub.h"
#include "FEAT_Scheduler/sch_basic_pub.h"


//#define SIG_TEST
#undef SIG_TEST


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



sig_filter_mem_t xdata* filter_mem_p;
uint8_t	xdata filter_mem_raw[SIG_FILTER_MEM_SIZE];


/**
  * sig_init () - sets up Signal Processing Module
  */
void sig_init ( void )
{
	float xdata * temp_coeff;
	filter_mem_p = (sig_filter_mem_t xdata* )filter_mem_raw;
	filter_mem_p->sig_filter_idx = FILTER_DEFAULT_SIZE;
	filter_mem_p->sig_filter_size = FILTER_DEFAULT_SIZE;
	filter_mem_p->sig_filter_type = FILTER_TYPE_BUTTER;
	offset = 0;
	temp_coeff = (float xdata*)&(filter_mem_p->dynamic);
	
	filter_mem_p->a = &(temp_coeff[offset]);
	offset += filter_mem_p->sig_filter_size;
	filter_mem_p->b = &(temp_coeff[offset]);
	offset += filter_mem_p->sig_filter_size;
	filter_mem_p->y = &(temp_coeff[offset]);
	offset += filter_mem_p->sig_filter_size;
	filter_mem_p->x = &(temp_coeff[offset]);

	for(i=0; i<filter_mem_p->sig_filter_size; i++)
	{
		(filter_mem_p->a)[i] = 0.0;
		(filter_mem_p->b)[i] = 0.0;
		(filter_mem_p->y)[i] = 0.0;
		(filter_mem_p->x)[i] = 0.0;
	}

#ifdef SIG_FILTER_DEFAULT_BUTTER
		filter_mem_p->sig_filter_idx = FILTER_DEFAULT_SIZE;
		filter_mem_p->sig_filter_size = FILTER_DEFAULT_SIZE;
		(filter_mem_p->a)[0] = 1;	
		(filter_mem_p->a)[1] = -1.957798683495732;
		(filter_mem_p->a)[2] = 0.958670943992033;

		(filter_mem_p->b)[0] = 0.000218065124075;
		(filter_mem_p->b)[1] = 0.00043613024815;
		(filter_mem_p->b)[2] = 0.000218065124075;
#endif // SIG_FILTER_DEFAULT_BUTTER
}



/**
* sig_test() - tests Signal Processing operation (e.g.)
*/
uint8_t sig_test()
{

}


/**
  * sig_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void sig_loop( void );


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/**
* sig_filter_new_data(input) - 
*	
*/
void sig_filter_reset(sig_filter_mem_t xdata *filter);

/**
* sig_filter_new_config(module) - 
*	
*/
void sig_filter_new_config(sig_filter_mem_t xdata *filter, uint8_t * module)
{
}

/**
* sig_filter_new_data(input) - 
*	
*/
float sig_filter_new_data( sig_filter_mem_t xdata *filter, float input )
{
	float new_y = B[1] * input;
	X[idx%] = input;
    for(i=2; i<3; i++)
	{
		new_y = new_y + X(idx-i+1)* B(i) - Y(idx-i+1)*A(i);
	}
	Y[idx] = new_y;
}


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################



#endif // _ENABLE_SIG_BASIC_

