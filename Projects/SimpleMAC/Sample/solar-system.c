/**
  ******************************************************************************
  * @file    solar-system.c 
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   Solar system program body
  * It supports sun and planet roles. 
  * Sun role allows user to form a network. 
  * Planet role allows an user to join to a network created by a sun.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "hal.h"
#include "phy-library.h"
#include "iap_bootloader.h"
#include <stdint.h>
#include "transmit.h"

/** @addtogroup sample_demos
 * Star topology demo \see solar-system.c for documentation.
 *@{
 */

/* Private define ------------------------------------------------------------*/
/* RX_DETAILS is a debugging macro for printing receive details.
 * WARNING:  THIS MACRO WILL CAUSE PRINTS FROM ISR CONTEXT! */
#define RX_DETAILS(x)
/* This macro causes the main loop to print details of the periodic events
 * and sleep events. */
#define MAIN_DETAILS(x) x

/* This mask is passed into halSleepForQsWithOptions, indicating that
 * the chip should wake on UART receive activity (GPIO PB2). */
#define UART_ACTIVE_MASK (0x00000400)

/* Define the number of quarter seconds between periodic maintenance events.
 * The periodic events do not need to occur very often, so run these
 * events once a minute. */
#define PERIODIC_MAINTENANCE_EVENTS_RATE (1*60*4)

/* Define the number of entries in the planet table. */
#define PLANET_TABLE_SIZE (5)

/* Define the number of energy readings that shold be taken on a given
 * channel when forming a network and looking for a channel with the
 * least ammount of activity. */
#define ENERGY_SAMPLES_COUNT (32)

RadioTransmitConfig radioTransmitConfig = RADIO_TRANSMIT_CONFIG_DEFAULTS;

/* Define values used in constructing and decoding packets */

/* These define the magic values needed when constructing the FCF of the
 * packets.  802.15.4 defines these numbers. */
#define FCF_DATA     (0x01)
#define FCF_MACCMD   (0x03)
#define FCF_ACKREQ   (0x20)
#define FCF_INTRAPAN (0x40)
#define FCF_NODST    (0x00)
#define FCF_SHORTDST (0x08)
#define FCF_LONGDST  (0x0C)
#define FCF_NOSRC    (0x00)
#define FCF_SHORTSRC (0x80)
#define FCF_LONGSRC  (0xC0)

/* This app uses only two frame types: MAC Command and Data. */
#define FT_DATA          (FCF_DATA)
#define FT_MAC_COMMAND   (FCF_MACCMD)

/* The following seven payload types are used with FT_DATA and the numerical
 * values are custom to this app. */
#define PT_GENERIC_DATA  (0x00)
#define PT_SUN_SEARCH    (0x01)
#define PT_SUN_AVAILABLE (0x02)
#define PT_JOIN_REQUEST  (0x03)
#define PT_JOIN_ACCEPTED (0x04)
#define PT_JOIN_DENIED   (0x05)
#define PT_LEAVING       (0x06)
/* This payload type is used with FT_MAC_COMMAND and the numerical value
 * is defined by 802.15.4. */
#define PT_POLLING       (0x04)

/* The packet type is identified by its frame type and its payload type.
 * These are stored in different parts of the packet.  Since payload type
 * requires a specific frame type, I roll these two values into definitions
 * that are easier to read when switching on a decoded packet.  Frame type
 * occupies the upper nibble and payload type occupies the lower. */
#define GENERIC_DATA_PACKET  ((FT_DATA       <<4) | (PT_GENERIC_DATA <<0))
#define SUN_SEARCH_PACKET    ((FT_DATA       <<4) | (PT_SUN_SEARCH   <<0))
#define SUN_AVAILABLE_PACKET ((FT_DATA       <<4) | (PT_SUN_AVAILABLE<<0))
#define JOIN_REQUEST_PACKET  ((FT_DATA       <<4) | (PT_JOIN_REQUEST <<0))
#define JOIN_ACCEPTED_PACKET ((FT_DATA       <<4) | (PT_JOIN_ACCEPTED<<0))
#define JOIN_DENIED_PACKET   ((FT_DATA       <<4) | (PT_JOIN_DENIED  <<0))
#define LEAVING_PACKET       ((FT_DATA       <<4) | (PT_LEAVING      <<0))
#define POLLING_PACKET       ((FT_MAC_COMMAND<<4) | (PT_POLLING      <<0))

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  boolean packetBeingProcessed;
  uint8_t packet[128];
  boolean ackFramePendingSet;
  uint32_t time;
  uint16_t errors;
  int8_t rssi;
  uint8_t lqi;
} RadioReceiveType;
RadioReceiveType rxData = {FALSE,};

typedef struct {
  boolean active;
  uint16_t shortAddr;
  uint8_t longAddr[8];
} PlanetTableType;

/* Private macros ------------------------------------------------------------*/
#define TURN_RADIO_ON()                             \
  do {                                              \
    ST_RadioWake();                               \
    radioPowerState = ST_RADIO_POWER_MODE_RX_ON; \
  } while(0)
#define TURN_RADIO_OFF()                          \
  do {                                            \
    ST_RadioSleep();                            \
    radioPowerState = ST_RADIO_POWER_MODE_OFF; \
  } while(0)

/* Private variables ---------------------------------------------------------*/
RadioPowerMode radioPowerState = ST_RADIO_POWER_MODE_OFF;
/* Define global network state; initialized when forming/joining/leaving */
boolean activeInNetwork = FALSE;
boolean availableSunFound = FALSE;
boolean networkJoinedStopSearching = FALSE;
uint8_t shortAddrCounter = 0;
uint8_t currSeqNum = 0;
uint8_t autoSendRate = 0;
uint16_t autoSendRateDstShortAddr = 0x0000;
uint16_t autoSendLastQsTick;
uint8_t autoPollRate = 0;
uint16_t autoPollLastQsTick;
PlanetTableType planetTable[PLANET_TABLE_SIZE];

#ifdef PLANET_ROLE
uint8_t planet_join_by_button = 0;
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if defined ( __CC_ARM )

/* Add dummy abort function required for MDK-ARM toolchain */
 void abort (void) {
 }
#endif

/**
  * @brief  This function initialize the network and the table 
  *  of the planets status  
  * @param  None
  * @retval None
  */
void initNetworkState(void)
{
  uint8_t i;
  
  activeInNetwork = FALSE;
  networkJoinedStopSearching = FALSE;
  shortAddrCounter = 0;
  currSeqNum = halCommonGetRandom()&0xFF; /*802.15.4 says to start with a random seq num*/
  autoSendRate = 0;
  autoPollRate = 0;
  txPacketInFlight = FALSE;
  framePendingPktEnroute = FALSE;
  for(i=0;i<PLANET_TABLE_SIZE;i++)
  {
    planetTable[i].active = FALSE;
    planetTable[i].shortAddr = 0x0000;
    memset(planetTable[i].longAddr, 0, sizeof(planetTable[i].longAddr));
  }
  ST_RadioSetPanId(0xFFFF);  /*return to broadcast PAN ID*/
  ST_RadioSetNodeId(0xFFFE); /*return to long addressing node ID*/
  clearDirectQueue();
  clearIndirectQueue();
}


