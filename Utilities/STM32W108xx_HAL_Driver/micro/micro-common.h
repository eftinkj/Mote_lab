/**
  ******************************************************************************
  * @file    micro-common.h 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   Utility and convenience functions for STM32W108 microcontroller,
  *          common to both the full and minimal hal.
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

/** @defgroup micro-common
  * @brief micro-common modules
  * @{
  */
/* Private define ------------------------------------------------------------*/
#ifndef __STM32W108XX_MICRO_COMMON_H__
#define __STM32W108XX_MICRO_COMMON_H__

#ifndef __STSTATUS_TYPE__
#define __STSTATUS_TYPE__
  /* This is necessary here because halSleepForQsWithOptions returns an
   * StStatus and not adding this typedef to this file breaks a
   * whole lot of builds. */
  typedef uint8_t StStatus;
#endif /* __STSTATUS_TYPE__ */


#define PORTA (0 << 3)
#define PORTB (1 << 3)
#define PORTC (2 << 3)

      /**
       * @brief Some registers and variables require identifying GPIO by
       * a single number instead of the port and pin.  This macro converts
       * Port A pins into a single number.
       */
#define PORTA_PIN(y) (PORTA|y)
      /**
       * @brief Some registers and variables require identifying GPIO by
       * a single number instead of the port and pin.  This macro converts
       * Port B pins into a single number.
       */
#define PORTB_PIN(y) (PORTB|y)
      /**
       * @brief Some registers and variables require identifying GPIO by
       * a single number instead of the port and pin.  This macro converts
       * Port C pins into a single number.
       */
#define PORTC_PIN(y) (PORTC|y)

      /**
       * @brief Some registers and variables require identifying GPIO by
       * a single number instead of the port and pin.  This macro converts
       * Port C pins into a single number.
       */
#define PORTx_PIN(x, y) (x|y)

/* Private typedef -----------------------------------------------------------*/

typedef uint8_t WakeupXtalState;

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO WakeupXtalState wakeupXtalState;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/** @defgroup  micro-common_Private_Functions
  * @{
  */

/**
  * @brief  Initializes microcontroller-specific peripherals. 
  * @param  None
  * @retval None
  */
void halInit(void);

/**
  * @brief  Restarts the microcontroller and therefore everything else. 
  * @param  None
  * @retval None
  */
void halReboot(void);

/**
  * @brief  Powers up microcontroller peripherals and board peripherals.
  * @param  None
  * @retval None
  */
void halPowerUp(void);

/**
  * @brief  Powers down microcontroller peripherals and board peripherals.
  * @param  None
  * @retval None
  */
void halPowerDown(void);

/** @brief Enumerations for the possible microcontroller sleep modes.
  * - SLEEPMODE_RUNNING
  *     Everything is active and running.  In practice this mode is not
  *     used, but it is defined for completeness of information.
  * - SLEEPMODE_IDLE
  *     Only the CPU is idled.  The rest of the chip continues running
  *     normally.  The chip will wake from any interrupt.
  * - SLEEPMODE_WAKETIMER
  *     The sleep timer clock sources remain running.  The RC is always
  *     running and the 32kHz XTAL depends on the board header.  Wakeup
  *     is possible from both GPIO and the sleep timer.  System time
  *     is maintained.  The sleep timer is assumed to be configured
  *     properly for wake events.
  * - SLEEPMODE_MAINTAINTIMER
  *     The sleep timer clock sources remain running.  The RC is always
  *     running and the 32kHz XTAL depends on the board header.  Wakeup
  *     is possible from only GPIO.  System time is maintained.
  * - SLEEPMODE_NOTIMER
  *     The sleep timer clock sources (both RC and XTAL) are turned off.
  *     Wakeup is possible from only GPIO.  System time is lost.
  */
typedef uint8_t SleepModes;
enum
{
  SLEEPMODE_RUNNING = 0,
  SLEEPMODE_IDLE = 1,
  SLEEPMODE_WAKETIMER = 2,
  SLEEPMODE_MAINTAINTIMER = 3,
  SLEEPMODE_NOTIMER = 4,
};

typedef uint8_t BootloaderModes;
enum
{
  BOOTLOADER_MODE_UART = 0,
  BOOTLOADER_MODE_OTA = 1
};

/** @brief  Blocks the current thread of execution for the specified
  *         amount of time, in microseconds. The function is implemented with 
  *         cycle-counted busy loops and is intended to create the short delays 
  *         required when interfacing with hardware peripherals.
  * @param  us:The specified time, in microseconds.Values should be 
  *         between 1 and 65535 microseconds.
  * @retval None
  * @ Note  The accuracy of the timing provided by this function is not specified,
  *         but a general rule is that when running off of a crystal oscillator it will
  *         be within 10us.  If the micro is running off of another type of oscillator
  *         (e.g. RC) the timing accuracy will potentially be much worse.
  */
