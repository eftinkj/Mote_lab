;;/**
;;  ******************************************************************************
;;  * @file    context-switch.s
;; * @author  MCD Application Team
;;  * @version V2.0.0RC1
;;  * @date    27-July-2012
;;  * @brief   Context save/restore for deep sleep using the PendSV exception.
;;  *
;;  *  @verbatim
;;  *
;;  *
;;    ===========================================================================
;;                         ##### How to use this driver #####
;;    ===========================================================================
;;      [..]
;;       # This file also contains a simple halInternalIdleSleep() function that
;;         executes just the WFI instruction for idle sleeping.
;;     
;;       # When the STM32W108XX enters deep sleep, the hardware will actually remove power
;;         from the Cortex-M3 core (in Deep Sleep 0, power is not removed but the core
;;         is held in reset).  Since this will clear the internal state of the core, it
;;         must be properly restored such that execution can resume from the sleep code.
;;         The simplest and most secure mechanism to do this is to perform a context save
;;         and restore.  Context save/restore is almost identical to a context switch
;;         used in multi-threaded systems with the main difference being only one stack
;;         pointer is used and the save/restore operations are disjoint.
;;     
;;      # When an interrupt is triggered in the STM32W108XX, the core automatically saves 8
;;        of the 16 CPU registers on the stack.  The ISR then only needs to save the
;;        other 8 registers and store the resulting stack pointer.  Restoring is the
;;        reverse operation where 8 registers are manually copied back with the other 8
;;        being restored on the return from interrupt.
;;     
;;      # As its last act, the deep sleep code will trigger the PendSV exception to
;;        perform a context save. When the core is booted upon deep sleep exit, the
;;        RESET_EVENT register informs cstartup if the chip just exited deep sleep.
;;        Cstartup will then trigger halTriggerContextRestore which sets up the stack
;;        pointer and trigger the PendSV exception to perform a restore.  When PendSV
;;        returns from interrupt context the system will be back at the same point it
;;        was before deep sleep.
;;              
;;    @endverbatim
;;  ******************************************************************************
;;  * @attention
;;  *
;;  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
;;  *
;;  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
;;  * You may not use this file except in compliance with the License.
;;  * You may obtain a copy of the License at:
;;  *
;;  *        http://www.st.com/software_license_agreement_liberty_v2
;;  *
;;  * Unless required by applicable law or agreed to in writing, software 
;;  * distributed under the License is distributed on an "AS IS" BASIS, 
;;  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;;  * See the License for the specific language governing permissions and
;;  * limitations under the License.
;;  *
;;  ******************************************************************************  
;;  */

;;/* Includes ------------------------------------------------------------------*/
#include "compiler/asm.h"

;;/**
;;  * @name boolean halPendSvSaveContext
;;  * @brief A simple boolean flag used to indicate if a context save or a context restore
;;  * should be performed.  Since context switching is handled with the PendSV
;;  * interrupt, parameters cannot be passed into the ISR and as such this boolean
;;  * flag is used. If this flag is zero, PendSV should perform a context restore.
;;  * If this flag is non-zero, PendSV should perform a context save.
;;  * @param None.
;;  * @retval None.
;;  * @note The smallest unit of storage is a single byte.
;;  */

        
 
        __EXPORT__ halPendSvSaveContext

halPendSvSaveContext
        __SPACE__ 1

;;/**
;;  * @name uint32_t savedMSP
;;  * @brief Private storage to hold the saved stack pointer.  This variable is only used
;;  * in this file and should not be extern'ed.  In our current design we
;;  * do not use real context switching, but only context saving and restoring.
;;  * As such, we only need to keep track of the Main Stack Pointer (MSP). This
;;  * variable is used to hold the MSP between a save and a restore.
;;  * @param None.
;;  * @retval None.
;;  */

        __EXPORT__ savedMSP
savedMSP
        __SPACE__ 4

