#ifndef _SCH_BASIC_PRV_H_
#define _SCH_BASIC_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
//#include "rtc.h"
#include "sch_basic_pub.h"


/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/

#define MAX_LOOPS		10

#define MAX_TIMEOUTS	30

// State constants for timeouts states
	// idle - not used/free
#define SCH_STATE_IDLE	0
	// pending - the timeout is active (waiting)
#define SCH_STATE_PENDING	1
	// expired - the timeout expired and was executed (should be released quickly)
#define SCH_STATE_EXPIRED	2
	// Busy - the timer is reserved for use but not set yet
#define SCH_STATE_BUSY		3


/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

//extern uint8_t		truck_state_test;
extern rtc_tick_t	XDATA	sch_timeout_ticks[MAX_TIMEOUTS];
extern uint8_t		XDATA	sch_timeout_state[MAX_TIMEOUTS];
extern list_size_t	XDATA	sch_timeout_count;

extern list_index_t	XDATA	sch_timeout_order[MAX_TIMEOUTS];
extern list_index_t	XDATA	sch_tout_head;
extern list_index_t	XDATA	sch_tout_free;
extern sch_cb_func_t	XDATA	sch_callback_funcs[MAX_TIMEOUTS];


extern sch_loop_func_t	sch_loop_funcs[MAX_LOOPS];

//code unsigned (*func_table []) (int value) = {  func_a,  func_b,  };

/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/



/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/



/**
*  sch_init_timeout_list () - (re)sets the timeout list
*/
void sch_init_timeout_list( void );



/**
*  get_free_timeout () - finds free slot in timeout tables,
*       RETURN: "index", or SCH_NO_TIMEOUT_ID if no space left
*/
list_index_t get_free_timeout( void );


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _SCH_BASIC_PRV_H_