void halCommonDelayMicroseconds(uint16_t us);

/** @brief  Request the appplication to enter in bootloader mode
  *         This function will check whwther the user flash contains the bootloader
  *         and if yes it will jump into it according to the user parameters.
  * @param  mode  The bootloader mode, 0 UART mode, 1 RF mode. All other
  *         values are reserved
  * @param  channel  The channel where the booloader will operate. 0 means
  *         default channel (only vaild for RF mode).
  * @param  panId  The panID where the booloader will operate. 0xFFFF means
  *         default panID (only vaild for RF mode).
  * @retval An error code or it will never return.
  */
StStatus halBootloaderStart(BootloaderModes mode, uint8_t channel, uint16_t panId);

/**
  * @brief  Configure an IO pin's operating mode
  * @param  io  The io pin to use, can be specified with the convenience macros
  *         PORTA_PIN(), PORTB_PIN(), PORTC_PIN()
  * @param  config   The configuration mode to use.
  * @retval None
  */
void halGpioConfig(uint32_t io, uint32_t config);

/**
  * @brief  Set/Clear single GPIO bit
  * @param  io  The io pin to use, can be specified with the convenience macros
  *         PORTA_PIN(), PORTB_PIN(), PORTC_PIN()
  * @param  value   A flag indicating whether to set or clear the io.
  * @retval None
  */
void halGpioSet(uint32_t io, boolean value);

/**
  * @brief  Calibrates the internal SlowRC to generate a 1024 Hz (1kHz) clock.
  * @param  None
  * @retval None
  */
void halInternalCalibrateSlowRc( void );

 /**
  * @brief  Calibrates the internal FastRC to generate a 12MHz clock.
  * @param  None
  * @retval None
  */
void halInternalCalibrateFastRc(void);

/**
  * @brief  Sets the trim values for the 1.8V and 1.2V regulators based upon
  *         manufacturing configuration.
  * @param  boostMode  Alter the regulator trim based upon the state
  *         of boost mode.  TRUE if boost mode is active, FALSE otherwise.
  * @retval None
  */
void halInternalSetRegTrim(boolean boostMode);

/**
  * @brief  Determine VREG_OUT in the current mode (normal or boost).
  * @param  None
  * @retval VREG_OUT in millivolts, depending on normal or boost mode
  */
uint16_t halInternalGetVreg(void);

/**
  * @brief  Takes a slow ADC measurement of VDD_PADS in millivolts.  Due to
  *         the conversions performed, this function takes slightly under 3.2ms with a
  *         variation across successive conversions approximately +/-2mv of the average
  *         conversion.
  * @param  None
  * @retval A slow measurement of VDD_PADS in millivolts.
  */
uint16_t stMeasureVddSlow(void);

/**
  * @brief  Takes a fast ADC measurement of VDD_PADS in millivolts.
  *         Due to the conversions performed, this function takes slightly under 150us
  *         with a variation across successive conversions approximately +/-20mv of
  *         the average conversion.
  * @param  None
  * @retval A fast measurement of VDD_PADS in millivolts.
  */
uint16_t stMeasureVddFast(void);

/**
  * @brief  Calibrates the GPIO pads.  This function is called from within
  *         the stack and HAL at appropriate times.
  * @param  None
  * @retval None
  */
void halCommonCalibratePads(void);


/**
  * @brief  This is the core function for enabling the XTAL, biasing
  *         the XTAL, checking the XTAL biasing, switching to the XTAL,
  *         configuring FCLK, and configuring flash access settings.  The ultimate
  *         result of calling this function until it returns TRUE is the chip is
  *         operating from the 24MHz XTAL, the XTAL is biased for lowest current
  *         consumption, the CPU's FCLK is being sourced from SYSCLK, and the flash
  *         is configured for optimal current consumption.
  *         The basic principle of this function is that it takes time for the
  *         XTAL to stabilize whenever it is enabled and/or the biasing is change;
  *         about 1.5ms every time the bias is changed.  This function will handle
  *         the XTAL configuration, set an interrupt event to indicate when the
  *         appropriate delay has elapsed, and return immediately.  This interrupt
  *         event should not be monitored directly by any code other than the clock
  *         module itself.  The state of the XTAL is learned by the return code
  *         of this function.  As long as this function returns FALSE, the XTAL
  *         is unstable.  Calling code can perform other actions until the XTAL
  *         is stable.
  *
  *         The suggested use of the four XTAL API functions is as follows:
  *         - halCommonStartXtal() is called once as soon as possible to start
  *         the XTAL.  Other actions may be performed while waiting for the XTAL
  *         to stabilize.
  *         - halCommonTryToSwitchToXtal() is called repeatedly to drive 
  *         the biasing and switch process.  Other actions that do not require
  *         a stable XTAL may be performed until this function returns TRUE.
  *         - halCommonSwitchToXtal() is called just once when a stable XTAL is
  *         required before moving on.  This function will block in the idle
  *         sleep state until the switch procedure has completed.
  *         - halCommonCheckXtalBiasTrim() is called periodically, after a switch
  *         has occurred, to check that the XTAL biasing is optimal and
  *         adjust if needed.
  *
  *         halCommonTryToSwitchToXtal() function will return immediately.  This
  *         function drives the switch process.  This function can be called
  *         any number of times and at anytime.
  * ]param  None
  * @retval TRUE: The chip has switched to and is now using the XTAL.  No further
  *         bias events are in process.
  *         FALSE: The XTAL is <b>unstable<b>.  The chip has not modified it's
  *         XTAL selection; it remains on the same clock source (OSCHF
  *         or XTAL).  There is a bias event in process.
  */
