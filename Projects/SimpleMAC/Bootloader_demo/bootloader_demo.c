/**
  ******************************************************************************
  * @file    bootloader_demo.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012 
  * @brief   Demo about over the air (OTA) ST bootloader protocol (see AN3262 )
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

/** @addtogroup demos
 * Bootloader demo \see bootloader_demo.c for documentation.
 *@{
 */

/* Includes ------------------------------------------------------------------*/
#include PLATFORM_HEADER
#include "board.h"
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "hal.h"
#include "serial_utils.h"
#include "command-interpreter2.h"
#include "phy-library.h"
#include "image.h"
#include "image256K.h"
#include "bl.h"
#include "timer.h"
#include "micro/cortexm3/iap_bootloader.h"
#include "bootloader_demo.h"
/* Private define ------------------------------------------------------------*/
#define MAC_ACK_RETRIES 3
#define TX_BUFFER(a) (txBufferControl.broadcast ? (txBroadcastBuffer.a) : (txBuffer.a))
#define TX_BUFFER_GET_POINTER(a) (txBufferControl.broadcast ? (&txBroadcastBuffer.a) : (&txBuffer.a))

/* Private typedef -----------------------------------------------------------*/
typedef uint8_t IEEEAddr[8];

enum txStatus {
  TX_SUCCESS = 0,
  TX_PENDING = 1,
  TX_FAILED = 2
};

typedef struct txBufferControlTypeStruct {
  uint8_t payload[128];
  uint8_t broadcast;
  uint8_t retryNumber;
  uint8_t retries;
  __IO uint8_t status;
  uint8_t pointer;
  uint8_t sequenceCounter;
} txBufferControlType;

#pragma pack(1)
typedef struct txBufferTypeStruct {
  uint8_t length;
  uint16_t fc;
  uint8_t seqNo;
  uint16_t dstPanID;
  IEEEAddr destEui64;
  IEEEAddr srcEui64;
  uint8_t payload[128];
} txBufferType;
typedef struct txBroadcastBufferTypeStruct {
  uint8_t length;
  uint16_t fc;
  uint8_t seqNo;
  uint16_t dstPanID;
  uint16_t dstShortAddr;
  uint16_t srcPanID;
  IEEEAddr srcEui64;
  uint8_t payload[128];
} txBroadcastBufferType;
typedef struct rxBufferTypeStruct {
  uint8_t length;
  uint16_t fc;
  uint8_t seqNo;
  uint16_t dstPanID;
  IEEEAddr destEui64;
  IEEEAddr srcEui64;
  uint8_t payload[128];
  uint16_t lastSequenceNumber;
  uint8_t packetReceived;
  uint8_t payloadLength;
  uint8_t payloadPointer;
} rxBufferType;
#pragma pack()

/* Private variables ---------------------------------------------------------*/
CommandEntry CommandTable[] = {
  { "setDestEui64", setDestEui64Action, "b",
    "set destination EUI64 for booloader commands\r\n"
    "parameters: eui64"},
  { "getDestEui64", getDestEui64Action, "", ""},
  { "findBLNodes", findNodesInBootloaderModeAction, "", ""},
  { "loadImage", loadImageAction, "", "Find first bootlader node and load test image to it"},
  { "get", getCommandAction, "", "get command"},
  { "bget", broadcastGetCommandAction, "", "get command (broadcast)"},
  { "getid", getIdCommandAction, "", "get id command"},
  { "bgetid", broadcastGetIdCommandAction, "", "get id command (broadcast)"},
  { "getversion", getVersionCommandAction, "", "get version command"},
  { "bgetversion", broadcastGetVersionCommandAction, "", "get version command (broadcast)"},
  { "read", readCommandAction, "wu", 
    "Read memory command\r\n"
    "parameters: address bytes"},
  { "write", writeCommandAction, "wub", 
    "Write memory command\r\n"
    "parameters: address bytes data"},
  { "writeIncremental", writeIncrementalCommandAction, "ub", 
    "Write inc memory command\r\n"
    "parameters: bytes data"},
  { "erase", eraseCommandAction, "ub",
    "Erase memory command\r\n"
    "parameters: pages page_list"},
  { "go", goAddressCommandAction, "w", 
    "Go command\r\n"
    "parameters: address"},
  { "help", helpAction, "", "List commands"},
  { NULL, NULL, NULL, NULL } /* NULL action makes this a terminator */
};

