/**
  ******************************************************************************
  * @file    low_level_init.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   low_level_init functions
  * @verbatim
  *
 ===============================================================================
                  ##### low_level_init function describtion #####
 ===============================================================================

 * This module contains the function `__low_level_init', a function
 * that is called before the `main' function of the program.  Normally
 * low-level initializations - such as setting the prefered interrupt
 * level or setting the watchdog - can be performed here.
 *
 * Note that this function is called before the data segments are
 * initialized, this means that this function cannot rely on the
 * values of global or static variables.
 *
 * When this function returns zero, the startup code will inhibit the
 * initialization of the data segments. The result is faster startup,
 * the drawback is that neither global nor static data will be
 * initialized.

  @endverbatim
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
#include "memmap.h"

#define IAP_BOOTLOADER_APP_SWITCH_SIGNATURE  0xb001204d
#define IAP_BOOTLOADER_MODE_UART  0
#define IAP_BOOTLOADER_MODE_OTA   1

/** @addtogroup HAL
  * @{
  */

/** @defgroup low_level_init
  * @brief low_level_init modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
  
#ifdef __ICCARM__  
#pragma language=extended
#endif
  
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#if (defined (__ICCARM__) || defined (__GNUC__))
NO_STRIPPING(NO_INIT(VAR_AT_SEGMENT(const HalFixedAddressTableType halFixedAddressTable, __FAT__)));
#elif defined __CC_ARM 
HalFixedAddressTableType halFixedAddressTable __attribute__((section("__FAT__"), zero_init));
#endif
  
/* extern const HalVectorTableType __vector_table[];*/
#ifdef __ICCARM__
extern const HalVectorTableType __vector_table[];
#elif defined __CC_ARM
  extern const HalVectorTableType __Vectors[];
#elif defined __GNUC__
  extern const HalVectorTableType g_pfnVectors[];
#else 
   #error "Vector table expected"  
#endif

#ifdef __CC_ARM
__asm void CPSID ()
{
	CPSID i
	BX LR
   
}
__asm void CPSIE ()
{
	CPSIE i
  BX LR 
}
#endif

/* Private function prototypes -----------------------------------------------*/
static void low_level_init2(void);
/* __interwork int __low_level_init(void);*/
int __low_level_init(void);
/* Private functions ---------------------------------------------------------*/

/** @defgroup  micro_Private_Functions
  * @{
  */

#ifdef CORTEXM3_STM32W108xB
/**
  * @brief  Clock switching for bootloader software activation. 
  * @param  None
  * @retval None
  */
static void switchClock(void)
{
  /* Enable the accurate 24MHz clock. */
  CLK->HSECR2 = CLK_HSECR2_EN;
  /* Wait 3 milliseconds after enabling the crystal before switching to it. */
  /* When running on OSCHF the MAC timer tick is 2 microseconds. */
  MACTMR->CR |= MACTMR_CR_EN;
  uint32_t startTime = MACTMR->CNTR;
  /* Wrapping is handled by the mask. */
  while (((MACTMR->CNTR - startTime) & MACTMR_CNTR_CNT) < (3000 / 2)) {}
  MACTMR->CR = 0x00;
  CLK->HSECR2 |= CLK_HSECR2_SW1;
}

static const uint16_t blOffset[] = {
  0x0715 - 0x03ad - 0x68,
  0x0719 - 0x03ad - 0x6C
};
#endif

/**
  * @brief  Sets the Stack pointer. 
  * @param  Address
  * @retval None
  */
static void setStackPointer(uint32_t address)
{
  #if (defined (__ICCARM__))
      asm("MOV SP, r0");
  #elif (defined (__GNUC__) || defined (__CC_ARM))
      __set_MSP(address);
  #else 
   #error "Inline assembler syntax expected"  
  #endif
}

/**
  * @brief  Configures interupts levels
  * @param  None
  * @retval Init value
  * @note Important note: This function is critical for waking up from deep sleep 
  * and it should not use more that 2 stack position
  * otherwise a stack corruption will occur when waking up from deep sleep.
  * All the steps not related to deep sleep have been moved to low_level_init2.
  * Perhaps this function should be rewritten in assembler to avoid stack usage 
  * all together when waking up from deep sleep.
  */
