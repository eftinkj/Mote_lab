/**
  ******************************************************************************
  * @file    serial_utils.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Header file for serial utilities  
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

#ifndef _SERIAL_UTILS_H_
#define _SERIAL_UTILS_H_

/* Includes ------------------------------------------------------------------*/

/** @addtogroup Shared
  * @{
  */

/** @addtogroup serial_utils
  * @{
  */
  
/* Private define ------------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

typedef boolean  (printfFlushHandler)(uint8_t *contents, 
                                      uint8_t length);
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/** @defgroup  serial_utils_Private_Function_Prototypes
  * @{
  */

uint8_t *writeHex(uint8_t *charBuffer, uint16_t value, uint8_t charCount);
boolean serialWriteString(const char * string);
boolean serialWaitSend(void);
boolean serialReadPartialLine(char *data, uint8_t max, uint8_t * index);
boolean serialWriteData(uint8_t *data, uint8_t length);
boolean serialWriteByte(uint8_t dataByte);
boolean serialReadByte(uint8_t *dataByte);

#endif /* _SERIAL_UTILS_H_*/

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
