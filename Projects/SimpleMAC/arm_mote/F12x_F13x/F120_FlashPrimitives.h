//-----------------------------------------------------------------------------
// F120_FlashPrimitives.h
//-----------------------------------------------------------------------------
// Copyright 2004 Silicon Laboratories, Inc.
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// AUTH: BW & GP
// DATE: 21 JUL 04
//
// Target: C8051F12x
// Tool chain: KEIL C51 7.06
//
//
// Release 1.1
// -Change typecast of FLASH_PAGESIZE to 1024L to fix bug in Flash_Clear()
// -07 FEB 2006 (GP)
//
// Release 1.0
//

#ifndef F120_FLASHPRIMITIVES_H
#define F120_FLASHPRIMITIVES_H

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
#define FLASH_PAGESIZE 1024UL
#endif

#ifndef FLASH_SCRATCHSIZE
#define FLASH_SCRATCHSIZE 256UL
#endif

#ifndef FLASH_TEMP
#define FLASH_TEMP 0x1F400UL            // address of temp page
#endif

#ifndef FLASH_LAST
#define FLASH_LAST 0x1F800UL            // last page of FLASH
#endif

typedef ULONG FLADDR;


//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
extern void FLASH_ByteWrite (FLADDR addr, char byte, bit SFLE);
extern unsigned char FLASH_ByteRead (FLADDR addr, bit SFLE);
extern void FLASH_PageErase (FLADDR addr, bit SFLE);

#endif // F120_FLASHPRIMITIVES_H