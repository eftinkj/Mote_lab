/****************************************************************************
**
** Copyright (C) 2007-2005 Maciej Zawodniok, James W. Fonda, Mohamad Rana Basheer.
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
#ifndef HW_SPECIFIC_H
#define HW_SPECIFIC_H

// ##########################################
// ## General/Environment selection
// ##########################################
//#define QT4
#undef QT4

//#define _STM32W_HIGH_POWER_
#undef _STM32W_HIGH_POWER_

//#define _ENABLE_NEW_ADC_MODULE_

// ### ARM SPECIFIC DEFS ##
// DEFINE if you want to use XBee module for base station (BS_ADDR)
#define _ENABLE_XBEE_COMPAT_4BS_TX_
//#undef _ENABLE_XBEE_COMPAT_4BS_TX_

// DEFINE if you want to use XBee module with non-BS address to receive messgaes
//#define _FORCE_XBEE_COMPAT_TX_
#undef _FORCE_XBEE_COMPAT_TX_


//#define NO_UART_OUT
#undef NO_UART_OUT

// ##########################################
// ## Proc variant selection
// ##########################################

/*
//#define _F310_
#undef _F310_

//#define _F340_
#undef _F340_

//#define _F120_
#undef _F120_

*/

#define _ARM_MOTE_
//#undef _ARM_MOTE_

//#define _UMR_MOTE_F1_
#undef _UMR_MOTE_F1_

// G4
//#define _UMR_MOTE_1_
#undef _UMR_MOTE_1_

// ISN
//#define _UMR_MOTE_2_
#undef _UMR_MOTE_2_

//#define _UMR_MOTE_F340_DEVKIT_
#undef _UMR_MOTE_F340_DEVKIT_

//#define _UMR_MOTE_F120_DEVKIT_
#undef _UMR_MOTE_F120_DEVKIT_

// ##########################################
// Specify Register Bank to be used by UART ISR
// (bank 0 -> default, bank 1, 2, and 3 available but need to be coordinated with other ISRs)
// ##########################################
#define HWM_INT_UART0_BANK	1
#define HWM_INT_RTC_BANK	2

// ##########################################
// ## Hardware version selection
// ##########################################

#ifdef _ARM_MOTE_
	#define _ARM_
	#define XDATA
	#define CODE
	#define REENTRANT
#endif // _ARM_MOTE_

#ifdef _UMR_MOTE_F1_
	#undef _F310_
	#define _F340_
	#undef _F120_
	#define XDATA	xdata
	#define CODE	code
	#define REENTRANT reentrant
#endif // _UMR_MOTE_F1_

#ifdef _UMR_MOTE_1_
	#undef _F310_
	#undef _F340_
	#define _F120_
	#define XDATA	xdata
	#define CODE	code
	#define REENTRANT reentrant
#endif // _UMR_MOTE_1_

#ifdef _UMR_MOTE_2_
	#define _F310_
	#undef _F340_
	#undef _F120_
//	#undef _ENABLE_FLA_BASIC_ // does not work because FEAT included later??)
	#define XDATA	xdata
	#define CODE	code
	#define REENTRANT reentrant
#endif // _UMR_MOTE_2_


#ifdef _UMR_MOTE_F340_DEVKIT_
	#undef _F310_
	#define _F340_
	#undef _F120_
	#define XDATA	xdata
	#define CODE	code
	#define REENTRANT reentrant
#endif // _UMR_MOTE_F340_DEVKIT_

#ifdef _UMR_MOTE_F120_DEVKIT_
	#undef _F310_
	#undef _F340_
	#define _F120_
	#define XDATA	xdata
	#define CODE	code
	#define REENTRANT reentrant
#endif // _UMR_MOTE_F120_DEVKIT_

// ##########################################
// ## Application versions
// ##########################################

//#define _ENABLE_ROBOTICS_01_
#undef _ENABLE_ROBOTICS_01_

// BOT by travis
//#define _ENABLE_ROBOTICS_02_
#undef _ENABLE_ROBOTICS_02_

// BOT by travis
//#define _ENABLE_ROBOTICS_03_
#undef _ENABLE_ROBOTICS_03_

// VEX by travis
//#define _ENABLE_ROBOTICS_04_
#undef _ENABLE_ROBOTICS_04_


#define _ENABLE_NETWORK_ONLY_
//#undef _ENABLE_NETWORK_ONLY_

//#define _ENABLE_NEM_UTILITIES_01_
#undef _ENABLE_NEM_UTILITIES_01_


//#define _ENABLE_CAMT_HEADSET_01_
#undef _ENABLE_CAMT_HEADSET_01_

// ARM based (Dizic DevKit)
//#define _ENABLE_CAMT_HEADSET_02_
//#undef _ENABLE_CAMT_HEADSET_02_
#define HSET_ASCI_UART_ENABLE_CR_FLUSH
//#undef HSET_ASCI_UART_ENABLE_CR_FLUSH
// Use if transparent UART (otherwise VEST DYNAMIC ADDRESSING IS USED)
#define _HSET_FIXED_DST_
//#undef _HSET_FIXED_DST_
// Use if transparent UART (otherwise VEST ADDRESSING IS USED)
#define _HSET_FIXED_DST_
//#undef _HSET_FIXED_DST_