/**
  * @brief  Lqi calculation
  * @param  chipErrors: The chipErrors parameter is the 10 bit chip error 
  *          count from the radio.
  * @param  packetLenght: The packetLength parameter is the byte size 
  *          of the packet, including the length byte at the start 
  *          and the 2 crc bytes at the end.
  * @retval lqi: To obtain the LQI, convert the total chip errors in 
  *          a packet (err/packet)to the chip errors per byte (err/byte).
  *          Scale up the chipErrors value to maintain reasonable precision 
  *          during the division.
  */
uint8_t calculateLqi(uint16_t chipErrors, uint8_t packetLength)
{
  uint32_t lqi;
  
  /* To obtain the LQI, convert the total chip errors in a packet (err/packet)
     to the chip errors per byte (err/byte).  Scale up the chipErrors value
     to maintain reasonable precision during the division.  We don't
     care about packets that have more than 4 chip errors per byte (6.25% chip
     error rate), so we scale up by a factor of 64. */
  lqi = chipErrors << 6;
  lqi /= packetLength;
  
  /* 02.15.4 says that an LQI should represent the best link quality as 0xFF
  * and the worst LQI as 0x00. */
  return (0xFF - (uint8_t)((lqi > 0xFF) ? (0xFF) : (lqi)));
}

/* packet reception processing */

/* ST_RadioReceiveIsrCallback executes in ISR context and copies the
  incoming packet into the rxData structure so it can be processed
  outside of ISR context by processRxPacket. */

/**
  * @brief  ST_RadioReceiveIsrCallback executes in ISR context nd copies the
  *          incoming packet into the rxData structure.
  * @param  packet:
  * @param  ackFramePendingSet:
  * @param  time:
  * @param  errors:
  * @param  rssi:
  * @retval None
  */
void ST_RadioReceiveIsrCallback(uint8_t *packet, boolean ackFramePendingSet, uint32_t time, uint16_t errors, int8_t rssi)
{
  uint8_t i;
  RX_DETAILS(printf("#");)
    if(activeInNetwork)
    {
      /* if there isn't already one being processed, save the
         incoming packet into the app's buffer for processing
         outside of ISR context. */
      if(!rxData.packetBeingProcessed)
      {
        for(i=0;i<=packet[0];i++)
        {
          rxData.packet[i] = packet[i];
        }
        rxData.ackFramePendingSet = ackFramePendingSet;
        rxData.time = time;
        rxData.errors = errors;
        rxData.rssi = rssi;
        rxData.packetBeingProcessed = TRUE;
        /* rocessRxPacket() in the main loop handles the rest */
      }
    }
}

/**
  * @brief  hnadles decoding all received packets and taking action
  *         in response to received packets.
  * @param  None
  * @retval None
  * @note   This finction executes outside of ISR context and operates
  *         on rxData structure. The rsData structure is populated by 
  *         ST_RadioReceiveIsrCallback.
  */