;;/**
;;  * @name void halPendSvIsr(void)
;;  * @brief This ISR is installed by cstartup in the vector table for the PendSV
;;  * exception.  The purpose of this ISR is to either save the current context
;;  * and trigger sleeping through the 'WFI' instruction, or restore a
;;  * previous context.  The variable halPendSvSaveContext is used to
;;  * decide if a save or a restore is performed.  Since entering/exiting interrupt
;;  * context automatically saves/restores 8 of the 16 CPU registers on the stack
;;  * we need to manually save the other 8 onto the stack as well.
;;  * When a context save is complete, the stack will have been expanded by 16
;;  * words with the current Stack Pointer stored in savedMSP.
;;  * When a context restore is complete, the stack will have been shrunk by 16
;;  * words with the old context restored after the return instruction.
;;  * @param None.
;;  * @retval None.
;;  * @note The IAR default handler name for PendSV, PendSV_Handler, is also
;;  * instantiated here so it routes to the same code as the St name halPendSvIsr.
;;  */
        __CODE__
        THUMB


        __EXPORT__ halPendSvIsr
halPendSvIsr
        LDR  R0, =halPendSvSaveContext  ;//load the variable's address
        LDRB R0, [R0]           ;//get the value in the variable
        CBZ  R0, contextRestore ;//if variable is zero, branch to contextRestore
contextSave
        MRS  R0, MSP          ;//load the main stack pointer into R0
        SUB  R0, R0, #0x20    ;//make room on the stack for 8 words (32 bytes)
        MSR  MSP, R0          ;//load new MSP from adjusted stack pointer
        STM  R0, {R4-R11}     ;//store R4-R11 (8 words) onto the stack
        LDR  R1, =savedMSP    ;//load address of savedMSP into R1
        STR  R0, [R1]         ;//store the MSP into savedMSP
        WFI                   ;//all saved, trigger deep sleep
        ;;// Even if we fall through the WFI instruction, we will immediately
        ;;// execute a context restore and end up where we left off with no
        ;;// ill effects.  Normally at this point the core will either be
        ;;// powered off or reset (depending on the deep sleep level).
contextRestore
        LDR  R0, =savedMSP    ;//load address of savedMSP into R0
        LDR  R0, [R0]         ;//load the MSP from savedMSP
        LDM  R0, {R4-R11}     ;//load R4-R11 (8 words) from the stack
        ADD  R0, R0, #0x20    ;//eliminate the 8 words (32 bytes) from the stack
        MSR  MSP, R0          ;//restore the MSP from R0
        BX   LR               ;//return to the old context
        
;;/**
;;  * @name void halTriggerContextRestore(void)
;;  * @brief Cstartup is responsible for triggering a context restore based upon the
;;  * RESET_EVENT register.  Since the stack pointer sits at the top of memory
;;  * after the core boots, cstartup cannot simply trigger a PendSV to restore
;;  * context as this will cause existing stack data to be over written.  Cstartup
;;  * disables interrupts, pends PendSV, and then calls this function. This
;;  * function simply configures the Stack Pointer to be past the previous data
;;  * such that when interrupts are enabled and PendSV fires it wont corrupt
;;  * previous data.
;;  * @param None.
;;  * @retval None.
;;  */

          THUMB
        __EXPORT__ halTriggerContextRestore
halTriggerContextRestore
        LDR   R0, =savedMSP    ;//load address of savedMSP into R0
        LDR   R0, [R0]         ;//load the MSP from savedMSP
        MSR   MSP, R0          ;//restore the MSP from R0
        CPSIE i                ;//enable interrupts and let PendSV fire
        BX    LR               ;//this return should never be triggered
        
;;/**
;;  * @name void halInternalIdleSleep(void)
;;  * @brief A simple internal function call (to be called from halSleep) for executing
;;  * the WFI instruction and entering the simple, idle sleep state.
;;  * The NOP before and after the WFI are necessary due to an issue where
;;  * lowest idle current is not obtained if the execution chain is BL > WFI > BX
;;  * and the WFI instruction exists at an address that ends with 0x0, 0x2, 0x8,
;;  * or 0xA (for these addresses, ignore the least significant bit since that is
;;  * the T-Bit and must be set on Cortex-M3 CPUs).  See case 11768.
;;  * @param None.
;;  * @retval None.
;;  */

     THUMB
        __EXPORT__ halInternalIdleSleep
halInternalIdleSleep
        NOP
        WFI                    ;//trigger idle sleep
        NOP
		    NOP                    // to avoid warning with Keil
        BX   LR               ; //return
        
        __END__
        
;;/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


