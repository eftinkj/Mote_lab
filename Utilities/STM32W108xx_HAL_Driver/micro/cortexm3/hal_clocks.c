/**
  ******************************************************************************
  * @file    clock.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the clock peripheral:
  *           + Internal/external clocks, 
  *           + Modes management
  *
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
#include "hal.h"
#include "mpu.h"
#include "mfg-token.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup hal clocks
  * @brief hal clocks driver modules
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* brief Set this define to 1 to run CPU clock at 12MHz to achieve some power
  consumption savings */
#ifndef CORTEXM3_STM32W108_CPU_CLOCK_12MHz
#define CORTEXM3_STM32W108_CPU_CLOCK_12MHz 0
#endif

/* Provide a simple means for enabling calibration debug output */
#define CALDBG(x)
/* #define CALDBG(x) x */

/* The xtal needs to wait DEFAULT_XTAL_SETTLING_TIME_US microseconds after the
   bias is changed from max to normal to give the xtal time to settle.
   100us was chosen as a safe value because it was ~10x larger than the worst
   case measured settling time with the xtals tested in May 2011 by svec */
#define DEFAULT_XTAL_SETTLING_TIME_US (100)

/* Some builds don't want to use the complex idle delay code which requires
interrupts. The define DISABLE_IDLING_DELAY simply swaps the fancy
idleDelayMicroseconds for the traditional halCommonDelayMicroseconds */
#ifdef DISABLE_IDLING_DELAY
  #define idleDelayMicroseconds(usDelay) halCommonDelayMicroseconds(usDelay)
#else /* DISABLE_IDLING_DELAY */

/* The slowest frequency for the 10kHz RC source is 8kHz (125us).  The PERIOD
register updates every 16 cycles, so to be safe 17 cycles = 2125us.  But,
we need twice this maximum time because the period measurement runs
asynchronously, and the value of CLK->LSI10KCR is changed immediately before
the delay */
#define SLOWRC_PERIOD_SETTLE_TIME 4250

/* The CLK->PERIODSR register measures the number of 12MHz clock cycles that
occur in 16 cycles of the SlowRC clock.  This is meant to smooth out the the
noise inherently present in the analog RC source.  While these 16 cycles
smooths out most noise, there is still some jitter in the bottom bits of
CLK->PERIODSR.  To further smooth out the noise, we take several readings of
CLK->PERIODSR and average them out.  Testing has shown that the bottom 3 and 4
bits of CLK->PERIODSR contain most of the jitter.  Averaging 8 samples will
smooth out 3 bits of jitter and provide a realiable and stable reading useful
in the calculations, while taking much less time than 16 or 32 samples */
#define SLOWRC_PERIOD_SAMPLES 8

/* The register CLK->LSI1KCR is a fractional divider that divides the 10kHz analog
source with the goal of generating a 1024Hz, clk1k output.
  10000Hz / CLK->LSI1KCR = 1024Hz.
Since the CLK->PERIODSR register measures the number of 12MHz cycles in 16
cycles of the RC:
     16 * 12000000
     ------------- = ~10kHz
      CLK->PERIODSR
and
  ~10kHz / 1024 = X
where X is the fractional number that belongs in CLK->LSI1KCR.  Since the
integer portion of CLK->LSI1KCR is bits 15:11 and the fractional is 10:0,
multiplying X by 2048 (bit shift left by 11) generates the proper CLK->LSI1KCR
register value.

Putting this all together:
     16 * 12000000 * 2048     384000000
     --------------------  = ------------  =  CLK->LSI1KCR
      CLK->PERIODSR * 1024       CLK->PERIODSR
*/
#define CLK1K_NUMERATOR 384000000

/* The slowest frequency for the FastRC source is 4MHz (250ns).  The PERIOD
   register updates every 256 cycles, so to be safe 257 cycles = 64us.  But,
   we need twice this maximum time because the period measurement runs
   asynchronously, and the value of CLK->HSICR is changed immediately before
   the delay */
#define FASTRC_PERIOD_SETTLE_TIME 128

/*  XTAL switching and bias trimming code */
/* Artificially increasing the MIN to 2 gives us a little breathing room
   and "feels safer". */
#define OSC24M_BIASTRIM_MIN      (0x2)
#define OSC24M_BIASTRIM_MAX      (0x0F)
#define OSC24M_BIASTRIM_UNINIT   (0xFFFF)
#define XTAL_STABILIZING_TIME_US (1500)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Flag indicating that CLK->HSECR1 has been set, but the requisite
   delay has not elapsed so the XTAL and the trim flags are not stable */
__IO boolean xtalUnstable = FALSE;

__IO WakeupXtalState wakeupXtalState = WAKEUP_XTAL_STATE_START;
uint8_t xtalAboveLoThresholdUs = DEFAULT_XTAL_SETTLING_TIME_US;
static boolean useXtalAsIs = FALSE;

/* Shadow variable for the bias trim register. */
tokTypeMfgOsc24mBiasTrim biasTrim = OSC24M_BIASTRIM_UNINIT;

