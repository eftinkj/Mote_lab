/**
  ******************************************************************************
  * @file    board.h 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Header file x STM32W108 Kits boards abstraction.
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
#include "hal.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup board
  * @brief board modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
#ifndef _BOARD_H_
#define _BOARD_H_

/* Generic definitions */
#define GPIO_PxCLR_BASE ((GPIOA_BASE + 0x0014))
#define GPIO_PxSET_BASE ((GPIOA_BASE + 0x0010))
#define GPIO_PxOUT_BASE ((GPIOA_BASE + 0x000C))
#define GPIO_PxIN_BASE  ((GPIOA_BASE + 0x0008))

/* Each port is offset from the previous port by the same amount */
#define GPIO_Px_OFFSET  (GPIOB_BASE-GPIOA_BASE)

#define BUTTON_S1_WAKE_SOURCE  halButtonSnWakeSource(BUTTON_S1)
#ifdef USE_MB950 
#define BUTTON_S2_WAKE_SOURCE  halButtonSnWakeSource(BUTTON_S2)
#define BUTTON_S3_WAKE_SOURCE  halButtonSnWakeSource(BUTTON_S3)
#define BUTTON_S4_WAKE_SOURCE  halButtonSnWakeSource(BUTTON_S4)
#define BUTTON_S5_WAKE_SOURCE  halButtonSnWakeSource(BUTTON_S5)
#endif /* USE_MB950 */

/** Description uart definition */
#define UART_TX PORTB_PIN(1)
#define UART_RX PORTB_PIN(2)
#define UART_RX_WAKE_SOURCE   0x00000400

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/

/** @defgroup  board_Private_Functions
  * @{
  */

/**@brief  Perform board specific action to power up the system.
  *        This code depends on the actual board features and configure
  *        the stm32w and on board devices for proper operation.
  *        Customer normally needs to modify this file to adapt it to their specific board.
  *@param  None
  *@retval None
  */
void halBoardPowerUp(void);

/**@brief  Perform board specific action to power down the system, usually before going to deep sleep.
  *        This code depends on the actual board features and configure
  *        the stm32w and on board devices for minimal power consumption.
  *        Customer normally needs to modify this file to adapt it to their specific board.
  *@param  None
  *@retval None
  */
void halBoardPowerDown(void);

/**@brief Return the Button Sn Wake up configuration for the halSleepWithOptions() function  
  *@param None
  *@retval Wake GPIO button (required by the sleep driver to configure the wake up source) 
  */
uint32_t halButtonSnWakeSource(Button_TypeDef Button);

#endif /* _BOARD_H_ */


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


