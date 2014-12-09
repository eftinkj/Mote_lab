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
//Network Membership Tables
///////////////////////////////////////////////

#ifndef _NETWORK_HARDWARE_ADFS_H_
#define _NETWORK_HARDWARE_ADFS_H_


//#include "common_constants.h"

//#define NODE_EUI8_RTLS_BECON
//#define NODE_EUI8_RTLS_LSENS

//MZ 2014/11/07
////#define NODE_EUI8_RTLS_BS

// Temp sensor node for HB
//#define NODE_EUI8_TEMP

#define NODE_EUI8_MMCR_SSN
//#define NODE_EUI8_MMCR_SHT
//#define NODE_EUI8_MMCR_RELAY

//#define NODE_10_MMCR_SSN
//#define NODE_11_MMCR_SSN
//#define NODE_12_MMCR_SSN
//#define NODE_13_MMCR_SSN
//#define NODE_14_MMCR_SSN

//#define NODE_7_MMCR_SOURCE
// Node 9 has HSET dest. as BS (0xFE) for testing)
//#define NODE_8_MMCR_TEST
//#define NODE_9_MMCR_TEST

//#define NODE_20_LOC_0
//#define NODE_21_LOC_1
//#define NODE_22_LOC_2
//#define NODE_23_LOC_3
//#define NODE_FA_LAPTOP

	// CC
//#define NODE_A_MMCR_VEST
	// CV
//#define NODE_B_MMCR_VEST
	// TR
//#define NODE_C_MMCR_VEST

//#define NODE_10_MMCR_VEST
//#define NODE_FE_MMCR_VEST

//#define NODE_MAG_SRC
//#define NODE_MAG_RELAY
//#define NODE_MAG_PC

//#define NODE_A_MMCR_SSN
//#define NODE_A2F_MMCR_SSN
//#define NODE_A_MMCR_TEST
//#define NODE_B_MMCR_TEST
//#define NODE_B_MMCR_SSN
//#define NODE_C_MMCR_TEST
//#define NODE_D_MMCR_TEST
//#define NODE_E_MMCR_TEST
//#define NODE_F_MMCR_TEST

//#define NODE_A_MMCR_SHT
//#define NODE_B_MMCR_SHT
//#define NODE_C_MMCR_SHT
//#define NODE_D_MMCR_SHT
//#define NODE_E_MMCR_SHT
//#define NODE_F_MMCR_SHT

//#define RSSI_SNIFFING_BS
//#define RSSI_SNIFFER_DOG

//#define ENABLE_ENERGY_SNIFFER


//#######################################################################
//### PCC testing
//#######################################################################

//#define NODE_1_PCC_SOURCE
//#define NODE_2_PCC_SOURCE
//#define NODE_6_PCC_RELAY

//#define NODE_3_PCC_SOURCE
//#define NODE_5_PCC_RELAY

//#define NODE_7_PCC_SOURCE
//#define NODE_4_PCC_SOURCE

//#define NODE_8_PCC_PALETTE
//#define NODE_9_PCC_PALETTE

//#define BOT_1
//#define QRHELO_1

//#define NODE_150_F1_SOURCE


//#define BEAM_REPEAT_DEFAULT_ACTION 1
#define BEAM_REPEAT_DEFAULT_ACTION 0


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RSSI_SNIFFING_BS
	#define _RSSI_SNIFFER_
	#define _HEX_SNIFFER_
	#define XBEE_COMPAT
//	#define SOURCE
//	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif


 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef RSSI_SNIFFER_DOG
	#define _RSSI_SNIFFER_
//	#define SOURCE
//	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif

 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_EUI8_RTLS_BECON
        #define _ENABLE_RTLS_BEACON_
		#define _ENABLE_MICROSTRAIN_
        #define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR 0x14 //my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif


 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_EUI8_TEMP
//	#define _ENABLE_RTLS_LSENS_
	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

//	#define DEFAULT_NODE_WEIGHT 1000
//	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS

	#define ENABLE_LCD_CHAR2x8
#endif



 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_EUI8_RTLS_BECON
        #define _ENABLE_RTLS_BEACON_
		#define _ENABLE_MICROSTRAIN_
        #define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR 0x14 //my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif

 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_EUI8_RTLS_LSENS
//	#define _ENABLE_RTLS_LSENS_
	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif

 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_EUI8_RTLS_BS
//	#define _ENABLE_RTLS_BS_
	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_EUI8_MMCR_RELAY
