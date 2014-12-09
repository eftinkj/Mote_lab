/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : talk.c
* Author             : MCD Application Team
* Version            : V1.0.1
* Date               : April 2010
* Description        : SimpleMac demo application.
*                      Download the same application image on two STM32W108 MB851
*                      platforms.
*                      Open 2 hyper terminals on the related USB Virtual COMs.
*                      What you you write on an hyperterminal is displayed on
*                      the other and viceversa.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

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

//#include PLATFORM_HEADER
//#include BOARD_HEADER
//#include "hal/hal.h"
//#include "hal/micro/cortexm3/uart.h"
//#include "include/phy-library.h"
//#include "hal/error.h"
//#include "stdio.h"
//#include "stdlib.h"

#include "arm_mote/common.h"

#include "FEAT_STM32W/STM32W_Radio.h"

/* Private define ------------------------------------------------------------*/
#define TIMEOUT_VALUE 100
#define BUFFER_SIZE 64

// #define serialReadByte __io_getcharNonBlocking
#include "arm_mote/HW_LIB/RTC.h"


//#define _RSSI_SNIFFER_
//#undef _RSSI_SNIFFER_

//#define _HEX_SNIFFER_
//#define XBEE_COMPAT


// # ifdef _RSSI_SNIFFER_
#ifdef _ASCI_SNIFFER_

	uint8_t	enable_pkt_trace = 1;
	uint32_t pkt_trace_len = 14;
#endif // _RSSI_SNIFFER_
	
#ifdef XBEE_COMPAT
	IAR_PACKED struct api_frame_rx16
	{
		uint8_t api_start;
		uint16_t api_length;
		uint8_t api_id;
		uint16_t api_source;
		uint8_t api_rssi;
		uint8_t api_options;
		uint8_t api_data; // with crc at the end
	};
	typedef struct api_frame_rx16 api_frame_rx16_t;
	#define API_RX16_MIN_LENGTH 5
	#define API_RX16_HEAD_LENGTH 0x05
#endif // XBEE_COMPAT
	
/* Private variables ---------------------------------------------------------*/

/* radioTransmitConfig variables required from the SimpleMac library for packet
   transmission */
RadioTransmitConfig radioTransmitConfig =
{
	TRUE,  // waitForAck;
	TRUE, // checkCca;
	4,     // ccaAttemptMax;
	3,     // backoffExponentMin;
	5,     // backoffExponentMax;
	FALSE //TRUE   // appendCrc;
};

/* generic data packet */
#pragma align txPacket
uint8_t txPacket[128] =
{
	0x0a, // length
	0x61, //0x61, // fcf - intra pan, ack request, data
	0x88, //0x08 // fcf - src, dst mode
	0x00, // seq
//	0x04, // dst pan l
//	0x16, // dst pan h
	0x32, // dst pan l
	0x33, // dst pan h
	0xFF, 0xFF,
	0x32, // src pan l
	0x33, // scr pan h
	0x0A, 0x00,
/*	0x05, // dst addr l
	0x16, // dst addr h
*/	0x00  // data
};

/* buffer for received packet */
uint8_t rxPacket[128];
/* received packet flag */
boolean packetReceived = FALSE;
/* received packet's RSSI */
uint8_t rxPacketRSSI;

/* packet transmission complete flag */
boolean txComplete = TRUE;


uint32_t rxpkt_oveflow_count_ = 0;
uint32_t rxpkt_wrong_count_ = 0;
uint8_t beforelast_id = 0;
uint8_t last_id = 0;


/* Private Functions ---------------------------------------------------------*/
StStatus radio_call_status = 0;
/*******************************************************************************
* Function Name  : sendSerialData
* @brief It allows to transmit the data
* @param - lenght of the data
*                  - data to be transmitted
* @retval None
*******************************************************************************/
boolean sendSerialData( uint8_t length, uint8_t *data )
{
	//ATOMIC(
		if ( txComplete != FALSE )
		{//8
			halCommonMemCopy( txPacket + 12, data, length );
			//
			txPacket[0] = length + 13;//12;//2 + 7;
			txPacket[3]++; /* increment sequence number */
			//txPacket[10]++; /* increment sequence number */

			txComplete = FALSE;
			radio_call_status = ST_RadioTransmit( txPacket );
		}
	//)
	return ~txComplete; //TRUE;
}/* end sendSerialData() */

/*******************************************************************************
* Function Name  : sendSerialData
* @brief It processes serial commands
* @param None
* @retval None
*******************************************************************************/
void processSerialInput( void )
{
	static uint8_t buffer[BUFFER_SIZE];
	static uint8_t bufferSize = 0;
	static uint32_t bufferTimeout = TIMEOUT_VALUE;
	
	if ( bufferSize == 0 )
		bufferTimeout = TIMEOUT_VALUE;

	if ( serialReadByte( buffer + bufferSize ) )
	{
#if (1)
		if ('?'==buffer[bufferSize])
		{
			//printf("Timers: temp=%d, lcd=%d\n", temp_sensor_timeout, lcd_timeout);
			print_timeouts();
		}
#endif

		bufferSize += 1;
	}

	bufferTimeout--;

	if ( ( bufferTimeout == 0 ) || ( bufferSize == BUFFER_SIZE ) )
	{
		if ( bufferSize > 0 )
		{
			sendSerialData( bufferSize, buffer ); // Send via Radio
			bufferSize = 0;
		}
	}
}/* end processSerialInput() */

