/**
  ******************************************************************************
  * @file    mouse.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012  
  * @brief   This is a mouse demo application, based on detection of MEMS movements
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

/** @addtogroup mouse_demos
 * MEMS mouse demo \see mouse.c for documentation.
 *
 *@{
 */

/* Includes ------------------------------------------------------------------*/
#ifndef USE_MB951
#include "mbxxx_i2c_lis302dl.h"
#endif /* USE_MB951 */

#include PLATFORM_HEADER
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "hal.h"
#include "phy-library.h"
#include "hal_uart.h"
#include <stdint.h>
#include <math.h>

/* Private define ------------------------------------------------------------*/
#define MAX_RETRIES 3
#define BUFFER_SIZE 64
#define TYPE_SERIAL 0
#define TYPE_BUTTON 1
#define STEADY_THRESHOLD 8

/* Define the number of quarter seconds between periodic maintenance events. */
/* The periodic events do not need to occur very often, so run these 
 * events once a minute. */
#define PERIODIC_MAINTENANCE_EVENTS_RATE (1*60*4)

#define BUTTON_STATE_IDLE         0
#define BUTTON_STATE_DOWN         1
#define BUTTON_STATE_PRESSED      2
#define BUTTON_STATE_WAIT_RELEASE 3
#define BUTTON_STATE_CLICKED      4

#define BUTTON_LEFT_MASK         ((1 << 0) | (1 << 1))
#define BUTTON_RIGHT_MASK        ((1 << 2) | (1 << 3))

#define BUTTON_LEFT_BIT          0
#define BUTTON_RIGHT_BIT         1

#define MOUSE_PAN_ID                   0x2506
#define MOUSE_SHORT_ID                 0x2506

#define INACTIVE_TIMEOUT (10 * 4) // 10 seconds
#define NO_MOVEMENT_TIMEOUT (10000) // in milliseconds
#define X_THRESHOLD 5
#define Y_THRESHOLD 5
/* Private variables ---------------------------------------------------------*/

/* radioTransmitConfig variables required from the SimpleMac library for packet 
   transmission */
RadioTransmitConfig radioTransmitConfig = RADIO_TRANSMIT_CONFIG_DEFAULTS;

/* generic data packet */
ALIGN_VAR(uint8_t txPacket[128], 2);
uint8_t txPacket[128] = { 
  0x0a,             /* length */
  0x41,             /* fcf - intra pan, no ack request, data */
  0x08,             /* fcf - src, dst mode */
  0x00,             /* seq */
  BYTE_0(MOUSE_PAN_ID),   /* dst pan l */
  BYTE_1(MOUSE_PAN_ID),   /* dst pan h */
  BYTE_0(MOUSE_SHORT_ID), /* dst addr l */
  BYTE_1(MOUSE_SHORT_ID), /* dst addr h */
  0x00              /* data */
};

/* buffer for received packet */
uint8_t rxPacket[128];
/* received packet flag */
boolean packetReceived = FALSE;
/* Retries counter */
uint8_t retries = MAX_RETRIES;

/* packet transmission complete flag */
__IO boolean txComplete = TRUE;

/* Number of transmission errors */
uint32_t txErrors = 0;

#ifndef USE_MB951
LIS302DL_InitTypeDef  LIS302DL_InitStruct;
#endif /* USE_MB951 */

/* Private function prototypes -----------------------------------------------*/
#ifdef MOUSE_TX
void processInput(void);
#endif /* MOUSE_TX */

/* Private Functions ---------------------------------------------------------*/
/* Add dummy abort function required for MDK-ARM toolchain */
#if defined ( __CC_ARM )
 void abort (void) {
 }
 #endif

/**
  * @brief  This function initialize the network and the table 
  *         of the planets status  
  * @param  x: 
  * @param  y:
  * @param  z:
  * @param  Pitch_Angle:
  * @param  Roll_Angle:
  * @retval None
  */
