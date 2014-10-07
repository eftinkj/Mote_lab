//-----------------------------------------------------------------------------
// F200_FlashPrimitives.c
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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F200_FlashPrimitives.h"
#include <c8051F200.h>

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
void FLASH_ByteWrite (FLADDR addr, char byte);
unsigned char FLASH_ByteRead (FLADDR addr);
void FLASH_PageErase (FLADDR addr);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// This routine writes <byte> to the linear FLASH address <addr>.
// Linear map is decoded as follows:
// Linear Address       Device Address
// ------------------------------------------------
// 0x00000 - 0x0FFFF    0x0000 - 0xFFFF
//
void FLASH_ByteWrite (FLADDR addr, char byte)
{
   bit EA_SAVE = EA;                   // preserve EA
   char xdata * data pwrite;           // FLASH write pointer

   EA = 0;                             // disable interrupts

   pwrite = (char xdata *) addr;       // initialize write pointer
   
   FLSCL  = FLASHSCALE;                // enable FLASH writes/erases
   PSCTL |= 0x01;                      // PSWE = 1

   *pwrite = byte;                     // write the byte

   PSCTL &= ~0x01;                     // PSWE = 0
   FLSCL |=  0x0F;                     // disable FLASH writes/erases

   EA = EA_SAVE;                       // restore interrupts
}

//-----------------------------------------------------------------------------
// FLASH_ByteRead
//-----------------------------------------------------------------------------
//
// This routine reads a <byte> from the linear FLASH address <addr>.
//
unsigned char FLASH_ByteRead (FLADDR addr)
{
   bit EA_SAVE = EA;                   // preserve EA
   char code * data pread;             // FLASH read pointer
   unsigned char byte;

   pread = (char code *) addr;         // initialize read pointer
   
   EA = 0;                             // disable interrupts

   byte = *pread;                      // read the byte

   EA = EA_SAVE;                       // restore interrupts

   return byte;
}

//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// This routine erases the FLASH page containing the linear FLASH address
// <addr>.
//
void FLASH_PageErase (FLADDR addr)
{
   bit EA_SAVE = EA;                   // preserve EA
   char xdata * data pwrite;           // FLASH write pointer

   EA = 0;                             // disable interrupts

   pwrite = (char xdata *) addr;       // initialize erase pointer

   FLSCL  = FLASHSCALE;                // enable FLASH writes/erases
   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1

   *pwrite = 0;                        // initiate page erase

   PSCTL &= ~0x03;                     // PSWE = 0; PSEE = 0
   FLSCL |=  0x0F;                     // disable FLASH writes/erases

   EA = EA_SAVE;                       // restore interrupts
}