void processRxPacket(void)
{
  uint8_t i;
  boolean pktHasSrcPanId = FALSE;
  uint8_t srcAddrOffset = 0;
  uint16_t srcPanId=0xFFFF;
  uint16_t shortSrcAddr=0;
  uint8_t longSrcAddr[8]={0,};
  uint8_t payloadStart;
  uint8_t packetType;
  uint8_t packet[128];
  
  if(!rxData.packetBeingProcessed)
  {
    return;
  }

  RX_DETAILS(
             printf("\r\n");
             printf("RX:");
             for(i=0;i<=rxData.packet[0];i++)
             {
               printf(" %02X", rxData.packet[i]);
             }
             printf("\r\n");
             printf("ackFramePendingSet = %d\r\n", rxData.ackFramePendingSet);
             printf("time = 0x%05X\r\n", rxData.time);
             printf("errors = %d\r\n", rxData.errors);
             printf("rssi = %d\r\n", rxData.rssi);
             );

  /* At a minimum there must be a dst PAN, a short dst addr, a short src
     addr, and a payload/command type.  That's a minimum of 10 bytes. */
  if(rxData.packet[0]<10) 
  {
    RX_DETAILS(printf("Length byte too short\r\n");)
      goto stopProcessing;
  }

  /* Determine the frame type. */
  switch(rxData.packet[1]&0x7) {
  case FT_DATA:
    RX_DETAILS(printf("FT_DATA\r\n");)
      packetType = (FT_DATA<<4);
    break;
  case FT_MAC_COMMAND:
    RX_DETAILS(printf("FT_MAC_COMMAND\r\n");)
      packetType = (FT_MAC_COMMAND<<4);
    break;
  default:
    RX_DETAILS(printf("Unknown frame type\r\n");)
      goto stopProcessing;
  };
  
  /* Is there a source PAN ID in the packet? */
  if((rxData.packet[1]&0x40)!=0x40)
  {
    pktHasSrcPanId=TRUE;
    srcAddrOffset = 2;
  }

  /* There are 4 possible addressing modes in our application: */
  switch(rxData.packet[2]) {
  case FCF_SHORTDST + FCF_SHORTSRC:
    RX_DETAILS(printf("short dst, short src\r\n");)
      shortSrcAddr = (rxData.packet[8+srcAddrOffset+1]<<8)|
        (rxData.packet[8+srcAddrOffset+0]<<0);
    payloadStart = 10+srcAddrOffset;
    if(pktHasSrcPanId)
    {
      srcPanId = (rxData.packet[9]<<8)|(rxData.packet[8]<<0);
    }
    break;
  case FCF_SHORTDST + FCF_LONGSRC:
    RX_DETAILS(printf("short dst, long src\r\n");)
      memcpy(longSrcAddr, (rxData.packet+8+srcAddrOffset), 8);
    payloadStart = 16+srcAddrOffset;
    if(pktHasSrcPanId)
    {
      srcPanId = (rxData.packet[9]<<8)|(rxData.packet[8]<<0);
    }
    break;
  case FCF_LONGDST + FCF_SHORTSRC:
    RX_DETAILS(printf("long dst, short src\r\n");)
      shortSrcAddr = (rxData.packet[14+srcAddrOffset+1]<<8)|
        (rxData.packet[14+srcAddrOffset+0]<<0);
    payloadStart = 16+srcAddrOffset;
    if(pktHasSrcPanId)
    {
      srcPanId = (rxData.packet[15]<<8)|(rxData.packet[14]<<0);
    }
    break;
  case FCF_LONGDST + FCF_LONGSRC:
    RX_DETAILS(printf("long dst, long src\r\n");)
      memcpy(longSrcAddr, (rxData.packet+14+srcAddrOffset), 8);
    payloadStart = 22+srcAddrOffset;
    if(pktHasSrcPanId)
    {
      srcPanId = (rxData.packet[15]<<8)|(rxData.packet[14]<<0);
    }
    break;
  default:
    RX_DETAILS(printf("Unknown addressing mode\r\n");)
      goto stopProcessing;
  }

  RX_DETAILS(
             if(pktHasSrcPanId)
             {
               printf("src pan = 0x%04X\r\n", srcPanId);
             }
             )
    
    /* Make sure the packet is long enough to obtain a payload type */
    if(rxData.packet[0]<payloadStart) 
    {
      RX_DETAILS(printf("Length byte too short\r\n");)
        goto stopProcessing;
    }

  /* Finish deriving the packet type by obtaining the payload type. */
  packetType |= (rxData.packet[payloadStart]<<0);
  RX_DETAILS(printf("packet type = 0x%02X\r\n", packetType);)

    switch(packetType) {
    case (GENERIC_DATA_PACKET): /* executes on SUN and PLANET */
      RX_DETAILS(printf("GENERIC_DATA_PACKET\r\n");)
        /* The LQI calculation wants the entire packet length.  The length
           byte in the received packet does not include the length byte
           itself or the two CRC bytes. */
        rxData.lqi = calculateLqi(rxData.errors, (rxData.packet[0]+3));
      printf("RX: Addr=0x%04X, VDD=%dmV, RxSFD=0x%05lX, ",
             shortSrcAddr,
             ((rxData.packet[payloadStart+1]<<0)|
              (rxData.packet[payloadStart+2]<<8)),
             rxData.time);
      if(rxData.packet[payloadStart+5]&0x80)
      {
        /* Only show the TX SFD if the transmitter was capable of getting
           the SFD into the packet in time. */
        rxData.packet[payloadStart+5] &= ~0x80;
        printf("TxSFD=0x%05X, ",
               ((rxData.packet[payloadStart+3]<< 0)|
                (rxData.packet[payloadStart+4]<< 8)|
                  (rxData.packet[payloadStart+5]<<16)));
      }
      else
      {
        printf("TxSFD=-------, ");
      }
      printf("RSSI=%ddBm, LQI=0x%02X\r\n",
             rxData.rssi,
             rxData.lqi);
#ifdef PC_GUI
      //printf("Planet Table\r\n");
      printf("   Active  ++range  ++ ** Active  ** ||  range || \r\n");
      for(i=0;i<PLANET_TABLE_SIZE;i++) 
      {
        printf("++%d++",i+2); 
        printf(" %d ", planetTable[i].active);
        printf("*+%d     ++",i+2);         
        printf("**  0x%04X     ** ", planetTable[i].shortAddr);
        printf("||   %d     ||",i+2);
        
        uint8_t k=8;
        while(k--) 
        {
          printf("%02X", planetTable[i].longAddr[k]);
        }
        printf("\r\n");
      }
      printf("END_TABLE\r\n");
#endif /* PC_GUI*/  
      break;

    case (SUN_SEARCH_PACKET): /* executes on SUN */
      RX_DETAILS(printf("SUN_SEARCH_PACKET\r\n");)
        /* As a sun, if I have room in my planet table I transmit that
           I am available.  Otherwise, I don't transmit anything. */
        for(i=0;i<PLANET_TABLE_SIZE;i++)
        {
          if(!planetTable[i].active)
          {
            /* Length */
            packet[0] = (24+2);
            packet[1] = FCF_DATA;
            packet[2] = FCF_LONGDST + FCF_LONGSRC;
            /* New sequence number */
            currSeqNum++;
            packet[3]=currSeqNum;
            /* dst PAN ID */
            packet[4] = (0xFFFF>>0)&0xFF;
            packet[5] = (0xFFFF>>8)&0xFF;
            /* long dst addr */
            memcpy((packet+6), longSrcAddr, 8);
            /* src PAN ID */
            packet[14] = (ST_RadioGetPanId()>>0)&0xFF;
            packet[15] = (ST_RadioGetPanId()>>8)&0xFF;
            /* Long src addr */
            memcpy((packet+16), ST_RadioGetEui64(), 8);
            /* payload type */
            packet[24] = PT_SUN_AVAILABLE;
            /* Broadcast response */
            enqueueTxPacket(TRUE, 0xFFFF, packet, 0);
            break;
          }
        }
      break;

    case (SUN_AVAILABLE_PACKET): /*executes on PLANET*/
      RX_DETAILS(printf("SUN_AVAILABLE_PACKET\r\n");)
        /* If I already found a sun, ignore any subsequent available messages */
        if(availableSunFound)
        {
          return;
        }
      availableSunFound=TRUE;
      /* As a planet, send a unicast join request to the sun.  But first,
         switch to the sun's PAN in anticipation of completing the join. */
      ST_RadioSetPanId(srcPanId);
      /* Length */
      packet[0] = (22+2);
      packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
      packet[2] = FCF_LONGDST + FCF_LONGSRC;
      /* New sequence number */
      currSeqNum++;
      packet[3]=currSeqNum;
      /* dst PAN ID */
      packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
      packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
      /* long dst addr */
      memcpy((packet+6), longSrcAddr, 8);
      /* long src addr */
      memcpy((packet+14), ST_RadioGetEui64(), 8);
      /* payload type */
      packet[22] = PT_JOIN_REQUEST;
      /* Unicast join request */
      enqueueTxPacket(TRUE, 0xFFFF, packet, 0);
      break;

    case (JOIN_REQUEST_PACKET): /* executes on SUN */
      RX_DETAILS(printf("JOIN_REQUEST_PACKET\r\n");)
      {
        uint8_t pt = PT_JOIN_DENIED;
        uint8_t assignedShortId[2] = {0xFE, 0xFF};
        /* sun responds with unicast packet to long ID, acceptance, and the
           assigned short ID as payload.  denied packets have a short ID 0xFFFE */

        /* Length */
        packet[0] = (24+2);
        packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
        packet[2] = FCF_LONGDST + FCF_LONGSRC;
        /* New sequence number */
        currSeqNum++;
        packet[3]=currSeqNum;
        /* dst PAN ID */
        packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
        packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
        /* long dst addr */
        memcpy((packet+6), longSrcAddr, 8);
        /* long src addr */
        memcpy((packet+14), ST_RadioGetEui64(), 8);

        /* sun looks for a free spot in the planet table
           if none, sun sends a unicast join denied, then idles
           if one, sun allocates the planet a short ID and sends a unicast
           join accepted to long id with short id in payload, then idles */
        for(i=0;i<PLANET_TABLE_SIZE;i++)
        {
          if(!planetTable[i].active)
          {
            planetTable[i].active = TRUE;
            shortAddrCounter++;
            planetTable[i].shortAddr = shortAddrCounter;
            memcpy(planetTable[i].longAddr, longSrcAddr, 8);
            pt = PT_JOIN_ACCEPTED;
            assignedShortId[0] = (shortAddrCounter>>0)&0xFF;
            assignedShortId[1] = (shortAddrCounter>>8)&0xFF;

#ifdef PC_GUI
            printf("<JOIN_LEAVE_PLANET>\r\n");
#endif 
           
            printf("Join: Planet 0x%04X (index %d) has joined the network\r\n", shortAddrCounter, i);

#ifdef PC_GUI
            printf("++  range  ++ ** Active  ** ||  range || \r\n");
            for(i=0;i<PLANET_TABLE_SIZE;i++) 
            {
              
              printf("|++|   %d     |++|",i+2);         
              printf("|**|   %d     |**| ", planetTable[i].active);
              printf("|+|   %d     |+|",i+2);
              printf("|*|0x%04X|*+|", planetTable[i].shortAddr);
              printf("%d", i+2);
              uint8_t k=8;
              while(k--)
              {
                printf("%02X", planetTable[i].longAddr[k]);
              }
              
              printf("\r\n");
              
            }
            printf("END_TABLE\r\n");
#endif /* PC_GUI */            
            break;
          }
        }
        /* payload type */
        packet[22] = pt;
        packet[23] = assignedShortId[0];
        packet[24] = assignedShortId[1];
        /* Unicast join request response */
        enqueueTxPacket(TRUE, 0xFFFF, packet, 0);
      }
      break;

    case (JOIN_ACCEPTED_PACKET): /* executes on PLANET */
      RX_DETAILS(printf("JOIN_ACCEPTED_PACKET\r\n");)
        /* pan ID is already set during PT_SUN_AVAILABLE */
        
        /* set my short ID to the one assigned to me via the payload */
        ST_RadioSetNodeId((rxData.packet[payloadStart+1]<<0)|
                          (rxData.packet[payloadStart+2]<<8));
      /* join complete, halt the search */
      networkJoinedStopSearching = TRUE;
      break;

    case (JOIN_DENIED_PACKET): /* executes on PLANET */
      RX_DETAILS(printf("JOIN_DENIED_PACKET\r\n");)
        /* return to broadcast PAN ID and try other suns/channels */
        ST_RadioSetPanId(0xFFFF);
      break;

    case (POLLING_PACKET): /*executes on SUN*/
      RX_DETAILS(printf("POLLING_PACKET\r\n");)
        /* Remember: ST_RadioDataPendingShortIdIsrCallback already did
           the hard work of determining is the polling node has data.  If
           ackFramePendingSet is true then we definitely have data from
           the polling device.  Send the packet now. */
        if(rxData.ackFramePendingSet) 
        {
          /* When transmitting a packet in response to receiving a packet,
             we must wait 1ms to ensure our new transmission does not disrupt
             the ACK automatically sent in response to the reception. */
          halCommonDelayMilliseconds(1);
          transmitIndirectPacketNow();
        }
      break;

    case (LEAVING_PACKET): /* executes on SUN */
      RX_DETAILS(printf("LEAVING_PACKET\r\n");)
        /* Try to remove the child from the table (but don't worry about it
        if we can't). */
        for(i=0;i<PLANET_TABLE_SIZE;i++) 
        {
          if(planetTable[i].active && (planetTable[i].shortAddr==shortSrcAddr)) 
          {
#ifdef PC_GUI
            printf("<JOIN_LEAVE_PLANET>\r\n");
#endif    
            printf("Leave: Planet 0x%04X (index %d) has left the network\r\n", shortSrcAddr, i);
            planetTable[i].active = FALSE;
            
#ifdef PC_GUI
            printf("++  range  ++ ** Active  ** ||  range || \r\n");
            for(i=0;i<PLANET_TABLE_SIZE;i++) {
              
              printf("++   %d     ++",i+2);         
              printf("**   %d     ** ", planetTable[i].active);
              printf("||   %d     ||",i+2);
              printf("*0x%04X*+", planetTable[i].shortAddr);
              printf("%d",i+2);
              
              uint8_t k=8;
              while(k--) {
                printf("%02X", planetTable[i].longAddr[k]);
              }
              printf("\r\n");
            }
            printf("END_TABLE\r\n");
#endif /* PC_GUI */
            break;
          }
        }
      break;

    default:
      RX_DETAILS(printf("Unknown payload type\r\n");)
        goto stopProcessing;
    }

stopProcessing:
  rxData.packetBeingProcessed = FALSE;
}

