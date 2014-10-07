/**
  ******************************************************************************
  * @file    hal_adc.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   ADC HAL functions
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
#include "hal_adc.h"
#include "stm32w108xx_adc.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup adc
  * @{
  */

#if (NUM_ADC_USERS > 8)
  #error NUM_ADC_USERS must not be greater than 8, or uint8_t variables in adc.c must be changed
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Define a channel field that combines ADC_CR_CHSELP and ADC_CR_CHSELN */
#define ADC_CHAN        (ADC_CR_CHSELP | ADC_CR_CHSELN)
#define ADC_CHAN_BIT     3

/* Private macros ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

static uint16_t adcData;             /* conversion result written by DMA */
static uint8_t adcPendingRequests;   /* bitmap of pending requests */
__IO static uint8_t adcPendingConversion; /* id of pending conversion */
static uint8_t adcReadingValid;      /* bitmap of valid adcReadings */
static uint16_t adcReadings[NUM_ADC_USERS];
static uint16_t adcConfig[NUM_ADC_USERS];
static boolean adcCalibrated;
static int16_t Nvss;
static int16_t Nvdd;
/* Modified the original ADC driver for enabling the ADC extended range mode required for 
   supporting the STLM20 temperature sensor.
   NOTE: 
   The ADC extended range is inaccurate due to the high voltage mode bug of the general purpose ADC 
   (see STM32W108 errata). As consequence, it is not reccomended to use this ADC driver for getting
   the temperature values 
 */
#ifdef ENABLE_ADC_EXTENDED_RANGE_BROKEN
static int16_t Nvref;
static int16_t Nvref2;
#endif /* ENABLE_ADC_EXTENDED_RANGE_BROKEN */
static uint16_t adcStaticConfig;

/* Private function prototypes -----------------------------------------------*/
#ifdef __CC_ARM
__asm void DMB()
{
	DMB
}
#endif

/* Private functions ---------------------------------------------------------*/

/** @defgroup  adc_Private_Functions
  * @{
  */

/**
  * @brief  Set ADC clock
  * @param  slow: boolean value
  * @retval None
  */
void halAdcSetClock(boolean slow)
{
  if (slow) {
    adcStaticConfig |= ADC_CR_CLK;
  } else {
    adcStaticConfig &= ~ADC_CR_CLK;
  }
}

/**
  * @brief  Set ADC range
  * @param  high: boolean value
  * @retval None
  */
void halAdcSetRange(boolean high)
{
  if (high) {
    adcStaticConfig |= (ADC_CR_HVSELN | ADC_CR_HVSELP);
  } else {
    adcStaticConfig &= ~(ADC_CR_HVSELN | ADC_CR_HVSELP);
  }
}

/**
  * @brief  Get ADC clock
  * @param  None
  * @retval boolean value
  */
boolean halAdcGetClock(void)
{
  /* Fix original function code */
  return (adcStaticConfig & ADC_CR_CLK) ? TRUE : FALSE;
}

/**
  * @brief  Get ADC range
  * @param  None
  * @retval boolean value
  */
boolean halAdcGetRange(void)
{
  /* Fix original function code */
  return (adcStaticConfig & ((ADC_CR_HVSELN | ADC_CR_HVSELP))) ? TRUE : FALSE;
}

/**
  * @brief  hal ADC Isr
  * @param  None
  * @retval None
  */
void halAdcIsr(void)
{
  uint8_t i;
  uint8_t conversion = adcPendingConversion; /* fix '__IO' warning; costs no flash */

  /* make sure data is ready and the desired conversion is valid */
  if ( (ADC->ISR & ADC_IER_DMABFIE)
        && (conversion < NUM_ADC_USERS) ) {
    adcReadings[conversion] = adcData;
    adcReadingValid |= BIT(conversion); /* mark the reading as valid */
    /* setup the next conversion if any */
    if (adcPendingRequests) {
      for (i = 0; i < NUM_ADC_USERS; i++) {
        if (BIT(i) & adcPendingRequests) {
          adcPendingConversion = i;     /* set pending conversion */
          adcPendingRequests ^= BIT(i); /* clear request: conversion is starting */
          ADC->CR = adcConfig[i]; 
          break; /* conversion started, so we're done here (only one at a time) */
        }
      }
    } else {                                /* no conversion to do */
      ADC->CR = 0;                          /* disable adc */
      adcPendingConversion = NUM_ADC_USERS; /* nothing pending, so go "idle" */
    }
  }
  ADC->ISR = 0xFFFF;
  
 /* asm("DMB"); */
  
#if (defined (__ICCARM__) || defined (__GNUC__))
  asm("DMB");
#elif defined __CC_ARM
	DMB();
#else 
  #error "Inline assembler syntax expected"  
#endif

}

