/****************************************************************************
**
** Copyright (C) 2006-2005 Maciej Zawodniok, James W. Fonda. All rights reserved.
**
** This file is part of the documentation of the UMR Mote Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
// main.c for the ARM implementation


#include "defs/esnl_pub.h"

#include "common.h"

#include "sys_func.h"


//#include "definitions.h"
//#include "HW_LIB/uart_0.h"
//#include "HW_LIB/adc_0.h"
//#include "HW_LIB/new_adc_0_pub.h"
#include "HW_LIB/RTC.h"

#include "packet.h"
//#include "at_cmd.h"
#ifdef _ENABLE_XBEE_API_
	#include "FEAT_XBee_API/API_frames.h"
	#include "FEAT_Xbee_API/AT_scripts.h"
#endif // _ENABLE_XBEE_API_

#include "FEAT_Networking/Phy_layer.h"

//#ifdef _ENABLE_APP_MOD_
	#include "Application.h"
//#endif // _ENABLE_APP_MOD_




#ifdef _ENABLE_ROUTING_
	#include "FEAT_Routing/packet_AODV.h"
	//#include "network_hardware.h"
	#include "FEAT_Routing/routing.h"
	#include "FEAT_Routing/routing_OEDSR.h"
	#include "FEAT_Routing/routing_AODV.h"
#endif // _ENABLE_ROUTING_


#ifdef _ENABLE_NETWORK_STACK_
	#include "FEAT_Networking/tsp_common_pub.h"
#endif // _ENABLE_NETWORK_STACK_

#ifdef _ENABLE_QUEUING_
	#include "FEAT_Queuing/Queuing.h"
#endif // _ENABLE_QUEUING_

#ifdef _ENABLE_ROBOTICS_01_
	#include "PRJ_Robotics_01/truck_basic_control_pub.h"
#endif // _ENABLE_ROBOTICS_01_

#ifdef _ENABLE_ROBOTICS_02_
	#include "robotics_02/bot_basic_control_pub.h"
#endif //_ENABLE_ROBOTICS_02_

#ifdef _ENABLE_ROBOTICS_03_
	#include "robotics_03/qrhelo_basic_control_pub.h"
#endif //_ENABLE_ROBOTICS_03_

#ifdef _ENABLE_ROBOTICS_04_
	#include "PRJ_Robotics_04/vex_tank_basic_control_pub.h"
#endif //_ENABLE_ROBOTICS_04_

#ifdef _ENABLE_NEM_UTILITIES_01_
	#include "PRJ_NEM_Utilities_01/mon_power_basic_pub.h"
#endif // _ENABLE_NEM_UTILITIES_01_

#if defined(_ENABLE_CAMT_HEADSET_01_) || defined(_ENABLE_CAMT_HEADSET_02_)
	#include "PRJ_CAMT_Headset_01/hset_basic_pub.h"
#endif // _ENABLE_CAMT_HEADSET_01_

#ifdef _ENABLE_LOGISTICS_01_
	#include "FEAT_LGS_01/lgs_basic_prv.h"
#endif // _ENABLE_LOGISTICS_01_

#ifdef _ENABLE_F1_MOTE_TEST_01_
	#include "HW_F1_Mote_01/hw_f1_mote_basic_pub.h"
#endif // _ENABLE_F1_MOTE_TEST_01_

#ifdef FEAT_ENABLE_CC
	#include "FEAT_Networking/CongestionControl.h"
#endif // FEAT_ENABLE_CC

#ifdef FEAT_ENABLE_CH_SWITCHING
	#include "FEAT_Networking/ChannelSwitching.h"
#endif

#ifdef _ENABLE_SPI_MODULE_
	#include "HW_LIB/spi_pub.h"
#endif // _ENABLE_SPI_MODULE_

#ifdef _ENABLE_SCH_BASIC_
	#include "FEAT_Scheduler/sch_basic_pub.h"
#endif // _ENABLE_SCH_BASIC_

#ifdef ENABLE_HIO_XBEE
	#include "FEAT_HIO/hio_xbee_pub.h"
#endif // ENABLE_HIO_XBEE

#ifdef _ENABLE_FLA_BASIC_
	#include "FEAT_Flash_Access/fla_basic_pub.h"
#endif // _ENABLE_FLA_BASIC_

#ifdef _ENABLE_SSN_BASIC_
	#include "FEAT_SigProc/ssn_basic_pub.h"
#endif // _ENABLE_SSN_BASIC_

#ifdef _ENABLE_SSN_8BIT_
	#include "FEAT_SigProc/ssn_8bit_pub.h"
#endif // _ENABLE_SSN_8BIT_



#ifdef _ENABLE_SRC_DUMMY_
	#include "FEAT_SigProc/src_dummy_pub.h"
#endif // _ENABLE_SRC_DUMMY_


#if defined(_ENABLE_RTLS_BEACON_)
        #include "FEAT_CogNet/rtls_beacon.h"
#endif // #if defined(_ENABLE_RTLS_BEACON_)

#if defined(_ENABLE_MICROSTRAIN_)
        #include "FEAT_CogNet/microstrain.h"
#endif // #if defined(_ENABLE_MICROSTRAIN_)

#if defined(_ENABLE_APP_TEMP_1WIRE_)
	#include "APP_TEMP_1WIRE/app_temp_1wire_main.h"
#endif // #if defined(_ENABLE_APP_TEMP_1WIRE_)

#include "FEAT_STM32W/STM32W_Radio.h"


#include "stm32w108xx_sc.h"
extern void lcd_cmd ( uint8_t Address, uint8_t x );
extern void lcd_printStr ( uint8_t Address, const char * s );
extern void lcd_setCursor ( uint8_t Address, uint8_t x , uint8_t y );


#ifdef _ENABLE_MAIN_



/**
  * Definition of the global variable declared in the "common.h"
  */
