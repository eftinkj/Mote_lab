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

#ifndef _FEAT_ENABLE_H_
#define _FEAT_ENABLE_H_

/**
  * Defines COMPILE-TIME configuration of code
  */

#define _ENABLE_MAIN_
//#undef _ENABLE_MAIN_

// #################################################
// ### SCHeduler for tasks and timeouts specific defines
// #################################################
#define _ENABLE_SCH_BASIC_
//#undef _ENABLE_SCH_BASIC_


//#define _ENABLE_SSN_BASIC_
#undef _ENABLE_SSN_BASIC_

#define _ENABLE_SSN_8BIT_
//#undef _ENABLE_SSN_8BIT_

// Use old format of ADC data packets
#define SSN_WITH_RAW_FORMAT
//#undef SSN_WITH_RAW_FORMAT

#if defined(_ENABLE_SSN_8BIT_) & defined(_ENABLE_SSN_BASIC_)
#error "Can only have one: SSN_BASIC or SSN_8BIT"
#endif


//#define _ENABLE_SRC_8BIT_ADC_
//#undef _ENABLE_SRC_8BIT_ADC_

//#define _ENABLE_SRC_DUMMY_
#undef _ENABLE_SRC_DUMMY_

//#define _SYS_ERROR_LOCKING_LED_
#undef _SYS_ERROR_LOCKING_LED_



#define ENABLE_UART
#define UART_BAUDRATE	115200
//#undef ENABLE_UART

// Enable/disable support for string format for Intel HEX  (default binary which is much faster!!)
//#define _ENABLE_FLA_STRING_INTEL_HEX_
#undef _ENABLE_FLA_STRING_INTEL_HEX_

#ifdef _F120_
	#define _ENABLE_FLA_BASIC_
	//#undef _ENABLE_FLA_BASIC_
#else
	// always undefine the Flash Access/Replicator
	#undef _ENABLE_FLA_BASIC_
#endif

// #################################################
// ### AODV routing specific defines
// #################################################
#define FEAT_ENABLE_AODV
//#undef FEAT_ENABLE_AODV

#define FEAT_ENABLE_MMCR
//#undef FEAT_ENABLE_MMCR
// Should we add all one-hop neighbors (when msg received)
#define MMCR_ENABLE_TRUE_NEIGHBORS

// #################################################
// ### Queueing specific defines
// #################################################
//#define FEAT_ENABLE_SFQ
#undef FEAT_ENABLE_SFQ


// #################################################
// ### Congestion Control specific defines
// #################################################
//#define FEAT_ENABLE_CC
#undef FEAT_ENABLE_CC

#ifdef FEAT_ENABLE_CC
	#define INCLUDE_PCC

	#define PCC_DEBUG_UPSTREAM_MSG
	//#undef PCC_DEBUG_UPSTREAM_MSG

	#define ENABLE_CC_DEBUG_COUNTERS
	//#undef ENABLE_CC_DEBUG_COUNTERS

#endif

// #################################################
// ### Channel Switching specific defines
// #################################################
#define FEAT_ENABLE_CH_SWITCHING
//#undef FEAT_ENABLE_CH_SWITCHING


// Enables High-level IO interface for XBee radio modules
#define ENABLE_HIO_XBEE
//#undef ENABLE_HIO_XBEE

// Enables the procedure to fix the XBee radio configuration (baudrate, API mode etc)
//#define FEAT_ENABLE_XBEE_RECOVERY_CONFIG
#undef FEAT_ENABLE_XBEE_RECOVERY_CONFIG


// Enables cycle count for ADC0
//#define ADC_ENABLE_CYCLE_COUNT
#undef ADC_ENABLE_CYCLE_COUNT



//#define _ENABLE_RFID_READER_DETECTION_
#undef _ENABLE_RFID_READER_DETECTION_


//#define _ENABLE_DLS_GATE_MOTE_
#undef _ENABLE_DLS_GATE_MOTE_


//#define _ENABLE_DLS_TRUCK_MOTE_
#undef _ENABLE_DLS_TRUCK_MOTE_

//#define _ENABLE_SIG_BASIC_
#undef _ENABLE_SIG_BASIC_

//#define _ENABLE_I2C_

// Enable App to read temperature over 1-wire Dallas interface
#define _ENABLE_APP_TEMP_1WIRE_
//#undef _ENABLE_APP_TEMP_1WIRE_
	  
//#define _ENABLE_LCD_CHAR_2x8_
#undef _ENABLE_LCD_CHAR_2x8_
	
#endif