/* To prevent oscillating around trim values in certain situations,
we must know some prior state. */
boolean previousLoValue = 1;

#ifdef BIASTRIM_TEST
/* The purpose of this define and test variables is to allow test code
(such as clock-test.c) to manipulate the results of the loSet and hiSet
variables, allowing the test code to verify the algorithm will
converge on the optimal value.  Effectively, this is a replacement
for the emulator only registers EMU_CLK->HSECR2_OSC24M_THRESH_H and
EMU_CLK->HSECR2_OSC24M_THRESH_L, allowing the same functionality to
be performed/tested on a real chip. */
extern boolean biasTrimTestEnabled;
extern uint32_t osc24mThreshLo;
extern uint32_t osc24mThreshHi;
#endif /* BIASTRIM_TEST */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/** @defgroup  clocks_Private_Functions
  * @{
  */

/**
  * @brief  Calculates delay 
  * @param  None
  * @retval None
  */
void idleDelayMicroseconds(uint16_t usDelay)
{
  uint32_t sysTickCount;
  
/* Don't bother with idling if delaying less than 30us since there is
   overhead associated with idling which could cause the delay to be
   too long.  While the overhead is not long, with such short delays
   it becomes a larger percentage of the delay so it's safer to just
   default to a traditional delay */
  if(usDelay<30) 
  {
    halCommonDelayMicroseconds(usDelay);
    return;
  }
  
  /* sysTickCount = (usDelay)*(1*(10**-6) / (1.0/((MHz)*(10**6)))) */
  if( (CLK->HSECR2&CLK_HSECR2_SW1) && CLK->CPUCR) 
  {
    /* 24MHz XTAL */
    sysTickCount = 24;
    
    /* a little more than 5us of overhead */
    usDelay -= 5;
  } 
  else if( (CLK->HSECR2&CLK_HSECR2_SW1) && !CLK->CPUCR) 
  {
    /* 12MHz XTAL */
    sysTickCount = 12;
    
    /* a little more than 10us of overhead */
    usDelay -= 10;
  }
  else if( !(CLK->HSECR2&CLK_HSECR2_SW1) && CLK->CPUCR) 
  {
    /* 12MHz OSCHF */
    sysTickCount = 12;
    /* a little more than 15us of overhead */
    usDelay -= 15;
  } 
  else 
  {
    /* 6MHz OSCHF */
    sysTickCount = 6;
    
    /* a little more than 20us of overhead */
    usDelay -= 20;
  }
  sysTickCount *= usDelay;
  
  ATOMIC(
    /* NOTE: SysTick's priority level causes it to bypass the ATOMIC behavior.
          The ATOMIC is used to block all normal interrupts.  Therefore,
          we don't have to use INTERRUPTS_ON when transitioning to
          idle sleep, but we still need to use PRIMASK to disable all
          interrupts, including SysTick, to transition to idle */
    __disable_irq();
    
    /* Enable SysTick from FCLK and enable interrupt */
     SysTick->CTRL= SysTick_CTRL_CLKSOURCE | SysTick_CTRL_ENABLE | SysTick_CTRL_TICKINT;
    
    /* Setting reload and count to zero will disable SysTick and clear COUNTFLAG */
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    
    /* Clear any stale SysTick interrupt */
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
    
    /* Setting reload value effectively starts timer counting */
    SysTick->LOAD = (sysTickCount-1);
    
#ifdef FLASH_ACCESS_FLASH_LP
    BYPASS_MPU(
               uint32_t FLASH_ACCESS_SAVED = FLASH_ACCESS;
               halInternalIdleSleep();
               FLASH->ACR= FLASH_ACCESS_SAVED;
               )
#else /* FLASH_ACCESS_FLASH_LP */
    halInternalIdleSleep();
    
#endif /* FLASH_ACCESS_FLASH_LP */
    __enable_irq();
    )
}
#endif /* DISABLE_IDLING_DELAY */

/**
  * @brief hal SysTick Interrupt status   
  * @param  None
  * @retval None
  */
void halSysTickIsr(void)
{
  /* Setting reload and count to zero will disable SysTick */
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
  
  /* Disable interrupt since we don't need it till the next tick event */
   SysTick->CTRL&= ~SysTick_CTRL_TICKINT;
  
  /* When the ISR fires, that means our delay is over and the XTAL is stable */
  xtalUnstable = FALSE;

  #ifndef DISABLE_IDLING_DELAY
    if (wakeupXtalState == WAKEUP_XTAL_STATE_BEFORE_LO_EN) 
    {
      /* Clear any past LO interrupts before checking current state */
      MGMT_IT->ISR = MGMT_ISR_HSECOMPLLIF;

      if ((CLK->HSECOMPR & CLK_HSECOMPR_LLEVEL) == CLK_HSECOMPR_LLEVEL) 
      {
        /* Special case: if the LO threshold has already been crossed we don't
         want to enable the LO crossing interrupt (we already missed it!),
         we just want to proceed as if we already saw the interrupt.
         This can happen if we go to sleep and then wakeup in less time than
         it takes for the xtal to ramp down */
        wakeupXtalState = WAKEUP_XTAL_STATE_READY_SWITCH;
      } 
      else 
      {
         /* We can now enable the LO interrupt, the xtal has had enough time to
            settle after being hit with max current.         
            We're in the SysTick interrupt which has the same priority as the
            MGMT interrupt, so we're guaranteed that the MGMT interrupt won't
            fire while we're running this SysTick code. */
        wakeupXtalState = WAKEUP_XTAL_STATE_LO_EN;
        halInternalInitMgmtIsrLo();
      }
    } 
    else if (wakeupXtalState == WAKEUP_XTAL_STATE_WAITING_FINAL) 
    {
      /* If we're waking up (as opposed to coming out of initial powerup) then we
         need to wait for the final xtal check to call the xtal "fully ready." */
      wakeupXtalState = WAKEUP_XTAL_STATE_FINAL;
    }
  #endif
}

/**
  * @brief Enables SysTick from FCLK and enable interrupt
  *        alSysTickIsr() is the handler for this ISR.
  * @param  None
  * @retval None
  */
void scheduleHalSysTickIsrUs(uint16_t usDelay)
{
  uint32_t sysTickCount;
  /* uint16_t usDelay = 100; */

  /* FIXME-svec-2011.01.31: we can possibly cut down this if/else tree if we
     decide what clks we'll support in this function */
  
  /* sysTickCount = (usDelay)*(1*(10**-6) / (1.0/((MHz)*(10**6)))) */
  if( (CLK->HSECR2&CLK_HSECR2_SW1) && CLK->CPUCR) 
  {
    /* 24MHz XTAL */
    sysTickCount = 24;
    
    /* a little more than 5us of overhead */
    usDelay -= 5;
  } 
  else if( (CLK->HSECR2&CLK_HSECR2_SW1) && !CLK->CPUCR) 
  {
    /* 12MHz XTAL */
    sysTickCount = 12;
    
    /* a little more than 10us of overhead */
    usDelay -= 10;
  } 
  else if( !(CLK->HSECR2&CLK_HSECR2_SW1) && CLK->CPUCR) 
  {
    /* 12MHz OSCHF */
    sysTickCount = 12;
    
    /* a little more than 15us of overhead */
    usDelay -= 15;
  } 
  else 
  {
    /* 6MHz OSCHF*/
    sysTickCount = 6;
    
    /* a little more than 20us of overhead */
    usDelay -= 20;
  }
  sysTickCount *= usDelay;
  
  /* Enable SysTick from FCLK and enable interrupt */
   SysTick->CTRL= SysTick_CTRL_CLKSOURCE | SysTick_CTRL_ENABLE | SysTick_CTRL_TICKINT;
  
  /* Setting reload and count to zero will disable SysTick and clear COUNTFLAG */
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
  
  /* Clear any stale SysTick interrupt */
  SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
  
  /* Setting reload value effectively starts timer counting */
  SysTick->LOAD = (sysTickCount-1);
}

/**
  * @brief Calibrates Slow RC
  * @param  None
  * @retval None
  */
void halInternalCalibrateSlowRc(void)
{
  uint8_t i;
  uint32_t average=0;
  int16_t delta;
  uint32_t period;
  
  CALDBG(
    stSerialPrintf(ST_ASSERT_SERIAL_PORT, "halInternalCalibrateSlowRc:\r\n");
  )
  
    /* STEP 1: coarsely tune SlowRC in analog section to ~10kHz */
    /* To operate properly across the full temperature and voltage range,
    the RC source in the analog section needs to be first coarsely tuned
    to 10kHz.  The CLK->LSI10KCR register, which is 2's compliment, provides 16
    steps at ~400Hz per step yielding approximate frequences of 8kHz at 7
    and 15kHz at -8.
    Start with our reset values for TUNE and CAL */
    CLK->PERIODCR = 0; //measure SlowRC
    CLK->LSI10KCR = 0x00;
    CLK->LSI1KCR = 0x5000;
    /* wait for the PERIOD register to properly update */
    idleDelayMicroseconds(SLOWRC_PERIOD_SETTLE_TIME);
    
    /* Measure the current CLK->PERIODSR to obtain a baseline */
    CALDBG(
      stSerialPrintf(ST_ASSERT_SERIAL_PORT,
      "period: %u, ", CLK->PERIODSR);
      stSerialPrintf(ST_ASSERT_SERIAL_PORT, "%u Hz\r\n", 
                       ((uint16_t)(((uint32_t)192000000)/((uint32_t)CLK->PERIODSR))));
    )
    /* For 10kHz, the ideal CLK->PERIODSR is 19200.  Calculate the PERIOD delta.
    It's possible for a chip's 10kHz source RC to be too far out of range
    for the CLK->LSI10KCR to bring it back to 10kHz.  Therefore, we have to
    ensure that our delta correction does not exceed the tune range so
    tune has to be capped to the end of the vailable range so it does not
    wrap.  Even if we cannot achieve 10kHz, the 1kHz calibration can still
    properly correct to 1kHz.
    Each CLK->LSI10KCR step yields a CLK->PERIODSR delta of *approximately* 800.
    Calculate how many steps we are off.  While dividing by 800 may seem
    like an ugly calculation, the precision of the result is worth the small
    bit of code and time needed to do a divide */
    period = CLK->PERIODSR;
    
    /* Round to the nearest integer*/
    delta = (19200+400) - period;
    delta /= 800;
    
    /* CLK->LSI10KCR is a 4 bit signed number.  cap the delta to 7/-8*/
    if(delta > 7) 
    {
      delta = 7;
    }
    if(delta < -8) 
    {
      delta = -8;
    }
    CALDBG(
      stSerialPrintf(ST_ASSERT_SERIAL_PORT, "TUNE steps delta: %d\r\n",
                        delta);
    )
    CLK->LSI10KCR = delta;
    /* wait for PERIOD to update before taking another sample */
    idleDelayMicroseconds(SLOWRC_PERIOD_SETTLE_TIME);
    CALDBG(
      stSerialPrintf(ST_ASSERT_SERIAL_PORT,
      "period: %u, ", CLK->PERIODSR);
      stSerialPrintf(ST_ASSERT_SERIAL_PORT, "%u Hz\r\n", 
                       ((uint16_t)(((uint32_t)192000000)/((uint32_t)CLK->PERIODSR))));
    )
    /* The analog section should now be producing an output of ~10kHz */
    
    /* ---- STEP 2: fine tune the SlowRC to 1024Hz ---- */
    /* Our goal is to generate a 1024Hz source.  The register CLK->LSI1KCR is a
    fractional divider that divides the 10kHz analog source and generates
    the clk1k output.  At reset, the default value is 0x5000 which yields a
    division of 10.000.  By averaging several samples of CLK->PERIODSR, we
    can then calculate the proper divisor need for CLK->LSI1KCR to make 1024Hz */
    for(i=0;i<SLOWRC_PERIOD_SAMPLES;i++) 
    {
      idleDelayMicroseconds(SLOWRC_PERIOD_SETTLE_TIME);
      average += CLK->PERIODSR;
    }
    /* calculate the average, with proper rounding */
    average = (average+(SLOWRC_PERIOD_SAMPLES/2))/SLOWRC_PERIOD_SAMPLES;
    CALDBG(
      stSerialPrintf(ST_ASSERT_SERIAL_PORT, "average: %u, %u Hz\r\n",
        ((uint16_t)average), ((uint16_t)(((uint32_t)192000000)/((uint32_t)average))));
    )    
    /* using an average period sample, calculate the clk1k divisor */
    CLK->LSI1KCR = (uint16_t)(CLK1K_NUMERATOR/average);
    CALDBG(
      stSerialPrintf(ST_ASSERT_SERIAL_PORT,"CLK->LSI1KCR=%2X\r\n",CLK->LSI1KCR);
    )
    /* The SlowRC timer is now producing a 1024Hz tick (+/-2Hz) */
    
  CALDBG(
    stSerialPrintf(ST_ASSERT_SERIAL_PORT, "DONE\r\n");
  )
}

/**
  * @brief Calibrates Fast RC
  *        The CLK->PERIODSR register measures the number of 12MHz cycles in 256
  *        cycles of OSCHF:
  *                       256 * 12000000
  *                      ------------- = ~12MHz
  *                       CLK->PERIODSR
  * @param  None
  * @retval None
  */   
void halInternalCalibrateFastRc(void)
{
  int32_t newTune = -16;
  
  CALDBG(
    stSerialPrintf(ST_ASSERT_SERIAL_PORT, "halInternalCalibrateFastRc:\r\n");
  )
  
    /* ---- coarsely tune FastRC in analog section to ~12MHz ---- */
    /* The RC source in the analog section needs to be coarsely tuned
       to 12MHz.  The CLK->HSICR register, which is 2's compliment, provides 32
       steps at ~0.5MHz per step yielding approximate frequences of 4MHz at 15
       and 20MHz at -16 */
    /* measure FastRC */
    CLK->PERIODCR = 1; 
  
    CALDBG(
      /* start at the fastest possible frequency */
      CLK->HSICR = newTune;
      
      /* wait for the PERIOD register to properly update */
      idleDelayMicroseconds(FASTRC_PERIOD_SETTLE_TIME);
      
      /* Measure the current CLK->PERIODSR to obtain a baseline */
      stSerialPrintf(ST_ASSERT_SERIAL_PORT,
      "period: %u, ", CLK->PERIODSR);
      stSerialPrintf(ST_ASSERT_SERIAL_PORT, "%u kHz\r\n", 
                       ((uint16_t)((((uint32_t)3072000000)/((uint32_t)CLK->PERIODSR))/1000)));
    )
    /* For 12MHz, the ideal CLK->PERIODSR is 256.  Tune the frequency down until
    the period is <= 256, which says the frequency is as close to 12MHz as
    possible (without going over 12MHz)
    Start at the fastest possible frequency (-16) and increase to the slowest
    possible (15).  When CLK->PERIODSR is <=256 or we run out of tune values,
    we're done */
    for(;newTune<16;newTune++) {
      /* decrease frequency by one step (by increasing tune value) */
      CLK->HSICR = newTune;
      
      /* wait for the PERIOD register to properly update */
      idleDelayMicroseconds(FASTRC_PERIOD_SETTLE_TIME);
      
      /* kickout if we're tuned */
      if(CLK->PERIODSR>=256) {
        break;
      }
    }
    CALDBG(
      /* Measure the current CLK->PERIODSR to show the final result */
      stSerialPrintf(ST_ASSERT_SERIAL_PORT,
      "period: %u, ", CLK->PERIODSR);
      stSerialPrintf(ST_ASSERT_SERIAL_PORT, "%u kHz\r\n", 
                       ((uint16_t)((((uint32_t)3072000000)/((uint32_t)CLK->PERIODSR))/1000)));
    )
    
    /* The analog section should now be producing an output of 11.5MHz - 12.0MHz */
}

/**
  * @brief  This function configures the flash access controller for optimal
  *         current consumption when FCLK is operating from XTAL.  By providing
  *         this function, the calling code does not have to be aware of the
  *         details of setting FLASH_ACCESS.
  *         NOTE: HALFCYCLE_ACCESS should not be used on STM32W108 
  * @param  None
  * @retval None
  */
static void halInternalConfigXtalFlashAccess(void)
{
  ATOMIC(
    BYPASS_MPU(
      #if defined(CORTEXM3_STM32W108)
      #if defined(CORTEXM3_STM32W108xB)
        /* For STM32W108xB, due to an hardware limitation we are forced to use 1 wait state both on 24MHz and 12MHz */
        FLASH->ACR= (FLASH_ACR_PRFTBE |
                        (1<<0));
      #endif
      #if defined(CORTEXM3_STM32W108CC)
        FLASH->ACR= (FLASH_ACR_PRFTBE |
                        ((CLK->CPUCR&CLK_CPUCR_SW2)<<0));
      #endif
      #else
        FLASH->ACR= (FLASH_ACCESS_FLASH_LP             |
                        FLASH_ACR_PRFTBE          |
                        (1<<0));
      #endif
    )
  )
}

/**
  * @brief  Enable SysTick from FCLK and enable interrupt
  * @param  None
  * @retval None
  */
static void scheduleNextBiasTrimEvent(void)
{
  /* Enable SysTick from FCLK and enable interrupt */
   SysTick->CTRL= SysTick_CTRL_CLKSOURCE | SysTick_CTRL_ENABLE | SysTick_CTRL_TICKINT;
  
  /* Setting reload and count to zero will disable SysTick and clear COUNTFLAG */
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
  
  /* Clear any stale SysTick interrupt */
  SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
  
  /* Setting reload value effectively starts timer counting. This code can
     be called from any clock configuration. */
  if( (CLK->HSECR2&CLK_HSECR2_SW1) && CLK->CPUCR) 
  {
    /* 24MHz XTAL */
    SysTick->LOAD = (24*XTAL_STABILIZING_TIME_US)-1;
  } else if( (CLK->HSECR2&CLK_HSECR2_SW1) && !CLK->CPUCR) {
    /* 12MHz XTAL */
    SysTick->LOAD = (12*XTAL_STABILIZING_TIME_US)-1;
  } else if( !(CLK->HSECR2&CLK_HSECR2_SW1) && CLK->CPUCR) {
    /* 12MHz OSCHF */
    SysTick->LOAD = (12*XTAL_STABILIZING_TIME_US)-1;
  } else {
    /* 6MHz OSCHF */
    SysTick->LOAD = (6*XTAL_STABILIZING_TIME_US)-1;
  }
}

/**
  * @brief  Setup the managment ISR
  * @param  None
  * @retval None
  */
void halInternalInitMgmtIsrLo(void)
{
  /* Setup the managment ISR so we are notified when the xtal crosses the 
    'LO' threshold */
  MGMT_IT->IER |= MGMT_ISR_HSECOMPLLIF;
  NVIC->ICPR[0] = NVIC_IxxR_MGMT;
  NVIC->ISER[0] = NVIC_IxxR_MGMT;
}

/**
  * @brief  Called only when waking from sleep
  *         We want to "kick" the xtal to get it to start ASAP, so we apply the max
  *         current to it 
  * @param  None
  * @retval None
  */
void halInternalPowerUpKickXtal(void)
{
  useXtalAsIs = FALSE;

  /* If the xtal is already enabled and we're running from it, then we'll try
     to keep using it as-is. As long as the bias is what we expect, we'll just
     keep on truckin'! */
  if ((CLK->HSECR2 & (CLK_HSECR2_EN | CLK_HSECR2_SW1))
                 == (CLK_HSECR2_EN | CLK_HSECR2_SW1)) 
  {
    /* FIXME-svec-2011.02.09: do we need to set previousLoValue=TRUE to make it
       faster? */
    xtalUnstable = FALSE;
    halCommonSwitchToXtal();

    useXtalAsIs = TRUE;
    return;
  }

  /* Send max current to the xtal so it ramps ASAP. */
  CLK->HSECR1 = OSC24M_BIASTRIM_MAX;
  CLK->HSECR2 = CLK_HSECR2_EN;

  /* We're going to give the xtal 100us to ramp/settle until we enable the LO
     interrupt. */
  wakeupXtalState = WAKEUP_XTAL_STATE_BEFORE_LO_EN;
  scheduleHalSysTickIsrUs(100);
}

/**
  * @brief  Switches to Xtal24M
  * @param  None
  * @retval None
  */
void halInternalSwitchToXtal24M(void)
{
/* We've found a valid trim value and we've waited for the oscillator
   to stabalize, it's now safe to select the XTAL */
  CLK->HSECR2 |= CLK_HSECR2_SW1;

/* Standard mode of operation is 24MHz (FCLK is sourced from SYSCLK) */
#if CORTEXM3_STM32W108_CPU_CLOCK_12MHz == 1
  CLK->CPUCR = 0;
#else
  CLK->CPUCR = CLK_CPUCR_SW2;
#endif

/* Configures flash access for optimal current consumption */
  halInternalConfigXtalFlashAccess();
}

/**
  * @brief  This is the core function for enabling the XTAL, 
  *          biasing the XTAL, checking the XTAL biasing, 
  *          switching to the XTAL, configuring FCLK, and configuring flash access settings
  * @param  None
  * @retval None
  */
boolean halCommonTryToSwitchToXtal(void)
{
  boolean loSet = 0;
  boolean hiSet = 0;
  
  if(biasTrim == OSC24M_BIASTRIM_UNINIT) 
  {
   /* halCommonGetMfgToken(&biasTrim, TOKEN_MFG_OSC24M_BIAS_TRIM);
      If the biasTrim loaded from the token is not a valid trim,
      select a common starting value.  Then the normal algorithm
      will occur to optimize the biasTrim. */
    if(biasTrim == OSC24M_BIASTRIM_UNINIT) 
    {
      /* Since many chips endup at minimum, let's just start at minimum
         and let the linear search do the rest */
      biasTrim = OSC24M_BIASTRIM_MIN;
  }    
    /* If the XTAL is not already enabled, then enable it */
    if((CLK->HSECR2&CLK_HSECR2_EN) != CLK_HSECR2_EN)
    {
      CLK->HSECR2 |= CLK_HSECR2_EN;
    }
    goto setNewTrim;
  }
  
  /* If the XTAL is not already enabled, then enable it.  But, we need
     to set the trim value and wait for the HI/LO flags to become stable
     before taking action on the flags. */
  if((CLK->HSECR2&CLK_HSECR2_EN) != CLK_HSECR2_EN) {
    CLK->HSECR2 |= CLK_HSECR2_EN;
    goto setNewTrim;
  }
  
  #ifdef DISABLE_IDLING_DELAY
  if( SysTick->CTRL& ST_CSR_COUNTFLAG) {
    halSysTickIsr();
  }
  #endif /* DISABLE_IDLING_DELAY */
  if(xtalUnstable) {
   /* This means our HI/LO flags cannot be read so there is nothing to
      do until the XTAL/flags are stable.  Indicate selection has not
      changed and the XTAL is unstable. */
    return FALSE;
  }
  
  /* OBTAIN THE HI/LO flags */
  loSet = ((CLK->HSECOMPR & CLK_HSECOMPR_LLEVEL) == CLK_HSECOMPR_LLEVEL);
  hiSet = ((CLK->HSECOMPR & CLK_HSECOMPR_HLEVEL) == CLK_HSECOMPR_HLEVEL);
  #ifdef BIASTRIM_TEST
  if(biasTrimTestEnabled) {
    loSet = (OSC24M_BIASTRIM>osc24mThreshLo);
    hiSet = (OSC24M_BIASTRIM>osc24mThreshHi);
  }
  #endif /* BIASTRIM_TEST */
  
  /* DETERMINE TRIM CHANGE */
  /* The goal is the lowest value for which HI=0 and LO=1.  For all possible
     scenarios, the LO flag gets priority.  That means the final TRIM should
     always be bounded by MAX and MIN and the LO flag should always be set. */
  if(!loSet) {
    /*The bias is too low, so we need to increment the bias trim.
      but don't trim above our max value */
    if(biasTrim<OSC24M_BIASTRIM_MAX) {
      biasTrim++;
    }
  } else if(hiSet && previousLoValue) {
    /* The bias is too high, so we need to decrement the bias trim.
       but don't trim below our min value */
    if(biasTrim>OSC24M_BIASTRIM_MIN) {
      biasTrim--;
    }
  }
  /* In a situation where LO and HI track identically to the same bias trim
     threshold value, the increment and decrement code would cause
     oscillation on the trim value.  Knowing that the previous LO was
     cleared and the current HI is set means that they match and we're
     already oscillating around the minimal value.  The net effect of knowing
     the previous value is to not oscillate and stay at the minimal value.
     The only penalty is two unnecessary trim settings; but that's needed
     to find the transition point where the flags change. */
  previousLoValue = loSet;
  
  /* Don't allow bias trim to dip below the min regardless of LO/HI. */
  if(biasTrim<OSC24M_BIASTRIM_MIN) {
    biasTrim = OSC24M_BIASTRIM_MIN;
  }
  
  /* TRY TO APPLY CHANGES */
  if(CLK->HSECR1 == biasTrim) {

    halInternalSwitchToXtal24M();
    
    /* Indicate the switch has ocurred and the XTAL is stable. */
    return TRUE;
  }
  
setNewTrim:
  xtalUnstable = TRUE;
  CLK->HSECR1 = biasTrim;
  scheduleNextBiasTrimEvent();
  /* Indicate selection has not changed and the XTAL is unstable. */
  return FALSE;
}

/**
  * @brief  performs/completes the switch process to the XTAL.  This function is the blocking form of
  *         halCommonTryToSwitchToXtal.  Instead of burning cycles, it idles the CPU and uses the xtalUnstable 
  *         event ISR to exit idling.
  * @param  None
  * @retval None
  */
void halCommonSwitchToXtal(void)
{
  boolean switched = FALSE;
  
  while(!switched) {
    #ifdef DISABLE_IDLING_DELAY
    switched = halCommonTryToSwitchToXtal();
    if(!switched) {
      halCommonDelayMicroseconds(XTAL_STABILIZING_TIME_US);
    }
    #else //DISABLE_IDLING_DELAY
    ATOMIC(
     /* NOTE: SysTick's priority level causes it to bypass the ATOMIC behavior.
            The ATOMIC is used to block all normal interrupts.  Therefore,
            we don't have to use INTERRUPTS_ON when transitioning to
            idle sleep, but we still need to use PRIMASK to disable all
            interrupts, including SysTick, to transition to idle. */
      __disable_irq();
      
     /* halCommonTryToSwitchToXtal could/will schedule an interrupt
      to take us out of idle sleep.  By scheduling that interrupt in
      an ATOMIC block, we ensure that the interrupt is still valid
      for when we invoke idle sleep. */
      switched = halCommonTryToSwitchToXtal();
      
      /* TRUE: Using the XTAL. xtalUnstable==FALSE. No ISR is pending.
         That means we're done, so return. */
      /* FALSE: XTAL *selection unchanged*. xtalUnstable==TRUE.
         *ISR is pending.* Even though we may be on the XTAL already, 
          the key here is that there is an ISR pending which will allow 
          us to exit idle sleep, however long (or short) in the future that may be. */
      if(!switched) {
#ifdef FLASH_ACCESS_FLASH_LP
        BYPASS_MPU(
                   uint32_t FLASH_ACCESS_SAVED = FLASH_ACCESS;
                   FLASH->ACR&= ~FLASH_ACCESS_FLASH_LP;
                   halInternalIdleSleep();
                   FLASH->ACR= FLASH_ACCESS_SAVED;
                   )
#else /* FLASH_ACCESS_FLASH_LP */
      halInternalIdleSleep();
#endif /* FLASH_ACCESS_FLASH_LP */
      }
      __enable_irq();
      )
    #endif /* DISABLE_IDLING_DELAY */
  }
}

#ifndef DISABLE_IDLING_DELAY

/**
  * @brief Blocks until Xtal above LO  
  * @param  None
  * @retval None
  */
static void halInternalBlockUntilXtalAboveLo(void)
{
  /* We must be above the LO threshold for at least xtalAboveLoThresholdUs in
     order to proceed. */
  boolean done = 0;

  /* We're not using the XTAL, the MAC Timer is running off OSCHF,
  that means the clock is half speed, 6MHz.  We need to halve our delay
  time. */
  uint16_t shiftedDelay = xtalAboveLoThresholdUs >> 1;

  /* MAC Timer is enabled in emRadioInit, which may not have been called yet. */
  /* This algorithm needs the MAC Timer so we enable it here. */
  MACTMR->CR |= MACTMR_CR_EN;

  while (! done) {

    /* Most of this code was stolen from halCommonDelayMicroseconds(). */
    uint32_t beginTime = MACTMR->CNTR;
    done = 1;

    do {
     /* We must check that we're above the LO threshold even if we've already
       gone past the timer delay limit. */
      if ((CLK->HSECOMPR & CLK_HSECOMPR_LLEVEL) != CLK_HSECOMPR_LLEVEL) 
      {
        done = 0;
        break;
      }

     /* since our max delay (255) is less than half the size of the 
        20 bit mac timer, we can easily just handle the potential for
        mac timer wrapping by subtracting the time delta and masking out
        the extra bits */
    } while( ((MACTMR->CNTR - beginTime)&MACTMR_CNTR_CNT) < shiftedDelay );
  }
}

/**
  * @brief we're ready to switch to the XTAL as the clock, then switch
  * @param  None
  * @retval None
  */
void halInternalBlockUntilXtal(void)
{
  if (useXtalAsIs) 
  {
    return;
  }
  while (wakeupXtalState != WAKEUP_XTAL_STATE_READY_SWITCH) 
  {
    ATOMIC(
     /* ATOMIC sets basepri==12, so any interrupts <priority12 are still
        running, which includes systick and mgmtisr. */
            
     /* NOTE: SysTick's priority level causes it to bypass the ATOMIC behavior.
            The ATOMIC is used to block all normal interrupts.  Therefore,
            we don't have to use INTERRUPTS_ON when transitioning to
            idle sleep, but we still need to use PRIMASK to disable all
            interrupts, including SysTick, to transition to idle. */
      
     /*   After __disable_irq *NO* interrupt can be handled (except NMI) until
       __enable_irq.  If an interrupt occurs the CPU will wakeup and set the
       appropriate 'iinterrupted' bit, but the handler won't run until after
       the __enable_irq() below. */
      __disable_irq();

      if (wakeupXtalState != WAKEUP_XTAL_STATE_READY_SWITCH) 
      {
#ifdef FLASH_ACCESS_FLASH_LP
        BYPASS_MPU(
                   uint32_t FLASH_ACCESS_SAVED = FLASH_ACCESS;
                   FLASH->ACR&= ~FLASH_ACCESS_FLASH_LP;
                   halInternalIdleSleep();
                   FLASH->ACR= FLASH_ACCESS_SAVED;
                   )
#else /* FLASH_ACCESS_FLASH_LP */
      halInternalIdleSleep();
#endif /* FLASH_ACCESS_FLASH_LP */
      }
      __enable_irq();
    )
  }

  /* Restore the bias from previous known good value. */
  CLK->HSECR1 = biasTrim;

  if (xtalAboveLoThresholdUs) 
  {
    halInternalBlockUntilXtalAboveLo();
  }
  halInternalSwitchToXtal24M();
  wakeupXtalState = WAKEUP_XTAL_STATE_WAITING_FINAL;
  scheduleHalSysTickIsrUs(400);
}
#endif /* DISABLE_IDLING_DELAY */

/**
  * @brief This is the final check the RX/TX code must use to make sure we're running
  *        on a stable xtal before any xtal-sensitive radio code can run.
  *        FIXME-svec-2011.02.02: similar to halInternalBlockUntilXtal(), perhaps can
  *        combine into 1 function with different callers to specify wakeupXtalState and
  *        function to call after while() loop? 
  * @param  None
  * @retval None
  */
void halCommonBlockUntilXtal(void)
{
  if ((! (RST->SR & RST_SR_WKUP)) || (useXtalAsIs)) 
  {
    return;
  }

  while (wakeupXtalState != WAKEUP_XTAL_STATE_FINAL) 
  {
    ATOMIC(
    /* ATOMIC sets basepri==12, so any interrupts <priority12 are still
       running, which includes systick and mgmtisr. */      
      
    /* NOTE: SysTick's priority level causes it to bypass the ATOMIC behavior.
            The ATOMIC is used to block all normal interrupts.  Therefore,
            we don't have to use INTERRUPTS_ON when transitioning to
            idle sleep, but we still need to use PRIMASK to disable all
            interrupts, including SysTick, to transition to idle. */
      
     /*  After __disable_irq() *NO* interrupt can be handled (except NMI) until
       __enable_irq.  If an interrupt occurs the CPU will wakeup and set the
       appropriate 'iinterrupted' bit, but the handler won't run until after
       the __enable_irq() below. */
      __disable_irq();
  
      if (wakeupXtalState != WAKEUP_XTAL_STATE_FINAL) 
      {
#ifdef FLASH_ACCESS_FLASH_LP
        BYPASS_MPU(
                   uint32_t FLASH_ACCESS_SAVED = FLASH_ACCESS;
                   FLASH->ACR&= ~FLASH_ACCESS_FLASH_LP;
                   halInternalIdleSleep();
                   FLASH->ACR= FLASH_ACCESS_SAVED;
                   )
#else /* FLASH_ACCESS_FLASH_LP */
      halInternalIdleSleep();
#endif /* FLASH_ACCESS_FLASH_LP */
      }
      __enable_irq();
      )
  }
  /* When we get here we know we've the 400us timer has expired, so
   the xtal should be settled.  Make sure the LO/HI conditions are met by
   using the same xtal setup code as the cold-powerup sequence uses. */
  xtalUnstable = FALSE;
  halCommonSwitchToXtal();
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