int8_t XDATA short_packet[SHORT_PACKET_SIZE];

int XDATA dist_approx_on_ = 0;

//char xdata rx_needs_to_send; // when the RX buffer have to be sent via TX
//int next_rx_state = RX_IDLE;

//unsigned char pkt_to_sent_len;
char *pkt_to_sent;

uint16_t XDATA mac_dest;


char XDATA enableDataTx_;


unsigned long int XDATA my_energy_;
unsigned long int XDATA my_delay_;
unsigned long int XDATA my_distance_;

unsigned char XDATA my_protocol_ = PROTOCOL_ROUTING_OEDSR ;
unsigned char XDATA my_scheduling_ = PROTOCOL_SCHEDULING_NONE;
unsigned char XDATA my_backoff_ = PHY_BACKOFF_DISABLE;
uint16_t XDATA my_weight_ = DEFAULT_NODE_WEIGHT;
uint16_t XDATA my_source_weight_ = DEFAULT_SOURCE_WEIGHT;
//unsigned char xdata my_CH_ = CLUSTERING_I_AM_CH;
uint16_t XDATA my_CH_ = DEFAULT_MY_CH;

unsigned char XDATA my_rssi_threshold_ ;

uint32_t XDATA my_tx_timeout_ = DEFAULT_TX_TIMEOUT; // Timeout for receiveing confirmation of packet TX (expires if no reponse from Radio)


unsigned char XDATA my_RF_retries_ = DEFAULT_RF_RETRIES;

unsigned char XDATA energy_depleted_control_ = 0;




unsigned char XDATA sent_DATA_ = 0;


// For delayed transmission with dequeueing
unsigned char XDATA pkt_to_sent_id = 0xFF;
unsigned char XDATA pkt_to_sent_len = 0;
//char * xdata pkt_to_sent;


unsigned char XDATA last_successful_frame_id_ = 1;




#endif // _ENABLE_MAIN_

//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//Main Loop
uint32_t temp_tick = RTC_MAX_TIMER_VALUE;
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_ENABLE_ADC_MODULE_) && defined(_ENABLE_NEW_ADC_MODULE_)
	#error BOTH ADC Modules are enabled!! - ONLY ONE CAN BE ACTIVE AT THE SAME TIME!!
#endif


