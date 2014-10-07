//-----------------------------------------------------------------------------
// F200_FlashPrimitives.h
//-----------------------------------------------------------------------------
// Copyright 2004 Silicon Laboratories, Inc.
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// AUTH: BW & GP
// DATE: 21 JUL 04
//
// Target: C8051F2xx
// Tool chain: KEIL C51 7.06
//
// Release 1.0
//

#ifndef F200_FLASHPRIMITIVES_H
#define F200_FLASHPRIMITIVES_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef unsigned char UCHAR;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#ifndef SYSCLK
#define SYSCLK 16000000L
#endif

#ifndef FLASHSCALE

#if   (SYSCLK < 50000L)
#define FLASHSCALE 0x80
#elif (SYSCLK < 100000L)
#define FLASHSCALE 0x81
#elif (SYSCLK < 200000L)
#define FLASHSCALE 0x82
#elif (SYSCLK < 400000L)
#define FLASHSCALE 0x83
#elif (SYSCLK < 800000L)
#define FLASHSCALE 0x84
#elif (SYSCLK < 1600000L)
#define FLASHSCALE 0x85
#elif (SYSCLK < 3200000L)
#define FLASHSCALE 0x86
#elif (SYSCLK < 6400000L)
#define FLASHSCALE 0x87
#elif (SYSCLK < 12800000L)
#define FLASHSCALE 0x88
#elif (SYSCLK < 25600000L)
#define FLASHSCALE 0x89
#endif // SYSCLK test

#endif // FLASHSCALE

#ifndef FLASH_PAGESIZE
#define FLASH_PAGESIZE 512
#endif

#ifndef FLASH_SCRATCHSIZE
#define FLASH_SCRATCHSIZE 128
#endif

#ifndef FLASH_TEMP
#define FLASH_TEMP 0x01800L
#endif

#ifndef FLASH_LAST
#define FLASH_LAST 0x01A00L            // last page of FLASH
#endif

typedef UINT FLADDR;

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
extern void FLASH_ByteWrite (FLADDR addr, char byte);
extern unsigned char FLASH_ByteRead (FLADDR addr);
extern void FLASH_PageErase (FLADDR addr);

#endif // F200_FLASHPRIMITIVES_H