#include "defs/esnl_pub.h"

#include "timer.h"
#include PLATFORM_HEADER

timer_struct time_val;

void timer_init()
{
  TIM_TimeBaseInitTypeDef init;
  uint32_t interrupts;
  uint8_t * priority;
  uint8_t tim1_pri;
  
  init.TIM_Prescaler = 0x0F; //Prescale by 2^15
  init.TIM_CounterMode = TIM_CounterMode_Up;
  init.TIM_Period = 366; //2 second for 6MHz Peripheral Clk
                         //1 second for 12MHz Peripheral Clk
  
  TIM_TimeBaseInit( TIM1, &init );
  TIM1->EGR = 0x00000000;
  TIM_UpdateDisableConfig( TIM1, DISABLE );               //Enables update event on timer1 overflow.
  TIM_SelectExternalTriggerClock( TIM1, TIM_EXTRIGPCLK ); //Selecting Peripheral clock as the source 
  TIM_UpdateRequestConfig( TIM1, TIM_UpdateSource_Global );
  time_val.current_time = 0;
  
  interrupts = *(NVIC->ISER);
  interrupts = interrupts | (1 << 16);
  *(NVIC->ISER) = interrupts;   //Enable NVIC interrupt for TIM1
  //priority = NVIC->IP;
  NVIC->IP[16] = 10; //Set the interrupt priority for TIM1 to 10.
  
  TIM_ITConfig( TIM1_IT, TIM_IT_Trigger, ENABLE );  //Enables timer interrupt for timer1.
  TIM_ITConfig( TIM1_IT, TIM_IT_Update, ENABLE );  //Enables timer interrupt for timer1.
 
  TIM_ARRPreloadConfig( TIM1, ENABLE );
  TIM_Cmd( TIM1, ENABLE );

  
  
  /*********************************************/
  /* DEBUGGING                                 */
  /*********************************************/  
      GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    time_val.flag = 0;
  
}
	
void timer_start()
{
    TIM_SetCounter( TIM1, 0x00000000 );               //Reset the counter.
    
    GPIO_ResetBits(GPIOC, GPIO_Pin_0);
}


void timer_stop()
{
    TIM_ITConfig( TIM1_IT, TIM_IT_Update, DISABLE );        //Disables timer interrupt for timer1.
}

void timer_set_frame_size()
{
  
}


//Never getting into this function.
//UIF bit in TIM1_ISR is being set, but not interrupt is generated.
//URS bit in TIM1_CR1 is '0'
//This is based on interrupts in stm32w108xx_it.c
void TIM1_IRQHandler( void )
{
    if( time_val.flag == 1 )
    {
        time_val.flag = 0;
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
    }
    else
    {
        time_val.flag = 1;
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
    }
    
}