void main_power_up()
{
// ***************************************************************************
// ************************* Global Variables Init ***************************
// ***************************************************************************
	SET_LED(RLED);

	my_protocol_ = PROTOCOL_ROUTING_OEDSR ;
	my_scheduling_ = PROTOCOL_SCHEDULING_NONE;
	my_backoff_ = PHY_BACKOFF_DISABLE;
	my_weight_ = DEFAULT_NODE_WEIGHT;
	my_source_weight_ = DEFAULT_SOURCE_WEIGHT;
	//unsigned char xdata my_CH_ = CLUSTERING_I_AM_CH;
	my_CH_ = DEFAULT_MY_CH;
	my_tx_timeout_ = DEFAULT_TX_TIMEOUT; // Timeout for receiveing confirmation of packet TX (expires if no reponse from Radio)
	my_RF_retries_ = DEFAULT_RF_RETRIES;
	energy_depleted_control_ = 0;
	sent_DATA_ = 0;
	// For delayed transmission with dequeueing
	pkt_to_sent_id = 0xFF;
	pkt_to_sent_len = 0;
	last_successful_frame_id_ = 1;


///////////////////////////////////////////////
	// initially disable DATA transmission
	enableDataTx_ = 0;
	my_distance_ = DEFAULT_DISTANCE;
	my_energy_ = DEFAULT_ENERGY;
	my_rssi_threshold_ = RSSI_THRESH_DEFAULT;

// ***************************************************************************
// ************************* Power Up Functions ******************************
// ***************************************************************************
	
#if defined(_ENABLE_ARM_ADC_)
// || defined(_ENABLE_SRC_8BIT_ADC_)
	aadc_powerup ();
#endif // _ENABLE_ARM_ADC_

#if defined(_ENABLE_SRC_8BIT_ADC_)
	src_8bit_powerup();
#endif // _ENABLE_ARM_ADC_
	
	rtc_init();		// Real-Time Clock/Counter setup
	
#ifdef _ENABLE_SCH_BASIC_
	sch_power_up ();	// Scheduler Setup
#endif // _ENABLE_SCH_BASIC_
		
	sys_init(NULL); // system error indicatio handling

	
// ***************************************************************************
// **************** Power Up Other Application Features **********************
// ***************************************************************************
#ifdef _ENABLE_SPI_MODULE_
	spi_init_sw ( );
#endif // _ENABLE_SPI_MODULE_

#ifdef _ENABLE_ROBOTICS_01_
	truck_init ();
#endif // _ENABLE_ROBOTICS_01_

#ifdef _ENABLE_ROBOTICS_02_
	bot_init ();
#endif // _ENABLE_ROBOTICS_02_

#ifdef _ENABLE_ROBOTICS_03_
	qrhelo_init ();
#endif // _ENABLE_ROBOTICS_03_


#ifdef _ENABLE_XBEE_API_
#ifdef FEAT_ENABLE_XBEE_RECOVERY_CONFIG
	// Setup XBee module lines
	XBEE_RESET_NOT = 0;	// reset XBee
	{ unsigned char i = 0; while ( i++ < 10 ); }
	XBEE_RESET_NOT = 1; // Start XBee (not-reset!!!)
	XBEE_DTR_NOT = 0;

	fix_XBee_baudrate();
#endif

#ifndef _TEMP_ARM_
	XBEE_RESET_NOT = 1; // Start XBee (not-reset!!!)
	XBEE_DTR_NOT = 0;
#endif // _TEMP_ARM_

#endif //_ENABLE_XBEE_API_


#ifdef _ENABLE_NETWORK_STACK_
	phy_init_scan_params();
#endif // _ENABLE_NETWORK_STACK_

#ifdef _ENABLE_QUEUING_
	que_init();
#endif // _ENABLE_QUEUING_

#ifdef _ENABLE_ROUTING_
	routing_init();
#endif // _ENABLE_ROUTING_

#ifdef _ENABLE_APP_MOD_
	app_init();
#endif // _ENABLE_APP_MOD_

#ifdef FEAT_ENABLE_CC
	cc_init ( 1 );
#endif

#ifdef FEAT_ENABLE_CH_SWITCHING
	cs_power_up( );
#endif

#ifdef _ENABLE_NETWORK_STACK_
	tsp_init();
#endif // _ENABLE_NETWORK_STACK_

#ifdef _ENABLE_SSN_BASIC_
	ssn_init ( );
#endif // _ENABLE_SSN_BASIC_

#ifdef _ENABLE_SSN_8BIT_
	ssn_init ( );
#endif // _ENABLE_SSN_8BIT_

#ifdef _ENABLE_SRC_DUMMY_
	src_init();
#endif // _ENABLE_SRC_DUMMY_

#if defined(_ENABLE_RFID_SKYETEK_)
	rfid_skyetek_power_up();
#endif // defined(_ENABLE_RFID_SKYETEK_)
	
#if defined(_ENABLE_I2C_)
	sht_power_up(); // Humidity and temp sensor via I2C
#endif // defined(_ENABLE_I2C_)

#if defined(_ENABLE_RTLS_BEACON_)
	rtls_beacon_power_up();
#endif // #if defined(_ENABLE_RTLS_BEACON_)

#if defined(_ENABLE_MICROSTRAIN_)
    microstrain_power_up();
#endif // #if defined(_ENABLE_MICROSTRAIN_)

	
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIOC->BSR = GPIO_Pin_2; // Set pin (1)
	GPIOC->BRR = GPIO_Pin_2; // Reset pin (0)
  
  
	//GPIO_InitTypeDef  GPIO_InitStructure; 
	/* sEE_I2C Peripheral Disable */
	//I2C_Cmd(SC2_I2C, DISABLE);
 	/* _I2C DeInit */
	//I2C_DeInit(SC2_I2C);

	/*!< GPIO configuration */  
 	/*!< Configure _I2C pins: SCL */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Set pull-up on sEE_I2C SCL pin */
	GPIO_SetBits(GPIOA, GPIO_Pin_2);

	/*!< Configure _I2C pins: SDA */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Set pull-up on sEE_I2C SDA pin */
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
 
	TIM2->CCER &= 0xFFFFEEEE; 
  
	//SC_I2C_TypeDef* SCx_I2C;
	I2C_InitTypeDef I2C_InitStruct;	// Create structure for SC2_I2C)
	uint8_t Address = 0x3e; // LCD Address default?? //0x112;
	//uint8_t Data = 0xAA;
	uint8_t contrast = 35;
	
	I2C_StructInit(&I2C_InitStruct); // Initialize SC2_I2C structure with defaults
	I2C_Init(SC2_I2C, &I2C_InitStruct); // Initialize SC2 to work as I2C
	I2C_Cmd(SC2_I2C, ENABLE); // ENABLE or DISABLE SC2_I2C peripheral
	
	I2C_AcknowledgeConfig(SC2_I2C, ENABLE); /// Enable ACK mode

	lcd_cmd ( Address, 0x38 );// 0b0011 1000 ) ; // function set
	lcd_cmd ( Address, 0x39 );// 0b0011 1001 ) ; // function set
	lcd_cmd ( Address, 0x04 );// 0b0000 0100 ) ; // EntryModeSet
	lcd_cmd ( Address, 0x14 );// 0b0001 0100 ) ; // interval osc
	lcd_cmd ( Address, 0x70 | ( contrast & 0xF ) );// 0b0111 0000 | ( contrast & 0xF ) ) ; // contrast Low
	lcd_cmd ( Address, 0x5C | ( ( contrast >> 4 ) & 0x3 ) );// 0b0101 1100 | ( ( contrast >> 4 ) & 0x3 ) ) ; // contast High/icon/power
	lcd_cmd ( Address, 0x6C );// 0b0110 1100 ) ; // follower control
	//delay ( 200 ) ;
	halCommonDelayMilliseconds(200);
	lcd_cmd ( Address, 0x38 );// 0b0011 1000 ) ; // function set
	lcd_cmd ( Address, 0x0C );// 0b0000 1100 ) ; // Display On
	lcd_cmd ( Address, 0x01 );// 0b0000 0001 ) ; // Clear Display
	//delay ( 2 ) ; 
	halCommonDelayMilliseconds(2);	

	// Tryout displau
	lcd_setCursor ( Address, 0 , 0 ) ;
	lcd_printStr ( Address, "SWITCH" ) ; 
	
//	I2C_GenerateSTART(SC2_I2C);
//	I2C_Send7bitAddress(SC2_I2C, Address, I2C_Direction_Transmitter); // I2C_Direction_Transmitter or I2C_Direction_Receiver
//	I2C_SendData(SC2_I2C, Data);
//	Data=I2C_ReceiveData(SC2_I2C);
	
	
	
	/**
  * @brief  Checks whether the specified I2C flag is set or not.
  * @param  SCx_I2C: where x can be 1 or 2 to select the Serial controller peripheral.
  * @param  I2C_FLAG: specifies the SCx_I2C flag to check.
  *   This parameter can be one of the following values:
  *     @arg I2C_FLAG_NACK: Not acknowledge flag
  *     @arg I2C_FLAG_BTF: Byte transfer finished flag
  *     @arg I2C_FLAG_BRF: Byte receive finished flag
  *     @arg I2C_FLAG_CMDFIN: Command finished flag
  * @retval The new state of I2C_FLAG (SET or RESET).
  */
//FlagStatus I2C_GetFlagStatus(SC_I2C_TypeDef* SCx_I2C, uint32_t I2C_FLAG);

//	I2C_GenerateSTOP(SC2_I2C);

}

