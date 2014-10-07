/**
 * DesfireTag.c
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Implementation of a Generic Tag.
 */
#include "Tag.h"
#include "GenericTag.h"
//#include "../Protocol/asn1.h"
#include "Protocol/Protocol.h"

#define DESFIRE_TIMEOUT 500

/*
// Expects
// SEQUENCE {
//   keyNo      INTEGER,
//   key        OCTET-STRING
// }
*/

//SKYETEK_STATUS 
//DesfireTag_AuthenticateTag(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_KEY        lpKey
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  SKYETEK_ADDRESS addr;
//  st_asn1_context context;
//  int64 w;
//  
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      lpKey == NULL || lpKey->lpData->data == NULL || lpKey->lpData->size == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpKey->number;
//  st_asn1_write_integer(context,w);
//  st_asn1_write_octet_string(context, lpKey->lpData->data, lpKey->lpData->size);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//  
//  memset(&addr,0,sizeof(SKYETEK_ADDRESS));
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->AuthenticateTag(lpReader,lpTag,&addr,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Returns SEQUENCE { app-id OCTET-STRING }
//*/
//SKYETEK_STATUS 
//DesfireTag_GetApplicationIDs(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         **ids,
//    unsigned int         *count
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataR = NULL;
//  LPSKYETEK_DATA lpData;
//  st_asn1_context context;
//  unsigned int num = 0, ix = 0;
//  
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || 
//      ids == NULL || count == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetApplicationIDs(lpReader,lpTag,&lpDataR,DESFIRE_TIMEOUT);
//
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//  {
//    *ids = NULL;
//    *count = 0;
//    return SKYETEK_SUCCESS;
//  }
//  
//  lpData = SkyeTek_AllocateData(lpDataR->size);
//  if( lpData == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//  st_asn1_allocate_context(&context);
//
//  /* First count them */
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  while(st_asn1_peek(context) == ST_OCTET_STRING) 
//  {
//    st_asn1_read_octet_string(context, lpData->data, &lpData->size);
//    num++;
//  }
//  st_asn1_finish_sequence(context);
//  SkyeTek_FreeData(lpData);
//
//  /* Allocate ID memory */
//  *ids = (LPSKYETEK_ID *)malloc(num * sizeof(LPSKYETEK_ID));
//  memset(*ids,0,(num*sizeof(LPSKYETEK_ID)));
//
//  /* Then copy out IDs */
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  for( ix = 0; ix < num; ix++ )
//  {
//    (*ids)[ix] = SkyeTek_AllocateID(16); /* 16 byte IDs? */
//    if( (*ids)[ix] == NULL )
//      goto cleanup;
//    st_asn1_read_octet_string(context, (*ids)[ix]->id, &(*ids)[ix]->length);
//  }
//  st_asn1_finish_sequence(context);
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//
//  *count = num;
//  return SKYETEK_SUCCESS;
//
//cleanup:
//  for( num = 0; num < ix; num++ )
//    SkyeTek_FreeID((*ids)[num]);
//  free(*ids);
//  *ids = NULL;
//  *count = 0;
//  return SKYETEK_OUT_OF_MEMORY;
//}
//
///*
//// Expects app-id OCTET-STRING
//*/
//SKYETEK_STATUS 
//DesfireTag_SelectApplication(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         lpId
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpId == NULL ||
//      lpId->id == NULL || lpId->length == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_write_octet_string(context, lpId->id, lpId->length);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->SelectApplication(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Expects
//// SEQUENCE {
////   id                            OCTET-STRING,
////   application-key-settings      SEQUENCE {
////     access-rights                ENUMERATED {
////       key0(0)..key13(13), allowAll(14), denyAll(15) }
////     configuration-frozen         BOOLEAN,
////     auth-req-create-delete-file  BOOLEAN,
////     auth-req-directory-access    BOOLEAN,
////     allow-app-master-key-change  BOOLEAN
////   }
////   num-of-keys                    INTEGER
//// }
//*/
//SKYETEK_STATUS 
//DesfireTag_CreateApplication(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         lpId,
//    LPSKYETEK_APP_KEY_SETTINGS  lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//  int64 w;
//  int i;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpId == NULL ||
//      lpId->id == NULL || lpId->length == 0 || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  st_asn1_write_octet_string(context, lpId->id, lpId->length);
//  st_asn1_start_sequence(context);
//  w = lpSettings->accessRights;
//  st_asn1_write_enumerated(context,w);
//  i = lpSettings->isConfigFrozen;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAuthReqCreateDelFile;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAuthReqDirAccess;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAllowAppMasterKeyChange;
//  st_asn1_write_boolean(context,i);
//  st_asn1_finish_sequence(context);
//  w = lpSettings->numKeys;
//  st_asn1_write_integer(context,w);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->CreateApplication(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//SKYETEK_STATUS 
//DesfireTag_DeleteApplication(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         lpId
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpId == NULL ||
//      lpId->id == NULL || lpId->length == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_write_octet_string(context, lpId->id, lpId->length);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->DeleteApplication(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///* 
//// Returns SEQUENCE { fileId INTEGER } 
//*/
//SKYETEK_STATUS 
//DesfireTag_GetFileIDs(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_ID         **lpFiles,
//    unsigned int         *count
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  unsigned int num = 0, ix = 0;
//  int64 w;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFiles == NULL || count == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetFileIDs(lpReader,lpTag,&lpDataR,DESFIRE_TIMEOUT);
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//  {
//    *lpFiles = NULL;
//    *count = 0;
//    return SKYETEK_SUCCESS;
//  }
//  
//  st_asn1_allocate_context(&context);
//
//  /* First count them */
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  while(st_asn1_peek(context) == ST_INTEGER) 
//  {
//    st_asn1_read_integer(context, &w);
//    num++;
//  }
//  st_asn1_finish_sequence(context);
//
//  /* Allocate ID memory */
//  *lpFiles = (LPSKYETEK_ID *)malloc(num * sizeof(LPSKYETEK_ID));
//  memset(*lpFiles,0,(num*sizeof(LPSKYETEK_ID)));
//
//  /* Then copy out IDs */
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  for( ix = 0; ix < num; ix++ )
//  {
//    (*lpFiles)[ix] = SkyeTek_AllocateID(1); /* 2 byte IDs? */
//    if( (*lpFiles)[ix] == NULL )
//      goto cleanup;
//    st_asn1_read_integer(context, &w);
//    (*lpFiles)[ix]->id[0] = (unsigned char)w;
//  }
//  st_asn1_finish_sequence(context);
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//
//  *count = num;
//  return SKYETEK_SUCCESS;
//
//cleanup:
//  for( num = 0; num < ix; num++ )
//    SkyeTek_FreeID((*lpFiles)[num]);
//  free(*lpFiles);
//  *lpFiles = NULL;
//  *count = 0;
//  return SKYETEK_OUT_OF_MEMORY;
//}
//
//SKYETEK_STATUS 
//DesfireTag_SelectFile(
//    LPSKYETEK_READER lpReader,
//    LPSKYETEK_TAG lpTag,
//    LPSKYETEK_ID lpFile
//    )
//{
//  return SKYETEK_NOT_SUPPORTED;
//}
//
///*
//// Expects
//// SEQUENCE {
////   fileId     INTEGER,
////   fileType                ENUMERATED {
////     standardDataFile(0),
////     backupDataFile(1),
////     valueFile(2),
////     linearRecordFile(3),
////     cyclicRecordFile(4) }
////   communicationSettings   ENUMERATED {
////     plain(0),
////     plainWithMacing(1),
////     encrypted(3) }
////   accessRights            SEQUENCE {
////     readAccess              ENUMERATED {
////       key0(0)..key13(13), allowAll(14), denyAll(15) }
////     writeAccess             ENUMERATED {
////       key0(0)..key13(13), allowAll(14), denyAll(15) }
////     readWriteAccess         ENUMERATED {
////       key0(0)..key13(13), allowAll(14), denyAll(15) }
////     changeAccess            ENUMERATED {
////       key0(0)..key13(13), allowAll(14), denyAll(15) }}
////   CHOICE {
////     dataFile              [1] SEQUENCE {
////       fileSize                  INTEGER }
////     valueFile             [2] SEQUENCE {
////       lowerLimit                INTEGER,
////       upperLimit                INTEGER,
////       value                     INTEGER,
////       limitedCreditEnabled      BOOLEAN }
////     recordFile            [3] SEQUENCE {
////       recordSize                INTEGER,
////       maxRecords                INTEGER }}
//// }
//*/
//SKYETEK_STATUS 
//DesfireTag_CreateDataFile(
//    LPSKYETEK_READER             lpReader,
//    LPSKYETEK_TAG                lpTag,
//    LPSKYETEK_ID                 lpFile,
//    LPSKYETEK_DATA_FILE_SETTINGS lpSettings
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpSettings == NULL ||
//      (lpSettings->type != STANDARD_DATA_FILE && 
//       lpSettings->type != BACKUP_DATA_FILE) )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//  w = lpSettings->type;
//  st_asn1_write_enumerated(context,w);
//  w = lpSettings->comm;
//  st_asn1_write_enumerated(context,w);
//  st_asn1_start_sequence(context);
//  w = lpSettings->readAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->writeAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->readWriteAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->changeAccess;
//  st_asn1_write_enumerated(context, w);
//  st_asn1_finish_sequence(context);
//
//  /*
//  //     dataFile              [1] SEQUENCE {
//  //       fileSize                  INTEGER }
//  */
//  st_asn1_start_context_specific(context, 1);
//  st_asn1_start_sequence(context);
//  w = lpSettings->fileSize;
//  st_asn1_write_integer(context,w);
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 1);
//
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->CreateFile(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//SKYETEK_STATUS 
//DesfireTag_CreateValueFile(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_VALUE_FILE_SETTINGS    lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//  int64 w;
//  int i;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpSettings == NULL ||
//      lpSettings->type != VALUE_FILE  )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//  w = lpSettings->type;
//  st_asn1_write_enumerated(context,w);
//  w = lpSettings->comm;
//  st_asn1_write_enumerated(context,w);
//  st_asn1_start_sequence(context);
//  w = lpSettings->readAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->writeAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->readWriteAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->changeAccess;
//  st_asn1_write_enumerated(context, w);
//  st_asn1_finish_sequence(context);
//
//  /*
//  //     valueFile             [2] SEQUENCE {
//  //       lowerLimit                INTEGER,
//  //       upperLimit                INTEGER,
//  //       value                     INTEGER,
//  //       limitedCreditEnabled      BOOLEAN }
//  */
//  st_asn1_start_context_specific(context, 2);
//  st_asn1_start_sequence(context);
//  w = lpSettings->lowerLimit;
//  st_asn1_write_integer(context,w);
//  w = lpSettings->upperLimit;
//  st_asn1_write_integer(context,w);
//  w = lpSettings->value;
//  st_asn1_write_integer(context,w);
//  i = lpSettings->limitedCreditEnabled;
//  st_asn1_write_boolean(context,i);
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 2);
//
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->CreateFile(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//SKYETEK_STATUS 
//DesfireTag_CreateRecordFile(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_RECORD_FILE_SETTINGS   lpSettings
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpSettings == NULL ||
//      (lpSettings->type != LINEAR_RECORD_FILE  && lpSettings->type != CYCLICAL_RECORD_FILE))
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//  w = lpSettings->type;
//  st_asn1_write_enumerated(context,w);
//  w = lpSettings->comm;
//  st_asn1_write_enumerated(context,w);
//  st_asn1_start_sequence(context);
//  w = lpSettings->readAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->writeAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->readWriteAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->changeAccess;
//  st_asn1_write_enumerated(context, w);
//  st_asn1_finish_sequence(context);
//
//  /*
//  //     recordFile            [3] SEQUENCE {
//  //       recordSize                INTEGER,
//  //       maxRecords                INTEGER }}
//  */
//  st_asn1_start_context_specific(context, 3);
//  st_asn1_start_sequence(context);
//  w = lpSettings->recordSize;
//  st_asn1_write_integer(context,w);
//  w = lpSettings->maxRecords;
//  st_asn1_write_integer(context,w);
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 3);
//
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->CreateFile(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Expects 
////   fileId INTEGER
//// Returns
////   SEQUENCE {
////     fileType                ENUMERATED {
////       standardDataFile(0),
////       backupDataFile(1),
////       valueFile(2),
////       linearRecordFile(3),
////       cyclicRecordFile(4) }
////     communicationSettings   ENUMERATED {
////       plain(0),
////       plainWithMacing(1),
////       encrypted(3) }
////     accessRights            SEQUENCE {
////       readAccess              ENUMERATED {
////         key0(0)..key13(13), allowAll(14), denyAll(15) }
////       writeAccess             ENUMERATED {
////         key0(0)..key13(13), allowAll(14), denyAll(15) }
////       readWriteAccess         ENUMERATED {
////         key0(0)..key13(13), allowAll(14), denyAll(15) }
////       changeAccess            ENUMERATED {
////         key0(0)..key13(13), allowAll(14), denyAll(15) }}
////   CHOICE {
////     dataFile              [1] SEQUENCE { fileSize INTEGER }
////     valueFile             [2] SEQUENCE {
////       lowerLimit                INTEGER,
////       upperLimit                INTEGER,
////       limitedCreditValue        INTEGER,
////       limitedCreditEnabled      BOOLEAN }
////     recordFile            [3] SEQUENCE {
////       recordSize                INTEGER,
////       maxRecords                INTEGER,
////       numRecords                INTEGER }}
//// }
//*/
//SKYETEK_STATUS 
//DesfireTag_GetCommonFileSettings(
//    LPSKYETEK_READER             lpReader,
//    LPSKYETEK_TAG                lpTag,
//    LPSKYETEK_ID                 lpFile,
//    LPSKYETEK_FILE_SETTINGS      lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  unsigned int num = 0, ix = 0;
//  int64 w;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetFileSettings(lpReader,lpTag,lpDataS,&lpDataR,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  /*
//  //     fileType ENUMERATED 
//  //     communicationSettings ENUMERATED 
//  //     accessRights  SEQUENCE {
//  //       readAccess ENUMERATED 
//  //       writeAccess ENUMERATED 
//  //       readWriteAccess ENUMERATED 
//  //       changeAccess ENUMERATED }
//  */
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->type = (SKYETEK_FILE_TYPE)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->comm = (SKYETEK_COMM_SETTINGS)w;
//  
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->readAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->writeAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->readWriteAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->changeAccess = (SKYETEK_ACCESS)w;
//  st_asn1_finish_sequence(context);
//
//  st_asn1_finish_sequence(context);
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//  return SKYETEK_SUCCESS;
//}
//
//SKYETEK_STATUS 
//DesfireTag_GetDataFileSettings(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_DATA_FILE_SETTINGS     lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  unsigned int num = 0, ix = 0;
//  int64 w;
//  int typ;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetFileSettings(lpReader,lpTag,lpDataS,&lpDataR,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->type = (SKYETEK_FILE_TYPE)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->comm = (SKYETEK_COMM_SETTINGS)w;
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->readAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->writeAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->readWriteAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->changeAccess = (SKYETEK_ACCESS)w;
//  st_asn1_finish_sequence(context);
//
//  /*
//  //     dataFile [1] SEQUENCE { fileSize INTEGER }
//  */
//  typ = st_asn1_peek(context);
//  if( -typ != 1 )
//  {
//    st_asn1_free_context(&context);
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_INVALID_FILE_TYPE;
//  }
//  st_asn1_start_context_specific(context, 1);
//  st_asn1_start_sequence(context);
//  st_asn1_read_integer(context, &w);
//  lpSettings->fileSize = (unsigned int)w;
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 1);
//  st_asn1_finish_sequence(context);
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//  return SKYETEK_SUCCESS;
//}
//
//SKYETEK_STATUS 
//DesfireTag_GetValueFileSettings(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_VALUE_FILE_SETTINGS    lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  unsigned int num = 0, ix = 0;
//  int64 w;
//  int typ, i;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetFileSettings(lpReader,lpTag,lpDataS,&lpDataR,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->type = (SKYETEK_FILE_TYPE)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->comm = (SKYETEK_COMM_SETTINGS)w;
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->readAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->writeAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->readWriteAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->changeAccess = (SKYETEK_ACCESS)w;
//  st_asn1_finish_sequence(context);
//
//  /*
//  //     valueFile             [2] SEQUENCE {
//  //       lowerLimit                INTEGER,
//  //       upperLimit                INTEGER,
//  //       limitedCreditValue        INTEGER,
//  //       limitedCreditEnabled      BOOLEAN }
//  */
//  typ = st_asn1_peek(context);
//  if( -typ != 2 )
//  {
//    st_asn1_free_context(&context);
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_INVALID_FILE_TYPE;
//  }
//  st_asn1_start_context_specific(context, 1);
//  st_asn1_start_sequence(context);
//  st_asn1_read_integer(context, &w);
//  lpSettings->lowerLimit = (unsigned int)w;
//  st_asn1_read_integer(context, &w);
//  lpSettings->upperLimit = (unsigned int)w;
//  st_asn1_read_integer(context, &w);
//  lpSettings->value = (unsigned int)w;
//  st_asn1_read_boolean(context, &i);
//  lpSettings->limitedCreditEnabled = (unsigned char)i;
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 1);
//  st_asn1_finish_sequence(context);
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//  return SKYETEK_SUCCESS;
//}
//
//SKYETEK_STATUS 
//DesfireTag_GetRecordFileSettings(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_ID                     lpFile,
//    LPSKYETEK_RECORD_FILE_SETTINGS   lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  unsigned int num = 0, ix = 0;
//  int64 w;
//  int typ;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetFileSettings(lpReader,lpTag,lpDataS,&lpDataR,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->type = (SKYETEK_FILE_TYPE)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->comm = (SKYETEK_COMM_SETTINGS)w;
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->readAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->writeAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->readWriteAccess = (SKYETEK_ACCESS)w;
//  st_asn1_read_enumerated(context, &w);
//  lpSettings->changeAccess = (SKYETEK_ACCESS)w;
//  st_asn1_finish_sequence(context);
//
//  /*
//  //     recordFile            [3] SEQUENCE {
//  //       recordSize                INTEGER,
//  //       maxRecords                INTEGER,
//  //       numRecords                INTEGER }}
//  */
//  typ = st_asn1_peek(context);
//  if( -typ != 3 )
//  {
//    st_asn1_free_context(&context);
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_INVALID_FILE_TYPE;
//  }
//  st_asn1_start_context_specific(context, 1);
//  st_asn1_start_sequence(context);
//  st_asn1_read_integer(context, &w);
//  lpSettings->recordSize = (unsigned int)w;
//  st_asn1_read_integer(context, &w);
//  lpSettings->maxRecords = (unsigned int)w;
//  st_asn1_read_integer(context, &w);
//  lpSettings->numRecords = (unsigned int)w;
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 1);  
//  st_asn1_finish_sequence(context);
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//  return SKYETEK_SUCCESS;
//}
//
//SKYETEK_STATUS 
//DesfireTag_ChangeFileSettings(
//    LPSKYETEK_READER             lpReader,
//    LPSKYETEK_TAG                lpTag,
//    LPSKYETEK_ID                 lpFile,
//    LPSKYETEK_FILE_SETTINGS      lpSettings
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//  w = lpSettings->comm;
//  st_asn1_write_enumerated(context,w);
//  st_asn1_start_sequence(context);
//  w = lpSettings->readAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->writeAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->readWriteAccess;
//  st_asn1_write_enumerated(context, w);
//  w = lpSettings->changeAccess;
//  st_asn1_write_enumerated(context, w);
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ChangeFileSettings(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Expects
//// SEQUENCE {
////   fileId     INTEGER,
////   offset     INTEGER
//// }
//// Returns
//// OCTET-STRING
//*/
//SKYETEK_STATUS 
//DesfireTag_ReadFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       *lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  LPSKYETEK_DATA lpDataR = NULL;
//  SKYETEK_ADDRESS addr;
//  st_asn1_context context;
//  int64 w;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpAddr == NULL || lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//  w = lpAddr->start;
//  st_asn1_write_integer(context,w);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  memset(&addr,0,sizeof(SKYETEK_ADDRESS));
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ReadFile(lpReader,lpTag,&addr,lpDataS,&lpDataR,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  *lpData = SkyeTek_AllocateData(lpDataR->size);
//  if( *lpData == NULL )
//  {
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_OUT_OF_MEMORY;
//  }
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_read_octet_string(context, (*lpData)->data, &(*lpData)->size);
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//  return SKYETEK_SUCCESS;
//}
//    	
///*
//// Expects
//// SEQUENCE {
////   fileId     INTEGER,
////   offset     INTEGER,
////   data       OCTET-STRING
//// }
//*/
//SKYETEK_STATUS 
//DesfireTag_WriteFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  SKYETEK_ADDRESS addr;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//  int64 w;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpAddr == NULL || 
//      lpData == NULL || lpData->data == NULL || lpData->size == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//  w = lpAddr->start;
//  st_asn1_write_integer(context,w);
//  st_asn1_write_octet_string(context,lpData->data,lpData->size);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  memset(&addr,0,sizeof(SKYETEK_ADDRESS));
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->WriteFile(lpReader,lpTag,&addr,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Expects file-id INTEGER
//*/
//SKYETEK_STATUS 
//DesfireTag_DeleteFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context, w);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->DeleteFile(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//SKYETEK_STATUS 
//DesfireTag_ClearFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context, w);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ClearFile(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Expects SEQUENCE { fileId INTEGER, value INTEGER }
//*/
//SKYETEK_STATUS 
//DesfireTag_CreditValueFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    unsigned int         amount
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context, w);
//  w = amount;
//  st_asn1_write_integer(context, w);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->CreditValueFile(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//SKYETEK_STATUS 
//DesfireTag_DebitValueFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    unsigned int         amount
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context, w);
//  w = amount;
//  st_asn1_write_integer(context, w);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->DebitValueFile(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//SKYETEK_STATUS 
//DesfireTag_LimitedCreditValueFile(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    unsigned int         amount
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context, w);
//  w = amount;
//  st_asn1_write_integer(context, w);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->LimitedCreditValueFile(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Expects file-id INTEGER, returns value INTEGER
//*/
//SKYETEK_STATUS 
//DesfireTag_GetValue(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    unsigned int         *value
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || value == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetValue(lpReader,lpTag,lpDataS,&lpDataR,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_read_integer(context, &w);
//  *value = (unsigned int)w;
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//  return SKYETEK_SUCCESS;
//}
//
///*
//// Expects
//// SEQUENCE {
////   fileId     INTEGER,
////   offset     INTEGER
//// }
//// Returns
//// OCTET-STRING
//*/
//SKYETEK_STATUS 
//DesfireTag_ReadRecords(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       *lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  SKYETEK_ADDRESS addr;
//  LPSKYETEK_DATA lpDataS;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  int64 w;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpAddr == NULL || lpData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//  w = lpAddr->start;
//  st_asn1_write_integer(context,w);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//
//  memset(&addr,0,sizeof(SKYETEK_ADDRESS));
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ReadRecords(lpReader,lpTag,&addr,lpDataS,&lpDataR,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  *lpData = SkyeTek_AllocateData(lpDataR->size);
//  if( *lpData == NULL )
//  {
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_OUT_OF_MEMORY;
//  }
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_read_octet_string(context, (*lpData)->data, &(*lpData)->size);
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//  return SKYETEK_SUCCESS;
//}
//  
///*
//// Expects
//// SEQUENCE {
////   fileId     INTEGER,
////   offset     INTEGER,
////   data       OCTET-STRING
//// }
//*/  	
//SKYETEK_STATUS 
//DesfireTag_WriteRecord(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_TAG        lpTag, 
//    LPSKYETEK_ID         lpFile,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       lpData
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  SKYETEK_ADDRESS addr;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//  int64 w;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpFile == NULL ||
//      lpFile->id == NULL || lpFile->length == 0 || lpAddr == NULL || 
//      lpData == NULL || lpData->data == NULL || lpData->size == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpFile->id[0];
//  st_asn1_write_integer(context,w);
//  w = lpAddr->start;
//  st_asn1_write_integer(context,w);
//  st_asn1_write_octet_string(context,lpData->data,lpData->size);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  memset(&addr,0,sizeof(SKYETEK_ADDRESS));
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->WriteRecord(lpReader,lpTag,&addr,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//SKYETEK_STATUS 
//DesfireTag_CommitTransaction(
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
//  return lppi->CommitTransaction(lpReader,lpTag,DESFIRE_TIMEOUT);
//}
//
//SKYETEK_STATUS 
//DesfireTag_AbortTransaction(
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
//  return lppi->AbortTransaction(lpReader,lpTag,DESFIRE_TIMEOUT);
//}
//
///*
//// Expects keyNo INTEGER, returns version INTEGER
//*/
//SKYETEK_STATUS 
//DesfireTag_GetKeyVersion(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_KEY        lpKey
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpKey == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  w = lpKey->number;
//  st_asn1_write_integer(context,w);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetKeyVersion(lpReader,lpTag,lpDataS,&lpDataR,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataR->data,lpDataR->size);
//  st_asn1_read_integer(context, &w);
//  lpKey->version = (unsigned char)w;
//  st_asn1_free_context(&context);
//  SkyeTek_FreeData(lpDataR);
//  return SKYETEK_SUCCESS;
//}
//
///*
//// Expects
//// SEQUENCE {
////   keyNo      INTEGER
////   keyVersion INTEGER
////   key        OCTET-STRING
////   currentKey OCTET-STRING OPTIONAL
//// }
//*/
//SKYETEK_STATUS 
//DesfireTag_ChangeKey(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    LPSKYETEK_KEY        lpNewKey,
//    LPSKYETEK_KEY        lpCurrentKey
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
//      lpReader->lpDevice == NULL || lpTag == NULL || lpNewKey == NULL ||
//      lpNewKey->lpData->data == NULL || lpNewKey->lpData->size == 0 )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = lpNewKey->number;
//  st_asn1_write_integer(context,w);
//  w = lpNewKey->version;
//  st_asn1_write_integer(context,w);
//  st_asn1_write_octet_string(context,lpNewKey->lpData->data,lpNewKey->lpData->size);
//  if( lpCurrentKey != NULL && lpCurrentKey->lpData != NULL && 
//      lpCurrentKey->lpData->data != NULL && lpCurrentKey->lpData->size > 0 )
//    st_asn1_write_octet_string(context,lpCurrentKey->lpData->data,lpCurrentKey->lpData->size);
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
// 
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ChangeKey(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Returns
//// SEQUENCE {
////   CHOICE {
////     application-key-settings  [1] SEQUENCE {
////       access-rights                ENUMERATED {
////         key0(0)..key13(13), allowAll(14), denyAll(15) }
////       configuration-frozen         BOOLEAN,
////       auth-req-create-delete-file  BOOLEAN,
////       auth-req-directory-access    BOOLEAN,
////       allow-app-master-key-change  BOOLEAN
////     }
////     master-key-settings       [2] SEQUENCE {
////       configuration-frozen           BOOLEAN,
////       auth-req-create-delete-app     BOOLEAN,
////       auth-req-app-directory-access  BOOLEAN,
////       allow-picc-master-key-change   BOOLEAN
////     }
////   },
////   numOfKeys                        INTEGER
//// }
//*/
//SKYETEK_STATUS 
//DesfireTag_GetApplicationKeySettings(
//    LPSKYETEK_READER              lpReader,
//    LPSKYETEK_TAG                 lpTag,
//    LPSKYETEK_APP_KEY_SETTINGS    lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  int64 w;
//  int i, typ;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetKeySettings(lpReader,lpTag,&lpDataR,DESFIRE_TIMEOUT);
//
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_DECODE,lpDataR->data,lpDataR->size);
//  st_asn1_start_sequence(context);
//  typ = st_asn1_peek(context);
//  if( -typ != 1 )
//  {
//    st_asn1_free_context(&context);
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_NO_APPLICATION_PRESENT;
//  }
//  st_asn1_start_context_specific(context, 1);
//  st_asn1_start_sequence(context);
//  st_asn1_read_enumerated(context,&w);
//  lpSettings->accessRights = (SKYETEK_ACCESS)w;
//  st_asn1_read_boolean(context,&i);
//  lpSettings->isConfigFrozen = (unsigned char)i;
//  st_asn1_read_boolean(context,&i);
//  lpSettings->isAuthReqCreateDelFile = (unsigned char)i;
//  st_asn1_read_boolean(context,&i);
//  lpSettings->isAuthReqDirAccess = (unsigned char)i;
//  st_asn1_read_boolean(context,&i);
//  lpSettings->isAllowAppMasterKeyChange = (unsigned char)i;
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 1); 
//  st_asn1_read_integer(context,&w);
//  lpSettings->numKeys = (unsigned int)w;
//  st_asn1_finish_sequence(context);
//  st_asn1_free_context(&context);
//
//  SkyeTek_FreeData(lpDataR);
//  return status;
//}
//
//SKYETEK_STATUS 
//DesfireTag_GetMasterKeySettings(
//    LPSKYETEK_READER                lpReader,
//    LPSKYETEK_TAG                   lpTag,
//    LPSKYETEK_MASTER_KEY_SETTINGS   lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataR = NULL;
//  st_asn1_context context;
//  int64 w;
//  int i, typ;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->GetKeySettings(lpReader,lpTag,&lpDataR,DESFIRE_TIMEOUT);
//
//  if( status != SKYETEK_SUCCESS )
//    return status;
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_DECODE,lpDataR->data,lpDataR->size);
//  
//  st_asn1_start_sequence(context);
//  typ = st_asn1_peek(context);
//  if( -typ != 2 )
//  {
//    st_asn1_free_context(&context);
//    SkyeTek_FreeData(lpDataR);
//    return SKYETEK_READER_PROTOCOL_ERROR;
//  }
//  st_asn1_start_context_specific(context, 2);
//  st_asn1_start_sequence(context);
//  st_asn1_read_boolean(context,&i);
//  lpSettings->isConfigFrozen = (unsigned char)i;
//  st_asn1_read_boolean(context,&i);
//  lpSettings->isAuthReqCreateDelApp = (unsigned char)i;
//  st_asn1_read_boolean(context,&i);
//  lpSettings->isAuthReqAppDirAccess = (unsigned char)i;
//  st_asn1_read_boolean(context,&i);
//  lpSettings->isAllowPiccMasterKeyChange = (unsigned char)i;
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 2); 
//  st_asn1_read_integer(context,&w);
//  lpSettings->numKeys = (unsigned int)w;
//  st_asn1_finish_sequence(context);
//  st_asn1_free_context(&context);
//
//  SkyeTek_FreeData(lpDataR);
//  return status;
//}
//
//
///*
//// Expects
//// CHOICE {
////   application-key-settings  [1] SEQUENCE {
////     access-rights                ENUMERATED {
////       key0(0)..key13(13), allowAll(14), denyAll(15) }
////     configuration-frozen         BOOLEAN,
////     auth-req-create-delete-file  BOOLEAN,
////     auth-req-directory-access    BOOLEAN,
////     allow-app-master-key-change  BOOLEAN
////   }
//// }
//*/
//SKYETEK_STATUS 
//DesfireTag_ChangeApplicationKeySettings(
//    LPSKYETEK_READER             lpReader,
//    LPSKYETEK_TAG                lpTag,
//    LPSKYETEK_APP_KEY_SETTINGS   lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//  int64 w;
//  int i;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_context_specific(context, 1);
//  st_asn1_start_sequence(context);
//  w = lpSettings->accessRights;
//  st_asn1_write_enumerated(context,w);
//  i = lpSettings->isConfigFrozen;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAuthReqCreateDelFile;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAuthReqDirAccess;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAllowAppMasterKeyChange;
//  st_asn1_write_boolean(context,i);
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 1);  
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ChangeKeySettings(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
///*
//// Expects
//// CHOICE {
////   master-key-settings       [2] SEQUENCE {
////     configuration-frozen           BOOLEAN,
////     auth-req-create-delete-app     BOOLEAN,
////     auth-req-app-directory-access  BOOLEAN,
////     allow-picc-master-key-change   BOOLEAN
////   }
//// }
//*/
//SKYETEK_STATUS 
//DesfireTag_ChangeMasterKeySettings(
//    LPSKYETEK_READER                 lpReader,
//    LPSKYETEK_TAG                    lpTag,
//    LPSKYETEK_MASTER_KEY_SETTINGS    lpSettings
//    )
//{
//  LPPROTOCOLIMPL lppi;
//  SKYETEK_STATUS status;
//  LPSKYETEK_DATA lpDataS;
//  st_asn1_context context;
//  int i;
//
//  if( lpReader == NULL || lpReader->lpProtocol == NULL || 
//      lpReader->lpProtocol->internal == NULL ||
//      lpReader->lpDevice == NULL || lpTag == NULL || lpSettings == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_context_specific(context, 2);
//  st_asn1_start_sequence(context);
//  i = lpSettings->isConfigFrozen;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAuthReqCreateDelApp;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAuthReqAppDirAccess;
//  st_asn1_write_boolean(context,i);
//  i = lpSettings->isAllowPiccMasterKeyChange;
//  st_asn1_write_boolean(context,i);
//  st_asn1_finish_sequence(context);
//  st_asn1_finish_context_specific(context, 2);  
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->ChangeKeySettings(lpReader,lpTag,lpDataS,DESFIRE_TIMEOUT);
//  SkyeTek_FreeData(lpDataS);
//  return status;
//}
//
//extern SKYETEK_STATUS 
//Iso14443ATag_InitializeSecureMemoryTag(
//    LPSKYETEK_READER          lpReader,
//    LPSKYETEK_TAG             lpTag,
//    SKYETEK_HMAC_ALGORITHM    hmac,
//    LPSKYETEK_KEY             lpKeyHMAC,
//    SKYETEK_CIPHER_ALGORITHM  cipher,
//    LPSKYETEK_KEY             lpKeyCipher,
//		int                       useKeyDerivationFunction
//    );
//
//extern SKYETEK_STATUS 
//Iso14443ATag_SetupSecureMemoryTag(
//    LPSKYETEK_READER          lpReader,
//    LPSKYETEK_TAG             lpTag,
//    LPSKYETEK_KEY             lpKeyHMAC,
//    LPSKYETEK_KEY             lpKeyCipher,
//		int                       useKeyDerivationFunction
//    );
//
//extern SKYETEK_STATUS 
//Iso14443ATag_InterfaceSend(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    SKYETEK_INTERFACE    intf,
//    SKYETEK_BLOCK        block,
//    LPSKYETEK_DATA       lpSendData,
//    LPSKYETEK_DATA       *lpRecvData
//    );
//
//extern SKYETEK_STATUS 
//Iso14443ATag_TransportSend(
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
//    );

TAGIMPL DesfireTagImpl = {
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
//  DesfireTag_AuthenticateTag,
//  GenericTag_SendTagPassword,
//  DesfireTag_GetApplicationIDs,
//  DesfireTag_SelectApplication,
//  DesfireTag_CreateApplication,
//  DesfireTag_DeleteApplication,
//  DesfireTag_GetFileIDs,
//  DesfireTag_SelectFile,
//  DesfireTag_CreateDataFile,
//  DesfireTag_CreateValueFile,
//  DesfireTag_CreateRecordFile,
//  DesfireTag_GetCommonFileSettings,
//  DesfireTag_GetDataFileSettings,
//  DesfireTag_GetValueFileSettings,
//  DesfireTag_GetRecordFileSettings,
//  DesfireTag_ChangeFileSettings,
//  DesfireTag_ReadFile,
//  DesfireTag_WriteFile,
//  DesfireTag_DeleteFile,
//  DesfireTag_ClearFile,
//  DesfireTag_CreditValueFile,
//  DesfireTag_DebitValueFile,
//  DesfireTag_LimitedCreditValueFile,
//  DesfireTag_GetValue,
//  DesfireTag_ReadRecords,
//  DesfireTag_WriteRecord,
//  DesfireTag_CommitTransaction,
//  DesfireTag_AbortTransaction,
//  GenericTag_EnableEAS,
//  GenericTag_DisableEAS,
//  GenericTag_ScanEAS,
//  GenericTag_ReadAFI,
//  GenericTag_WriteAFI,
//  GenericTag_ReadDSFID,
//  GenericTag_WriteDSFID,
//  DesfireTag_GetKeyVersion,
//  DesfireTag_ChangeKey,
//  DesfireTag_GetApplicationKeySettings,
//  DesfireTag_GetMasterKeySettings,
//  DesfireTag_ChangeApplicationKeySettings,
//  DesfireTag_ChangeMasterKeySettings,
//  Iso14443ATag_InitializeSecureMemoryTag,
//  Iso14443ATag_SetupSecureMemoryTag,
//  GenericTag_InterfaceSend,
//  Iso14443ATag_TransportSend,
//  GenericTag_InitiatePayment,
//  GenericTag_ComputePayment
};