ALIGN_VAR(txBufferType txBuffer, 2);
ALIGN_VAR(txBroadcastBufferType txBroadcastBuffer, 2);
rxBufferType rxBuffer;
txBufferControlType txBufferControl;

uint8_t len;
uint8_t buff[COMMAND_BUFFER_LENGTH];
uint8_t currIndex = 0;
uint8_t serialPort = 1;
uint8_t interactive;

RadioTransmitConfig radioTransmitConfig = RADIO_TRANSMIT_CONFIG_DEFAULTS;
static const uint8_t prodId256k[] = {0x01,0x09,0xB0};

/* Private function prototypes -----------------------------------------------*/
extern boolean responsePrintf(const char * formatString, ...);
static uint8_t transmitByte(uint8_t *data, uint8_t lastByte);
static uint8_t receiveByte(uint8_t *data);
static void txBufferInit(uint8_t broadcast);
static void rxBufferInit(void);
uint8_t findNodesInBootloaderMode (IEEEAddr *eui64, uint8_t maxEUI64);

/* Private functions ---------------------------------------------------------*/
#if defined ( __CC_ARM )

/* Add dummy abort function required for MDK-ARM toolchain */
 void abort (void) {
 }
#endif
/**
  * @brief command Name
  * @param cmdCode Command code
  * @retval command Name
  */
static char *commandName(uint8_t cmdCode)
{
  char *returnValue = "Unknown";
  switch (cmdCode) {
  case GET: returnValue = "GET";
    break;
  case GET_VERSION: returnValue = "GET_VERSION";
    break;
  case GET_ID: returnValue = "GET_ID";
    break;
  case ERASE: returnValue = "ERASE";
    break;
  case WRITE: returnValue = "WRITE";
    break;
  case WRITE_INCREMENTAL: returnValue = "WRITE_INCREMENTAL";
    break;  
  case READ: returnValue = "READ";
    break;
  case GO: returnValue = "GO";
    break;
  default: 
    break;
  }

  return returnValue;
}

/**
  * @brief process Command Input
  * @param interactive status
  * @retval process Command Input status
  */
boolean processCmdInput(uint8_t interactive)
{
  if (interactive) {
    /* continue calling emberSerialReadPartialLine() until it returns success, indicating a full line has been read */
    if (!serialReadPartialLine((char *)buff, COMMAND_BUFFER_LENGTH, &currIndex)) {
      return FALSE;
    }
    
    len=0;
    /* search foward looking for first CR, LF, or NULL to determine length of the string */
    while((buff[len]!='\n') && (buff[len] !='\r') && (buff[len]!='\0')) {
      len++;
    }
    buff[len ++] = '\r'; /* set the final char to be CR */
    buff[len ++] = '\n'; /* set the final char to be CR */
    
    currIndex = 0;
    return processCommandString(buff, len);
  } else {
    return processCommandInput();
  }
}

/**
  * @brief set Dest Eui64 Action
  * @param None
  * @retval None
  */
void setDestEui64Action(void)
{
  IEEEAddr destEui64;
  copyStringArgument(0, (uint8_t *) &destEui64, 8, 0);
  { int i; for (i = 7; i >= 0; i--) {
      txBuffer.destEui64[i] = destEui64[7-i];
    }
  }
  responsePrintf("{&N call... OK}\r\n", "SetDestEui64");
}

/**
  * @brief get Dest Eui64 Action
  * @param None
  * @retval None
  */