// Start CMD exchange

#define LCD_CMD_00	0x00
void lcd_cmd ( uint8_t Address, uint8_t x ) {
	I2C_GenerateSTART(SC2_I2C);
	I2C_Send7bitAddress(SC2_I2C, Address, I2C_Direction_Transmitter); // I2C_Direction_Transmitter or I2C_Direction_Receiver
//   Wire . write ( 0b00000000 ) ; // CO = 0,RS = 0
//   Wire . write ( x ) ;
	I2C_SendData(SC2_I2C, LCD_CMD_00);
	I2C_SendData(SC2_I2C, x);
	//Data=I2C_ReceiveData(SC2_I2C);
//   Wire . endTransmission ( ) ;
	I2C_GenerateSTOP(SC2_I2C);
}

#define LCD_DATA_CONT	( 0xC0 )
//( 0b11000000 )
#define LCD_DATA_END	( 0x40 )
//( 0b01000000 )
 void lcd_printStr ( uint8_t Address, const char * s ) {
	I2C_GenerateSTART(SC2_I2C);
	I2C_Send7bitAddress(SC2_I2C, Address, I2C_Direction_Transmitter); // I2C_Direction_Transmitter or I2C_Direction_Receiver
	while ( * s ) {
		if ( * ( s + 1 ) ) {
			//lcd_contdata ( * s ) ;
			I2C_SendData(SC2_I2C, LCD_DATA_CONT);
			I2C_SendData(SC2_I2C, *s);
		} else {
			//lcd_lastdata ( * s ) ;
			I2C_SendData(SC2_I2C, LCD_DATA_END);
			I2C_SendData(SC2_I2C, *s);		}
		s ++ ;
	}
	I2C_GenerateSTOP(SC2_I2C);
} 
/*
void lcd_contdata ( byte x ) {
   Wire . write ( 0b11000000 ) ; // CO = 1, RS = 1
   Wire . write ( x ) ;
}
 
void lcd_lastdata ( byte x ) {
   Wire . write ( 0b01000000 ) ; // CO = 0, RS = 1
   Wire . write ( x ) ;
}
*/
 

