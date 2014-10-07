//-----------------------------------------------------------------------------
//  F326_FlashUtils.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// FID:            326000026
// Target:         C8051F326/7
// Tool chain:     Keil C51 8.00 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (GP)
//    -30 JAN 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F326_FlashPrimitives.h"
#include "F326_FlashUtils.h"

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
void FLASH_Write (FLADDR dest, char *src, unsigned numbytes);
char * FLASH_Read (char *dest, FLADDR src, unsigned numbytes);
void FLASH_Clear (FLADDR addr, unsigned numbytes);

// FLASH update/copy routines
void FLASH_Update (FLADDR dest, char *src, unsigned numbytes);
void FLASH_Copy (FLADDR dest, FLADDR src, unsigned numbytes);

// FLASH test routines
void FLASH_Fill (FLADDR addr, ULONG length, UCHAR fill);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH_Clear
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 
//   1)  FLADDR dest - target address where to start clearing bytes
//                     range is 0 to <FLASH_TEMP>-1
//   2)  unsigned numbytes  - number of bytes to clear
//                            range is 1 to <FLASH_PAGESIZE>
//
// This routine erases <numbytes> starting from the FLASH addressed by
// <dest> by performing a read-modify-write operation using <FLASH_TEMP> as
// a temporary holding area.
//
//-----------------------------------------------------------------------------

void FLASH_Clear (FLADDR dest, unsigned numbytes)
{
   FLADDR dest_1_page_start;           // First address in 1st page
                                       // containing <dest>
   FLADDR dest_1_page_end;             // Last address in 1st page
                                       // containing <dest>
   FLADDR dest_2_page_start;           // First address in 2nd page
                                       // containing <dest>
   FLADDR dest_2_page_end;             // Last address in 2nd page
                                       // containing <dest>
   unsigned numbytes_remainder;        // When crossing page boundary,
                                       // number of <src> bytes on 2nd page
   unsigned FLASH_pagesize;            // Size of FLASH page to update

   FLADDR  wptr;                       // Write address
   FLADDR  rptr;                       // Read address

   unsigned length;

   FLASH_pagesize = FLASH_PAGESIZE;

   dest_1_page_start = dest & ~(FLASH_pagesize - 1);
   dest_1_page_end = dest_1_page_start + FLASH_pagesize - 1;
   dest_2_page_start = (dest + numbytes)  & ~(FLASH_pagesize - 1);
   dest_2_page_end = dest_2_page_start + FLASH_pagesize - 1;

   if (dest_1_page_end == dest_2_page_end) {

      // 1. Erase Scratch page
      FLASH_PageErase (FLASH_TEMP);

      // 2. Copy bytes from first byte of dest page to dest-1 to Scratch page

      wptr = FLASH_TEMP;
      rptr = dest_1_page_start;
      length = dest - dest_1_page_start;
      FLASH_Copy (wptr, rptr, length);

      // 3. Copy from (dest+numbytes) to dest_page_end to Scratch page

      wptr = FLASH_TEMP + dest - dest_1_page_start + numbytes;
      rptr = dest + numbytes;
      length = dest_1_page_end - dest - numbytes + 1;
      FLASH_Copy (wptr, rptr, length);

      // 4. Erase destination page
      FLASH_PageErase (dest_1_page_start);

      // 5. Copy Scratch page to destination page
      wptr = dest_1_page_start;
      rptr = FLASH_TEMP;
      length = FLASH_pagesize;
      FLASH_Copy (wptr, rptr, length);

   } else {                            // Value crosses page boundary
      // 1. Erase Scratch page
      FLASH_PageErase (FLASH_TEMP);

      // 2. Copy bytes from first byte of dest page to dest-1 
	  //    to Scratch page

      wptr = FLASH_TEMP;
      rptr = dest_1_page_start;
      length = dest - dest_1_page_start;
      FLASH_Copy (wptr, rptr, length);

      // 3. Erase destination page 1
      FLASH_PageErase (dest_1_page_start);

      // 4. Copy Scratch page to destination page 1
      wptr = dest_1_page_start;
      rptr = FLASH_TEMP;
      length = FLASH_pagesize;
      FLASH_Copy (wptr, rptr, length);

      // now handle 2nd page

      // 5. Erase Scratch page
      FLASH_PageErase (FLASH_TEMP);

      // 6. Copy bytes from numbytes remaining to dest-2_page_end 
	  //    to Scratch page

      numbytes_remainder = numbytes - (dest_1_page_end - dest + 1);
      wptr = FLASH_TEMP + numbytes_remainder;
      rptr = dest_2_page_start + numbytes_remainder;
      length = FLASH_pagesize - numbytes_remainder;
      FLASH_Copy (wptr, rptr, length);

      // 7. Erase destination page 2
      FLASH_PageErase (dest_2_page_start);

      // 8. Copy Scratch page to destination page 2
      wptr = dest_2_page_start;
      rptr = FLASH_TEMP;
      length = FLASH_pagesize;
      FLASH_Copy (wptr, rptr, length);
   }
}