void getDestEui64Action(void)
{
  responsePrintf("{&N call... OK, ", "getEui64");
  responsePrintf("{%t:","destEui64");
  { int i; for (i = 7; i >= 0; i--) {
      responsePrintf("%x",(txBuffer.destEui64)[i]);
    }
    responsePrintf("}");
  }
  responsePrintf("}\r\n");
}

/**
  * @brief load Image Action
  * @param None
  * @retval None
  */
void loadImageAction(void)
{
  uint32_t bytesWritten;
  uint32_t state;
  uint32_t startTime, totalTime = 0;
  uint8_t  failed = 0;
  uint8_t found = 0;
  uint8_t is256K = FALSE;
  imageDescType imageDesc;

  /* Take first node in bootloader mode */
  found = findNodesInBootloaderMode(&txBuffer.destEui64, 1);

  if (found) {

    /* Detect if node is 128K or 256K using the product ID */
    {
      uint8_t reply[3];
      uint8_t status;
      status = bootloadCommand(GET_ID, NULL, sizeof(reply), reply);
      
      if ((status == BOOTLOAD_COMMAND_SUCCESS) && (halCommonMemCompare(reply, prodId256k, 3) == 0))
	is256K = TRUE;
    }

    printf ("Loading image on remote node with EUI64(%s): ", is256K ? "256K" : "128K");
    { int i; for (i = 7; i >= 0; i--) {
	printf("%02X",(txBuffer.destEui64)[i]);
      }
    }
    printf ("\r\n");


    txBufferInit(FALSE);
    rxBufferInit();
    if (is256K) {
      imageDesc.imagePtr = image256K;
      imageDesc.imageSize = sizeof(image256K);
      imageDesc.imageOffset = 0;
    } else {
      imageDesc.imagePtr = image;
      imageDesc.imageSize = sizeof(image);
      imageDesc.imageOffset = IAP_BOOTLOADER_APPLICATION_ADDRESS - MFB_BOTTOM;
    }

    blInit(&imageDesc, transmitByte, receiveByte);

    while (TRUE) {
      startTime = TIME_CurrentTime();
      state = blStateMachine(&bytesWritten);
      totalTime += TIME_CurrentTime() - startTime;
      if (state == BL_STATE_FAILURE)
	failed = 1;
      printf ("Written %05lu/%05d\r", bytesWritten, sizeof(image));
      TIMER_Tick();
      if (state == BL_STATE_IDLE) {
	printf ("\r\n%s\r\n", failed ? "Failed" : "OK");
	break;
      }
    }
    if (!failed)
      printf ("Total time (not including screen messages) = %lu.%06lu sec.\r\n", (totalTime)/1000000, (totalTime) % 1000000);
  } else {
    printf ("No nodes found in bootloader mode\r\n");
  }
}

/**
  * @brief Command Execution
  * @param cmdCode command code
  * @param *args pointer on btlCommandArgType
  * @param replyLength reply length
  * @param *reply pointer on replay message
  * @param broadcast broadcast status
  * @retval None
  */
static void cmdExecution(uint8_t cmdCode, btlCommandArgType *args, uint16_t replyLength, uint8_t *reply, uint8_t broadcast)
{
  uint8_t status;
  char *command;
  char commandString[64];

  txBufferInit(broadcast);
  rxBufferInit();

  status = bootloadCommand(cmdCode, args, replyLength, reply);

  command = commandName(cmdCode);
  strcpy(commandString, command);
  if (broadcast) {
    strcpy(commandString + strlen(commandString), "(broadcast)");
  }

  responsePrintf("{&N Bootload command... &t2x \r\n", commandString, "status", status);
  responsePrintf("{%t:","reply");
  if (replyLength == 0xFFFF) {
    replyLength = reply[0]+2;
  }
  if (status == BOOTLOAD_COMMAND_SUCCESS)
  {
    uint8_t i;
    for (i = 0; i < replyLength; i++) {
      responsePrintf("%x", reply[i]);
    }
    responsePrintf ("}");
  }
  responsePrintf ("}\r\n");
}