// 
void lcd_setCursor ( uint8_t Address, uint8_t x , uint8_t y ) {
   lcd_cmd ( Address, 0x80 | ( y * 0x40 + x ) ) ;
} 

void main_init()
{

  
#ifdef _ENABLE_ARM_ADC_
	aadc_init ();
#endif // _ENABLE_ARM_ADC_

#ifdef _ENABLE_SRC_8BIT_ADC_
	src_8bit_adc_init ();
#endif // _ENABLE_SRC_8BIT_ADC_

#ifdef _ENABLE_SCH_BASIC_
	sch_init();
#endif // _ENABLE_SCH_BASIC_
	
	///////////////////////////////////////////////
#ifdef _ENABLE_APP_MOD_
#ifdef _ENABLE_ADC_MODULE_
	app_suspendADC();
#endif // _ENABLE_ADC_MODULE_
#endif // _ENABLE_APP_MOD_
///////////////////////////////////////////////'

#ifdef _ENABLE_NEM_UTILITIES_01_
	mon_power_init();
	mon_power_test();
#endif

#if defined(_ENABLE_CAMT_HEADSET_01_) || defined(_ENABLE_CAMT_HEADSET_02_)
	hset_init();
#endif // _ENABLE_CAMT_HEADSET_01_

#ifdef _ENABLE_F1_MOTE_TEST_01_
	hw_f1_mote_init ( );
#endif // _ENABLE_F1_MOTE_TEST_01_

#ifdef _ENABLE_ROBOTICS_04_
	vex_tank_init ();
#endif // _ENABLE_ROBOTICS_04_

#ifdef ENABLE_HIO_XBEE
	hio_xbee_init();
#endif // ENABLE_HIO_XBEE

#ifdef FEAT_ENABLE_CH_SWITCHING
	cs_init( );
#endif

#ifdef _ENABLE_LOGISTICS_01_
	lgs_init ( );
#endif // _ENABLE_LOGISTICS_01_


#ifdef _ENABLE_FLA_BASIC_
	fla_init();
#endif // _ENABLE_FLA_BASIC_

#ifndef _TEMP_ARM_
	// Discard any byte received so far
	RI0 = 0;
#endif // _TEMP_ARM_


	// #############################
	// AT commands in a batch test
#ifdef _ENABLE_XBEE_API_
	AT_GT_set();
#endif // _ENABLE_XBEE_API_

	CLEAR_LED(RLED);
	CLEAR_LED(YLED);

#if defined(_ENABLE_RFID_SKYETEK_)
	 rfid_skyetek_init();
#endif // defined(_ENABLE_RFID_SKYETEK_)
	
#if defined(_ENABLE_I2C_)
	sht_init();
#endif // defined(_ENABLE_I2C_)
        
#if defined(_ENABLE_RTLS_BEACON_)
	rtls_beacon_init();
#endif // #if defined(_ENABLE_RTLS_BEACON_)

#if defined(_ENABLE_MICROSTRAIN_)
	microstrain_init();
#endif // #if defined(_ENABLE_MICROSTRAIN_)
	
}




