/**
  ******************************************************************************
  * @file    talk.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   This is an example of RF application which shows point-to-point 802.15.4
  *       wireless communication using the STM32W108 microcontroller.
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

/** @addtogroup talk_demos
 * Point-to-point demo \see talk.c for documentation.
 *@{
 */

/* Includes ------------------------------------------------------------------*/
#include PLATFORM_HEADER
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "hal.h"
#include "phy-library.h"
#include "hal_uart.h"
#include <stdint.h>

/* Private define ------------------------------------------------------------*/
#define TIMEOUT_VALUE 100
#define MAX_RETRIES 3
#define BUFFER_SIZE 64
#define TYPE_SERIAL 0
#define TYPE_BUTTON 1
/* Define the number of quarter seconds between periodic maintenance events.
 * The periodic events do not need to occur very often, so run these
 * events once a minute. */
#define PERIODIC_MAINTENANCE_EVENTS_RATE (1*60*4)

#define BUTTON_CLICKED 1
#define BUTTON_IDLE    0

#define BUTTON_ACTION_1 0
#define BUTTON_ACTION_2 1
#define BUTTON_ACTION_3 2
#define BUTTON_ACTION_4 3
#define BUTTON_ACTION_5 4

#define PTP_PAN_ID                   0x3332
//0x1406
#define PTP_SHORT_ID                 0x0000
//0x1406

#define INACTIVE_TIMEOUT (10 * 4) // 10 seconds
/* Private variables ---------------------------------------------------------*/

/* radioTransmitConfig variables required from the SimpleMac library for packet 
   transmission */
RadioTransmitConfig radioTransmitConfig = RADIO_TRANSMIT_CONFIG_DEFAULTS;

/* generic data packet */
ALIGN_VAR(uint8_t txPacket[128], 2);
uint8_t txPacket[128] = { 
  0x0a, /* length */
  0x61, /* fcf - intra pan, ack request, data */
  0x08, /* fcf - src, dst mode */
  0x00, /* seq */
  BYTE_0(PTP_PAN_ID), /* dst pan l */
  BYTE_1(PTP_PAN_ID), /* dst pan h */
  BYTE_0(PTP_SHORT_ID), /* dst addr */
  BYTE_1(PTP_SHORT_ID), /* dst addr */
  0x00  /* data */
};

/* buffer for received packet */
uint8_t rxPacket[128];
/* received packet flag */
__IO boolean packetReceived = FALSE;
/* Retries counter */
uint8_t retries = MAX_RETRIES;

/* packet transmission complete flag */
__IO boolean txComplete = TRUE;

/* Private Functions ---------------------------------------------------------*/
#if defined ( __CC_ARM )

/* Add dummy abort function required for MDK-ARM toolchain */
void abort (void) {
}
#endif
/**
  * @brief  This function allows to transmit the data
  * @param  lenght: lenght of the data
  * @param  data: data to be transmitted
  * @param  type: type of data (serial or button)
  * @retval None
  */
void sendData(uint8_t length, uint8_t *data, uint8_t type)
{
  
  while (txComplete == FALSE);
  txPacket[8] = type;
  halCommonMemCopy(txPacket+9, data, length);
  txPacket[0] = length + 2 + 7 + 1;
  txPacket[3]++; /* increment sequence number */
  
  txComplete = FALSE;
  ST_RadioTransmit(txPacket);
}

/**
  * @brief  This function return whether a button has been pressed and released
  * @param  button
  * @retval BUTTON_clicked or BUTTON_IDLE
  */
uint8_t getButtonStatus(Button_TypeDef button)
{
  if (STM_EVAL_PBGetState(button) == 0x00)
  {
    /*  Indicate button pression detected */
    STM_EVAL_LEDOn(LED1);
    /* Wait for release */
    while (STM_EVAL_PBGetState(button) == 0x00);
    halCommonDelayMilliseconds(50);
    while (STM_EVAL_PBGetState(button) == 0x00);
    return BUTTON_CLICKED;
  }
  else
  {
    return BUTTON_IDLE;
  }
}

/**
  * @brief  Send packet to remote node and shortly flashed LED1
  * @param  Button
  * @retval None
  */
void buttonAction(uint8_t buttonAction)
{
  sendData(1, &buttonAction, TYPE_BUTTON);
}

/**
  * @brief  Perform some led toggling according to the button received from remote node
  * @param  button
  * @retval None
  */
