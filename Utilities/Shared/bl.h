/**
  ******************************************************************************
  * @file    bl.h
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   bl header file.
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
#include "memmap.h"

/** @addtogroup Shared
  * @{
  */

/** @addtogroup bootloader
  * @{
  */ 

/* Private define ------------------------------------------------------------*/
  
#define BOOTLOAD_COMMAND_SUCCESS 0
#define BOOTLOAD_COMMAND_NACKED  1
#define BOOTLOAD_COMMAND_TIMEOUT 2

/* Bootloader command codes */
#define GET         0x00
#define GET_VERSION 0x01
#define GET_ID      0x02
#define ERASE       0x43
#define WRITE       0x31
#define WRITE_INCREMENTAL       0x36
#define READ        0x11
#define GO          0x21

/* Private typedef -----------------------------------------------------------*/

typedef enum blStateEnum {
  BL_STATE_INIT,
  BL_STATE_INIT_OK,
  BL_STATE_GET,
  BL_STATE_GET_ID,
  BL_STATE_ERASE,
  BL_STATE_WRITE_SETUP,
  BL_STATE_WRITE,
  BL_STATE_WRITE_INCREMENTAL,
  BL_STATE_IDLE,
  BL_STATE_GO,
  BL_STATE_FAILURE
} blStateType;

typedef struct btlCommandArgStruct {
  uint8_t length;
  uint8_t *data;
} btlCommandArgType;

typedef struct imageDescStruct {
  const uint8_t* imagePtr;
  uint32_t imageSize;
  uint32_t imageOffset;
} imageDescType;

typedef uint8_t (*writeFunctionPointerType)(uint8_t *, uint8_t);
typedef uint8_t (*readFunctionPointerType)(uint8_t *);

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/** @defgroup  bl_Private_Function_Prototypes
  * @{
  */

/**
  * @brief  Manages bootloader state machine.
  * @param  bytes written.
  * @retval bootloader state.
  */
blStateType blStateMachine(uint32_t *bytesWritten);

/**
  * @brief  initialize the bootloader.
  * @param  image Ptr.
  * @param  write byte function.
  * @param  read byte function    
  * @retval None.
  */
void blInit (imageDescType *imagePtr, writeFunctionPointerType writeByteFunc, readFunctionPointerType readByteFunc);

/**
  * @brief  Commands the bootloader.
  * @param  cmd code.
  * @param  args.
  * @param  reply lenght.
  * @param  reply.    
  * @retval None.
  */
uint8_t bootloadCommand(uint8_t cmdCode, btlCommandArgType *args, uint16_t replyLength, uint8_t *reply);

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