/**
*	main / main_arm - the main function of the Mote code
*/
int main_arm( void )
{

///////////////////////////////////////////////
// Already called in ARM's "main()" function -> "mote.c" file
  //	main_power_up();
///////////////////////////////////////////////

///////////////////////////////////////////////
	main_init();
///////////////////////////////////////////////

#ifndef NO_UART_OUT
//	printf ("Ended initialization; Loop starts now...\r\n");
#endif
	
//Main Loop
///////////////////////////////////////////////
	while ( 1 )
	{
	///////////////////////////////////////////////
		//TOGGLE_LED(RLED);
		// Periodically adjust RTC time drift
		rtc_make_adjustment();
	///////////////////////////////////////////////

	///////////////////////////////////////////////
	//	SCHEDULER main loop function
	//		Handles timeouts and dynamic loop functions
	///////////////////////////////////////////////
	#ifdef _ENABLE_SCH_BASIC_
		sch_loop( );
	#endif // _ENABLE_SCH_BASIC_


	} // while (1)

}
///////////////////////////////////////////////
//End Main Loop
///////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////
//Function Calls








/**
  * release_pkt_in_tx() - releases packet that is being transmited
  *		this includes buffer freeing and reseting "pkt_to_sent_*" variables
  */
void release_pkt_in_tx()
{
	// packet is no longer needed -> free the buffer slot
#ifdef _ENABLE_QUEUING_
	que_make_empty_buffer ( pkt_to_sent_id );
#endif // _ENABLE_QUEUING_
	pkt_to_sent_len = 0;
	pkt_to_sent_id = 0xFF;
}

