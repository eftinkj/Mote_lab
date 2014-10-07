/**
  ******************************************************************************
  * @file    mb951.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    30-November-2012
  * @brief   This file provides:
  *            - set of firmware functions to manage Leds, push-button and COM ports
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
#include "mb951.h"

/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup MB951
  * @{
  */   
    
/** @defgroup MB951_LOW_LEVEL 
  * @brief This file provides firmware functions to manage Leds, push-buttons and 
  *        COM port available on MB951 board from STMicroelectronics.
  * @{
  */ 

/** @defgroup MB951_LOW_LEVEL_Private_TypesDefinitions
  * @{
  */

/* Uncomment the line below according to the MB951 board revision used.
  */

/**
  * @}
  */ 

/** @defgroup MB951_LOW_LEVEL_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup MB951_LOW_LEVEL_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup MB951_LOW_LEVEL_Private_Variables
  * @{
  */
GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED3_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED3_PIN};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {S1_BUTTON_GPIO_PORT};

const uint16_t BUTTON_PIN[BUTTONn] = {S1_BUTTON_PIN};

const uint16_t BUTTON_EXTI_IRQn[BUTTONn] = {S1_BUTTON_EXTI_IRQn};

const uint16_t BUTTON_EXTI_SOURCE[BUTTONn] = {S1_BUTTON_EXTI_SOURCE};

const uint16_t BUTTON_IRQn[BUTTONn] = {S1_BUTTON_IRQn};

SC_UART_TypeDef* COM_UART[COMn] = {EVAL_COM1};

GPIO_TypeDef* COM_TX_PORT[COMn] = {EVAL_COM1_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn] = {EVAL_COM1_RX_GPIO_PORT};

const uint16_t COM_TX_PIN[COMn] = {EVAL_COM1_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {EVAL_COM1_RX_PIN};

/**
  * @}
  */

/** @defgroup MB951_LOW_LEVEL_Private_FunctionPrototypes
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup MB951_LOW_LEVEL_Private_Functions
  * @{
  */ 

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED3
  * @retval None
  */
void STM_EVAL_LEDInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
  GPIO_PORT[Led]->BSR = GPIO_PIN[Led];
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED3
  * @retval None
  */
void STM_EVAL_LEDOn(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED3
  * @retval None
  */
void STM_EVAL_LEDOff(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BSR = GPIO_PIN[Led];
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED3
  * @retval None
  */
void STM_EVAL_LEDToggle(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}

/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_S1: Push Button  
  * @param  Button_Mode: Specifies Button mode.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                     generation capability
  * @retval None
  */
void STM_EVAL_PBInit(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure Button pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PUD;
  GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
  GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);
  
  /* Set pull-up on button I/O */
  GPIO_SetBits(BUTTON_PORT[Button], BUTTON_PIN[Button]);
  
  if (Button_Mode == BUTTON_MODE_EXTI)
  {
    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Source = BUTTON_EXTI_SOURCE[Button];
    EXTI_InitStructure.EXTI_IRQn = BUTTON_EXTI_IRQn[Button];
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling_Edge;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_DigitalFilterCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure); 
  }
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_S1: Push Button  
  * @retval The Button GPIO pin value.
  */
uint32_t STM_EVAL_PBGetState(Button_TypeDef Button)
{
  /* Return the push button status on MB951 board */
  return GPIO_ReadInputDataBit(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:    
  *     @arg COM1
  * @param  UART_InitStruct: pointer to a UART_InitTypeDef structure that
  *   contains the configuration information for the specified UART peripheral.
  * @retval None
  */
void STM_EVAL_COMInit(COM_TypeDef COM, UART_InitTypeDef* UART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Disable the serial controller interface */
  UART_Cmd(COM_UART[COM], DISABLE); 

  /* UART configuration: Set up the parameters specific for the UART operating mode */
  UART_Init(COM_UART[COM], UART_InitStruct);  

  /* Configure UART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);
  /* Set pull-up on UART Tx pin */
  GPIO_SetBits(COM_TX_PORT[COM], COM_TX_PIN[COM]);

  /* Configure UART Rx as input with pull-up */
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);
  /* Set pull-up on UART Rx pin */
  GPIO_SetBits(COM_RX_PORT[COM], COM_RX_PIN[COM]);

  /* Enable the UART peripheral */
  UART_Cmd(COM_UART[COM], ENABLE);
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

/**
  * @}
  */

/**
  * @}
  */ 
    

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
