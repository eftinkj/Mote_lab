/**
  ******************************************************************************
  * @file    iar.h 
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    09-October-2012 
  * @brief   Compiler and Platform specific definitions and typedefs for the
  *  IAR ARM C compiler.
  * @note iar.h should be included first in all source files by setting the
  *  preprocessor macro PLATFORM_HEADER to point to it. iar.h automatically
  *  includes platform-common.h.
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
#ifndef __IAR_H__
#define __IAR_H__
    
/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#ifdef RF_LIBRARIES_BUILDING /* internal define */
 #include "Libraries/SimpleMAC/superphy-source/stack/platform/micro/stm32w108/regs.h" 
#endif
/** @addtogroup HAL
  * @{
  */

/** @defgroup IAR_ARM_C_compiler
  * @brief IAR ARM C compiler
  * @{
  */
/* Private define ------------------------------------------------------------*/

#ifndef __ICCARM__
  #error Improper PLATFORM_HEADER
#endif

#if (__VER__ < 6040000)
  #error Only IAR EWARM versions later than 6.40.1 are supported
#endif /* __VER__ */

  #include <intrinsics.h>
  #include <stdarg.h>
#if defined (CORTEXM3_STM32W108)
    #include "stm32w108xx.h"
#else
    #error Unknown CORTEXM3 micro
#endif /* CORTEXM3_STM32W108 */
/* Provide a default NVIC configuration file.  The build process can
   override this if it needs to. */
#ifndef NVIC_CONFIG
   #define NVIC_CONFIG "nvic-config.h"
#endif /* NVIC_CONFIG */

/* suppress warnings about unknown pragmas
   (as they may be pragmas known to other platforms) */
#pragma diag_suppress = pe161

/* Define that the minimal hal is being used.*/
#define MINIMAL_HAL

/**
  * @brief A typedef to make the size of the variable explicitly known.
  * These are a set of typedefs to make the size of all variable declarations
  * explicitly known.
  */
typedef unsigned char  boolean;
typedef unsigned int   PointerType;

/* NOTE!! IF THIS VALUE IS CHANGED, NVIC-CONFIG.H MUST ALSO BE UPDATED */
#define INTERRUPTS_DISABLED_PRIORITY (12 << 3)

/**
  * @brief Use the Master Program Memory Declarations from platform-common.h
  */
#define _HAL_USE_COMMON_PGM_

/** @defgroup  Miscellaneous Macros
  * @{
  */

/**
  * @brief A convenient method for code to know what endiannes processor
  * it is running on.  For the Cortex-M3, we are little endian.
  */
#define BIGENDIAN_CPU  FALSE

/**
  * @brief A friendlier name for the compiler's intrinsic for not
  * stripping.
  */
#define NO_STRIPPING(var)  __root var
#define NO_INIT(var) __no_init var

/**
  * @brief A friendlier name for the compiler's intrinsic for eeprom
  * reference.
  */
#define EEPROM  errorerror

#ifndef __SOURCEFILE__
/**
  * @brief The __SOURCEFILE__ macro is used by asserts to list the
  * filename if it isn't otherwise defined, set it to the compiler intrinsic
  * which specifies the whole filename and path of the sourcefile
  */
#define __SOURCEFILE__ __FILE__
#endif

#ifndef BOOTLOADER
#undef __delay_cycles
/**
  * @brief __delay_cycles() is an intrinsic IAR call; however, we
  * have explicity disallowed it since it is too specific to the system clock.
  * \note Please use halCommonDelayMicroseconds() instead, because it correctly
  * accounts for various system clock speeds.
  */
#define __delay_cycles(x)  please_use_halCommonDelayMicroseconds_instead_of_delay_cycles
#endif

/**
  * @brief Set debug level based on whether or DEBUG is defined.
  * For the STM32W108xx, basic debugging support is included if DEBUG is not defined.
  */
#ifndef DEBUG_LEVEL
  #ifdef DEBUG
    #define DEBUG_LEVEL FULL_DEBUG
  #else
    #define DEBUG_LEVEL BASIC_DEBUG
  #endif
#endif

/**
  * @brief Set the application start and end address.
  * This are useful to detect whether an image is for bootloader mode or not.
  * This can be used also to clone image to another node via bootloader.
  */
