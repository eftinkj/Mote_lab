/**
  ******************************************************************************
  * @file    memmap.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   STM32W108 series memory map definitions used by the full hal
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

#ifndef __MEMMAP_H__
#define __MEMMAP_H__

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define RAM_BOTTOM          (0x20000000u)
#ifdef CORTEXM3_STM32W108xB
#define RAM_SIZE_B          (0x2000)
#endif
#ifdef CORTEXM3_STM32W108CC
#define RAM_SIZE_B          (0x4000)
#endif
#define RAM_SIZE_W          (RAM_SIZE_B/4)
#define RAM_TOP             (RAM_BOTTOM+RAM_SIZE_B-1)

#define MFB_BOTTOM          (0x08000000u)
#ifdef CORTEXM3_STM32W108xB
#define MFB_SIZE_B          (0x20000)          
#endif
#ifdef CORTEXM3_STM32W108CC
#define MFB_SIZE_B          (0x40000)          
#endif
#define MFB_SIZE_W          (MFB_SIZE_B/4)
#define MFB_TOP             (MFB_BOTTOM+MFB_SIZE_B-1)    
#ifdef CORTEXM3_STM32W108xB
#define MFB_PAGE_SIZE_B     (1024)
#endif
#ifdef CORTEXM3_STM32W108CC
#define MFB_PAGE_SIZE_B     (2048)
#endif
#define MFB_PAGE_SIZE_W     (MFB_PAGE_SIZE_B/4)
#ifdef CORTEXM3_STM32W108xB
#define MFB_PAGE_MASK_B     (0xFFFFFC00)
#define MFB_REGION_SIZE     (4) /* One write protection region is 4 pages for a total of 4K. */
#endif
#ifdef CORTEXM3_STM32W108CC
#define MFB_PAGE_MASK_B     (0xFFFFF800)
#define MFB_REGION_SIZE     (2) /* One write protection region is 2 pages for a total of 4K. Last write protection is covering 132K. */
#endif
#define MFB_ADDR_MASK       (0x0003FFFFu)

#define CIB_BOTTOM          (0x08040800u)
#ifdef CORTEXM3_STM32W108xB
#define CIB_SIZE_B          (0x200)
#endif
#ifdef CORTEXM3_STM32W108CC
#define CIB_SIZE_B          (0x800)
#endif
#define CIB_SIZE_W          (CIB_SIZE_B/4)
#define CIB_TOP             (CIB_BOTTOM+CIB_SIZE_B-1)
#ifdef CORTEXM3_STM32W108xB
#define CIB_PAGE_SIZE_B     (512)
#endif
#ifdef CORTEXM3_STM32W108CC
#define CIB_PAGE_SIZE_B     (2048)
#endif
#define CIB_PAGE_SIZE_W     (CIB_PAGE_SIZE_B/4)
#define CIB_OB_BOTTOM       (CIB_BOTTOM+0x00)   /* bottom address of CIB option bytes */
#define CIB_OB_TOP          (CIB_BOTTOM+0x0F)   /* top address of CIB option bytes */

#define FIB_BOTTOM          (0x08040000u)
#define FIB_SIZE_B          (0x800)
#define FIB_SIZE_W          (FIB_SIZE_B/4)
#define FIB_TOP             (FIB_BOTTOM+FIB_SIZE_B-1)
#define FIB_PAGE_SIZE_B     (2048)
#define FIB_PAGE_SIZE_W     (FIB_PAGE_SIZE_B/4)

#ifdef CORTEXM3_STM32W108CC
#define FIB1_BOTTOM          (0x08041000u)
#define FIB1_SIZE_B          (0x4000)
#define FIB1_SIZE_W          (FIB1_SIZE_B/4)
#define FIB1_TOP             (FIB1_BOTTOM+FIB1_SIZE_B-1)
#define FIB1_PAGE_SIZE_B     (2048)
#define FIB1_PAGE_SIZE_W     (FIB1_PAGE_SIZE_B/4)
#endif

#define FPEC_KEY1           0x45670123          /* magic key defined in hardware */
#define FPEC_KEY2           0xCDEF89AB          /* magic key defined in hardware */

/* Constant for MPU initialization */
#define MPU_FLASH_REGION_SIZE 0x12 /* 512K including main user flash and FIB */
#if MFB_SIZE_B == 0x40000
#define MPU_FLASH_SUBREGION_PROTECTION 0xE0
#elif MFB_SIZE_B == 0x20000
#define MPU_FLASH_SUBREGION_PROTECTION 0xEC
#endif
#define MPU_RAM_REGION_SIZE 0x0D /* 16K RAM size max */
#if RAM_SIZE_B == 0x4000
#define MPU_RAM_SUBREGION_PROTECTION 0x00
#elif RAM_SIZE_B == 0x2000
#define MPU_RAM_SUBREGION_PROTECTION 0xF0
#endif


/* Translation between page number and simee (word based) address */
#define SIMEE_ADDR_TO_PAGE(x)   ((uint8_t)(((uint16_t)(x)) / (MFB_PAGE_SIZE_B/2)))
#define PAGE_TO_SIMEE_ADDR(x)   (((uint16_t)(x)) * (MFB_PAGE_SIZE_B/2))

/* Translation between page number and code addresses, used by bootloaders */
#define PROG_ADDR_TO_PAGE(x)    ((uint8_t)((((uint32_t)(x))&MFB_PAGE_MASK_B) / MFB_PAGE_SIZE_B))
#define PAGE_TO_PROG_ADDR(x)    ((((uint32_t)(x)) * MFB_PAGE_SIZE_B)|MFB_BOTTOM)


/* Private typedef -----------------------------------------------------------*/

/** @brief A union that describes the entries of the vector table.  The union is needed
  * since the first entry is the stack pointer and the remainder are function
  * pointers.
  * 
  * Normally the vectorTable below would require entries such as:
  * { .topOfStack = x },
  * { .ptrToHandler = y },
  * But since ptrToHandler is defined first in the union, this is the default
  * type which means we don't need to use the full, explicit entry.  This makes
  * the vector table easier to read because it's simply a list of the handler
  * functions.  topOfStack, though, is the second definition in the union so
  * the full entry must be used in the vectorTable.
  * 
  */
typedef union
{
  void (*ptrToHandler)(void);
  void *topOfStack;
} HalVectorTableType;

/** @brief If any of these address table definitions ever need to change, it is highly
  * desirable to only add new entries, and only add them on to the end of an
  * existing address table... this will provide the best compatibility with
  * any existing code which may utilize the tables, and which may not be able to 
  * be updated to understand a new format (example: bootloader which reads the 
  * application address table) 
  */


/** @brief Generic Address table definition which describes leading fields which
  * are common to all address table types 
  */
typedef struct {
  void *topOfStack;
  void (*resetVector)(void);
  void (*nmiHandler)(void);
  void (*hardFaultHandler)(void);
  uint16_t type;
  uint16_t version;
  const HalVectorTableType *vectorTable;
  /* Followed by more fields depending on the specific address table type */
} HalBaseAddressTableType;

/* Hal only references the FAT */
#include "memmap-fat.h"

#endif /* __MEMMMAP_H__ */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

