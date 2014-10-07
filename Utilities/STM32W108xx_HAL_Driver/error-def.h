/**
  ******************************************************************************
  * @file    error-def.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Return-code definitions for API functions.
  * @verbatim

 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================

    (#) Many SimpleMAC API functions return an StStatus value to indicate
        the success or failure of the call.
        Return codes are one byte long.
        This page documents the possible status codes and their meanings.
   
    (#) See error-def.h for source code.
    (#) See also error.h for information on how the values for the return codes are
        built up from these definitions.
        The file error-def.h is separated from error.h because utilities will use this file
        to parse the return codes.
   
   (#) Note Do not include error-def.h directly. It is included by error.h inside 
       an enum typedef.

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

/** @addtogroup HAL
  * @{
  */

/** @defgroup ERROR DEFINITION
  * @{
  */
/* Private define ------------------------------------------------------------*/

#ifndef __ERROR_DEF_H__
#define __ERROR_DEF_H__

/** @defgroup Generic Messages
  * @brief These messages are system wide.
  * @{
  */

/**
  * @brief The generic "no error" message.
  */
#define ST_SUCCESS  0x00

/**
  * @brief The generic "fatal error" message.
  */
#define ST_ERR_FATAL   0x01

/**
  * @brief An invalid value was passed as an argument to a function.
  */
#define ST_BAD_ARGUMENT 0x02

/**
  * @}
  */ 
   
/** @defgroup Serial Manager Errors
  * @{
  */

/**
  * @brief Specified an invalid baud rate.
  */
#define ST_SERIAL_INVALID_BAUD_RATE     0x20

/**
  * @brief Specified an invalid serial port.
  */
#define ST_SERIAL_INVALID_PORT       0x21

/**
  * @brief Tried to send too much data.
  */
#define ST_SERIAL_TX_OVERFLOW   0x22

/**
  * @brief There was not enough space to store a received character
  * and the character was dropped.
  */
#define ST_SERIAL_RX_OVERFLOW      0x23

/**
  * @brief Detected a UART framing error.
  */
#define ST_SERIAL_RX_FRAME_ERROR       0x24

/**
  * @brief Detected a UART parity error.
  */
#define ST_SERIAL_RX_PARITY_ERROR        0x25

/**
  * @brief There is no received data to process.
  */
#define ST_SERIAL_RX_EMPTY        0x26

/**
  * @brief The receive interrupt was not handled in time, and a 
  * character was dropped.
  */
#define ST_SERIAL_RX_OVERRUN_ERROR    0x27

/**
  * @}
  */ 
   
/** @defgroup MAC Errors
  * @{
  */ 

/**
  * @brief We expected to receive an ACK following the transmission, but
  * the MAC level ACK was never received.
  */
#define ST_MAC_NO_ACK_RECEIVED        0x40

/**
  * @}
  */ 
   
/** @defgroup Flash Errors
  * @{
  */ 

/** 
  * @brief A fatal error has occured while trying to write data to the
  * Flash.  The target memory attempting to be programmed is already programmed.
  * The flash write routines were asked to flip a bit from a 0 to 1, which is
  * physically impossible and the write was therefore inhibited.  The data in
  * the flash cannot be trusted after this error.
  */
#define ST_ERR_FLASH_WRITE_INHIBITED     0x46

/** 
 * @brief A fatal error has occured while trying to write data to the
 * Flash and the write verification has failed.  The data in the flash
 * cannot be trusted after this error, and it is possible this error is the
 * result of exceeding the life cycles of the flash.
 */
#define ST_ERR_FLASH_VERIFY_FAILED          0x47

/** 
  * @brief A fatal error has occured while trying to write data to the
  * flash, possibly due to write protection or an invalid address.  The data in
  * the flash cannot be trusted after this error, and it is possible this error
  * is the result of exceeding the life cycles of the flash.
  */
#define ST_ERR_FLASH_PROG_FAIL        0x4B

/** 
  * @brief A fatal error has occured while trying to erase flash, possibly
  * due to write protection.  The data in the flash cannot be trusted after
  * this error, and it is possible this error is the result of exceeding the
  * life cycles of the flash.
  */
#define ST_ERR_FLASH_ERASE_FAIL        0x4C

