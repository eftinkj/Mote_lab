//-----------------------------------------------------------------------------
// F320_FlashPrimitives.h
//-----------------------------------------------------------------------------
// Copyright 2004 Silicon Laboratories, Inc.
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// AUTH: BW & GP
// DATE: 21 JUL 04
//
// Target: C8051F32x
// Tool chain: KEIL C51 7.06
//
// Release 1.0
//
#ifndef F320_FLASHPRIMITIVES_H
#define F320_FLASHPRIMITIVES_H

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

#ifndef FLASH_PAGESIZE
#define FLASH_PAGESIZE 512
#endif

#ifndef FLASH_TEMP
#define FLASH_TEMP 0x03a00L
#endif

#ifndef FLASH_LAST
#define FLASH_LAST 0x03c00L
#endif

typedef UINT FLADDR;

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
extern void FLASH_ByteWrite (FLADDR addr, char byte);
extern unsigned char FLASH_ByteRead (FLADDR addr);
extern void FLASH_PageErase (FLADDR addr);

#endif // F320_FLASHPRIMITIVES_H