/**
  ******************************************************************************
  * @file    error.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Return codes for API functions and module definitions.
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

#ifndef __ERRORS_H__
#define __ERRORS_H__

/* Includes ------------------------------------------------------------------*/

/** @addtogroup HAL
  * @{
  */

/** @defgroup error
  * @{
  */

/**
  * @brief  Return type for St functions.
  */
#ifndef __STSTATUS_TYPE__
#define __STSTATUS_TYPE__
  typedef uint8_t StStatus;
#endif /* __STSTATUS_TYPE__ */

/**
  * @addtogroup status_codes Status codes
  * @{
  */

/**
  * @brief Macro used by error-def.h to define all of the return codes.
  *
  * @param symbol  The name of the constant being defined. All St returns
  *                begin with ST_. For example, ST_SUCCESS.
  * 
  * @param value   The value of the return code. For example, 0x61.
  */
#define DEFINE_ERROR(symbol, value)   ST_ ## symbol = value,

enum {
#include "error-def.h"
/** Gets defined as a count of all the possible return codes in the 
  *  stack API. 
  */
  ST_ERROR_CODE_COUNT
};

#undef DEFINE_ERROR

#endif /* __ERRORS_H__ */

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

