/**
  ******************************************************************************
  * @file    mb954.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    30-November-2012
  * @brief   This file contains definitions for MB954 Leds, push-buttons
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
#ifndef __MB954_H
#define __MB954_H

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

/** @addtogroup MB954
  * @{
  */
   
/** @addtogroup MB954_LOW_LEVEL
  * @{
  */ 

/** @defgroup MB954_LOW_LEVEL_Exported_Types
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

/** @defgroup MB954_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** 
  * @brief  Uncomment the line corresponding to the STMicroelectronics STM32W
  *   board used in your application.
  *   
  *  Tip: To avoid modifying this file each time you need to switch between these
  *       boards, you can define the board in your toolchain compiler preprocessor.    
  */ 
#if !defined (USE_MB954_REVB) && !defined (USE_MB954_REVC) && !defined (USE_MB954_REVA)
  /* #define USE_MB954_REVA */        /*!< USE_MB954_REVA: board MB954 rev A */
  /* #define USE_MB954_REVB */   /*!< USE_MB954_REVB: board MB954 rev B */
  /* #define USE_MB954_REVC */   /*!< USE_MB954_REVC: board MB954 rev C */
#endif

/** @addtogroup MB954_LOW_LEVEL_LED
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

/** @addtogroup MB954_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          1

/**
 * @brief S1 push-button
 */
#define S1_BUTTON_PIN                GPIO_Pin_3      /* GPIO11: PB.03 */
#define S1_BUTTON_GPIO_PORT          GPIOB           /* GPIOB */

#define S1_BUTTON_EXTI_IRQn          EXTI_IRQC
#define S1_BUTTON_EXTI_SOURCE        EXTI_SourcePB3
#define S1_BUTTON_IRQn               EXTIC_IRQn 

/** @addtogroup MB954_LOW_LEVEL_COM
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

/** @addtogroup MB954_LOW_LEVEL_MEMS_I2C
  * @{
  */

/**
  * @brief  MEMS I2C Interface pins
  */
#define LIS302DL_I2C                           SC2_I2C

#define LIS302DL_I2C_SDA_PIN                   GPIO_Pin_1                  /* GPIO01: PA.01 */
#define LIS302DL_I2C_SDA_GPIO_PORT             GPIOA                       /* GPIOA */

#define LIS302DL_I2C_SCL_PIN                   GPIO_Pin_2                  /* GPIO02: PA.02 */
#define LIS302DL_I2C_SCL_GPIO_PORT             GPIOA                       /* GPIOA */

#define LIS302DL_HW_ADDRESS     ((uint8_t)0x38)     
/**
  * @}
  */


/** @addtogroup MB954_LOW_LEVEL_TSENSOR_ADC
  * @{
  */
/**
  * @brief  STLM20 Temperature Sensor ADC Interface pins
  */
#define STLM20_ADC                         ADC

#define STLM20_ADC_CHANNEL_PIN             GPIO_Pin_4                  /* PA.04 */
#define STLM20_ADC_CHANNEL_GPIO_PORT       GPIOA                       /* GPIOA */

/**
  * @}
  */
   
/** @addtogroup MB954_LOW_LEVEL_I2C_EE
  * @{
  */

#ifdef USE_MB954_REVA
#elif defined (USE_MB954_REVB) || defined (USE_MB954_REVC)

/**
  * @brief  I2C EEPROM Interface pins
  */

#define sEE_I2C                          SC2_I2C
   
#define sEE_I2C_SCL_PIN                  GPIO_Pin_2                  /* GPIO02: PA.02 */
#define sEE_I2C_SCL_GPIO_PORT            GPIOA                       /* GPIOA */

#define sEE_I2C_SDA_PIN                  GPIO_Pin_1                  /* GPIO01: PA.01 */
#define sEE_I2C_SDA_GPIO_PORT            GPIOA                       /* GPIOA */

/**
  * @}
  */

#else 
 #error "Please select first the STM32W board to be used."
#endif /* USE_MB954_REVA */            

/** @defgroup MB954_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button);
void STM_EVAL_COMInit(COM_TypeDef COM, UART_InitTypeDef* UART_InitStruct);
void LIS302DL_LowLevel_DeInit(void);
void LIS302DL_LowLevel_Init(void);
void STLM20_LowLevel_DeInit(void);
void STLM20_LowLevel_Init(void);

#if defined (USE_MB954_REVB) || defined (USE_MB954_REVC)
 void sEE_LowLevel_DeInit(void);
 void sEE_LowLevel_Init(void);
#endif /* USE_MB954_REVB or USE_MB954_REVC */

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __MB954_H */
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
