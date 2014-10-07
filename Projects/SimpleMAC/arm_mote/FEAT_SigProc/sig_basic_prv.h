#ifndef _SIG_BASIC_PRV_H_
#define _SIG_BASIC_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
//#include "rtc.h"
#include "sig_basic_pub.h"


#define SIG_FILTER_TYPE_BUTTER

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/



typedef struct
{
	uint8_t sig_filter_idx;
	uint8_t sig_filter_size;
	uint8_t sig_filter_type;
	float xdata *a;
	float xdata *b;
	float xdata *y;
	float xdata *x;
	uint8_t dynamic; // pointer to the rest of memory
}  PACKED sig_filter_mem_t;

typedef struct
{
	uint8_t module_type;
	uint8_t module_size;
	uint8_t message_type;
	uint8_t filter_size;
	float coeff;
}  PACKED sig_filter_mod_t;


//#define FLA_MEM_STRUCT_OFFSET_BUFF	sizeof(void xdata*)
#define SIG_FILTER_MEM_STRUCT_SIZE			sizeof(sig_filter_mem_t)
// Depth of the filter (in Z^(-1) steps)
#define SIG_FILTER_Z_BUFF_SIZE			6

#define SIG_FILTER_MEM_SIZE	(SIG_FILTER_MEM_STRUCT_SIZE - 1 + 4 * SIG_FILTER_Z_BUFF_SIZE)

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/


/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
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
void sig_filter_new_config(sig_filter_mem_t xdata *filter, uint8_t * module);

/**
* sig_filter_new_data(input) - 
*	
*/
float sig_filter_new_data( sig_filter_mem_t xdata *filter, float input );


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/



// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _FLA_BASIC_PRV_H_