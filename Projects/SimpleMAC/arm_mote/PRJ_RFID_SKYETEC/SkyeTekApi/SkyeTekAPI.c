/**
 * SkyeTekAPI.c
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Implementation of the SkyeTek C-API.
 */
#include "SkyeTekAPI.h"
//#include "Device/DeviceFactory.h"
//#include "Device/Device.h"
//#include "Device/SerialDevice.h"
//#include "Reader/ReaderFactory.h"
#include "Reader/Reader.h"
#include "Tag/TagFactory.h"
//#include "Tag/Tag.h"
#include "Protocol/Protocol.h"

/****************************************************
 * READER IMPLEMENTATIONS
 ****************************************************/

SKYETEK_STATUS 
SkyeTek_GetTags(
    LPSKYETEK_READER   lpReader, 
    SKYETEK_TAGTYPE    tagType, 
    LPSKYETEK_TAG      lpTags, 
    unsigned short     *count
    )
{
  LPREADER_IMPL lpri;
  if( lpReader == NULL || lpReader->internal == NULL )
    return SKYETEK_INVALID_PARAMETER;
  lpri = (LPREADER_IMPL)lpReader->internal;

  return lpri->GetTags(lpReader,tagType,lpTags,count);
}
