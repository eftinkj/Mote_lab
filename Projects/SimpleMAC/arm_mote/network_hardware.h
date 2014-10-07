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

//#define NODE_2
//#define NODE_3
//#define NODE_4
//#define NODE_5
//#define NODE_6
//#define NODE_7
//#define NODE_8
//#define NODE_9
//#define NODE_10
//#define NODE_11
//#define NODE_12
//#define NODE_13
//#define NODE_14
//#define NODE_15
//#define NODE_16
//#define NODE_17
//#define NODE_18
//#define NODE_19
//#define NODE_20
//#define NODE_21

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
//#define NODE_20_LAST_RELAY
//#define NODE_1_ADFS_SOURCE
//#define NODE_2_ADFS_SOURCE
//#define NODE_3_ADFS_SOURCE
//#define NODE_5_ADFS_RELAY
//#define NODE_6_ADFS_RELAY




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

///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NODE_20_LAST_RELAY
#undef SOURCE
#define MY_ADDR 20

#define DEFAULT_NODE_WEIGHT 200
#define DEFAULT_SOURCE_WEIGHT 200

#define DEFAULT_DISTANCE 200
#define DEFAULT_ENERGY 0x000186A0
// it IS  a niegbor of the BS
//#undef BS_NEIGHBOR
#define BS_NEIGHBOR

#undef CH_START_ROUTE
//#define CH_START_ROUTE
#endif

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
	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY 0x000186A0
	// it IS  a niegbor of the BS
	//#undef BS_NEIGHBOR
	#define BS_NEIGHBOR

	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
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
// The ISN is at 2500 in the default profile for testing
#ifdef NODE_2
	#undef SOURCE
	#define MY_ADDR 0x02

	//	#define NEXT_HOP 0xFE
	// #define PREVIOUS_HOP 1
	#define DEFAULT_DISTANCE 2400
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
#endif

///////////////////////////////////////////////
#ifdef NODE_3
	#undef SOURCE
	#define MY_ADDR 0x03

	#define DEFAULT_DISTANCE 2100
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_4
	#undef SOURCE
	#define MY_ADDR 0x04

	#define DEFAULT_DISTANCE 2100
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_5
	#undef SOURCE
	#define MY_ADDR 0x05

	#define DEFAULT_DISTANCE 2100
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_6
	#undef SOURCE
	#define MY_ADDR 0x06

	#define DEFAULT_DISTANCE 1800
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_7
	#undef SOURCE
	#define MY_ADDR 0x07

	#define DEFAULT_DISTANCE 1800
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_8
	#undef SOURCE
	#define MY_ADDR 0x08

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_9
	#undef SOURCE
	#define MY_ADDR 0x09

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_A
	#undef SOURCE
	#define MY_ADDR 0x0A

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_10
	#undef SOURCE
	#define MY_ADDR 0x10

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_11
	#undef SOURCE
	#define MY_ADDR 0x11

	#define DEFAULT_DISTANCE 1200
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_12
	#undef SOURCE
	#define MY_ADDR 0x12

	#define DEFAULT_DISTANCE 1200
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_13
	#undef SOURCE
	#define MY_ADDR 0x13

	#define DEFAULT_DISTANCE 900
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_14
	#undef SOURCE
	#define MY_ADDR 0x14
	#define DEFAULT_DISTANCE 900
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_15
	#undef SOURCE
	#define MY_ADDR 0x15

	#define DEFAULT_DISTANCE 900
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_16
	#undef SOURCE
	#define MY_ADDR 0x16

	#define DEFAULT_DISTANCE 600
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_17
	#undef SOURCE
	#define MY_ADDR 0x17

	#define DEFAULT_DISTANCE 600
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_18
	#undef SOURCE
	#define MY_ADDR 0x18

	#define DEFAULT_DISTANCE 300
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	#define BS_NEIGHBOR
	// #undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_19
	#undef SOURCE
	#define MY_ADDR 0x19

	#define DEFAULT_DISTANCE 300
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	#define BS_NEIGHBOR
	// #undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif
///////////////////////////////////////////////
#ifdef NODE_20
	#undef SOURCE
	#define MY_ADDR 0x20

	#define DEFAULT_DISTANCE 300
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	#define BS_NEIGHBOR
	// #undef BS_NEIGHBOR
	#undef CH_START_ROUTE
	//#define CH_START_ROUTE
#endif

///////////////////////////////////////////////
//Second CH Node for JRNL Paper
#ifdef NODE_21
	#undef SOURCE
	#define MY_ADDR 0x021
	//#define NEXT_HOP 10

	#define DEFAULT_DISTANCE 2400
	#define DEFAULT_ENERGY 0x000186A0
	// it IS NO a niegbor of the BS
	//#define BS_NEIGHBOR
	#undef BS_NEIGHBOR
	//#undef CH_START_ROUTE
	#define CH_START_ROUTE
#endif


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

//Network Membership Tables
///////////////////////////////////////////////
//#define NODE_2
//#define NODE_3
//#define NODE_4
//#define NODE_5
//#define NODE_6
//#define NODE_7
//#define NODE_8
//#define NODE_9
//#define NODE_10
//#define NODE_11
//#define NODE_12
//#define NODE_13

//#define NODE_14
//#define NODE_15


///////////////////////////////////////////////
/*
#ifdef NODE_XX
// SOURCE tells is the node should generate data
 #undef SOURCE
// MY_ADDR defines what address the node uses
 #define MY_ADDR 1
// This is actually not needed
 #define NODE_ID_STR "0001"
// neither does this one (pre-routing info)
 #define NEXT_HOP 2
 // and neither does theic one ----- sensor node does not have previous node
 #define PREVIOUS_HOP -1
// this one is important when the RSSI reading is not used....
 #define DEFAULT_DISTANCE 1200
#endif
*/
///////////////////////////////////////////////