/**
  * @brief  start next conversion
  * @param  None
  * @retval ADC user
  */
/* An internal support routine called from functions below.
   Returns the user number of the started conversion, or NUM_ADC_USERS
   otherwise. */
ADCUser startNextConversion(void)
{
  uint8_t i;
  
  ATOMIC (
    /* start the next requested conversion if any */
    if (adcPendingRequests && !(ADC->CR & ADC_CR_ADON)) {
      for (i = 0; i < NUM_ADC_USERS; i++) {
        if ( BIT(i) & adcPendingRequests) {
          adcPendingConversion = i;     /* set pending conversion */
          adcPendingRequests ^= BIT(i); /* clear request */
          ADC->CR = adcConfig[i];       /* set the configuration to desired */
          ADC->ISR = 0xFFFF;
          NVIC->ISER[0] = NVIC_IxxR_ADC;
	  break;                       /* see DDTS MBTst38936 */
        }
      }
    } else {
      i = NUM_ADC_USERS;
    }
  )
  return i;
}

/**
  * @brief  Initialize internal ADC 
  * @param  None
  * @retval None
  */
void halInternalInitAdc(void)
{
  ADC_InitTypeDef ADC_InitStruct;
  
  /* reset the state variables */
  adcPendingRequests = 0;
  adcPendingConversion = NUM_ADC_USERS;
  adcCalibrated = FALSE;
  adcStaticConfig = ADC_CR_CLK | ADC_CR_ADON; /* init config: 1MHz, low voltage */

  /* set all adcReadings as invalid */
  adcReadingValid = 0;

  /* disable ADC, turn off HV buffers */
  ADC_DMA_ChannelReset();
  ADC_DMA_Config((uint32_t)&adcData, 1);
  ADC_InitStruct.ADC_Offset = 0;
  ADC_InitStruct.ADC_Gain = 0x00008000;
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_VoltageP = ADC_VoltageP_Low;
  ADC_InitStruct.ADC_VoltageN = ADC_VoltageN_Low;
  ADC_InitStruct.ADC_Input = ADC_SOURCE_VREF;
  ADC_InitStruct.ADC_DMAMode = ADC_DMAMode_AutoWrap;
  ADC_InitStruct.ADC_Clock = ADC_Clock_6MHz;
  ADC_Init(&ADC_InitStruct);
  
  ADC_DMA_ChannelLoadEnable();
  
  /* Clear the ADC interrupts and enable */
  ADC->IER = ADC_IER_DMABFIE;
  
  ADC->ISR = 0xFFFF;
  NVIC->ISER[0] = NVIC_IxxR_ADC;

  stCalibrateVref();
}

/**
  * @brief  Return the address of the token in NVM
  * @param  ADC user identifier
  * @param  ADC reference
  * @param  ADC channel
  * @param  ADC rate    
  * @retval St status
  */
StStatus halStartAdcConversion(ADCUser id, ADCReferenceType reference, ADCChannelType channel, ADCRateType rate)
{
   if(reference != ADC_REF_INT)
    return ST_ERR_FATAL;

  /* save the chosen configuration for this user */
  adcConfig[id] = ( ((rate << 13) & ADC_CR_SMP)
                  | ((channel << ADC_CHAN_BIT) & ADC_CHAN)
                  | adcStaticConfig);

  /* if the user already has a pending request, overwrite params */
  if (adcPendingRequests & BIT(id)) {
    return ST_ADC_CONVERSION_DEFERRED;
  }

  ATOMIC (
    /* otherwise, queue the transaction */
    adcPendingRequests |= BIT(id);
    /* try and start the conversion if there is not one happening */
    adcReadingValid &= ~BIT(id);
  )
  if (startNextConversion() == id)
    return ST_ADC_CONVERSION_BUSY;
  else
    return ST_ADC_CONVERSION_DEFERRED;
}