//-----------------------------------------------------------------------------
// FLASH_Update
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 
//   1)  FLADDR dest - target address where to start updating bytes
//                     range is 0 to (FLASH_TEMP-1)
//   2)  char *src   - pointer to address where source bytes are located
//   3)  unsigned numbytes  - number of bytes to clear
//                            range is 1 to <FLASH_PAGESIZE>
//
// This routine replaces <numbytes> from <src> to the FLASH addressed by
// <dest>.  This function calls FLASH_Clear() to handle the dirty work of
// initializing all <dest> bytes to 0xff's prior to copying the bytes from
// <src> to <dest>.
//
//-----------------------------------------------------------------------------

void FLASH_Update (FLADDR dest, char *src, unsigned numbytes)
{
   // 1. Erase <numbytes> starting from <dest>
   FLASH_Clear (dest, numbytes);

   // 2. Write <numbytes> from <src> to <dest>
   FLASH_Write (dest, src, numbytes);
}

//-----------------------------------------------------------------------------
// FLASH_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 
//   1)  FLADDR dest - target address where to start writing bytes
//                     range is 0 to (FLASH_TEMP-1)
//   2)  char *src   - pointer to address where source bytes are located
//   3)  unsigned numbytes   - number of bytes to write
//                     range is limited by Flash size and starting location
//
// This routine copies <numbytes> from <src> to the linear FLASH address
// <dest>.
//
//-----------------------------------------------------------------------------
void FLASH_Write (FLADDR dest, char *src, unsigned numbytes)
{
   FLADDR i;

   for (i = dest; i < dest+numbytes; i++) {
      FLASH_ByteWrite (i, *src++);
   }
}
//-----------------------------------------------------------------------------
// FLASH_Read
//-----------------------------------------------------------------------------
//
// Return Value :
//   1)  char *     -  pointer to bytes read back
// Parameters   : 
//   1)  char *dest -  target address where to store bytes read from Flash
//   2)  char *src   - pointer to address where source bytes are located
//   3)  unsigned numbytes   - number of bytes to read
//                     range is limited by Flash size and starting location
//
// This routine copies <numbytes> from the linear FLASH address <src> to
// <dest>.
//
//-----------------------------------------------------------------------------
char * FLASH_Read (char *dest, FLADDR src, unsigned numbytes)
{
   FLADDR i;

   for (i = 0; i < numbytes; i++) {
      *dest++ = FLASH_ByteRead (src+i);
   }
   return dest;
}

//-----------------------------------------------------------------------------
// FLASH_Copy
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 
//   1)  FLADDR dest -  target address where to copy the source bytes
//   2)  FLADDR src   - target address where source bytes are located
//   3)  unsigned numbytes   - number of bytes to read
//                     range is limited by Flash size and starting location
//
// This routine copies <numbytes> from <src> to the linear FLASH address
// <dest>.
//
//-----------------------------------------------------------------------------

void FLASH_Copy (FLADDR dest, FLADDR src, unsigned numbytes)
{
   FLADDR i;

   for (i = 0; i < numbytes; i++) {

      FLASH_ByteWrite ((FLADDR) dest+i, FLASH_ByteRead((FLADDR) src+i));
   }
}


//-----------------------------------------------------------------------------
// FLASH_Fill
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : 
//   1)  FLADDR addr   - target address where to start filling
//   2)  ULONG length  - number of bytes to fill
//                       range is limited by Flash size and starting point
//   3)  UCHAR fill    - char to fill 
//                       range is 0x00 to 0xFF
//
// This routine fills the FLASH beginning at <addr> with <length> bytes.
//
//-----------------------------------------------------------------------------
void FLASH_Fill (FLADDR addr, ULONG length, UCHAR fill)
{
   FLADDR i;

   for (i = 0; i < length; i++) {
      FLASH_ByteWrite (addr+i, fill);
   }
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------