/* ********************* SimpleMac callbacks functions ************************/

// Calculate chaceksum over 8-bit (unsigned) data blocks
uint8_t add8( uint8_t*data, uint8_t len)
{
	uint8_t i, temp = 0;
	for(i=0; i<len; i++)
	{
		temp += data[i];
	}
	return temp;
}

/*******************************************************************************
* Function Name  : ST_RadioReceiveIsrCallback
* @brief Radio Receiver callback function
* @param - packet: received packet
*                  - ackFramePendingSet: frame pending bit in the received packet
*                  - time: MAC timer when the SFD was received
*                  - errors: numbers of correlator erros in the packet
*                  - rssi: energy detected in the packet
* @retval None
*******************************************************************************/
uint16_t src;
void ST_RadioReceiveIsrCallback( uint8_t *packet,
								 boolean ackFramePendingSet,
								 uint32_t time,
								 uint16_t errors,
								 int8_t rssi )
{
	/* note this is executed from interupt context */
	uint8_t i;

	/* Copy the packet to a buffer that can be accessed from the main loop;
	   don't do the copy if there is already a packet there being processed */
#ifdef _RSSI_SNIFFER_
	if (0x88 == packet[2])
	{
		src = packet[9];
		src = src << 8;
		src += packet[8];
		printf("%d %d\r", src, rssi);
	}
#endif // _RSSI_SNIFFER_
#ifdef	_ASCI_SNIFFER_
	if (1 == enable_pkt_trace)
	{
		printf("P");
		for(int i = 0; i< (packet[0]>pkt_trace_len?pkt_trace_len:packet[0]); i++)
			printf(" %2x", packet[i]);
		printf("\r");
	}
#endif // _ASCI_SNIFFER_
#ifdef	_HEX_SNIFFER_
	#ifndef XBEE_COMPAT
	//	if (1 == enable_pkt_trace)
	if (0x88 == packet[2])
	{
		__write(_LLIO_STDOUT, (unsigned char *)packet, packet[0]);
	}
	#else
	if (0x88 == packet[2])
	{	
		api_frame_rx16_t temp;
		uint8_t sum = 0;
		src = packet[9];
		src = src << 8;
		src += packet[8];
		temp.api_start = 0x7e;
		temp.api_length = ENDIAN16((uint16_t)packet[0]-6);//+API_RX16_HEAD_LENGTH);
		temp.api_id = 0x81;
		temp.api_source = ENDIAN16(src);
		temp.api_rssi = 255-rssi;
		temp.api_options = packet[3]; // WILL USE FOR seq id
		//
		sum += add8((unsigned char *)&temp.api_id, 5);
		sum += add8( (unsigned char *)&(packet[12]), packet[0]-11 );
		sum = 0xFF - sum;
		__write(_LLIO_STDOUT, (unsigned char *)&temp, 8);
		__write(_LLIO_STDOUT, (unsigned char *)&(packet[12]), packet[0]-11);
		__write(_LLIO_STDOUT, (unsigned char *)&sum, 1);
	} else {
		__write(_LLIO_STDOUT, (unsigned char *)packet, packet[0]);
	}
	#endif // ELSE not defined XBEE_COMPAT
#endif // _HEX_SNIFFER_

#if !defined(_HEX_SNIFFER_) && !defined(_RSSI_SNIFFER_) && !defined(_ASCI_SNIFFER_)
	if ( packetReceived == FALSE )
	{
		if ( 0x88 == packet[2] )
		{
			for ( i = 0; i <= packet[0]; i++ )
			{
				rxPacket[i] = packet[i];
			}
			beforelast_id = last_id;
			last_id = packet[1];
			rxPacketRSSI = 255 - rssi;
			
			//rs_rx_rssi(STRADIO_GET_SRC_ADDR(packet), rssi); // add the rssi info to sensor - make id dynamic
			
			packetReceived = TRUE;
		}
		else
		{
			rxpkt_wrong_count_ ++;
		}
	}
	else
	{
		rxpkt_oveflow_count_ ++;
	}
#endif // if neither _RSSI_SNIFFER_ nor _HEX_SNIFFER_ nor _ASCI_SNIFFER_

}/* end ST_RadioReceiveIsrCallback() */


/*******************************************************************************
* Function Name  : ST_RadioDataPendingShortIdIsrCallback
* @brief Callback for Radio Short Id data pending
* @param shortId address         : 
* @retval TRUE/FALSE
*******************************************************************************/
boolean ST_RadioDataPendingShortIdIsrCallback( uint16_t shortId )
{
	return FALSE;
}/* end ST_RadioDataPendingShortIdIsrCallback() */