/**
  * @brief  This callback must be defined.  This callback is enabled 
  *         using the function ST_RadioEnableOverflowNotification but should
  *         never be called by the phy library.
  * @param  None 
  * @retval None
  * @note   If the function is called, something went wrong with receive.  
  *         Probably long latency.
  */
void ST_RadioOverflowIsrCallback(void)
{
  assert(FALSE);
}

/**
  * @brief  Obtain an uint8_t value from the command line where the value is 
  *         provided by the user as a decimal.
  * @param  None
  * @retval None
  */
uint8_t getInt8uFromUser(void)
{
  int ch;
  char textEntry[4]={0,};
  uint8_t number;
  uint8_t i=0;

  /* Obtain the number from the user.  The most I can accept is a 3 digit
  decimal number < 256  */
  printf("(8bit decimal num)> ");
  /* Flush any input characters left over (could be some stray line endings) */
  ch=getchar();
  ch=getchar();
  do {
    WDG_ReloadCounter();
    processRxPacket();
    txTick();
    ch=getchar();
    if(ch>=0)
    {
      if(ch=='\r' || ch=='\n')
      {
        printf("\r\n");
      }
      else 
      {
        printf("%c", ch);
      }
      textEntry[i]=ch;
      i++;
    }
  } while(ch!='\r' && ch!='\n' && i<sizeof(textEntry));
  textEntry[3] = '\0';
  number = atoi(textEntry);

  printf("Entered number: %d\r\n", number);
  return number;
}

/**
  * @brief  Attempt to deep sleep for a duration define by qsToNextEvent.
  * @param  qsToNextEvent
  * @retval None
  * @note   Deepsleep will also wakeup from UART_ACTIVE_MASK. This function will 
  *         check for and prevent sleep if the application is active,
  *         expecting further  operations, and should not go to sleep.
  */
void tryToSleep(uint32_t qsToNextEvent)
{
  /* Sleep can only occur while we're not the sun.  When a node
     resets or leaves, it's default node ID is not 0x0000 so the device
     will begin to sleep. */
#ifdef SUN_ROLE
  return;
#else
  StStatus status = ST_SUCCESS;

  static boolean commandPromptOn=TRUE;

if(ST_RadioGetNodeId() == 0x0000)
  {
    return;
  }

  /* Don't sleep if the radio is busy */
  if(txPacketInFlight || rxData.packetBeingProcessed || framePendingPktEnroute) 
  {
    return;
  }

  /* Don't sleep if there has been GPIO activity */
  if(NVIC->ISPR[0]&NVIC_IxxR_IRQD)
  {
    if(!commandPromptOn)
    {
      printf("\r\nStaying awake until next command:\r\n");
      printf("> ");
      commandPromptOn=TRUE;
    }
    return;
  }

  MAIN_DETAILS(printf("Sleep... ");)
    if(commandPromptOn) 
    {
      printf("(perform UART activity to wake)\r\n");
      commandPromptOn=FALSE;
    }
  fflush(stdout);

  ST_RadioSleep();
  
  ATOMIC(
         halPowerDown();
#ifdef PLANET_ROLE
         halSleepForQsWithOptions(&qsToNextEvent, UART_ACTIVE_MASK|BUTTON_S1_WAKE_SOURCE);
#else
         halSleepForQsWithOptions(&qsToNextEvent, UART_ACTIVE_MASK);
#endif
         halPowerUp();
         
         /* It is the application's responsibility to reinitialize the UART */
         uartInit();
         )
    /* The SimpleMAC library states that the radio should not be woken within
       250us of calling ST_RadioSleep.  Since I do not know the shortest
       possible time sleeping could take, I just tack on a delay here.  If
       sleep time becomes critical, this tryToSleep() function should be
       analyzed and optimized. */
    halCommonDelayMicroseconds(200);
  status = ST_RadioInit(radioPowerState);
  assert(status==ST_SUCCESS);  
  MAIN_DETAILS(printf("awake\r\n");)
    
#endif /* SUN_ROLE */
}

/**
  * @brief  print information about the node. This command
  *          is inviked by 'i'.
  * @param  None
  * @retval None 
  */
