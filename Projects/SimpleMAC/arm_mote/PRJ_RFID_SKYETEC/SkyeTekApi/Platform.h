/**
 * Platform.h
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Platform definitions.
 */
#ifndef SKYETEK_PLATFORM_H
#define SKYETEK_PLATFORM_H

#include "defs/esnl_pub.h"

/* Define which devices to use */
#define STAPI_SERIAL 1

#define SKYETEK_API 


#ifndef UINT32
typedef uint32_t UINT32;
#endif

#ifndef UINT16
typedef uint16_t UINT16;
#endif

#ifndef UINT8
typedef uint8_t UINT8;
#endif
  
union _16Bits 
{ 
	UINT16  w; 
	UINT8   b[2]; 
}; 
  
union _32Bits 
{ 
	UINT32  l; 
	UINT16  w[2]; 
	UINT8   b[4]; 
};

#define max(a,b) (a > b) ? a : b
#define _T(s) s

typedef char TCHAR;
typedef TCHAR* LPTSTR;

#endif 
