#include "defs/esnl_pub.h"

#ifdef _ENABLE_SCH_BASIC_

#include "sch_basic_prv.h"
#include "sch_basic_pub.h"


//#include "API_frames.h"
//#include "tsp_common_pub.h"
//#include "Application.h"

#include "sys_func.h"


//#define _SCH_DEBUG_ENABLE_
#undef _SCH_DEBUG_ENABLE_
#define _SCH_DEBUG_LEVEL_

#include "stm32w108xx_gpio.h"
#include "..\HW_LIB\RTC.h"

#define P_CLK_FREQ 12000000 //12 MHz



/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

//extern uint8_t		truck_state_test;
rtc_tick_t	XDATA	sch_timeout_ticks[MAX_TIMEOUTS];
uint8_t		XDATA	sch_timeout_state[MAX_TIMEOUTS];
list_size_t	XDATA	sch_timeout_count;

list_index_t	XDATA	sch_timeout_order[MAX_TIMEOUTS];
list_index_t	XDATA	sch_tout_head;
list_index_t	XDATA	sch_tout_free;
sch_cb_func_t	XDATA	sch_callback_funcs[MAX_TIMEOUTS];
uint8_t XDATA *	XDATA	sch_callback_context[MAX_TIMEOUTS];
char *	sch_callback_name[MAX_TIMEOUTS];

//sch_loop_func_t	xdata	sch_loop_funcs[MAX_LOOPS];
sch_loop_func_t sch_loop_funcs[MAX_LOOPS];
uint8_t sch_loop_funcs_on[MAX_LOOPS];
uint32_t start_time_diff[MAX_TIMEOUTS];
uint32_t end_time_diff[MAX_TIMEOUTS];
uint32_t dummy = 0;



/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/

void sch_default_callback( uint8_t tid)
#ifndef _TEMP_ARM_
	TASK_FUNC
#endif // _TEMP_ARM_
{
	//Do default handling HERE
}

/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

#define SCH_FUNC_OFF	0
#define SCH_FUNC_ON		1

char str_NONE[] = "NONE";

/**
  * sch_power_up () - power up (tasks/system) SCHEDULING module
  */
void sch_power_up ( void )
{
	uint8_t i;
    
	//sch_timeout_ticks[MAX_TIMEOUTS];
	for(i=0; i< MAX_TIMEOUTS; i++)
	{
		sch_timeout_state[i] = SCH_STATE_IDLE;
		sch_timeout_order[i] = (i+1)%MAX_TIMEOUTS;
		sch_callback_funcs[i] = (sch_cb_func_t)NULL; //sch_default_callback;
		sch_callback_name[i] = str_NONE;
                start_time_diff[i] = 0;
                end_time_diff[i] = 0;
	}
	sch_timeout_order[MAX_TIMEOUTS - 1] = SCH_NO_TIMEOUT_ID; // last points to nothing
	sch_timeout_count = MAX_TIMEOUTS; // initially all empty
	sch_tout_head = SCH_NO_TIMEOUT_ID; // empty
	sch_tout_free = 0; // first empty ID



	for(i=0; i< MAX_LOOPS; i++)
	{
		sch_loop_funcs[i] = SCH_NO_FUNC_ID;
		sch_loop_funcs_on[i] = SCH_FUNC_OFF;
	}
	sch_timeout_count = 0; // empty

    
    
}

/**
  * sch_init () - sets up (tasks/system) SCHEDULING module
  */
void sch_init ( void )
{
	// Any initializations - e.g. periodic scheduler job/task

}



/**
* sch_test() - tests SCHEDULING operation (e.g. RTC correctness)
*/
uint8_t sch_test()
{
	return 0;
}