void infoCmd(void)
{
  uint8_t i;
  uint8_t * EUI64 = ST_RadioGetEui64();
  printf("<CERC_NODE_INFO_BEG>\r\n");
  printf("\r\n");
  if(activeInNetwork) {
    if(ST_RadioGetNodeId()==0x0000) {
      printf("Network role:          sun\r\n");
    } else {
      printf("Network role:          planet\r\n");
    }
  } else {
    printf("Network role:           <not active in a network>\r\n");
  }
  
  if(radioPowerState == ST_RADIO_POWER_MODE_OFF) {
    printf("Radio:                       Off\r\n");
  } else {
    printf("Radio:                       On\r\n");
  }
  if(activeInNetwork) {
    printf("In network:               Yes\r\n");
  } else {
    printf("In network:               No\r\n");
  }
  printf("Channel:                   %d\r\n", ST_RadioGetChannel());
  printf("Power:                       %d dBm\r\n", ST_RadioGetPower());
  printf("EUI64:                        0x");
  i=8;
  while(i--) {
    printf("%02X", EUI64[i]);
  }
  printf("\r\n");
  printf("PAN ID:                     0x%04X\r\n", ST_RadioGetPanId());
  printf("Node ID:                    0x%04X\r\n", ST_RadioGetNodeId());
  printf("Send Rate:                 %d\r\n", autoSendRate);
  printf("Poll Rate:                   %d\r\n", autoPollRate);
  printf("<CERC_NODE_INFO_END>");
}

/**
  * @brief  Form a network and become a sun. This command 
  *          is invoked by 'f'.
  * @param  None 
  * @retval None
  */
void formCmd(void)
{
  uint8_t i;
  uint8_t searchChannel;
  int8_t searchEnergy;
  uint8_t minEnergyChannel = ST_MIN_802_15_4_CHANNEL_NUMBER;
  int8_t minEnergy = 127;
  StStatus status = ST_SUCCESS;
  
  printf("\r\n");
  if(activeInNetwork)
  {
    printf("Already in network\r\n");
    return;
  }
  printf("Inactive node forming network and becoming the sun\r\n");

  initNetworkState();

  TURN_RADIO_ON();

  /* Loop across all channels */
  for(searchChannel=ST_MIN_802_15_4_CHANNEL_NUMBER; searchChannel<=ST_MAX_802_15_4_CHANNEL_NUMBER; searchChannel++)
  {
    int8_t maxEnergy=-128;
    status = ST_RadioSetChannel(searchChannel);
    assert(status==ST_SUCCESS);

    for(i=0;i<ENERGY_SAMPLES_COUNT;i++)
    {
      /* Wait 16 symbol periods for energy readings to settle. */
      halCommonDelayMicroseconds(256);
      /* Find the max energy for this channel */
      searchEnergy = ST_RadioEnergyDetection();
      if(searchEnergy > maxEnergy)
      {
        maxEnergy = searchEnergy;
      }
    }
    /* Find the channel with the lowest max energy */
    if(maxEnergy < minEnergy)
    {
      minEnergy = maxEnergy;
      minEnergyChannel = searchChannel;
    }
  }

  /* Select the channel with the lowest max energy */
  status = ST_RadioSetChannel(minEnergyChannel);
  assert(status==ST_SUCCESS);
  printf("Selected channel %d with energy %d dBm\r\n",
         minEnergyChannel, minEnergy);

  /* Give our new network a random PAN ID */
  ST_RadioSetPanId(halCommonGetRandom());
  printf("Randomly chose PAN ID: 0x%04X\r\n", ST_RadioGetPanId());

  /* When forming a network, the sun always assigns itself Node ID 0 0
  (shortAddrCounter is initialized to 0 by initNetworkState) */
  ST_RadioSetNodeId(shortAddrCounter);

  /* Address filtering was already enabled during  app initialization */
  /* Planet table was already cleared out by initNetworkState() */

  /* The new network is now formed and ready */
  activeInNetwork = TRUE;
}

/**
  * @brief  Join a network and become a planet. This command 
  *          is inviked by 'l'.
  * @param  None
  * @retval None
  * @Note   After the first packet is transmitted here, the rest of 
  *          the join process is handled by processRxPacket.
  */
void joinCmd(void)
{
  uint8_t packet[128];
  uint8_t searchChannel;
  uint32_t lastTime;
  StStatus status = ST_SUCCESS;

  printf("\r\n");
  if(activeInNetwork)
  {
    printf("Already in network\r\n");
    return;
  }
  printf("Inactive node joining network and becoming a planet\r\n");

  initNetworkState();

  TURN_RADIO_ON();
  /* While we're not joined to a network yet, the receive ISR needs to be able
     to process packets.  This state will either stay TRUE if the join
     succeeds or return to FALSE if it doesn't. */
  activeInNetwork = TRUE;

  /* Length */
  packet[0] = (18+2);
  packet[1] = FCF_DATA;
  packet[2] = FCF_SHORTDST + FCF_LONGSRC;
  /* packet[3] - New sequence number is handled immediately before transmission */
  /* dst PAN ID */
  packet[4] = (0xFFFF>>0)&0xFF;
  packet[5] = (0xFFFF>>8)&0xFF;
  /* Short dst addr */
  packet[6] = (0xFFFF>>0)&0xFF;
  packet[7] = (0xFFFF>>8)&0xFF;
  /* src PAN ID */
  packet[8] = (0xFFFF>>0)&0xFF;
  packet[9] = (0xFFFF>>8)&0xFF;
  /* Long src addr */
  memcpy((packet+10), ST_RadioGetEui64(), 8);
  /* payload type */
  packet[18] = PT_SUN_SEARCH;

  printf("Trying channel");
  /* Loop across all channels */
  for(searchChannel=ST_MIN_802_15_4_CHANNEL_NUMBER; searchChannel<=ST_MAX_802_15_4_CHANNEL_NUMBER; searchChannel++) 
  {
    /* Since we wait 200ms per channel while trying to join, the watchdog
       must be reset to prevent it from triggering. */
    WDG_ReloadCounter();

    printf(" %d", searchChannel);
    status = ST_RadioSetChannel(searchChannel);
    assert(status==ST_SUCCESS);

    /* Populate the join packet with a new sequence number */
    currSeqNum++;
    packet[3]=currSeqNum;

    /* Broadcast searching for any suns */
    availableSunFound = FALSE;
    enqueueTxPacket(TRUE, 0xFFFF, packet, 0);

    /* Dwell for 200ms trying to complete a join with any sun on this channel*/
    lastTime = halCommonGetInt32uMillisecondTick();
    do 
    {
      processRxPacket();
      txTick();
    }
    while(elapsedTimeInt32u(lastTime, halCommonGetInt32uMillisecondTick()) < 200);

    /* If our join was accepted and we've successfully transmitted our join
       acceptence acknowledgment, we can exit the join channel search. */
    if(networkJoinedStopSearching)
    {
      printf("\r\n");
      printf("Joined on channel %d with PAN ID 0x%04X.  My ID is now 0x%04X.\r\n",
             ST_RadioGetChannel(),
             ST_RadioGetPanId(),
             ST_RadioGetNodeId());
      activeInNetwork = TRUE;
#ifdef PLANET_ROLE
      autoSendRate = 60;
      STM_EVAL_LEDOn(LED3);
#endif
      return;
    }
  }

  printf("\r\n");
  printf("Did not join.  Returning to inactive state.\r\n");
  /* Getting here means we did not successful join a network. */
  activeInNetwork = FALSE;
}

