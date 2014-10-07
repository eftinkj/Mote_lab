/**
  ******************************************************************************
  * @file    response.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   response program body
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
#include "serial_utils.h"
#include "error.h"

/** @addtogroup Shared
  * @{
  */

/** @addtogroup response
  * @{
  */ 

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*  Verbosity options */
boolean responseHeaderDisplay=TRUE;
boolean responseCommandDisplay=TRUE;
//boolean responseTagDisplay=TRUE;    /* disabling tag display not yet supported */
boolean responseDelimDisplay=TRUE;
boolean responseVerboseDisplay=TRUE;

/* --------------------------------
   A simple printf() implementation
   Supported format specifiers are:
    %% - percent sign
    %c - single byte character
    %s - ram string
    %p - flash string  (non-standard)
    %u - 2-byte unsigned decimal
    %d - 2-byte signed decimal
    %x %2x %4x - 1, 2, 4 byte hex value (always 0 padded) (non-standard)
   Special response format specifiers:
    %n : command name const string</li>
    %N : command name RAM string</li>
    %t : tag const string</li>
    %T : tag RAM string</li>
    && : ampersand
    additionally, if any of the response format specifiers are prefixed with 
    '&' instead of '%' they will be auto-wrapped in curly braces {}
    See the platformtest architecture spec for details.
 
   A few macros and a function help make this readable:
     - flush the local buffer to the output
     - ensure that there is some room in the local buffer
     - add a single byte to the local buffer
     - convert a nibble to its ascii hex character
     - convert an uint16_t to a decimal string
   Most of these only work within the emPrintfInternal() function.

   Current champion is %4x which writes 8 bytes.  (%s and %p can write
   more, but they do their own overflow checks). */

/* Private define ------------------------------------------------------------*/
#define LOCAL_BUFFER_SIZE 16
#define MAX_SINGLE_COMMAND_BYTES 10

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  printfFlushHandler *flushHandler; 
  uint8_t localBuffer[LOCAL_BUFFER_SIZE + MAX_SINGLE_COMMAND_BYTES];
  uint8_t *localBufferPointer;
  uint8_t *localBufferLimit;
  uint8_t count;
  uint8_t total;
} PrintfState; 

#define failIfNot(foo)  do { if(!(foo)) goto fail; } while(0)

/* Private functions ---------------------------------------------------------*/

/** @defgroup  response_Private_Functions
  * @{
  */

/** @brief  Flush buffer
  * @param  state   
  * @retval boolean value
  */
static boolean flushBuffer(PrintfState *state)
{
  uint8_t count = state->localBufferPointer - state->localBuffer;
  if (!(state->flushHandler(state->localBuffer, count)))
    return FALSE;
  state->total += count;
  state->localBufferPointer = state->localBuffer;
  return TRUE;
}

#define addByte(byte) \
do { *(state->localBufferPointer++) = (byte); } while (FALSE)

/** @brief  write hex
  * @param  charBuffer
  * @param  value
  * @param  charCount    
  * @retval unsigned value
  */
uint8_t *writeHex(uint8_t *charBuffer, uint16_t value, uint8_t charCount);


/** @brief  Write a string
  * @param  string    
  * @retval boolean value
  */   
static boolean printfWriteString(PrintfState *state, uint8_t *str)
{
    uint8_t len;
    
    for (len=0; str[len] != '\0'; len++)
      ;
    if(state->flushHandler(str, len)) {
      state->total += len;
      return TRUE;
    } else
      return FALSE;  
}

/** @brief  Returns number of characters written
  * @param  state
  * @param  string
  * @param  args    
  * @retval unsigned value
  */ 
