/**
  ******************************************************************************
  * @file    system-timer.h 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Header file for system_timer APIs.
  * @verbatim
  *
 ===============================================================================
                           ##### system-timer #####
 ===============================================================================
  *
  * Functions that provide access to the system clock.
  *
  * A single system tick (as returned by ::halCommonGetInt16uMillisecondTick() and
  * ::halCommonGetInt32uMillisecondTick() ) is approximately 1 millisecond.
  *
  * - When used with a 32.768kHz crystal, the system tick is 0.976 milliseconds.
  *
  * - When used with a 3.6864MHz crystal, the system tick is 1.111 milliseconds.
  *
  * A single quarter-second tick (as returned by
  * ::halCommonGetInt16uQuarterSecondTick() ) is approximately 0.25 seconds.
  *
  * The values used by the time support functions will wrap after an interval.
  * The length of the interval depends on the length of the tick and the number
  * of bits in the value. However, there is no issue when comparing time deltas
  * of less than half this interval with a subtraction, if all data types are the
  * same.

    @verbatim
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

/** @addtogroup HAL
  * @{
  */

/** @defgroup system-timer
  * @brief system-timer modules
  * @{
  */

/* Private define ------------------------------------------------------------*/

#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  system-timer_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the system tick.
  * @param  None
  * @retval Time to update the async registers after RTC is started (units of 100 
  *         microseconds).
  */
uint16_t halInternalStartSystemTimer(void);


/**
  * @brief  Returns the current system time in system ticks, as a 16-bit
  *         value.
  * @param  None
  * @retval The least significant 16 bits of the current system time, in system
  *         ticks.
  */
uint16_t halCommonGetInt16uMillisecondTick(void);

/**
  * @brief  Returns the current system time in system ticks, as a 32-bit
  *         value. 
  * @param  None
  * @retval The least significant 32 bits of the current system time, in 
  *         system ticks.
  */
uint32_t halCommonGetInt32uMillisecondTick(void);

/**
 * @brief  Returns the current system time in quarter second ticks, as a
 *         16-bit value.
 * @param  None
 * @retval The least significant 16 bits of the current system time, in system
 *         ticks multiplied by 256.
 */
uint16_t halCommonGetInt16uQuarterSecondTick(void);

#endif /* __SYSTEM_TIMER_H__ */


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
