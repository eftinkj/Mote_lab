/**
  ******************************************************************************
  * @file    command-interpreter2.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   processes commands incoming over the serial port.
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
#include "hal_uart.h"
#include "serial_utils.h"
#include "command-interpreter2.h"

/** @addtogroup Shared
  * @{
  */

/** @addtogroup command-interpreter
  * @{
  */
  
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static void callCommandAction(void);
static uint32_t stringToUnsignedInt(uint8_t argNum, boolean swallowLeadingSign);
static uint8_t charDowncase(uint8_t c);

/* Private typedef -----------------------------------------------------------*/

typedef struct {

  /* Finite-state machine's current state. */ 
  uint8_t state;

  /* The command line is stored in this buffer. */
  /* Spaces and trailing '"' and '}' characters are removed,
     and hex strings are converted to bytes. */ 
  uint8_t buffer[COMMAND_BUFFER_LENGTH];

  /* Indices of the tokens (command(s) and arguments) in the above buffer.
     The (+ 1) lets us store the ending index. */
  uint8_t tokenIndices[MAX_TOKEN_COUNT + 1];

  /* The number of tokens read in, including the command(s). */ 
  uint8_t tokenCount;

  /* Used while reading in the command line. */
  uint8_t index;

  /* First error found in this command. */ 
  uint8_t error; 

  /* Storage for reading in a hex string. A value of 0xFF means unused. */ 
  uint8_t hexHighNibble;

  /* The token number of the first true argument after possible nested commands. */
  uint8_t argOffset;

} CommandState;

CommandState commandState;

enum {
  CMD_AWAITING_ARGUMENT,
  CMD_READING_ARGUMENT,
  CMD_READING_STRING,                  /* have read opening " but not closing " */
  CMD_READING_HEX_STRING,              /* have read opening { but not closing } */
  CMD_READING_TO_EOL                   /* clean up after error */
};

#ifdef TEST
char *stateNames[] =
  {
    "awaiting argument",
    "reading argument",
    "reading string",
    "reading hex string",
    "reading to eol"
  };
#endif

/* Private functions ---------------------------------------------------------*/

/** @defgroup  command-interpreter_Private_Functions
  * @{
  */

/** @brief  Initialize the state maachine.
  * @param  None
  * @retval None
  */
void commandReaderInit(void)
{
  commandState.state = CMD_AWAITING_ARGUMENT;
  commandState.index = 0;
  commandState.tokenIndices[0] = 0;
  commandState.tokenCount = 0;
  commandState.error = CMD_SUCCESS;
  commandState.hexHighNibble = 0xFF;
}

/** @brief  Returns a value > 15 if ch is not a hex digit.
  * @param  character
  * @retval unsigned value
  */
static uint8_t hexToInt(uint8_t ch)
{
  return ch - (ch >= 'a' ? 'a' - 10
               : (ch >= 'A' ? 'A' - 10
                  : (ch <= '9' ? '0'
                     : 0)));
}

/** @brief  token lenght
  * @param  num
  * @retval unsigned value
  */
uint8_t tokenLength(uint8_t num)
{
  return (commandState.tokenIndices[num + 1] 
          - commandState.tokenIndices[num]);
}

/** @brief  token pointer
  * @param  tokenNum
  * @retval unsigned value
  */
uint8_t *tokenPointer(int8_t tokenNum)
{
  return commandState.buffer + commandState.tokenIndices[tokenNum];
}

/** @brief  processes command string. This is a state machine for parsing commands.
 *    If 'input' is NULL 'sizeOrPort' is treated as a port and characters are 
 *    read from there. Goto's are used where one parse state naturally falls 
 *    into another, and to save flash.
  * @param  input
  * @param  sizeOrPort  
  * @retval boolean value
  */