void getPitchAndRoll (int8_t x, int8_t y, int8_t z, int16_t *Pitch_Angle, int16_t *Roll_Angle)
{
    int16_t Longitudinal_axis, Lateral_axis, Vertical_axis;
    double inclination_tg_squere;

   /*     *************************************************** 
    *             ACCELEROMETER AXIS ORIENTATION
    *	  ***************************************************

    *                     -------------------------
    *                     |           TV          |
    *                     -------------------------

    *            
    *                         + Longitudinal_axis
    *                                  ^
    *                                  |
    *                                  |
    *                                -----
    *                                | R |
    *                                | E |
    *                                | M |
    *              + Lateral_axis <--| O |
    *                                | T |
    *                                | E |
    *                                -----
    *                                 (.) + Vertical_axis

    *    ***************************************************/

    /* Set next parameter accordingly to LIS302DL orientation */
    Longitudinal_axis = y;        /* Add the right sign (+/-) to match figure! */
    Lateral_axis = x;             /* Add the right sign (+/-) to match figure! */
    Vertical_axis = - z;     /* Add the right sign (+/-) to match figure! */       

    /* MOUSE UP DOWN HAVE TO BE PROPORTIONAL TO PITCH ANGLE */
    inclination_tg_squere = (int32_t)(Longitudinal_axis * Longitudinal_axis);
    inclination_tg_squere/= (int32_t)((Lateral_axis * Lateral_axis)+(Vertical_axis * Vertical_axis));
    inclination_tg_squere = sqrt(inclination_tg_squere);

    *Pitch_Angle = (int16_t) ((atan(inclination_tg_squere) * 360/ (2 * 3.14)) * ((Longitudinal_axis < 0) ? 1 : -1));

    /* MOUSE UP DOWN HAVE TO BE PROPORTIONAL TO ROLL ANGLE */
    inclination_tg_squere = (int32_t)(Lateral_axis * Lateral_axis);
    inclination_tg_squere/= (int32_t)((Vertical_axis * Vertical_axis)+(Longitudinal_axis * Longitudinal_axis));
    inclination_tg_squere = sqrt(inclination_tg_squere);

    *Roll_Angle = (int16_t) ((atan(inclination_tg_squere) * 360/ (2 * 3.14)) * ((Lateral_axis < 0) ? -1 : 1));
}

/**
  * @brief  This function allows to transmit the data
  * @param  lenght: lenght of the data
  * @param  data: data to be transmitted
  * @param  type: type of data (serial or button)
  * @retval None
  */
void sendData(uint8_t length, uint8_t *data, uint8_t type)
{
  uint32_t startTime = halCommonGetInt32uMillisecondTick ();
  while (txComplete == FALSE) 
  {
    uint32_t now = halCommonGetInt32uMillisecondTick();
    if ((now - startTime) > 200) 
    {
      startTime = now;
      STM_EVAL_LEDToggle(LED3);
    }
  }
  txPacket[8] = type;
  halCommonMemCopy(txPacket+9, data, length);
  txPacket[0] = length + 2 + 7 + 1;
  txPacket[3]++; /* increment sequence number */
  
  STM_EVAL_LEDOn(LED1);
  txComplete = FALSE;
  if (ST_RadioTransmit(txPacket) != ST_SUCCESS)
  {
    txComplete = TRUE;
  }
}

/**
  * @brief  Filter in software rebouncing
  * @param  button 
  * @param  state
  * @param  thisMillisecondTick: time of pressing
  * @param  thisMillisecondTick: time of releasing
  * @retval BUTTON_clicked or BUTTON_IDLE
  */
uint8_t buttonStateMachine(Button_TypeDef button, uint8_t state, uint32_t thisMillisecondTick, uint32_t *lastMillisecondsTick)
{
  uint8_t nextState = state;

  nextState = state;
  switch (state) {
  case BUTTON_STATE_IDLE:
    if (STM_EVAL_PBGetState(button) == 0x00)
    {
      nextState = BUTTON_STATE_DOWN;
      *lastMillisecondsTick = halCommonGetInt32uMillisecondTick();
    }
    break;
  case BUTTON_STATE_DOWN:
    if (STM_EVAL_PBGetState(button) == 0x00)
    {
      if ((thisMillisecondTick - *lastMillisecondsTick) > 50)
      {
        nextState = BUTTON_STATE_PRESSED;
      }
    }
    else
    {
      nextState = BUTTON_STATE_IDLE;
    }
    break;
  case BUTTON_STATE_PRESSED:
    nextState = BUTTON_STATE_WAIT_RELEASE;
    break;
  case BUTTON_STATE_WAIT_RELEASE:
    if (STM_EVAL_PBGetState(button) != 0x00)
    {
      nextState = BUTTON_STATE_CLICKED;
    }
    break;
  case BUTTON_STATE_CLICKED:
    nextState = BUTTON_STATE_IDLE;
  default:
    break;
  }

  return nextState;
}