//	#define SOURCE
	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif


 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_EUI8_MMCR_SHT
	#define SSN_TEST	/* test new source code */
	#define _ENABLE_I2C_

	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE
	#define NODE_2 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

//     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
   #define DEFAULT_MY_CH CLUSTERING_BS
#endif

 //////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_EUI8_MMCR_SSN
	#define SOURCE
	#define SSN_TEST	/* test new source code */
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
//	#define MY_ADDR 0x10
	#define EUI64_TRUNK_ID
	#define MY_ADDR my_addr8_
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif


#ifdef NODE_10_MMCR_SSN
	#define SOURCE
	#define SSN_TEST	/* test new source code */
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x10
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
//	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif


#ifdef NODE_11_MMCR_SSN
	#define SOURCE
	#define SSN_TEST	/* test new source code */
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 1
	#define MY_ADDR 0x11
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
//	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif



#ifdef NODE_12_MMCR_SSN
	//#define SOURCE
	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 2
	#define MY_ADDR 0x12
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
//	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif



#ifdef NODE_13_MMCR_SSN
	//#define SOURCE
	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 3
	#define MY_ADDR 0x13
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
//	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif



#ifdef NODE_14_MMCR_SSN
	//#define SOURCE
	#define SSN_TEST	/* test new source code */
//	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 4
	#define MY_ADDR 0x14
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
//	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif



#ifdef NODE_7_MMCR_SOURCE
	#define SOURCE
	#define SSN_TEST
	#define MY_ADDR 0x07
	#define HSET_DST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 2500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH HSET_DST
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1
#endif

#ifdef NODE_8_MMCR_TEST
	#define SOURCE
	#define MY_ADDR 0x08
	#define HSET_DST 0x09

	#define MY_ADC_CHANNEL 0

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 2500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH HSET_DST
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1
#endif

#ifdef NODE_9_MMCR_TEST
	#define SOURCE
	#define MY_ADDR 0x09
	#define HSET_DST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 2500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH HSET_DST
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1
#endif

//////////////////////////////////////////////////
// RSSI collection support
//////////////////////////////////////////////////


#ifdef NODE_20_LOC_0
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x20
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif // NODE_20_LOC_0


#ifdef NODE_21_LOC_1
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x21
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif // NODE_21_LOC_1


#ifdef NODE_22_LOC_2
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x22
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif // NODE_22_LOC_2


#ifdef NODE_23_LOC_3
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x23
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif // NODE_23_LOC_3


#ifdef NODE_FA_LAPTOP
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0xFA
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS

#endif // NODE_FA_LAPTOP



//////////////////////////////////////////////////
//////////////////////////////////////////////////


#define VEST_CC_ADDR	0x1A
#define VEST_CV_ADDR	0x1B
#define VEST_TR_ADDR	0x1C


#ifdef NODE_A_MMCR_VEST
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR VEST_CC_ADDR
	#define HSET_DST VEST_CV_ADDR

	#define MY_DEST 0xFE
	#define NODE_6 // change for each node

//	#define UART_BAUDRATE	115200

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1
#endif

#ifdef NODE_B_MMCR_VEST
//	#define SOURCE
	#define MY_ADDR VEST_CV_ADDR
	#define HSET_DST VEST_CC_ADDR

	#define MY_DEST 0xFE
	#define NODE_2 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

#ifdef NODE_C_MMCR_VEST
//	#define SOURCE
	#define MY_ADDR VEST_TR_ADDR
	#define HSET_DST 0x0A

	#define MY_DEST 0xFE
	#define NODE_3 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


#ifdef NODE_10_MMCR_VEST
//	#define SOURCE
	#define MY_ADDR 0x10
	#define HSET_DST 0x0A

	#define MY_DEST 0xFE
//	#define NODE_3 // change for each node
	#define TRUE_2HOP_MMCR

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif



#define MAG_RELAY_ADDR	0x1A
#define MAG_PC_ADDR	0x1B
#define MAG_SRC_ADDR	0x1C

#ifdef NODE_MAG_SRC
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0xFE
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
//	#define NODE_6 // change for each node

	#ifdef UART_BAUDRATE
		#undef UART_BAUDRATE
	#endif //UART_BAUDRATE
	#define UART_BAUDRATE	1200

#error #define _DUMMY_UART_TX_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS
#endif



