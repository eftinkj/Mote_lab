/**
  ******************************************************************************
  * @file    mfg-token.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   Cortex-M3 Manufacturing-Token system
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
#include "hal_flash.h"
#include "mfg-token.h"

/** @addtogroup HAL
  * @{
  */

/** @defgroup mfg-token
  * @brief mfg-token modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
#define DEFINETOKENS
#define TOKEN_MFG(name,creator,iscnt,isidx,type,arraysize,...) \
const uint16_t TOKEN_##name = TOKEN_##name##_ADDRESS;
#include "token-manufacturing.h"
#undef TOKEN_DEF
#undef TOKEN_MFG
#undef DEFINETOKENS

/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* BIG_INFO block */
#define DATA_BIG_INFO_BASE                                   (0x08040000u)
#define DATA_BIG_INFO_END                                    (0x080407FFu)
#define DATA_BIG_INFO_SIZE                                   (DATA_BIG_INFO_END - DATA_BIG_INFO_BASE + 1)

/* Private variables ---------------------------------------------------------*/
static const uint8_t nullEui[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  mfg-token_Private_Functions
  * @{
  */


/**
  * @brief  Gets Mfg Token data. 
  * @param  Data
  * @param  Token
  * @param  The index
  * @param  The lenght of the data
  * @retval None
  */
void halInternalGetMfgTokenData(void *data, uint16_t token, uint8_t index, uint8_t len)
{
  uint8_t *ram = (uint8_t*)data;
  
  /* 0x7F is a non-indexed token.  Remap to 0 for the address calculation */
  index = (index==0x7F) ? 0 : index;
  
  if(token == MFG_EUI_64_LOCATION)
  {
    /* There are two EUI64's stored in the Info Blocks, St and Custom.
    0x0A00 is the address used by the generic EUI64 token, and it is
    token.c's responbility to pick the returned EUI64 from either St
    or Custom.  Return the Custom EUI64 if it is not all FF's, otherwise
    return the St EUI64. */
    tokTypeMfgEui64 eui64;
    halCommonGetMfgToken(&eui64, TOKEN_MFG_CUSTOM_EUI_64);
    if(MEMCOMPARE(eui64,nullEui, 8 /*EUI64_SIZE*/) == 0) 
    {
      halCommonGetMfgToken(&eui64, TOKEN_MFG_ST_EUI_64);
    }
    MEMCOPY(ram, eui64, 8 /*EUI64_SIZE*/);
  } 
  else
  {
    /* read from the Information Blocks.  The token ID is only the
    bottom 16bits of the token's actual address.  Since the info blocks
    exist in the range DATA_BIG_INFO_BASE-DATA_BIG_INFO_END, we need
    to OR the ID with DATA_BIG_INFO_BASE to get the real address. */
    uint32_t realAddress = (DATA_BIG_INFO_BASE|token) + (len*index);
    uint8_t *flash = (uint8_t *)realAddress;
    MEMCOPY(ram, flash, len);
  }
}

/**
  * @brief  Sets Mfg Token data.
  * @param  Token
  * @param  pointer to the data
  * @param  The lenght of the data
  * @retval None
  */
void halInternalSetMfgTokenData(uint16_t token, void *data, uint8_t len)
{
  StStatus flashStatus;
  uint32_t realAddress = (DATA_BIG_INFO_BASE|token);
  uint8_t * flash = (uint8_t *)realAddress;
  uint32_t i;
  
  /* The flash library (and hardware) requires the address and length to both
     be multiples of 16bits.  Since this API is only valid for writing to
     the CIB, verify that the token+len falls within the CIB. */
  assert((token&1) != 1);
  assert((len&1) != 1);
  assert((realAddress>=CIB_BOTTOM) && ((realAddress+len-1)<=CIB_TOP));
  
  /* CIB manufacturing tokens can only be written by on-chip code if the token
     is currently unprogrammed.  Verify the entire token is unwritten.  The
     flash library performs a similar check, but verifying here ensures that
     the entire token is unprogrammed and will prevent partial writes. */
  for(i=0;i<len;i++) 
  {
    assert(flash[i] == 0xFF);
  }
  
  /* Remember, the flash library operates in 16bit quantities, but the
     token system operates in 8bit quantities.  Hence the divide by 2. */
  flashStatus = halInternalFlashWrite(realAddress, data, (len/2));
  assert(flashStatus == ST_SUCCESS);
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
