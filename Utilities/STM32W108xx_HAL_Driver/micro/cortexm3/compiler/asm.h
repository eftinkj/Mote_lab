/**
  ******************************************************************************
  * @file    asm.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    09-October-2012
  * @brief   asm header file
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
  
#ifndef __ASM_H__
#define __ASM_H__

#ifdef __IAR_SYSTEMS_ASM__

/* Includes ------------------------------------------------------------------*/

/** @addtogroup HAL
  * @{
  */

/** @defgroup asm
  * @{
  */
  
/* Private define ------------------------------------------------------------*/
    
  /* IAR V5 Definitions. */
  #define __END__                  END
  #define __EXPORT__               EXPORT
  #define __IMPORT__               IMPORT
  #define __SPACE__                DS8
  #define __EQU__(a, b)            a EQU b
  #define __WEAK__                 PUBWEAK
  #define __THUMB__                THUMB

  #if __VER__ >= 5000000

    /* IAR V5 Definitions. */
          RSEG RESETINFO:DATA
          RSEG CSTACK:DATA
    #define __CODE__                    SECTION .text:CODE:REORDER:NOROOT(2)
    #define __BSS__                     SECTION .bss:DATA:NOROOT(2)
    #define __BEGIN_RESETINFO__(offset) SFB(RESETINFO + offset)
    #define __END_RESETINFO__(offset)   SFE(RESETINFO + offset)
    #define __BEGIN_STACK__(offset)     SFB(CSTACK + offset)
    #define __END_STACK__(offset)       SFE(CSTACK + offset)
    #define __CFI__(x)

  #else /* __VER__ */
    
    #error IAR versions less that 5.xx are not supported

  #endif /* __VER__ */

#endif /* __IAR_SYSTEMS_ASM__ */

#ifdef __GNUC__

  /* GCC Definitions. */
      .syntax unified
      .thumb
  #define __CODE__                 .text
  #define __THUMB__                .thumb_func
  #define __BSS__                  .bss
  #define __END__                  .end
  #define __EXPORT__               .global
  #define __IMPORT__               .extern
  #define __SPACE__                .space
  #define __EQU__(a, b)            .equ a, b
  #define __WEAK__                 .weak
  #define __BEGIN_NVDATA__(offset) (_noinit + offset)
  #define __BEGIN_STACK__(offset)  (_stack + offset)
  #define __END_STACK__(offset)    (_estack + offset)
  #define __CFI__(x)

#endif /* __GNUC__ */
#ifdef __CC_ARM

  #define __END__                  END
  #define __EXPORT__               EXPORT
  #define __IMPORT__               IMPORT
  #define __SPACE__                SPACE
  #define __EQU__(a, b)            a EQU b
 // #define __WEAK__                 PUBWEAK
 // #define __THUMB__                THUMB
          AREA RESETINFO,DATA
          AREA CSTACK,DATA
  #define __CODE__                    AREA |.text|,CODE//.text
  #define __BSS__                     .bss
 // #define __BEGIN_RESETINFO__(offset) SFB(RESETINFO + offset)
 // #define __END_RESETINFO__(offset)   SFE(RESETINFO + offset)
 // #define __BEGIN_STACK__(offset)     __initial_sp + offset
 // #define __END_STACK__(offset)       SFE(CSTACK + offset)
  #define __CFI__(x)
	
#endif /*ARMCC*/

#endif /* __ASM_H__ */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