#ifdef NODE_MAG_RELAY
	#define MY_ADDR MAG_RELAY_ADDR
	#define HSET_DST MAG_PC_ADDR

	#define MY_DEST 0xFE
//	#define NODE_2 // change for each node

	#ifdef UART_BAUDRATE
		#undef UART_BAUDRATE
	#endif //UART_BAUDRATE
	#define UART_BAUDRATE	1200

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS
#endif


#ifdef NODE_MAG_PC
	#define MY_ADDR MAG_PC_ADDR
	#define HSET_DST MAG_RELAY_ADDR

	#define MY_DEST 0xFE
//	#define NODE_3 // change for each node

//	#define UART_BAUDRATE	115200

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS
#endif











#ifdef NODE_A2F_MMCR_SSN
	#define SOURCE
	#define SSN_TEST	/* test new source code */
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x0A
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	//#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
     #define DEFAULT_MY_CH 0x0F
//   #define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


#ifdef NODE_A_MMCR_SSN
	#define SOURCE
	#define SSN_TEST	/* test new source code */
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x0A
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	//#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
//	#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif




#ifdef NODE_A_MMCR_TEST
	#define SOURCE
	#define SSN_TEST	/* test new source code */
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x0A
	#define HSET_DST 0x0B

	#define MY_DEST 0xFE
	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
//	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


#ifdef NODE_B_MMCR_SSN
//	#define SOURCE
	#define MY_ADDR 0x0B
	#define HSET_DST 0xFE
	#define MY_DEST 0xFE
	#define NODE_2 // change for each node

	#define SOURCE
	#define SSN_TEST	/* test new source code */
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0

//	#define MY_ADDR 0x0A
//	#define HSET_DST 0xFE

//	#define MY_DEST 0xFE
//	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1
#endif // NODE_B_MMCR_SSN


#ifdef NODE_B_MMCR_TEST
//	#define SOURCE
	#define MY_ADDR 0x0B
	#define HSET_DST 0x0A

	#define MY_DEST 0xFE
	#define NODE_2 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


#ifdef NODE_C_MMCR_TEST
//	#define SOURCE
	#define MY_ADDR 0x0C
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_3 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

#ifdef NODE_D_MMCR_TEST
//	#define SOURCE
	#define MY_ADDR 0x0D
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_4 // change for each node
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1800
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

#ifdef NODE_E_MMCR_TEST
	#define SOURCE
	#define SSN_TEST	/* test new source code */
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 1

	#define MY_ADDR 0x0E
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
//	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


///////////////////////////////////////////////
#ifdef NODE_F_MMCR_TEST
	#define SOURCE
	#define SSN_TEST	/* test new source code */
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 2

	#define MY_ADDR 0x0F
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_5 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
//	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_A_MMCR_SHT
	//#define SOURCE
	#define SSN_TEST	/* test new source code */
	#define _ENABLE_I2C_
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	//#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 0
	#define MY_ADDR 0x0A
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	//#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
//	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
   //#define DEFAULT_MY_CH 0x0B
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//  #define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

#ifdef NODE_B_MMCR_SHT
//	#define SOURCE
	#define MY_ADDR 0x0B
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_2 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif
#ifdef NODE_C_MMCR_SHT
//	#define SOURCE
	#define MY_ADDR 0x0C
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_3 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

#ifdef NODE_D_MMCR_SHT
//	#define SOURCE
	#define MY_ADDR 0x0D
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_4 // change for each node
	#define MY_ADC_CHANNEL 3
	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1800
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

#ifdef NODE_E_MMCR_SHT
	//#define SOURCE
	#define SSN_TEST	/* test new source code */
    #define _ENABLE_I2C_
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	//#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 1

	#define MY_ADDR 0x0E
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_6 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
//	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


#ifdef NODE_F_MMCR_SHT
	//#define SOURCE
	#define SSN_TEST	/* test new source code */
    #define _ENABLE_I2C_
	//#define _ENABLE_SRC_DUMMY_
	//#define _ENABLE_ARM_ADC_
	//#define _ENABLE_SRC_8BIT_ADC_
	#define MY_ADC_CHANNEL 2

	#define MY_ADDR 0x0F
	#define HSET_DST 0xFE

	#define MY_DEST 0xFE
	#define NODE_5 // change for each node

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
//	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
     #define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//   #define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#ifdef NODE_150_F1_SOURCE
	#define SOURCE
	#define MY_ADDR 150

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 2500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif






