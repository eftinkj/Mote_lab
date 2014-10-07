/**
  ******************************************************************************
  * @file    sleep.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Main program body
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
#include "micro-common.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup SLEEP
  * @brief sleep modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
/* We don't have a real register to hold this composite information.
  Pretend we do so halGetWakeInfo can operate like halGetResetInfo.
  This "register" is only ever set by halInternalSleep.
   [31] = WakeInfoValid
   [30] = SleepSkipped
   [29] = CSYSPWRUPREQ
   [28] = CDBGPWRUPREQ
   [27] = PWR->WAKECR2
   [26] = TIMER_WAKE_WRAP
   [25] = TIMER_WAKE_COMPB
   [24] = TIMER_WAKE_COMPA
   [23:0] = corresponding GPIO activity */
#define WAKEINFOVALID_INTERNAL_WAKE_EVENT_BIT 31
#define SLEEPSKIPPED_INTERNAL_WAKE_EVENT_BIT  30
#define CSYSPWRUPREQ_INTERNAL_WAKE_EVENT_BIT  29
#define CDBGPWRUPREQ_INTERNAL_WAKE_EVENT_BIT  28
#define WAKECORE_INTERNAL_WAKE_EVENT_BIT      27
#define WRAP_INTERNAL_WAKE_EVENT_BIT          26
#define CMPB_INTERNAL_WAKE_EVENT_BIT          25
#define CMPA_INTERNAL_WAKE_EVENT_BIT          24
/* This define shifts events from the PWR->WAKESR register into the proper
   place in the halInternalWakeEvent variable */
#define INTERNAL_WAKE_EVENT_BIT_SHIFT         20
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint32_t halInternalWakeEvent=0;

/* forceSleepTmrCnt is a flag indicating that the system timer module
   should force a fake value for SLEEPTMR_CNT whenever the timer is
   accessed.  For FogBugz 11909/11920 workaround.*/
boolean forceSleepTmrCnt = FALSE;
/* wakeupSleepTmrCnt is a capturing of the SLEEPTMR_CNT immediately upon
   waking up from deep sleep.  This value is used to know when the counter
   has ticked forward.  For FogBugz 11909/11920 workaround.*/
