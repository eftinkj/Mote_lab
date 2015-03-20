#ifndef _TIMER_H_
#define _TIMER_H_

#include "stm32w108xx_tim.h"

typedef struct
{
    uint32_t       current_time;
    uint8_t        flag;
} timer_struct;

void timer_init();
void timer_start();
void timer_stop();
void timer_set_frame_size();
extern void TIM1_IRQHandler();

#endif // _TIMER_H_