boolean halCommonTryToSwitchToXtal(void);

void halInternalPowerUpKickXtal(void);
void halInternalInitMgmtIsrLo(void);
void halInternalBlockUntilXtal(void);
void halCommonBlockUntilXtal(void);

enum {
  WAKEUP_XTAL_STATE_START          = 0,
  WAKEUP_XTAL_STATE_BEFORE_LO_EN   = 1,
  WAKEUP_XTAL_STATE_LO_EN          = 2,
  WAKEUP_XTAL_STATE_READY_SWITCH   = 3,
  WAKEUP_XTAL_STATE_WAITING_FINAL  = 4,
  WAKEUP_XTAL_STATE_FINAL          = 5,
};

/**
  * @brief  This function is intended to be called periodically, from the
  *         HAL and application, to check the XTAL bias trim is within
  *         appropriate levels and adjust if not.  It will return immediately.
  * Simply redirect to the primary switch function, which handles all XTAL
  *         switching and biasing activities.  We don't care about the return code.
  *         TRUE is ideal but we could get FALSE (even if on the XTAL) because there
  *         is a biasing event in process and the XTAL is unstable.
  * @param  None
  * @retval None
  * @Note   This function can be called any number of times and at anytime.
  */
#define halCommonCheckXtalBiasTrim() halCommonTryToSwitchToXtal()

/**
  * @brief  This function is intended to initiate the XTAL start, bias, and
  *         switch procedure. It will return immediately.  This allows the calling
  *         code to do other things while waiting for the XTAL to stabilize.
  * Simply redirect to the primary switch function, which handles all XTAL
  *         switching and biasing activities.  We don't care about the return code.
  *         TRUE is ideal but we could get FALSE (even if on the XTAL) because there
  *         is a biasing event in process and the XTAL is unstable.
  * @param  None
  * @retval None
  * @Note   The functions halCommonTryToSwitchToXtal() and halCommonSwitchToXtal() are
  *         intended for completing the process.  This function can be called any
  *         number of times and at anytime.
  */
#define halCommonStartXtal() halCommonTryToSwitchToXtal()

/**
  * @brief  This function switches the chip to using the XTAL.  This includes
  *         biasing the XTAL and changing the CPU to 24MHz (FCLK sourced from SYSCLK).
  *         This function <b>blocks</b> until the switch procedure has occurred.
  *         While blocking, this function invokes idle sleep to reduce current
  *         consumption.
  * @param  None
  * @retval None
  * @Note   It is possible to use this function as a replacement for 
  *         the three other XTAL APIs, including halCommonCheckXtalBiasTrim(), as
  *         long as blocking is acceptable. This function can be called any
  *         number of times and at anytime.
  */
void halCommonSwitchToXtal(void);

/**
  * @brief  Blocks the current thread of execution for the specified
  *         amount of time, in milliseconds.
  * @param  ms  The specified time, in milliseconds.
  * @retval None
  * @Note   The function is implemented with cycle-counted busy loops
  *         and is intended to create the short delays required when interfacing with
  *         hardware peripherals.  This function works by simply adding another
  *         layer on top of halCommonDelayMicroseconds().
  */
void halCommonDelayMilliseconds(uint16_t ms);

/**
  * @brief  Puts the microcontroller to sleep in a specified mode, allows
  *         the GPIO wake sources to be determined at runtime.  This function 
  *         requires the GPIO wake sources to be defined at compile time in the board
  *         file.
  * @param  sleepMode  A microcontroller sleep mode.
  * @param  gpioWakeBitMask  A bit mask of the GPIO that are allowed to wake
  *         the chip from deep sleep.  A high bit in the mask will enable waking
  *         the chip if the corresponding GPIO changes state.  bit0 is PA0, bit1 is
  *         PA1, bit8 is PB0, bit16 is PCO, bit23 is PC7, bits[31:24] are ignored.
  * @retval None
  * @Note   This routine always enables interrupts.
  */
