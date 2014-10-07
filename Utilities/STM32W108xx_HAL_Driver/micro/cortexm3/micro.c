/**
  ******************************************************************************
  * @file    micro.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   STM32W108 micro specific minimal HAL functions
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
#include "board.h"
#include "error.h"
#include "micro-common.h"
#include "board.h"
#include "system-timer.h"
#include "hal_adc.h"
#include "memmap.h"
#include "iap_bootloader.h"

#include <stdlib.h>
#include <string.h>

/** @addtogroup HAL
  * @{
  */

/** @defgroup micro
  * @brief micro modules
  * @{
  */

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
#pragma pack(1)
typedef struct appSwitchStruct {
  uint32_t signature;
  uint8_t mode;
  uint8_t channel;
  union {
    uint16_t panID;
    uint16_t offset;
  } param;
} appSwitchStructType;
#pragma pack()

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static appSwitchStructType *appSwitch = (appSwitchStructType *) RAM_BOTTOM;
static uint16_t seed0 = 0xbeef;
static uint16_t seed1 = 0xface;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup  micro_Private_Functions
  * @{
  */

/**
  * @brief  Initializes microcontroller-specific peripherals. 
  * @param  None
  * @retval None
  */
void halInit(void)
{
  halCommonStartXtal();
  /* Disable the REG_EN external regulator enable signal.  Out of reset this
     signal overrides PA7.  By disabling it early, PA7 is reclaimed as a GPIO.
     If an external regulator is required, the following line of code should
     be deleted. */
  GPIO_DBG->DBGCR &= ~GPIO_DBGCR_EXTREGEN;
  halInternalSetRegTrim(FALSE);
  halInternalInitAdc();
  halCommonCalibratePads();
  halBoardPowerUp();
  halCommonSwitchToXtal();
  halInternalCalibrateFastRc();
  
  #ifndef DISABLE_WATCHDOG
    WDG_Cmd(ENABLE);
  #endif

  halInternalStartSystemTimer();
}

/**
  * @brief  Restarts the microcontroller and therefore everything else. 
    of the planets status  
  * @param  None
  * @retval None
  */
void halReboot(void)
{
  INTERRUPTS_OFF();

  /* FCLK must be 6MHz to allow the SYSRESETREQ signal to cleanly
     propagate and reset the chip.  Switch SYSCLK first since we need
     the cycles used by switching FCLK to guarantee the SYSCLK is
     stable and ready for SYSRESETREQ. */
  CLK->HSECR2 = 0x00; /* Guarantee SYSCLK is sourced from OSCHF */
  CLK->CPUCR = 0x00;   /* Guarantee FCLK is sourced from PCLK */
  
  SCB->AIRCR = (0x05FA0000 | SCB_AIRCR_SYSRESETREQ_Msk); /* trigger the reset */
  /* NOTE: SYSRESETREQ is not the same as nRESET. t will not do the debug
     pieces: DWT, ITM, FPB, vector catch, etc */
}

/**
  * @brief  Powers down microcontroller peripherals and board peripherals.  
  * @param  None
  * @retval None
  */
void halPowerDown(void)
{
  halBoardPowerDown();
}

/**
  * @brief  Powers up microcontroller peripherals and board peripherals.  
  * @param  None
  * @retval None
  */
void halPowerUp(void)
{
  halInternalPowerUpKickXtal();
  halInternalInitAdc();
  halCommonCalibratePads();  
  halBoardPowerUp();
  halInternalBlockUntilXtal();
}

/**
  * @brief  Seeds the pseudorandom number generator.
  * @param  seed 
  * @retval None
  */
void halCommonSeedRandom(uint32_t seed)
{
  seed0 = (uint16_t) seed;
  if (seed0 == 0)
    seed0 = 0xbeef;
  seed1 = (uint16_t) (seed >> 16);
  if (seed1 == 0)
    seed1 = 0xface;
}

/**
  * @brief  shift the current value and copy to the new val
  * @param  value
  * @param  taps
  * @retval the shifted value
  */
static uint16_t shift(uint16_t *val, uint16_t taps)
{
  uint16_t newVal = *val;

  if (newVal & 0x8000)
    newVal ^= taps;
  *val = newVal << 1;
  return newVal;
}

/**
  * @brief  pseudorandom number generator.   
  * @param  None
  * @retval random number generated
  */
uint16_t halCommonGetRandom(void)
{
  return (shift(&seed0, 0x0062) ^ shift(&seed1, 0x100B));
}

/**
  * @brief  Refer to the C stdlib memcpy().  
  * @param  dest
  * @param  source
  * @param  number of bytes
  * @retval None
  */
void halCommonMemCopy(void *dest, const void *source, uint16_t bytes)
{
  memcpy(dest, source, bytes);
}

/**
  * @brief  Refer to the C stdlib memcmp(). 
  * @param  source0
  * @param  source1
  * @param  number of bytes
  * @retval the result of the comparaison
  */
int8_t halCommonMemCompare(const void *source0, const void *source1, uint16_t bytes)
{
  return memcmp(source0, source1, bytes);
}

/**
  * @brief  Refer to the C stdlib memset().  
  * @param  dest
  * @param  the value
  * @param  number of bytes
  * @retval None
  */
void halCommonMemSet(void *dest, uint8_t val, uint16_t bytes)
{
  memset(dest, val, bytes);
}

/**
  * @brief  Manage the Isr of the chip
  * @param  None
  * @retval None
  */
void halManagementIsr(void)
{
  if (MGMT_IT->ISR & MGMT_ISR_HSECOMPLLIF) {
    /* The xtal has crossed the LO threshold indicating we're ready
     * to switch to it. */
    wakeupXtalState = WAKEUP_XTAL_STATE_READY_SWITCH;

    /* We're done with this interrupt. */
    MGMT_IT->IER &= ~MGMT_ISR_HSECOMPLLIF;
    MGMT_IT->ISR = MGMT_ISR_HSECOMPLLIF;
  }
}

/**
  * @brief  Start the bootloader mode
  * @param  the mode
  * @param  the channel
  * @param  panID
  * @retval the status
  */
StStatus halBootloaderStart(uint8_t mode, uint8_t channel, uint16_t panID)
{
  if (mode<= BOOTLOADER_MODE_OTA) 
  {
#ifdef CORTEXM3_STM32W108xB
    if (mode ==  BOOTLOADER_MODE_UART) 
    {
      uint8_t cut = *(__IO uint8_t *) 0x08040798;
      if (!((halFixedAddressTable.baseTable.type == FIXED_ADDRESS_TABLE_TYPE) &&
            (((halFixedAddressTable.baseTable.version & FAT_MAJOR_VERSION_MASK) 
              == 0x0000 ) &&
             (halFixedAddressTable.baseTable.version == 0x0003) /* checking presence of valid version */ 
               ) && (cut >= 2) && (cut <= 3)))     
	/* Cut not supported */
	return ST_ERR_FATAL;
    } 
    else 
    {
      /* Check that OTA bootloader is at the base of the flash */
      if (*((uint32_t *) (MFB_BOTTOM + 28)) == IAP_BOOTLOADER_APP_SWITCH_SIGNATURE)
      {
        appSwitch->channel = ((channel >= 11) && (channel <= 26)) ? channel :IAP_BOOTLOADER_DEFAULT_CHANNEL;
        appSwitch->param.panID = panID;
      }
      else 
      {
        return ST_ERR_FATAL;
      }
    }
#endif
#if defined (CORTEXM3_STM32W108CC)
    if (mode ==  BOOTLOADER_MODE_OTA)
    {
      appSwitch->channel = ((channel >= 11) && (channel <= 26)) ? channel :IAP_BOOTLOADER_DEFAULT_CHANNEL;
      appSwitch->param.panID = panID;
    }
#endif
    appSwitch->signature = IAP_BOOTLOADER_APP_SWITCH_SIGNATURE;
    appSwitch->mode = mode;
    halReboot();
  }
  
  return ST_BAD_ARGUMENT;
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