uint8_t current_loop_idx;
uint8_t timeout_idx;
/**
  * sch_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void sch_loop( void )
{


    
    for(current_loop_idx=0; current_loop_idx< MAX_LOOPS; current_loop_idx++)
	{
            
        if (sch_loop_funcs_on[current_loop_idx] == SCH_FUNC_ON)
		{
			(sch_loop_funcs[current_loop_idx])();			
		}      
            
	}
    


	while ( (SCH_NO_TIMEOUT_ID != sch_tout_head )
		&& (sch_timeout_ticks[sch_tout_head ] < rtc_get_ticks()) )
	{

        // Remove from head for the purpose of consistency
		ATOMIC(
			   	timeout_idx = sch_tout_head ;
				sch_tout_head  = sch_timeout_order[sch_tout_head ];
				sch_timeout_order[timeout_idx] = 0xEE;//sch_tout_free;
				sch_timeout_count--;
				)
		// Execute
                start_time_diff[timeout_idx] = rtc_get_ticks() - sch_timeout_ticks[timeout_idx];
		(sch_callback_funcs[timeout_idx ])(sch_callback_context[timeout_idx] );
		// Free the Timer slot for others to use
		sch_timeout_state[timeout_idx] = SCH_STATE_IDLE;
                end_time_diff[timeout_idx] = rtc_get_ticks() - sch_timeout_ticks[timeout_idx];
      
	}
    
        if(SCH_NO_TIMEOUT_ID != sch_tout_head)
        {
            dummy++;
        }
    
}


void sch_remove_timeout(uint8_t tidx, char*name)
{
	uint8_t temp = sch_tout_head;
	if (SCH_NO_TIMEOUT_ID == temp) {printf("ERROR DELETING TO 255\n"); return;} // not found anything (maybe JUST executed)
	//printf("SCH_TO_DEL= %d,%s (r=%s)\n\d", temp, sch_callback_name[temp], name);
	//print_timeouts();
	if (  sch_callback_name[tidx] != name ) 
	{
		printf("ERROR DELETING wrong name %d,%s (req=%s)\n", temp, sch_callback_name[temp], name); 
		return; // wrong removal
	}
	sch_timeout_count--;
	if (tidx == temp) // TIDX is the HEAD
	{
		sch_tout_head = sch_timeout_order[tidx]; // remove timeout from ORDERED list
		sch_timeout_order[tidx] = 0xEE;//sch_tout_free;
		sch_timeout_state[tidx] = SCH_STATE_IDLE; // free timeout record
		return;
	}
	// Find the previous item in the list to skip/remove the TIDX
	while (SCH_NO_TIMEOUT_ID != sch_timeout_order[temp])
	{
		if ( tidx == sch_timeout_order[temp])
		{
		// Remove from the list
			//		ATOMIC(
			   	sch_timeout_order[temp] = sch_timeout_order[tidx]; // skip the TIDX
				sch_timeout_order[tidx] = 0xEE;//sch_tout_free;
				sch_timeout_state[tidx] = SCH_STATE_IDLE; // free timeout record
//				)
				return;
		}
		temp = sch_timeout_order[temp]; // go to next
	}
	// NOT FOUND
	sch_timeout_count++; // revert to original count
}

/**
  * sch_correct_time_shift (offset) - updates schedule timeouts to accomodate
  *		for the RTC shift due to clock (re-)synchronization (e.g. from BEAM pkt)
  */
void sch_correct_time_shift ( int32_t offset)
{
	// TODO: handle case when the RTC has been set to a new value (large change)
	// 		this will require shift in all timeouts
}

/**
  * sch_create_timeout (timeout, callback_func) - sets a new timeout at "timoeut"
  *		RTC ticks (absolute value). When timeout expires, the "callback_func" is
  *		executed (function has to be REENTRANT and accept timeout ID as param)
  *	RETURNS: timeout ID or "SCH_NO_TIMEOUT_ID" if unsuccesful
  */
uint8_t sch_create_timeout( rtc_tick_t timeout, sch_cb_func_t callback_func, uint8_t* t_context, char*name)
{
	list_index_t tidx;
	list_index_t order_idx = sch_tout_head;
	// TODO: find a free timeout ID
	tidx = get_free_timeout();
// DEBUG
	printf("SCH_TO_SET= %d,%s #\n", tidx, name);
	if (SCH_NO_TIMEOUT_ID == tidx)
	{
		return SCH_NO_TIMEOUT_ID;
	}
	//		then, set the timeout
	sch_timeout_count++;
	sch_timeout_ticks[tidx] = timeout;
	sch_timeout_state[tidx] = SCH_STATE_PENDING;
	sch_callback_funcs[tidx] = callback_func;
	sch_callback_context[tidx] = t_context;
	sch_callback_name[tidx] = name;

	//		then, insert into the timeouts' list
	if ((SCH_NO_TIMEOUT_ID == sch_tout_head) || (timeout < sch_timeout_ticks[sch_tout_head]))
	{
		sch_timeout_order[tidx] = sch_tout_head; // put as first
		sch_tout_head = tidx;
	}
	else
	{
		// Until the current item is last (next is empty)
		//		OR until next item in the list is later than the new timeout
		while ((SCH_NO_TIMEOUT_ID != sch_timeout_order[order_idx])
			   && (timeout > sch_timeout_ticks[sch_timeout_order[order_idx]]))
		{
			order_idx = sch_timeout_order[order_idx];
		}
		sch_timeout_order[tidx] = sch_timeout_order[order_idx]; // inherit the next in list
		sch_timeout_order[order_idx] = tidx; // connect to the previous item (earlier event)
		
	}
	return tidx;
}




/**
  * sch_add_loop( sch_loop_func_t loop_func) - sets a new loop function to
  *		be executed every time in the main loop
  *	RETURNS: loop function ID or "SCH_NO_FUNC_ID" if unsuccesful
  */