#ifdef BOT_1
	// #define SOURCE
	#undef SOURCE
	#define MY_ADDR 6

	#define DEFAULT_NODE_WEIGHT 2000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define NEXT_HOP 0xFE
	#define DEFAULT_DISTANCE 1000
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
	
	//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


/////////////////////////////////////////////////////////////
#ifdef QRHELO_1
	// #define SOURCE
	#undef SOURCE
	#define MY_ADDR 7

	#define DEFAULT_NODE_WEIGHT 2000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define NEXT_HOP 0xFE
	#define DEFAULT_DISTANCE 1000
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
	
	//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


///////////////////////////////////////////////

#ifdef NODE_1_PCC_SOURCE
	#define SOURCE
	#define MY_ADDR 1

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 2500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
	//#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE

//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH 6
	
	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

///////////////////////////////////////////////

#ifdef NODE_2_PCC_SOURCE
	#define SOURCE
	#define MY_ADDR 0x02

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 2500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
	//#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH 6

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


///////////////////////////////////////////////

#ifdef NODE_3_PCC_SOURCE
	#define SOURCE
	#define MY_ADDR 0x03

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 2500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	#undef BS_NEIGHBOR
	//#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH 5

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


///////////////////////////////////////////////

#ifdef NODE_7_PCC_SOURCE
	#define SOURCE
	#define MY_ADDR 7

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1000
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif

///////////////////////////////////////////////

#ifdef NODE_4_PCC_SOURCE
	#define SOURCE
	#define MY_ADDR 4

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


#ifdef NODE_8_PCC_PALETTE
	#undef SOURCE
	#define MY_ADDR 8

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


#ifdef NODE_9_PCC_PALETTE
	#undef SOURCE
	#define MY_ADDR 9

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
//	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


#ifdef NODE_5_PCC_RELAY
	// #define SOURCE
	#undef SOURCE
	#define MY_ADDR 5

	#define DEFAULT_NODE_WEIGHT 2000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define NEXT_HOP 0xFE
	#define DEFAULT_DISTANCE 700
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH 6

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif

#ifdef NODE_6_PCC_RELAY
	// #define SOURCE
	#undef SOURCE
	#define MY_ADDR 6

	#define DEFAULT_NODE_WEIGHT 2000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define NEXT_HOP 0xFE
	#define DEFAULT_DISTANCE 800
	//#define DEFAULT_ENERGY 0x000186A0
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE
	/*! it IS  a niegbor of the BS */
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

//	#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
	//#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif



//#######################################################################
//### ADFS testing
//#######################################################################


//#define NODE_20_LAST_RELAY
//#define NODE_1_ADFS_SOURCE
//#define NODE_2_ADFS_SOURCE
//#define NODE_7_ADFS_SOURCE
//#define NODE_3_ADFS_SOURCE
//#define NODE_5_ADFS_RELAY
//#define NODE_6_ADFS_RELAY

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

//#define NODE_2_SOURCE_TEST
//#define NODE_A
//#define NODE_9
//#define NODE_9
//#define NEXT_HOP 6
//#define NEXT_HOP 5
//#define NODE_1_SOURCE
//#define NODE_2_SOURCE
//#define NODE_40_SOURCE
//#define NODE_40_SOURCE_BS

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////











///////////////////////////////////////////////

#ifdef NODE_1_ADFS_SOURCE
	#define SOURCE
	#define MY_ADDR 1

	#define DEFAULT_NODE_WEIGHT 200
	#define DEFAULT_SOURCE_WEIGHT 200

	// BASE STATION = 0xFE, CH = 0xA
	//#define NEXT_HOP 0xFE
	// #define NEXT_HOP 0xA
	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY 0x000186A0
	// it IS  a niegbor of the BS
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

///////////////////////////////////////////////

#ifdef NODE_2_ADFS_SOURCE
	#define SOURCE
	#define MY_ADDR 0x02

	#define DEFAULT_NODE_WEIGHT 200
	#define DEFAULT_SOURCE_WEIGHT 200

	// BASE STATION = 0xFE, CH = 0xA
	//#define NEXT_HOP 0xFE
	// #define NEXT_HOP 0x6
	//#define NEXT_HOP ADFS_NEXT_HOP
	#define DEFAULT_DISTANCE 2000
	#define DEFAULT_ENERGY 0x000186A0
	// it IS  a niegbor of the BS
	#undef BS_NEIGHBOR
	//#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif

