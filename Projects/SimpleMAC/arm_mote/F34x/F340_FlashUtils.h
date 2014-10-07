//-----------------------------------------------------------------------------
//  F340_FlashUtils.h
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// FID:            34X000027
// Target:         C8051F34x
// Tool chain:     Keil C51 8.00 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (GP)
//    -30 JAN 2006
//

#ifndef F340_FLASHUTILS_H
#define F340_FLASHUTILS_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F340_FlashPrimitives.h"

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
extern void FLASH_Write (FLADDR dest, char *src, unsigned numbytes);
extern char * FLASH_Read (char *dest, FLADDR src, unsigned numbytes);
extern void FLASH_Clear (FLADDR addr, unsigned numbytes);

// FLASH update/copy routines

// copy with destination preservation
extern void FLASH_Update (FLADDR dest, char *src, unsigned numbytes);   
// low-level FLASH/FLASH byte copy
extern void FLASH_Copy (FLADDR dest, FLADDR src, unsigned numbytes);    

// FLASH test routines
extern void FLASH_Fill (FLADDR addr, ULONG length, UCHAR fill);

#endif                                 // F340_FLASHUTILS_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------