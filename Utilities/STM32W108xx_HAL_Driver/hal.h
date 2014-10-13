/**
  ******************************************************************************
  * @file    hal.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Generic set of HAL includes for all platforms.
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
#ifndef __HAL_H__
#define __HAL_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup HAL
  * @{
  */

/** @defgroup hal
  * @{
  */

#ifndef RF_LIBRARIES_BUILDING     
  #if defined (USE_MB851_REVA_REVB) || defined (USE_MB851_REVC) || defined (USE_MB851_REVD)
   #include "mb851.h"
  #elif defined (USE_MB954_REVA) || defined (USE_MB954_REVB) || defined (USE_MB954_REVC)
   #include "mb954.h"
  #elif defined (USE_MB951)
   #include "mb951.h"
  #elif defined (USE_MB950)
   #include "mb950.h"
  #elif defined (USE_MB_SNT_X2)
   #include "MB_SnT_X2/mb_snt_x2.h"
  #else 
  #error "Please select first the STM32W board to be used."
  #endif /* USE_MB851_REVA_REVB or USE_MB851_REVC or USE_MB851_REVD*/
#endif    

#include "micro-common.h"
#include "micro/system-timer.h"
#include "micro/cortexm3/nvm.h"
#include "hal_uart.h"
#include "micro/cortexm3/hal_adc.h"

#ifdef __cplusplus
}
#endif

#endif /*__HAL_H__ */

/**
  * @} 
  */
  
/**
  * @} 
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

