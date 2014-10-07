/**
  ******************************************************************************
  * @file    timer.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   This file provides the timer support for RF4CE
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

/** @addtogroup Shared
  * @{
  */

/** @addtogroup timer
  * @{
  */
  
/* Private define ------------------------------------------------------------*/

#ifdef PCWIN32
#include <sys/time.h> 
static struct timeval init;
static struct timezone tz;
#else
#include PLATFORM_HEADER
#include "Utilities/STM32W108xx_HAL_Driver/hal.h"
#include "error.h"
#include "phy-library.h"
#include "timer.h"
static uint32_t init;
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static uint32_t timeCur=0;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ----------------------------------------------------------*/

/** @defgroup  timer_Private_Functions
  * @{
  */
  
#if 0

/**
  * @brief  indicates current time.
  * @param  value of the current time.
  * @retval unsigned value current time.
  */
static uint32_t TIME_Current(struct timeval *current)
{
  struct timeval diff;

  diff.tv_sec = current->tv_sec - init.tv_sec;
  diff.tv_usec = current->tv_usec - init.tv_usec;
  
  if (diff.tv_usec < 0) {
    diff.tv_usec += 1000000;
    diff.tv_sec--;
  }
  return (diff.tv_usec + (diff.tv_sec * 1000000));
}
#endif

/**
  * @brief  indicates current time.
  * @param  None
  * @retval unsigned value current time.
  */
uint32_t TIME_CurrentTime()
{
  TIMER_Tick();
  return timeCur;
}

/**
  * @brief  calculates elapsed time.
  * @param  start time
  * @retval elapsed time.
  */
uint32_t TIME_ELAPSED(uint32_t startTime)
{
  return TIME_CurrentTime() - startTime;
}

/**
  * @brief  Initializes the timer.
  * @param  None
  * @retval None.
  */
void TIMER_Init()
{
#ifdef PCWIN32
  gettimeofday(&init, &tz);
#else
  init = ST_RadioGetMacTimer();
#endif
  return;
}

/**
  * @brief  timer tick.
  * @param  None
  * @retval None.
  */
void TIMER_Tick(void) {
#ifndef PCWIN32
  uint32_t diff, newTime;

  newTime = ST_RadioGetMacTimer();
  if (init<newTime)
    diff = newTime - init;
  else
    diff = newTime + ((1 << 20) - init); /* 1048576 = 2^20 usec ~1 sec */

  timeCur += diff;
#else
  struct timeval diff, newTime;
  gettimeofday(&newTime, &tz);

  diff.tv_sec = newTime.tv_sec - init.tv_sec;
  diff.tv_usec = newTime.tv_usec - init.tv_usec;
  
  if (diff.tv_usec < 0) {
    diff.tv_usec += 1000000;
    diff.tv_sec--;
  }
  timeCur += diff.tv_usec + (diff.tv_sec * 1000000);
#endif
  init = newTime;
  return;

}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */ 
  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