void halSleepWithOptions(SleepModes sleepMode, uint32_t gpioWakeBitMask);

/**
  * @brief  ses the system timer to enter ::SLEEPMODE_WAKETIMER for
  *         approximately the specified amount of time (provided in quarter seconds),
  *         the GPIO wake sources can be provided at runtime.
  * @param  duration The amount of time, expressed in quarter seconds, that the
  *         micro should be placed into ::SLEEPMODE_WAKETIMER.  When the function returns,
  *         this parameter provides the amount of time remaining out of the original
  *         sleep time request (normally the return value will be 0).
  * @param  gpioWakeBitMask  A bit mask of the GPIO that are allowed to wake
  *         the chip from deep sleep.  A high bit in the mask will enable waking
  *         the chip if the corresponding GPIO changes state.  bit0 is PA0, bit1 is
  *         PA1, bit8 is PB0, bit16 is PCO, bit23 is PC7, bits[31:24] are ignored.
  * @retval An StStatus value indicating the success or
 *          failure of the command.
  * @Note   This routine always enables interrupts.
  * @Note   The maximum sleep time of the hardware is limited on STM32W108 platforms
  *         to 48.5 days.  Any sleep duration greater than this limit will wake up
  *         briefly (e.g. 16 microseconds) to reenable another sleep cycle.
  * @note   This function returns ::ST_SUCCESS and the duration parameter is
  *         decremented to 0 after sleeping for the specified amount of time.  If an
  *         interrupt occurs that brings the chip out of sleep, the function returns
  *         ::ST_SLEEP_INTERRUPTED and the duration parameter reports the amount of
  *         time remaining out of the original request.
  */
StStatus halSleepForQsWithOptions(uint32_t *duration, uint32_t gpioWakeBitMask);

/**
  * @brief  Provides access to assembly code which triggers idle sleep.
  * @param  None
  * @retval None
  */
void halInternalIdleSleep(void);

/**
  * @brief  Puts the microcontroller to sleep in a specified mode.  This
  *         internal function performs the actual sleep operation.  This function
  *         assumes all of the wake source registers are configured properly.
  * @param  sleepMode  A microcontroller sleep mode
  * @retval None
  * @note   This routine always enables interrupts.
  */
void halInternalSleep(SleepModes sleepMode);


/**
  * @brief  Obtains the events that caused the last wake from sleep.  The
  *
  * @param  None
  * @retval The events that caused the last wake from sleep. 
  * @Note   meaning of each bit is as follows:
  *         - [31] = WakeInfoValid
  *         - [30] = SleepSkipped
  *         - [29] = CSYSPWRUPREQ
  *         - [28] = CDBGPWRUPREQ
  *         - [27] = PWR_WAKESR_CORE
  *         - [26] = PWR_WAKESR_WRAP
  *         - [25] = PWR_WAKESR_COMPB
  *         - [24] = PWR_WAKESR_COMPA
  *         - [23:0] = corresponding GPIO activity
  *         WakeInfoValid means that ::halSleepWithOptions (::halInternalSleep) has been called
  *         at least once.  Since the power on state clears the wake event info,
  *         this bit says the sleep code has been called since power on.
  *         SleepSkipped means that the chip never left the running state.  Sleep can
  *         be skipped if any wake event occurs between going ::ATOMIC and transferring
  *         control from the CPU to the power management state machine.  Sleep can
  *         also be skipped if the debugger is connected (JTAG/SerialWire CSYSPWRUPREQ
  *         signal is set).  The net affect of skipping sleep is the Low Voltage
  *         domain never goes through a power/reset cycle.  
  */
uint32_t halGetWakeInfo(void);


/** @brief  Seeds the ::halCommonGetRandom() pseudorandom number
  *         generator.
  * @param  seed  A seed for the pseudorandom number generator.
  * @retval None
  * @Note   It should be called by the application during initialization with a seed
  *         from the radio randon number generator.
  */
void halCommonSeedRandom(uint32_t seed);

/** @brief  Runs a standard LFSR to generate pseudorandom numbers.
  * @param  None
  * @retval None
  * @Note   Called by the MAC in the stack to choose random backoff slots.
  *         Complicated implementations may improve the MAC's
  *         ability to avoid collisions in large networks, but it is \b critical to
  *         implement this function to return quickly.
  */
uint16_t halCommonGetRandom(void);

#endif /* __STM32W108XX_MICRO_COMMON_H__ */


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
