/**
 * Iso14443ATag.c
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Implementation of a Generic Tag.
 */
#include "Tag/Tag.h"
#include "Tag/GenericTag.h"
#include "Protocol/Protocol.h"
//#include "../Protocol/asn1.h"

#define ISO14443A_TIMEOUT 5000

/*
// Expects
// SEQUENCE {
//   macAlgorithm           ENUMERATED {
//     HMAC-SHA1(1), HMAC-SHA224(2), HMAC-SHA256(3), HMAC-SHA384(4),
//     HMAC-SHA512(5), HMAC-MD5(6)
//   },
//   macKeyName         [1] OCTET STRING OPTIONAL,
//   macKey                 OCTET STRING,
//   cipherAlgorithm        ENUMERATED OPTIONAL {
//     TDEA-56(18), TDEA-112(34), TDEA-168(66),
//     AES-128(49), AES-192(81), AES-256(97)
//   },
//   cipherKeyName      [2] OCTET STRING OPTIONAL,
//   cipherKey              OCTET STRING OPTIONAL
// }
*/
//SKYETEK_STATUS 
//Iso14443ATag_InitializeSecureMemoryTag(
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
///*
//// Expects
//// SEQUENCE {
////   macKeyName         [1] OCTET STRING OPTIONAL,
////   macKey                 OCTET STRING,
////   cipherKeyName      [2] OCTET STRING OPTIONAL,
////   cipherKey              OCTET STRING OPTIONAL
//// }
//*/
//SKYETEK_STATUS 
//Iso14443ATag_SetupSecureMemoryTag(
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
//  if( lpKeyCipher != NULL && lpKeyCipher->lpData != NULL && lpKeyCipher->lpData->data != NULL && lpKeyCipher->lpData->size > 0 ) {
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
////   interface            ENUMERATED { 7816-4(1) }
////   CHOICE {
////     iso7816-4         [1] SEQUENCE {
////       CLA        OCTET-STRING,
////       INS        OCTET-STRING,
////       P1_P2      OCTET-STRING,
////       DATA       OCTET-STRING OPTIONAL,
////       Le         INTEGER }
////   }
//// }
//// Returns
//// OCTET-STRING
//*/
//SKYETEK_STATUS 
//Iso14443ATag_TransportSend(
//    LPSKYETEK_READER     lpReader,
//    LPSKYETEK_TAG        lpTag,
//    SKYETEK_TRANSPORT    transport,
//    SKYETEK_BLOCK        block,
//    LPSKYETEK_DATA       lpCla,
//    LPSKYETEK_DATA       lpIns,
//    LPSKYETEK_DATA       lpP1p2,
//    LPSKYETEK_DATA       lpData, 
//    unsigned int         le,
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
//      lpCla == NULL || lpCla->data == NULL || lpCla->size == 0 ||
//      lpIns == NULL || lpIns->data == NULL || lpIns->size == 0 ||
//      lpP1p2 == NULL || lpP1p2->data == NULL || lpP1p2->size == 0 ||
//      lpRecvData == NULL )
//    return SKYETEK_INVALID_PARAMETER;
//
//  lpDataS = SkyeTek_AllocateData(1024);
//  if( lpDataS == NULL )
//    return SKYETEK_OUT_OF_MEMORY;
//
//  st_asn1_allocate_context(&context);
//  st_asn1_init(context, ST_ASN1_ENCODE,lpDataS->data,lpDataS->size);
//  st_asn1_start_sequence(context);
//  w = transport;
//  st_asn1_write_enumerated(context,w);
//  if( transport == T7816D4 )
//  {
//    st_asn1_start_context_specific(context, 1);
//    st_asn1_start_sequence(context);
//    st_asn1_write_octet_string(context, lpCla->data, lpCla->size);
//    st_asn1_write_octet_string(context, lpIns->data, lpIns->size);
//    st_asn1_write_octet_string(context, lpP1p2->data, lpP1p2->size);
//    if( lpData != NULL && lpData->data != NULL && lpData->size > 0 )
//      st_asn1_write_octet_string(context, lpData->data, lpData->size);
//    w = le;
//    st_asn1_write_integer(context,w);
//    st_asn1_finish_sequence(context);
//    st_asn1_finish_context_specific(context, 1);
//  }
//  st_asn1_finish_sequence(context);
//	lpDataS->size = st_asn1_finalize(context);
//  st_asn1_free_context(&context);
//  
//  lppi = (LPPROTOCOLIMPL)lpReader->lpProtocol->internal;
//  status = lppi->InterfaceSend(lpReader,lpTag,lpDataS,&lpDataR,5000);
//  if( status != SKYETEK_SUCCESS )
//  {
//    SkyeTek_FreeData(lpDataS);
//    return status;
//  }
//  if( lpDataR == NULL || lpDataR->data == NULL || lpDataR->size == 0 )
//    return SKYETEK_SUCCESS;
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

TAGIMPL Iso14443ATagImpl = {
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
//  Iso14443ATag_InitializeSecureMemoryTag,
//  Iso14443ATag_SetupSecureMemoryTag,
//  GenericTag_InterfaceSend,
//  Iso14443ATag_TransportSend,
//  GenericTag_InitiatePayment,
//  GenericTag_ComputePayment
};