#define APPLICATION_IMAGE_START ((uint32_t) __section_begin("FLASH_IMAGE"))
#define APPLICATION_IMAGE_END ((uint32_t) __section_end ("FLASH_IMAGE"))

/**
 * @brief Set the application start and end address of RAM used
 */
#define RAM_START ((uint32_t) __section_begin("RAM_BLOCK"))
#define RAM_END ((uint32_t) __section_end ("RAM_BLOCK"))


/**
  * @brief Macro to reset the watchdog timer.  Note:  be very very
  * careful when using this as you can easily get into an infinite loop if you
  * are not careful.
  */
#define WDG_ReloadCounter()  WDG_ReloadCounter()

/**
  * @brief Define __attribute__ to nothing since it isn't handled by IAR.
  */
#define __attribute__(nothing)

/**
  * @brief GPIO_PxOUT Bit Field Values
  */
#define GPIOOUT_PULLUP            (0x1u)
#define GPIOOUT_PULLDOWN          (0x0u)

/**
  * @brief Declare a variable as unused to avoid a warning.  Has no effect
  * in IAR builds
  */
#define UNUSED

/**
  * @brief Some platforms need to cast enum values that have the high bit set.
  */
#define SIGNED_ENUM

/**
  * @brief Define the magic value that is interpreted by IAR C-SPY's Stack View.
  */
#define STACK_FILL_VALUE  0xCDCDCDCD

/**
  * @brief Define a generic RAM function identifier to a compiler specific one.
  */
#ifdef RAMEXE
  /* If the whole build is running out of RAM, as chosen by the RAMEXE build
     define, then define RAMFUNC to nothing since it's not needed.*/
  #define RAMFUNC
#else
  #define RAMFUNC __ramfunc
#endif /* RAMEXE */

/**
 * @brief Define a generic no operation identifier to a compiler specific one.
 */
#define NO_OPERATION() __no_operation()

/**
  * @brief A convenience macro that makes it easy to change the field of a
  * register to any value.
  */
#define SET_REG_FIELD(reg, field, value, pos)                          \
  do{                                                                  \
    reg = ((reg & (~field)) | (uint32_t) (((uint32_t) value) << pos)); \
    }while(0)

/**
  * @brief Stub for code not running in simulation.
  */
#define simulatedTimePasses()

/**
  * @brief Stub for code not running in simulation.
  */
#define simulatedTimePassesMs(x)

/**
  * @brief Stub for code not running in simulation.
  */
#define simulatedSerialTimePasses()

/**
  * @brief Use the Divide and Modulus Operations from platform-common.h
  */
#define _HAL_USE_COMMON_DIVMOD_

/**
  * @brief Provide a portable way to specify the segment where a variable
  * lives.
  */
#define VAR_AT_SEGMENT(__variableDeclaration, __segmentName) \
    __variableDeclaration @ __segmentName

#define _QUOTEME(a) #a
#define QUOTEME(a) _QUOTEME(a)
/**
  * @brief Provide a portable way to align variables to specific requirements
  */
#define ALIGN_VAR(__variableDeclaration, alignment) _Pragma(QUOTEME(data_alignment=alignment)) \
 __variableDeclaration

/**
  * @brief Provide a portable way to define weak functions
  */
#define WEAK_FUNCTION(function) __weak function

/**
  * @brief Provide a portable way to define weak variables
  */
#define WEAK_VARIABLE(var) __weak var 

/**
  * @}
  */ 
   
/** @defgroup  Portable segment names
  * @{
  */   

/**
  * @brief Portable segment names
  */
#define __NO_INIT__       ".noinit"
#define __INTVEC__ ".intvec"
#define __CSTACK__ "CSTACK"
#define __DATA_INIT__ ".data_init"
#define __DATA__ ".data"
#define __BSS__ ".bss"
#define __CONST__ ".rodata"
#define __TEXT__ ".text"
#define __TEXTRW_INIT__ ".textrw_init"
#define __TEXTRW__ ".textrw"
#define __FAT__ "FAT" // Fixed address table
#define __NVM__ "NVM" //Non-Volatile Memory data storage