uint8_t sch_add_loop( sch_loop_func_t loop_func)
{
	uint8_t i = SCH_NO_TIMEOUT_ID;
	for( i= 0; i < MAX_LOOPS; i++)
	{
		if (sch_loop_funcs_on[i] == SCH_FUNC_OFF)
		{
			break;
		}
	}
	if (MAX_LOOPS > i)
	{
		sch_loop_funcs[i] = loop_func;
		sch_loop_funcs_on[i] = SCH_FUNC_ON;
		return i;
	}
	// else not found free space
	return 	SCH_NO_TIMEOUT_ID;
}

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/**
*  sch_init_timeout_list () - (re)sets the timeout list
*/
void sch_init_timeout_list()
{

}

/**
*  get_free_timeout () - finds free slot in timeout tables,
*       RETURN: "index", or SCH_NO_TIMEOUT_ID if no space left
*		NOTE: Sets state of timer to SCH_STATE_BUSY
*/
list_index_t get_free_timeout()
{
	list_index_t i;
	for(i=0; i < MAX_TIMEOUTS; i++)
	{
		if ( SCH_STATE_IDLE == sch_timeout_state[i] )
		{
			sch_timeout_state[i]= SCH_STATE_BUSY;
			return i;
		}
	}
	printf("NO TIMEOUTS LEFT = %d\n", sch_timeout_count);
	return SCH_NO_TIMEOUT_ID;
}

/**
*  print_timeouts () - prints state of the timeout table,
*       uses STDOUT (mostly serial interface UART)
*/
void print_timeouts()
{
	list_index_t i;
	for(i=0; i < MAX_TIMEOUTS-20; i++)
	{
		printf(" %d/%d,/%s\n" 
			   , sch_timeout_state[i]
				   , sch_timeout_order[i]
				   , sch_callback_name[i]);
	}	
}

// ############################################################################
// ############################################################################




// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################

timer_struct time_val;

void timer_init()
{
  //TIM_TimeBaseInitTypeDef init;
  uint32_t interrupts;
  uint8_t * priority;
  uint8_t tim1_pri;
  
  //interrupts = *(NVIC->ISER);
  /*interrupts = NVIC->ISER[0];
  interrupts = interrupts | (1 << 16);
  NVIC->ISER[0] = interrupts;   //Enable NVIC interrupt for TIM1
  NVIC->IP[16] = 10; //Set the interrupt priority for TIM1 to 10.
  
  TIM1_IT->IER = TIM1_IT->IER | 0x01;    //Enable Update Interrupt.
  TIM1->SMCR = TIM1->SMCR & 0xFFFFFFF8; //Select Peripheral Clk
  TIM1->ARR = 366;
  TIM1->PSC = TIM1->PSC | 0x0F; //Set prescalar to 15
  TIM1->CR1 = TIM1->CR1 & 0xFFFFFF81; //Clear CMS, DIR, OPM, URS, and UDIS bits
  TIM1->CR1 = TIM1->CR1 | 0x00000081; //Set ARPE and CEN bits.
  TIM1->CNT = 0x0000;                 //Initialize the count to 0.
  TIM1->EGR = TIM1->EGR | 0x01;       //Update the shadow registers (ARR in particular)
  */
  time_val.current_time = 0;
  //Peripheral Clock running at ~12 MHZ
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  NVIC_InitTypeDef Init;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Prescaler = 15;
  TIM_TimeBaseInitStruct.TIM_Period = 366;
  TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
  TIM_PrescalerConfig(TIM1, 15, TIM_PSCReloadMode_Immediate);
  TIM_ITConfig(TIM1_IT, TIM_IT_Update, ENABLE);
  TIM_UpdateRequestConfig(TIM1, TIM_UpdateSource_Global);
  TIM_UpdateDisableConfig(TIM1, DISABLE);
  TIM_Cmd(TIM1, ENABLE);
  Init.NVIC_IRQChannel = TIM1_IRQn;
  Init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&Init);
  
  
  
  
  
  /*********************************************/
  /* DEBUGGING                                 */
  /*********************************************/  
      GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    time_val.flag = 0;
  
}
	
void timer_start()
{
    //TIM_SetCounter( TIM1, 0x00000000 );               //Reset the counter.
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_6);
}


void timer_stop()
{
    TIM_ITConfig( TIM1_IT, TIM_IT_Update, DISABLE );        //Disables timer interrupt for timer1.
}

void timer_set_frame_size()
{
  
}


//Never getting into this function.
//UIF bit in TIM1_ISR is being set, but no interrupt is generated.
//URS bit in TIM1_CR1 is '0'
//This is based on interrupts in stm32w108xx_it.c
void TIM1_IRQHandler( void )
{
    TIM_ClearITPendingBit( TIM1_IT, TIM_IT_Update );
    if( time_val.flag == 1 )
    {
        time_val.flag = 0;
        GPIO_ResetBits(GPIOA, GPIO_Pin_6);
    }
    else
    {
        time_val.flag = 1;
        GPIO_SetBits(GPIOA, GPIO_Pin_6);
    }
    
}


#endif // _ENABLE_NEM_UTILITIES_01_