int __low_level_init(void)
{
#ifdef CORTEXM3_STM32W108xB
  /* Ensure there is enough margin on VREG_1V8 for stable RAM reads by */
  /* setting it to a code of 6.  VREG_1V2 can be left at its reset value. */
  PWR->VREGCR = 0x00000307;
#endif
  
  /* This code should be careful about the use of local variables in case the
     reset type happens to be a deep sleep reset.  If the reset is not from
    deep sleep, then locals can be freely used */
  
  /* When the Cortex-M3 exits reset, interrupts are enable.  Explicitely
     disable them immediately using the standard set PRIMASK instruction.
     Injecting an assembly instruction this early does not effect optimization. */
      
/* asm("CPSID i"); */                                                              
 #if (defined (__ICCARM__) || defined (__GNUC__))  
	asm("CPSID i");
 #elif defined __CC_ARM
        CPSID();
 #else 
   #error "Inline assembler syntax expected"  
 #endif
  
  /* It is quite possible that when the Cortex-M3 begins executing code the
     Core Reset Vector Catch is still left enabled.  Because this VC would
     cause us to halt at reset if another reset event tripped, we should
     clear it as soon as possible.  If a debugger wants to halt at reset,
     it will set this bit again. */
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_VC_CORERESET_Msk;
  
  /* Configure flash access for optimal current consumption early
     during boot to save as much current as we can. */
  FLASH->ACR= (FLASH_ACR_PRFTBE | (1 << 0));
  
  /*****---- Always remap the vector table ----*****/
  /* We might be coming from a bootloader at the base of flash, or even in the
     NULL_BTL case, the BAT/AAT will be at the beginning of the image */

#ifdef __ICCARM__
  SCB->VTOR = (uint32_t)__vector_table;
#elif defined __CC_ARM
  SCB->VTOR = (uint32_t)__Vectors;
#elif __GNUC__
  SCB->VTOR = (uint32_t)g_pfnVectors;
#else
  #error "Vector table expected"
#endif

  /*****---- Always Configure Interrupt Priorities ----************************/
  /* The STM32W support 5 bits of priority configuration.
     The cortex allows this to be further divided into preemption and a
     "tie-breaker" sub-priority.
     We configure a scheme that allows for 3 bits (8 values) of preemption and
     2 bits (4 values) of tie-breaker by using the value 4 in PRIGROUP.
     The value 0x05FA0000 is a special key required to write to this register. */
  SCB->AIRCR = (0x05FA0000 | (4 <<8));
  
  /* A few macros to help with interrupt priority configuration.  Really only 
     uses 6 of the possible levels, and ignores the tie-breaker sub-priority
     for now.
     Don't forget that the priority level values need to be shifted into the
     top 5 bits of the 8 bit priority fields. (hence the <<3)
   
     NOTE: The ATOMIC and DISABLE_INTERRUPTS macros work by setting the 
          current priority to a value of 12, which still allows CRITICAL and 
          HIGH priority interrupts to fire, while blocking MED and LOW.
          If a different value is desired, INTERRUPTS_DISABLED_PRIORITY will need
          to be edited. */
#define CRITICAL (0  <<3)
#define HIGH     (8  <<3)
#define MED      (16 <<3)
#define LOW      (28 <<3)
#define NONE     (31 <<3)
  
/* With optimization turned on, the compiler will indentify all the values
   and variables used here as constants at compile time and will truncate
   this entire block of code to 98 bytes, comprised of 7 load-load-store
     operations. */
   /* vect00 is fixed                        Stack pointer */
   /* vect01 is fixed                        Reset Vector */
   /* vect02 is fixed                        NMI Handler */
   /* vect03 is fixed                        Hard Fault Handler */

  SCB->SHP[0] = CRITICAL;   /* Memory Fault Handler */ 
  SCB->SHP[1] = CRITICAL;   /* Bus Fault Handler */
  SCB->SHP[2] = CRITICAL;   /* Usage Fault Handler */
  SCB->SHP[3] = NONE;       /* Reserved */
  
  SCB->SHP[4] = NONE;       /* Reserved */
  SCB->SHP[5] = NONE;       /* Reserved */
  SCB->SHP[6] = NONE;       /* Reserved */
  SCB->SHP[7] = HIGH;       /* SVCall Handler */
  
  SCB->SHP[8] = MED;        /* Debug Monitor Handler */
  SCB->SHP[9] = NONE;       /* Reserved */
  SCB->SHP[10] = HIGH;      /* PendSV Handler */
  SCB->SHP[11] = HIGH;      /* halSysTickIsr */

  NVIC->IP[0] = MED;       /* Timer 1 Handler */
  NVIC->IP[1] = MED;       /* Timer 2 Handler */
  NVIC->IP[2] = HIGH;      /* Management Handler */
  NVIC->IP[3] = MED;       /* BaseBand Handler */
  
  NVIC->IP[4] = MED;      /* Sleep Timer Handler */
  NVIC->IP[5] = MED;      /* SC1 Handler */
  NVIC->IP[6] = MED;      /* SC2 Handler */
  NVIC->IP[7] = MED;      /* Security Handler */
  
  NVIC->IP[8] = MED;      /* MAC Timer Handler */
  NVIC->IP[9] = MED;      /* MAC TX Handler */
  NVIC->IP[10] = MED;     /* MAC RX Handler */
  NVIC->IP[11] = MED;     /* ADC Handler */
  
  NVIC->IP[12] = MED;     /* GPIO IRQA Handler */
  NVIC->IP[13] = MED;     /* GPIO IRQB Handler */
  NVIC->IP[14] = MED;     /* GPIO IRQC Handler */
  NVIC->IP[15] = MED;     /* GPIO IRQD Handler */
  NVIC->IP[16] = LOW;     /* Debug Handler */
  /* vect33 not implemented */
  /* vect34 not implemented */
  /* vect35 not implemented */
  
  /*****---- Always Configure System Handlers Control and Configuration ----*****/
  SCB->CCR = SCB_CCR_DIV_0_TRP_Msk;
  SCB->SHCSR = ( SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk);
  
  if((RST->SR&RST_SR_WKUP) == RST_SR_WKUP)
  {
    /* Since the 13 NVIC registers above are fixed values, they are restored
       above (where they get set anyways during normal boot sequences) instead
       of inside of the halInternalSleep code: */
    
    void halTriggerContextRestore(void);
    extern __IO boolean halPendSvSaveContext;
    halPendSvSaveContext = 0;       /* 0 means restore context */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; /* pend halPendSvIsr to enable later */
    halTriggerContextRestore();     /* sets MSP, enables interrupts */
    /* if the context restore worked properly, we should never return here */
    while(1) { ; }
  }
  low_level_init2();
  /*==========================================================================*/
  /*                     Choose if segment initialization                     */
  /*                        should be done or not.                            */
  /*                      Return: 0 to omit seg_init                          */
  /*                         1 to run seg_init                                */
  /*==========================================================================*/
  return 1;
}
/* Second stage low level init to be executed under normal condition, but not when waking up from deep sleep */