/**
  * @brief get Command Action
  * @param None
  * @retval None
  */
void getCommandAction(void)
{
  uint8_t reply[64];

  cmdExecution(GET, NULL, 0xFFFF, reply, FALSE); 
}

/**
  * @brief get IdCommand Action
  * @param None
  * @retval None
  */
void getIdCommandAction(void)
{
  uint8_t reply[3];

  cmdExecution(GET_ID, NULL, sizeof(reply), reply, FALSE); 
}

/**
  * @brief get Version Command Action
  * @param None
  * @retval None
  */
void getVersionCommandAction(void)
{
  uint8_t reply[3];

  cmdExecution(GET_VERSION, NULL, sizeof(reply), reply, FALSE); 
}

/**
  * @brief read Command Action
  * @param None
  * @retval None
  */
void readCommandAction(void)
{
  uint8_t reply[128];
  uint32_t address;
  uint32_t bigEndianAddress;
  uint8_t bytes;
  btlCommandArgType args[3];  

  address = unsignedCommandArgument(0);
  args[0].length = 4;
  args[0].data = (uint8_t *) &bigEndianAddress;

  /* Address must be sent big endian */
  {
    uint8_t i;
    uint8_t *addr = (uint8_t *) &address;
    uint8_t *BEaddr = (uint8_t *) &bigEndianAddress;
    for (i = 0; i < 4; i++)
      BEaddr[3-i] = addr[i];
  }

  bytes = unsignedCommandArgument(1) - 1;
  args[1].length = 1;
  args[1].data = &bytes;

  /* Terminate args list */
  args[2].length = 0;

  cmdExecution(READ, args, bytes + 1, reply, FALSE); 
}

/**
  * @brief write Command Action
  * @param None
  * @retval None
  */
void writeCommandAction(void)
{
  uint8_t data[128];
  uint32_t address;
  uint32_t bigEndianAddress;
  btlCommandArgType args[3];  

  address = unsignedCommandArgument(0);
  args[0].length = 4;
  args[0].data = (uint8_t *) &bigEndianAddress;

  /* Address must be sent big endian */
  {
    uint8_t i;
    uint8_t *addr = (uint8_t *) &address;
    uint8_t *BEaddr = (uint8_t *) &bigEndianAddress;
    for (i = 0; i < 4; i++)
      BEaddr[3-i] = addr[i];
  }

  data[0] = unsignedCommandArgument(1) - 1;
  copyStringArgument(2, (uint8_t *) data+1, data[0] + 1, 0);

  args[1].length = data[0] + 2;
  args[1].data = data;

  /* Terminate args list */
  args[2].length = 0;

  cmdExecution(WRITE, args, 0, NULL, FALSE); 
}

/**
  * @brief erase Command Action
  * @param None
  * @retval None
  */
void eraseCommandAction(void)
{
  uint8_t data[128];
  btlCommandArgType args[2];  

  data[0] = unsignedCommandArgument(0) - 1;
  copyStringArgument(1, (uint8_t *) data+1, data[0] + 1, 0);

  args[0].length = data[0] + 2;
  args[0].data = data;

  /* Terminate args list */
  args[1].length = 0;

  cmdExecution(ERASE, args, 0, NULL, FALSE); 
}

/**
  * @brief go Address Command Action
  * @param None
  * @retval None
  */
void goAddressCommandAction(void)
{
  uint32_t address = unsignedCommandArgument(0);
  uint32_t bigEndianAddress;
  btlCommandArgType args[2];  

  args[0].length = 4;
  args[0].data = (uint8_t *) &bigEndianAddress;
  args[1].length = 0;
  /* Address must be sent big endian */
  {
    uint8_t i;
    uint8_t *addr = (uint8_t *) &address;
    uint8_t *BEaddr = (uint8_t *) &bigEndianAddress;
    for (i = 0; i < 4; i++)
      BEaddr[3-i] = addr[i];
  }
  cmdExecution(GO, args, 0, NULL, FALSE); 
}

