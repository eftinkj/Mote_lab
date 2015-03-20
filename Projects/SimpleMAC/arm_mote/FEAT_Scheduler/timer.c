#include "defs/esnl_pub.h"

#include "timer.h"

timer_struct time_val;

void timer_init()
{
  TIM_TimeBaseInitTypeDef init;
  
  init.TIM_Prescaler = 0x0F; //Prescale by 2^15
  init.TIM_CounterMode = TIM_CounterMode_Up;
  init.TIM_Period = 366; //2 second for 6MHz Peripheral Clk
                         //1 second for 12MHz Peripheral Clk
  
  TIM_TimeBaseInit( TIM1, &init );
  TIM_UpdateDisableConfig( TIM1, DISABLE );               //Enables update event on timer1 overflow.
  TIM_SelectExternalTriggerClock( TIM1, TIM_EXTRIGPCLK ); //Selecting Peripheral clock as the source 
  TIM_UpdateRequestConfig( TIM1, TIM_UpdateSource_Regular );
  time_val.current_time = 0;
  
  
  /*********************************************/
  /* DEBUGGING                                 */
  /*********************************************/  
      GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    time_val.flag = 0;
  
}
	
void timer_start()
{
    TIM_SetCounter( TIM1, 0x00000000 );               //Reset the counter.
    TIM_ITConfig( TIM1_IT, TIM_IT_Trigger, ENABLE );  //Enables timer interrupt for timer1.
}


void timer_stop()
{
    TIM_ITConfig( TIM1_IT, TIM_IT_Trigger, DISABLE );        //Disables timer interrupt for timer1.
}

void timer_set_frame_size()
{
  
}

void TIM1_IRQHandler()
{
    if( time_val.flag )
    {
        time_val.flag = 0;
        GPIO_ResetBits(GPIOC, GPIO_Pin_4);
    }
    else
    {
        time_val.flag = 1;
        GPIO_SetBits(GPIOC, GPIO_Pin_4);
    }
    
}

