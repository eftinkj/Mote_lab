//-----------------------------------------------------------------------------
// F520_FlashUtils.h
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// FID:            52X000004
// Target:         C8051F520/1/2/3/4/5/6/7; C8051F530/1/2/3/4/5/6/7
// Tool chain:     Keil C51 8.00
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (GP)
//    -29 SEP 2006
//

//-----------------------------------------------------------------------------
// Open Header #define
//-----------------------------------------------------------------------------

#ifndef _F520_FLASHUTILS_H_
#define _F520_FLASHUTILS_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F520_FlashPrimitives.h"

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
void FLASH_Write (FLADDR dest, char *src, unsigned int numbytes);
char * FLASH_Read (char *dest, FLADDR src, unsigned int numbytes);
void FLASH_Clear (FLADDR addr, unsigned int numbytes);

// FLASH update/copy routines
void FLASH_Update (FLADDR dest, char *src, unsigned int numbytes);   
void FLASH_Copy (FLADDR dest, FLADDR src, unsigned int numbytes);    

// FLASH test routines
void FLASH_Fill (FLADDR addr, ULONG length, UCHAR fill);

//-----------------------------------------------------------------------------
// Close Header #define
//-----------------------------------------------------------------------------

#endif    // _F520_FLASHUTILS_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------