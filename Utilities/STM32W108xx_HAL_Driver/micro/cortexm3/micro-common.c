/**
  ******************************************************************************
  * @file    micro-common.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   STM32W108 micro-common specific minimal HAL functions common to
  *          full and minimal hal
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
#include "board.h"
#include "error.h"
#include "micro-common.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup micro common
  * @brief micro common modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  micro-common_Private_Functions
  * @{
  */

/**
  * @brief  Configure an IO pin's operating mode
  * @param  io  The io pin to use, can be specified with the convenience macros
  *         PORTA_PIN(), PORTB_PIN(), PORTC_PIN()
  * @param  config   The configuration mode to use.
  * @retval None
  */
void halGpioConfig(uint32_t io, uint32_t config)
{
  static __IO uint32_t *const configRegs[] = 
    { (__IO uint32_t *)GPIOA_BASE,
      (__IO uint32_t *)(GPIOA_BASE + 4),
      (__IO uint32_t *)GPIOB_BASE,
      (__IO uint32_t *)(GPIOB_BASE + 4),
      (__IO uint32_t *)GPIOC_BASE,
      (__IO uint32_t *)(GPIOC_BASE + 4) };
  uint32_t portcfg;
  portcfg = *configRegs[io/4];                /* get current config */                   
  portcfg = portcfg & ~((0xF)<<((io&3)*4));   /* mask out config of this pin */
  *configRegs[io/4] = portcfg | (config <<((io&3)*4));
}

/**
  * @brief  Set/Clear single GPIO bit
  * @param  io  The io pin to use, can be specified with the convenience macros
  *         PORTA_PIN(), PORTB_PIN(), PORTC_PIN()
  * @param  value   A flag indicating whether to set or clear the io.
  * @retval None
  */
void halGpioSet(uint32_t gpio, boolean value)
{
  if(gpio/8 < 3) {
    if (value) {
      *((__IO uint32_t *)(GPIO_PxSET_BASE+(GPIO_Px_OFFSET*(gpio/8)))) = BIT(gpio&7);
    } else {
      *((__IO uint32_t *)(GPIO_PxCLR_BASE+(GPIO_Px_OFFSET*(gpio/8)))) = BIT(gpio&7);
    }
  }
}

/**
  * @brief  Starts system timer 
  * @param  None
  * @retval Return unsigned value
  */
uint16_t halInternalStartSystemTimer(void)
{
  /* Since the SleepTMR is the only timer maintained during deep sleep, it is
   * used as the System Timer (RTC).  We maintain a 32 bit hardware timer
   * configured for a tick value time of 1024 ticks/second (0.9765625 ms/tick)
   * using either the 10 kHz internal SlowRC clock divided and calibrated to
   * 1024 Hz or the external 32.768 kHz crystal divided to 1024 Hz.
   * With a tick time of ~1ms, this 32bit timer will wrap after ~48.5 days. */
  
  /* disable top-level interrupt while configuring */
  NVIC->ICER[0] = NVIC_IxxR_SLEEPTMR;
  
#ifdef ENABLE_OSC32K
    #ifdef DIGITAL_OSC32_EXT
      /* Disable both OSC32K and SLOWRC if using external digital clock input */
      CLK->SLEEPCR = 0;
    #else /*!DIGITAL_OSC32_EXT */
      /* Enable the 32kHz XTAL (and disable SlowRC since it is not needed) */
      CLK->SLEEPCR = CLK_SLEEPCR_LSEEN;
    #endif
    /* Sleep timer configuration is the same for crystal and external clock */
    SLPTMR->CR = (SLPTMR_CR_EN            | /* enable TMR */
                   (0 << 10)| /* TMR not paused when halted */
                   (5 << 4)  | /* divide down to 1024Hz */
                   (1 << 0)) ; /* select XTAL */
  #else /* !ENABLE_OSC32K */
    /* Enable the SlowRC (and disable 32kHz XTAL since it is not needed) */
    CLK->SLEEPCR = CLK_SLEEPCR_LSI10KEN;
    SLPTMR->CR = (SLPTMR_CR_EN            | /* enable TMR */
                   (0 << 10)| /* TMR not paused when halted */
                   (0 << 4)  | /* already 1024Hz */
                   (0 << 0)) ; /* select SlowRC */
    #ifndef DISABLE_RC_CALIBRATION
      halInternalCalibrateSlowRc(); /* calibrate SlowRC to 1024Hz */
    #endif /* DISABLE_RC_CALIBRATION */
#endif /*ENABLE_OSC32K */
  
  /* clear out any stale interrupts */
  SLPTMR->ISR = (SLPTMR_IER_WRAP | SLPTMR_IER_CMPA | SLPTMR_IER_CMPB);
  /* turn off second level interrupts.  they will be enabled elsewhere as needed */
  SLPTMR->IER = 0x00;
  /* enable top-level interrupt */
  NVIC->ISER[0] = NVIC_IxxR_SLEEPTMR;
  
  return 0;
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