#ifdef NODE_3_ADFS_SOURCE
	#define SOURCE
	#define MY_ADDR 3

	#define DEFAULT_NODE_WEIGHT 200
	#define DEFAULT_SOURCE_WEIGHT 200

	// BASE STATION = 0xFE, CH = 0xA
	//#define NEXT_HOP 0xFE
	// #define NEXT_HOP 0xA
	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY 0x000186A0
	// it IS  a niegbor of the BS
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 1

#endif


#ifdef NODE_7_ADFS_SOURCE
	#define SOURCE
	#define MY_ADDR 7

	#define DEFAULT_NODE_WEIGHT 200
	#define DEFAULT_SOURCE_WEIGHT 200

	// BASE STATION = 0xFE, CH = 0xA
	//#define NEXT_HOP 0xFE
	// #define NEXT_HOP 0xA
	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY 0x000186A0
	// it IS  a niegbor of the BS
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
	
	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


#ifdef NODE_5_ADFS_RELAY
	// #define SOURCE
	#undef SOURCE
	#define MY_ADDR 5

	#define DEFAULT_NODE_WEIGHT 600
	#define DEFAULT_SOURCE_WEIGHT 200

	// BASE STATION = 0xFE, CH = 0xA
	#define NEXT_HOP 0xFE
	// #define NEXT_HOP 0xA
	#define DEFAULT_DISTANCE 1000
	#define DEFAULT_ENERGY 0x000186A0
	// it IS  a niegbor of the BS
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
	
	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


#ifdef NODE_6_ADFS_RELAY
	// #define SOURCE
	#undef SOURCE
	#define MY_ADDR 6

	#define DEFAULT_NODE_WEIGHT 600
	#define DEFAULT_SOURCE_WEIGHT 200

	// BASE STATION = 0xFE, CH = 0xA
	#define NEXT_HOP 0xFE
	// #define NEXT_HOP 0xA
	#define DEFAULT_DISTANCE 1000
	#define DEFAULT_ENERGY 0x000186A0
	// it IS  a niegbor of the BS
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif















































//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_1_SOURCE
#define SOURCE
#define MY_ADDR 1

#define DEFAULT_NODE_WEIGHT 200
#define DEFAULT_SOURCE_WEIGHT 200

#define DEFAULT_DISTANCE 1500
#define DEFAULT_ENERGY 0x000186A0
// it IS  a niegbor of the BS
//#undef BS_NEIGHBOR
#define BS_NEIGHBOR

//#undef CH_START_ROUTE
#define CH_START_ROUTE
#endif

///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_2_SOURCE
#define SOURCE
#define MY_ADDR 2

#define DEFAULT_NODE_WEIGHT 200
#define DEFAULT_SOURCE_WEIGHT 200

#define DEFAULT_DISTANCE 1000
#define DEFAULT_ENERGY 0x000186A0
// it IS  a niegbor of the BS
#undef BS_NEIGHBOR
//#define BS_NEIGHBOR

//#undef CH_START_ROUTE
#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_2_SOURCE_TEST
#define SOURCE
#define MY_ADDR 2

#define DEFAULT_NODE_WEIGHT 200
#define DEFAULT_SOURCE_WEIGHT 200

#define DEFAULT_DISTANCE 2500
#define DEFAULT_ENERGY 0x000186A0
// it IS  a niegbor of the BS
#undef BS_NEIGHBOR
//#define BS_NEIGHBOR

//#undef CH_START_ROUTE
#define CH_START_ROUTE
#endif

///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_40_SOURCE
#define SOURCE
#define MY_ADDR 40

#define DEFAULT_NODE_WEIGHT 200
#define DEFAULT_SOURCE_WEIGHT 200

#define DEFAULT_DISTANCE 2500
#define DEFAULT_ENERGY 0x000186A0
// it IS  a niegbor of the BS
#undef BS_NEIGHBOR
//#define BS_NEIGHBOR

//#undef CH_START_ROUTE
#define CH_START_ROUTE
#endif

///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_40_SOURCE_BS
#define SOURCE
#define MY_ADDR 40

#define DEFAULT_NODE_WEIGHT 200
#define DEFAULT_SOURCE_WEIGHT 200

#define DEFAULT_DISTANCE 1000
#define DEFAULT_ENERGY 0x000186A0
// it IS  a niegbor of the BS
//#undef BS_NEIGHBOR
#define BS_NEIGHBOR

//#undef CH_START_ROUTE
#define CH_START_ROUTE
#endif


#endif