/**
  * @brief write Incremental Command Action
  * @param None
  * @retval None
  */
void writeIncrementalCommandAction(void)
{
  uint8_t data[128];
  btlCommandArgType args[2];  

  data[0] = unsignedCommandArgument(0) - 1;
  copyStringArgument(1, (uint8_t *) data+1, data[0] + 1, 0);

  args[0].length = data[0] + 2;
  args[0].data = data;

  /* Terminate args list */
  args[1].length = 0;

  cmdExecution(WRITE_INCREMENTAL, args, 0, NULL, FALSE); 
}

/**
  * @brief broadcast GetCommand Action
  * @param None
  * @retval None
  */
void broadcastGetCommandAction(void)
{
  uint8_t reply[64];

  cmdExecution(GET, NULL, 0xFFFF, reply, TRUE); 
}

/**
  * @brief broadcast GetId Command Action
  * @param None
  * @retval None
  */
void broadcastGetIdCommandAction(void)
{
  uint8_t reply[3];

  cmdExecution(GET_ID, NULL, sizeof(reply), reply, TRUE); 
}

/**
  * @brief broadcast Get Version Command Action
  * @param None
  * @retval None
  */
void broadcastGetVersionCommandAction(void)
{
  uint8_t reply[3];

  cmdExecution(GET_VERSION, NULL, sizeof(reply), reply, TRUE); 
}

/**
  * @brief find Nodes In Bootloader Mode
  * @param None
  * @retval None
  */
uint8_t findNodesInBootloaderMode (IEEEAddr *eui64, uint8_t maxEUI64)
{
  uint8_t status;
  uint8_t reply[3];
  uint8_t count = 0;

  txBufferInit(TRUE);
  rxBufferInit();

  status = bootloadCommand(GET_VERSION, NULL, sizeof(reply), reply);

  if (status == BOOTLOAD_COMMAND_SUCCESS) {
    if (count < maxEUI64) {
	halCommonMemCopy(eui64[count++], &rxBuffer.srcEui64, sizeof(IEEEAddr));
    }
    while (count < maxEUI64) {
      uint32_t startTime;
      uint8_t found = FALSE;

      startTime = TIME_CurrentTime();
      while (TIME_ELAPSED(startTime) < 1000000) {
	if (rxBuffer.packetReceived) {
	  halCommonMemCopy(eui64[count++], &rxBuffer.srcEui64, sizeof(IEEEAddr));
	  rxBuffer.packetReceived = FALSE;
	  found = TRUE;
	  break;
	}
      }
      if (! found)
	break;
    }
  }
  return count;
}

/**
  * @brief find Nodes In Boot loader Mode Action
  * @param None
  * @retval None
  */
void findNodesInBootloaderModeAction(void)
{
  IEEEAddr eui64List[8];
  uint8_t eui64Count = 0;
  uint8_t i;

  eui64Count = findNodesInBootloaderMode(eui64List, 
					 sizeof(eui64List) / sizeof (IEEEAddr));
  responsePrintf("{&N Bootload command... &t2x \r\n", "findNodesInBootloaderMode", "count", eui64Count);
  for (i = 0; i < eui64Count; i++) {
    int8_t j;
    responsePrintf("{%t_%d:","EUI64", i);
    for (j = 7; j >= 0; j--)
      printf ("%02X", eui64List[i][j]);
    responsePrintf ("}\r\n");
  }
  responsePrintf ("}\r\n");
}

/**
  * @brief help Action
  * @param None
  * @retval None
  */
void helpAction(void)
{
  CommandEntry *cmd;

  for (cmd = CommandTable; cmd->name != NULL; cmd++) {
    printf ("%s %s %s\r\n\r\n", cmd->name, cmd->argumentTypes, cmd->description);
  }
}

/**
  * @brief transmit byte
  * @param *data pointer on data to transmit
  * @param lastByte plast byte to transmit
  * @retval transmit operation status 
  */
