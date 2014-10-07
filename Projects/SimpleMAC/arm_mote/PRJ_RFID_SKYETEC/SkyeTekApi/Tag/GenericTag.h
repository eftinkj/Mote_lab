/**
 * GenericTag.h
 * Copyright © 2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Allows other functions to inherit generic tag functions.
 */
#ifndef GENERIC_TAG_H
#define GENERIC_TAG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Tag functions */
SKYETEK_STATUS 
GenericTag_SelectTag(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_ReadTagData(    
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ADDRESS    lpAddr,
    unsigned char        encrypt,
    unsigned char        hmac,
    LPSKYETEK_DATA       *lpData
    );

SKYETEK_STATUS 
GenericTag_WriteTagData(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ADDRESS    lpAddr, 
    unsigned char        encrypt,
    unsigned char        hmac,
    LPSKYETEK_DATA       lpData
    );

SKYETEK_STATUS 
GenericTag_ReadTagConfig(    
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ADDRESS    lpAddr,
    LPSKYETEK_DATA       *lpData
    );

SKYETEK_STATUS 
GenericTag_WriteTagConfig(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ADDRESS    lpAddr, 
    LPSKYETEK_DATA       lpData
    );

SKYETEK_STATUS 
GenericTag_LockTagBlock(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ADDRESS    lpAddr,
    LPSKYETEK_DATA       lpData
    );

SKYETEK_STATUS 
GenericTag_ActivateTagType(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_DeactivateTagType(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_SetTagBitRate(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    unsigned char        rate
    );

SKYETEK_STATUS 
GenericTag_GetTagInfo(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_MEMORY     lpMemory
    );

SKYETEK_STATUS 
GenericTag_GetLockStatus(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ADDRESS    lpAddr,
    unsigned char        *status
    );

SKYETEK_STATUS 
GenericTag_KillTag(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_KEY        lpKey
    );

SKYETEK_STATUS 
GenericTag_ReviveTag(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_EraseTag(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_FormatTag(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_DeselectTag(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_AuthenticateTag(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_KEY        lpKey
    );

SKYETEK_STATUS 
GenericTag_SendTagPassword(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_DATA       lpData
    );

SKYETEK_STATUS 
GenericTag_GetApplicationIDs(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ID         **lpIds,
    unsigned int         *count
    );

SKYETEK_STATUS 
GenericTag_SelectApplication(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ID         lpId
    );

SKYETEK_STATUS 
GenericTag_CreateApplication(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ID         lpId,
    LPSKYETEK_APP_KEY_SETTINGS  lpSettings
    );

SKYETEK_STATUS 
GenericTag_DeleteApplication(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ID         lpId
    );

SKYETEK_STATUS 
GenericTag_GetFileIDs(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ID         **lpFiles,
    unsigned int         *count
    );

SKYETEK_STATUS 
GenericTag_SelectFile(
    LPSKYETEK_READER    lpReader,
    LPSKYETEK_TAG       lpTag,
    LPSKYETEK_ID        lpFile
    );

SKYETEK_STATUS 
GenericTag_CreateDataFile(
    LPSKYETEK_READER             lpReader,
    LPSKYETEK_TAG                lpTag,
    LPSKYETEK_ID                 lpFile,
    LPSKYETEK_DATA_FILE_SETTINGS lpSettings
    ); 

SKYETEK_STATUS 
GenericTag_CreateValueFile(
    LPSKYETEK_READER                 lpReader,
    LPSKYETEK_TAG                    lpTag,
    LPSKYETEK_ID                     lpFile,
    LPSKYETEK_VALUE_FILE_SETTINGS    lpSettings
    );

SKYETEK_STATUS 
GenericTag_CreateRecordFile(
    LPSKYETEK_READER                 lpReader,
    LPSKYETEK_TAG                    lpTag,
    LPSKYETEK_ID                     lpFile,
    LPSKYETEK_RECORD_FILE_SETTINGS   lpSettings
    );

SKYETEK_STATUS 
GenericTag_GetCommonFileSettings(
    LPSKYETEK_READER             lpReader,
    LPSKYETEK_TAG                lpTag,
    LPSKYETEK_ID                 lpFile,
    LPSKYETEK_FILE_SETTINGS      lpSettings
    );

SKYETEK_STATUS 
GenericTag_GetDataFileSettings(
    LPSKYETEK_READER                 lpReader,
    LPSKYETEK_TAG                    lpTag,
    LPSKYETEK_ID                     lpFile,
    LPSKYETEK_DATA_FILE_SETTINGS     lpSettings
    );

SKYETEK_STATUS 
GenericTag_GetValueFileSettings(
    LPSKYETEK_READER                 lpReader,
    LPSKYETEK_TAG                    lpTag,
    LPSKYETEK_ID                     lpFile,
    LPSKYETEK_VALUE_FILE_SETTINGS    lpSettings
    );

SKYETEK_STATUS 
GenericTag_GetRecordFileSettings(
    LPSKYETEK_READER                 lpReader,
    LPSKYETEK_TAG                    lpTag,
    LPSKYETEK_ID                     lpFile,
    LPSKYETEK_RECORD_FILE_SETTINGS   lpSettings
    );

SKYETEK_STATUS 
GenericTag_ChangeFileSettings(
    LPSKYETEK_READER             lpReader,
    LPSKYETEK_TAG                lpTag,
    LPSKYETEK_ID                 lpFile,
    LPSKYETEK_FILE_SETTINGS      lpSettings
    );

SKYETEK_STATUS 
GenericTag_ReadFile(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile,
    LPSKYETEK_ADDRESS    lpAddr,
    LPSKYETEK_DATA       *lpData
    );

SKYETEK_STATUS 
GenericTag_WriteFile(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile,
    LPSKYETEK_ADDRESS    lpAddr,
    LPSKYETEK_DATA       lpData
    );

SKYETEK_STATUS 
GenericTag_DeleteFile(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile
    );

SKYETEK_STATUS 
GenericTag_ClearFile(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile
    );

SKYETEK_STATUS 
GenericTag_CreditValueFile(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile,
    unsigned int         amount
    );


SKYETEK_STATUS 
GenericTag_DebitValueFile(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile,
    unsigned int         amount
    );

SKYETEK_STATUS 
GenericTag_LimitedCreditValueFile(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile,
    unsigned int         amount
    );

SKYETEK_STATUS 
GenericTag_GetValue(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile,
    unsigned int         *value
    );

SKYETEK_STATUS 
GenericTag_ReadRecords(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile,
    LPSKYETEK_ADDRESS    lpAddr,
    LPSKYETEK_DATA       *lpData
    );
    	
SKYETEK_STATUS 
GenericTag_WriteRecord(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_ID         lpFile,
    LPSKYETEK_ADDRESS    lpAddr,
    LPSKYETEK_DATA       lpData
    );

SKYETEK_STATUS 
GenericTag_CommitTransaction(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_AbortTransaction(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_EnableEAS(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_DisableEAS(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag
    );

SKYETEK_STATUS 
GenericTag_ScanEAS(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    unsigned char        *state
    );

SKYETEK_STATUS 
GenericTag_ReadAFI(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_DATA       *lpData
    );
    	
SKYETEK_STATUS 
GenericTag_WriteAFI(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_DATA       lpData
    );

SKYETEK_STATUS 
GenericTag_ReadDSFID(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_DATA       *lpData
    );
    	
SKYETEK_STATUS 
GenericTag_WriteDSFID(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag, 
    LPSKYETEK_DATA       lpData
    );

SKYETEK_STATUS 
GenericTag_GetKeyVersion(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_KEY        lpKey
    );

SKYETEK_STATUS 
GenericTag_ChangeKey(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_KEY        lpNewKey,
    LPSKYETEK_KEY        lpVurrentKey
    );

SKYETEK_STATUS 
GenericTag_GetApplicationKeySettings(
    LPSKYETEK_READER        lpReader,
    LPSKYETEK_TAG           lpTag,
    LPSKYETEK_APP_KEY_SETTINGS  lpSettings
    );

SKYETEK_STATUS 
GenericTag_GetMasterKeySettings(
    LPSKYETEK_READER        lpReader,
    LPSKYETEK_TAG           lpTag,
    LPSKYETEK_MASTER_KEY_SETTINGS  lpSettings
    );

SKYETEK_STATUS 
GenericTag_ChangeApplicationKeySettings(
    LPSKYETEK_READER             lpReader,
    LPSKYETEK_TAG                lpTag,
    LPSKYETEK_APP_KEY_SETTINGS   lpSettings
    );

SKYETEK_STATUS 
GenericTag_ChangeMasterKeySettings(
    LPSKYETEK_READER                 lpReader,
    LPSKYETEK_TAG                    lpTag,
    LPSKYETEK_MASTER_KEY_SETTINGS    lpSettings
    );

SKYETEK_STATUS 
GenericTag_InitializeSecureMemoryTag(
    LPSKYETEK_READER          lpReader,
    LPSKYETEK_TAG             lpTag,
    SKYETEK_HMAC_ALGORITHM    hmac,
    LPSKYETEK_KEY             lpKeyHMAC,
    SKYETEK_CIPHER_ALGORITHM  cipher,
    LPSKYETEK_KEY             lpKeyCipher,
		int                       useKeyDerivationFunction
    );

SKYETEK_STATUS 
GenericTag_SetupSecureMemoryTag(
    LPSKYETEK_READER          lpReader,
    LPSKYETEK_TAG             lpTag,
    LPSKYETEK_KEY             lpKeyHMAC,
    LPSKYETEK_KEY             lpKeyCipher,
		int                       useKeyDerivationFunction
    );

SKYETEK_STATUS 
GenericTag_InterfaceSend(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    SKYETEK_INTERFACE    inteface,
    SKYETEK_BLOCK        block,
    LPSKYETEK_DATA       lpSendData,
    LPSKYETEK_DATA       *lpRecvData
    );

SKYETEK_STATUS 
GenericTag_TransportSend(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    SKYETEK_TRANSPORT    transport,
    SKYETEK_BLOCK        block,
    LPSKYETEK_DATA       lpCla,
    LPSKYETEK_DATA       lpIns,
    LPSKYETEK_DATA       lpP1p2,
    LPSKYETEK_DATA       lpData, 
    unsigned int         lpLe,
    LPSKYETEK_DATA       *lpRecvData
    );

SKYETEK_STATUS 
GenericTag_InitiatePayment(
    LPSKYETEK_READER         lpReader,
    LPSKYETEK_TAG            lpTag,
    SKYETEK_PAYMENT_SYSTEM  *lpPaymentSystem
    );
  
SKYETEK_STATUS 
GenericTag_ComputePayment(
    LPSKYETEK_READER         lpReader,
    LPSKYETEK_TAG            lpTag,
    SKYETEK_TRANSACTION      transaction,
    SKYETEK_PAYMENT_SYSTEM  *lpPaymentSystem
    );

#ifdef __cplusplus
}
#endif

#endif
