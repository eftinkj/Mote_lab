/**
  ******************************************************************************
  * @file    system-timer.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   STM32W108 System Timer HAL functions.
  * @verbatim
  *
 ===============================================================================
                              ##### Note #####
 ===============================================================================
  * The Sleep Timer count and compare registers are only 16 bits, but
  * the counter and comparators themselves are actually 32bits.  To deal with
  * this, there are High ("H") and Low ("L") registers.  The High register is
  * the "action" register.  When working with SLEEPTMR_CNT, reading the "H"
  * register will return the upper 16 bits and simultaneously trigger the
  * capture of the lower 16bits in the "L" register.  The "L" register may then
  * be read.  When working with the SLEEPTMR_CMP registers, writing "L" will
  * set a shadow register.  Writing "H" will cause the value from the "H" write
  * and the "L" shadow register to be combined and simultaneously loaded into
  * the true 32bit comparator.


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
#include "error.h"
#include "micro-common.h"
#include "system-timer.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup system-timer
  * @brief system-timer modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
#define CONVERT_QS_TO_TICKS(x) ((x) << 8)
#define CONVERT_TICKS_TO_QS(x) ((x) >> 8)
#define TIMER_MAX_QS           0x1000000 /* = 4194304 seconds * 4 = 16777216 */

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* A simple flag used by internalSleepForQs to check that it has exited
   from sleep mode at the request of the expected timer interrupt. */
static boolean sleepTimerInterruptOccurred = FALSE;

/* sleepTmrArtificalCnt is the value expected for the SLEEPTMR_CNT if
   internalSleepForQs completed successfully due to a timer wakeup.  This
   is the value artifically returned when forceSleepTmrCnt is TRUE. */
uint32_t sleepTmrArtificalCnt = 0;

/* forceSleepTmrCnt is a flag indicating that the system timer module
   should force a fake value for SLEEPTMR_CNT whenever the timer is
   accessed.  For FogBugz 11909/11920 workaround. */
extern boolean forceSleepTmrCnt;

/* wakeupSleepTmrCnt is a capturing of the SLEEPTMR_CNT immediately upon
   waking up from deep sleep.  This value is used to know when the counter
   has ticked forward.  For FogBugz 11909/11920 workaround. */
