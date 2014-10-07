//-----------------------------------------------------------------------------
// F520_FlashPrimitives.h
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// FID:            52X000002
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

#ifndef _F520_FLASHPRIMITIVES_H_
#define _F520_FLASHPRIMITIVES_H_

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

typedef unsigned long ULONG;
typedef unsigned int  UINT;
typedef unsigned char UCHAR;
typedef UINT FLADDR;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#ifndef FLASH_PAGESIZE
#define FLASH_PAGESIZE 512
#endif

#ifndef FLASH_TEMP
#define FLASH_TEMP 0x1A00L             // For 8K Flash devices
//#define FLASH_TEMP 0x0C00L           // For 4K Flash devices
//#define FLASH_TEMP 0x0400L           // For 2K Flash devices
#endif

#ifndef FLASH_LAST
#define FLASH_LAST 0x1C00L             // For 8K Flash devices
//#define FLASH_LAST 0x0E00L           // For 4K Flash devices
//#define FLASH_LAST 0x0600L           // For 2K Flash devices
#endif

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

void          FLASH_ByteWrite (FLADDR addr, char byte);
unsigned char FLASH_ByteRead  (FLADDR addr);
void          FLASH_PageErase (FLADDR addr);

//-----------------------------------------------------------------------------
// Close Header #define
//-----------------------------------------------------------------------------

#endif    // _F520_FLASHPRIMITIVES_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------