boolean processCommandString(uint8_t *input, uint8_t sizeOrPort)
{
  boolean isEol = FALSE;
  boolean isSpace, isQuote;

  while (TRUE) {
    uint8_t next;
    
    if (input == NULL) {
      switch (serialReadByte(&next)) {
      case TRUE:
        break;
      case FALSE:
        return isEol;
      default:
        commandState.error = CMD_ERR_PORT_PROBLEM;
        goto CMD_READING_TO_EOL;
      }
    } else if (sizeOrPort == 0) {
      return isEol;
    } else {
      next = *input;
      input += 1;
      sizeOrPort -= 1;
    }

    /*   fprintf(stderr, "[processing '%c' (%s)]\n", next, stateNames[commandState.state]); */

    if (next == '\r')
      continue;

    isEol =   (next == '\n');
    isSpace = (next == ' ');
    isQuote = (next == '"');

    switch (commandState.state) {

    case CMD_AWAITING_ARGUMENT:
      if (isEol) {
        callCommandAction();
      } else if (! isSpace) {
        if (isQuote) {
          commandState.state = CMD_READING_STRING;
        } else if (next == '{') {
          commandState.state = CMD_READING_HEX_STRING;
        } else {
          commandState.state = CMD_READING_ARGUMENT;
        }
        goto WRITE_TO_BUFFER;
      }
      break;

    case CMD_READING_ARGUMENT:
      if (isEol || isSpace) {
        goto END_ARGUMENT;
      } else {
        goto WRITE_TO_BUFFER;
      }

    case CMD_READING_STRING:
      if (isQuote) {
        goto END_ARGUMENT;
      } else { 
        goto WRITE_TO_BUFFER;
      }

    case CMD_READING_HEX_STRING: {
      boolean waitingForLowNibble = (commandState.hexHighNibble != 0xFF);
      if (next == '}') {
        if (waitingForLowNibble) {
          commandState.error = CMD_ERR_ARGUMENT_SYNTAX_ERROR;
          goto CMD_READING_TO_EOL;          
        }
        goto END_ARGUMENT;
      } else {
        uint8_t value = hexToInt(next);
        if (value < 16) {
          if (waitingForLowNibble) {
            next = (commandState.hexHighNibble << 4) + value;
            commandState.hexHighNibble = 0xFF;
            goto WRITE_TO_BUFFER;
          } else {
            commandState.hexHighNibble = value;
          }
        } else if (! isSpace) {
          commandState.error = CMD_ERR_ARGUMENT_SYNTAX_ERROR;
          goto CMD_READING_TO_EOL;
        }
      }
      break;
    }

    CMD_READING_TO_EOL:
      commandState.state = CMD_READING_TO_EOL;
      
    case CMD_READING_TO_EOL:
      if (isEol) {
        if (commandState.error != CMD_SUCCESS) {
          commandErrorHandler(commandState.error);
        }
        commandReaderInit();
      }
      break;

    END_ARGUMENT:
      if (commandState.tokenCount == MAX_TOKEN_COUNT) {
        commandState.error = CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
        goto CMD_READING_TO_EOL;
      }
      commandState.tokenCount += 1;
      commandState.tokenIndices[commandState.tokenCount] = commandState.index;
      commandState.state = CMD_AWAITING_ARGUMENT;
      if (isEol) {
        callCommandAction();
      }
      break;

    WRITE_TO_BUFFER:
      if (commandState.index == COMMAND_BUFFER_LENGTH) {
        commandState.error = CMD_ERR_STRING_TOO_LONG;
        goto CMD_READING_TO_EOL;
      }
      if (commandState.state == CMD_READING_ARGUMENT) {
        next = charDowncase(next);
      }
      commandState.buffer[commandState.index] = next;
      commandState.index += 1;
      break;
    }
  }
}

/** @brief  char Down case. Command lookup and processing 
  * @param  char
  * @retval char
  */
static uint8_t charDowncase(uint8_t c)
{
  if ('A' <= c && c <= 'Z')
    return c + 'a' - 'A';
  else
    return c;
}

/** @brief  returns first byte of the arg
  * @param  arg Num
  * @retval unsigned value
  */
static uint8_t firstByteOfArg(uint8_t argNum)
{
  uint8_t tokenNum = argNum + commandState.argOffset;
  return commandState.buffer[commandState.tokenIndices[tokenNum]];
}

/** @brief  returns first byte of the arg
  * @param  Command entry
  * @param  tokenNum  
  * @retval None
  */
static CommandEntry *commandLookup(CommandEntry *commandFinger, uint8_t tokenNum)
{
  uint8_t *command = tokenPointer(tokenNum);
  uint8_t length = tokenLength(tokenNum);
  uint8_t i;
  for (i = 0;
       commandFinger->action != NULL;
       i++, commandFinger++) {
    PGM_P name = commandFinger->name;
    uint8_t *finger = command;
    for (;; name++, finger++) {
      if (finger - command == length) {
        if (*name == 0) {
          return commandFinger;
        } else {
          break;
        }
      } else if (charDowncase(*finger) != charDowncase(*name)) {
        break;
      }
    }
  }
  return NULL;
}

/** @brief  call command action
  * @param  None 
  * @retval None
  */
