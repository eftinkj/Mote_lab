/**
  ******************************************************************************
  * @file    hal-flash.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   Implements the generic hal flash manipulation routines.
  *
  *  @verbatim
  *
  *
    ===========================================================================
     ## Definition and description of public hal flash manipulation routines.##
    ===========================================================================
      [..]
      (#)  During an erase or a write the flash is not available,
       which means code will not be executable from flash. These routines still
       execute from flash, though, since the bus architecture can support doing so.
       (+) This also means all interrupts will be disabled.
       (+) Hardware documentation indicates 40us for a write and 21ms for an erase.
       (+) See hal_flash.h for source code.
              
    @endverbatim
  ******************************************************************************
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

#include PLATFORM_HEADER
#include "error.h"
#include "fib-bootloader.h"
#include "mpu.h"
#include "memmap.h"
#include "hal_flash.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup hal_flash
  * @brief hal_flash modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* A translation table used to convert FibStatus codes to corresponding
   StStatus values */
static const StStatus fibToStStatus[] = {
  ST_SUCCESS,                    /* FIB_SUCCESS             0 */
  ST_BAD_ARGUMENT,               /* FIB_ERR_UNALIGNED       1 */
  ST_BAD_ARGUMENT,               /* FIB_ERR_INVALID_ADDRESS 2 */
  ST_BAD_ARGUMENT,               /* FIB_ERR_INVALID_TYPE    3 */
  ST_ERR_FLASH_PROG_FAIL,        /* FIB_ERR_WRITE_PROTECTED 4 */
  ST_ERR_FLASH_PROG_FAIL,        /* FIB_ERR_WRITE_FAILED    5 */
  ST_ERR_FLASH_WRITE_INHIBITED,  /* FIB_ERR_ERASE_REQUIRED  6 */
  ST_ERR_FLASH_VERIFY_FAILED     /* FIB_ERR_VERIFY_FAILED   7 */
};
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
boolean flashEraseIsActive = FALSE;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  hal_flash_Private_Functions
  * @{
  */

/**
  * @brief The purpose of flashEraseIsActive and halFlashEraseIsActive() is so that
  * interrupts can query the flash library to find out of Flash Erase is
  * active when their ISR gets invoked.  This is useful because Flash Erase
  * causes the chip to go ATOMIC for 21ms and this delay will disrupt interrupt
  * latency. By having a sinple API that an ISR can query for this state,
  * the ISR can appriopriately adjust for a 21ms latency time.
  */
boolean halFlashEraseIsActive(void)
{
  return flashEraseIsActive;
}

static boolean verifyFib(void)
{
  /* Ensure that a programmed FIB of a proper version is present */
  return ( (halFixedAddressTable.baseTable.type == FIXED_ADDRESS_TABLE_TYPE) &&
           ( ( (halFixedAddressTable.baseTable.version & FAT_MAJOR_VERSION_MASK) 
               == 0x0000 ) &&
             (halFixedAddressTable.baseTable.version >= 0x0002) 
           )
         );
}

/* The parameter 'eraseType' chooses which erasure will be performed while
   the 'address' parameter chooses the page to be erased during MFB page erase. */
StStatus halInternalFlashErase(uint8_t eraseType, uint32_t address)
{
  FibStatus status;
  
  ATOMIC(
    BYPASS_MPU(
      flashEraseIsActive = TRUE;

        assert(verifyFib());
        status = halFixedAddressTable.fibFlashErase(
                                           (((uint32_t)eraseType) | DO_ERASE), 
                                           address);
    )
  )
  /* If there are any interrupts pending that could have been delayed for 21ms,
     they will be serviced here since we exit the ATOMIC block.  These ISRs
     can query the flash library and find out that erasing is active.  After
     this point, we're no longer ATOMIC/disrupting latency so our erase
     active flag should be cleared. */
  flashEraseIsActive = FALSE;
  
  if(status!=FIB_SUCCESS) {
    return fibToStStatus[status];
  }
    status = halFixedAddressTable.fibFlashErase(
                                        (((uint32_t)eraseType) | DO_VERIFY), 
                                        address);
  return fibToStStatus[status];
}


  /*The parameter 'address' defines the starting address of where the
    programming will occur - this parameter MUST be half-word aligned since all
    programming operations are HW.  The parameter 'data' is a pointer to a buffer
    containin the 16bit half-words to be written.  Length is the number of 16bit
    half-words contained in 'data' to be written to flash.
    NOTE: This function can NOT write the option bytes and will throw an error
    if that is attempted.*/
StStatus halInternalFlashWrite(uint32_t address, uint16_t * data, uint32_t length)
{
  FibStatus status;
    
  length = length * 2;  /* fib routines specify length in bytes */
  
  ATOMIC(
    BYPASS_MPU( 
        /* Ensure that a programmed FIB of a proper version is present */
        assert(verifyFib());
        status = halFixedAddressTable.fibFlashWrite(address, 
                                                    (uint8_t *)data, 
                                                    length,
                                                    0);
    )
  )
  
  if(status!=FIB_SUCCESS) {
    return fibToStStatus[status];
  }
    status = halFixedAddressTable.fibFlashWrite(address,
                                                (uint8_t *)data,
                                                0,
                                                length);
  
  return fibToStStatus[status];
}


/* The parameter 'byte' is the option byte number to be programmed.  This
   parameter can have a value of 0 through 7.  'data' is the 8bit value to be
   programmed into the option byte since the hardware will calculate the
   compliment and program the full 16bit option byte. */
StStatus halInternalCibOptionByteWrite(uint8_t byte, uint8_t data)
{
  uint16_t dataAndInverse = HIGH_LOW_TO_INT(~data, data);
  /* There are only 8 option bytes, don't try to program more than that. */
  if (byte > 7) {
    return ST_ERR_FLASH_PROG_FAIL;
  }
  return halInternalFlashWrite(CIB_OB_BOTTOM + (byte << 1), &dataAndInverse, 1);
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