//#define _ENABLE_F1_MOTE_TEST_01_
#undef _ENABLE_F1_MOTE_TEST_01_


//#define _ENABLE_LOGISTICS_01_
#undef _ENABLE_LOGISTICS_01_

// ##########################################
// ## Application dependencies
// ##########################################
// Default setting
#undef _ENABLE_PWM8_MODULE_


#ifdef _ENABLE_NETWORK_ONLY_
	#ifdef _ARM_
		#define _ARM_BASIC_STACK_
	#else // _ARM_
		#define _UMR_BASIC_STACK_
	#endif // else _ARM_
#endif // _ENABLE_NETWORK_ONLY_


// Project/application specific
#ifdef _ENABLE_ROBOTICS_01_
	// Enable PWM8 code
	#define _ENABLE_PWM8_MODULE_
	#define _UMR_BASIC_STACK_
#endif // _ENABLE_ROBOTICS_01_

#ifdef _ENABLE_ROBOTICS_02_
 	// Enable PWM8 code
 	#define _ENABLE_PWM8_MODULE_
#endif // _ENABLE_ROBOTICS_02_

#ifdef _ENABLE_ROBOTICS_03_
 	// Enable PWM8 code
 	#define _ENABLE_PWM8_MODULE_
#endif // _ENABLE_ROBOTICS_03_

#ifdef _ENABLE_ROBOTICS_04_
 	// Enable PWM8 code
 	#define _ENABLE_PWM8_MODULE_
	//  Enable ADC code
	//#define _ENABLE_ADC_MODULE_
	#define _UMR_BASIC_STACK_
#endif // _ENABLE_ROBOTICS_04_

#ifdef _ENABLE_NEM_UTILITIES_01_
	#define	_ENABLE_SPI_MODULE_
//	#define _SPI_REVERSE_LOGIC_
	#define _ENABLE_ADC_MODULE_
#endif // _ENABLE_NEM_UTILITIES_01_


#ifdef _ENABLE_CAMT_HEADSET_01_
//	#define _ENABLE_SPI_MODULE_
// Support for voice encoder via UART1
	#define _ENABLE_UART_PC_
	#undef _ENABLE_UART0_
	#define _ENABLE_UART1_

	#define _UMR_BASIC_STACK_
#endif

#ifdef _ENABLE_CAMT_HEADSET_02_
//	#define _ENABLE_SPI_MODULE_
// Support for voice encoder via UART1
	#undef _ENABLE_UART0_
	#define _ARM_BASIC_STACK_
#endif


#ifdef _ENABLE_F1_MOTE_TEST_01_
	#define _ENABLE_F1_MOTE_BASIC_CONFIG_01_
	#define _UMR_BASIC_STACK_
#endif

#ifdef _ENABLE_LOGISTICS_01_
	#define _ENABLE_RFID_READER_DETECTION_
	#define _UMR_BASIC_STACK_
#endif // _ENABLE_LOGISTICS_01_


// #########################################
// ###  Common configuration sets        ###
// #########################################
#ifdef _UMR_BASIC_STACK_
//	#define	_ENABLE_SPI_MODULE_
	#ifndef _ENABLE_NEW_ADC_MODULE_
		#define _ENABLE_ADC_MODULE_
	#endif // _ENABLE_NEW_ADC_MODULE_
	#define _ENABLE_OLD_UART_
//	#define EXTENDED_UART
//	#define _ENABLE_UART_PC_
//	#define _ENABLE_UART0_
//  #define _ENABLE_UART1_
	#define _ENABLE_XBEE_API_
	#define _ENABLE_OEDSR_
	#define _ENABLE_ROUTING_
	#define _ENABLE_QUEUING_
	#define _ENABLE_APP_MOD_
	#define _ENABLE_NETWORK_STACK_
//	#define _ENABLE_ADC_MODULE_
#endif // _UMR_BASIC_STACK_

#ifdef _ENABLE_F1_MOTE_BASIC_CONFIG_01_
	#define	_ENABLE_SPI_MODULE_
#endif // _ENABLE_F1_MOTE_BASIC_CONFIG_01_

#ifdef _ARM_BASIC_STACK_
//	#define	_ENABLE_SPI_MODULE_
	#ifndef _ENABLE_NEW_ADC_MODULE_
//		#define _ENABLE_ADC_MODULE_
	#endif // _ENABLE_NEW_ADC_MODULE_
//	#define _ENABLE_OLD_UART_
//	#define EXTENDED_UART
//	#define _ENABLE_UART_PC_
//	#define _ENABLE_UART0_
//  #define _ENABLE_UART1_
//	#define _ENABLE_XBEE_API_
	#define _ENABLE_OEDSR_
	#define _ENABLE_ROUTING_
	#define _ENABLE_QUEUING_
	#define _ENABLE_APP_MOD_
	#define _ENABLE_NETWORK_STACK_
//	#define _ENABLE_ADC_MODULE_
#endif // _ARM_BASIC_STACK_

#endif
