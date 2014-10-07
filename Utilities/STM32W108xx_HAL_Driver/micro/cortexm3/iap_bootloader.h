/**
  ******************************************************************************
  * @file    iap_bootloader.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   IAP bootloader specific definitions and customization
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

#ifndef __IAP_BOOTLOADER_H__
#define __IAP_BOOTLOADER_H__

/* Includes ------------------------------------------------------------------*/

/** @addtogroup HAL
  * @{
  */

/** @defgroup iap_bootloader
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/**
  * @brief Application start address
  */
#define IAP_BOOTLOADER_APPLICATION_ADDRESS   0x08003000

/**
  * @brief Radio PAN ID for OTA bootloader
  */
#define IAP_BOOTLOADER_PAN_ID 0xb00b

/**                         
  * @brief Signature used for switch to application
  */
#define IAP_BOOTLOADER_APP_SWITCH_SIGNATURE 0xb001204d

/**
  * @brief Radio default channel for OTA bootloader
  */
#define IAP_BOOTLOADER_DEFAULT_CHANNEL 0x0F
#ifndef INCLUDE_FROM_ASSEMBLER

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/** @defgroup  adc_Private_Functions_prototypes
  * @{
  */

/** @brief Indicate whether hardware activation is force for OTA bootloader
  *   This function is provided to allow users to customize the hardware activation of the OTA bootloader based on the specific
  *   features of their board.
  *   This code should be simple, reliable and small in footprint. It return TRUE or FALSE to indicate that
  *   the OTA bootloader must be activated or not.
  *   The default example implementation is for ST boards and it activates OTA bootloader by button S1 pressure.
  * @param  None  
  * @retval TRUE or FALSE to indicate that hardware activation is wanted or not.
  */
extern uint8_t bootloaderOTAHardwareActivation(void);

/** @brief  Provide a mechanism to inform users that the OTA bootloader has been activated.
  *   This function is provided to allow users to customize the indication of activation of the OTA bootloader based on the specific
  *   features of their board.
  *   This code should be simple, reliable and small in footprint. 
  *   The default example implementation is for ST boards and it will switch LED D1 on when the OTA bootloader is activated.
  * @param  None
  * @retval None  
  */
extern void bootloaderOTAIndicateActivation(void);


/** @brief  
  * @param
  * @retval
  */
void bootloaderOTAApplication(void);


#endif /* INCLUDE_FROM_ASSEMBLER */
#endif /* __IAP_BOOTLOADER_H__ */

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
