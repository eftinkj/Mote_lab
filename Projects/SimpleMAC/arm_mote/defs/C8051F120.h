
//#ifdef __KEIL__

#ifdef __SDCC__
//#ifndef __KEIL__
        #include "c8051F120_SDCC.h"
#else // SDCC
        #ifdef __KEIL__
                #include "c8051F120_KEIL.h"
        #else // KEIL
        //#include "ioC8051F120_mod.h"
        #include <ioC8051F120.h>
        #include "page_defs.h"
    #endif // not KEIL (and not SDCC)
#endif // not SDCC

#ifndef _C8051F120_COMMON_H_
#define _C8051F120_COMMON_H_


// bit 7 (MSB)
#define SPIBSY_MASK 0x80


#endif // _C8051F120_COMMON_H_