/**
  * @brief  Leave the network an become an inactive node. this command 
  *         is invoked by 'l'.
  * @param  None
  * @retval None
  */
void leaveCmd(void)
{
  uint8_t packet[128];
  uint16_t dstShortAddr = 0x0000;
  uint32_t lastTime;

  printf("\r\n");
  if(!activeInNetwork)
  {
    printf("Not in network\r\n");
    return;
  }
  printf("Leaving network and becoming an inactive node\r\n");

  /* A planet sends a courtesy leave message.  Don't bother with an ACK since
  * the planet will leave no matter what. */
  if(ST_RadioGetNodeId() != 0x0000)
  {
    /*Length*/
    packet[0] = (10+2);
    packet[1] = FCF_DATA + FCF_INTRAPAN;
    packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
    /* New sequence number */
    currSeqNum++;
    packet[3] = currSeqNum;
    /* dst PAN ID */
    packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
    packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
    /* Short dst addr */
    packet[6] = (dstShortAddr>>0)&0xFF;
    packet[7] = (dstShortAddr>>8)&0xFF;
    /* Short src addr */
    packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
    packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
    /* payload type */
    packet[10] = PT_LEAVING;
    enqueueTxPacket(TRUE, dstShortAddr, packet, 0);
    /* Give ourselves 200ms to complete any transactions before disappearing. */
    lastTime = halCommonGetInt32uMillisecondTick();
    do 
    {
      processRxPacket();
      txTick();
    }
    while(elapsedTimeInt32u(lastTime, halCommonGetInt32uMillisecondTick()) < 200);
  }

  /* Initilizing will return us to a clean, inactive state */
  initNetworkState();
  TURN_RADIO_OFF();
#ifdef PLANET_ROLE
  STM_EVAL_LEDOff(LED3);
#endif
}

/**
  * @brief  Send a data packet. This command is indirectly 
  *          invoked by 's'.
  * @param  vddMillivolts: the value of the vdd measurement
  * @param  dstShortAddr:  the address of the node
  * @param  sendDirectly:  boolean value 
  * @retval None
  */
void sendVddDataPacket(uint16_t vddMillivolts, uint16_t dstShortAddr, boolean sendDirectly)
{
  uint8_t packet[128];

  /* Length */
  packet[0] = (15+2);
  packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
  packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
  /* New sequence number */
  currSeqNum++;
  packet[3] = currSeqNum;
  /* dst PAN ID */
  packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
  packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
  /* Short dst addr */
  packet[6] = (dstShortAddr>>0)&0xFF;
  packet[7] = (dstShortAddr>>8)&0xFF;
  /* Short src addr */
  packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
  packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
  /* payload type */
  packet[10] = PT_GENERIC_DATA;
  /* my VDD_PADS */
  packet[11] = (vddMillivolts>>0)&0xFF;
  packet[12] = (vddMillivolts>>8)&0xFF;
  /* Zero out the TX SFD payload.  The MSB is used to indicate SFD valid. */
  packet[13] = 0;
  packet[14] = 0;
  packet[15] = 0;
  enqueueTxPacket(sendDirectly, dstShortAddr, packet, 13);
}

/**
  * @brief  Allow the user to select which planet a sun should 
  *          send a data packet to. This command is indirectly
  *          invoked by 's'.
  *          This function is invoked by the command 's'
  * @param  dstShortAddr: the short address of a device
  * @retval return a boolean value.
  * @note   executes on sun node
  */
boolean selectPlanetFromTable(uint16_t * dstShortAddr)
{
  uint8_t entry;
  uint8_t i, k;

  printf("Select an entry number from the (active) planet list:\r\n");
  printf("Entry | Short Address |    Long Address\r\n");
  for(i=0;i<PLANET_TABLE_SIZE;i++)
  {
    if(planetTable[i].active)
    {
      printf("   %d  | ", i);
      printf("    0x%04X    | 0x", planetTable[i].shortAddr);
      k=8;
      while(k--)
      {
        printf("%02X", planetTable[i].longAddr[k]);
      }
      printf("\r\n");
    }
  }
  entry = getInt8uFromUser();

  if(entry>=PLANET_TABLE_SIZE)
  {
    printf("Chosen number outside of planet table scope\r\n");
    return FALSE;
  }
  if(!planetTable[entry].active)
  {
    printf("Chosen number is not an active planet\r\n");
    return FALSE;
  }
  
  *dstShortAddr = planetTable[entry].shortAddr;
  
  return TRUE;
}

/**
  * @brief  Implement a sending data on a sun node. This command is indirectly 
  *          invoked by 's'.
  * @param  autoSend:
  * @retval None
  * @note   executes on sun node
  */
void sunSendCmd(boolean autoSend)
{
  uint16_t vddMillivolts = halMeasureVdd(ADC_SAMPLE_CLOCKS_512);
  uint16_t dstShortAddr=autoSendRateDstShortAddr;
  
  if(!autoSend)
  {
    printf("Send my VDD measurement (%d mV) to a planet...\r\n",
           vddMillivolts);
    
    if(!selectPlanetFromTable(&dstShortAddr)) 
    {
      return;
    }
    
    printf("Sending indirectly to sleepy planet 0x%04X (waiting for poll...)\r\n",
           dstShortAddr);
  }

  sendVddDataPacket(vddMillivolts, dstShortAddr, FALSE);
}

/**
  * @brief  Implement sending data on a planet node. This command 
  *          is indirectly invoked by 's'.
  * @param  autosend
  * @retval None
  * @note   executes on planet node
  */
void planetSendCmd(boolean autoSend)
{
  uint16_t vddMillivolts = halMeasureVdd(ADC_SAMPLE_CLOCKS_512);
  /* A planet only sends to the sun which is always short address 0x0000 */
  uint16_t dstShortAddr=0x0000;
  
  if(!autoSend)
  {
    printf("Sending my VDD measurement (%d mV) to sun (0x0000)\r\n",
           vddMillivolts);
  }
  
  sendVddDataPacket(vddMillivolts, dstShortAddr, TRUE);
}

/**
  * @brief  Send a data packet. This command is invoked by 's'.
  * @param  autoSend: a boolean variable  
  * @retval None
  * @note   In addition to being invoked manually by the user, 
  *          this function can be called due to periodic 
  *          automatic send events, controlled by the rate command.
  *          The Plnet sends a periodic message to the sun;
  */
void sendCmd(boolean autoSend)
{
  if(!autoSend) 
  {
    printf("\r\n");
    if(!activeInNetwork) 
    {
      printf("Not active in a network\r\n");
      return;
    }
  }
  if(ST_RadioGetNodeId() == 0x0000) 
  {
    sunSendCmd(autoSend);
    return;
  } 
  else 
  {
    planetSendCmd(autoSend);
  }
}

/**
  * @brief  Clear the indirect transmission queue. Command
  *          invoked by 'c'.
  * @param  None
  * @retval None
  */
void clearQCmd(void)
{
  printf("\r\n");
  if(!activeInNetwork)
  {
    printf("Not active in a network\r\n");
    return;
  }
  if(ST_RadioGetNodeId() != 0x0000) 
  {
    printf("Only the sun has an indirect transmit queue\r\n");
    return;
  }
  
  printf("Clearing indirect transmit queue\r\n");
  clearIndirectQueue();
}

