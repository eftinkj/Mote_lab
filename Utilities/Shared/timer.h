/**
  ******************************************************************************
  * @file    timer.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Timer header file. This timer can use SysTick
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
  
#ifndef TIMER_H
#define TIMER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup Shared
  * @{
  */

/** @addtogroup timer
  * @{
  */ 
  
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/** @defgroup  timer_Private_Function_Prototypes
  * @{
  */
  
/**
  * @brief  indicates current time.
  * @param  None
  * @retval unsigned value current time.
  */
uint32_t TIME_CurrentTime(void);

/**
  * @brief  calculates elapsed time.
  * @param  start time
  * @retval elapsed time.
  */
uint32_t TIME_ELAPSED(uint32_t startTime);

/**
  * @brief  Initializes the timer.
  * @param  None
  * @retval None.
  */
void TIMER_Init(void);

/**
  * @brief  timer tick.
  * @param  None
  * @retval None.
  */
void TIMER_Tick(void);

#endif /* TIMER_H */

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