void ledAction(uint8_t buttonAction)
{
  uint32_t i;
  
  if (buttonAction == BUTTON_ACTION_1)
  {
    STM_EVAL_LEDToggle(LED1);
  }
  else if (buttonAction == BUTTON_ACTION_2)
  {
    STM_EVAL_LEDToggle(LED3);
  }
  else if (buttonAction == BUTTON_ACTION_3)
  {
    STM_EVAL_LEDToggle(LED1);
    STM_EVAL_LEDToggle(LED3);
  }
  else if (buttonAction == BUTTON_ACTION_4)
  {
    STM_EVAL_LEDOff(LED1);
    for (i = 0; i < 5; i++)
    {
      STM_EVAL_LEDToggle(LED1);
      halCommonDelayMilliseconds(200);
    }
  } 
  else if (buttonAction == BUTTON_ACTION_5) 
  {
    STM_EVAL_LEDOff(LED3);
    for (i = 0; i < 5; i++)
    {
      STM_EVAL_LEDToggle(LED3);
      halCommonDelayMilliseconds(200);
    }
  }
}


int mz_t_ = 0;

/**
  * @brief  Processes serial input and button preses
  * @param  None
  * @retval None
  */
void processInput(void)
{
  static uint8_t buffer[BUFFER_SIZE];
  static uint8_t bufferSize = 0;
  static uint32_t bufferTimeout = TIMEOUT_VALUE;

  if (bufferSize ==0)
  {
    bufferTimeout = TIMEOUT_VALUE;
  }
  
  if (__io_getcharNonBlocking(buffer+bufferSize))
  {
    bufferSize+=1;
  }
  
  if (getButtonStatus(BUTTON_S1) == BUTTON_CLICKED)
  {
    buttonAction(BUTTON_ACTION_1);
  }
#ifdef USE_MB950
  else if (getButtonStatus(BUTTON_S2) == BUTTON_CLICKED)
  {
    buttonAction(BUTTON_ACTION_2);
  }
  else if (getButtonStatus(BUTTON_S3) == BUTTON_CLICKED)
  {
    buttonAction(BUTTON_ACTION_3);
  } 
  else if (getButtonStatus(BUTTON_S4) == BUTTON_CLICKED)
  {
    buttonAction(BUTTON_ACTION_4);
  } 
  else if (getButtonStatus(BUTTON_S5) == BUTTON_CLICKED)
  {
    buttonAction(BUTTON_ACTION_5);
  }
#endif /* USE_MB950 */  
  else
  {
    bufferTimeout--;
    if ((bufferTimeout == 0) || (bufferSize == BUFFER_SIZE))
    {
      if (bufferSize > 0)
      {
        sendData(bufferSize, buffer, TYPE_SERIAL);
		/*if (0==mz_t_)
		{
			mz_t_=1;
			printf("MZ Start\n");
			//ST_RadioStartTransmitTone();
			ST_RadioStartTransmitStream();
		} else {
			mz_t_=0;
			printf("MZ Stop\n");
			//ST_RadioStopTransmitTone();
			ST_RadioStopTransmitStream();
		}*/
        bufferSize = 0;
      }
    }
  }
}

/**
  * @brief  Processes packet received in RX
  * @param  None
  * @retval None
  */