static void callCommandAction(void)
{
  CommandEntry *commandFinger = CommandTable;
  uint8_t tokenNum = 0;
  uint8_t i;

  if (commandState.tokenCount == 0) {
    goto kickout2;
  }

  /* Lookup the command. */
  while (TRUE) {
    commandFinger = commandLookup(commandFinger, tokenNum);
    if (commandFinger != NULL) {
      tokenNum += 1;
      if (commandFinger->argumentTypes[0] == 'n') {
        /* Nested commands are implemented by overloading the action
           field of a command with a pointer to another table of
           commands.  The intermediate (void *) cast is necessary
           to appease the Phyton compiler, though Richard claims
           the compiler should not distinguish between a function
           pointer and any other pointer. */
        commandFinger = (CommandEntry *)(void *)(commandFinger->action);
      } else {
        commandState.argOffset = tokenNum;
        break;
      }
    } else {
      commandState.error = CMD_ERR_NO_SUCH_COMMAND;
      goto kickout;
    }
  }

  /* Validate the arguments. */
  for(i = 0; tokenNum < commandState.tokenCount; tokenNum++, i++) {
    uint8_t type = commandFinger->argumentTypes[i];
    uint8_t firstChar = firstByteOfArg(i);
    switch(type) {
      
    /* Integers */
    case 'u':
    case 'v':
    case 'w':
    case 's': {
      uint32_t limit = (type == 'u' ? 0xFF
                      : (type == 'v' ? 0xFFFF
                         : (type =='s' ? 0x7F : -1)));
      if (stringToUnsignedInt(i, TRUE) > limit) {
        commandState.error = CMD_ERR_ARGUMENT_OUT_OF_RANGE;
      }
      break;
    }

    /* String */
    case 'b':
      if (firstChar != '"' && firstChar != '{') {
        commandState.error = CMD_ERR_ARGUMENT_SYNTAX_ERROR;
      }
      break;

    case 0:
      commandState.error = CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
      break;

    default:
      commandState.error = CMD_ERR_INVALID_ARGUMENT_TYPE;
      break;
    }

    if (commandState.error != CMD_SUCCESS) {
      goto kickout;
    }
  }

  if (0 != commandFinger->argumentTypes[commandState.tokenCount 
                                        - commandState.argOffset]) {
    commandState.error = CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
  }

 kickout:

  if (commandState.error == CMD_SUCCESS) {
    (commandFinger->action)();
  } else {
    commandErrorHandler(commandState.error);
  }

 kickout2:

  commandReaderInit();
}

/** @brief  String to unsigned int. Retrieving arguments
  * @param  argNum
  * @param  swallowLeadingSign  
  * @retval result
  */
static uint32_t stringToUnsignedInt(uint8_t argNum, boolean swallowLeadingSign)
{
  uint8_t tokenNum = argNum + commandState.argOffset;
  uint8_t *string = commandState.buffer + commandState.tokenIndices[tokenNum];
  uint8_t length = tokenLength(tokenNum);
  uint32_t result = 0;
  uint8_t base = 10;
  uint8_t i;
  for (i = 0; i < length; i++) {
    uint8_t next = string[i];
    if (swallowLeadingSign && i == 0 && next == '-') {
      /* do nothing */
    } else if ((next == 'x' || next == 'X')
        && result == 0) {
      base = 16;
    } else {
      uint8_t value = hexToInt(next);
      if (value < base) {
        result = result * base + value;
      } else {
        commandState.error = CMD_ERR_ARGUMENT_SYNTAX_ERROR;
        return 0;
      }
    }
  }
  return result;
}

/** @brief  Unsigned command argument
  * @param  argNum 
  * @retval unsigned value
  */
uint32_t unsignedCommandArgument(uint8_t argNum) 
{
  return stringToUnsignedInt(argNum, FALSE);
}

/** @brief  Signed command argument
  * @param  argNum 
  * @retval Signed value
  */
int16_t signedCommandArgument(uint8_t argNum)
{
  boolean negative = (firstByteOfArg(argNum) == '-');
  int16_t result = (int16_t) stringToUnsignedInt(argNum, negative);
  return (negative ? -result : result);
}

/** @brief  string command argument
  * @param  argNum
  * @param  lenght
  * @retval unsigned value
  */
uint8_t *stringCommandArgument(int8_t argNum, uint8_t *length)
{
  uint8_t tokenNum = argNum + commandState.argOffset;
  uint8_t leadingQuote = (argNum < 0 ? 0 : 1);
  if (length != NULL) {
    *length = tokenLength(tokenNum) - leadingQuote;
  }
  return tokenPointer(tokenNum) + leadingQuote;
}

/** @brief  Copy string argument
  * @param  argNum
  * @param  destination
  * @param  max lenght
  * @param  leftpad    
  * @retval unsigned value: arg lenght
  */
uint8_t copyStringArgument(int8_t argNum, uint8_t *destination, uint8_t maxLength, boolean leftPad)
{
  uint8_t padLength;
  uint8_t argLength;
  uint8_t *contents = stringCommandArgument(argNum, &argLength);
  if (argLength > maxLength) {
    argLength = maxLength;
  }
  padLength = leftPad ? maxLength - argLength : 0;
  MEMSET(destination, 0, padLength);
  MEMCOPY(destination + padLength, contents, argLength);
  return argLength;
}

#if !defined(APPLICATION_HAS_COMMAND_ERROR_HANDLER)
#include <stdio.h>
PGM_NO_CONST PGM_P CommandErrorNames[] =
  {
    "no error",
    "serial port error",
    "no such command",
    "wrong number of arguments",
    "integer argument out of range",
    "argument syntax error",
    "string too long",
    "invalid argument type"
  };

/** @brief  Command error handler
  * @param  Command status  
  * @retval None
  */
void commandErrorHandler(CommandStatus status) {
  printf("%s\r\n", CommandErrorNames[status]);
}
#endif

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
