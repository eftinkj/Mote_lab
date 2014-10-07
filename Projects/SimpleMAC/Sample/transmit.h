/**
  ******************************************************************************
  * @file    transmit.h 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   transmit header file
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
  * See the License for the specific language golverning permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
  /* This is a debugging macro for printing transmit details.
   *  WARNING: THIS MACRO WILL CAUSE PRINTS FROM ISR CONTEXT!*/                                                          
#define TX_DETAILS(x)           

  /* Define the size of the direct transmit packet queue.*/
#define TX_DIRECT_QUEUE_SIZE   (5)

  /*The total number of transmit attempts when a MAC ACK should
   * have been received or when CCA fails.  This count includes the very
   * first transmission.*/
#define TX_ATTEMPTS_COUNT      (4)

/* Private variables ---------------------------------------------------------*/

  /*Indicates if a packet is currently in flight.*/
extern boolean txPacketInFlight;



  /* @brief Indicates that the frame pending bit was set and there is a
   * new packet enroute to this node..*/
extern boolean framePendingPktEnroute;

  /* @brief The system time, in milliseconds, when the set frame pending
   * bit was seen by ST_RadioTransmitCompleteIsrCallback.*/
extern uint16_t framePendingMsTick;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  The tick function for driving the transmit queues.  This
  *         function should be called as often as possible.
  * @param  None
  * @retval None
  */
void txTick(void);

/**
  * @brief  Copy a packet onto the transmit queue.
  * @param  direct: if TRUE the packet is placed on the direct queue.  If
  *         FALSE the packet is placed on the indirect queue.
  * @param  dstShortAddr: The short address of the destination.  If long
  *         addressing is used, this parameter is ignored.
  * @param  packet:A pointer to the packet being sent.  The packet is
  *         copied into the transmit queue's local packet buffer.
  * @param  sfdPtrOffset:time should be injected.  If this parameter is 0, the SFD time is
  *         not injected into the packet. 
  * @retval None
  */
void enqueueTxPacket(boolean direct, uint16_t dstShortAddr, uint8_t * packet, uint8_t sfdPtrOffset);


/**
  * @brief  Tells the indirect queue to mark the next packet as active and
  *         to send it now.
  * @param  None
  * @retval None
  */
void transmitIndirectPacketNow(void);

/**
  * @brief  Looks into the indirect queue and determines if there is data
  *         pending for the given short address.
  * @param  ShortId : the short address of the node polling for data
  * @retval TRUE if there is a data pending, FALSE if not
  */
boolean isDataPendingForShortId(uint16_t shortId);

/**
  * @brief  Clears the direct queue of all packets/data.
  * @param  None
  * @retval None
  */
void clearDirectQueue(void);

/**
  * @brief  IClears the indirect queue of all packets/data
  * @param  None
  * @retval None
  */

void clearIndirectQueue(void);

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
