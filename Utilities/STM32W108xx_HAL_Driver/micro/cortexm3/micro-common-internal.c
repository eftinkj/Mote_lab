/**
  ******************************************************************************
  * @file    micro-common-internal.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   STM32W108 internal, micro specific HAL functions.
  * @Note    This file is provided for completeness and it should not be modified
  *          by customers as it comtains code very tightly linked to undocumented
  *          device features
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
#include "mfg-token.h"
#include "hal_adc.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup micro-common-internal
  * @brief micro-common-internal modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
   /* Specify whether the regulator trimming is set for normal mode or radio boost mode */
__IO uint8_t regTrimBoostMode = FALSE;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  micro_Private_Functions
  * @{
  */

/**
  * @brief  Calibrates the GPIO pads.
  * @param  None
  * @retval None
  */
void halCommonCalibratePads(void)
{
  /* We don't need extreme accuracy for this measurement, so we can use
   * the fastest ADC sample rate */
  if(halMeasureVdd(ADC_SAMPLE_CLOCKS_32) < 2700) {
    GPIO_DBG->DBGCR |= GPIO_DBGCR_PADDRIVE;
  } else {
    GPIO_DBG->DBGCR &= ~GPIO_DBGCR_PADDRIVE;
  }
}

/**
  * @brief  Sets the trim values for the 1.8V and 1.2V regulators based upon 
  *         manufacturing configuration
  * @param  Boolean value of the boostmode
  * @retval None
  */
void halInternalSetRegTrim(boolean boostMode)
{
  tokTypeMfgRegTrim regTrim;
  uint8_t trim1V2;
  uint8_t trim1V8;
  
  halCommonGetMfgToken(&regTrim, TOKEN_MFG_REG_TRIM);
  /* The compiler can optimize this function a bit more and keep the 
   * values in processor registers if we use separate local vars instead
   * of just accessing via the structure fields */
  trim1V8 = regTrim.regTrim1V8;
  trim1V2 = regTrim.regTrim1V2;
  
  /* If tokens are erased, default to reasonable values, otherwise use the
   * token values. */
  if((trim1V2 == 0xFF) && (trim1V8 == 0xFF))
  {
    trim1V8 = 4;
    trim1V2 = 0;
  }
  
  /* When the radio is in boost mode, we have to increase the 1.8V trim. */
  if(boostMode)
  {
    trim1V8 += 2;
    regTrimBoostMode = TRUE;
  }
  else
  {
    regTrimBoostMode = FALSE;
  }
  
  /* Clamp at 7 to ensure we don't exceed max values, accidentally set
     other bits, or wrap values. */
  if(trim1V8 > 7)
  {
    trim1V8 = 7;
  }
  if(trim1V2 > 7)
  {
    trim1V2 = 7;
  }
  
  /* only set the trim fields so that any test bits are not overwritten
     (they are used by some test scripts to force an external voltage in) */
  SET_REG_FIELD(PWR->VREGCR, PWR_VREGCR_1V8TRIM, trim1V8, 7);
  SET_REG_FIELD(PWR->VREGCR, PWR_VREGCR_1V2TRIM, trim1V2, 0);   
}

/**
  * @brief  Determine VREG_OUT in the current mode (normal or boost).
  * @param  None
  * @retval VREG_OUT in millivolts, depending on normal or boost mode
  */
uint16_t halInternalGetVreg(void)
{
  tokTypeMfgRegVoltage1V8 vregOutTok;

  /* Convert the value into mV. VREG_OUT is ideally 1.8V, but it won't be
     exactly 1.8V.  The actual value is stored in the manufacturing token
     TOKEN_MFG_1V8_REG_VOLTAGE. The token stores the value in 10^-4, but we
     need 10^-3 so divide by 10. If this token is not set (0xFFFF), then
     assume 1800mV. */
  halCommonGetMfgToken(&vregOutTok, TOKEN_MFG_1V8_REG_VOLTAGE);
  if(vregOutTok == 0xFFFF)
  {
    vregOutTok = 1800;
  } else
  {
    vregOutTok /= 10;
  }
  /* Adjust vregOutTok if in boost mode, which raises Vreg by 2 clicks.
     Multiply it by the ratio (47 - regTrimNormal) / (47 - regTrimBoost)
     where regTrimNormal and regTrimBoost are the regulator trim values for
     the two modes (from regTrim1V8 token).  The 47 - trim comes from the
     ratio of resistors used in the bandgap circuit.  The correction is
     typically +5% (regTrimNormal is usually 3 or 4 and boost is 2 higher). */
  if (regTrimBoostMode)
   {
    tokTypeMfgRegTrim regTrim;
    uint8_t regTrimNormal, regTrimBoost;

    halCommonGetMfgToken(&regTrim, TOKEN_MFG_REG_TRIM);
    regTrimNormal = regTrim.regTrim1V8;
    if (regTrimNormal == 0xFF)
    { /* If token erased, assume reasonable defaults */
      regTrimNormal = 4; /* 4 from halInternalSetRegTrim() and Stromboli-S-015_*.doc */
    }
    regTrimBoost = regTrimNormal + 2;
    if (regTrimBoost > 7)
    {
      regTrimBoost = 7; /* Peg it at max of 7 */
    }
    /* Increase vregOutTok by the above-indicated ratio for boost compensation */
    vregOutTok = (((((uint32_t) vregOutTok) * (47 - regTrimNormal))
                                            + (47 - regTrimBoost )/2) /* rounding */
                                            / (47 - regTrimBoost));
  }
  return ((uint16_t) vregOutTok);
}

/**
  * @brief  Blocks the current thread of execution for the specified amount 
  *         of time, in microseconds.
  * @param  The value of the delay on us
  * @retval None
  */
void halCommonDelayMicroseconds(uint16_t us)
{
  /* halCommonDelayMicroseconds
           -enables MAC Timer and leaves it enabled.
           -does not touch MAC Timer Compare registers.
           -max delay is 65535 usec.
           NOTE: This function primarily designed for when the chip is running off of
                 the XTAL, which is the most common situation.  When running from
                 OSCHF, though, the clock speed is cut in half, so the input parameter
                 is divided by two.  With respect to accuracy, we're now limited by
                 the accuracy of OSCHF (much lower than XTAL). */
  uint32_t beginTime = MACTMR->CNTR;
  
  /* If we're not using the XTAL, the MAC Timer is running off OSCHF,
     that means the clock is half speed, 6MHz.  We need to halve our delay
     time. */
  if((CLK->HSECR2 & CLK_HSECR2_SW1) != CLK_HSECR2_SW1)
  {
    us >>= 1;
  }
    
  /* we have about 2us of overhead in the calculations */
  if(us<=2)
  {
    return;
  }
  
  /* MAC Timer is enabled in stmRadioInit, which may not have been called yet.
    This algorithm needs the MAC Timer so we enable it here. */
  MACTMR->CR |= MACTMR_CR_EN;

   /* since our max delay (65535<<1) is less than half the size of the 
    20 bit mac timer, we can easily just handle the potential for
    mac timer wrapping by subtracting the time delta and masking out
    the extra bits */
  while(((MACTMR->CNTR - beginTime)&MACTMR_CNTR_CNT) < us )
  {
    ; /* spin */
  }
}

/**
  * @brief  Blocks the current thread of execution for the specified amount
  *         of time, in milliseconds
  * Burning cycles for milliseconds is generally a bad idea, but it is
  * necessary in some situations.  If you have to burn more than 65ms of time,
  * the halCommonDelayMicroseconds function becomes cumbersome, so this
  * function gives you millisecond granularity.
  * @param  The value of the delay in ms
  * @retval None
  */
void halCommonDelayMilliseconds(uint16_t ms)
{
  if(ms == 0)
  {
    return;
  }
  
  while(ms-- > 0) 
  {
    halCommonDelayMicroseconds(1000);
  }
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
