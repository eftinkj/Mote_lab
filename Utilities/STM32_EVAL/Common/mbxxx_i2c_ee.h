/**
  ******************************************************************************
  * @file    mbxxx_i2c_ee.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    30-November-2012
  * @brief   This file contains all the functions prototypes for 
  *          the mbxxx_i2c_ee.c firmware driver.
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
#ifndef __MBXXX_I2C_EE_H
#define __MBXXX_I2C_EE_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#if defined (USE_MB851_REVC) || defined (USE_MB851_REVD)
 #include "mb851.h"
#elif defined (USE_MB954_REVB) || defined (USE_MB954_REVC)
 #include "mb954.h"
#else 
 #error "Please select first the STM32W board to be used."
#endif /* USE_MB851_REVC or  USE_MB851_REVD */


/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup COMMON
  * @{
  */
  
/** @addtogroup MBXXX_I2C_EE
  * @{
  */  

/** @defgroup MBXXX_I2C_EE_Exported_Types
  * @{
  */ 

/**
  * @}
  */
  
/** @defgroup MBXXX_I2C_EE_Exported_Constants
  * @{
  */

/* Select which EEPROM will be used with this driver */
#define sEE_M24M01  
   
/* Uncomment the following line to use the default sEE_TIMEOUT_UserCallback() 
   function implemented in stm32373c_eval_i2c_ee.c file.
   sEE_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...). */   
/* #define USE_DEFAULT_TIMEOUT_CALLBACK */
   
#if !defined (sEE_M24C64) && !defined (sEE_M24M01)
/* Use the defines below the choose the EEPROM type */
#define sEE_M24M01  /* Support the device: M24M01. */
/*#define sEE_M24C64*/  /* Support the devices: M24C32 and M24C64 */
#endif

#ifdef sEE_M24C64
/* For M24C64 devices, E0,E1 and E2 pins are all used for device 
  address selection (ne need for additional address lines). According to the 
  Harware connection on the board. */

 #define sEE_HW_ADDRESS         0xA0   /* E0 = E1 = E2 = 0 */ 

#elif defined (sEE_M24M01)
/* The sEE_M24M01 contains 2 blocks (64Kbytes each) with the adresses below 
   EEPROM Addresses defines */
 #define sEE_HW_ADDRESS     0xA0      /* Block 1 : E1= 0; E2 = 0; A16 = 0 */
 /*#define sEE_HW_ADDRESS     0xA2*/  /* Block 2 : E1= 0; E2 = 0; A16 = 1 */

#endif /* sEE_M24C64 */
   

#if defined (sEE_M24C64)
 #define sEE_PAGESIZE           32
#elif defined (sEE_M24M01)
 #define sEE_PAGESIZE           128   
#endif
   
/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define sEE_FLAG_TIMEOUT         ((uint32_t)0x200)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * sEE_FLAG_TIMEOUT))
#define sEE_TIMEOUT              ((uint32_t)0x100)
/* Maximum number of trials for sEE_WaitEepromStandbyState() function */
#define sEE_MAX_TRIALS_NUMBER     300
      
#define sEE_OK                    0
#define sEE_FAIL                  1   

/**
  * @}
  */ 
  
/** @defgroup MBXXX_I2C_EE_Exported_Macros
  * @{
  */    
/**
  * @}
  */ 

/** @defgroup MBXXX_I2C_EE_Exported_Functions
  * @{
  */ 
void     sEE_DeInit(void);
void     sEE_Init(void);
uint32_t sEE_WritePage(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t* NumByteToWrite);
void     sEE_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
uint32_t sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
uint32_t sEE_WaitEepromStandbyState(void);

/* USER Callbacks: These are functions for which prototypes only are declared in
   EEPROM driver and that should be implemented into user applicaiton. */  
/* sEE_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occure during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...).
   You can use the default timeout callback implementation by uncommenting the 
   define USE_DEFAULT_TIMEOUT_CALLBACK in stm32373c_eval_i2c_ee.h file.
   Typically the user implementation of this callback should reset I2C peripheral
   and re-initialize communication or in worst case reset all the application. */
uint32_t sEE_TIMEOUT_UserCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* __MBXXX_I2C_EE_H */
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
