/**
  ******************************************************************************
  * @file    bl.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   bootloader
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
#include <stdlib.h>
#include "bl.h"
#include "timer.h"

/** @addtogroup Shared
  * @{
  */

/** @addtogroup bootloader
  * @{
  */
  
/* Private defines -----------------------------------------------------------*/
#define APP_BUFF_SIZE 1024
#define APPLICATION_BASE_ADDRESS (0x08000000 + imageOffset)

#define ACK  0x79
#define NACK 0x1f

#define BTL_ACK_TIMEOUT 4000000 /* 4 sec. */

#define MAX_WRITE_PACKET_SIZE 96

#define TIMER_EXPIRED(startTime, timeout) (TIME_CurrentTime() > (startTime + timeout))

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static blStateType bootloadState;
static blStateType nextBootloadState;
static const uint8_t *image;
static uint32_t imageSize;
static uint32_t imageOffset;
static readFunctionPointerType readByte;
static writeFunctionPointerType writeByte;

static const uint8_t prodId256k[] = {0x01,0x09,0xB0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  bootloader_Private_Functions
  * @{
  */
  
/**
  * @brief  calculates checksum of n bytes located at address a
  * @param  address
  * @param  bytes number  
  * @retval result
  */
static uint8_t checksum(uint8_t *a, uint16_t n)
{
  uint16_t k;
  uint8_t result = 0;

  if (n == 1)
    result = (~a[0]) & 0xFF;
  else
    for(k=0; k<n; k++)
      result^= *(a+k);
  
  return result;
}

/**
  * @brief  reads bytes wuth timeout. DataLength = 0xFFFF means that first 
  *   bytes is the number of bytes to be read
  * @param  data
  * @param  data lenght
  * @param  timeout    
  * @retval return value
  */
static uint8_t readBytesWithTimeout (uint8_t *data, uint16_t dataLength, uint32_t timeout)
{
  uint8_t returnValue = 1;
  uint32_t startTime;
  uint8_t i = 0;
  
  if (dataLength == 0xFFFF) { /* Special flag to handle variable length reply */
    startTime = TIME_CurrentTime();
    while (1) {
      if (readByte(data)) {
	dataLength = *data + 1;
	data++;
	break;
      }
      if (TIMER_EXPIRED(startTime, timeout)) {
	returnValue = 0;
	break;
      }
    }
  }

  if (returnValue == 1) {
    while ((i < dataLength) && (returnValue == 1)) {
      startTime = TIME_CurrentTime();
      while (1) {
	if (readByte(data + i)) {
	  i++;
	  break;
	}
	if (TIMER_EXPIRED(startTime, timeout)) {
	  returnValue = 0;
	  break;
	}
      }
    }
  }
  return returnValue;
}

/**
  * @brief  bootloader command
  * @param  command code
  * @param  bootloader command arg
  * @param  reply lenght
  * @param  reply  
  * @retval return value
  */
uint8_t bootloadCommand(uint8_t cmdCode, btlCommandArgType *args, uint16_t replyLength, uint8_t *reply)
{
  uint8_t c;
  uint8_t returnValue = BOOTLOAD_COMMAND_SUCCESS;
  btlCommandArgType *arg;

  /* Send command code */
  if (returnValue == BOOTLOAD_COMMAND_SUCCESS) {
    writeByte(&cmdCode, 0);
    c = 255 - cmdCode;
    writeByte(&c, 1);

    if (readBytesWithTimeout(&c, 1, BTL_ACK_TIMEOUT)) {
      returnValue = (c != ACK) ? BOOTLOAD_COMMAND_NACKED : BOOTLOAD_COMMAND_SUCCESS;
    } else {
      returnValue = BOOTLOAD_COMMAND_TIMEOUT;
    }
  }

  /* Send optional arguments */
  if (returnValue == BOOTLOAD_COMMAND_SUCCESS) {
    for (arg = args; (arg != NULL) && (arg->length != 0); arg++) {
      uint8_t i, c;
      for (i = 0; i < arg->length; i++)
	writeByte(&(arg->data[i]), 0);
      c = checksum(arg->data, arg->length);
      writeByte(&c, 1);

      if (readBytesWithTimeout(&c, 1, BTL_ACK_TIMEOUT)) {
	returnValue = (c != ACK) ? BOOTLOAD_COMMAND_NACKED : BOOTLOAD_COMMAND_SUCCESS;
      } else {
	returnValue = BOOTLOAD_COMMAND_TIMEOUT;
      }
    }
  }
  
  /* Read possible reply */
  if (returnValue == BOOTLOAD_COMMAND_SUCCESS) {
    if (readBytesWithTimeout(reply, replyLength, BTL_ACK_TIMEOUT)) {
    } else {
      returnValue = BOOTLOAD_COMMAND_TIMEOUT;
    }
  }

  if ((returnValue == BOOTLOAD_COMMAND_SUCCESS) && 
      (cmdCode != READ) &&
      (cmdCode != WRITE) &&
      (cmdCode != WRITE_INCREMENTAL) &&
      (cmdCode != ERASE)) {
    if (readBytesWithTimeout(&c, 1, BTL_ACK_TIMEOUT)) {
      returnValue = (c != ACK) ? BOOTLOAD_COMMAND_NACKED : BOOTLOAD_COMMAND_SUCCESS;
    } else {
      returnValue = BOOTLOAD_COMMAND_TIMEOUT;
    }
  }

  return returnValue;
}

/**
  * @brief  rounds to power of 2 the remaining image fragment to upload 
  *   towards target
  * @param  Fragment Size 
  * @retval unsigned return value
  */
uint16_t resizeImageUnit(uint32_t FragmentSize)
{
  if (FragmentSize >= MAX_WRITE_PACKET_SIZE)
    return MAX_WRITE_PACKET_SIZE;
  else
    return ((FragmentSize + 1) & ~1);
}  

/**
  * @brief  bootloader initializing
  * @param  image desc type
  * @param  writeFunctionPointer
  * @param  writeFunctionPointer 
  * @retval None
  */
void blInit (imageDescType * imagePtr, writeFunctionPointerType writeByteFunc, readFunctionPointerType readByteFunc)
{
  bootloadState = BL_STATE_INIT;
  if (imagePtr) {
    image = imagePtr->imagePtr;
    imageSize = imagePtr->imageSize;
    imageOffset = imagePtr->imageOffset;
  }
  writeByte = writeByteFunc;
  readByte = readByteFunc;
}

/**
  * @brief  bootloader state machine
  * @param  bytes written
  * @retval bootloader state
  */
blStateType blStateMachine(uint32_t *bytesWritten)
{
#ifdef OTA_BL
  static uint8_t c;
#endif
  static uint8_t params[128];
  static uint32_t sentBytes = 0;     

  nextBootloadState = bootloadState;

  switch(bootloadState) { 
  case BL_STATE_INIT:
    { 
      sentBytes = 0;
#ifdef OTA_BL
      /* enter boot mode */
      resetExtensionBoard(0);
      /* send 0x7F signal to startup the transfer speed calculation of the bootloader */
      c = 0x7f;
      writeByte(&c);    
#endif             
      nextBootloadState = BL_STATE_INIT_OK;

    }
    break;
  case BL_STATE_INIT_OK:
#ifdef OTA_BL
    if (readByte(&c))
      {  
	/* ack received here after 0x7F (sent at UNDEF state) is taken in account as initialization succeeded */          
	if (c == ACK)
	  {
	    nextBootloadState = BL_STATE_GET;            
	  }    
      }
#else          
    nextBootloadState = BL_STATE_GET;            
#endif
    break;
  case BL_STATE_GET:
    {
      uint8_t reply[64];

      uint8_t state = bootloadCommand(GET, NULL, 0xFFFF, reply); 
      /* uint8_t state = btlCommand(GET,NULL); */
      if (state == BOOTLOAD_COMMAND_SUCCESS)
	{    
	  nextBootloadState = BL_STATE_GET_ID;       
	} else 
	{
	  nextBootloadState = BL_STATE_FAILURE;
	}
    }
    break;
  case BL_STATE_GET_ID:
    {
      uint8_t reply[3];
      uint16_t pageSize = 1024;
      /* uint8_t state = btlCommand(GET_ID,NULL); */
      uint8_t state = bootloadCommand(GET_ID, NULL, sizeof(reply), reply);

      if (state == BOOTLOAD_COMMAND_SUCCESS) {            
	uint8_t y;
	uint8_t s = 0;
        uint8_t pages;

        if (halCommonMemCompare(reply, prodId256k, 3) == 0) {
            pageSize = 2048;
        }

        pages = ((uint8_t) ((imageSize+pageSize-1) / pageSize));
	
	params[0] = pages - 1;
	for(y=0;y < pages ;y++) 
	  { 
	    params[y+1] = y + (imageOffset/pageSize);
	    s += y;
	  }
	s+=params[0];
	nextBootloadState = BL_STATE_ERASE;         
      } else {
	nextBootloadState = BL_STATE_FAILURE;
      }
    }
    break;
  case BL_STATE_ERASE:
    {
      btlCommandArgType args[2];  
      args[0].length = params[0]+2;
      args[0].data = params;
      args[1].length = 0;
      //if (btlCommand(ERASE,params) == COMMAND_SUCCESS) {    
      if (bootloadCommand(ERASE,args, 0, NULL) == BOOTLOAD_COMMAND_SUCCESS) {    
	nextBootloadState = BL_STATE_WRITE_SETUP;          
      } else {
	nextBootloadState = BL_STATE_FAILURE;
      }
    }
    break;
  case BL_STATE_WRITE_SETUP:
    {
      uint32_t curraddress = APPLICATION_BASE_ADDRESS + sentBytes;
          
      params[3] = (uint8_t) curraddress;
      params[2] = (uint8_t)(curraddress >> 8);
      params[1] = (uint8_t)(curraddress >> 16);
      params[0] = (uint8_t)(curraddress >> 24);
          
      nextBootloadState = BL_STATE_WRITE;
    }
    break;
  case BL_STATE_WRITE: /* Here is where we start writing the image */
    {
      btlCommandArgType args[3];
      uint8_t *data = params + 4;
      
      args[0].length = 4;
      args[0].data = (uint8_t *) params;
      args[1].length = (resizeImageUnit(imageSize - sentBytes)) + 1;
      halCommonMemCopy(data + 1, image + sentBytes, args[1].length - 1); 
      data[0] = args[1].length - 2;
      args[1].data = data;
      args[2].length = 0;
      if (bootloadCommand(WRITE,args, 0, NULL) == BOOTLOAD_COMMAND_SUCCESS) {
	sentBytes += data[0] + 1;
	if (sentBytes >= imageSize) {
	  uint32_t curraddress = APPLICATION_BASE_ADDRESS;
	  
	  params[3] = (uint8_t) curraddress;
	  params[2] = (uint8_t)(curraddress >> 8);
	  params[1] = (uint8_t)(curraddress >> 16);
	  params[0] = (uint8_t)(curraddress >> 24);
	  
	  nextBootloadState = BL_STATE_GO;
	} else {
	  nextBootloadState = BL_STATE_WRITE_INCREMENTAL;
	}
      } else {
	nextBootloadState = BL_STATE_FAILURE;
      }
    }
    break;
  case BL_STATE_WRITE_INCREMENTAL: /* Here is where we start writing the image */
    {
      btlCommandArgType args[2];
      uint8_t *data = params + 4;
      
      args[0].length = (resizeImageUnit(imageSize - sentBytes)) + 1;
      halCommonMemCopy(data + 1, image + sentBytes, args[0].length - 1); 
      data[0] = args[0].length - 2;
      args[0].data = data;
      args[1].length = 0;
      if (bootloadCommand(WRITE_INCREMENTAL,args, 0, NULL) == BOOTLOAD_COMMAND_SUCCESS) {
	sentBytes += data[0] + 1;
	if (sentBytes >= imageSize) {
	  uint32_t curraddress = APPLICATION_BASE_ADDRESS;
	  
	  params[3] = (uint8_t) curraddress;
	  params[2] = (uint8_t)(curraddress >> 8);
	  params[1] = (uint8_t)(curraddress >> 16);
	  params[0] = (uint8_t)(curraddress >> 24);
	  
	  nextBootloadState = BL_STATE_GO;
	} else {
	  nextBootloadState = BL_STATE_WRITE_INCREMENTAL;
	}
      } else {
	nextBootloadState = BL_STATE_FAILURE;
      }
    }
    break;
  case BL_STATE_GO:
    {
      btlCommandArgType args[2];
      args[0].length = 4;
      args[0].data = params;
      args[1].length = 0;

      if (bootloadCommand(GO,args, 0, NULL) == BOOTLOAD_COMMAND_SUCCESS)
	nextBootloadState = BL_STATE_IDLE;
      else
	nextBootloadState = BL_STATE_FAILURE;
    }
    break;
  case BL_STATE_FAILURE:
    nextBootloadState = BL_STATE_IDLE;
    break;
  case BL_STATE_IDLE:
    break;
  default:
    break;
  }
  bootloadState = nextBootloadState;
  *bytesWritten = sentBytes;
  
  return (bootloadState);
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
