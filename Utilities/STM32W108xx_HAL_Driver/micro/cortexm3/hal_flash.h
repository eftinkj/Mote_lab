/**
  ******************************************************************************
  * @file    hal-flash.h 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   Header for hal_flash for APIs
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
#ifndef __HAL_FLASH_H__
#define __HAL_FLASH_H__

/* Includes ------------------------------------------------------------------*/
#include "memmap.h"

/** @addtogroup hal
  * @{
  */

/** @addtogroup hal_flash
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup hal_flash_Exported_Constants
  * @{
  */

/**
  * @brief Assign numerical value to the type of erasure requested.
  * @note The following eraseType definitions must match the FIB erase types!
  */
#define MFB_MASS_ERASE 0x01
#define MFB_PAGE_ERASE 0x02
#define CIB_ERASE      0x03

/**
  * @}
  */

/** @defgroup hal_flash_Exported_Functions
  * @{
  */

/** @brief Tells the calling code if a Flash Erase operation is active.
  *
  * This state is import to know because Flash Erasing is ATOMIC for 21ms
  * and could disrupt interrupt latency.  But if an ISR can know that it wasn't
  * serviced immediately due to Flash Erasing, then the ISR has the opportunity
  * to correct in whatever manner it needs to.
  * 
  * @return A boolean flag: TRUE if Flash Erase is active, FALSE otherwise.
  */
boolean halFlashEraseIsActive(void);

/** @brief Erases a section of flash back to all 0xFFFF.
  * 
  * @param eraseType Choose one of the three types of erasures to perform.
  *  - MFB_MASS_ERASE (0x01) Erase the entire main flash block.
  *  - MFB_PAGE_ERASE (0x02) Erase one hardware page in the main flash block
  *  chosen by the \c address parameter.
  *  - CIB_ERASE      (0x03) Erase the entire customer information block.
  * 
  * @param address This parameter is only effectual when a MFB_PAGE_ERASE is
  * being performed.  The hardware page encapsulating the address given in this
  * parameter will be erased.  A hardware page size depends on the chip
  * 
  * @return An ::StStatus value indicating the success or failure of the
  * command:
  *  - ST_ERR_FATAL if the \c eraseType is not valid
  *  - ST_ERR_FLASH_ERASE_FAIL if erasing failed due to write protection
  *  - ST_ERR_FLASH_VERIFY_FAILED if erase verification failed
  *  - ST_SUCCESS if erasure completed and verified properly
  */
StStatus halInternalFlashErase(uint8_t eraseType, uint32_t address);

/** @brief Writes a block of words to flash.  A page is erased
  * to 0xFFFF at every address.  Only two writes can be performed to the same
  * address between erasures and this is enforced by the flash interface
  * controller.  If the value already in the address being written to is 0xFFFF,
  * any value can be written.  If the value is not 0xFFFF and not 0x0000, only
  * 0x0000 can be written.  If the value is 0x0000, nothing can be written.
  *
  * \b NOTE: This function can NOT write the option bytes and will throw an
  * error if that is attempted.
  *
  * @param address The starting address of where the programming will occur.
  * This parameter MUST be half-word aligned since all programming operations
  * are half-words.  Also, the address parameter is NOT a pointer.  This
  * routines will cast the address to a pointer for the actual hardware access.
  *
  * @param data A pointer to a buffer containing the 16bit (half-words) to
  * be written.
  *
  * @param length The number of 16bit (half-words) contained in the data buffer
  * to be written to flash.
  *
  * @return An ::StStatus value indicating the success or failure of the
  * command:
  *  - ST_ERR_FLASH_PROG_FAIL if the address is not half-word aligned, the
  *    address is inside the option bytes, write protection is enabled, or the
  *    address is being written to more than twice between erasures.
  *  - ST_ERR_FLASH_VERIFY_FAILED if write verification failed
  *  - ST_SUCCESS if writing completed and verified properly
  */
StStatus halInternalFlashWrite(uint32_t address, uint16_t * data, uint32_t length);

/** @brief Writes an option byte to the customer information block.  Only
  * two writes can be performed to the same address between erasures and this
  * is enforced by the flash interface controller.
  *
  * @param byte The option byte number, 0 though 7, to be programmed.
  *
  * @param data The 8 bit value to be programmed into the option byte.  The
  * hardware is responsible for calculating the compliment and programming
  * the full 16bit option byte space.
  *
  * @return An ::StStatus value indicating the success or failure of the
  * command:
  *  - ST_ERR_FLASH_PROG_FAIL if the byte chosen is greater than 7, write
  *    protection is enabled, or the byte is being written to more than twice
  *    between erasures.
  *  - ST_ERR_FLASH_VERIFY_FAILED if write verification failed
  *  - ST_SUCCESS if writing completed and verified properly
  */
StStatus halInternalCibOptionByteWrite(uint8_t byte, uint8_t data);

/**
  * @}
  */

#endif /* __HAL_FLASH_H__ */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

