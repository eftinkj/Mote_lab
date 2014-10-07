/**
  ******************************************************************************
  * @file    fib-bootloader.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Definition and description of FIB bootloader shared functions.
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

#ifndef __FIB_BOOTLOADER_H__
#define __FIB_BOOTLOADER_H__

/* Includes ------------------------------------------------------------------*/

/** @addtogroup HAL
  * @{
  */

/** @defgroup fib-bootloader
  * @{
  */

/* Private define ------------------------------------------------------------*/
  
/* Reset signatures. */
#define FIB_RESET_GO_ZERO          0xF00F0100
/* Reset signatures 0xF00F0100 to 0xF00F010F can be chosen using the go command. */
#define FIB_RESET_GO_JUMP          0xF00F0110
#define FIB_RESET_BAUD_RATE        0xF00F0111
#define FIB_RESET_READ_UNPROTECT   0xF00F0112
#define FIB_RESET_GPIO_DBGSR_BOOTMODE    0xF00F0113
#define FIB_RESET_PART_DATA        0xF00F0114
#define FIB_RESET_NMI_HARD_FAULT   0xF00F0115


/* Status values. */
typedef uint32_t FibStatus;

#define FIB_SUCCESS             0
#define FIB_ERR_UNALIGNED       1
#define FIB_ERR_INVALID_ADDRESS 2
#define FIB_ERR_INVALID_TYPE    3
#define FIB_ERR_WRITE_PROTECTED 4
#define FIB_ERR_WRITE_FAILED    5
#define FIB_ERR_ERASE_REQUIRED  6
#define FIB_ERR_VERIFY_FAILED   7


/* Erase types. */
typedef uint32_t FibEraseType;

#define MFB_MASS_ERASE 0x01
#define MFB_PAGE_ERASE 0x02
#define CIB_ERASE      0x03

#define DO_ERASE  0x0100
#define DO_VERIFY 0x0200

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

FibStatus fibFlashWrite(uint32_t address, uint8_t *data,
                        uint32_t writeLength, uint32_t verifyLength);

FibStatus fibFlashErase(FibEraseType eraseType, uint32_t address);

#endif /* __FIB_BOOTLOADER_H__ */

/**
  * @}
  */ 

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