/**
  * @brief  request ADC data
  * @param  ADC user identifier
  * @param  value  
  * @retval St status
  */
StStatus halRequestAdcData(ADCUser id, uint16_t *value)
{
  /* Both the ADC interrupt and the global interrupt need to be enabled,
     otherwise the ADC ISR cannot be serviced. */
  boolean intsAreOff = ( INTERRUPTS_ARE_OFF()
                        || !(NVIC->ISER[0] & NVIC_IxxR_ADC) 
                        || !(ADC->IER & ADC_IER_DMABFIE) );
  StStatus stat;

  ATOMIC (
    /* If interupts are disabled but the flag is set,
       manually run the isr...
       FIXME -= is this valid??? */
    if( intsAreOff 
      && ( (NVIC->ISER[0] & NVIC_IxxR_ADC) && (ADC->IER & ADC_IER_DMABFIE) )) {
      halAdcIsr();
    }

    /* check if we are done */
    if (BIT(id) & adcReadingValid) {
      *value = adcReadings[id];
      adcReadingValid ^= BIT(id);
      stat = ST_ADC_CONVERSION_DONE;
    } else if (adcPendingRequests & BIT(id)) {
      stat = ST_ADC_CONVERSION_DEFERRED;
    } else if (adcPendingConversion == id) {
      stat = ST_ADC_CONVERSION_BUSY;
    } else {
      stat = ST_ADC_NO_CONVERSION_PENDING;
    }
  )
  return stat;
}

/**
  * @brief  Read ADC blocking
  * @param  ADC user identifier
  * @param  value 
  * @retval St status
  */
StStatus halReadAdcBlocking(ADCUser id, uint16_t *value)
{
  StStatus stat;

  do {
    stat = halRequestAdcData(id, value);
    if (stat == ST_ADC_NO_CONVERSION_PENDING)
      break;
  } while(stat != ST_ADC_CONVERSION_DONE);
  return stat;
}

/**
  * @brief  Calibrate the ADC
  * @param  ADC user identifier
  * @retval St status
  */
StStatus halAdcCalibrate(ADCUser id)
{
  StStatus stat;
/* Modified the original ADC driver for enabling the ADC extended range mode required for 
     supporting the STLM20 temperature sensor.
     NOTE: 
     The ADC extended range is inaccurate due to the high voltage mode bug of the general purpose ADC 
     (see STM32W108 errata). As consequence, it is not reccomended to use this ADC driver for getting
     the temperature values 
   */
#ifdef ENABLE_ADC_EXTENDED_RANGE_BROKEN
  if(halAdcGetRange()){
    
    halStartAdcConversion(id,
                          ADC_REF_INT,
                          ADC_SOURCE_VREF_VREF2,
                          ADC_CONVERSION_TIME_US_4096);
    
    stat = halReadAdcBlocking(id, (uint16_t *)(&Nvref));
    if (stat == ST_ADC_CONVERSION_DONE) {
      halStartAdcConversion(id,
                            ADC_REF_INT,
                            ADC_SOURCE_VREF2_VREF2,
                            ADC_CONVERSION_TIME_US_4096);
      stat = halReadAdcBlocking(id, (uint16_t *)(&Nvref2));
    }
    if (stat == ST_ADC_CONVERSION_DONE) {
      adcCalibrated = TRUE;
    } else {
      adcCalibrated = FALSE;
      stat = ST_ERR_FATAL;
    }
    return stat;    
    
  }  
#endif /* ENABLE_ADC_EXTENDED_RANGE_BROKEN */
  halStartAdcConversion(id,
                        ADC_REF_INT,
                        ADC_SOURCE_GND_VREF2,
                        ADC_CONVERSION_TIME_US_4096);
  stat = halReadAdcBlocking(id, (uint16_t *)(&Nvss));
  if (stat == ST_ADC_CONVERSION_DONE) {
    halStartAdcConversion(id,
                          ADC_REF_INT,
                          ADC_SOURCE_VREG2_VREF2,
                          ADC_CONVERSION_TIME_US_4096);
    stat = halReadAdcBlocking(id, (uint16_t *)(&Nvdd));
  }
  if (stat == ST_ADC_CONVERSION_DONE) {
    Nvdd -= Nvss;
    adcCalibrated = TRUE;
  } else {
    adcCalibrated = FALSE;
    stat = ST_ERR_FATAL;
  }
  return stat;
}

