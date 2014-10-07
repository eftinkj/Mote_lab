/**
 * GenericTag.c
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Implementation of a Generic Tag.
 */
#include "Tag/Tag.h"
#include "Protocol/Protocol.h"
//#include "../Protocol/asn1.h"

#define GENERIC_TIMEOUT 300

//SKYETEK_STATUS 
//GenericTag_SelectTag(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->SelectTag(lpReader,lpTag,GENERIC_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//GenericTag_ReadTagData(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ADDRESS    lpAddr,
//    unsigned char        encrypt,
//    unsigned char        hmac,
//    LPSKYETEK_DATA       *lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  PROTOCOL_FLAGS flags;
//  unsigned int to;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpAddr == NULL || lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  memset(&flags, 0, sizeof(flags));
//  flags.isEncrypt = encrypt;
//  flags.isHMAC = hmac;
//  to = lpAddr->blocks * 5000;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->ReadTagData(lpReader,lpTag,flags,lpAddr,lpData,to);
//}
//
//SKYETEK_STATUS 
//GenericTag_WriteTagData(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ADDRESS    lpAddr, 
//    unsigned char        encrypt,
//    unsigned char        hmac,
//    LPSKYETEK_DATA       lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  PROTOCOL_FLAGS flags;
//  unsigned int to;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpAddr == NULL || lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  memset(&flags, 0, sizeof(flags));
//  flags.isEncrypt = encrypt;
//  flags.isHMAC = hmac;
//  to = lpAddr->blocks * 5000;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->WriteTagData(lpReader,lpTag,flags,lpAddr,lpData,to);
//}
//
//SKYETEK_STATUS 
//GenericTag_ReadTagConfig(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       *lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  PROTOCOL_FLAGS flags;
//  unsigned int to;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpAddr == NULL || lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  memset(&flags, 0, sizeof(flags));
//  to = (lpAddr->blocks ? lpAddr->blocks : 255) * 5000;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->ReadTagConfig(lpReader,lpTag,flags,lpAddr,lpData,to);
//}
//
//SKYETEK_STATUS 
//GenericTag_WriteTagConfig(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ADDRESS    lpAddr, 
//    LPSKYETEK_DATA       lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  PROTOCOL_FLAGS flags;
//  unsigned int to;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpAddr == NULL || lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  memset(&flags, 0, sizeof(flags));
//  to = lpAddr->blocks * 5000;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->WriteTagConfig(lpReader,lpTag,flags,lpAddr,lpData,to);
//}
//
//SKYETEK_STATUS GenericTag_LockTagBlock(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  PROTOCOL_FLAGS flags;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpAddr == NULL  )
//    return SKYETEK_INVALID_PARAMETER;
//  memset(&flags, 0, sizeof(flags));
//  flags.isLock = 1;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->WriteTagData(lpReader,lpTag,flags,lpAddr,lpData,GENERIC_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//GenericTag_ActivateTagType(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_DeactivateTagType(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_SetTagBitRate(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    unsigned char        rate
//    )
//{
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetTagInfo(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_MEMORY     lpMemory
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  LPSKYETEK_DATA lpData = NULL;
//  SKYETEK_STATUS status;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpMemory == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetTagInfo(lpReader,lpTag,&lpData,1200);
//  if( status != SKYETEK_SUCCESS )
//    return status;
//
//  if( lpData == NULL || lpData->data == NULL || lpData->size < 6 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//
//  lpMemory->startBlock = (lpData->data[0] << 8) | lpData->data[1];
//  lpMemory->maxBlock = (lpData->data[2] << 8) | lpData->data[3];
//  lpMemory->bytesPerBlock = (lpData->data[4] << 8) | lpData->data[5];
//
//  SkyeTek_FreeData(lpData);
//  return SKYETEK_SUCCESS;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetLockStatus(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ADDRESS    lpAddr,
//    unsigned char        *st
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  LPSKYETEK_DATA lpData = NULL;
//  SKYETEK_STATUS status;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpAddr == NULL || st == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetLockStatus(lpReader,lpTag,lpAddr,&lpData,GENERIC_TIMEOUT*3);
//  if( status != SKYETEK_SUCCESS )
//    return status;
//
//  if( lpData == NULL || lpData->data == NULL || lpData->size != 1 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//
//  *st = lpData->data[0];
//  SkyeTek_FreeData(lpData);
//  return SKYETEK_SUCCESS;
//}
//
//SKYETEK_STATUS 
//GenericTag_KillTag(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_KEY        lpKey
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpKey == NULL ||
//      lpKey->lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->KillTag(lpReader,lpTag,lpKey->lpData,1000);
//}
//
//SKYETEK_STATUS 
//GenericTag_ReviveTag(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->ReviveTag(lpReader,lpTag,1000);
//}
//
//SKYETEK_STATUS 
//GenericTag_EraseTag(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->EraseTag(lpReader,lpTag,1000);
//}
//
//SKYETEK_STATUS 
//GenericTag_FormatTag(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->FormatTag(lpReader,lpTag,1000);
//}
//
//SKYETEK_STATUS 
//GenericTag_DeselectTag(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->DeselectTag(lpReader,lpTag,GENERIC_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//GenericTag_AuthenticateTag(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_KEY        lpKey
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  LPSKYETEK_DATA lpData;
//  unsigned int ix;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpKey == NULL  )
//    return SKYETEK_INVALID_PARAMETER;
//
//  // data is either type + key or type + number
//  if( lpKey->lpData != NULL && lpKey->lpData->data != NULL && lpKey->lpData->size > 0 )
//    lpData = SkyeTek_AllocateData(lpKey->lpData->size + 1);
//  else
//    lpData = SkyeTek_AllocateData(2);
//  if( lpData == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  lpData->data[0] = lpKey->version;
//  if( lpKey->lpData != NULL && lpKey->lpData->data != NULL && lpKey->lpData->size > 0 )
//  {
//    for( ix = 0; ix < lpKey->lpData->size; ix++ )
//      lpData->data[ix+1] = lpKey->lpData->data[ix];
//  }
//  else
//  {
//    lpData->data[1] = lpKey->number;
//  }
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->AuthenticateTag(lpReader,lpTag,&lpKey->address,lpData,GENERIC_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//GenericTag_SendTagPassword(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_DATA       lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpData == NULL || lpData->data == NULL || lpData->size == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->SendTagPassword(lpReader,lpTag,lpData,5000);
//}
//
//SKYETEK_STATUS 
//GenericTag_GetApplicationIDs(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         **ids,
//    unsigned int         *count
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_SelectApplication(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         lpId
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_CreateApplication(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         lpId,
//    LPSKYETEK_APP_KEY_SETTINGS  lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_DeleteApplication(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         lpId
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetFileIDs(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         **lpFiles,
//    unsigned int         *count
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_SelectFile(
//    LPSKYETEK_READER lpReader,
//    LPSKYETEK_TAG lpTag,
//    LPSKYETEK_ID lpFile
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_CreateDataFile(
//    LPSKYETEK_READER             lpReader,
//    LPSKYETEK_TAG                lpTag,
//    LPSKYETEK_ID                 lpFile,
//    LPSKYETEK_DATA_FILE_SETTINGS lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_CreateValueFile(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_VALUE_FILE_SETTINGS    lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_CreateRecordFile(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_RECORD_FILE_SETTINGS   lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetCommonFileSettings(
//    LPSKYETEK_READER             lpReader,
//    LPSKYETEK_TAG                lpTag,
//    LPSKYETEK_ID                 lpFile,
//    LPSKYETEK_FILE_SETTINGS      lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetDataFileSettings(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_DATA_FILE_SETTINGS     lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetValueFileSettings(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_VALUE_FILE_SETTINGS    lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetRecordFileSettings(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_RECORD_FILE_SETTINGS   lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_ChangeFileSettings(
//    LPSKYETEK_READER             lpReader,
//    LPSKYETEK_TAG                lpTag,
//    LPSKYETEK_ID                 lpFile,
//    LPSKYETEK_FILE_SETTINGS      lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_ReadFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       *lpData
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//    	
//SKYETEK_STATUS 
//GenericTag_WriteFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       lpData
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_DeleteFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_ClearFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_CreditValueFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    unsigned int         amount
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_DebitValueFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    unsigned int         amount
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_LimitedCreditValueFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    unsigned int         amount
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetValue(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    unsigned int         *value
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_ReadRecords(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       *lpData
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//    	
//SKYETEK_STATUS 
//GenericTag_WriteRecord(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       lpData
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_CommitTransaction(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_AbortTransaction(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_EnableEAS(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->EnableEAS(lpReader,lpTag,GENERIC_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//GenericTag_DisableEAS(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->DisableEAS(lpReader,lpTag,GENERIC_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//GenericTag_ScanEAS(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag,
//    unsigned char        *state
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || state == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ScanEAS(lpReader,lpTag,GENERIC_TIMEOUT);
//  if( status == SKYETEK_SUCCESS )
//  {
//    *state = 1;
//    return SKYETEK_SUCCESS;
//  }
//  else if( status == SKYETEK_FAILURE )
//  {
//    *state = 0;
//    return SKYETEK_SUCCESS;
//  }
//  return status;
//}
//
//SKYETEK_STATUS 
//GenericTag_ReadAFI(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_DATA       *lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->ReadAFI(lpReader,lpTag,lpData,GENERIC_TIMEOUT);
//}
//    	
//SKYETEK_STATUS 
//GenericTag_WriteAFI(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_DATA       lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpData == NULL || lpData->data == NULL || lpData->size == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->WriteAFI(lpReader,lpTag,lpData,GENERIC_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//GenericTag_ReadDSFID(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_DATA       *lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->ReadDSFID(lpReader,lpTag,lpData,GENERIC_TIMEOUT);
//}
//    	
//SKYETEK_STATUS 
//GenericTag_WriteDSFID(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_DATA       lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpData == NULL || lpData->data == NULL || lpData->size == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  return lppi->WriteDSFID(lpReader,lpTag,lpData,GENERIC_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//GenericTag_GetKeyVersion(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_KEY        lpKey
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_ChangeKey(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_KEY        lpNewKey,
//    LPSKYETEK_KEY        lpCurrentKey
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetApplicationKeySettings(
//    LPSKYETEK_READER              lpReader,
//    LPSKYETEK_TAG                 lpTag,
//    LPSKYETEK_APP_KEY_SETTINGS    lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_GetMasterKeySettings(
//    LPSKYETEK_READER                lpReader,
//    LPSKYETEK_TAG                   lpTag,
//    LPSKYETEK_MASTER_KEY_SETTINGS   lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_ChangeApplicationKeySettings(
//    LPSKYETEK_READER             lpReader,
//    LPSKYETEK_TAG                lpTag,
//    LPSKYETEK_APP_KEY_SETTINGS   lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_ChangeMasterKeySettings(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_MASTER_KEY_SETTINGS    lpSettings
//    )
//{
//  /* Only supported in Desfire */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
//SKYETEK_STATUS 
//GenericTag_InitializeSecureMemoryTag(
//    LPSKYETEK_READER          lpReader,
//    LPSKYETEK_TAG             lpTag,
//    SKYETEK_HMAC_ALGORITHM    hmac,
//    LPSKYETEK_KEY             lpKeyHMAC,
//    SKYETEK_CIPHER_ALGORITHM  cipher,
//    LPSKYETEK_KEY             lpKeyCipher,
//		int                       useKeyDerivationFunction
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//  int64 w;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpKeyHMAC == NULL || lpKeyHMAC->lpData == NULL || lpKeyHMAC->lpData->data == NULL || lpKeyHMAC->lpData->size < 1 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(2048);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = hmac;
//  st_asn1_write_enumerated(context,w);
//  if( lpKeyHMAC->name != NULL && _tcslen(lpKeyHMAC->name) > 0) {
//    st_asn1_start_context_specific(context, 1);
//    st_asn1_write_octet_string(context, (unsigned char *)lpKeyHMAC->name, _tcslen(lpKeyHMAC->name)*sizeof(TCHAR));
//    st_asn1_finish_context_specific(context, 1);
//  }
//  st_asn1_write_octet_string(context, lpKeyHMAC->lpData->data, lpKeyHMAC->lpData->size);
//  if( cipher > 0 )
//  {
//    w = cipher;
//    st_asn1_write_enumerated(context,w);
//    if( lpKeyCipher->name != NULL && _tcslen(lpKeyCipher->name) > 0) {
//      st_asn1_start_context_specific(context, 2);
//      st_asn1_write_octet_string(context, (unsigned char *)lpKeyCipher->name, _tcslen(lpKeyCipher->name)*sizeof(TCHAR));
//      st_asn1_finish_context_specific(context, 2);
//    }
//    if( lpKeyCipher != NULL && lpKeyCipher->lpData != NULL && lpKeyCipher->lpData->data != NULL && lpKeyCipher->lpData->size > 0 )
//      st_asn1_write_octet_string(context, lpKeyCipher->lpData->data, lpKeyCipher->lpData->size);
//  }
//
//	if (useKeyDerivationFunction)
//		st_asn1_write_boolean(context, 1);
//
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->InitializeSecureMemoryTag(lpReader,lpTag,lpDataS,10000);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//SKYETEK_STATUS 
//GenericTag_SetupSecureMemoryTag(
//    LPSKYETEK_READER          lpReader,
//    LPSKYETEK_TAG             lpTag,
//    LPSKYETEK_KEY             lpKeyHMAC,
//    LPSKYETEK_KEY             lpKeyCipher,
//		int                       useKeyDerivationFunction
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpKeyHMAC == NULL || lpKeyHMAC->lpData == NULL || lpKeyHMAC->lpData->data == NULL || lpKeyHMAC->lpData->size < 1 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  if( lpKeyHMAC->name != NULL && _tcslen(lpKeyHMAC->name) > 0) {
//    st_asn1_start_context_specific(context, 1);
//    st_asn1_write_octet_string(context, (unsigned char *)lpKeyHMAC->name, _tcslen(lpKeyHMAC->name)*sizeof(TCHAR));
//    st_asn1_finish_context_specific(context, 1);
//  }
//  st_asn1_write_octet_string(context, lpKeyHMAC->lpData->data, lpKeyHMAC->lpData->size);
//  if (lpKeyCipher != NULL && lpKeyCipher->lpData != NULL && lpKeyCipher->lpData->data != NULL && lpKeyCipher->lpData->size > 0) {
//    if( lpKeyCipher->name != NULL && _tcslen(lpKeyCipher->name) > 0) {
//      st_asn1_start_context_specific(context, 2);
//      st_asn1_write_octet_string(context, (unsigned char *)lpKeyCipher->name, _tcslen(lpKeyCipher->name)*sizeof(TCHAR));
//      st_asn1_finish_context_specific(context, 2);
//    }
//    st_asn1_write_octet_string(context, lpKeyCipher->lpData->data, lpKeyCipher->lpData->size);
//  }
//
//	if (useKeyDerivationFunction)
//		st_asn1_write_boolean(context, 1);
//
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->SetupSecureMemoryTag(lpReader,lpTag,lpDataS,10000);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Expects
//// SEQUENCE {
////   interface            ENUMERATED { 14443-4(1), 7816-3(2), 7816-12(3)}
////   CHOICE {
////     iso14443-4         [1] SEQUENCE {
////       blockType  ENUMERATED { I_BLOCK(0), R_BLOCK(160), S_BLOCK(192) },
////       data       OCTET-STRING }
////   }
//// }
//// Returns
//// OCTET-STRING
//*/
//SKYETEK_STATUS 
//GenericTag_InterfaceSend(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    SKYETEK_INTERFACE    intf,
//    SKYETEK_BLOCK        block,
//    LPSKYETEK_DATA       lpSendData,
//    LPSKYETEK_DATA       *lpRecvData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  int64 w;
//  
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpSendData == NULL || lpSendData->data == NULL || lpSendData->size == 0 ||
//      lpRecvData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(2048);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = intf;
//  st_asn1_write_enumerated(context,w);
//  if( intf == I14443D4 )
//  {
//    st_asn1_start_context_specific(context, 1);
//    st_asn1_start_sequence(context);
//    w = block;
//    st_asn1_write_enumerated(context,w);
//    st_asn1_write_octet_string(context, lpSendData->data, lpSendData->size);
//    st_asn1_finish_sequence(context);
//    st_asn1_finish_context_specific(context, 1);
//  }
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//  
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->InterfaceSend(lpReader,lpTag,lpDataS,&lpDataR,GENERIC_TIMEOUT);
//  if( status != SKYETEK_SUCCESS )
//  {
//    SkyeTek_FreeData(lpDataS);
//    return status;
//  }
//  if( lpDataR == NULL  )
//    return SKYETEK_SUCCESS;
//  if( lpDataR->data == NULL || lpDataR->size == 0 )
//  {
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_SUCCESS;
//  }
//  
//  *lpRecvData = SkyeTek_AllocateData(lpDataR->size);
//  if( *lpRecvData == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_read_octet_string(context, (*lpRecvData)->data, &(*lpRecvData)->size);
//  st_asn1_free_context(&context);
//  
//  return SKYETEK_SUCCESS;
//}
//
//SKYETEK_STATUS 
//GenericTag_TransportSend(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    SKYETEK_TRANSPORT    transport,
//    SKYETEK_BLOCK        block,
//    LPSKYETEK_DATA       lpCla,
//    LPSKYETEK_DATA       lpIns,
//    LPSKYETEK_DATA       lpP1p2,
//    LPSKYETEK_DATA       lpData, 
//    unsigned int         lpLe,
//    LPSKYETEK_DATA       *lpRecvData
//    )
//{
//  /* Only supported in ISO14443A */
//  return SKYETEK_NOT_SUPPORTED;
//}
//
///*
//  // Returns
//  //   SEQUENCE {
//  //     paymentSystemType         ENUMERATED {
//  //       expressPay(1),
//  //       payPass(2),
//  //       discover(3) }
//  //     applicationId                  OCTET-STRING,
//  //     applicationLabel               OCTET-STRING,
//  //     applicationInterchangeProfile  INTEGER,
//  //     applicationElementaryFiles     OCTET-STRING
//  //   }
//*/
//SKYETEK_STATUS 
//GenericTag_InitiatePayment(
//    LPSKYETEK_READER         lpReader,
//    LPSKYETEK_TAG            lpTag,
//    SKYETEK_PAYMENT_SYSTEM  *lpPaymentSystem
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  int64 w;
//  size_t len;
//  int l;
// 
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL ||
//      lpPaymentSystem == NULL)
//    return SKYETEK_INVALID_PARAMETER;
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->InitiatePayment(lpReader,lpTag,&lpDataR,GENERIC_TIMEOUT);
//  if( status != SKYETEK_SUCCESS )
//  {
//    return status;
//  }
//  if( lpDataR == NULL  )
//    return SKYETEK_SUCCESS;
//  if( lpDataR->data == NULL || lpDataR->size == 0 )
//  {
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_SUCCESS;
//  }
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//
//  st_asn1_start_sequence(context);
//
//  st_asn1_read_enumerated(context, &w);
//  lpPaymentSystem->systemType = (SKYETEK_PAYMENT_TYPE)w;
//
//  len = sizeof(lpPaymentSystem->id);
//  st_asn1_read_octet_string(context,
//                            lpPaymentSystem->id,
//                            &len);
//  lpPaymentSystem->idLength = len;
//
//  len = sizeof(lpPaymentSystem->label);
//  st_asn1_read_octet_string(context,
//                            (unsigned char*) lpPaymentSystem->label,
//                            &len);
//  if (len < sizeof(lpPaymentSystem->label))
//    lpPaymentSystem->label[len] = '\0';
//
//  st_asn1_read_integer(context, &w);
//  lpPaymentSystem->interchangeProfile = (unsigned short)w;
//
//  len = sizeof(lpPaymentSystem->elementaryFiles);
//  st_asn1_read_octet_string(context,
//                            lpPaymentSystem->elementaryFiles,
//                            &len);
//  lpPaymentSystem->elementaryFilesLength = len;
//  
//  st_asn1_finish_sequence(context);
//
//  l = st_asn1_finalize(context);
//  
//  st_asn1_free_context(&context);
//
//  lpPaymentSystem->track1Length = 0;
//  lpPaymentSystem->track2Length = 0;
//  
//  if (l > 0)
//    return SKYETEK_SUCCESS;
//  else
//    return SKYETEK_FAILURE;
//}
//
///*
//  // Expects
//  // SEQUENCE {
//  //   CHOICE {
//  //     expresspay-options  [1] SEQUENCE {
//  //       transactionType       ENUMERATED {
//  //         OFFLINE(1),
//  //         ONLINE(2),
//  //         REJECT(3) } }
//  //   }
//  // }
//  // Returns
//  //   SEQUENCE {
//  //     track1            [1] OCTET-STRING OPTIONAL,
//  //     track2            [2] OCTET-STRING OPTIONAL,
//  //   }
//*/
//SKYETEK_STATUS 
//GenericTag_ComputePayment(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    SKYETEK_TRANSACTION  transaction,
//    SKYETEK_PAYMENT_SYSTEM  *lpPaymentSystem)
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  int64 w;
//  int l;
//  size_t len;
//  
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL ||
//      lpPaymentSystem == NULL)
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(128);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  if (transaction != 0) {
//    st_asn1_start_context_specific(context, 1);
//    st_asn1_start_sequence(context);
//    w = transaction;
//    st_asn1_write_enumerated(context, w);
//    st_asn1_finish_sequence(context);
//    st_asn1_finish_context_specific(context, 1);
//  }
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//  
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ComputePayment(lpReader,lpTag,lpDataS,&lpDataR,GENERIC_TIMEOUT);
//  if( status != SKYETEK_SUCCESS )
//  {
//    SkyeTek_FreeData(lpDataS);
//    return status;
//  }
//  if( lpDataR == NULL  )
//    return SKYETEK_SUCCESS;
//  if( lpDataR->data == NULL || lpDataR->size == 0 )
//  {
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_SUCCESS;
//  }
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_DECODE,lpDataR->data,lpDataR->size);
//
//  st_asn1_start_sequence(context);
//
//  if (st_asn1_peek(context) == -1) {
//    st_asn1_start_context_specific(context, 1);
//    len = sizeof(lpPaymentSystem->track1);
//    st_asn1_read_octet_string(context,
//                              lpPaymentSystem->track1,
//                              &len);
//    lpPaymentSystem->track1Length = len;
//    st_asn1_finish_context_specific(context, 1);
//  } else
//    lpPaymentSystem->track1Length = 0;
//  
//  if (st_asn1_peek(context) == -2) {
//    st_asn1_start_context_specific(context, 2);
//    len = sizeof(lpPaymentSystem->track2);
//    st_asn1_read_octet_string(context,
//                              lpPaymentSystem->track2,
//                              &len);
//    lpPaymentSystem->track2Length = len;
//    st_asn1_finish_context_specific(context, 2);
//  } else
//    lpPaymentSystem->track2Length = 0;
//  
//  st_asn1_finish_sequence(context);
//
//  l = st_asn1_finalize(context);
//  
//  st_asn1_free_context(&context);
//
//  if (l > 0)
//    return SKYETEK_SUCCESS;
//  else
//    return SKYETEK_FAILURE;
//}

TAGIMPL GenericTagImpl = {
  0
//  GenericTag_SelectTag,
//  GenericTag_ReadTagData,
//  GenericTag_WriteTagData,
//  GenericTag_ReadTagConfig,
//  GenericTag_WriteTagConfig,
//  GenericTag_LockTagBlock,
//  GenericTag_ActivateTagType,
//  GenericTag_DeactivateTagType,
//  GenericTag_SetTagBitRate,
//  GenericTag_GetTagInfo,
//  GenericTag_GetLockStatus,
//  GenericTag_KillTag,
//  GenericTag_ReviveTag,
//  GenericTag_EraseTag,
//  GenericTag_FormatTag,
//  GenericTag_DeselectTag,
//  GenericTag_AuthenticateTag,
//  GenericTag_SendTagPassword,
//  GenericTag_GetApplicationIDs,
//  GenericTag_SelectApplication,
//  GenericTag_CreateApplication,
//  GenericTag_DeleteApplication,
//  GenericTag_GetFileIDs,
//  GenericTag_SelectFile,
//  GenericTag_CreateDataFile,
//  GenericTag_CreateValueFile,
//  GenericTag_CreateRecordFile,
//  GenericTag_GetCommonFileSettings,
//  GenericTag_GetDataFileSettings,
//  GenericTag_GetValueFileSettings,
//  GenericTag_GetRecordFileSettings,
//  GenericTag_ChangeFileSettings,
//  GenericTag_ReadFile,
//  GenericTag_WriteFile,
//  GenericTag_DeleteFile,
//  GenericTag_ClearFile,
//  GenericTag_CreditValueFile,
//  GenericTag_DebitValueFile,
//  GenericTag_LimitedCreditValueFile,
//  GenericTag_GetValue,
//  GenericTag_ReadRecords,
//  GenericTag_WriteRecord,
//  GenericTag_CommitTransaction,
//  GenericTag_AbortTransaction,
//  GenericTag_EnableEAS,
//  GenericTag_DisableEAS,
//  GenericTag_ScanEAS,
//  GenericTag_ReadAFI,
//  GenericTag_WriteAFI,
//  GenericTag_ReadDSFID,
//  GenericTag_WriteDSFID,
//  GenericTag_GetKeyVersion,
//  GenericTag_ChangeKey,
//  GenericTag_GetApplicationKeySettings,
//  GenericTag_GetMasterKeySettings,
//  GenericTag_ChangeApplicationKeySettings,
//  GenericTag_ChangeMasterKeySettings,
//  GenericTag_InitializeSecureMemoryTag,
//  GenericTag_SetupSecureMemoryTag,
//  GenericTag_InterfaceSend,
//  GenericTag_TransportSend,
//  GenericTag_InitiatePayment,
//  GenericTag_ComputePayment
};