/**
  * @brief  Poll for a data packet. This command is 
  *          invoked by 'p'. 
  * @param  autopoll:
  * @retval None 
  * @Note  This function can be called due to periodic auotmatic
  *          poll events, controlled by the rate command.
  */
void pollCmd(boolean autoPoll)
{
  uint8_t packet[128];
  /* polls are always sent to the sun */
  uint16_t dstShortAddr = 0x0000;

  if(!autoPoll)
  {
    printf("\r\n");
    if(!activeInNetwork)
    {
      printf("Not active in a network\r\n");
      return;
    }
    if(ST_RadioGetNodeId() == 0x0000)
    {
      printf("Only a planet can poll\r\n");
      return;
    }
    
    printf("Sending poll packet to sun\r\n");
  }

  /* Length */
  packet[0] = (10+2);
  packet[1] = FCF_MACCMD + FCF_ACKREQ + FCF_INTRAPAN;
  packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
  /* New sequence number */
  currSeqNum++;
  packet[3] = currSeqNum;
  /* dst PAN ID */
  packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
  packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
  /* Short dst addr */
  packet[6] = (dstShortAddr>>0)&0xFF;
  packet[7] = (dstShortAddr>>8)&0xFF;
  /* Short src addr */
  packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
  packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
  /* payload type */
  packet[10] = PT_POLLING;

  enqueueTxPacket(TRUE, dstShortAddr, packet, 0);
}

/**
  * @brief  Define the rate of periodic automatic send and poll events by the 
  *         command 'r'. 
  * @param  None
  * @retval None
  * @Note   This function implements a sub-menu to determine which
  *         event the user wants to control and obtain the new rate value.
  */
void rateCmd(void)
{
  int ch;
  
  printf("\r\n");
  if(!activeInNetwork)
  {
    printf("Not active in a network\r\n");
    return;
  }

  if(ST_RadioGetNodeId() == 0x0000)
  {
    autoSendRate = 0;
    if(!selectPlanetFromTable(&autoSendRateDstShortAddr))
    {
      printf("Send rate is now %d qs\r\n", autoSendRate);
      return;
    }
    printf("Set send rate in quarterseconds.  rate=0 is off.  255 max.\r\n");
    autoSendRate = getInt8uFromUser();
    printf("Send rate is now %d qs\r\n", autoSendRate);
    /* Put the last tick in the past so it immediately triggers a new event */
    autoSendLastQsTick = (uint16_t)
      ((uint16_t)halCommonGetInt16uQuarterSecondTick() - (uint16_t)autoSendRate);
    return;
  }

  printf("Enter 's' to adjust send rate or 'p' to adjust poll rate: ");
  do
  {
    WDG_ReloadCounter();
    processRxPacket();
    txTick();
    ch=getchar();
  } while(ch!='s' && ch!='p');
  printf("\r\n");
  switch(ch) 
  {
  case 's':
    autoSendRate = 0;
    autoSendRateDstShortAddr = 0x0000; /*Planet always sends to sun*/
    printf("Set send rate in quarterseconds.  rate=0 is off.  255 max.\r\n");
    autoSendRate = getInt8uFromUser();
    printf("Send rate is now %d qs\r\n", autoSendRate);
    /* Put the last tick in the past so it immediately triggers a new event */
    autoSendLastQsTick = (uint16_t)
      ((uint16_t)halCommonGetInt16uQuarterSecondTick() - (uint16_t)autoSendRate);
    return;
  case 'p':
    autoPollRate = 0;
    printf("Set poll rate in quarterseconds.  rate=0 is off.  255 max.\r\n");
    autoPollRate = getInt8uFromUser();
    printf("Poll rate is now %d qs\r\n", autoPollRate);
    /* Put the last tick in the past so it immediately triggers a new event */
    autoPollLastQsTick = (uint16_t)
      ((uint16_t)halCommonGetInt16uQuarterSecondTick() - (uint16_t)autoPollRate);
    return;
  }
}

/**
  * @brief  Print the table giving planets status by the command 't'.
  * @param  None
  * @retval None
  */
void planetTableCmd(void)
{
  uint8_t i,k;
  
  printf("\r\n");
  if(!activeInNetwork) {
    printf("Not active in a network\r\n");
    return;
  }
  if(ST_RadioGetNodeId() != 0x0000) {
    printf("Not a sun\r\n");
    return;
  }
  printf("<PLANET_TABLE_DISPALAY>\r\n");
  printf("Planet Table\r\n");
  printf("Active | DataPending |  Short Address  |    Long Address\r\n");
  for(i=0;i<PLANET_TABLE_SIZE;i++) {

    printf("   %d     | ", planetTable[i].active);
    printf("        %d           | ", isDataPendingForShortId(planetTable[i].shortAddr));
    printf("     0x%04X        | 0x", planetTable[i].shortAddr);
    k=8;
    while(k--) {
      printf("%02X", planetTable[i].longAddr[k]);
    }
    printf("\r\n");
  }
  printf("<PLANET_TABLE_END>\r\n");
}

/**
  * @brief   Print the help menu by the command '?'.
  * @param  None
  * @retval None
  */
void helpCmd(void)
{
  printf("\r\n");
  printf("CMD Description  (compatible roles)\r\n");
  printf("i   Display status information  (both)\r\n");
#ifdef SUN_ROLE
  printf("f   Form a network  (make inactive node a sun)\r\n");
#endif
#ifdef PLANET_ROLE
  printf("j   Join a network  (make inactive node a planet)\r\n");
#endif
  printf("l   Leave a network  (both)\r\n");
  printf("s   Send data  (both)\r\n");
#ifdef SUN_ROLE
  printf("c   Clear indirect transmit queue  (sun only)\r\n");
#endif
#ifdef PLANET_ROLE
  printf("p   Poll for data  (planet only)\r\n");
#endif
  printf("r   Adjust send/poll rates  (both)\r\n");
#ifdef SUN_ROLE
  printf("t   Display the planet table  (sun only)\r\n");
#endif

  printf("o   Enter OTA bootloader mode \r\n");
  printf("u   Enter UART bootloader mode \r\n");
  printf("?   Display this help menu\r\n");
}

/**
  * @brief  Main program
  * @param  None
  * @retval int value
  */