/**
  * @}
  */ 
   
/** @defgroup HAL Module Errors
  * @{
  */ 

/**
  * @brief Conversion is complete.
  */
#define ST_ADC_CONVERSION_DONE     0x80

/**
  * @brief Conversion cannot be done because a request is being 
  * processed.
  */
#define ST_ADC_CONVERSION_BUSY     0x81

/**
  * @brief Conversion is deferred until the current request has been 
  * processed.
  */
#define ST_ADC_CONVERSION_DEFERRED  0x82

/**
  * @brief No results are pending.
  */
#define ST_ADC_NO_CONVERSION_PENDING     0x84

/**
  * @brief Sleeping (for a duration) has been abnormally interrupted
  * and exited prematurely.
  */
#define ST_SLEEP_INTERRUPTED   0x85

/**
  * @}
  */ 
   
/** @defgroup PHY Errors
  * @{
  */ 
/**
  * @brief The transmit hardware buffer underflowed.
  */
#define ST_PHY_TX_UNDERFLOW       0x88

/**
  * @brief The transmit hardware did not finish transmitting a packet.
  */
#define ST_PHY_TX_INCOMPLETE      0x89

/**
  * @brief An unsupported channel setting was specified.
  */
#define ST_PHY_INVALID_CHANNEL            0x8A

/**
  * @brief An unsupported power setting was specified.
  */
#define ST_PHY_INVALID_POWER        0x8B

/**
  * @brief The requested operation cannot be completed because the radio
  * is currently busy, either transmitting a packet or performing calibration.
  */
#define ST_PHY_TX_BUSY       0x8C

/**
  * @brief The transmit attempt failed because all CCA attempts indicated that
  * the channel was busy.
  */
#define ST_PHY_TX_CCA_FAIL      0x8D

/**
  * @brief The software installed on the hardware doesn't recognize the
  * hardware radio type.
  */
#define ST_PHY_OSCILLATOR_CHECK_FAILED   0x8E

/**
  * @brief The expected ACK was received after the last transmission.
  */
#define ST_PHY_ACK_RECEIVED      0x8F

/**
  * @}
  */ 
   
/** @defgroup Miscellaneous Utility Errors
  * @{
  */ 
/**
  * @brief A critical and fatal error indicating that the version of the
  * stack trying to run does not match with the chip it is running on. The
  * software (stack) on the chip must be replaced with software that is
  * compatible with the chip.
  */
#define ST_STACK_AND_HARDWARE_MISMATCH   0xB0

/** 
  * @brief An index was passed into the function that was larger
  * than the valid range.
  */
#define ST_INDEX_OUT_OF_RANGE         0xB1

/**
  * @brief There are no empty entries left in the table.
  */
#define ST_TABLE_FULL           0xB4

/**
  * @brief The requested table entry has been erased and contains
  *   no valid data.
  */
#define ST_TABLE_ENTRY_ERASED 0xB6


/**
  * @}
  */ 
   
/** @defgroup Application Errors
  * @brief These error codes are available for application use.
  * @{
  */ 

/**
  * @brief This error is reserved for customer application use.
  *  This will never be returned from any portion of the network stack or HAL.
  */
#define ST_APPLICATION_ERROR_0       0xF0
#define ST_APPLICATION_ERROR_1       0xF1
#define ST_APPLICATION_ERROR_2       0xF2
#define ST_APPLICATION_ERROR_3       0xF3
#define ST_APPLICATION_ERROR_4       0xF4
#define ST_APPLICATION_ERROR_5       0xF5
#define ST_APPLICATION_ERROR_6       0xF6
#define ST_APPLICATION_ERROR_7       0xF7
#define ST_APPLICATION_ERROR_8       0xF8
#define ST_APPLICATION_ERROR_9       0xF9
#define ST_APPLICATION_ERROR_10      0xFA
#define ST_APPLICATION_ERROR_11      0xFB
#define ST_APPLICATION_ERROR_12      0xFC
#define ST_APPLICATION_ERROR_13      0xFD
#define ST_APPLICATION_ERROR_14      0xFE
#define ST_APPLICATION_ERROR_15      0xFF


#endif /*  __ERROR_DEF_H__ */


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
