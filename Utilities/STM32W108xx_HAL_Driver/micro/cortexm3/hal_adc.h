/**
  ******************************************************************************
  * @file    adc.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Header for STM32W A/D converter.
  *
 @verbatim

 ===============================================================================
                        ##### Sample A/D converter driver #####
 ===============================================================================
    [..]  To use the ADC system, include this file and ensure that
          :: halInternalInitAdc() is called whenever the microcontroller is
          started.  
    [..]  This code does not allow access to the continuous reading mode of
          the ADC, which some clients may require.
         (#) Many functions in this file return an ::StStatus value.  See
         error-def.h for definitions of all ::StStatus return values.

 @endverbatim
  ******************************************************************************
  *@attention
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

/** @addtogroup adc
  * Sample A/D converter driver.
  *
  * @note EM35x ADC driver support is preliminary and essentailly untested -
  * please do not attempt to use this ADC driver on this platform. It may be 
  * changed in a future release
  *
  *@{
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __STM32W108_ADC_H__
#define __STM32W108_ADC_H__

/* Includes ------------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* A type for the ADC User enumeration. */
typedef uint8_t ADCUser;
enum
{
  /** LQI User ID. */
  ADC_USER_LQI = 0,
  /** Application User ID */
  ADC_USER_APP = 1,
  /** Application User ID */
  ADC_USER_APP2 = 2
};

/** @brief Be sure to update ::NUM_ADC_USERS if additional users are added
  * to the ::ADCUser list.
  */
#define NUM_ADC_USERS 3 /* make sure to update if the above is adjusted  */


/** @brief A type for the channel enumeration 
  */
typedef uint8_t ADCChannelType;

/** @brief A type for the reference voltage enumeration 
  */
typedef uint8_t ADCReferenceType;

/** @brief A type for the sample rate enumeration 
  */
typedef uint8_t ADCRateType;

/** @brief ADCReferenceType Enumeration.
  *         Note: STM32W108 only supports one reference type: Internal
  */
enum {
  /** Internal reference. */
  ADC_REF_INT  = 42
};


/** @brief ADCRateType enumeration.  
  *         These rates are specified in the number of clock cycles
  *         That a conversion takes.  The actual time taken will depend on the
  *         selected ADC clock rate.  (Default is 6MHz)
  */
enum {
  /** Rate 32 cycles, 5 effective bits in ADC_DATA[15:11] */
  ADC_SAMPLE_CLOCKS_32   = 0x0,
  /** Rate 64 cycles, 6 effective bits in ADC_DATA[15:10] */
  ADC_SAMPLE_CLOCKS_64   = 0x1,
  /** Rate 128 cycles, 7 effective bits in ADC_DATA[15:9] */
  ADC_SAMPLE_CLOCKS_128  = 0x2,
  /** Rate 256 cycles, 8 effective bits in ADC_DATA[15:8] */
  ADC_SAMPLE_CLOCKS_256  = 0x3,
  /** Rate 512 cycles, 9 effective bits in ADC_DATA[15:7] */
  ADC_SAMPLE_CLOCKS_512  = 0x4,
  /** Rate 1024 cycles, 10 effective bits in ADC_DATA[15:6] */
  ADC_SAMPLE_CLOCKS_1024 = 0x5,
  /** Rate 2048 cycles, 11 effective bits in ADC_DATA[15:5] */
  ADC_SAMPLE_CLOCKS_2048 = 0x6,
  /** Rate 4096 cycles, 12 effective bits in ADC_DATA[15:4] */
  ADC_SAMPLE_CLOCKS_4096 = 0x7,
};


/** @brief ADC rates for compatibility with EM2xx applications.  For EM35x only
  *  applications, the ADC_SAMPLE_CLOCKS_nnn definitions should be used instead
  */
enum
{
  /** Rate 32 us, 5 effective bits in ADC_DATA[15:11] */
  ADC_CONVERSION_TIME_US_32   = 0x0,
  /** Rate 64 us, 6 effective bits in ADC_DATA[15:10] */
  ADC_CONVERSION_TIME_US_64   = 0x1,
  /** Rate 128 us, 7 effective bits in ADC_DATA[15:9] */
  ADC_CONVERSION_TIME_US_128  = 0x2,
  /** Rate 256 us, 8 effective bits in ADC_DATA[15:8] */
  ADC_CONVERSION_TIME_US_256  = 0x3,
  /** Rate 512 us, 9 effective bits in ADC_DATA[15:7] */
  ADC_CONVERSION_TIME_US_512  = 0x4,
  /** Rate 1024 us, 10 effective bits in ADC_DATA[15:6] */
  ADC_CONVERSION_TIME_US_1024 = 0x5,
  /** Rate 2048 us, 11 effective bits in ADC_DATA[15:5] */
  ADC_CONVERSION_TIME_US_2048 = 0x6,
  /** Rate 4096 us, 12 effective bits in ADC_DATA[15:4] */
  ADC_CONVERSION_TIME_US_4096 = 0x7,
};


#if defined(EMBER_TEST) && !defined(ADC_CR_CHSELN_BITS)
  #define ADC_CR_CHSELN_BITS (4)
#endif

/** @brief Macro that returns the ADCChannelType, from a given couple of sources
  * (positive and negative). To be used with halStartAdcConversion().
  */
#define ADC_SOURCE(P,N)   (( P << ADC_CR_CHSELN_BITS ) + N)

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/** @brief  Returns the ADC channel from a given GPIO. Its value can can be used
  *         inside the ADC_SOURCE(P,N) macro to retrieve the input pair for
  *         halStartAdcConversion().
  *
  * @param  io  The GPIO pin (it can be specified with the convenience macros
  *           PORTA_PIN(), PORTB_PIN(), PORTC_PIN() )
  * 
  * @retval The ADC_MUX value connected to the given GPIO.
  */
