/**
  ******************************************************************************
  * @file    serial_utils.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   Driver for serial_utilities  management on STM32W108 MB851 board
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
#include "hal.h"
#include "error.h"
#include "hal_uart.h"
#include "serial_utils.h"

/** @addtogroup Shared
  * @{
  */

/** @addtogroup serial_utils
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  serial_utils_Private_Functions
  * @{
  */

/** @brief  write hex
  * @param  charBuffer
  * @param  value
  * @param  charCount    
  * @retval unsigned value
  */  
uint8_t *writeHex(uint8_t *charBuffer, uint16_t value, uint8_t charCount)
{
  uint8_t c = charCount;
  charBuffer += charCount;
  for (; c; c--) {
    uint8_t n = value & 0x0F;
    value = value >> 4;
    *(--charBuffer) = n + (n < 10
                           ? '0'
                           : 'A' - 10);
  }
  return charBuffer + charCount;
}

/** @brief  Write a string
  * @param  string    
  * @retval boolean value
  */ 
boolean serialWriteString(const char * string)
{
   while (*string != '\0')
   {
     __io_putchar(*string);
     string++;
   }
   return TRUE;
}

/** @brief  Wait on serial 
  * @param  string    
  * @retval None
  */ 
boolean serialWaitSend(void)
{
  __io_flush();
  return TRUE;
}

/** @brief  Read a serial input line
  * @param  data
  * @param  max
  * @param  index    
  * @retval boolean value
  */ 
boolean serialReadPartialLine(char *data, uint8_t max, uint8_t * index)
{
  boolean err;
  uint8_t ch;

  if (((*index) == 0) || ((*index) >= max))
    data[0] = '\0';

  for (;;) {   
    err = serialReadByte(&ch);
    /* no new serial char?, keep looping */
    if (!err) return err;

    /* handle bogus characters */
    if ( ch > 0x7F ) continue;

    /* handle leading newline - fogBUGZ # 584 */
    if (((*index) == 0) &&
        ((ch == '\n') || (ch == 0))) continue;

    /* Drop the CR, or NULL that is part of EOL sequence. */
    if ((*index) >= max) {
      *index = 0;
      if ((ch == '\r') || (ch == 0)) continue;
    }

    /* handle backspace */
    if ( ch == 0x8 ) {
      if ( (*index) > 0 ) {
        /* delete the last character from our string */
        (*index)--;
        data[*index] = '\0';
        /* echo backspace */
        serialWriteString("\010 \010");
      }
      /* don't add or process this character */
      continue;
    }

    /* if the string is about to overflow, fake in a CR */
    if ( (*index) + 2 > max ) {
      ch = '\r';
    }

    serialWriteByte(ch); /* term char echo */

    /* upcase that char */
    if ( ch>='a' && ch<='z') ch = ch - ('a'-'A');

    /* build a string until we press enter */
    if ( ( ch == '\r' ) || ( ch == '\n' ) ) {
      data[*index] = '\0';

      if (ch == '\r') {
        serialWriteByte('\n'); /* "append" LF */
        *index = 0;                       /* Reset for next line; \n next */
      } else {
        serialWriteByte('\r'); /* "append" CR */
        *index = max;          /* Reset for next line; \r,\0 next */
      }

      return TRUE;
    } 
      
    data[(*index)++] = ch;
  }
}

/** @brief  Write serial data
  * @param  data
  * @param  length   
  * @retval boolean value: status
  */ 
boolean serialWriteData(uint8_t *data, uint8_t length)
{
  uint8_t i=0;
  for (i = 0; i < length; i++) 
  {
    __io_putchar(data[i]);
  }
  return TRUE;
}

/** @brief  Write a byte
  * @param  dataByte  
  * @retval boolean value
  */ 
boolean serialWriteByte(uint8_t dataByte)
{
  return serialWriteData(&dataByte, 1);
}

/** @brief  Non blocking read 
  * @param  dataByte: buffer containing the read byte if any
  * @retval TRUE if there is a data, FALSE otherwise
  */
boolean serialReadByte(uint8_t *dataByte)
{
  return __io_getcharNonBlocking(dataByte);
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
