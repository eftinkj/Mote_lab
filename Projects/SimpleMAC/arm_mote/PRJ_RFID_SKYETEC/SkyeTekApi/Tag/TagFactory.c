/**
 * TagFactory.c
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Implementation of the TagFactory.
 */
#include "SkyeTekAPI.h"
#include "Tag/Tag.h"
#include "Tag/TagFactory.h"

/* NOTE: We can add factories for each tag type 
 *       later if there is tag-specific data that
 *       needs to be stored. For now, it's overkill.
 */
SKYETEK_STATUS CreateTagImpl(SKYETEK_TAGTYPE type, LPSKYETEK_ID lpId, LPSKYETEK_TAG tag)
{
  unsigned int ix;
  
  if( tag == NULL )
    return SKYETEK_INVALID_PARAMETER;
  
  tag->type = type;
  InitializeTagType(tag);
  
//  if( lpId != NULL && lpId->id != NULL && lpId->length > 0 && lpId->length < SKYETEK_MAX_TAG_LENGTH )
//  {
//    for(ix = 0; ix < lpId->length; ix++)
//      tag->id->id[ix] = lpId->id[ix];
//  }
  
  for(ix = 0 ; ix < lpId->length; ix++)
  {
    tag->friendly[ix]=lpId->id[ix];// Friendly in future
  }
  
  return SKYETEK_SUCCESS;
}

SKYETEK_STATUS InitializeTagType(LPSKYETEK_TAG tag)
{
  unsigned int ix = 0;

  if( tag == NULL )
    return SKYETEK_INVALID_PARAMETER;

  ix = tag->type >> 8;
  switch(ix)
  {
    case 0x00:
      if( tag->type == ISO_14443A_AUTO_DETECT || tag->type == ISO_MIFARE_ULTRALIGHT )
        tag->internal = &Iso14443ATagImpl;
      else if( tag->type == ISO_14443B_AUTO_DETECT )
        tag->internal = &Iso14443BTagImpl;
      else
        tag->internal = &GenericTagImpl;
      break;
    case 0x02:
      if( tag->type == MIFARE_DESFIRE )
        tag->internal = &DesfireTagImpl;
      else 
        tag->internal = &Iso14443ATagImpl;
      break;
    case 0x03:
      tag->internal = &Iso14443BTagImpl;
      break;
    default:
      tag->internal = &GenericTagImpl;
      break;
  }
  return SKYETEK_SUCCESS;
}