int main(void)
{
  /*!< Application main:  Entry point, initialization, 
  command line implementation, and event control.
  */
  uint8_t ch;
  uint32_t seed;
  uint16_t currQsTick=0;
  uint16_t lastPeriodicEventsQsTick;
  StStatus status = ST_SUCCESS;
  
  /* Initialize the HAL.*/
  halInit();
  
  /* Initialize the UART.  uartInit() will configure PB1=OUT_ALT and PB2=IN. */
  uartInit();
  
  /* Connect IRQD to PB2/SC1RXD */
  EXTI->CR[1] = PORTB_PIN(2);
  /* Allow the IRQD flag to set on any IRQD activity.  This does not
     enable the top level interrupt since no interrupt is desired; just
     an activity flag is needed to know when to avoid deep sleep. */
  EXTI->TSR[3] = ( 3 << 5); /* GPIO Mode bits position: 5 */
  EXTI->PR = EXTI_PR_IRQDP;
  NVIC->ICPR[0] = NVIC_IxxR_IRQD;
  
  lastPeriodicEventsQsTick = halCommonGetInt16uQuarterSecondTick();
  
  INTERRUPTS_ON();
  
#ifdef PLANET_ROLE
  printf("\r\nSimpleMAC (%s) Sample Application: 'Planet role'!\r\n",SIMPLEMAC_VERSION_STRING);
#endif
  
#if defined (SUN_ROLE) && defined (PC_GUI) 
  printf("\r\nSimpleMAC GUI(%s) Sample Application: 'Sun role'!\r\n",SIMPLEMAC_VERSION_STRING);
#elif defined (SUN_ROLE) 
  printf("\r\nSimpleMAC (%s) Sample Application: 'Sun role'!\r\n",SIMPLEMAC_VERSION_STRING);
#else
#endif
  
  /* Initialize random number generation. */
  ST_RadioGetRandomNumbers((uint16_t *)&seed, 2);
  halCommonSeedRandom(seed);
  
  /* Initialize the radio. */
  ST_RadioEnableOverflowNotification(TRUE);
  status = ST_RadioInit(radioPowerState);
  assert(status==ST_SUCCESS);
  
  printf("Enter ? for list of commands\r\n");
  
  while(1) 
  {
    printf("\r\n> ");
    do 
    {
      WDG_ReloadCounter();
      processRxPacket();
      txTick();
      currQsTick = halCommonGetInt16uQuarterSecondTick();
      /* Run periodic poll event */
      if(autoPollRate > 0)
      {
        if(elapsedTimeInt16u(autoPollLastQsTick, currQsTick)>autoPollRate)
        {
          autoPollLastQsTick = currQsTick;
          MAIN_DETAILS(printf("Automatic poll\r\n");)
            pollCmd(TRUE);
        }
      }
      /* Run periodic send event */
      if(autoSendRate > 0)
      {
        if(elapsedTimeInt16u(autoSendLastQsTick, currQsTick)>autoSendRate)
        {
          autoSendLastQsTick = currQsTick;
          MAIN_DETAILS(printf("Automatic send\r\n");)
            sendCmd(TRUE);
        }
      }
      /* Calculate events and attempt to sleep.  (tryToSleep will not do
         anything on a sun.) */
      {
        uint32_t qsToNextEvent = 0xFFFFFFFF;
        uint16_t now = halCommonGetInt16uQuarterSecondTick();
        if(autoPollRate>0)
        {
          uint16_t qsDelta;
          if(timeGTorEqualInt16u(now, (autoPollLastQsTick+autoPollRate)))
          {
            qsToNextEvent = 0;
          }
          else
          {
            qsDelta = elapsedTimeInt16u(now,(autoPollLastQsTick+autoPollRate));
            qsToNextEvent = (qsDelta < qsToNextEvent) ? qsDelta : qsToNextEvent;
          }
        }
        if(autoSendRate > 0)
        {
          uint16_t qsDelta;
          if(timeGTorEqualInt16u(now, (autoSendLastQsTick+autoSendRate)))
          {
            qsToNextEvent = 0;
          } 
          else
          {
            qsDelta = elapsedTimeInt16u(now,(autoSendLastQsTick+autoSendRate));
            qsToNextEvent = (qsDelta < qsToNextEvent) ? qsDelta : qsToNextEvent;
          }
        }
        /* If there are no timed events, qsToNextEvent is maximally large,
           which is 34 years.  That should be long enough. */
        
        if(framePendingPktEnroute)
        {
          /* If framePending got set, stay awake for 200ms giving plenty
             of time for packet(s) to come in. */
          if(elapsedTimeInt16u(framePendingMsTick, halCommonGetInt16uMillisecondTick()) > 200)
          {
            framePendingPktEnroute = FALSE;
          }
        }
        else
        {
          /* Only sleep if qsToNextEvent says there an event is coming up.  If
             qsToNextEvent is 0, halSleepForQsWithOptions() will immediately
             return.  This is not a problem except for the fact that tryToSleep()
             prints status and would fill the screen with extraneous
             messages.
          NOTE: If there is no upcoming events, qsToNextEvent will be set to 0xFFFFFFFF
                 so we will enter sleep and only wake on non-timed events.*/
          if(qsToNextEvent > 0)
          {
            tryToSleep(qsToNextEvent);
          }
        }
      }
      /* Run periodic maintenance events */
      if(elapsedTimeInt16u(lastPeriodicEventsQsTick, currQsTick) > PERIODIC_MAINTENANCE_EVENTS_RATE)
      {
        MAIN_DETAILS(printf("Periodic maintenance events\r\n");)
          if(ST_RadioCheckRadio())
          {
            ST_RadioCalibrateCurrentChannel();
          }
        halCommonCheckXtalBiasTrim();
        halCommonCalibratePads();
        lastPeriodicEventsQsTick = halCommonGetInt16uQuarterSecondTick();
      }
#ifdef SUN_ROLE
    } while ((!__io_getcharNonBlocking(&ch)));
#endif
#ifdef PLANET_ROLE
    if (STM_EVAL_PBGetState(BUTTON_S1) == 0x00)
    {
      planet_join_by_button = 1;
    }
  } while ((!__io_getcharNonBlocking(&ch)) && (!planet_join_by_button));
  
  if (planet_join_by_button)
  {
    planet_join_by_button = 0;
    joinCmd();
  }
  else /* do switch */
#endif 
    
    switch(ch) 
    {
    case 'i':
      infoCmd();
      break;
#ifdef SUN_ROLE
    case 'f':
      formCmd();
      break;
#endif
#ifdef PLANET_ROLE
    case 'j':
      joinCmd();
      break;
#endif
    case 'l':
      leaveCmd();
      break;
    case 's':
      sendCmd(FALSE);
      break;
#ifdef SUN_ROLE
    case 'c':
      clearQCmd();
      break;
#endif
#ifdef PLANET_ROLE
    case 'p':
      pollCmd(FALSE);
      break;
#endif
    case 'r':
      rateCmd();
      break;
#ifdef SUN_ROLE
    case 't':
      planetTableCmd();
      break;
#endif
     case 'o':
      printf ("Entering OTA bootloader mode ...\r\n");
      fflush(stdout);
      halBootloaderStart(BOOTLOADER_MODE_OTA, IAP_BOOTLOADER_DEFAULT_CHANNEL, IAP_BOOTLOADER_PAN_ID);
      printf ("Node does not contain IAP bootloader\r\n");
      break;
#ifdef PC_GUI
    case 'R':
      halReboot();
      break;
#endif 
      
    case 'u':
      printf ("Entering Uart bootloader mode ...\r\n");
      fflush(stdout);
      halBootloaderStart(BOOTLOADER_MODE_UART, IAP_BOOTLOADER_DEFAULT_CHANNEL, IAP_BOOTLOADER_PAN_ID);
      printf ("Node does not contain IAP bootloader\r\n");
      break;
      
    case '?':
      helpCmd();
      break;
    default:
      printf("Unknown Command\r\n");
      break;
    case '\r':
    case '\n':
      /*ignored*/
      break;
    }
  /* After a command is executed, we're allowed to go back to sleep so
     clear out the UART activity flag.  If there is packet or event
     activity, it will independently keep us awake as needed. */
  EXTI->PR = EXTI_PR_IRQDP;
  NVIC->ICPR[0] = NVIC_IxxR_IRQD;
  }
}

/* USER Callbacks: The MAC Timer compare functionality is not used in this app. */
void ST_RadioMacTimerCompareIsrCallback(void)
 {}

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