/**
  * @brief  Check button status
  * @param  buttonMask
  * @param  buttonsStatus
  * @retval None
  */
void getButtonsStatus(uint8_t *buttonsMask, uint8_t *buttonsStatus)
{
  static uint8_t buttonStatus[5] = {BUTTON_STATE_IDLE, BUTTON_STATE_IDLE, BUTTON_STATE_IDLE, BUTTON_STATE_IDLE, BUTTON_STATE_IDLE};
  static uint32_t buttonStatusTime[5] = {0,0,0,0,0};
  uint32_t thisMillisecondTick = halCommonGetInt32uMillisecondTick();
  static Button_TypeDef buttonList[5];
  static uint8_t firstTime = 1;
  uint8_t i;

  if (firstTime)
  {
    firstTime = 0;
    buttonList[0] = BUTTON_S1;
#ifdef USE_MB950
    buttonList[1] = BUTTON_S2;
    buttonList[2] = BUTTON_S3;
    buttonList[3] = BUTTON_S4;
    buttonList[4] = BUTTON_S5;
#endif /* USE_MB950 */    
  }

  *buttonsMask = 0;
  *buttonsStatus = 0;

  for (i = 0; i < BUTTONn; i++)
  {
    buttonStatus[i] = buttonStateMachine(buttonList[i], buttonStatus[i], thisMillisecondTick, &buttonStatusTime[i]);
    if (buttonStatus[i] == BUTTON_STATE_PRESSED) 
    {
      /* Button i pressed */
      *buttonsMask |= (1 << i);
      *buttonsStatus |= (1 << i);
    }
    if (buttonStatus[i] == BUTTON_STATE_CLICKED)
    {
      /* Button i released */
      *buttonsMask |= (1 << i);
    }
  }
  return;
}

/**
  * @brief  Detect whether the mouse is steady or is moving
  * @param  x displacement
  * @param  y displacement
  * @retval Boolean flag to say whether the mouse is still or not
  */
boolean mouseSteady(int8_t x, int8_t y)
{
  static int8_t xWindowMin = 127, yWindowMin = 127, xWindowMax = -128, yWindowMax = -128;
  boolean returnValue = FALSE;

  if (x < xWindowMin)
  {
    xWindowMin = x;
  }
  if (x > xWindowMax)
  {
    xWindowMax = x;
  }
  if (y < yWindowMin)
  {
    yWindowMin = y;
  }
  if (y > yWindowMax)
  {
    yWindowMax = y;
  }

  if (((xWindowMax - xWindowMin) <= X_THRESHOLD) && ((yWindowMax - yWindowMin) <= Y_THRESHOLD))
  {
    returnValue = TRUE;
  }
  else
  {
    xWindowMin = 127;
    yWindowMin = 127; 
    xWindowMax = -128;
    yWindowMax = -128;
  }

  return returnValue;
}

/**
  * @brief  Processes packed received in RX
  * @param  None
  * @retval None
  */
void processReceivedPackets(void)
{
  uint8_t i;
  /* print out any packets that were received */
  if(packetReceived == TRUE)
  {
    STM_EVAL_LEDOn(LED3);
    if (rxPacket[8] == TYPE_SERIAL)
    {
      for (i = 9; i <= rxPacket[0]; i++)
      {
        putchar(rxPacket[i]);
      }
    }
    /* The packet has been processed, so free the single entry queue up */
    packetReceived = FALSE;
    STM_EVAL_LEDOff(LED3);
  }
}

/**
  * @brief  Perform periodic maintenance tasks required by STM32W108
  * @param  None
  * @retval None
  */

void periodicMaintenanceEvents(void)
{
  static uint16_t lastPeriodicEventsQsTick = 0;

  if (lastPeriodicEventsQsTick == 0) 
  {
    lastPeriodicEventsQsTick = halCommonGetInt16uQuarterSecondTick();
  }

  /* Run periodic maintenance events */
  if(elapsedTimeInt16u(lastPeriodicEventsQsTick, halCommonGetInt16uQuarterSecondTick()) > PERIODIC_MAINTENANCE_EVENTS_RATE)
  {
    if(ST_RadioCheckRadio())
    {
      ST_RadioCalibrateCurrentChannel();
    }
    halCommonCheckXtalBiasTrim();
    halCommonCalibratePads();
    lastPeriodicEventsQsTick = halCommonGetInt16uQuarterSecondTick();
  }
}

