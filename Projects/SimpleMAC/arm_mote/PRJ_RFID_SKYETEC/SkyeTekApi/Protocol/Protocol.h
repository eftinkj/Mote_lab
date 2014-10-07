/**
 * Protocol.h
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Protocol implementation.
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "SkyeTekAPI.h"

typedef struct PROTOCOL_FLAGS
{
  unsigned char isInventory;
  unsigned char isLoop;
  unsigned char isEncrypt;
  unsigned char isHMAC;
  unsigned char isLock;
} PROTOCOL_FLAGS, *LPPROTOCOL_FLAGS;

typedef struct TAGTYPE_ARRAY
{
  SKYETEK_TAGTYPE type;
} TAGTYPE_ARRAY, *LPTAGTYPE_ARRAY;

/**
 * Tag select callback used by inventory and loop modes.
 * Note that the callback function should copy any memory
 * from lpData before returning; it will be freed by the caller.
 * @param type Tag type
 * @param lpData Data pointer
 * @param user User data
 * @return 0 to stop inventory/loop, 1 to continue
 */ 
typedef unsigned char 
(*PROTOCOL_TAG_SELECT_CALLBACK)(
    SKYETEK_TAGTYPE type,
    LPSKYETEK_DATA lpData,
    void  *user
    );

typedef struct PROTOCOLIMPL 
{
  /** Protocol version */
  unsigned char version;

  /* Reader Functions */
//  SKYETEK_STATUS 
//  (*SelectTags)(
//    LPSKYETEK_READER             lpReader, 
//    SKYETEK_TAGTYPE              tagType, 
//    PROTOCOL_TAG_SELECT_CALLBACK callback,
//    PROTOCOL_FLAGS               flags,
//    void                         *user,
//    unsigned int                 timeout
//    );

  SKYETEK_STATUS 
  (*GetTags)(
    LPSKYETEK_READER   lpReader, 
    SKYETEK_TAGTYPE    tagType, 
    LPTAGTYPE_ARRAY    tagTypes, 
    LPSKYETEK_DATA     lpData, 
    unsigned int       *count,
    unsigned int       timeout
    );

//  SKYETEK_STATUS 
//  (*GetTagsWithMask)(
//    LPSKYETEK_READER   lpReader, 
//    SKYETEK_TAGTYPE    tagType, 
//	LPSKYETEK_ID	   lpTagIdMask,
//    LPTAGTYPE_ARRAY    **tagTypes, 
//    LPSKYETEK_DATA     **lpData, 
//    unsigned int       *count,
//    unsigned int       timeout
//    );

//  SKYETEK_STATUS 
//  (*StoreKey)(
//    LPSKYETEK_READER     lpReader,
//    SKYETEK_TAGTYPE      type,
//    LPSKYETEK_ADDRESS    lpAddr,
//    LPSKYETEK_DATA       lpData,
//    unsigned int         timeout
//    );
//
//  SKYETEK_STATUS 
//  (*LoadKey)(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_ADDRESS    lpAddr,
//    unsigned int         timeout
//    );
//
//  SKYETEK_STATUS 
//  (*LoadDefaults)(
//    LPSKYETEK_READER     lpReader,
//    unsigned int         timeout
//    );
//
//  SKYETEK_STATUS 
//  (*ResetDevice)(
//    LPSKYETEK_READER     lpReader,
//    unsigned int         timeout
//    );
//
//  SKYETEK_STATUS 
//  (*Bootload)(
//    LPSKYETEK_READER     lpReader,
//    unsigned int         timeout
//    );
//
//  SKYETEK_STATUS 
//  (*GetSystemParameter)(
//    LPSKYETEK_READER             lpReader, 
//    LPSKYETEK_ADDRESS            lpAddr,
//    LPSKYETEK_DATA               *lpData,
//    unsigned int                 timeout
//    );
//
//  SKYETEK_STATUS 
//  (*SetSystemParameter)(
//    LPSKYETEK_READER             lpReader, 
//    LPSKYETEK_ADDRESS            lpAddr,
//    LPSKYETEK_DATA               lpData,
//    unsigned int                 timeout
//    );
//
//  SKYETEK_STATUS 
//  (*GetDefaultSystemParameter)(
//    LPSKYETEK_READER             lpReader, 
//    LPSKYETEK_ADDRESS            lpAddr,
//    LPSKYETEK_DATA               *lpData,
//    unsigned int                 timeout
//    );
//
//  SKYETEK_STATUS 
//  (*SetDefaultSystemParameter)(
//    LPSKYETEK_READER             lpReader, 
//    LPSKYETEK_ADDRESS            lpAddr,
//    LPSKYETEK_DATA               lpData,
//    unsigned int                 timeout
//    );
//
//  SKYETEK_STATUS 
//  (*AuthenticateReader)(
//    LPSKYETEK_READER     lpReader, 
//    LPSKYETEK_DATA       lpData,
//    unsigned int         timeout
//    );
//
//  SKYETEK_STATUS 
//  (*EnableDebug)(
//    LPSKYETEK_READER     lpReader,
//    unsigned int         timeout
//    );
//
//  SKYETEK_STATUS 
//  (*DisableDebug)(
//    LPSKYETEK_READER     lpReader,
//    unsigned int         timeout
//    );
//
//  SKYETEK_STATUS 
//  (*UploadFirmware)(
//    LPSKYETEK_READER                      lpReader,
//    TCHAR                                  *file, 
//    unsigned char                         defaultsOnly,
//    SKYETEK_FIRMWARE_UPLOAD_CALLBACK      callback, 
//    void                                  *user
//    );
//
//  SKYETEK_STATUS 
//  (*EnterPaymentScanMode)(
//    LPSKYETEK_READER     lpReader,
//    unsigned int timeout
//    );
//
//  SKYETEK_STATUS 
//  (*ScanPayments)(
//    LPSKYETEK_READER              lpReader,
//    SKYETEK_PAYMENT_SCAN_CALLBACK callback,
//    void                          *user,
//    unsigned int                  timeout
//    );

  /* Tag functions */
  SKYETEK_STATUS 
  (*SelectTag)(
      LPSKYETEK_READER     lpReader, 
      LPSKYETEK_TAG        lpTag,
      unsigned int         timeout
      );

  SKYETEK_STATUS 
  (*ReadTagData)(    
      LPSKYETEK_READER     lpReader, 
      LPSKYETEK_TAG        lpTag,
      PROTOCOL_FLAGS       flags,
      LPSKYETEK_ADDRESS    lpAddr,
      LPSKYETEK_DATA       *lpData,
      unsigned int         timeout
      );

  SKYETEK_STATUS 
  (*WriteTagData)(
      LPSKYETEK_READER     lpReader, 
      LPSKYETEK_TAG        lpTag,
      PROTOCOL_FLAGS       flags,
      LPSKYETEK_ADDRESS    lpAddr,
      LPSKYETEK_DATA       lpData,
      unsigned int         timeout
      );

  SKYETEK_STATUS 
  (*ReadTagConfig)(    
      LPSKYETEK_READER     lpReader, 
      LPSKYETEK_TAG        lpTag,
      PROTOCOL_FLAGS       flags,
      LPSKYETEK_ADDRESS    lpAddr,
      LPSKYETEK_DATA       *lpData,
      unsigned int         timeout
      );

  SKYETEK_STATUS 
  (*WriteTagConfig)(
      LPSKYETEK_READER     lpReader, 
      LPSKYETEK_TAG        lpTag,
      PROTOCOL_FLAGS       flags,
      LPSKYETEK_ADDRESS    lpAddr,
      LPSKYETEK_DATA       lpData,
      unsigned int         timeout
      );

//  SKYETEK_STATUS 
//  (*ActivateTagType)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*DeactivateTagType)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );

  SKYETEK_STATUS 
  (*SetTagBitRate)(
      LPSKYETEK_READER     lpReader, 
      LPSKYETEK_TAG        lpTag, 
      LPSKYETEK_DATA       lpData,
      unsigned int         timeout
      );

  SKYETEK_STATUS 
  (*GetTagInfo)(
      LPSKYETEK_READER     lpReader, 
      LPSKYETEK_TAG        lpTag,
      LPSKYETEK_DATA       *lpData,
      unsigned int         timeout
      );

//  SKYETEK_STATUS 
//  (*GetLockStatus)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_ADDRESS    lpAddr,
//      LPSKYETEK_DATA       *lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*KillTag)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ReviveTag)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*EraseTag)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag, 
//      unsigned int         timeout 
//      );
//
//  SKYETEK_STATUS 
//  (*FormatTag)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*DeselectTag)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*AuthenticateTag)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_ADDRESS    lpAddr,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*SendTagPassword)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );

//  SKYETEK_STATUS 
//  (*GetApplicationIDs)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       *lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*SelectApplication)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*CreateApplication)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*DeleteApplication)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*GetFileIDs)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*SelectFile)(
//      LPSKYETEK_READER    lpReader,
//      LPSKYETEK_TAG       lpTag,
//      LPSKYETEK_DATA      lpData,
//      unsigned int        timeout
//      );
//
//  SKYETEK_STATUS 
//  (*CreateFile)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*GetFileSettings)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpSendData,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ChangeFileSettings)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpSendData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ReadFile)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_ADDRESS    lpAddr,
//      LPSKYETEK_DATA       lpSendData,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*WriteFile)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_ADDRESS    lpAddr,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*DeleteFile)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ClearFile)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*CreditValueFile)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//
//  SKYETEK_STATUS 
//  (*DebitValueFile)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*LimitedCreditValueFile)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*GetValue)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       lpSendData,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ReadRecords)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_ADDRESS    lpAddr,
//      LPSKYETEK_DATA       lpSendData,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//    	  
//  SKYETEK_STATUS 
//  (*WriteRecord)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_ADDRESS    lpAddr,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*CommitTransaction)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*AbortTransaction)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*EnableEAS)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*DisableEAS)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ScanEAS)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ReadAFI)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       *lpData,
//      unsigned int         timeout
//      );
//    	  
//  SKYETEK_STATUS 
//  (*WriteAFI)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ReadDSFID)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       *lpData,
//      unsigned int         timeout
//      );
//    	  
//  SKYETEK_STATUS 
//  (*WriteDSFID)(
//      LPSKYETEK_READER     lpReader, 
//      LPSKYETEK_TAG        lpTag, 
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*GetKeyVersion)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpSendData,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ChangeKey)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*GetKeySettings)(
//      LPSKYETEK_READER        lpReader,
//      LPSKYETEK_TAG           lpTag,
//      LPSKYETEK_DATA          *lpRecvData,
//      unsigned int            timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ChangeKeySettings)(
//      LPSKYETEK_READER             lpReader,
//      LPSKYETEK_TAG                lpTag,
//      LPSKYETEK_DATA               lpData,
//      unsigned int                 timeout
//      );
//
//  SKYETEK_STATUS 
//  (*InitializeSecureMemoryTag)(
//      LPSKYETEK_READER          lpReader,
//      LPSKYETEK_TAG             lpTag,
//      LPSKYETEK_DATA            lpData,
//      unsigned int              timeout
//      );
//
//  SKYETEK_STATUS 
//  (*SetupSecureMemoryTag)(
//      LPSKYETEK_READER          lpReader,
//      LPSKYETEK_TAG             lpTag,
//      LPSKYETEK_DATA            lpData,
//      unsigned int              timeout
//      );
//
//  SKYETEK_STATUS 
//  (*InterfaceSend)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpSendData,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*TransportSend)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpSendData,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*InitiatePayment)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*ComputePayment)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_TAG        lpTag,
//      LPSKYETEK_DATA       lpSendData,
//      LPSKYETEK_DATA       *lpRecvData,
//      unsigned int         timeout
//      );
//
//  SKYETEK_STATUS 
//  (*GetDebugMessages)(
//      LPSKYETEK_READER     lpReader,
//      LPSKYETEK_DATA       *lpData,
//      unsigned int         timeout
//      );

} PROTOCOLIMPL, *LPPROTOCOLIMPL;

extern PROTOCOLIMPL STPV3Impl;


#endif