static uint8_t transmitByte(uint8_t *data, uint8_t lastByte)
{
  uint8_t returnValue = TX_SUCCESS;

  txBufferControl.payload[txBufferControl.pointer++] = *data;
  if (lastByte) {
    if (txBufferControl.broadcast) {
      txBroadcastBuffer.length = txBufferControl.pointer + 19;
      txBroadcastBuffer.seqNo = txBufferControl.sequenceCounter++;
    } else {
      txBuffer.length = txBufferControl.pointer + 23;
      txBuffer.seqNo  = txBufferControl.sequenceCounter++;
    }
    
    txBufferControl.status = TX_PENDING;
    txBufferControl.retryNumber = 0;
    halCommonMemCopy(TX_BUFFER(payload), txBufferControl.payload, txBufferControl.pointer);

    returnValue = ST_RadioTransmit((uint8_t *) TX_BUFFER_GET_POINTER(length));
    if (returnValue != TX_SUCCESS) {
      txBufferControl.status = TX_FAILED;
    } 
    
    /* Wait for transmission to complete */
    while (txBufferControl.status == TX_PENDING);

    returnValue = txBufferControl.status;
    txBufferControl.pointer = 0;
  }
  return returnValue;
}

/**
  * @brief receive byte
  * @param *data pointer on data to receive 
  * @retval receive operation status
  */
static uint8_t receiveByte(uint8_t *data)
{
  uint8_t returnValue = FALSE;
  if (rxBuffer.packetReceived) {
    *data = rxBuffer.payload[rxBuffer.payloadPointer++];
    if (rxBuffer.payloadPointer >= rxBuffer.payloadLength)
      rxBuffer.packetReceived = FALSE;
    returnValue = TRUE;
  }
  return returnValue;
}

/**
  * @brief Tx Buffer initialization
  * @param None
  * @retval None
  */
static void txBufferInit(uint8_t broadcast)
{
  if (broadcast) {
    txBufferControl.broadcast = TRUE;
    txBufferControl.retries = 0;

    txBroadcastBuffer.length = 0;
    txBroadcastBuffer.fc = 0xc801;
    txBroadcastBuffer.dstPanID = 0xFFFF;
    txBroadcastBuffer.dstShortAddr = 0xFFFF;
    txBroadcastBuffer.srcPanID = IAP_BOOTLOADER_PAN_ID;
  } else {
    txBufferControl.broadcast = FALSE;
    txBufferControl.retries =  MAC_ACK_RETRIES;

    txBuffer.length = 0;
    txBuffer.fc = 0xcc61;
    txBuffer.dstPanID = IAP_BOOTLOADER_PAN_ID;
  }

  halCommonMemCopy ((void *) TX_BUFFER(srcEui64), ST_RadioGetEui64(), 8);
  
  txBufferControl.retryNumber = 0;
  txBufferControl.status = TX_SUCCESS;
  txBufferControl.pointer = 0;
}

/**
  * @brief Rx Buffer initialization
  * @param None
  * @retval None
  */
static void rxBufferInit(void)
{
  halCommonMemSet (&rxBuffer, 0x00, sizeof(rxBuffer));

  rxBuffer.lastSequenceNumber = 0xffff; /* Invalid value */
}

/**
  * @brief Main program
  * @param None
  * @retval int value
  */
int main (void)
{
  uint8_t returnValue;
  uint32_t seed;

  interactive = 1;

  halInit();
  ST_RadioGetRandomNumbers((uint16_t *)&seed, 2);
  halCommonSeedRandom(seed);
  uartInit();
  INTERRUPTS_ON();  

  /* Initialize radio (analog section, digital baseband and MAC).
  Leave radio powered up in non-promiscuous rx mode */
  returnValue = ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);

  assert(returnValue==ST_SUCCESS); 

  TIMER_Init();

  printf("Bootloader demo application\r\n");
  
  responsePrintf("{&N API call... &t2x}\r\n", "halGetResetInfo", "resetInfo", 0);

  txBufferInit(FALSE);
  rxBufferInit();
  blInit(NULL, transmitByte, receiveByte);

  ST_RadioSetPanId(IAP_BOOTLOADER_PAN_ID);
  ST_RadioSetChannel(IAP_BOOTLOADER_DEFAULT_CHANNEL);

  commandReaderInit();

  while(1) {
    /* Process input and print prompt if it returns TRUE.*/
    if (processCmdInput(interactive)) {
      if (interactive) {
	printf(">");
      }
      TIMER_Tick();
    }
  }
}


