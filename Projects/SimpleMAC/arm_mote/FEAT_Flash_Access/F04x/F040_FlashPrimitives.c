//-----------------------------------------------------------------------------
// F040_FlashPrimitives.c
//-----------------------------------------------------------------------------
// Copyright 2004 Silicon Laboratories, Inc.
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// AUTH: BW & GP
// DATE: 21 JUL 04
//
// Target: C8051F04x
// Tool chain: KEIL C51 7.06
//
// Release 1.0
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F040_FlashPrimitives.h"
#include <c8051F040.h>

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
void FLASH_ByteWrite (FLADDR addr, char byte, bit SFLE);
unsigned char FLASH_ByteRead (FLADDR addr, bit SFLE);
void FLASH_PageErase (FLADDR addr, bit SFLE);

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
void FLASH_ByteWrite (FLADDR addr, char byte, bit SFLE)
{
   char SFRPAGE_SAVE = SFRPAGE;        // preserve SFRPAGE
   bit EA_SAVE = EA;                   // preserve EA
   char xdata * data pwrite;           // FLASH write pointer

   EA = 0;                             // disable interrupts

   pwrite = (char xdata *) addr;       // initialize write pointer
   
   SFRPAGE = LEGACY_PAGE;

   FLSCL |= 0x01;                      // enable FLASH writes/erases
   PSCTL |= 0x01;                      // PSWE = 1

   if (SFLE) {
      PSCTL |= 0x04;                   // set SFLE
   }

   RSTSRC = 0x02;                      // enable VDDMON as reset source
   *pwrite = byte;                     // write the byte

   if (SFLE) {
      PSCTL &= ~0x04;                  // clear SFLE
   }
   PSCTL &= ~0x01;                     // PSWE = 0
   FLSCL &= ~0x01;                     // disable FLASH writes/erases

   SFRPAGE = SFRPAGE_SAVE;             // restore SFRPAGE
   EA = EA_SAVE;                       // restore interrupts
}

//-----------------------------------------------------------------------------
// FLASH_ByteRead
//-----------------------------------------------------------------------------
//
// This routine reads a <byte> from the linear FLASH address <addr>.
//
unsigned char FLASH_ByteRead (FLADDR addr, bit SFLE)
{
   char SFRPAGE_SAVE = SFRPAGE;        // preserve SFRPAGE
   bit EA_SAVE = EA;                   // preserve EA
   char code * data pread;             // FLASH read pointer
   unsigned char byte;

   pread = (char code *) addr;         // initialize read pointer
   
   EA = 0;                             // disable interrupts

   SFRPAGE = LEGACY_PAGE;

   if (SFLE) {
      PSCTL |= 0x04;                   // set SFLE
   }

   byte = *pread;                      // read the byte

   if (SFLE) {
      PSCTL &= ~0x04;                  // clear SFLE
   }

   SFRPAGE = SFRPAGE_SAVE;             // restore SFRPAGE
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
void FLASH_PageErase (FLADDR addr, bit SFLE)
{
   char SFRPAGE_SAVE = SFRPAGE;        // preserve SFRPAGE
   bit EA_SAVE = EA;                   // preserve EA
   char xdata * data pwrite;           // FLASH write pointer

   EA = 0;                             // disable interrupts

   pwrite = (char xdata *) addr;       // initialize erase pointer
   
   SFRPAGE = LEGACY_PAGE;

   FLSCL |= 0x01;                      // enable FLASH writes/erases
   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1

   if (SFLE) {
      PSCTL |= 0x04;                   // set SFLE
   }

   RSTSRC = 0x02;                      // enable VDDMON as reset source
   *pwrite = 0;                        // initiate page erase

   if (SFLE) {
      PSCTL &= ~0x04;                  // clear SFLE
   }

   PSCTL &= ~0x03;                     // PSWE = 0; PSEE = 0
   FLSCL &= ~0x01;                     // disable FLASH writes/erases

   SFRPAGE = SFRPAGE_SAVE;             // restore SFRPAGE
   EA = EA_SAVE;                       // restore interrupts
}