//=============================================================================
// The '#pragma segment=' declaration must be used before attempting to access
// the segments so the compiler properly handles the __segment_*() functions.
//
// The segment names used here are the default segment names used by IAR. Refer
// to the IAR Compiler Reference Guide for a proper description of these
// segments.
//=============================================================================
#pragma segment=__NO_INIT__
#pragma segment=__INTVEC__
#pragma segment=__CSTACK__
#pragma segment=__DATA_INIT__
#pragma segment=__DATA__
#pragma segment=__BSS__
#pragma segment=__CONST__
#pragma segment=__TEXT__
#pragma segment=__TEXTRW_INIT__
#pragma segment=__TEXTRW__
#pragma segment=__FAT__
#pragma segment=__NVM__
/* Special pragma to get the application image start and end address */
#pragma segment="FLASH_IMAGE"
#pragma segment="RAM_BLOCK"

/* MPU is unused with this platform header variant */
#define _HAL_MPU_UNUSED_

/**
  * @}
  */ 
   
/** @defgroup  Global Interrupt Manipulation Macros
  * @brief The special purpose BASEPRI register is used to enable and disable
  * interrupts while permitting faults.
  * When BASEPRI is set to 1 no interrupts can trigger. The configurable faults
  * (usage, memory management, and bus faults) can trigger if enabled as well as 
  * the always-enabled exceptions (reset, NMI and hard fault).
  * When BASEPRI is set to 0, it is disabled, so any interrupt can triggger if 
  * its priority is higher than the current priority.
  * @{
  */

#define ATOMIC_LITE(blah) ATOMIC(blah)
#define DECLARE_INTERRUPT_STATE_LITE DECLARE_INTERRUPT_STATE
#define DISABLE_INTERRUPTS_LITE() DISABLE_INTERRUPTS()
#define RESTORE_INTERRUPTS_LITE() RESTORE_INTERRUPTS()

#ifdef BOOTLOADER
    /* The bootloader does not use interrupts */
    #define DECLARE_INTERRUPT_STATE
    #define DISABLE_INTERRUPTS() do { } while(0)
    #define RESTORE_INTERRUPTS() do { } while(0)
    #define INTERRUPTS_ON() do { } while(0)
    #define INTERRUPTS_OFF() do { } while(0)
    #define INTERRUPTS_ARE_OFF() (FALSE)
    #define ATOMIC(blah) { blah }
    #define HANDLE_PENDING_INTERRUPTS() do { } while(0)
    #define SET_BASE_PRIORITY_LEVEL(basepri) do { } while(0)
#else  /* BOOTLOADER */
  
/**
  * @brief This macro should be called in the local variable
  * declarations section of any function which calls DISABLE_INTERRUPTS()
  * or RESTORE_INTERRUPTS().
  */
#define DECLARE_INTERRUPT_STATE uint8_t _emIsrState
    
/* The core Global Interrupt Manipulation Macros start here. */
  
/**
  * @brief Disable interrupts, saving the previous state so it can be
  * later restored with RESTORE_INTERRUPTS().
  * @note Do not fail to call RESTORE_INTERRUPTS().
  * @note It is safe to nest this call.
  */
#define DISABLE_INTERRUPTS()                      \
    do {                                          \
      _emIsrState = __get_BASEPRI();              \
      __set_BASEPRI(INTERRUPTS_DISABLED_PRIORITY);                          \
    } while(0)
  
/** 
  * @brief Restore the global interrupt state previously saved by
  * DISABLE_INTERRUPTS() 
  * \note Do not call without having first called DISABLE_INTERRUPTS()
  * to have saved the state.
  * \note It is safe to nest this call.
  */
#define RESTORE_INTERRUPTS()      \
    do {                          \
      __set_BASEPRI(_emIsrState); \
    } while(0)
  
/**
  * @brief Enable global interrupts without regard to the current or
  * previous state.
  */
#define INTERRUPTS_ON()               \
    do {                              \
      __set_BASEPRI(0);               \
    } while(0)
  
/**
  * @brief Disable global interrupts without regard to the current or
  * previous state.
  */
#define INTERRUPTS_OFF()                                \
    do {                                                \
      __set_BASEPRI(INTERRUPTS_DISABLED_PRIORITY);      \
    } while(0)
  
