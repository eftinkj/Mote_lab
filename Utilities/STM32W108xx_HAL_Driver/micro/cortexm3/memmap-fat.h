/**
  ******************************************************************************
  * @file    memmap-fat.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   STM32W108 series memory map fixed address table definition
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
  
#ifndef __MEMMAP_FAT_H__
#define __MEMMAP_FAT_H__

#ifndef __MEMMAP_TABLES_H__

/* Includes ------------------------------------------------------------------*/

/** @addtogroup HAL
  * @{
  */

/** @defgroup memmap-fat
  * @{
  */

  /* if we weren't included via memmap-tables.h, define a dummy type for the BAT */
  typedef void HalBootloaderAddressTableType;
#endif

#include "fib-bootloader.h"

/* ****************************************************************************
   If any of these address table definitions ever need to change, it is highly
   desirable to only add new entries, and only add them on to the end of an
   existing address table... this will provide the best compatibility with
   any existing code which may utilize the tables, and which may not be able to 
   be updated to understand a new format (example: bootloader which reads the 
   application address table) */

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/* Description of the Fixed Address Table (FAT) */
typedef struct {
  HalBaseAddressTableType baseTable;
  void *CustomerInformationBlock;
  HalBootloaderAddressTableType *bootloaderAddressTable;
  void *startOfUnusedRam;
  /* pointers to shared functions */
  FibStatus (* fibFlashWrite)(uint32_t address, uint8_t *data,
                              uint32_t writeLength, uint32_t verifyLength);
  FibStatus (* fibFlashErase)(FibEraseType eraseType, uint32_t address);
} HalFixedAddressTableType;

#if (defined (__ICCARM__) || defined (__GNUC__))
extern const HalFixedAddressTableType halFixedAddressTable; 
#elif defined __CC_ARM
extern HalFixedAddressTableType halFixedAddressTable;
#endif

#define FIXED_ADDRESS_TABLE_TYPE        (0x0FA7)

/* The current versions of the address tables.
   Note that the major version should be updated only when a non-backwards
   compatible change is introduced (like removing or rearranging fields)
   adding new fields is usually backwards compatible, and their presence can
   be indicated by incrementing only the minor version */
#define FAT_VERSION                     (0x0003)
#define FAT_MAJOR_VERSION               (0x0000)
#define FAT_MAJOR_VERSION_MASK          (0xFF00)

/* *** FAT Version history: *** */
/*  0x0003 - Cstartup chip initialization update
   0x0002 - Add function pointers for shared flash drivers, Restore peripheral 
            registers before resetting
   0x0001 - Initial version */



#endif /* __MEMMAP_FAT_H__ */

/**
  * @}
  */ 

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