/**
  * @brief  Check if the device needs to go to deep sleep to save power
  * @param  active: lag to indicate whether the node is active or not
  * @param  batteryOperated: Flag to activate the power management
  * @retval None
  */
void powerManagement(boolean active, boolean batteryOperated)
{
  static uint32_t inactiveStartTime = 0;

  if (!batteryOperated)
  {
    return;
  }

  if (inactiveStartTime == 0)
  {
    inactiveStartTime = halCommonGetInt16uQuarterSecondTick();
  }
  
  if (active)
  {
    /* Reset inactive start time */
    inactiveStartTime = halCommonGetInt16uQuarterSecondTick();
  } 
  else
  {
    if(elapsedTimeInt16u(inactiveStartTime, halCommonGetInt16uQuarterSecondTick()) > INACTIVE_TIMEOUT) 
    {
      StStatus status;

      ST_RadioSleep();
      ATOMIC(
#ifndef USE_MB951
             LIS302DL_LowpowerCmd(LIS302DL_LOWPOWERMODE_POWERDOWN);
#endif /* USE_MB951 */
             halPowerDown();
#ifdef USE_MB950             
             halSleepWithOptions(SLEEPMODE_NOTIMER, BUTTON_S1_WAKE_SOURCE | BUTTON_S2_WAKE_SOURCE | BUTTON_S3_WAKE_SOURCE | BUTTON_S4_WAKE_SOURCE | BUTTON_S5_WAKE_SOURCE);
#else
             halSleepWithOptions(SLEEPMODE_NOTIMER, BUTTON_S1_WAKE_SOURCE);
#endif /* USE_MB950 */
             halPowerUp();
             )
        /* It is the application's responsibility to reinitialize the UART */
      uartInit();
      status = ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
      assert(status==ST_SUCCESS);  
      inactiveStartTime = halCommonGetInt16uQuarterSecondTick();
#ifndef USE_MB951
      /* Set configuration of LIS302DL*/
      LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
      LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
      LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
      LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
      LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
      LIS302DL_Init(&LIS302DL_InitStruct);
#endif /* USE_MB951 */ 
      printf("Wakeup %08lx\r\n", halGetWakeInfo());
    }
  }
}

/**
  * @brief  Check whether the system is battery powered or not
  * @param  None
  * @retval Flag to indicate whether battery operated or not
  */
boolean powerFromBatteries(void)
{
  uint16_t threshold = 3100;
  
  /* Optional printf */
  printf ("Measured Vdd (mV) = %d\r\n", halMeasureVdd(ADC_SAMPLE_CLOCKS_512));
  
#ifdef USE_MB950 
    /* MB950 has a BAT46 diode to avoid conflict between battery and USB power, so threshold is lowered by 400 mV */
    threshold -= 400;
#endif /* USE_MB950 */
    
  return halMeasureVdd(ADC_SAMPLE_CLOCKS_512) > threshold ? FALSE : TRUE;
}

/******************************************************************************/
/*                          SimpleMac callbacks functions                     */ 
/******************************************************************************/

/**
  * @brief  Radio Receiver callback function
  * @param  packet: received packet
  * @param  ackFramePendingSet: frame pending bit in the received packet
  * @param  time: MAC timer when the SFD was received
  * @param  errors: numbers of correlator erros in the packet
  * @param  rssi: energy detected in the packet
  * @retval None
  */
void ST_RadioReceiveIsrCallback(uint8_t *packet, boolean ackFramePendingSet, uint32_t time, uint16_t errors, int8_t rssi)
{
  /* note this is executed from interrupt context */
  uint8_t i;

  /* Copy the packet to a buffer that can be accessed from the main loop;
  don't do the copy if there is already a packet there being processed */
  if(packetReceived == FALSE) 
  {
    for(i=0; i<=packet[0]; i++)
    {
      rxPacket[i] = packet[i];
    }
    packetReceived = TRUE;
  }
}

/**
  * @brief  adio Transmit callback function
  * @param  status: status of the packet transmission
  * @param  sfdSentTime: MAC timer when the SFD was sent 
  * @param  framePending: TRUE if the received ACK indicates that data 
  *         is pending for this node
  * @retval None
  */