uint8_t responsePrintfInternal(PrintfState *state,  const char * string, va_list args)
{
  boolean processingTag;
  boolean addDelim;
  uint16_t decimalValue = 0;
  boolean decimalValueAvailable;

  state->localBufferPointer = state->localBuffer;
  state->localBufferLimit = state->localBuffer + LOCAL_BUFFER_SIZE;
  state->total = 0;

  for (; *string; string++) {
    uint8_t next = *string;
    if ((next != '%') && (next != '&')) {
      if((next == '{') || (next == '}') || (next =='#')) { 
        if(responseDelimDisplay) addByte(next);
      } else {
        if(responseVerboseDisplay) addByte(next);
      }
    } else {
      string += 1;
      if((*string == '%') || (*string == '&')) {
        addByte(*string);
      } else {
        if(next == '&' && responseDelimDisplay) {
            addDelim = TRUE;
            /* initial bracket for all but command name, which is handled separate */
            if((*string != 'n') && (*string != 'N'))
              addByte('{');
        } else {
          addDelim = FALSE;
        }        
        processingTag = FALSE;
        switch (*string) {
          case 'n':
          case 'N': {
            uint8_t *str = va_arg(args, uint8_t *);
            if(responseCommandDisplay) {
              if(addDelim) {
                addByte('{'); addByte('(');
              }
              failIfNot(flushBuffer(state));
              failIfNot(printfWriteString(state, str));
              if(addDelim) {
                addByte(')'); addByte('}');
              }
            }
            addDelim = FALSE;  /* suppress add at end of loop */
            break; }
          case 't':
          case 'T':
              {
                uint8_t *str = va_arg(args, uint8_t *);
                failIfNot(flushBuffer(state));
                failIfNot(printfWriteString(state, str));
              }
            failIfNot(flushBuffer(state));
            addByte(':');
            processingTag = TRUE;
            string += 1;
            /* fall through to default case for handling of specifier after tag */
          default:
            /* pass-through to standard identifiers */
            
            {
              /* Allow space padding for ints, up to five digits plus sign. */
              uint8_t widthSpecifier = *string - '0';
              uint8_t formatSpecifier = *(string+1);
              if ( (widthSpecifier > 1)
                 &&(widthSpecifier <= 6)
                 &&( (formatSpecifier == 'd')
                   ||(formatSpecifier == 'u') ) ) {
                uint8_t numDigits = 0;
                uint16_t tempValue = va_arg(args, int);
                decimalValue = tempValue;
                decimalValueAvailable = TRUE;
                if ( (formatSpecifier == 'd') && (tempValue & 0x8000) ) {
                  /* Account for the '-' sign. */
                  numDigits++;
                  /* Convert from 2's complement so we can count digits. */
                  tempValue = ~tempValue + 1;
                }
                if (tempValue) {
                  while (tempValue) {
                    /* Count the digits in the value. */
                    numDigits++;
                    tempValue /= 10;
                  }
                }
                else {
                  /* Zero is one digit. */
                  numDigits++;
                }
                while (numDigits++ < widthSpecifier) {
                  /* Pad. */
                  addByte(' ');
                }
                string += 1;
              }
              else {
                decimalValueAvailable = FALSE;
              }
            }

            switch(*string) {
              case 'c':
                /* character */
                addByte(va_arg(args, unsigned int) & 0xFF);
                break;
              case 'p': 

              case 's': {
                /* string */
                uint8_t *arg = va_arg(args, uint8_t *);
                failIfNot(flushBuffer(state));
                failIfNot(printfWriteString(state, arg));
                break; }
              case 'u':         /* unsigned 2-byte */
              case 'd': {       /* signed 2-byte */
                uint16_t value = decimalValueAvailable
                             ? decimalValue
                             : va_arg(args, int) ;
                if (value == 0)
                  addByte('0');
                else {
                  uint8_t i = 0;
                  uint8_t numBuff[5]; /* max uint16_t is 5 characters (65535) */
                  if ( (*string == 'd') && (value & 0x8000) ) {
                    addByte('-');
                    if (value != 0x8000) {
                      value = (uint16_t) (- ((int16_t) value));
                    }
                  }
                  while (value) {
                    numBuff[i++] = value % 10;
                    value /= 10;
                  }
                  while (i--)
                    addByte(numBuff[i] + '0');
                }
                break;
              }
              case 'h':
              case 'H': 
                /* 0x prefix. */
                addByte('0'); addByte('x');
                /* fall through to processing of x */
              case 'x':
              case 'X': {
                /* single hex byte (always prints 4 chars, e.g. 0x1A) */
                uint8_t data = va_arg(args, int);
                state->localBufferPointer = writeHex(state->localBufferPointer, data, 2);
                break; }
              case '2':
                /* %2x only, 2 hex bytes (always prints 6 chars, e.g. 0x1A2B) */
              case '4':
                /* %4x only, 4 hex bytes (always prints 10 chars, e.g. 0x1A2B3C4D) */
                string += 1;
                if (*string == 'h' || *string == 'H') {
                  /* 0x prefix. */
                  addByte('0'); addByte('x');
                } else if (*string != 'x' && *string != 'X') {
                  string -= 1;
                  break;
                } 
                if (*(string - 1) == '2') {
                  uint16_t data = va_arg(args, int);
                  state->localBufferPointer = writeHex(state->localBufferPointer, data, 4);
                } else {
                  uint32_t data = va_arg(args, uint32_t);
                  state->localBufferPointer = writeHex(state->localBufferPointer, 
                                                             (uint16_t) (data >> 16), 
                                                             4);
                  state->localBufferPointer = writeHex(state->localBufferPointer, 
                                                             (uint16_t) data, 
                                                             4);
                }
                break;
              case '\0':
                if(!processingTag) string -=1;  /* allow top loop to catch it */
              default:
                /* unknown specifier, skip it unless.. */
                if(processingTag) {
                  /* there was no specifier immediately after, back-out the ':' */
                  state->localBufferPointer = state->localBuffer;
                  string -= 1;
                }

            }                          
        }
        if(addDelim) {
          addByte('}');
        }
      }
    }
    if (state->localBufferLimit <= state->localBufferPointer)
      failIfNot(flushBuffer(state));
  }
  
 /* done: */
  failIfNot(flushBuffer(state));
  return state->total;

 fail:
  return 0;
}

/** @brief  response header printf
  * @param  format string  
  * @retval status
  */ 
boolean responseHeaderPrintf(const char * formatString, ...)
{
  va_list ap;
  boolean status = TRUE;
  PrintfState state;

  if(!responseHeaderDisplay)
    return TRUE;

  state.flushHandler = serialWriteData;

  va_start (ap, formatString);
  if(0 == responsePrintfInternal(&state, formatString, ap))
    status = FALSE;
  va_end (ap); 
  return status;
}

/** @brief  print the response
  * @param  format string   
  * @retval status
  */ 
boolean responsePrintf(const char * formatString, ...)
{
  va_list ap;
  boolean status = TRUE;
  PrintfState state;

  state.flushHandler = serialWriteData;

  va_start (ap, formatString);
  if(0 == responsePrintfInternal(&state, formatString, ap))
    status = FALSE;
  va_end (ap); 
  return status;
}

/** @brief  response print flexible
  * @param  flushapp
  * @param  format string    
  * @retval status
  */ 
boolean responsePrintfFlexible(printfFlushHandler *flushapp, const char * formatString, ...)
{
  va_list ap;
  boolean status = TRUE;
  PrintfState state;

  state.flushHandler = flushapp;

  va_start (ap, formatString);
  if(0 == responsePrintfInternal(&state, formatString, ap))
    status = FALSE;
  va_end (ap); 

  return status;
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
