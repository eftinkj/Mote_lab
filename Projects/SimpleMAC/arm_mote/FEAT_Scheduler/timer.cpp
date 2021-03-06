#include "defs/esnl_pub.h"

#include "timer.h"

void timer_init()
{
  TIM_TimeBaseInitTypeDef init;
  
  init.TIM_Prescalar = 0x0F; //Prescale by 2^15
  init.TIM_CounterMode = TIM_CounterMode_Up;
  init.TIM_Period = 366; //2 second for 6MHz Peripheral Clk
                         //1 second for 12MHz Peripheral Clk
  
  TIM_TimerBaseInit( TIM1, &init );
  TIM_UpdateDisableConfig( TIM1, DISABLE );               //Enables update event on timer1 overflow.
  TIM_SelectExternalTriggerClock( TIM1, TIM_EXTRIGPCLK ); //Selecting Peripheral clock as the source 
  TIM_UpdateRequestConfig( TIM1, TIM_UpdateSource_Regular );
  
  
  /*********************************************/
  /* DEBUGGING                                 */
  /*********************************************/  
      GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIOB->BSR = GPIO_Pin_3; // Set pin (1)
	//GPIOB->BRR = GPIO_Pin_3; // Reset pin (0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    flag = 0;
  
}
	
void timer::start()
{
    TIM_SetCounter( TIM1, 0x00000000 );               //Reset the counter.
    TIM_ITConfig( TIM1_IT, TIM_IT_Trigger, ENABLE );  //Enables timer interrupt for timer1.
}


void timer::stop()
{
    TIM_ITConfig( TIM1_IT, TIM_IT_Trigger, DISABLE );        //Disables timer interrupt for timer1.
}

void set_frame_size()
{
  
}

void TIM1_IRQHandler()
{
    if( flag )
    {
        flag = 0;
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
    }
    else
    {
        flag = 1;
        GPIO_SetBits(GPIOB, GPIO_Pin_3);
    }
    
}