void processReceivedPackets(void)
{
    uint8_t i;
    /* print out any packets that were received */
    if(packetReceived == TRUE) 
    {
      if (rxPacket[8] == TYPE_SERIAL) 
      {
        for (i = 9; i <= rxPacket[0]; i++)
        {
          putchar(rxPacket[i]);
        }
      }
      if (rxPacket[8] == TYPE_BUTTON)
      {
        ledAction(rxPacket[9]);
      }
      /* The packet has been processed, so free the single entry queue up */
      packetReceived = FALSE;
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
  * @brief  check if the device needs to go to deep sleep to save power
  * @param  active: Flag to indicate whether the node is active or not
  * @param  batteryOperated: Flag to activate power management
  * @retval None
  */
void powerManagement(boolean active, boolean batteryOperated)
{
  static uint32_t inactiveStartTime = 0;
  
  if (!batteryOperated)
    return;
  
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
             halPowerDown();
             halSleepWithOptions(SLEEPMODE_NOTIMER,
                                 BUTTON_S1_WAKE_SOURCE|
#ifdef USE_MB950
                                 BUTTON_S2_WAKE_SOURCE|
                                 BUTTON_S3_WAKE_SOURCE|
                                 BUTTON_S4_WAKE_SOURCE|
                                 BUTTON_S5_WAKE_SOURCE|
#endif /* USE_MB950 */
                                 UART_RX_WAKE_SOURCE);
             halPowerUp();
             )
      /* It is the application's responsibility to reinitialize the UART */
      uartInit();
    
      status = ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
      assert(status==ST_SUCCESS);  
      inactiveStartTime = halCommonGetInt16uQuarterSecondTick();
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
  uint16_t threshold = 3200;

  /* Optional printf */
  printf ("Measured Vdd (mV) = %d\r\n", halMeasureVdd(ADC_SAMPLE_CLOCKS_512));
  
#ifdef USE_MB950
  /* MB950 has a BAT46 diode to avoid conflict between battery and USB power, so threshold is lowered by 400 mV */
  threshold -= 400;
#endif /* USE_MB950 */
  
  return halMeasureVdd(ADC_SAMPLE_CLOCKS_512) > threshold ? FALSE : TRUE;
}

/**
  * @brief  Radio Receiver callback function
  * @param  packet: lreceived packet
  * @param  ackFramePendingSet: frame pending bit in the received packet
  * @param  MAC timer when the SFD was received
  * @param  errors: numbers of correlator erros in the packet
  * @param  rssi: energy detected in the packet
  * @retval None
  */
void ST_RadioReceiveIsrCallback(uint8_t *packet, boolean ackFramePendingSet, uint32_t time, uint16_t errors, int8_t rssi)
{
  /* note this is executed from interupt context */
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
  * @brief  Radio Transmit callback function
  * @param  status: status of the packet transmission
  * @param  sfdSentTime: MAC timer when the SFD was sent
  * @param  framePending: TRUE if the received ACK indicates that data
  *         is pending for this node
  * @retval None
  */
void ST_RadioTransmitCompleteIsrCallback(StStatus status, uint32_t sfdSentTime, boolean framePending)
{
  switch(status)
  {
  case ST_SUCCESS:
    /* This should not happen */
    break;
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
      uint32_t i;
      STM_EVAL_LEDOn(LED1);
      STM_EVAL_LEDOn(LED3);
      for (i = 0; i < 10; i++)
      {
        STM_EVAL_LEDToggle(LED1);
        STM_EVAL_LEDToggle(LED3);
        halCommonDelayMilliseconds(100);
      }
      STM_EVAL_LEDOff(LED1);
      STM_EVAL_LEDOff(LED3);
    }
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
  * @retval boolean value
  */
boolean ST_RadioDataPendingShortIdIsrCallback(uint16_t shortId)
{
  return FALSE;
}

/**
  * @brief  Callback for Radio Long  Id data pending 
  * @param  longId: long id address
  * @retval boolean value
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
  * @param  sfdSentTime:MAC timer when the SFD was sent 
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
  * @brief  Main program
  * @param  None
  * @retval int value
  */
int main(void)
{
  /*!< Application main:  talk main routine */
  uint32_t seed;
  StStatus status = ST_SUCCESS;
  boolean batteryOperated = FALSE;

  /* UART Initialization */
  halInit();

  ST_RadioGetRandomNumbers((uint16_t *)&seed, 2);
  halCommonSeedRandom(seed);
  
  uartInit();
    
  INTERRUPTS_ON();
  
  batteryOperated = powerFromBatteries();
  
  /* Initialize radio (analog section, digital baseband and MAC).
   * Leave radio powered down */
  status = ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
  assert(status==ST_SUCCESS); 

  /* Set channel */
//  ST_RadioSetChannel(ST_MIN_802_15_4_CHANNEL_NUMBER);
  ST_RadioSetChannel(0x14);

  /* Setup some node and pan ids.  The packet above is also sent to a device
  * with the same node and pan id so that two nodes running this same image
  * will talk to each other, even though its not right to have two nodes
  * with the same node id */
  ST_RadioSetNodeId(PTP_SHORT_ID);
  ST_RadioSetPanId(PTP_PAN_ID);

  ST_RadioSetPower(16);
  
  printf("\r\nSimpleMAC (%s) Talk Application\r\n",SIMPLEMAC_VERSION_STRING);
ST_RadioWake();

  while(1)
  {
    processInput();
    
    powerManagement(!txComplete || packetReceived, batteryOperated);
    
    processReceivedPackets();
    
    periodicMaintenanceEvents();
    
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
