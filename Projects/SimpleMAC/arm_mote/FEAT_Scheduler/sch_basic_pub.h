#ifndef _SCH_BASIC_PUB_H_
#define _SCH_BASIC_PUB_H_

/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "HW_LIB/rtc.h"

/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/
#define SCH_NO_TIMEOUT_ID	255
#define SCH_NO_FUNC_ID	0


// loop function
//typedef void (code *sch_loop_func_t)(void);
#ifdef __SDCC__
	typedef CODE void ( * XDATA sch_loop_func_t)(void);
#else
	typedef void ( * code sch_loop_func_t)(void);
#endif // not _SDCC_

#ifdef __SDCC__
	typedef CODE void ( * XDATA sch_cb_func_t)(uint8_t XDATA *);// TASK_FUNC;
#else
	typedef void ( * code sch_cb_func_t)(uint8_t XDATA *) TASK_FUNC;
#endif // not _SDCC_


//code unsigned (*func_table []) (int value) = {  func_a,  func_b,  };

// callback function for timeouts
//typedef code void (* xdata sch_cb_func_t)(uint8_t tid) TASK_FUNC;
typedef uint8_t list_index_t;	// index in a queue tables
typedef uint8_t list_size_t;	// variable to store size of a table/buffer/queue


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
  * sch_power_up () - power up (tasks/system) SCHEDULING module
  */
void sch_power_up ( void );


/**
  * sch_init () - sets up (tasks/system) SCHEDULING module
  */
void sch_init ( void );



/**
* sch_test() - tests SCHEDULING operation (e.g. RTC correctness)
*/
uint8_t sch_test( void );


/**
  * sch_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void sch_loop( void );


/**
  * sch_correct_time_shift (offset) - updates schedule timeouts to accomodate
  *		for the RTC shift due to clock (re-)synchronization (e.g. from BEAM pkt)
  */
void sch_correct_time_shift ( int32_t offset);

/**
  * sch_create_timeout (timeout, callback_func) - sets a new timeout at "timoeut"
  *		RTC ticks (absolute value). When timeout expires, the "callback_func" is
  *		executed (function has to be REENTRANT and accept timeout ID as param)
  * 	optionally passes name
  *	RETURNS: timeout ID or "SCH_NO_TIMEOUT_ID" if unsuccesful
  */
uint8_t sch_create_timeout( rtc_tick_t timeout, sch_cb_func_t callback_func, uint8_t *t_context, char* name);

/**
  * sch_remove_timeout (timeout_ID) - removes a timeout from the execution list
  *		(Cancels that timout)
  *	RETURNS: NOTHING
  */
void sch_remove_timeout(uint8_t tidx, char*name);

/**
  * sch_add_loop( sch_loop_func_t loop_func) - sets a new loop function to
  *		be executed every time in the main loop
  *	RETURNS: loop function ID or "SCH_NO_FUNC_ID" if unsuccesful
  */
uint8_t sch_add_loop( sch_loop_func_t loop_func);



/**
*  print_timeouts () - prints state of the timeout table,
*       uses STDOUT (mostly serial interface UART)
*/
void print_timeouts();

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################


#endif // _SCH_BASIC_PUB_H_