extern uint32_t wakeupSleepTmrCnt;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  system-timer_Private_Functions
  * @{
  */

/**
  * @brief  Return a 16 bit real time clock value. 
  * @param  None
  * @retval 16 bit real time clock value
  * @note   With 1024 clock ticks per second,
  *         a single clock tick occurs every 0.9769625 milliseconds, and a rollover
  *         of the 16-bit timer occurs every 64 seconds.
  */
uint16_t halCommonGetInt16uMillisecondTick(void)
{
  return (uint16_t)halCommonGetInt32uMillisecondTick();
}

/**
  * @brief  Return a 16 bit real time clock value. 
  * @param  None
  * @retval 16 bit real time clock value
  */
uint16_t halCommonGetInt16uQuarterSecondTick(void)
{
  return (uint16_t)(halCommonGetInt32uMillisecondTick() >> 8);
}

/**
  * @brief  Return a 32 bit real time clock value. 
  * @param  None
  * @retval 32 bit real time clock value
  * @note   With 1024 clock ticks per second,
  *         a single clock tick occurs every 0.9769625 milliseconds, and a rollover
  *         of the 32-bit timer occurs approximately every 48.5 days.
  */
uint32_t halCommonGetInt32uMillisecondTick(void)
{
  uint32_t currentTime;
  currentTime = SLPTMR->CNTH<<16;
  currentTime |= SLPTMR->CNTL;
  
/* forceSleepTmrCnt is set in halInternalSleep instead of performing
   the delay.  When we slept for exactly the correct amount of time
   we know what the time is supposed to be.  So, as long as the timer
   counter is still stuck on a stale value, we can return the
   value it should be (according to halSleepForQuarterSeconds). */
  if(forceSleepTmrCnt && (currentTime == wakeupSleepTmrCnt)) 
  {
    return sleepTmrArtificalCnt;
  }
  forceSleepTmrCnt = FALSE;
  
  return currentTime;
}

/**
  * @brief  Sleep the ISR Timer
  * @param  None
  * @retval None
  */
void halSleepTimerIsr(void)
{
  /* clear the second level interrupts */
  SLPTMR->ISR = SLPTMR_IER_WRAP | SLPTMR_IER_CMPA | SLPTMR_IER_CMPB;
  
  /* mark a sleep timer interrupt as having occurred */
  sleepTimerInterruptOccurred = TRUE;
}

/**
  * @brief  Sleep for specific time
  * @param  boolean value of use Gpio Wake Mask
  * @param  Duration
  * @param  The value of the Gpio Wake Mask
  * @retval ST status
  */
static StStatus internalSleepForQs(boolean useGpioWakeMask, uint32_t *duration, uint32_t gpioWakeBitMask)
{
  StStatus status = ST_SUCCESS;
  uint32_t sleepOverflowCount;
  uint32_t remainder;
  uint32_t startCount;
  
  /* There is really no reason to bother with a duration of 0qs */
  if(*duration==0)
  {
    INTERRUPTS_ON();
    return status;
  }
  
  ATOMIC(
         /* disable top-level interrupt while configuring */
         NVIC->ICER[0] = NVIC_IxxR_SLEEPTMR;
         
       /* Our tick is calibrated to 1024Hz, giving a tick of 976.6us and an
          overflow of 4194304.0 seconds.  Calculate the number of sleep overflows
          in the duration */
         sleepOverflowCount = (*duration)/TIMER_MAX_QS;
         /* calculate the remaining ticks */
         remainder = CONVERT_QS_TO_TICKS((*duration)%TIMER_MAX_QS);
         /* grab the starting sleep count */
         startCount = halCommonGetInt32uMillisecondTick();
         
         sleepTimerInterruptOccurred = FALSE;
         
         if(remainder) 
         {
           /* set CMPA value */
           SLPTMR->CMPAL = (startCount+remainder)&0xFFFF;
           SLPTMR->CMPAH = ((startCount+remainder)>>16)&0xFFFF;
           /* clear any stale interrupt flag and set the CMPA interrupt */
           SLPTMR->ISR = SLPTMR_IER_CMPA;
           SLPTMR->IER = SLPTMR_IER_CMPA;
         }
         if(sleepOverflowCount) 
         {
           /* set CMPB value */
           SLPTMR->CMPBL = startCount&0xFFFF;
           SLPTMR->CMPBH = (startCount>>16)&0xFFFF;
         /* clear any stale interrupt flag and set the CMPB interrupt
            this will also disable the CMPA interrupt, since we only want to wake
            on overflows, not the remainder yet */
           SLPTMR->ISR = SLPTMR_IER_CMPB;
           SLPTMR->IER = SLPTMR_IER_CMPB;
         }
         
         /* enable top-level interrupt */
         NVIC->ISER[0] = NVIC_IxxR_SLEEPTMR;
         )
    
    while(*duration > 0)
    {
    /* Record what the SLEEPTMR_CNT is supposed to be when waking up
       after a timed sleep.  halInternalSleep is responsible for setting
       the forceSleepTmrCnt flag. */
      if(SLPTMR->IER&SLPTMR_IER_CMPB)
      {
        /* use the value written into SLEEPTMR_CMPB */
        sleepTmrArtificalCnt = startCount;
      } 
      else 
      { /* SLPTMR->IER&SLPTMR_IER_CMPA */
        /* use the value written into SLEEPTMR_CMPA */
        sleepTmrArtificalCnt = (startCount+remainder);
      }
      
      halSleepWithOptions(SLEEPMODE_WAKETIMER, gpioWakeBitMask);
      
      SLPTMR->IER = 0x00; /* disable all SleepTMR interrupts */
      
    /* If we didn't come out of sleep via a compare or overflow interrupt,
       it was an abnormal sleep interruption; report the event. */
      if(!sleepTimerInterruptOccurred) 
      {
        status = ST_SLEEP_INTERRUPTED;
      /* Update duration to account for how long last sleep was.  Using the
         startCount variable is always valid because full timer wraps always
         return to this value and the remainder is an offset from this value.
         And since the duration is always reduced after each full timer wrap,
         we only need to calculate the final duration here. */
        *duration -= CONVERT_TICKS_TO_QS(halCommonGetInt32uMillisecondTick() -
                                         startCount);
        break;
      } else
      {
        if(sleepOverflowCount) 
        {
          sleepOverflowCount--;
          *duration -= TIMER_MAX_QS;
        } else 
        {
          *duration -= CONVERT_TICKS_TO_QS(remainder);
        }
        sleepTimerInterruptOccurred = FALSE;
        if(sleepOverflowCount) 
        {
          /* enable sleeping for a full timer wrap */
          SLPTMR->ISR = SLPTMR_IER_CMPB;
          SLPTMR->IER = SLPTMR_IER_CMPB;
        } else if(!sleepOverflowCount && (*duration>0))
        {
          /* enable sleeping for the remainder */
          SLPTMR->ISR = SLPTMR_IER_CMPA;
          SLPTMR->IER = SLPTMR_IER_CMPA;
        }
      }
    }
  
  return status;
}

/**
  * @brief  Sleep for Milliseconds
  * @param  boolean value of use Gpio Wake Mask
  * @param  Duration
  * @param  The value of the Gpio Wake Mask
  * @retval ST status
  */
static StStatus internalSleepForMilliseconds(boolean useGpioWakeMask, uint32_t *duration, uint32_t gpioWakeBitMask)
{
  StStatus status = ST_SUCCESS;
  uint32_t startCount;
  
  /* There is really no reason to bother with a duration of 0 ms */
  if(*duration==0) 
  {
    INTERRUPTS_ON();
    return status;
  }
  
  ATOMIC(
         /* disable top-level interrupt while configuring */
         NVIC->ICER[0] = NVIC_IxxR_SLEEPTMR;
         
         /* Our tick is calibrated to 1024Hz, giving a tick of 976.6us and an
          * overflow of 4194304.0 seconds.  As a result, our seconds are comprised of
          * 1024 "milliseconds". */
         
         /* grab the starting sleep count */
         startCount = halCommonGetInt32uMillisecondTick();
         sleepTimerInterruptOccurred = FALSE;
         /* set CMPA value */
         SLPTMR->CMPAL = (startCount + *duration) & 0xFFFF;
         SLPTMR->CMPAH = ((startCount + *duration) >> 16) & 0xFFFF;
         /* clear any stale interrupt flag and set the CMPA interrupt */
         SLPTMR->ISR = SLPTMR_IER_CMPA;
         SLPTMR->IER = SLPTMR_IER_CMPA;
         /* enable top-level interrupt */
         NVIC->ISER[0] = NVIC_IxxR_SLEEPTMR;
         )
    
    while(*duration > 0) 
    {
      /* Record what the SLEEPTMR_CNT is supposed to be when waking up
       * after a timed sleep.  halInternalSleep is responsible for setting
       * the forceSleepTmrCnt flag. */
      sleepTmrArtificalCnt = (startCount + *duration);
      
      halSleepWithOptions(SLEEPMODE_WAKETIMER, gpioWakeBitMask);
      
      SLPTMR->IER = 0x00; /* disable all SleepTMR interrupts */
      
      /* If we didn't come out of sleep via a compare or overflow interrupt,
       * it was an abnormal sleep interruption; report the event. */
      if(!sleepTimerInterruptOccurred) 
      {
        status = ST_SLEEP_INTERRUPTED;
        /* Update duration to account for how long last sleep was.  Using the
         * startCount variable is valid because full timer wraps do not occur
         * in this function. */
        *duration -= halCommonGetInt32uMillisecondTick() - startCount;
        break;
      } else
      {
        /* We have completed the full sleep cycle uninterrupted, set duration to 0
         * so we exit the function */
        *duration = 0;
        sleepTimerInterruptOccurred = FALSE;
      }
    }
  
  return status;
}

/**
  * @brief  Uses the system timer to enter SLEEPMODE_WAKETIMER 
  *         for approximately the specified amount of time 
  * @param  duration
  * @param  gpio wake bit mask
  * @retval None
  */
StStatus halSleepForQsWithOptions(uint32_t *duration, uint32_t gpioWakeBitMask)
{
  return internalSleepForQs(TRUE, duration, gpioWakeBitMask);
}

/**
  * @brief  Uses the system timer to enter SLEEPMODE_WAKETIMER 
  *         for approximately the specified amount of time 
  * @param  duration
  * @param  gpio wake bit mask
  * @retval None
  */
StStatus halSleepForMillisecondsWithOptions(uint32_t *duration, uint32_t gpioWakeBitMask)
{
  return internalSleepForMilliseconds(TRUE, duration, gpioWakeBitMask);
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