uint32_t wakeupSleepTmrCnt = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  micro_common_Private_Functions
  * @{
  */

/**
  * @brief Gets the internal wake event info
  * @param None
  * @retval internal wake event info
  */
uint32_t halGetWakeInfo(void)
{
  return halInternalWakeEvent;
}

/**
  * @brief  hal internal sleep mode.
  * @param PWR_SleepMode selects the sleep mode 
  * @retval None
  */
void halInternalSleep(SleepModes sleepMode)
{
  /* Timer restoring always takes place during the wakeup sequence.  We save
     the state here in case SLEEPMODE_NOTIMER is invoked, which would disable
     the clocks.*/
  uint32_t CLK_SLEEPCRSAVED = CLK->SLEEPCR;
  
  /* This code assumes all wake source registers are properly configured.
     As such, it should be called from halSleepWithOptions() or from
     halSleepForQsWithOptions() which configues the wake sources.*/
  
  /* The parameter gpioWakeSel is a bitfield composite of the GPIO wake
     sources derived from the 3 ports, indicating which of the 24 GPIO
     are configured as a wake source.*/
  uint32_t gpioWakeSel  = (PWR->WAKEPAR<<0);
         gpioWakeSel |= (PWR->WAKEPBR<<8);
         gpioWakeSel |= (PWR->WAKEPCR<<16);
  
  /* PB2 is also PWR_WAKECR1_SC1.  Set this wake source if PB2's GPIO wake is set.*/
  if(PWR->WAKEPBR & PWR_WAKEPxR_Px2) {
    PWR->WAKECR1 |= PWR_WAKECR1_SC1;
  }
  
  /* PA2 is also PWR_WAKECR1_SC2.  Set this wake source if PA2's GPIO wake is set.*/
  if(PWR->WAKEPAR & PWR_WAKEPxR_Px2) {
    PWR->WAKECR1 |= PWR_WAKECR1_SC2;
  }
  
  /* The PWR_WAKECR1_IRQD source can come from any pin based on IRQD's sel register.*/
  if(gpioWakeSel & BIT(EXTI->CR[1])) {
    PWR->WAKECR1 |= PWR_WAKECR1_IRQD;
  }
  
  halInternalWakeEvent = 0; /* clear old wake events */
  
  switch(sleepMode)
  {
    case SLEEPMODE_NOTIMER:
      /* The sleep timer clock sources (both RC and XTAL) are turned off.
         Wakeup is possible from only GPIO.  System time is lost.
         NOTE: Timer restoring always takes place during the wakeup sequence.*/
      CLK->SLEEPCR = 0;
      goto deepSleepCore;
    
    case SLEEPMODE_WAKETIMER:
      /* The sleep timer clock sources remain running.  The RC is always
         running and the 32kHz XTAL depends on the board header.  Wakeup
         is possible from both GPIO and the sleep timer.  System time
         is maintained.  The sleep timer is assumed to be configured
         properly for wake events.
         NOTE: This mode assumes the caller has configured the *entire*
              sleep timer properly.*/
      
      if(SLPTMR->IER&SLPTMR_IER_WRAP) {
        PWR->WAKECR1 |= PWR_WAKECR1_WRAP;
      }
      if(SLPTMR->IER&SLPTMR_IER_CMPB) {
        PWR->WAKECR1 |= PWR_WAKECR1_COMPB;
      }
      if(SLPTMR->IER&SLPTMR_IER_CMPA) {
        PWR->WAKECR1 |= PWR_WAKECR1_COMPA;
      }
      /* fall into SLEEPMODE_MAINTAINTIMER's sleep code: */
    case SLEEPMODE_MAINTAINTIMER:
      /* The sleep timer clock sources remain running.  The RC is always
         running and the 32kHz XTAL depends on the board header.  Wakeup
         is possible from only GPIO.  System time is maintained.
         NOTE: System time is maintained without any sleep timer interrupts
              because the hardware sleep timer counter is large enough
              to hold the entire count value and not need a RAM counter.*/

  /*============================================================================
                          ##### Core deep sleep code #####
  ============================================================================*/    
deepSleepCore:
      /* Interrupts *must* be/stay disabled for DEEP SLEEP operation
         INTERRUPTS_OFF will use BASEPRI to disable all interrupts except
         fault handlers and PendSV. */
      INTERRUPTS_OFF();
      /* This is the point of no return.  From here on out, only the interrupt
         sources available in PWR->WAKECR1 will be captured and propagated across
         deep sleep.
        stick all our saved info onto stack since it's only temporary */
      {
        boolean restoreWatchdog = WDG_GetStatus();
        boolean skipSleep = FALSE;
        
        /* Only three register blocks keep power across deep sleep:
            CM_HV, GPIO, SLOW_TIMERS  */

        /* All other register blocks lose their state across deep sleep:
            MAC, SECURITY, SERIAL, TMR1, TMR2, EVENT, CM_LV, RAM_CTRL,
            AUX_ADC, FLASH_CONTROL, ITM, DWT, FPB, NVIC, TPIU */

        /* The sleep code will only save and restore registers where it is
           meaningful and necessary to do so.  In most cases, there must still
           be a powerup function to restore proper state.*/

        /* NOTE: halPowerUp() and halPowerDown() will always be called before
           and after this function.  halPowerDown and halPowerUp should leave
           the modules in a safe state and then restart the modules.
           (For example, shutting down and restarting Timer1) */

        /* ----BASEBAND
                  reinitialized by stStackPowerUp() */
        /*----MAC
                  reinitialized by stStackPowerUp() */
        /*----SECURITY
                  reinitialized by stStackPowerUp() */
        /*----SERIAL
                  reinitialized by halPowerUp() or similar */
        /*----TMR1
                  reinitialized by halPowerUp() or similar */
        /*----TMR2
                  reinitialized by halPowerUp() or similar */
        /*----EVENT
          SRC or FLAG interrupts are not saved or restored
          MISS interrupts are not saved or restored
          MAC_RX_INT_MASK - reinitialized by stStackPowerUp()
          MAC_TX_INT_MASK - reinitialized by stStackPowerUp()
          MAC_TIMER_INT_MASK - reinitialized by stStackPowerUp()
          BB_INT_MASK - reinitialized by stStackPowerUp()
          SEC_INT_MASK - reinitialized by stStackPowerUp() */
        uint32_t SLPTMR_IERSAVED = SLPTMR->IER;
        uint32_t MGMT_IT_IERSAVED = MGMT_IT->IER;
        /* TIM1_IT->IER - reinitialized by halPowerUp() or similar
           TIM2_IT->IER - reinitialized by halPowerUp() or similar
           SC1_IT->IER - reinitialized by halPowerUp() or similar
           SC2_IT->IER - reinitialized by halPowerUp() or similar
           ADC->IER - reinitialized by halPowerUp() or similar */
        uint32_t EXTI_TSRSAVED_0 = EXTI->TSR[0];
        uint32_t EXTI_TSRSAVED_1 = EXTI->TSR[1];
        uint32_t EXTI_TSRSAVED_2 = EXTI->TSR[2];
        uint32_t EXTI_TSRSAVED_3 = EXTI->TSR[3];
        /* SC1_IT->ICR - reinitialized by halPowerUp() or similar
           SC2_IT->ICR - reinitialized by halPowerUp() or similar */
        /* ----CM_LV */
        uint32_t CLK_HSECR1SAVED = CLK->HSECR1;
        uint32_t CLK_HSICRSAVED = CLK->HSICR;
        uint32_t CLK_DITHERCRSAVED = CLK->DITHERCR;
        /* CLK->HSECR2 - reinitialized by halPowerUp() or similar
           CLK->CPUCR  - reinitialized by halPowerUp() or similar
           TMR1_CLK_SEL - reinitialized by halPowerUp() or similar
           TMR2_CLK_SEL - reinitialized by halPowerUp() or similar */
        uint32_t GPIO_PCTRACECRSAVED = GPIO_DBG->PCTRACECR;
        /*----RAM_CTRL */
        uint32_t MEM_RAMPROTR1SAVED = MEM->RAMPROTR1;
        uint32_t MEM_RAMPROTR2SAVED = MEM->RAMPROTR2;
        uint32_t MEM_RAMPROTR3SAVED = MEM->RAMPROTR3;
        uint32_t MEM_RAMPROTR4SAVED = MEM->RAMPROTR4;
        uint32_t MEM_RAMPROTR5SAVED = MEM->RAMPROTR5;
        uint32_t MEM_RAMPROTR6SAVED = MEM->RAMPROTR6;
        uint32_t MEM_RAMPROTR7SAVED = MEM->RAMPROTR7;
        uint32_t MEM_RAMPROTR8SAVED = MEM->RAMPROTR8;
        uint32_t MEM_RAMCRSAVED = MEM->RAMCR;
        /*----AUX_ADC
                  reinitialized by halPowerUp() or similar */
        /*----CAL_ADC
                  reinitialized by stStackPowerUp() */
        /*----FLASH_CONTROL
                  configured on the fly by the flash library */
        /*----ITM
                  reinitialized by halPowerUp() or similar */
        /*----DWT
                  not used by software on chip */
        /*----FPB
                  not used by software on chip */
        /*----NVIC
            SysTick->CTRL- fixed, restored by cstartup when exiting deep sleep
           SysTick->LOAD - fixed, restored by cstartup when exiting deep sleep */
        uint32_t NVIC_ISERSAVED = NVIC->ISER[0]; //mask against wake sources
        /* NVIC->ISPR[0] - used below when overlapping interrupts and wake sources
           NVIC_IPR_3to0 - fixed, restored by cstartup when exiting deep sleep
           NVIC_IPR_7to4 - fixed, restored by cstartup when exiting deep sleep
           NVIC_IPR_11to8 - fixed, restored by cstartup when exiting deep sleep
           NVIC_IPR_15to12 - fixed, restored by cstartup when exiting deep sleep
           NVIC_IPR_19to16 - fixed, restored by cstartup when exiting deep sleep */
        uint32_t SCB_VTORSAVED = SCB->VTOR;
        /* SCB->CCR - fixed, restored by cstartup when exiting deep sleep
           SCB->SHP[0] - fixed, restored by cstartup when exiting deep sleep
           SCB->SHP[1] - fixed, restored by cstartup when exiting deep sleep
           SCB->SHP[2] - fixed, restored by cstartup when exiting deep sleep
           SCB->SHCSR - fixed, restored by cstartup when exiting deep sleep */
        /*----TPIU
                  reinitialized by halPowerUp() or similar*/

        /* stmDebugPowerDown() should have shutdown the DWT/ITM/TPIU already.*/
        
        /* freeze input to the GPIO from LV (alternate output functions freeze)*/
        PWR->DSLEEPCR1 = PWR_DSLEEPCR1_LVFREEZE;
        /* record GPIO state for wake monitoring purposes
           By having a snapshot of GPIO state, we can figure out after waking
           up exactly which GPIO could have woken us up.
           Reading the three IN registers is done separately to avoid warnings
           about undefined order of __IO access. */
        uint32_t GPIO_IN_SAVED =   GPIOA->IDR;
               GPIO_IN_SAVED |= (GPIOB->IDR<<8);
               GPIO_IN_SAVED |= (GPIOC->IDR<<16);
        /* reset the power up events by writing 1 to all bits. */
        PWR->WAKESR = 0xFFFFFFFF;

        /* By clearing the events, the wake up event capturing is activated.
          At this point we can safely check our interrupt flags since event
          capturing is now overlapped.  Up to now, interrupts indicate
          activity, after this point, powerup events indicate activity.
          If any of the interrupt flags are set, that means we saw a wake event
          sometime while entering sleep, so we need to skip over sleeping */

        /*--possible interrupt sources for waking:
            IRQA, IRQB, IRQC, IRQD
            SleepTMR CMPA, CMPB, Wrap
            PWR->WAKECR2 (DebugIsr)*/

        /* check for IRQA interrupt and if IRQA (PB0) is wake source */
        if((NVIC->ISPR[0]&NVIC_IxxR_IRQA) && (PWR->WAKEPBR&PWR_WAKEPxR_Px0) && (PWR->WAKECR1&PWR_WAKECR1_MONEN))
        {
          skipSleep = TRUE;
          /* log IRQA as a wake event */
          halInternalWakeEvent |= BIT(PORTB_PIN(0));
        }
        /* check for IRQB interrupt and if IRQB (PB6) is wake source */
        if((NVIC->ISPR[0]&NVIC_IxxR_IRQB) && (PWR->WAKEPBR&PWR_WAKEPxR_Px6) && (PWR->WAKECR1&PWR_WAKECR1_MONEN))
        {
          skipSleep = TRUE;
          /* log IRQB as a wake event */
          halInternalWakeEvent |= BIT(PORTB_PIN(6));
        }
        /* check for IRQC interrupt and if IRQC (EXTI->CR[0]) is wake source */
        if((NVIC->ISPR[0]&NVIC_IxxR_IRQC) && (gpioWakeSel&BIT(EXTI->CR[0])) && (PWR->WAKECR1&PWR_WAKECR1_MONEN))
        {
          skipSleep = TRUE;
          /* log IRQC as a wake event */
          halInternalWakeEvent |= BIT(EXTI->CR[0]);
        }
        /* check for IRQD interrupt and if IRQD (EXTI->CR[1]) is wake source */
        if((NVIC->ISPR[0]&NVIC_IxxR_IRQD) && (gpioWakeSel&BIT(EXTI->CR[1])) && ((PWR->WAKECR1&PWR_WAKECR1_MONEN) || (PWR->WAKECR1&PWR_WAKECR1_IRQD)))
        {
          skipSleep = TRUE;
          /* log IRQD as a wake event */
          halInternalWakeEvent |= BIT(EXTI->CR[1]);
        }
        /* check for SleepTMR CMPA interrupt and if SleepTMR CMPA is wake source */
        if((NVIC_IxxR_SLEEPTMR&SLPTMR_IER_CMPA) && (PWR->WAKECR1&PWR_WAKECR1_COMPA))
        {
          skipSleep = TRUE;
          /* log SleepTMR CMPA as a wake event */
          halInternalWakeEvent |= BIT32(CMPA_INTERNAL_WAKE_EVENT_BIT);
        }
        /*check for SleepTMR CMPB interrupt and if SleepTMR CMPB is wake source */
        if((NVIC_IxxR_SLEEPTMR&SLPTMR_IER_CMPB) && (PWR->WAKECR1&PWR_WAKECR1_COMPB))
        {
          skipSleep = TRUE;
          /* log SleepTMR CMPB as a wake event */
          halInternalWakeEvent |= BIT32(CMPB_INTERNAL_WAKE_EVENT_BIT);
        }
        /* check for SleepTMR WRAP interrupt and if SleepTMR WRAP is wake source */
        if((NVIC_IxxR_SLEEPTMR&SLPTMR_IER_WRAP) && (PWR->WAKECR1&PWR_WAKECR1_WRAP))
        {
          skipSleep = TRUE;
          /* log SleepTMR WRAP as a wake event */
          halInternalWakeEvent |= BIT32(WRAP_INTERNAL_WAKE_EVENT_BIT);
        }
        /* check for Debug interrupt and if PWR->WAKECR2 is wake source */
        if((NVIC->ISPR[0]&NVIC_IxxR_DEBUG) && (PWR->WAKECR1&PWR_WAKECR1_CORE))
        {
          skipSleep = TRUE;
          /* log PWR->WAKECR2 as a wake event */
          halInternalWakeEvent |= BIT32(WAKECORE_INTERNAL_WAKE_EVENT_BIT);
        }
        
        /* only propagate across deep sleep the interrupts that are both
        enabled and possible wake sources */
        {
          uint32_t wakeSourceInterruptMask = 0;
          
          if(PWR->WAKEPBR & PWR_WAKEPxR_Px0)
          {
            wakeSourceInterruptMask |= NVIC_IxxR_IRQA;
          }
          if(PWR->WAKEPBR & PWR_WAKEPxR_Px6)
          {
            wakeSourceInterruptMask |= NVIC_IxxR_IRQB;
          }
          if(gpioWakeSel&BIT(EXTI->CR[0]))
          {
            wakeSourceInterruptMask |= NVIC_IxxR_IRQC;
          }
          if(gpioWakeSel&BIT(EXTI->CR[1]))
          {
            wakeSourceInterruptMask |= NVIC_IxxR_IRQD;
          }
          if((PWR->WAKECR1&PWR_WAKECR1_COMPA) || (PWR->WAKECR1&PWR_WAKECR1_COMPB) ||
             (PWR->WAKECR1&PWR_WAKECR1_WRAP))
          {
            wakeSourceInterruptMask |= NVIC_IxxR_SLEEPTMR;
          }
          if(PWR->WAKECR1&PWR_WAKECR1_CORE) 
          {
            wakeSourceInterruptMask |= NVIC_IxxR_DEBUG;
          }
          
          NVIC_ISERSAVED &= wakeSourceInterruptMask;
        }

        /* disable watchdog while sleeping (since we can't reset it asleep) */
        WDG_Cmd(DISABLE);
        
      /*  The chip is not allowed to enter a deep sleep mode (which could
          cause a core reset cycle) while CSYSPWRUPREQ is set.  CSYSPWRUPREQ
          indicates that the debugger is trying to access sections of the
          chip that would get reset during deep sleep.  Therefore, a reset
          cycle could very easily cause the debugger to error and we don't
          want that.  While the power management state machine will stall
          if CSYSPWRUPREQ is set (to avoid the situation just described),
          in this stalled state the chip will not be responsive to wake
          events.  To be sensitive to wake events, we must handle them in
          software instead.  To accomplish this, we request that the
          CSYSPWRUPACK be inhibited (which will indicate the debugger is not
          connected).  But, we cannot induce deep sleep until CSYSPWRUPREQ/ACK
          go low and these are under the debuggers control, so we must stall
          and wait here.  If there is a wake event during this time, break
          out and wake like normal.  If the ACK eventually clears,
          we can proceed into deep sleep.  The PWR->CSYSPWRUPACKCR
          functionality will hold off the debugger (by holding off the ACK)
          until we are safely past and out of deep sleep.  The power management
          state machine then becomes responsible for clearing
          PWR->CSYSPWRUPACKCR and responding to a CSYSPWRUPREQ with a
          CSYSPWRUPACK at the right/safe time.*/
        PWR->CSYSPWRUPACKCR = PWR_CSYSPWRUPACKCR_INHIBIT;
        {
          /* Use a local copy of PWR->WAKECR1 to avoid warnings from the compiler
             about order of __IO accesses */
          uint32_t wakeSel = PWR->WAKECR1;
          /* stall until a wake event or CSYSPWRUPREQ/ACK clears */
          while( (PWR->CSYSPWRUPACKSR) && (!(PWR->WAKESR&wakeSel)) ) {}
          /* if there was a wake event, allow CSYSPWRUPACK and skip sleep */
          if(PWR->WAKESR&wakeSel)
          {
            PWR->CSYSPWRUPACKCR = 0x00;
            skipSleep = TRUE;
          }
        }

        if(!skipSleep)
        {
          /* FogBugz 7283 states that we must switch to the OSCHF when entering
             deep sleep since using the 24MHz XTAL could result in RAM
             corruption.  This switch must occur at least 2*24MHz cycles before
             sleeping.*/
          /* FogBugz 8858 states that we cannot go into deep-sleep when the
             chip is clocked with the 24MHz XTAL with a duty cycle as low as
             70/30 since this causes power_down generation timing to fail.*/
          CLK->HSECR2 &= ~CLK_HSECR2_SW1;
          /* If DS12 needs to be forced regardless of state, clear
             PWR->DSLEEPCR2 here.  This is hugely dangerous and
             should only be done in very controlled chip tests.*/
          SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;      /* enable deep sleep */
          extern __IO boolean halPendSvSaveContext;
          halPendSvSaveContext = 1;          /* 1 means save context */
          /* The INTERRUPTS_OFF used at the beginning of this function set
             BASEPRI such that the only interrupts that will fire are faults
             and PendSV.  Trigger PendSV now to induce a context save.*/
          SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;    /* pend the context save and Dsleep */
          /* Since the interrupt will not fire immediately it is possible to
             execute a few lines of code.  To stay halted in this spot until the
             WFI instruction, spin on the context flag (which will get cleared
             during the startup sequence when restoring context).*/
          while(halPendSvSaveContext) {}
          /* I AM ASLEEP.  WHEN EXECUTION RESUMES, CSTARTUP WILL RESTORE TO HERE
             Save the value of the SLEEPTMR_CNT register immediately after
             waking up from a real sleep.  For FogBugz 11909/11920 workaround.*/
          wakeupSleepTmrCnt = SLPTMR->CNTH<<16;
          wakeupSleepTmrCnt |= SLPTMR->CNTL;
        } 
        else 
        {
          /* Record the fact that we skipped sleep */
          halInternalWakeEvent |= BIT32(SLEEPSKIPPED_INTERNAL_WAKE_EVENT_BIT);
          /* If this was a true deep sleep, we would have executed cstartup and
             PRIMASK would be set right now.  If we skipped sleep, PRIMASK is not
             set so we explicitely set it to guarantee the powerup sequence
             works cleanly and consistently with respect to interrupt
             dispatching and enabling.*/
          __disable_irq();
        }
        
        /* Clear the interrupt flags for all wake sources. This
           is necessary because if we don't execute an actual deep sleep cycle
           the interrupt flags will never be cleared.  By clearing the flags,
           we always mimick a real deep sleep as closely as possible and
           guard against any accidental interrupt triggering coming out
           of deep sleep.  (The interrupt dispatch code coming out of sleep
           is responsible for translating wake events into interrupt events,
           and if we don't clear interrupt flags here it's possible for an
           interrupt to trigger even if it wasn't the true wake event.)*/
        SLPTMR->ISR = (SLPTMR_IER_CMPA | SLPTMR_IER_CMPB | SLPTMR_IER_WRAP);
        EXTI->PR = (EXTI_PR_IRQAP | EXTI_PR_IRQBP | EXTI_PR_IRQCP | EXTI_PR_IRQDP);
        
        /* immediately restore the registers we saved before sleeping
           so IRQ and SleepTMR capture can be reenabled as quickly as possible
           this is safe because our global interrupts are still disabled
           other registers will be restored later */

        CLK->SLEEPCR = CLK_SLEEPCRSAVED;
        SLPTMR->IER = SLPTMR_IERSAVED;
        MGMT_IT->IER = MGMT_IT_IERSAVED;
        EXTI->TSR[0] = EXTI_TSRSAVED_0;
        EXTI->TSR[1] = EXTI_TSRSAVED_1;
        EXTI->TSR[2] = EXTI_TSRSAVED_2;
        EXTI->TSR[3] = EXTI_TSRSAVED_3;
        CLK->HSECR1 = CLK_HSECR1SAVED;
        CLK->HSICR = CLK_HSICRSAVED;
        CLK->DITHERCR = CLK_DITHERCRSAVED;
        GPIO_DBG->PCTRACECR = GPIO_PCTRACECRSAVED;
        MEM->RAMPROTR1 = MEM_RAMPROTR1SAVED;
        MEM->RAMPROTR2 = MEM_RAMPROTR2SAVED;
        MEM->RAMPROTR3 = MEM_RAMPROTR3SAVED;
        MEM->RAMPROTR4 = MEM_RAMPROTR4SAVED;
        MEM->RAMPROTR5 = MEM_RAMPROTR5SAVED;
        MEM->RAMPROTR6 = MEM_RAMPROTR6SAVED;
        MEM->RAMPROTR7 = MEM_RAMPROTR7SAVED;
        MEM->RAMPROTR8 = MEM_RAMPROTR8SAVED;
        MEM->RAMCR = MEM_RAMCRSAVED;
        NVIC->ISER[0] = NVIC_ISERSAVED;
        SCB->VTOR = SCB_VTORSAVED;
        
        /* PWR->WAKECR2/NVIC_IxxR_DEBUG and INT_IRQx is cleared by NVIC->ICPR[0] below */
        NVIC->ICPR[0] = 0xFFFFFFFF;
        
        /* Now that we're awake, normal interrupts are operational again
           Take a snapshot of the new GPIO state and the EVENT register to
           record our wake event*/
        uint32_t GPIO_IN_NEW =   GPIOA->IDR;
               GPIO_IN_NEW |= (GPIOB->IDR<<8);
               GPIO_IN_NEW |= (GPIOC->IDR<<16);
        /* Only operate on power up events that are also wake events.  Power
           up events will always trigger like an interrupt flag, so we have
           to check them against events that are enabled for waking. (This is
           a two step process because we're accessing two __IO values.)*/
        uint32_t powerUpEvents = PWR->WAKESR;
               powerUpEvents &= PWR->WAKECR1;
        halInternalWakeEvent |= ((GPIO_IN_SAVED^GPIO_IN_NEW)&gpioWakeSel);
        /* PWR_WAKESR_SC1 is PB2 which is bit 10 */
        halInternalWakeEvent |= (!!(powerUpEvents&PWR_WAKESR_SC1))<<((1*8)+2);
        /* PWR_WAKESR_SC2 is PA2 which is bit 2 */
        halInternalWakeEvent |= (!!(powerUpEvents&PWR_WAKESR_SC2))<<((0*8)+2);
        /* PWR_WAKESR_IRQD is chosen by EXTI->CR[1] */
        halInternalWakeEvent |= (!!(powerUpEvents&PWR_WAKESR_IRQD))<<(EXTI->CR[1]);
        halInternalWakeEvent |= ((powerUpEvents & 
                                  (PWR_WAKESR_CSYSPWRRUPREQ  |
                                   PWR_WAKESR_CPWRRUPREQ  |
                                   PWR_WAKESR_CORE      |
                                   PWR_WAKESR_WRAP  |
                                   PWR_WAKESR_COMPB |
                                   PWR_WAKESR_COMPA ))
                                          <<INTERNAL_WAKE_EVENT_BIT_SHIFT);
        /* at this point wake events are fully captured and interrupts have
           taken over handling all new events */
        /* Bring limited interrupts back online.  INTERRUPTS_OFF will use
           BASEPRI to disable all interrupts except fault handlers and PendSV.
           PRIMASK is still set though (global interrupt disable) so we need
           to clear that next.*/
        INTERRUPTS_OFF();
        
        /* Now that BASEPRI has taken control of interrupt enable/disable,
           we can clear PRIMASK to reenable global interrupt operation.*/
        __enable_irq();
        
        /* wake events are saved and interrupts are back on track,
           disable gpio freeze */
        PWR->DSLEEPCR1 = 0x00;
        
        /* restart watchdog if it was running when we entered sleep
           do this before dispatching interrupts while we still have tight
           control of code execution*/
        if(restoreWatchdog) 
        {
          WDG_Cmd(ENABLE);
        }
        
        /* Pend any interrupts associated with deep sleep wake sources.  The
           restoration of NVIC->ISER[0] above and the changing of BASEPRI below
           is responsible for proper dispatching of interrupts at the end of
           halSleepWithOptions.*/

        /* The PWR->WAKECR2 wake source triggers a Debug Interrupt. If NVIC_IxxR_DEBUG
           interrupt is enabled and PWR->WAKECR2 is a wake event, then pend the
           Debug interrupt (using the wake_core bit).*/
        if((NVIC->ISER[0]&NVIC_IxxR_DEBUG) &&
            (halInternalWakeEvent&BIT(WAKECORE_INTERNAL_WAKE_EVENT_BIT)))
        {
          PWR->WAKECR2 = PWR_WAKECR2_COREWAKE;
        }

        /* The SleepTMR CMPA is linked to a real ISR.  If the SleepTMR CMPA
           interrupt is enabled and CMPA is a wake event, then pend the CMPA
           interrupt (force the second level interrupt).*/
        if((SLPTMR->IER&SLPTMR_IER_CMPA) &&
            (halInternalWakeEvent&BIT(CMPA_INTERNAL_WAKE_EVENT_BIT)))
        {
          SLPTMR->IFR = SLPTMR_IER_CMPA;
        }

        /* The SleepTMR CMPB is linked to a real ISR.  If the SleepTMR CMPB
           interrupt is enabled and CMPB is a wake event, then pend the CMPB
           interrupt (force the second level interrupt).*/
        if((SLPTMR->IER&SLPTMR_IER_CMPB) &&
            (halInternalWakeEvent&BIT(CMPB_INTERNAL_WAKE_EVENT_BIT)))
        {
          SLPTMR->IFR = SLPTMR_IER_CMPB;
        }

        /* The SleepTMR WRAP is linked to a real ISR.  If the SleepTMR WRAP
           interrupt is enabled and WRAP is a wake event, then pend the WRAP
           interrupt (force the second level interrupt).*/
        if((SLPTMR->IER&SLPTMR_IER_WRAP) &&
            (halInternalWakeEvent&BIT(WRAP_INTERNAL_WAKE_EVENT_BIT)))
        {
          SLPTMR->IFR = SLPTMR_IER_WRAP;
        }

        /* The four IRQs are linked to a real ISR.  If any of the four IRQs
           triggered, then pend their ISR */
        /* If the IRQA interrupt mode is enabled and IRQA (PB0) is wake
           event, then pend the interrupt. */
        if(((EXTI->TSR[0]&EXTI_TSR_INTMOD)!=0) && (halInternalWakeEvent&BIT(PORTB_PIN(0))))
        {
          NVIC->ISPR[0] = NVIC_IxxR_IRQA;
        }
        /* If the IRQB interrupt mode is enabled and IRQB (PB6) is wake
           event, then pend the interrupt. */
        if(((EXTI->TSR[1]&EXTI_TSR_INTMOD)!=0) && (halInternalWakeEvent&BIT(PORTB_PIN(6))))
        {
          NVIC->ISPR[0] = NVIC_IxxR_IRQB;
        }
        /* If the IRQC interrupt mode is enabled and IRQC (EXTI->CR[0]) is wake
           event, then pend the interrupt. */
        if(((EXTI->TSR[2]&EXTI_TSR_INTMOD)!=0) && (halInternalWakeEvent&BIT(EXTI->CR[0])))
        {
          NVIC->ISPR[0] = NVIC_IxxR_IRQC;
        }
        /* If the IRQD interrupt mode is enabled and IRQD (EXTI->CR[1]) is wake
           event, then pend the interrupt. */
        if(((EXTI->TSR[3]&EXTI_TSR_INTMOD)!=0) && (halInternalWakeEvent&BIT(EXTI->CR[1])))
        {
          NVIC->ISPR[0] = NVIC_IxxR_IRQD;
        }

        /* Due to FogBugz 11909/11920, SLEEPTMR_CNT may not have updated yet so
           we must ensure that the CNT register updates before returning.  It's
           only necessary to wait for the CNT to update when we've gone to
           sleep, the SLEEPTMR is enabled, and the sleep mode used a timer.
           This code could delay for up to 1ms, but will return as soon as it
           can.  In the situation where the chip slept for a known amount of
           time, this code will not delay and instead the system timer will
           report a fake, but accurate time.*/
        if((!skipSleep) && (SLPTMR->CR&SLPTMR_CR_EN) &&
           (CLK->SLEEPCR&CLK_SLEEPCR_LSI10KEN) && (sleepMode!=SLEEPMODE_NOTIMER))
        {
          uint32_t currSleepTmrCnt;
          
         #ifdef BUG11909_WORKAROUND_C
          /* THIS WORKAROUND IS NOT PROVEN 100% RELIABLE.  THIS SHOULD NOT BE
             USED UNTIL IT IS PROVEN PERFECTLY RELIABLE.
             This workaround attempts to force the SLEEPTMR_CNT to tick sooner
             than normal.  It does so by toggling between the clock sources
             to get the CNT to increment.  There is a chance the SLEEPTMR_CNT
             could become random doing this! */
          {
            currSleepTmrCnt = SLPTMR->CNTH<<16;
            currSleepTmrCnt |= SLPTMR->CNTL;
            if(currSleepTmrCnt == wakeupSleepTmrCnt) {
              uint32_t GPIOC_ODRSAVED = GPIOC->ODR;
              uint32_t GPIOC_CRHSAVED = GPIOC->CRH;
              uint32_t SLPTMR_CRSAVED = SLPTMR->CR;
              /* It is not necessary to do anything with CLK->SLEEPCR.*/
              GPIOC->BSR = GPIO_BSR_BS7;
              SET_REG_FIELD(GPIOC->CRH, GPIO_BSR_BS7, GPIO_Mode_OUT_PP, 12);
              do {
                /* Toggling between RC/XTAL will produce a clock edge
                   into the timer and cause CNT to increment.*/
                SLPTMR->CR ^= SLPTMR_CR_CLKSEL;
                currSleepTmrCnt = SLPTMR->CNTH<<16;
                currSleepTmrCnt |= SLPTMR->CNTL;
              } while(currSleepTmrCnt == wakeupSleepTmrCnt);
              GPIOC->ODR = GPIOC_ODRSAVED;
              GPIOC->CRH = GPIOC_CRHSAVED;
              SLPTMR->CR = SLPTMR_CRSAVED;
              forceSleepTmrCnt = FALSE;
            }
          }
          #endif /* BUG11909_WORKAROUND_C */
          
          /* Knowing that halSleepTimerIsr is about to be taken (when
             interrupts get enabled) tells us that the chip woke up due
             to the timer and therefore sleepTmrArtificalCnt is valid
             and needs to be forced.  This allows us to bypass delaying
             for SLEEPTMR_CNT to tick forward.  For FogBugz 11909/11920
             workaround.*/
          if((NVIC->ISER[0]&NVIC_IxxR_SLEEPTMR) && (NVIC->ISPR[0]&NVIC_IxxR_SLEEPTMR)) {
            /* sleepTmrArtificalCnt was set before sleeping
               by halSleepForQuarterSeconds */
            forceSleepTmrCnt = TRUE;
          } else {
            uint16_t us = 1000;
            uint32_t beginTime;
            forceSleepTmrCnt = FALSE;
            /* It is possible to be in a situation where the SLEEPTMR is no
               longer ticking (32k XTAL issues).  To guard against getting
               stuck in this loop, use the MAC Timer to timeout after 1ms (since
               that is the maximum time this loop would normally delay for).*/
            MACTMR->CR |= MACTMR_CR_EN;
            if((CLK->HSECR2&CLK_HSECR2_SW1)!=CLK_HSECR2_SW1) {
              us >>= 1;
            }
            beginTime = MACTMR->CNTR;
            do{
              currSleepTmrCnt = SLPTMR->CNTH<<16;
              currSleepTmrCnt |= SLPTMR->CNTL;
            }while((currSleepTmrCnt == wakeupSleepTmrCnt) &&
                   (((MACTMR->CNTR - beginTime)&MACTMR_CNTR_CNT) < us));
          }
        }
      }

      /* Mark the wake events valid just before exiting */
      halInternalWakeEvent |= BIT32(WAKEINFOVALID_INTERNAL_WAKE_EVENT_BIT);
      
      /* We are now reconfigured, appropriate ISRs are pended, and ready to go,
         so enable interrupts! */
      INTERRUPTS_ON();
      
      break; /* and deep sleeping is done! */
    
    case SLEEPMODE_IDLE:
      /* Only the CPU is idled.  The rest of the chip continues runing
         normally.  The chip will wake from any interrupt.*/
      {
        boolean restoreWatchdog = WDG_GetStatus();
        /* disable watchdog while sleeping (since we can't reset it asleep)*/
        WDG_Cmd(DISABLE);
        /* Normal ATOMIC/INTERRUPTS_OFF/INTERRUPTS_ON uses the BASEPRI mask
           to juggle priority levels so that the fault handlers can always
           be serviced.  But, the WFI instruction is only capable of
           working with the PRIMASK bit.  Therefore, we have to switch from
           using BASEPRI to PRIMASK to keep interrupts disabled so that the
           WFI can return on an interrupt
           Globally disable interrupts with PRIMASK */
        __disable_irq();
        /* Bring the BASEPRI up to 0 to allow interrupts (but still disabled
           with PRIMASK)*/
        INTERRUPTS_ON();
        /* an internal function call is made here instead of injecting the
        "WFI" assembly instruction because injecting assembly code will
        cause the compiler's optimizer to reduce efficiency.*/
#ifdef FLASH_ACCESS_FLASH_LP
        BYPASS_MPU(
                   uint32_t FLASH_ACCESS_SAVED = FLASH_ACCESS;
                   FLASH->ACR&= ~FLASH_ACCESS_FLASH_LP;
                   halInternalIdleSleep();
                   FLASH->ACR= FLASH_ACCESS_SAVED;
                   )
#else
        halInternalIdleSleep();
#endif /* FLASH_ACCESS_FLASH_LP */
        /* restart watchdog if it was running when we entered sleep */
        if(restoreWatchdog)
          WDG_Cmd(ENABLE);
        /* The WFI instruction does not actually clear the PRIMASK bit, it
           only allows the PRIMASK bit to be bypassed.  Therefore, we must
           manually clear PRIMASK to reenable all interrupts.*/
        __enable_irq();
      }
      break;
      
    default:
      /* Oops! Invalid sleepMode parameter.*/
      assert(0);
  }
}

/**
  * @brief Configures the sleep mode option.
  * @param PWR_SleepMode selects the sleep mode 
  *        This parameter can be any of the @ref PWR_SleepModes_TypeDef enumeration.
  * @param gpioWakeBitMask selects the gpio wake bit mask 
  * @retval None
  */
void halSleepWithOptions(SleepModes sleepMode, uint32_t gpioWakeBitMask)
{
  /* configure all GPIO wake sources */
  PWR->WAKEPAR = (gpioWakeBitMask>>0)&0xFF;
  PWR->WAKEPBR = (gpioWakeBitMask>>8)&0xFF;
  PWR->WAKEPCR = (gpioWakeBitMask>>16)&0xFF;
  
  /* use the defines found in the board file to choose our wakeup source(s)*/
  PWR->WAKECR1 = 0;  /* start with no wake sources */
  
  /* if any of the GPIO wakeup monitor bits are set, enable the top level
     GPIO wakeup monitor */
  if((PWR->WAKEPAR)||(PWR->WAKEPBR)||(PWR->WAKEPCR))
  {
    PWR->WAKECR1 |= PWR_WAKECR1_MONEN;
  }
  /* NOTE: The use of PWR_WAKECR1_CPWRRUPREQ should not be necessary since asserting
     CDBGPWRUPREQ will bring the chip to DS0 where the debug components are
     maintained but it's not necessary to completely wake to the running state.*/
  
  /* always wakeup when the debugger attempts to access the chip */
  PWR->WAKECR1 |= PWR_WAKECR1_CSYSPWRRUPREQ;

  /* always wakeup when the debug channel attempts to access the chip */
  PWR->WAKECR1 |= PWR_WAKECR1_CORE;
  /* the timer wakeup sources are enabled below in POWERSAVE, if needed */
  
  /* wake sources are configured so do the actual sleeping */
  halInternalSleep(sleepMode);
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