/*******************************************************************************
* Function Name  : ST_RadioDataPendingLongIdIsrCallback
* @brief Callback for Radio Long  Id data pending
* @param long id address
* @retval TRUE/FALSE 
*******************************************************************************/
boolean ST_RadioDataPendingLongIdIsrCallback( uint8_t* longId )
{
	return FALSE;
}/* ST_RadioDataPendingShortIdIsrCallback() */



/*******************************************************************************
 **
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
}/* end ST_RadioOverflowIsrCallback() */



/*******************************************************************************
* Function Name  : ST_RadioSfdSentIsrCallback
* @brief Radio SFD sent event callback
* @param sfdSentTime:MAC timer when the SFD was sent
* @retval None
*******************************************************************************/
void ST_RadioSfdSentIsrCallback( uint32_t sfdSentTime )
{
}/* end ST_RadioSfdSentIsrCallback() */


/*******************************************************************************
* Function Name  : ST_RadioMacTimerCompareIsrCallback
* @brief Radio MAC timer comapre Event callback
* @param None
* @retval None
*******************************************************************************/
void ST_RadioMacTimerCompareIsrCallback( void )
{
}/* end ST_RadioMacTimerCompareIsrCallback() */


#define PERIODIC_MAINTENANCE_EVENTS_RATE	1000
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

#define BUTTON_CLICKED 1
#define BUTTON_IDLE    0
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



extern int main_arm( void ); // declaration of main function with scheduler loop

/*******************************************************************************
* Function Name  : main.
* @brief talk main routine.
* @param None
* @retval None
*******************************************************************************/
void main( void )
{
	uint32_t seed;
	
	/* Initialization */
	halInit();
	ST_RadioGetRandomNumbers((uint16_t *)&seed, 2);
	halCommonSeedRandom(seed);
	//ST_RadioGetRandomNumbers( ( uint16_t * )&seed, 2 );
	//srand( seed );

#ifdef ENABLE_UART
//	uartInit( UART_BAUDRATE, 8, PARITY_NONE, 1 );
	uartInit( ); // New version with default config (115200 bps, 8bit, 1stop, no parity)
#endif // ENABLE_UART

	INTERRUPTS_ON();

	/* init leds */
	//halInitLed();
	STM_EVAL_LEDInit(RLED);
	STM_EVAL_LEDInit(YLED);
	

	/* Initialize radio (analog section, digital baseband and MAC).
	Leave radio powered up in non-promiscuous rx mode */
	  ST_RadioEnableOverflowNotification(TRUE);
	assert( ST_RadioInit( ST_RADIO_POWER_MODE_RX_ON ) == ST_SUCCESS );

	/* Setup some node and pan ids.  The packet above is also sent to a device
	   with the same node and pan id so that two nodes running this same image
	   will talk to each other, even though its not right to have two nodes
	   with the same node id */
    //  ST_RadioSetNodeId(0x0030);
    //  ST_RadioSetPanId(0x1604);
	ST_RadioSetChannel( 0x14 );

	main_power_up();

	radio_init();

// Debugging type printout -> to check is anything works
#ifndef NO_UART_OUT
	uint8_t len = 10;
	uint8_t data[] = "TR.TEST_1\r";
	sendSerialData( len, data ); // Send via Radio
    // Print via UART (USB-to-serial):
	printf( "\r\nSimpleMAC (%s) Talk Application (EUI=%x)\r\n", SIMPLEMAC_VERSION_STRING, ST_RadioGetEui64() );
#endif

   sch_add_loop(processSerialInput);     
// *********************************** Main loop *****************************
	// This should be end of the road (inside is an infinite loop)
	
// Special application -> only reads Radio input and prints to UART - no scheduling
#if defined(_RSSI_SNIFFER_) || defined(_HEX_SNIFFER_) || defined(_ASCI_SNIFFER_)
	// Enable promiscous mode:
	/* Set promiscuous mode: receive any packet on the selected radio
	channel */
	/* Disable address filtering*/
	ST_RadioEnableAddressFiltering(FALSE);
	/* Turn off automatic acknowledgment */
	ST_RadioEnableAutoAck(FALSE);
	ST_RadioSetCoordinator(TRUE);
	ST_RadioEnableReceiveCrc(FALSE);
	// forwarding the RSSI info is done inside the ISR
	int8_t rf_en_ = 0;
	uint8_t chan = ST_RadioGetChannel();
	while (1)
	{
#ifdef ENABLE_ENERGY_SNIFFER
		rf_en_ = ST_RadioEnergyDetection();
		if (-50 < rf_en_)
		printf("Energy = %d\n",  rf_en_);
		periodicMaintenanceEvents();
		if(BUTTON_CLICKED == getButtonStatus(BUTTON_S1))
		{
			if (ST_MAX_802_15_4_CHANNEL_NUMBER < ++chan) chan = ST_MIN_802_15_4_CHANNEL_NUMBER;
			ST_RadioSetChannel(chan);
			printf("Channel=%d (%x)\n", chan, chan);
		}
#endif // #ifdef ENABLE_ENERGY_SNIFFER
	}
        
#else //_RSSI_SNIFFER_
	
	main_arm(); // The main code with Scheduler

#endif // else _RSSI_SNIFFER_


}/* end main ()*/