/**
  * @brief  Configures interrupt level
  * @param  None
  * @retval None
  */
static void low_level_init2(void)
{
#ifdef CORTEXM3_STM32W108xB
  /* USART bootloader software activation check */
  if ((*((uint32_t *)RAM_BOTTOM) == IAP_BOOTLOADER_APP_SWITCH_SIGNATURE) && (*((uint8_t *)(RAM_BOTTOM+4)) == IAP_BOOTLOADER_MODE_UART))
  {
    uint8_t cut = *(__IO uint8_t *) (FIB_BOTTOM + 0x0798);
    uint16_t offset = 0;
    typedef void (*EntryPoint)(void);     
    offset = (halFixedAddressTable.baseTable.version == 3) ? blOffset[cut - 2] : 0;
    *((uint32_t *)RAM_BOTTOM) = 0;
    if (offset) 
    {
      switchClock();
    }
    EntryPoint entryPoint = (EntryPoint)(*(uint32_t *)(FIB_BOTTOM+4) - offset);
    setStackPointer(*(uint32_t *)FIB_BOTTOM);
    entryPoint();
  }
#elif defined (CORTEXM3_STM32W108CC)
  /* Embedeed USART or RF bootloader software activation check */
  if ((*((uint32_t *)RAM_BOTTOM) == IAP_BOOTLOADER_APP_SWITCH_SIGNATURE))
  {
    typedef void (*EntryPoint)(void);
    EntryPoint entryPoint;
    uint32_t bootAddress;
    *((uint32_t *)RAM_BOTTOM) = 0;
    if ((*((uint8_t *)(RAM_BOTTOM+4)) == IAP_BOOTLOADER_MODE_UART) || (*((uint8_t *)(RAM_BOTTOM+4)) == IAP_BOOTLOADER_MODE_OTA)) 
    {
      bootAddress = (*((uint8_t *)(RAM_BOTTOM+4)) == IAP_BOOTLOADER_MODE_UART) ? FIB_BOTTOM : (FIB1_BOTTOM + 0x800);
      entryPoint = (EntryPoint)(*(uint32_t *)(bootAddress+4));
      setStackPointer(*((uint32_t *)bootAddress));
      entryPoint();
    }
  }
#endif
  
  INTERRUPTS_OFF();

#if (defined (__ICCARM__) || defined (__GNUC__)) 
	asm("CPSIE i");
#elif defined __CC_ARM
    CPSIE();
#else 
 #error "Inline assembler syntax expected"  
#endif
  
}
/*#pragma language=default*/ 

#ifdef __ICCARM__
#pragma language=default
#endif

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
