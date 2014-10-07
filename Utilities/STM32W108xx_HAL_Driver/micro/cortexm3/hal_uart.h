/**
  ******************************************************************************
  * @file    hal_uart.h 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Header for STM32W  hal uart drivers, supporting IAR's standard library
  *          IO routines.
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

/** @addtogroup HAL
  * @{
  */

/** @defgroup HAL_UART
  * @brief HAL UART modules
  * @{
  */

#ifndef __HAL_UART_MIN_H__
#define __HAL_UART_MIN_H__
#ifdef __ICCARM__
#include <yfuns.h>
#endif

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/** @defgroup  hal_uart_Functions
  * @{
  */

/**
 * @brief hardware abstraction layer UART initialization
 */
void uartInit(void);

#ifdef __ICCARM__

/**
  * @brief Flush the output stream.  DLib_Config_Full.h defines fflush(), but
  * this library includes too much code so we compile with DLib_Config_Normal.h
  * instead which does not define fflush().  Therefore, we manually define
  * fflush() in the low level UART driver.  This function simply redirects
  * to the __write() function with a NULL buffer, triggering a flush.
  * 
  * @param handle  The output stream.  Should be set to 'stdout' like normal.
  *  
  * @return Zero, indicating success.
  */
size_t fflush(int handle);

/**
  * @brief Define the stdout stream.  Since we compile with DLib_Config_Normal.h
  * it does not define 'stdout'.  There is a low-level IO define '_LLIO_STDOUT'
  * which is equivalent to stdout.  Therefore, we define 'stdout' to be
  * '_LLIO_STDOUT'.
  */
#define stdout _LLIO_STDOUT
#endif

/**
  * @brief Read the input byte if any. 
  */
boolean __io_getcharNonBlocking(uint8_t *data);
void __io_putchar( char c);
int __io_getchar(void);
void __io_flush( void );

#endif /* __HAL_UART_MIN_H__*/


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