/**
  * @brief  Convert value to voltage
  * Use the ratio of the sample reading to the of VDD_PADSA/2, a 'known'
  * value (nominally 900mV in normal mode, but slightly higher in boost mode)
  * to convert to 100uV units. 
  * FIXME: support external Vref use #define of Vref, ignore VDD_PADSA 
  * @param  value
  * @retval voltage
  */
int16_t halConvertValueToVolts(uint16_t value)
{
  int32_t N;
  int16_t V;
  int32_t nvalue;
  
  if (!adcCalibrated) {
    halAdcCalibrate(ADC_USER_LQI);
  }
  if (adcCalibrated) {
 /* Modified the original ADC driver for enabling the ADC extended range mode required for 
     supporting the STLM20 temperature sensor.
     NOTE: 
     The ADC extended range is inaccurate due to the high voltage mode bug of the general purpose ADC 
     (see STM32W108 errata). As consequence, it is not reccomended to use this ADC driver for getting
     the temperature values 
   */
#ifdef ENABLE_ADC_EXTENDED_RANGE_BROKEN
    if(halAdcGetRange()){  /* High range. */
      
      N = (((int32_t)value + Nvref - 2*Nvref2) << 16)/(2*(Nvref-Nvref2));
      /* Calculate voltage with: V = (N * VREF) / (2^16) where VDD = 1.2 volts */
      /* Mutiplying by 1.2*10000 makes the result of this equation 100 uVolts */
      V = (int16_t)((N*12000L) >> 16);
      if (V > 21000) {  /* VDD_PADS ? */
        V = 21000;
      }      
      
    }
    else {
 #endif /* ENABLE_ADC_EXTENDED_RANGE_BROKEN */
      assert(Nvdd);
      nvalue = value - Nvss;
      /* Convert input value (minus ground) to a fraction of VDD/2. */
      N = ((nvalue << 16) + Nvdd/2) / Nvdd;
      /* Calculate voltage with: V = (N * Vreg/2) / (2^16) */
      /* Mutiplying by Vreg/2*10 makes the result units of 100 uVolts */
      /* (in fixed point E-4 which allows for 13.5 bits vs millivolts */
      /* which is only 10.2 bits). */
      V = (int16_t)((N*((int32_t)halInternalGetVreg())*5) >> 16);
      if (V > 12000) {
        V = 12000;
      }
 #ifdef ENABLE_ADC_EXTENDED_RANGE_BROKEN    
    }
 #endif /* ENABLE_ADC_EXTENDED_RANGE_BROKEN */   
  } else {
    V = -32768;
  }
  return V;
}

/**
  * @brief  Get ADC channel from GPIO
  * @param  Input output
  * @retval Channel
  */
uint8_t halGetADCChannelFromGPIO(uint32_t io)
{
	switch(io)
	{
	case PORTB_PIN(5):
		return ADC_MUX_ADC0;
			
	case PORTB_PIN(6):
		return ADC_MUX_ADC1;
		
	case PORTB_PIN(7):
		return ADC_MUX_ADC2;
		
	case PORTC_PIN(1):
		return ADC_MUX_ADC3;
		
	case PORTA_PIN(4):
		return ADC_MUX_ADC4;
		
	case PORTA_PIN(5):
		return ADC_MUX_ADC5;
		
	case PORTB_PIN(0):
		return ADC_MUX_VREF;
		
	default :
		return 0x0F; /* Invalid analogue source */
			
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
