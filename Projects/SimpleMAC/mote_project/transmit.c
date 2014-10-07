/**
  ******************************************************************************
  * @file    transmit.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   Main program body
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "hal.h"
#include "phy-library.h"
#include <stdint.h>
#include "transmit.h"

/* NOTE: The packet itself (and therefore TransmitCompleteIsrCallback) knows
      if the packet needs an ACK and if the packet should be retransmitted.*/
/* -inUse says a tx slot has a valid packet and the slot cannot be reused
   -sendNow tells the tick to send the packet now
   -dsShortAddr is the short address the packet is being sent to
   -packet is the actual data buffer being transmitted
   -sfdPtr says where SfdSentIsrCallback should write the SFD*/

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  boolean inUse;
  boolean sendNow;
  uint16_t dstShortAddr;
  uint8_t packet[128];
  uint8_t * sfdPtr;
} TxQueueType;

/* Private variables ---------------------------------------------------------*/
uint8_t txDirectQHead=0;
uint8_t txDirectQTail=0;
TxQueueType txDirectQ[TX_DIRECT_QUEUE_SIZE]={{0,},};
/* The indirect transmit queue is exactly size 1.  This application can
 * only hold a single indirect packet regardless of the number of planets. */
TxQueueType txIndirectQ={0,};

/* These variables are for the packet currently being transmitted. */
boolean * txPacketInUse; //points to the relevant variable in the Q
uint8_t   * txPacket;      //points to the relevant variable in the Q
uint8_t   * txPacketSfd;   //points to the relevant variable in the Q
uint8_t     txPacketAttemptsLeft=TX_ATTEMPTS_COUNT;
boolean   txPacketInFlight=FALSE;
boolean   txPacketIsDirect=TRUE;
uint32_t    txPacketDroppedNotifcation=0;
boolean   framePendingPktEnroute=FALSE;
uint16_t    framePendingMsTick;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Clear the direct transmission queue
  * @param  None
  * @retval None
  */
void clearDirectQueue(void)
{
  uint8_t i;
  /* there is a current transmission*/
  if(txPacketInFlight && txPacketIsDirect)
  {
    printf("Direct transmission in progess.  txDirectQ NOT cleared.\r\n");
    return;
  }

  txPacketAttemptsLeft=0;
  for(i=0;i<TX_DIRECT_QUEUE_SIZE;i++)
  {
    txDirectQ[i].inUse=FALSE;
  }
  /* Initialize the value of the head and the tail of the paquet to 0 */
  txDirectQHead = 0;
  txDirectQTail = 0;
}

/**
  * @brief  Clear the indirect transmission queue
  * @param  None
  * @retval None
  */
void clearIndirectQueue(void)
{
  if(txPacketInFlight && !txPacketIsDirect)
  {
    printf("Indirect transmission in progess.  txIndirectQ NOT cleared.\r\n");
    return;
  }

  txPacketAttemptsLeft = 0;
  txIndirectQ.inUse=FALSE;
}

/**
  * @brief  Confirm the end of the transmission 
  * @param  None
  * @retval None
  */