uint8_t halGetADCChannelFromGPIO(uint32_t io);

void halAdcSetClock(boolean slow);
boolean halAdcGetClock(void);
void halAdcSetRange(boolean high);
boolean halAdcGetRange(void);


/** @brief  Measures VDD_PADS in millivolts at the specified sample rate
  *         Due to the conversions performed, this function takes slightly under 250us
  *         with a variation across successive conversions approximately +/-20mv of
  *         the average conversion.
  * @param  ADC rate type
  * @retval A measurement of VDD_PADS in millivolts.
  */
uint16_t halMeasureVdd(ADCRateType rate);

/** @brief Initializes and powers-up the ADC.  Should also be
  * called to wake from sleep.
  */
void halInternalInitAdc(void);


/** @brief Shuts down the voltage reference and ADC system to
  * minimize power consumption in sleep.
  */
void halInternalSleepAdc(void);


/** @brief Starts an ADC conversion for the user specified by \c id.
  *
  * @appusage The application must set \c reference to the voltage
  * reference desired (see the ADC references enum,
  * set \c channel to the channel number
  * required (see the ADC channel enum), and set \c rate to reflect the
  * number of bits of accuracy desired (see the ADC rates enum)
  *
  * @param id        An ADC user.
  * 
  * @param reference Voltage reference to use, chosen from enum
  * ::ADCReferenceType
  * 
  * @param channel   Microprocessor channel number.
  * 
  * @param rate     Rate number
  * 
  * @return One of the following: 
  * - ST_ADC_CONVERSION_DEFERRED   if the conversion is still waiting 
  * to start.
  * - ST_ADC_CONVERSION_BUSY       if the conversion is currently taking 
  * place.
  * - ST_ERR_FATAL                 if a passed parameter is invalid.
  */
StStatus halStartAdcConversion(ADCUser id,
                                  ADCReferenceType reference,
                                  ADCChannelType channel,
                                  ADCRateType rate);

/** @brief Returns the status of a pending conversion
  * previously started by ::halStartAdcConversion().  If the conversion
  * is complete, writes the raw register value of the conversion (the unaltered
  * value taken directly from the ADC's data register) into \c value.
  *
  * @param id     An ADC user.
  *
  * @param value  Pointer to an uint16_t to be loaded with the new value. Take
  * note that the Atmel's ADC only generates 8-bit values which are loaded into
  * the lower 8 bits of \c value.
  * 
  * @return One of the following: 
  * - ::ST_ADC_CONVERSION_DONE       if the conversion is complete.
  * - ::ST_ADC_CONVERSION_DEFERRED   if the conversion is still waiting 
  * to start.
  * - ::ST_ADC_CONVERSION_BUSY       if the conversion is currently taking 
  * place.
  * - ::ST_ADC_NO_CONVERSION_PENDING if \c id does not have a pending 
  * conversion.
  */
StStatus halRequestAdcData(ADCUser id, uint16_t *value);

/** @brief Waits for the user's request to complete and then,
  * if a conversion was done, writes the raw register value of the conversion
  * (the unaltered value taken directly from the ADC's data register) into
  * \c value and returns ::ST_ADC_CONVERSION_DONE, or immediately
  * returns ::ST_ADC_NO_CONVERSION_PENDING.
  *
  * @param id     An ADC user.
  *
  * @param value  Pointer to an uint16_t to be loaded with the new value. Take
  * note that the Atmel's ADC only generates 8-bit values which are loaded into
  * the lower 8 bits of \c value.
  * 
  * @return One of the following: 
  * - ::ST_ADC_CONVERSION_DONE        if the conversion is complete.
  * - ::ST_ADC_NO_CONVERSION_PENDING  if \c id does not have a pending 
  * conversion.
  */
StStatus halReadAdcBlocking(ADCUser id, uint16_t *value);


/** @brief Calibrates or recalibrates the ADC system. 
  *
  * @appusage Use this function to (re)calibrate as needed. This function is
  * intended for the STM32W108 microcontroller, which requires proper calibration to calculate
  * a human readible value (a value in volts).  If the app does not call this
  * function, the first time (and only the first time) the function 
  * ::halConvertValueToVolts() is called, this function is invoked.  To
  * maintain accurate volt calculations, the application should call this
  * whenever it expects the temperature of the micro to change.
  *
  * @param id  An ADC user.
  *
  * @return One of the following: 
  * - ::ST_ADC_CONVERSION_DONE        if the calibration is complete.
  * - ::ST_ERR_FATAL                  if the calibration failed.
  */
StStatus halAdcCalibrate(ADCUser id);


/** @brief Convert the raw register value (the unaltered value taken
  * directly from the ADC's data register) into a signed fixed point value with
  * units 10^-4 Volts.  The returned value will be in the range -12000 to
  * +12000 (-1.2000 volts to +1.2000 volts).
  *
  * @appusage Use this function to get a human useful value.
  *
  * @param value  An uint16_t to be converted.
  * 
  * @return Volts as signed fixed point with units 10^-4 Volts. 
  */
int16_t halConvertValueToVolts(uint16_t value);

/** @brief Calibrates Vref to be 1.2V +/-10mV.
  *
  *  @appusage This function must be called from halInternalInitAdc() before
  *  making ADC readings.  This function is not intended to be called from any
  *  function other than halInternalInitAdc().  This function ensures that the
  *  master cell voltage and current bias values are calibrated before
  *  calibrating Vref.
  */
void stCalibrateVref(void);

/**
  * @} 
  */


#endif /* __STM32W108_ADC_H__ */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