/******************************************************************************/
/*                       SimpleMac callbacks functions                        */
/******************************************************************************/
/**
  * @brief Radio Receiver callback function
  * @param packet: received packet
  * @param ackFramePendingSet: frame pending bit in the received packet
  * @param time: MAC timer when the SFD was received
  * @param errors: numbers of correlator erros in the packet
  * @param rssi: energy detected in the packet
  * @retval None
  */
void ST_RadioReceiveIsrCallback(uint8_t *packet,
                                boolean ackFramePendingSet,
                                uint32_t time,
                                uint16_t errors,
                                int8_t rssi)
{
  rxBufferType *packetReceived = (rxBufferType *) packet;
  /* note this is executed from interupt context */

  /* Copy the packet to a buffer that can be accessed from the main loop;
     don't do the copy if there is already a packet there being processed */
  if ((packetReceived->seqNo != rxBuffer.lastSequenceNumber) &&
      (packetReceived->fc == 0xCC61)) {
    if(rxBuffer.packetReceived == FALSE) {
      halCommonMemCopy(&rxBuffer, packet, packet[0] + 1);
      rxBuffer.packetReceived = TRUE;
      rxBuffer.payloadPointer = 0;
      rxBuffer.payloadLength = rxBuffer.length - 21;
    } else {
      /* This should not happen according to the protocol definition */
    }
    rxBuffer.lastSequenceNumber = packetReceived->seqNo;
  }
}

/**
  * @brief Radio Transmit callback function 
  * @param status: status of the packet transmission
  * @param sfdSentTime: MAC timer when the SFD was sent 
  * @param framePending: TRUE if the received ACK indicates that data 
  *                    is pending for this node
  * @retval None
  */
void ST_RadioTransmitCompleteIsrCallback(StStatus status,
                                         uint32_t sfdSentTime,
                                         boolean framePending)
{
  txBufferControl.status = TX_FAILED;
  switch(status) {
    case ST_SUCCESS:
      /* Success for broadcast packets */
      txBufferControl.status = TX_SUCCESS;
      break;
    case ST_PHY_TX_CCA_FAIL:
    case ST_MAC_NO_ACK_RECEIVED:
      if (txBufferControl.retryNumber < txBufferControl.retries) {
	uint8_t returnValue;
	txBufferControl.retryNumber++;
	txBufferControl.status = TX_PENDING;
	returnValue = ST_RadioTransmit((uint8_t *) TX_BUFFER_GET_POINTER(length));
	if (returnValue != TX_SUCCESS) {
	  txBufferControl.status = TX_FAILED;
	} 
      }
      break;
    case ST_PHY_ACK_RECEIVED:
      txBufferControl.status = TX_SUCCESS;
      break;
    default:
      break;
  }
}

/**
  * @brief Callback for Radio Short Id data pending 
  * @param shortId address
  * @retval TRUE/FALSE
  */
boolean ST_RadioDataPendingShortIdIsrCallback(uint16_t shortId)
{
  return FALSE;
}

/**
  * @brief Callback for Radio Long  Id data pending 
  * @param long id address
  * @retval TRUE/FALSE
  */
boolean ST_RadioDataPendingLongIdIsrCallback(uint8_t* longId)
{
  return FALSE;
}

/**
  * @brief Radio overflow event callback
  * @param None 
  * @retval None
  */
void ST_RadioOverflowIsrCallback(void)
{
}

/**
  * @brief Radio SFD sent event callback
  * @param sfdSentTime:MAC timer when the SFD was sent 
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
