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

#ifndef _NETWORK_HARDWARE_LGS_H_
#define _NETWORK_HARDWARE_LGS_H_


//#include "common_constants.h"

//#######################################################################
//### PCC testing
//#######################################################################

//#define NODE_LGS_GATE_MAIN
//#define NODE_LGS_GATE_DP1
//#define NODE_LGS_GATE_DP2
//#define NODE_LGS_GATE_DP3
//
//#define NODE_LGS_TRUCK_1
//#define NODE_LGS_TRUCK_2
//#define NODE_LGS_TRUCK_3

//#define NODE_LGS_PALETTE_1
//#define NODE_LGS_PALETTE_2
//#define NODE_LGS_PALETTE_3



#define LGS_MOTE_TYPE_DP		0x01
#define LGS_MOTE_TYPE_TRUCK		0x02
#define LGS_MOTE_TYPE_PALETTE	0x03
#define LGS_MOTE_TYPE_RFID		0x04

#define BEAM_REPEAT_DEFAULT_ACTION 1

#define MAIN_WAREHOUSE_ID	1
#define DP1_ID	2
#define DP2_ID	3
#define DP3_ID	4

#define TRUCK1_ID	1
#define TRUCK2_ID	2
#define TRUCK3_ID	3

#define PALETTE_1_ID	1
#define PALETTE_2_ID	2
#define PALETTE_3_ID	3

#define LGS_TO_NET_ID(lgs_node_id, lgs_node_type) (lgs_node_id + (16*lgs_node_type))
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
  

#undef BS_NEIGHBOR

///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_GATE_MAIN
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_DP
	#define LGS_NODE_ID		MAIN_WAREHOUSE_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#undef _ENABLE_RFID_READER_DETECTION_
	#define _ENABLE_DLS_GATE_MOTE_
	#undef _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_GATE_DP1
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_DP
	#define LGS_NODE_ID		DP1_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#undef _ENABLE_RFID_READER_DETECTION_
	#define _ENABLE_DLS_GATE_MOTE_
	#undef _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_GATE_DP1___ccc
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_DP
	#define LGS_NODE_ID		DP1_ID_2
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#undef _ENABLE_RFID_READER_DETECTION_
	#define _ENABLE_DLS_GATE_MOTE_
	#undef _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

//	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_GATE_DP2
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_DP
	#define LGS_NODE_ID		DP2_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#undef _ENABLE_RFID_READER_DETECTION_
	#define _ENABLE_DLS_GATE_MOTE_
	#undef _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif



///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_GATE_DP3
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_DP
	#define LGS_NODE_ID		DP3_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#undef _ENABLE_RFID_READER_DETECTION_
	#define _ENABLE_DLS_GATE_MOTE_
	#undef _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

//	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif



///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_TRUCK_1
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_TRUCK
	#define LGS_NODE_ID		TRUCK1_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#undef _ENABLE_RFID_READER_DETECTION_
	#undef _ENABLE_DLS_GATE_MOTE_
	#define _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

//	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_TRUCK_2
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_TRUCK
	#define LGS_NODE_ID		TRUCK2_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#undef _ENABLE_RFID_READER_DETECTION_
	#undef _ENABLE_DLS_GATE_MOTE_
	#define _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

	#define BS_NEIGHBOR
//	#undef BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif



///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_TRUCK_3
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_TRUCK
	#define LGS_NODE_ID		TRUCK3_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#undef _ENABLE_RFID_READER_DETECTION_
	#undef _ENABLE_DLS_GATE_MOTE_
	#define _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

//	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif



///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_PALETTE_1
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_PALETTE
	#define LGS_NODE_ID		PALETTE_1_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#define _ENABLE_RFID_READER_DETECTION_
	#undef _ENABLE_DLS_GATE_MOTE_
	#undef _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

//	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_PALETTE_2
	#define SOURCE

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_PALETTE
	#define LGS_NODE_ID		PALETTE_2_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#define _ENABLE_RFID_READER_DETECTION_
	#undef _ENABLE_DLS_GATE_MOTE_
	#undef _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif


///////////////////////////////////////////////
///////////////////////////////////////////////
#ifdef NODE_LGS_PALETTE_3
	#define SOURCE
	#define MY_ADDR 22

	#define LGS_NODE_TYPE	LGS_MOTE_TYPE_PALETTE
	#define LGS_NODE_ID		PALETTE_3_ID
	#define MY_ADDR LGS_TO_NET_ID(LGS_NODE_ID, LGS_NODE_TYPE)

	#define _ENABLE_RFID_READER_DETECTION_
	#undef _ENABLE_DLS_GATE_MOTE_
	#undef _ENABLE_DLS_TRUCK_MOTE_

	#define DEFAULT_NODE_WEIGHT 1000
	#define DEFAULT_SOURCE_WEIGHT 500

	#define DEFAULT_DISTANCE 1500
	#define DEFAULT_ENERGY ENERGY_DISABLE_VALUE

//	#define BS_NEIGHBOR
	#define CH_START_ROUTE
	
	#define DEFAULT_MY_CH CLUSTERING_I_AM_CH
//	#define DEFAULT_MY_CH CLUSTERING_BS

	// Number of upstream sources (including myself)
	#define PCC_JNL_NUMBER_OF_UPSTREAM 2

#endif



///////////////////////////////////////////////





#endif // _NETWORK_HARDWARE_LGS_H_

