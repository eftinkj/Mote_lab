/**
  ******************************************************************************
  * @file    board.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Board file x STM32W108 Kits boards 
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

#include PLATFORM_HEADER
#include "board.h"
#include "error.h"
#include "hal.h"
#include "mfg-token.h"
#include <string.h>

/** @addtogroup HAL
  * @{
  */

/** @defgroup board
  * @brief board modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern GPIO_TypeDef* BUTTON_PORT[]; 
extern const uint16_t BUTTON_PIN[];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  board_Private_Functions
  * @{
  */

/**
  * @brief  Perform board specific action to power down the system, usually before going to deep sleep.
  *         This code depends on the actual board features and configure
  *         the stm32w and on board devices for minimal power consumption.
  *         Customer normally needs to modify this file to adapt it to their specific board. 
  * @param  None
  * @retval None
  */
void halBoardPowerDown(void)
{
#if defined (USE_MB954_REVA) || defined (USE_MB954_REVB) || defined (USE_MB954_REVC)
  GPIO_InitTypeDef  GPIO_InitStructure;
#endif /* USE_MB954_REVA or USE_MB954_REVB or USE_MB954_REVC */
  
  /* Set everything to input value */
  GPIOA->CRL = (GPIO_Mode_IN              <<0)|
                (GPIO_Mode_IN              <<4)|
                (GPIO_Mode_IN              <<8)|
                (GPIO_Mode_IN              <<12);
  GPIOA->CRH = (GPIO_Mode_IN              <<0)|  /* PTI EN */
                (GPIO_Mode_IN              <<4)|  /* PTI_DATA */
                (GPIO_Mode_IN              <<8)|
                (GPIO_Mode_IN              <<12);
  GPIOB->CRL = (GPIO_Mode_IN              <<0)|
                (GPIO_Mode_IN              <<4)|  /* Uart TX */
                (GPIO_Mode_IN              <<8)|  /* Uart RX */
                (GPIO_Mode_IN              <<12);
  GPIOB->CRH = (GPIO_Mode_IN              <<0)|
                (GPIO_Mode_IN              <<4)|
                (GPIO_Mode_IN              <<8)|
                (GPIO_Mode_IN              <<12);
  GPIOC->CRL = (GPIO_Mode_IN              <<0)|
                (GPIO_Mode_IN              <<4)|
                (GPIO_Mode_IN              <<8)|
                (GPIO_Mode_IN              <<12);
  GPIOC->CRH = (GPIO_Mode_IN              <<0)|
                (GPIO_Mode_IN              <<4)|
                (GPIO_Mode_IN              <<8)|  /* OSC32K */
                (GPIO_Mode_IN              <<12);  /* OSC32K */

  /* Configure GPIO for BUTTONSs */
  STM_EVAL_PBInit(BUTTON_S1, BUTTON_MODE_GPIO);
#ifdef USE_MB950  
  STM_EVAL_PBInit(BUTTON_S2, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_S3, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_S4, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_S5, BUTTON_MODE_GPIO);
#endif
  
  /* Configure GPIO for LEDs */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED3);
  
#if defined (USE_MB954_REVA) || defined (USE_MB954_REVB) || defined (USE_MB954_REVC)
  /* Configure GPIO for power amplifier */
  /* SiGe Ant Sel to output. */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Set pull-up on PB5 pin */
  GPIO_SetBits(GPIOB, GPIO_Pin_5);
  /* SiGe Standby */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Set pull-down on PB6 pin */
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
#endif /* USE_MB954_REVA or USE_MB954_REVB or USE_MB954_REVC */
}

/**
  * @brief  Perform board specific action to power up the system.
  *         This code depends on the actual board features and configure
  *         the stm32w and on board devices for proper operation.
  *         Customer normally needs to modify this file to adapt it to their specific board.
  * @param  None
  * @retval None
  */
void halBoardPowerUp(void)
{
#if defined (USE_MB954_REVA) || defined (USE_MB954_REVB) || defined (USE_MB954_REVC)
  GPIO_InitTypeDef  GPIO_InitStructure;
#endif /* USE_MB954_REVA or USE_MB954_REVB or USE_MB954_REVC */
  /* Set everything to input value */
  GPIOA->CRL = (GPIO_Mode_IN            <<0)|
               (GPIO_Mode_IN            <<4)|
               (GPIO_Mode_IN            <<8)|
               (GPIO_Mode_IN            <<12);
  GPIOA->CRH = (GPIO_Mode_IN            <<0)|  /* PTI EN */
               (GPIO_Mode_IN            <<4)|  /* PTI_DATA */
               (GPIO_Mode_IN            <<8)|
               (GPIO_Mode_IN            <<12);
  GPIOB->CRL = (GPIO_Mode_IN            <<0)|
               (GPIO_Mode_AF_PP         <<4)|  /* Uart TX */
               (GPIO_Mode_IN            <<8)|  /* Uart RX */
               (GPIO_Mode_IN            <<12);
  GPIOB->CRH = (GPIO_Mode_IN            <<0)|
               (GPIO_Mode_IN            <<4)|
               (GPIO_Mode_IN            <<8)|
               (GPIO_Mode_IN            <<12);
  GPIOC->CRL = (GPIO_Mode_IN            <<0)|
               (GPIO_Mode_IN            <<4)|
               (GPIO_Mode_IN            <<8)|
               (GPIO_Mode_IN            <<12);
  GPIOC->CRH = (GPIO_Mode_IN            <<0)|
               (GPIO_Mode_IN            <<4)|
               (GPIO_Mode_IN            <<8)|  /* OSC32K */
               (GPIO_Mode_IN            <<12);  /* OSC32K */
  
#ifndef USE_MB951  
  /* Configure GPIO for I2C access */
#ifdef USE_MB_SNT_X2
  //
#else
  LIS302DL_LowLevel_Init();
#endif /* else #ifdef USE_MB_SNT_X2) */
#endif /* USE_MB951 */  
  
  /* Configure GPIO for LEDs */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED3);
  /* Configure GPIO for BUTTONSs */
  STM_EVAL_PBInit(BUTTON_S1, BUTTON_MODE_GPIO);
#ifdef USE_MB950  
  STM_EVAL_PBInit(BUTTON_S2, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_S3, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_S4, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_S5, BUTTON_MODE_GPIO);
#endif

#if defined (USE_MB954_REVA) || defined (USE_MB954_REVB) || defined (USE_MB954_REVC)
  /* Configure GPIO for power amplifier */
  /* SiGe Ant Sel to output. */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Set pull-up on PB5 pin */
  GPIO_SetBits(GPIOB, GPIO_Pin_5);
  /* SiGe Standby */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Set pull-up on PB6 pin */
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  /* SiGe LNA (default LNA off )*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Set pull-down on PB7 pin */
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
  /* SiGe nTX Active */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
#endif /* USE_MB954_REVA or USE_MB954_REVB or USE_MB954_REVC */
}

/**
  * @brief  Wake up GPIO button configuration
  * @param  None
  * @retval Wake button 
  */
uint32_t halButtonSnWakeSource(Button_TypeDef Button)
{
  if(BUTTON_PORT[Button] == GPIOA)
  {
    return BUTTON_PIN[Button];
  }
  else if (BUTTON_PORT[Button] == GPIOB)
  {
    return (BUTTON_PIN[Button] << 8);
  }
  else if (BUTTON_PORT[Button] == GPIOC)
  {
    return (BUTTON_PIN[Button] << 16);
  }
  else
  {
    return 0;
  }
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