void ST_RadioTransmitCompleteIsrCallback(StStatus status, uint32_t sfdSentTime, boolean framePending)
{
  switch(status) {
  case ST_PHY_TX_CCA_FAIL:
  case ST_MAC_NO_ACK_RECEIVED:
    /* Retry */
    if (retries > 0)
    {
      retries--;
      ST_RadioTransmit(txPacket);
      break;
    } 
    else 
    {
      txErrors++;
    }
  case ST_SUCCESS:
  case ST_PHY_ACK_RECEIVED:
    retries = MAX_RETRIES;
    txComplete = TRUE;
    /* Clear led to indicate transmission complete */
    STM_EVAL_LEDOff(LED1);
    break;
  default:
    break;
  }
}

/**
  * @brief  Callback for Radio Short Id data pending
  * @param  shortId: shortId address
  * @retval TRUE/FALSE
  */
boolean ST_RadioDataPendingShortIdIsrCallback(uint16_t shortId)
{
  return FALSE;
}

/**
  * @brief  Callback for Radio Long  Id data pending
  * @param  longId: long id address
  * @retval TRUE/FALSE
  */
boolean ST_RadioDataPendingLongIdIsrCallback(uint8_t* longId)
{
  return FALSE;
}

/**
  * @brief  Radio overflow event callback
  * @param  None
  * @retval None
  */
void ST_RadioOverflowIsrCallback(void)
{
}

/**
  * @brief  Radio SFD sent event callback
  * @param  sfdSentTime: MAC timer when the SFD was sent 
  * @retval None
  */
void ST_RadioSfdSentIsrCallback(uint32_t sfdSentTime)
{
}

/**
  * @brief  Radio MAC timer comapre Event callback
  * @param  None
  * @retval None
  */
void ST_RadioMacTimerCompareIsrCallback(void)
{
}

/**
  * @brief  Mouse main program
  * @param  None
  * @retval None
*/
int main(void)
{
  /* Application main:  Entry point, initialization, command line implementation
     and event control.*/
  uint32_t seed;
  StStatus status = ST_SUCCESS;
#ifdef MOUSE_TX
  boolean batteryOperated = FALSE;
  char *role = "TX";
#else
  char *role = "RX";
#endif

  /* Initialization */
  halInit();

  ST_RadioGetRandomNumbers((uint16_t *)&seed, 2);
  halCommonSeedRandom(seed);
  uartInit();
  INTERRUPTS_ON();
#ifdef MOUSE_TX
  batteryOperated = powerFromBatteries();
  {
#if defined (USE_MB951)
      while (1) 
      {
        printf("Error this application requires a board with MEMS device\r\n");
        halCommonDelayMilliseconds(1000);
      }
#else 
      /* Set configuration of LIS302DL*/
      LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
      LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
      LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
      LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
      LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
      LIS302DL_Init(&LIS302DL_InitStruct);
#endif /* USE_MB951 */
  }
#else
  {
#if !defined (USE_MB851_REVC) && !defined (USE_MB851_REVD) && !defined (USE_MB954_REVB) && !defined (USE_MB954_REVC) && !defined (USE_MB950) && !defined (USE_MB951)
    while (1)
    {
      printf("Error this application requires a board with STM32F as PC interface\r\n");
      halCommonDelayMilliseconds(1000);
    }
#endif /* USE_MB851_REVC or USE_MB851_REVD or USE_MB954_REVB or USE_MB954_REVC or USE_MB950 or USE_MB951 */
  }
#endif
  /* Initialize radio (analog section, digital baseband and MAC)
  Leave radio powered */
  status = ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
  assert(status==ST_SUCCESS);

  /* Setup some node and pan ids.  The packet above is also sent to a device
  with the same node and pan id so that two nodes running this same image
  will talk to each other, even though its not right to have two nodes
  with the same node id */
  ST_RadioSetNodeId(MOUSE_SHORT_ID);
  ST_RadioSetPanId(MOUSE_PAN_ID);

  /* Set channel */
  ST_RadioSetChannel(ST_MIN_802_15_4_CHANNEL_NUMBER);

  printf("\r\nSimpleMAC (%s) Mouse demo (%s)\r\n",SIMPLEMAC_VERSION_STRING, role);

  while(1)
  {
#ifdef MOUSE_TX
    processInput();
    powerManagement(!txComplete, batteryOperated);
#else
    processReceivedPackets();
#endif
    periodicMaintenanceEvents();

  }
}

#ifdef MOUSE_TX
/**
  * @brief  Processes serial input and button presses
  * @param  None
  * @retval None
  */