/** @brief Compare BASEPRI to the priority used to disable normal interrupts (not faults). 
  * BASEPRI = 0 means all interrupts are allowed.  If BASEPRI is not
  * 0, then the value in BASEPRI indicates which interrupts are allowed.
  * Interrupts with a priority number < than BASEPRI are allowed but
  * interrupts with a priority number >= than BASEPRI are disabled.
  * The line of equivalent c code:
  *  return ((BASEPRI < INTERRUPTS_DISABLED_PRIORITY) ? 0 : 1);
  *
  * NOTE: Normal interrupts have a priority number higher than
  * INTERRUPTS_DISABLED_PRIORITY, but faults and utility exceptions have
  * a lower number so they can still be serviced when interrupts are off.
  *
  * NOTE: This function is designed to work as a compliment to _enableBasePri
  * and _disableBasePri which toggle between 0 and
  * INTERRUPTS_DISABLED_PRIORITY.  This function is not designed to
  * work with every value of BASEPRI and could return incorrect status
  * depending on the value in BASEPRI and the priority number of the
  * exceptions(s) in question.
  * @returns TRUE if global interrupts are disabled.
  */
#define INTERRUPTS_ARE_OFF() ((__get_BASEPRI() < INTERRUPTS_DISABLED_PRIORITY) ? 0 : 1)
  
/**
  * @returns TRUE if global interrupt flag was enabled when 
  * ::DISABLE_INTERRUPTS() was called.
  */
#define INTERRUPTS_WERE_ON() (_emIsrState == 0)
  
/**
  * @brief A block of code may be made atomic by wrapping it with this
  * macro.  Something which is atomic cannot be interrupted by interrupts.
  */
#define ATOMIC(blah)           \
  {                            \
    DECLARE_INTERRUPT_STATE;   \
    DISABLE_INTERRUPTS();      \
    { blah }                   \
    RESTORE_INTERRUPTS();      \
  }
  
/**
  * @brief Allows any pending interrupts to be executed. Usually this
  * would be called at a safe point while interrupts are disabled (such as
  * within an ISR).
  * 
  * Takes no action if interrupts are already enabled.
  */
#define HANDLE_PENDING_INTERRUPTS()   \
    do {                              \
      if (INTERRUPTS_ARE_OFF()) {     \
        INTERRUPTS_ON();              \
        INTERRUPTS_OFF();             \
      }                               \
   } while (0)
  
/**
  * @brief Sets the base priority mask (BASEPRI) to the value passed,
  * bit shifted up by PRIGROUP_POSITION+1.  This will inhibit the core from
  * taking all interrupts with a preemptive priority equal to or less than
  * the BASEPRI mask.  This macro is dependent on the value of
  * PRIGROUP_POSITION in nvic-config.h. Note that the value 0 disables the
  * the base priority mask.
  *
  * Refer to the "PRIGROUP" table in nvic-config.h to know the valid values
  * for this macro depending on the value of PRIGROUP_POSITION.  With respect
  * to the table, this macro can only take the preemptive priority group
  * numbers denoted by the parenthesis.
  */
#define SET_BASE_PRIORITY_LEVEL(basepri)   \
    do {                                   \
      __set_BASEPRI(basepri);              \
    } while(0)
  
#endif /* BOOTLOADER */
 
/**
  * @}
  */ 
   
/** @defgroup  External Declarations
  * @brief These are routines that are defined in certain header files that we don't
  * want to include, e.g. stdlib.h
  * @{
  */      

/**
  * @brief Use the C Standard Library Memory Utilities from platform-common.h
  */
#define _HAL_USE_COMMON_MEMUTILS_

/**
  * @brief Returns the absolute value of I (also called the magnitude of I).
  * That is, if I is negative, the result is the opposite of I, but if I is
  * nonnegative the result is I.
  *
  * @param I  An integer.
  *
  * @return A nonnegative integer.
  */
int abs(int I);


/**
  * @brief Include platform-common.h last to pick up defaults and common definitions.
  */
#define PLATCOMMONOKTOINCLUDE
  #include "platform-common.h"
#undef PLATCOMMONOKTOINCLUDE

#endif /* __IAR_H__ */


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

