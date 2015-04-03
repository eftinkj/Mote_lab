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
#define FALSE 0
#define TRUE 1
#define US_PER_S 1000000 // 1 Million us per s



/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

static uint64_t s_cur_time;
static uint32_t s_frame_size;

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

uint8_t current_loop_idx;
uint8_t timeout_idx;
Task array[MAX_LOOPS] = { 0 };

uint64_t convert_us_ticks( uint32_t us ); //convert microseconds to ticks
uint64_t convert_ticks_us( uint32_t ticks ); //convert ticks to microseconds


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



/**
  * sch_loop() - executes main loop block (BUT DOES NOT LOOP ITSELF!!!)
  */
void sch_loop( void )
{

//only call the jobs that have been released
//periodic, sporadic, and aperiodic
uint8_t i; //Loop index var

    
    for(i=0; i< MAX_LOOPS; i++)
	{
            
        if (array[i].validJob == SCH_FUNC_ON)
		{
			if(array[i].ready == true)
            {
                ATOMIC
                (
                array[i].running = true;
                array[i].ready = false;
                )
                (array[i].toExecute)();	
                ATOMIC( array[i].running = false; )
            }
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
uint8_t sch_add_loop( sch_loop_func_t loop_func, uint32_t period) //period is in microseconds
{
    
	uint8_t i = SCH_NO_TIMEOUT_ID;
        bool done;
        
	for( i= 0; i < MAX_LOOPS; i++)
	{
		if (array[i].validJob == SCH_FUNC_OFF)
		{
			break;
		}
	}
    

    done = false;
    
    if( i != 0 )
    {
        
        for (i; i>0 && !done ; i--)
        {
          if (array[i-1].period > period)
            array[i] = array[i-1];
          else
            done = true;
          
        }
    }
    
    if( done )
        i++;
    
	if (MAX_LOOPS > i)
	{
                array[i].releaseTime = 0;
                array[i].period = period;
                array[i].deadlineAbsolute = 100000000;
                array[i].running = false;
                array[i].ready = false;
                array[i].validJob = true;
                array[i].toExecute = loop_func;
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
  uint8_t i;
  
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
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_Period = 720;  //Initialize frame size to 60us.
  TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
  TIM_PrescalerConfig(TIM1, 0, TIM_PSCReloadMode_Immediate);
  TIM_ITConfig(TIM1_IT, TIM_IT_Update, ENABLE);
  TIM_UpdateRequestConfig(TIM1, TIM_UpdateSource_Global);
  TIM_UpdateDisableConfig(TIM1, DISABLE);
  TIM_Cmd(TIM1, ENABLE);
  Init.NVIC_IRQChannel = TIM1_IRQn;
  Init.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&Init);
  
  timer_set_frame_size( 500 );
  s_cur_time = 0;
  s_frame_size = 500; //Frame size was initialized to 60us

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

//Set the frame size in microseconds.
void timer_set_frame_size(uint32_t frame_sz)
{
    uint32_t psc; //Prescalar
    uint32_t frame; //Temp frame size
    uint64_t tmp;
    uint8_t i;    //Loop index var.
    uint8_t done; 
    
    tmp = (uint64_t)frame_sz * P_CLK_FREQ / US_PER_S;
    done = FALSE;
    psc = 0;
    frame = frame_sz;
    
    
    while( psc < 17 && done != TRUE )
    {
        if( tmp > 65535 )
        {
            psc = psc + 1;
            tmp = tmp >> 1;
        }
        else
        {
            done = TRUE;
        }   
    }
    
    if( psc > 15 ) //If the frame size given was too large, do nothing.
    {
        return;
    }
    
    TIM1->PSC = psc;
    TIM1->ARR = tmp;
    s_frame_size = frame_sz;
    
    return;
}

//Returns the number of us left in the frame.
uint32_t get_rem_frame_time()
{
    uint32_t count;
    uint32_t rem_count;
    uint32_t rem_time;
    
    count = TIM1->CNT;
    rem_count = TIM1->ARR - count;
    rem_time = (uint32_t)convert_ticks_us( rem_count );
    
    return rem_time;
}

//Returns the amount of time the current frame has been executing
uint32_t get_exe_frame_time()
{
    uint32_t count;
    uint32_t exe_time;
    
    count = TIM1->CNT;
    exe_time = (uint32_t)convert_ticks_us( count );
    
    return exe_time;
}

uint64_t get_current_time()
{
    return( s_cur_time + get_exe_frame_time() );
}

//Never getting into this function.
//UIF bit in TIM1_ISR is being set, but no interrupt is generated.
//URS bit in TIM1_CR1 is '0'
//This is based on interrupts in stm32w108xx_it.c
void TIM1_IRQHandler( void )
{
    uint8_t i;  //loop index var
    
    //Clear interrupt flag:
    TIM_ClearITPendingBit( TIM1_IT, TIM_IT_Update );
    
    //Toggle pin A6 (for debugging)
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
    
    //Increment current time:
    s_cur_time = s_cur_time + s_frame_size;
    
    //Test for jobs that are currently running or are past their deadline:
    for( i = 0; i < MAX_LOOPS; i++)
    {
        if( array[i].validJob == true )
        {
            if( array[i].ready == true && array[i].deadlineAbsolute < s_cur_time ) //If the job was not executed in time:
            {
                while(1); //Hold indefinitely
            }
            
            if( array[i].running == true ) //If the job was interrupted while it was running:
                while(1); //Hold indefinitely
            
        }
    }
    
    //Release periodic jobs:
    for( i = 0; i < MAX_LOOPS; i++ )
    {
        if( array[i].validJob == true )
        {
            if( array[i].releaseTime < s_cur_time )
            {
                array[i].ready = true;
                array[i].releaseTime = s_cur_time + array[i].period;
                array[i].deadlineAbsolute = array[i].releaseTime;
            }
        }
        
    }
    
    
    
}

//convert microseconds to ticks
uint64_t convert_us_ticks( uint32_t us ) 
{
    uint32_t psc;
    uint32_t ticks;

    psc = TIM1->PSC;
    ticks = us * P_CLK_FREQ / US_PER_S;
    ticks = ticks >> psc;
    
    return ticks;
}

//convert ticks to microseconds
uint64_t convert_ticks_us( uint32_t ticks ) 
{
    uint32_t psc;
    uint32_t us;
    
    psc = TIM1->PSC;
    us = ticks * US_PER_S / P_CLK_FREQ;
    us = us << psc;
    
    return us;
}

#endif // _ENABLE_NEM_UTILITIES_01_