void processInput(void)
{
  static uint8_t buffer[BUFFER_SIZE];
  static uint32_t lastMillisecondTick = 0, lastMouseMovemement = 0;
  uint32_t thisMillisecondTick;
  uint8_t buttonsStatus, buttonsMask;
  uint8_t buttonReport = 0;
  static uint8_t buttonTimeout = 0;
  static uint8_t lastButtonReport = 0;
  double gain = 1.0;
  int16_t Pitch_Angle, Roll_Angle;

  getButtonsStatus(&buttonsMask, &buttonsStatus);

  if (buttonsMask) 
  {
    /* Some bits are valid */
    if (buttonsMask & BUTTON_LEFT_MASK)
    {
      uint8_t pressed = (buttonsStatus & buttonsMask & BUTTON_LEFT_MASK) ? 1 : 0;
      /* Left button activity (pressed or released) */
      buttonReport |= (pressed << BUTTON_LEFT_BIT);
    }
    else
    {
      buttonReport |= lastButtonReport & (1 << BUTTON_LEFT_BIT);
    }

    if (buttonsMask & BUTTON_RIGHT_MASK)
    {
      uint8_t pressed = (buttonsStatus & buttonsMask & BUTTON_RIGHT_MASK) ? 1 : 0;
      /* Right button activity (pressed or released) */
      buttonReport |= (pressed << BUTTON_RIGHT_BIT);
    }
    else
    {
      buttonReport |= lastButtonReport & (1 << BUTTON_RIGHT_BIT);
    }

    buttonTimeout = 10;
    buffer[0] = 0xFC;
    buffer[1] = 0x00;
    buffer[2] = 0x06;
    buffer[3] = 0x1F;
    buffer[4] = 0;
    buffer[5] = buttonReport;
    buffer[6] = 0;
    buffer[7] = 0;
    buffer[8] = 0x00;
    buffer[9] = 0xCC;
    sendData(10, buffer, TYPE_SERIAL);
    lastButtonReport = buttonReport;
  }

  thisMillisecondTick = halCommonGetInt32uMillisecondTick();
  if (((thisMillisecondTick - lastMillisecondTick) >= 9) && (LIS302DL_GetStatus() & STATUS_REG_ZYXDA))
  {
    int8_t MOUSE_X, MOUSE_Y;
    LIS302DL_DataTypeDef axyz;

    if (buttonTimeout > 0)
    {
      buttonTimeout--;
      return;
    }

    LIS302DL_ReadACC(&axyz);

    lastMillisecondTick = halCommonGetInt32uMillisecondTick();

    getPitchAndRoll ((int8_t) axyz.outx_l,
                     (int8_t) axyz.outy_l,
                     (int8_t) axyz.outz_l,
                     &Pitch_Angle,
                     &Roll_Angle);

    if (abs(Pitch_Angle) < STEADY_THRESHOLD)
    {
      Pitch_Angle = 0;
    }
    else
    {
      Pitch_Angle += (Pitch_Angle < 0 ? 1: -1) * STEADY_THRESHOLD;
    }

    if (abs(Roll_Angle) < STEADY_THRESHOLD)
    {
      Roll_Angle = 0;
    }
    else
    {
      Roll_Angle += (Roll_Angle < 0 ? 1: -1) * STEADY_THRESHOLD;
    }

    MOUSE_X = (int8_t) (Pitch_Angle * gain);
    MOUSE_Y = (int8_t) (Roll_Angle * gain);

    buffer[0] = 0xFC;
    buffer[1] = 0x00;
    buffer[2] = 0x06;
    buffer[3] = 0x1F;
    buffer[4] = 0;
    buffer[5] = lastButtonReport;
    buffer[6] = MOUSE_X;
    buffer[7] = MOUSE_Y;
    buffer[8] = 0;
    buffer[9] = 0xCC;

    /* Keep mouse movement */
    if (!mouseSteady(MOUSE_X, MOUSE_Y))
    {
      lastMouseMovemement = halCommonGetInt32uMillisecondTick();
    }

    if (((MOUSE_X != 0) || (MOUSE_Y != 0)) && ((halCommonGetInt32uMillisecondTick() - lastMouseMovemement) < NO_MOVEMENT_TIMEOUT)) 
    {
      sendData(10, buffer, TYPE_SERIAL);
    }
  }
}
#endif /* MOUSE_TX */

/**
  * @brief  MEMS accelerometre management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LIS302DL_TIMEOUT_UserCallback(void)
{
  /* MEMS Accelerometer Timeout error occured */
  while (1)
  {   
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
