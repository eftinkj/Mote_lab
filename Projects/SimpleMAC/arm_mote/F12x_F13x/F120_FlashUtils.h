//-----------------------------------------------------------------------------
// F120_FlashUtils.h
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
// Release 1.1
// -Upgrading release version due to change in FlashPrimitives.h
// -07 FEB 2006 (GP)
//
// Release 1.0
//

#ifndef F120_FLASHUTILS_H
#define F120_FLASHUTILS_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F120_FlashPrimitives.h"

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
//extern void FLASH_Write (FLADDR dest, char *src, unsigned numbytes, bit SFLE);
extern void FLASH_Write (FLADDR dest, char xdata *src, unsigned numbytes, unsigned char SFLE) reentrant;

extern char * FLASH_Read (char *dest, FLADDR src, unsigned numbytes, bit SFLE);
extern void FLASH_Clear (FLADDR addr, unsigned numbytes, bit SFLE);

// FLASH update/copy routines
extern void FLASH_Update (FLADDR dest, char *src, unsigned numbytes, bit SFLE);
extern void FLASH_Copy (FLADDR dest, bit destSFLE, FLADDR src, bit srcSFLE,
                 unsigned numbytes);

// FLASH test routines
extern void FLASH_Fill (FLADDR addr, ULONG length, UCHAR fill, bit SFLE);

#endif // F120_FLASHUTILS_H