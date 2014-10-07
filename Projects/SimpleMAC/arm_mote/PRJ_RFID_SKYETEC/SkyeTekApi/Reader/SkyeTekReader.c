/**
 * SkyeTekReader.c
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Implementation of the SkyeTekReader
 */
#include "SkyeTekAPI.h"
//#include "PRJ_RFID_SKYETEC/ReaderFactory.h"
#include "Reader/Reader.h"
//#include "../Device/Device.h"
#include "Protocol/Protocol.h"
#include "Tag/TagFactory.h"

#define MOTE_RFID_NUM_MAX_TAG_TYPE 8

SKYETEK_STATUS 
SkyeTekReader_GetTags(
    LPSKYETEK_READER   lpReader, 
    SKYETEK_TAGTYPE    tagType, 
    LPSKYETEK_TAG      lpTags, 
    unsigned short     *count
    )
{
  LPPROTOCOLIMPL lppi;
  unsigned int num;
  unsigned int ix = 0;
  
  TAGTYPE_ARRAY tagTypes[MOTE_RFID_NUM_MAX_TAG_TYPE];
  SKYETEK_DATA sdData[MOTE_RFID_NUM_MAX_TAG_TYPE];

  SKYETEK_STATUS status;

  if( lpReader->lpProtocol == NULL )
    return SKYETEK_INVALID_PARAMETER;

  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;

  status = lppi->GetTags(lpReader,tagType,&tagTypes[0],&sdData[0],&num,5000);
//  if( status != SKYETEK_SUCCESS )
//    return status;
  
  if( num > 0 )
  {
    // check number of tags
    if( num > MOTE_RFID_NUM_MAX_TAGS)
    {
      return SKYETEK_OUT_OF_MEMORY;
    }
    
    for( ix = 0; ix < num; ix++ )
    {
      status = CreateTagImpl(tagTypes[ix].type, (LPSKYETEK_ID)&sdData[ix], &lpTags[ix]);
      if( status != SKYETEK_SUCCESS )
        return status;
    }
  }

  *count = num;
  status = SKYETEK_SUCCESS;
  return status;
}

int
SkyeTekReader_DoesRIDMatch(
      LPSKYETEK_READER      lpReader,
      const unsigned char   *id
    )
{
  unsigned long i;
  if( lpReader == NULL || lpReader->id == NULL || id == NULL )
    return 0;
  for( i = 0; i < lpReader->id->length; i++ )
  {
    if( lpReader->id->id[i] != id[i] )
      return 0;
  }
  return 1;
}

void
SkyeTekReader_CopyRIDToBuffer(
      LPSKYETEK_READER      lpReader,
      unsigned char   *id
    )
{
  unsigned long i;
  if( lpReader == NULL || lpReader->id == NULL || id == NULL )
    return;
  for( i = 0; i < lpReader->id->length; i++ )
  {
    id[i] = lpReader->id->id[i];
  }
}

READER_IMPL SkyetekReaderImpl = {
  SkyeTekReader_GetTags,
  SkyeTekReader_DoesRIDMatch,
  SkyeTekReader_CopyRIDToBuffer
};

