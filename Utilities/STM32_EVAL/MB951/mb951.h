/**
  ******************************************************************************
  * @file    mb951.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    30-November-2012
  * @brief   This file contains definitions for MB951 Leds, push-buttons
  *          and COM ports hardware resources.
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
#ifndef __MB8951_H
#define __MB951_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32w108xx.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup MB951
  * @{
  */
   
/** @addtogroup MB951_LOW_LEVEL
  * @{
  */ 

/** @defgroup MB951_LOW_LEVEL_Exported_Types
  * @{
  */
typedef enum 
{
  LED1 = 0,
  LED3 = 1
} Led_TypeDef;

typedef enum 
{
  BUTTON_S1 = 0
} Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

typedef enum 
{
  COM1 = 0
} COM_TypeDef;

/**
  * @}
  */ 

/** @defgroup MB951_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** @addtogroup MB951_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             2

#define LED1_PIN                         GPIO_Pin_5   /* GPIO05: PA.05 */
#define LED1_GPIO_PORT                   GPIOA        /* GPIOA */
  
#define LED3_PIN                         GPIO_Pin_0   /* GPIO00: PA.00 */
#define LED3_GPIO_PORT                   GPIOA        /* GPIOA */

/**
  * @}
  */ 

/** @addtogroup MB951_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          1

/**
 * @brief S1 push-button
 */
#define S1_BUTTON_PIN                GPIO_Pin_3      /* GPIO03: PA.03 */
#define S1_BUTTON_GPIO_PORT          GPIOA           /* GPIOA */

#define S1_BUTTON_EXTI_IRQn          EXTI_IRQC
#define S1_BUTTON_EXTI_SOURCE        EXTI_SourcePA3
#define S1_BUTTON_IRQn               EXTIC_IRQn

/**
  * @}
  */
  
/** @addtogroup MB951_LOW_LEVEL_COM
  * @{
  */
#define COMn                             1

/**
 * @brief Definition for COM port1, connected to SC1
 */ 
#define EVAL_COM1                        SC1_UART

#define EVAL_COM1_TX_PIN                 GPIO_Pin_1          /* GPIO09: PB.01 */
#define EVAL_COM1_TX_GPIO_PORT           GPIOB               /* GPIOB */

#define EVAL_COM1_RX_PIN                 GPIO_Pin_2          /* GPIO10: PB.02 */   
#define EVAL_COM1_RX_GPIO_PORT           GPIOB               /* GPIOB */

#define EVAL_COM1_IRQn                   UART1_IRQn

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup MB951_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);
void STM_EVAL_COMInit(COM_TypeDef COM, UART_InitTypeDef* UART_InitStruct);

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __MB951_H */
/**
  * @}
  */ 

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
