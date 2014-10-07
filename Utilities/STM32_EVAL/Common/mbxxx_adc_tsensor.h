/**
  ******************************************************************************
  * @file    mbxxx_adc_tsensor.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    30-November-2012
  * @brief   This file contains all the functions prototypes for 
  *          the mbxxx_adc_tsensor.c firmware driver.
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
#ifndef __MBXXX_ADC_TSENSOR_H
#define __MBXXX_ADC_TSENSOR_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#if defined (USE_MB851_REVA_REVB) || defined (USE_MB851_REVC) || defined (USE_MB851_REVD)
 #include "mb851.h"
#elif defined (USE_MB954_REVA) || defined (USE_MB954_REVB) || defined (USE_MB954_REVC)
 #include "mb954.h"
#else 
 #error "Please select first the STM32W board to be used."
#endif 

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup COMMON
  * @{
  */
  
/** @addtogroup MBXXX_ADC_TSENSOR 
  * @{
  */  

/** @defgroup MBXXX_ADC_TSENSOR_Exported_Types
  * @{
  */ 

/**
  * @}
  */
  
/** @defgroup MBXXX_ADC_TSENSOR_Exported_Constants
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup MBXXX_ADC_TSENSOR_Exported_Macros
  * @{
  */    
/**
  * @}
  */ 

/** @defgroup MBXXX_ADC_TSENSOR_Exported_Functions
  * @{
  */ 
void STLM20_DeInit(void);
void STLM20_Init(void);
int16_t STLM20_ReadTemp(void);
#ifdef __cplusplus
}
#endif

#endif /* __MBXXX_ADC_TSENSOR_H */
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
