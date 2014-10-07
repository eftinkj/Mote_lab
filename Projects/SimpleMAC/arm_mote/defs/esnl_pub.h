/****************************************************************************
**
** Copyright (C) 2007-2005 Maciej Zawodniok, James W. Fonda, Mohamad Rana Basheer.
**  All rights reserved.
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
#ifndef _ESNL_PUB_H_
#define _ESNL_PUB_H_



#include "hw_specific.h"
#include "feat_enable.h"

//#include 'network_hardware.h"
#ifdef _ENABLE_LOGISTICS_01_
	#include "network_hardware_LGS.h"
#else // not _ENABLE_LOGISTICS_01_
	#include "network_hardware_ADFS.h"
#endif // else/not _ENABLE_LOGISTICS_01_

#ifndef WIN32
//	#define RAND_MAX			32767
	#define TASK_FUNC			reentrant
	#define _NO_INIT_
	#define _CODE_SPACE_		code
	#define _EXT_MEM_			xdata
#else
	//do not pack any structures
	#pragma pack(1)

	#define TASK_FUNC		
	#define _NO_INIT_
	#define _CODE_SPACE_		const
	#define _EXT_MEM_		
#endif

#ifndef QT4


#include <stdio.h>


#ifdef _ARM_
	// Temporary fixes to code for ARM porting - NEED TO BE REMOVED/FIXED SOON!
	#define _TEMP_ARM_
#include PLATFORM_HEADER
//#include BOARD_HEADER
#include "hal.h"
//#include "hal/micro/cortexm3/uart.h"
//#include "include/phy-library.h"
#include "error.h"
//#include "hal/micro/cortexm3/memmap.h"

#ifdef _ARM_DIZIC_
//        #define RLED LED_D1
//        #define YLED LED_D3
        #define RLED LED1
        #define YLED LED3
#endif // _ARM_DIZIC_
//#ifdef _ARM_X1_
		#define RLED LED1
		#define YLED LED3
//#endif // _ARM_X1_
        #define TOGGLE_LED(l) STM_EVAL_LEDToggle(l)
        #define SET_LED(l) STM_EVAL_LEDOn(l)
        #define CLEAR_LED(l) STM_EVAL_LEDOff(l)
//halToggleLed(l)
//        #define SET_LED(l) halSetLed(l)
//        #define CLEAR_LED(l) halClearLed(l)

#else // _ARM_
        #define TOGGLE_LED(l) (l=~l)
        #define SET_LED(l)    (l=1)
        #define CLEAR_LED(l)  (l=0)
#ifdef _F310_
	#include "c8051F310.h"
	#include "const_proc_F310.h"
#else
	#ifdef _F340_
		#include "c8051F340.h"
		#include "const_proc_F340.h"
	#else
		#include "c8051F120.h"
		#include "const_proc_F120.h"
	#endif // _F340_ else
#endif // _F310_ else
#endif // else _ARM_


#ifdef _UMR_MOTE_F1_
	#include "const_mote_f1.h"
#endif // _UMR_MOTE_F1_




	//#define __SDCC__
	//#define __KEIL__
	#ifndef __KEIL__
		#define __SDCC__
	#endif

//	#define TASK_FUNC		reentrant

#ifdef _ARM_
//	#include "stm32w108_type.h"
	//typedef unsigned long 	uint32_t;
	//typedef long 			sint32_t;
	//typedef long 			int32_t;
	//typedef unsigned short 	uint16_t;
	//typedef signed short	sint16_t;
	//typedef short 			int16_t;
#else // _ARM_
	typedef unsigned long 	uint32_t;
	typedef long 			sint32_t;
	typedef long 			int32_t;
	typedef unsigned int 	uint16_t;
	typedef int 			sint16_t;
	typedef int 			int16_t;
#endif // else _ARM_
//	typedef unsigned char	uint8_t;
	typedef char			sint8_t;
	//typedef char			int8_t;
#ifndef _ARM_
	typedef uint8_t			boolean;
#endif // not _ARM_
//	typedef unsigned char			bool;

	#define BYTE_SIZE_IN_BITS	8

	#ifndef true
		#define true 1
	#endif
	#ifndef false
		#define false 0
	#endif

     #ifndef _ARM_

	#ifdef DEBUG
		#define assert(set)																							\
		{																																\
		if(!set)																												\
			{																															\
			printf("Assert at line: %s on file %s\n",__FILE__, __LINE__);	\
			while(1);																											\
			}																															\
		}
	#else
		#define assert(set)
	#endif
    #endif // no _ARM_
	#define assert_always() assert(0)

	#ifdef __SDCC__
		#define FLIP_BIT(b)	(1-b)
		#define SWAP16(x) (( (x >> 8)  & 0x00ff)  | ( (x << 8)  & 0xff00) )
		#define SWAP32(x) (((x >> 24) & 0x000000ff)  | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000) | ((x << 24)  & 0xff000000) )

	#endif // __SDCC__
	#ifdef __KEIL__
		#define FLIP_BIT(b)	(~b)
		#define SWAP16(x) (x)
		#define SWAP32(x) (x)
	#endif // __KEIL__

#ifdef _ARM_
		#define ENDIAN16(a)	SWAP16(a)
		#define ENDIAN32(a) SWAP32(a)
#else // _ARM_
		#define	ENDIAN16(a)	a
		#define ENDIAN32(a) a
#endif // else _ARM_

	#define PACKED
#ifdef _ARM_
	#define IAR_PACKED __packed
#else // _ARM_
	#define IAR_PACKED
#endif // else _ARM_
	
#else // ! QT4

	#define PACKED __attribute__((packed))
	#define IAR_PACKED

#endif // !QT4 else

#endif //_ESNL_DEFINES_H_