void packetIsDone(void)
{
  *txPacketInUse=FALSE;
  txPacketInFlight=FALSE;
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void advanceHead(void)
{
  txDirectQHead++;
  if(txDirectQHead >= TX_DIRECT_QUEUE_SIZE)
  {
    txDirectQHead = 0;
  }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void advanceTail(void)
{
  if(txPacketIsDirect)
  {
    txDirectQTail++;
    if(txDirectQTail>=TX_DIRECT_QUEUE_SIZE)
    {
      txDirectQTail=0;
    }
  }
}

/**
  * @brief  Send paquet as an indirect transmission 
  * @param  None
  * @retval None
  */
void indirectTxTick(void)
{
  StStatus status = ST_SUCCESS;
  
  /* Send next available packet if there already isn't a packet in flight. */
  if(txIndirectQ.inUse && txIndirectQ.sendNow && txPacketInFlight == FALSE)
  {
    /* Linkup our current transmission to the Q. */
    txPacketInUse = &txIndirectQ.inUse;
    txPacket = txIndirectQ.packet;
    txPacketSfd = txIndirectQ.sfdPtr;
    txPacketIsDirect = FALSE;
    txPacketInFlight = TRUE;
    txPacketAttemptsLeft = TX_ATTEMPTS_COUNT;
    /* Now send the packet!*/
    if(txPacketSfd != NULL)
    {
      /* Since we're going to inject the TX SFD into the packet, we have to
      enable the SfdSentIsrCallback.  The SfdSentIsrCallback will then
      disable itself when it is done injecting the SFD.*/
      ST_RadioEnableSfdSentNotification(TRUE);
    }
    status = ST_RadioTransmit(txPacket);
    assert(status==ST_SUCCESS);
    txPacketAttemptsLeft--;
  }
}

/**
  * @brief  Send paquet as a direct transmission
  * @param  None
  * @retval None
  */
void directTxTick(void)
{
  StStatus status = ST_SUCCESS;
  /* Send next available packet if there already isn't a packet in flight.*/
  if(txDirectQ[txDirectQTail].inUse == TRUE && txPacketInFlight == FALSE)
  {
    /* Linkup our current transmission to the Q.*/
    txPacketInUse = &txDirectQ[txDirectQTail].inUse;
    txPacket = txDirectQ[txDirectQTail].packet;
    txPacketSfd = txDirectQ[txDirectQTail].sfdPtr;
    txPacketIsDirect = TRUE;
    txPacketInFlight = TRUE;
    txPacketAttemptsLeft = TX_ATTEMPTS_COUNT;
    /* Now send the packet!*/
    if(txPacketSfd != NULL)
    {
      /* Since we're going to inject the TX SFD into the packet, we have to
      enable the SfdSentIsrCallback.  The SfdSentIsrCallback will then
      disable itself when it is done injecting the SFD. */
      ST_RadioEnableSfdSentNotification(TRUE);
    }
    status = ST_RadioTransmit(txPacket);
    assert(status==ST_SUCCESS);
    txPacketAttemptsLeft--;
  }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void txTick(void)
{
  indirectTxTick();
  directTxTick();
  if(txPacketDroppedNotifcation)
  {
    printf("Failed all transmit attempts.  Dropping the packet.\r\n");
    txPacketDroppedNotifcation--;
  }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void transmitIndirectPacketNow(void)
{
  txIndirectQ.sendNow=TRUE;
  indirectTxTick();
}

/**
  * @brief  
  * @param  dstShortAddr
  * @param  packet
  * @param  sfdPtrOffset
  * @retval None
  */
void directTransmit(uint16_t dstShortAddr, uint8_t * packet, uint8_t sfdPtrOffset)
{
  TxQueueType * headPkt;
  
  headPkt = &txDirectQ[txDirectQHead];
  if(headPkt->inUse)
  {
    printf("txDirectQ is full!  Dropping new TX packet.\r\n");
    return;
  }

  /*Place new packet into the Q*/
  headPkt->sendNow=TRUE;
  headPkt->dstShortAddr=dstShortAddr;
  memcpy(headPkt->packet, packet, packet[0]+1);
  if(sfdPtrOffset > 0)
  {
    headPkt->sfdPtr=headPkt->packet+sfdPtrOffset;
  }
  else 
  {
    headPkt->sfdPtr=NULL;
  }

  /*Mark the new packet ready to be sent.*/
  headPkt->inUse=TRUE;
  
  /*Make room for another packet in the Q.*/
  advanceHead();
}

/**
  * @brief  
  * @param  dstShortAddr
  * @param  packet
  * @param  sfdPtrOffset
  * @retval None
  */
void indirectTransmit(uint16_t dstShortAddr, uint8_t * packet, uint8_t sfdPtrOffset)
{
  if(txIndirectQ.inUse) 
  {
    printf("txIndirectQ is full!  Dropping new TX packet.\r\n");
    return;
  }

  /*Place new packet into the Q*/
  txIndirectQ.sendNow=FALSE;
  txIndirectQ.dstShortAddr=dstShortAddr;
  memcpy(txIndirectQ.packet, packet, packet[0]+1);
  if(sfdPtrOffset > 0) 
  {
    txIndirectQ.sfdPtr=txIndirectQ.packet+sfdPtrOffset;
  }
  else
  {
    txIndirectQ.sfdPtr=NULL;
  }

  /*Mark the new packet ready to be sent.*/
  txIndirectQ.inUse=TRUE;
}

/**
  * @brief  
  * @param  direct
  * @param  dstShortAddr
  * @param  packet
  * @param  sfdPtrOffset
  * @retval None
  */
void enqueueTxPacket(boolean direct, uint16_t dstShortAddr, uint8_t * packet, uint8_t sfdPtrOffset)
{
  if(direct)
  {
    directTransmit(dstShortAddr, packet, sfdPtrOffset);
  }
  else
  {
    indirectTransmit(dstShortAddr, packet, sfdPtrOffset);
  }

  txTick();
}

/**
  * @brief  
  * @param  status
  * @param  sfdSentTime
  * @param  framepending
  * @retval None
  */
void ST_RadioTransmitCompleteIsrCallback(StStatus status, uint32_t sfdSentTime, boolean framePending)
{
  StStatus status_code = ST_SUCCESS;
  TX_DETAILS(printf("ST_RadioTransmitCompleteIsrCallback\r\n");)
  TX_DETAILS(printf("sfdSentTime = 0x%05X\r\n", sfdSentTime);)
  TX_DETAILS(printf("framePending = %d\r\n", framePending);)
  TX_DETAILS(printf("status = 0x%02X - ", status);)

    switch(status){
    case ST_SUCCESS:
      TX_DETAILS(printf("ST_SUCCESS\r\n");)
        /* The inFlight packet required no ACK and was successfully sent,
        therefore I am done with this packet.*/
        packetIsDone();
      advanceTail();
      break;
    case ST_PHY_ACK_RECEIVED:
      TX_DETAILS(printf("ST_PHY_ACK_RECEIVED\r\n");)
        /* The inFlight packet required an ACK and is done*/
        packetIsDone();
      advanceTail();
      /* Save the framePending bit and calculate the framePending timeout
      so that sleepy devices stay awake to accept incoming packets.*/
      if(framePending)
      {
        framePendingPktEnroute = TRUE;
        framePendingMsTick = halCommonGetInt16uMillisecondTick();
      }
      break;
    case ST_MAC_NO_ACK_RECEIVED:
      TX_DETAILS(printf("ST_MAC_NO_ACK_RECEIVED\r\n");)
        /* The inFlight packet required and ACK, which we didn't get, so
        it must be retransmitted*/
        if(txPacketAttemptsLeft>0)
        {
          if(txPacketSfd != NULL)
          {
            ST_RadioEnableSfdSentNotification(TRUE);
          }
          status_code = ST_RadioTransmit(txPacket);
          assert(status_code==ST_SUCCESS);
          txPacketAttemptsLeft--;
        }
        else
        {
          txPacketDroppedNotifcation++;
          /* Failed all transmit attempts.  Dropping the packet.*/
          packetIsDone();
          advanceTail();
        }
      break;
    case ST_PHY_TX_CCA_FAIL:
      TX_DETAILS(printf("ST_PHY_TX_CCA_FAIL\r\n");)
        /* The inFlight packet never got sent and must be retransmitted*/
        if(txPacketAttemptsLeft>0) 
        {
          if(txPacketSfd != NULL)
          {
            ST_RadioEnableSfdSentNotification(TRUE);
          }
          status_code = ST_RadioTransmit(txPacket);
          assert(status_code==ST_SUCCESS);
          txPacketAttemptsLeft--;
        }
        else
        {
          txPacketDroppedNotifcation++;
          /* Failed all transmit attempts.  Dropping the packet.*/
          packetIsDone();
          advanceTail();
        }
      break;
    case ST_PHY_TX_UNDERFLOW:
      assert(FALSE);
      break;
    case ST_PHY_TX_INCOMPLETE:
      assert(FALSE);
      break;
    default:
      assert(FALSE);
    };
}

/**
  * @brief  
  * @param  sfdSentTime
  * @retval None
  */
void ST_RadioSfdSentIsrCallback(uint32_t sfdSentTime)
{
  /* Inject the SFD time into the packet being transmitted.*/
  txPacketSfd[0] = (sfdSentTime>> 0)&0xFF;
  txPacketSfd[1] = (sfdSentTime>> 8)&0xFF;
  txPacketSfd[2] = (sfdSentTime>>16)&0xFF;
  /* Mark the SFD time as valid.  This is important to do because if
  this ISR doesn't operate fast enough the packet might be gone before
  the time is fully injected.*/
  txPacketSfd[2] |= 0x80;

  /* To prevent this IsrCallback from being triggered more times than desired,
  it is only enabled immediately before specific transmissions and then
  immediately disabled here.*/
  ST_RadioEnableSfdSentNotification(FALSE);
}

/**
  * @brief  
  * @param  shortId
  * @retval boolean
  */
boolean isDataPendingForShortId(uint16_t shortId)
{
  return ((shortId==txIndirectQ.dstShortAddr)&&txIndirectQ.inUse) ? TRUE:FALSE;
}

/**
  * @brief  
  * @param  shortId
  * @retval boolean
  */
boolean ST_RadioDataPendingShortIdIsrCallback(uint16_t shortId)
{
  return isDataPendingForShortId(shortId);
}

/**
  * @brief  
  * @param  longId
  * @retval boolean
  * @note   This application does not use long IDs with indirect transmissions.
  */
boolean ST_RadioDataPendingLongIdIsrCallback(uint8_t* longId)
{
  return FALSE;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
