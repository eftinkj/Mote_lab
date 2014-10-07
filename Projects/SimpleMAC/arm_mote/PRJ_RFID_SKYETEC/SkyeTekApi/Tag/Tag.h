/**
 * Tag.h
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Tag implementation.
 */
#ifndef TAG_H
#define TAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "SkyeTekAPI.h"
  
typedef struct TAGIMPL 
{
//  /* Tag functions */
//	SKYETEK_STATUS 
//    (*SelectTag)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag
//        );
//
//	SKYETEK_STATUS 
//    (*ReadTagData)(    
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ADDRESS    lpAddr,
//        unsigned char        encrypt,
//        unsigned char        hmac,
//        LPSKYETEK_DATA       *lpData
//        );
//
//	SKYETEK_STATUS 
//    (*WriteTagData)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ADDRESS    lpAddr, 
//        unsigned char        encrypt,
//        unsigned char        hmac,
//        LPSKYETEK_DATA       lpData
//        );
//
//	SKYETEK_STATUS 
//    (*ReadTagConfig)(    
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ADDRESS    lpAddr,
//        LPSKYETEK_DATA       *lpData
//        );
//
//	SKYETEK_STATUS 
//    (*WriteTagConfig)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ADDRESS    lpAddr, 
//        LPSKYETEK_DATA       lpData
//        );
//
//	SKYETEK_STATUS 
//    (*LockTagBlock)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ADDRESS    lpAddr,
//        LPSKYETEK_DATA       lpData
//        );
//
//	SKYETEK_STATUS 
//    (*ActivateTagType)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag
//        );
//
//	SKYETEK_STATUS 
//    (*DeactivateTagType)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag
//        );
//
//	SKYETEK_STATUS 
//    (*SetTagBitRate)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        unsigned char        rate
//        );
//
//	SKYETEK_STATUS 
//    (*GetTagInfo)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_MEMORY     lpMemory
//        );
//
//	SKYETEK_STATUS 
//    (*GetLockStatus)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ADDRESS    lpAddr,
//        unsigned char        *status
//        );
//
//	SKYETEK_STATUS 
//    (*KillTag)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_KEY        lpKey
//        );
//
//	SKYETEK_STATUS 
//    (*ReviveTag)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag
//        );
//
//	SKYETEK_STATUS 
//    (*EraseTag)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag
//        );
//
//	SKYETEK_STATUS 
//    (*FormatTag)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag
//        );
//	
//    SKYETEK_STATUS 
//    (*DeselectTag)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag
//        );
//
//    SKYETEK_STATUS 
//    (*AuthenticateTag)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_KEY        lpKey
//        );
//
//    SKYETEK_STATUS 
//    (*SendTagPassword)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_DATA       lpData
//        );
//
//	SKYETEK_STATUS 
//    (*GetApplicationIDs)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ID         **lpIds,
//        unsigned int         *count
//        );
//
//	SKYETEK_STATUS 
//    (*SelectApplication)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ID         lpId
//        );
//	
//    SKYETEK_STATUS 
//    (*CreateApplication)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ID         lpId,
//        LPSKYETEK_APP_KEY_SETTINGS  lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*DeleteApplication)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ID         lpId
//        );
//
//	SKYETEK_STATUS 
//    (*GetFileIDs)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_ID         **lpFiles,
//        unsigned int         *count
//        );
//
//	SKYETEK_STATUS 
//    (*SelectFile)(
//        LPSKYETEK_READER    lpReader,
//        LPSKYETEK_TAG       lpTag,
//        LPSKYETEK_ID        lpFile
//        );
//
//    SKYETEK_STATUS 
//    (*CreateDataFile)(
//        LPSKYETEK_READER             lpReader,
//        LPSKYETEK_TAG                lpTag,
//        LPSKYETEK_ID                 lpFile,
//        LPSKYETEK_DATA_FILE_SETTINGS lpSettings
//        ); 
//
//    SKYETEK_STATUS 
//    (*CreateValueFile)(
//        LPSKYETEK_READER                 lpReader,
//        LPSKYETEK_TAG                    lpTag,
//        LPSKYETEK_ID                     lpFile,
//        LPSKYETEK_VALUE_FILE_SETTINGS    lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*CreateRecordFile)(
//        LPSKYETEK_READER                 lpReader,
//        LPSKYETEK_TAG                    lpTag,
//        LPSKYETEK_ID                     lpFile,
//        LPSKYETEK_RECORD_FILE_SETTINGS   lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*GetCommonFileSettings)(
//        LPSKYETEK_READER             lpReader,
//        LPSKYETEK_TAG                lpTag,
//        LPSKYETEK_ID                 lpFile,
//        LPSKYETEK_FILE_SETTINGS      lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*GetDataFileSettings)(
//        LPSKYETEK_READER                 lpReader,
//        LPSKYETEK_TAG                    lpTag,
//        LPSKYETEK_ID                     lpFile,
//        LPSKYETEK_DATA_FILE_SETTINGS     lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*GetValueFileSettings)(
//        LPSKYETEK_READER                 lpReader,
//        LPSKYETEK_TAG                    lpTag,
//        LPSKYETEK_ID                     lpFile,
//        LPSKYETEK_VALUE_FILE_SETTINGS    lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*GetRecordFileSettings)(
//        LPSKYETEK_READER                 lpReader,
//        LPSKYETEK_TAG                    lpTag,
//        LPSKYETEK_ID                     lpFile,
//        LPSKYETEK_RECORD_FILE_SETTINGS   lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*ChangeFileSettings)(
//        LPSKYETEK_READER             lpReader,
//        LPSKYETEK_TAG                lpTag,
//        LPSKYETEK_ID                 lpFile,
//        LPSKYETEK_FILE_SETTINGS      lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*ReadFile)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile,
//        LPSKYETEK_ADDRESS    lpAddr,
//        LPSKYETEK_DATA       *lpData
//        );
//
//    SKYETEK_STATUS 
//    (*WriteFile)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile,
//        LPSKYETEK_ADDRESS    lpAddr,
//        LPSKYETEK_DATA       lpData
//        );
//
//	SKYETEK_STATUS 
//    (*DeleteFile)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile
//        );
//	
//    SKYETEK_STATUS 
//    (*ClearFile)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile
//        );
//
//    SKYETEK_STATUS 
//    (*CreditValueFile)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile,
//        unsigned int         amount
//        );
//
//
//	SKYETEK_STATUS 
//    (*DebitValueFile)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile,
//        unsigned int         amount
//        );
//
//    SKYETEK_STATUS 
//    (*LimitedCreditValueFile)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile,
//        unsigned int         amount
//        );
//
//    SKYETEK_STATUS 
//    (*GetValue)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile,
//        unsigned int         *value
//        );
//
//    SKYETEK_STATUS 
//    (*ReadRecords)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile,
//        LPSKYETEK_ADDRESS    lpAddr,
//        LPSKYETEK_DATA       *lpData
//        );
//    	    
//    SKYETEK_STATUS 
//    (*WriteRecord)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_ID         lpFile,
//        LPSKYETEK_ADDRESS    lpAddr,
//        LPSKYETEK_DATA       lpData
//        );
//
//    SKYETEK_STATUS 
//    (*CommitTransaction)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag
//        );
//
//    SKYETEK_STATUS 
//    (*AbortTransaction)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag
//        );
//
//	SKYETEK_STATUS 
//    (*EnableEAS)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag
//        );
//
//    SKYETEK_STATUS 
//    (*DisableEAS)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag
//        );
//
//    SKYETEK_STATUS 
//    (*ScanEAS)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag,
//        unsigned char        *state
//        );
//
//    SKYETEK_STATUS 
//    (*ReadAFI)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_DATA       *lpData
//        );
//    	    
//    SKYETEK_STATUS 
//    (*WriteAFI)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_DATA       lpData
//        );
//
//    SKYETEK_STATUS 
//    (*ReadDSFID)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_DATA       *lpData
//        );
//    	    
//    SKYETEK_STATUS 
//    (*WriteDSFID)(
//        LPSKYETEK_READER     lpReader, 
//        LPSKYETEK_TAG        lpTag, 
//        LPSKYETEK_DATA       lpData
//        );
//
//    SKYETEK_STATUS 
//    (*GetKeyVersion)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_KEY        lpKey
//        );
//
//    SKYETEK_STATUS 
//    (*ChangeKey)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        LPSKYETEK_KEY        lpNewKey,
//        LPSKYETEK_KEY        lpVurrentKey
//        );
//
//    SKYETEK_STATUS 
//    (*GetApplicationKeySettings)(
//        LPSKYETEK_READER            lpReader,
//        LPSKYETEK_TAG               lpTag,
//        LPSKYETEK_APP_KEY_SETTINGS  lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*GetMasterKeySettings)(
//        LPSKYETEK_READER                lpReader,
//        LPSKYETEK_TAG                   lpTag,
//        LPSKYETEK_MASTER_KEY_SETTINGS   lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*ChangeApplicationKeySettings)(
//        LPSKYETEK_READER             lpReader,
//        LPSKYETEK_TAG                lpTag,
//        LPSKYETEK_APP_KEY_SETTINGS   lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*ChangeMasterKeySettings)(
//        LPSKYETEK_READER                 lpReader,
//        LPSKYETEK_TAG                    lpTag,
//        LPSKYETEK_MASTER_KEY_SETTINGS    lpSettings
//        );
//
//    SKYETEK_STATUS 
//    (*InitializeSecureMemoryTag)(
//        LPSKYETEK_READER          lpReader,
//        LPSKYETEK_TAG             lpTag,
//        SKYETEK_HMAC_ALGORITHM    hmac,
//        LPSKYETEK_KEY             lpKeyHMAC,
//        SKYETEK_CIPHER_ALGORITHM  cipher,
//        LPSKYETEK_KEY             lpKeyCipher,
//				int                       useKeyDerivationFunction
//        );
//
//    SKYETEK_STATUS 
//    (*SetupSecureMemoryTag)(
//        LPSKYETEK_READER          lpReader,
//        LPSKYETEK_TAG             lpTag,
//        LPSKYETEK_KEY             lpKeyHMAC,
//        LPSKYETEK_KEY             lpKeyCipher,
//				int                       useKeyDerivationFunction
//        );
//
//    SKYETEK_STATUS 
//    (*InterfaceSend)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        SKYETEK_INTERFACE    inteface,
//        SKYETEK_BLOCK        block,
//        LPSKYETEK_DATA       lpSendData,
//        LPSKYETEK_DATA       *lpRecvData
//        );
//
//    SKYETEK_STATUS 
//    (*TransportSend)(
//        LPSKYETEK_READER     lpReader,
//        LPSKYETEK_TAG        lpTag,
//        SKYETEK_TRANSPORT    transport,
//        SKYETEK_BLOCK        block,
//        LPSKYETEK_DATA       lpCla,
//        LPSKYETEK_DATA       lpIns,
//        LPSKYETEK_DATA       lpP1p2,
//        LPSKYETEK_DATA       lpData, 
//        unsigned int         lpLe,
//        LPSKYETEK_DATA       *lpRecvData
//        );
//
//    SKYETEK_STATUS 
//    (*InitiatePayment)(
//        LPSKYETEK_READER         lpReader,
//        LPSKYETEK_TAG            lpTag,
//        SKYETEK_PAYMENT_SYSTEM  *lpPaymentSystem
//        );
//
//    SKYETEK_STATUS 
//    (*ComputePayment)(
//        LPSKYETEK_READER         lpReader,
//        LPSKYETEK_TAG            lpTag,
//        SKYETEK_TRANSACTION      transaction,
//        SKYETEK_PAYMENT_SYSTEM  *lpPaymentSystem
//        );
  int dummy;
} TAGIMPL, *LPTAGIMPL;


extern TAGIMPL GenericTagImpl;
extern TAGIMPL DesfireTagImpl;
extern TAGIMPL Iso14443ATagImpl;
extern TAGIMPL Iso14443BTagImpl;

typedef struct _TAGTYPEDESC 
{
  SKYETEK_TAGTYPE   type;
  unsigned char     bytesToBlock;
  TCHAR              *name;
  unsigned char     band;
} TAGTYPEDESC, *LPTAGTYPEDESC;


static TAGTYPEDESC TagTypeDescriptions [] = {
  { AUTO_DETECT, 4, _T("Auto Detect"), 0 },
	/* STPV3 Tag Types */
  { ISO_15693_AUTO_DETECT, 4, _T("ISO15693 Auto Detect"), 0 },
  { TI_15693_AUTO_DETECT, 4, _T("TI 15693 Auto Detect"), 0 },
  { TAGIT_HF1_STANDARD, 4, _T("Tag-It HF-I Standard (2k bits)"), 0 },
	{ TAGIT_HF1_PRO, 4, _T("Tag-It HF-I Pro"), 0 },
	{ TAGIT_HF1_PLUS, 4, _T("Tag-It HF-I Plus"), 0 },
	{ PHILIPS_15693_AUTO_DETECT, 4, _T("Philips 15693 Auto Detect"), 0 },
	{ ICODE_SLI_SL2, 4, _T("I-Code SLI SL2 (1k bits)"), 0 },
	{ ST_15693_AUTO_DETECT, 4, _T("ST 15693 Auto Detect"), 0 },
	{ ISO_LRI64, 4, _T("LRI 64"), 0 },
	{ LRI512, 4, _T("LRI 512"), 0 },
  { LRI2K, 4, _T("LRI 2K"), 0 },
  { LRIS2K, 4, _T("LRIS 2K"), 0 },
	{ EM_15693_AUTO_DETECT, 4, _T("EM 15693 Auto Detect"), 0 },
	{ EM4006, 4, _T("EM4006"), 0 },
	{ EM4034, 4, _T("EM4034"), 0 },
	{ EM4035_CRYPTO, 4, _T("EM4035 (Crypto)"), 0 },
	{ EM4135, 4, _T("EM4135"), 0 },
	{ INFINEON_15693_AUTO_DETECT, 4, _T("Infineon 15693 Auto Detect"), 0 },
	{ SRF55V02P, 4, _T("Infineon SRF55V02P"), 0 },
	{ SRF55V02S, 4, _T("Infineon SRF55V02S"), 0 },
	{ SRF55V10P, 4, _T("Infineon SRF55V10P"), 0 },
	{ SRF55V10S, 4, _T("Infineon SRF55V10S"), 0 },
	{ SRF55V01P, 4, _T("Infineon SRF55V01P"), 0 },
  { FUJITSU_AUTO_DETECT, 4, _T("Fujitsu Auto Detect"), 0 },
  { MB89R118, 4, _T("MB89R118 (GammaTag)"), 0 },
  { ISO_14443A_AUTO_DETECT, 4, _T("ISO14443A Auto Detect"), 0 },
  { TAGSYS_15693, 4, _T("TagSys 15693"), 0 },
  { TAGSYS_C370, 4, _T("TagSys C370"), 0 },
	{ PHILIPS_14443A_AUTO_DETECT, 4, _T("Philips 14443A Auto Detect"), 0 },
	{ ISO_MIFARE_ULTRALIGHT, 4, _T("Mifare Ultralight"), 0 },
	{ MIFARE_1K, 4, _T("Mifare 1k"), 0 },
	{ MIFARE_4K, 4, _T("Mifare 4k"), 0 },
	{ MIFARE_DESFIRE, 4, _T("Mifare DESfire"), 0 },
	{ MIFARE_PROX, 4, _T("Mifare Pro X"), 0 },
	{ INNOVISION_14443A_AUTO_DETECT, 4, _T("Innovision 14443A Auto Detect"), 0 },
	{ JEWEL, 4, _T("Jewel"), 0 },
  { INFINEON_14443A_AUTO_DETECT, 4, _T("Infineon 14443A Auto Detect"), 0 },
  { SLE66R35_MIFARE, 4, _T("Infineon SLE66R35 Mifare"), 0 },
  { SLE55R01, 4, _T("Infineon SLE55R01"), 0 },
  { SLE55R02, 4, _T("Infineon SLE55R02"), 0 },
  { SLE55R04, 4, _T("Infineon SLE55R04"), 0 },
  { SLE55R08, 4, _T("Infineon SLE55R08"), 0 },
  { SLE55R16, 4, _T("Infineon SLE55R16"), 0 },
	{ ISO_14443B_AUTO_DETECT, 4, _T("ISO14443B Auto Detect"), 0 },
	{ ATMEL_14443B_AUTO_DETECT, 4, _T("Atmel 14443B Auto Detect"), 0 },
	{ CRYPTORF_1K, 4, _T("CryptoRF (1k bits)"), 0 },
	{ CRYPTORF_2K, 4, _T("CryptoRF (2k bits)"), 0 },
	{ CRYPTORF_4K, 4, _T("CryptoRF (4k bits)"), 0 },
	{ CRYPTORF_8K, 4, _T("CryptoRF (8k bits)"), 0 },
	{ CRYPTORF_16K, 4, _T("CryptoRF (16k bits)"), 0 },
	{ CRYPTORF_32K, 4, _T("CryptoRF (32k bits)"), 0 },
	{ CRYPTORF_64K, 4, _T("CryptoRF (64k bits)"), 0 },
	{ AT88RF001, 4, _T("AT88RF001 (256 bits)"), 0 },
	{ AT88RF020, 4, _T("AT88RF020"), 0 },
	{ SAMSUNG_14443B_AUTO_DETECT, 4, _T("Samsung 14443B Auto Detect"), 0 },
	{ S3C89K8, 4, _T("S3C89K8 (8k bits)"), 0 },
	{ S3C89V5, 4, _T("S3C89V5 (16k bits)"), 0 },
	{ S3C89V8, 4, _T("S3C89V8 (8192)"), 0 },
	{ S3CC9G4, 4, _T("S3CC9G4 (4096)"), 0 },
	{ S3CC9GC, 4, _T("S3CC9GC (72kB)"), 0 },
	{ S3CC9GW, 4, _T("S3CC9GW (144 kB)"), 0 },
	{ S3CC9W4, 4, _T("S3CC9W4 (4 kB)"), 0 },
	{ S3CC9W9, 4, _T("S3CC9W9 (32 kB)"), 0 },
  { ST_MICRO_14443B_AUTO_DETECT, 4, _T("ST Micro 14443B Auto Detect"), 0},
  { ST_MICRO_SRIX4K,4, _T("ST Micro SRIX4K"), 0},
  { ST_MICRO_SRI176,4, _T("ST Micro SRI176"), 0},
  { ST_MICRO_SRI512,4, _T("ST Micro SRI512"), 0},
  { AMEX_CARD, 4, _T("AMEX Card"), 0},
  { AMEX_FOB, 4, _T("AMEX FOB"), 0},
	{ ISO_18000_3_MODE1_AUTO_DETECT, 4, _T("ISO18000-3 Mode 1 Auto Detect"), 0 },
	{ M1_TI_15693_AUTO_DETECT, 4, _T("TI 15693 Auto Detect (M1)"), 0 },
	{ M1_TAGIT_HF1_STANDARD, 4, _T("Tag-It HF-I Standard (2k bits) (M1)"), 0 },
	{ M1_TAGIT_HF1_PRO, 4, _T("Tag-It HF-I Pro (M1)"), 0 },
	{ M1_TAGIT_HF1_PLUS, 4, _T("Tag-It HF-I Plus (M1)"), 0 },
	{ M1_PHILIPS_15693_AUTO_DETECT, 4, _T("Philips 15693 Auto Detect (M1)"), 0 },
	{ M1_ICODE_SLI_SL2, 4, _T("I-Code SLI SL2 (1k bits) (M1)"), 0 },
	{ M1_ST_15693_AUTO_DETECT, 4, _T("ST 15693 Auto Detect (M1)"), 0 },
	{ M1_LRI64, 4, _T("LRI 64 (M1)"), 0 },
	{ M1_LRI512, 4, _T("LRI 512 (M1)"), 0 },
	{ M1_EM_15693_AUTO_DETECT, 4, _T("EM 15693 Auto Detect (M1)"), 0 },
	{ M1_EM4006, 4, _T("EM4006 (M1)"), 0 },
	{ M1_EM4034, 4, _T("EM4034 (M1)"), 0 },
	{ M1_EM4035_CRYPTO, 4, _T("EM4035 (Crypto) (M1)"), 0 },
	{ M1_EM4135, 4, _T("EM4135 (M1)"), 0 },
	{ M1_INFINEON_15693_AUTO_DETECT, 4, _T("Infineon 15693 Auto Detect (M1)"), 0 },
	{ M1_MYD2K, 4, _T("My-D 2K (M1)"), 0 },
	{ M1_MYD2KS, 4, _T("My-D 2KS (M1)"), 0 },
	{ M1_MYD10K, 4, _T("My-D 10K (M1)"), 0 },
	{ M1_MYD10KS, 4, _T("My-D 10KS (M1)"), 0 },
	{ ISO_18000_3_MODE1_EXTENDED_AUTO_DETECT, 4, _T("ISO18000-3 Mode 1 Extended Auto Detect"), 0 },
	{ RFU, 4, _T("RFU"), 0 },
	{ TAGSYS, 4, _T("TagSys"), 0 },
	{ ISO_18000_3_MODE2_AUTO_DETECT, 4, _T("ISO18000-3 Mode 2 Auto Detect"), 0 },
	{ INFINEON_AUTO_DETECT, 4, _T("Infineon Auto Detect"), 0 },
	{ INFINEON_PJM_TAG, 4, _T("Infineon PJM Tag"), 0 },
	{ ISO_18092_AUTO_DETECT, 4, _T("ISO18092 Auto Detect"), 0 },
	{ ISO_21481_AUTO_DETECT, 4, _T("ISO21481 Auto Detect"), 0 },
	{ SKYETEK_AFE, 4, _T("SkyeTek Crypto API"), 0 },
	{ HF_PROPRIETARY_RFU, 4, _T("HF Proprietary RFU"), 0 },
	{ TAGIT_HF, 4, _T("Tag-It HF"), 0 },
	{ ICODE1, 4, _T("I-Code1"), 0 },
	{ HF_EPC, 4, _T("HF EPC"), 0 },
	{ LTO_PHILIPS, 4, _T("LTO - Philips"), 0 },
	{ LTO_ATMEL, 4, _T("LTO - Atmel"), 0 },
	{ FELICIA, 4, _T("FeliCA"), 0 },
	{ PICOTAG_2K, 4, _T("PicoTag 2k"), 0 },
	{ PICOTAG_16K, 4, _T("PicoTag 16k"), 0 },
	{ PICOTAG_2KS, 4, _T("PicoTag 2kS"), 0 },
	{ PICOTAG_16KS, 4, _T("PicoTag 16kS"), 0 },
	{ HID_ICLASS, 4, _T("HID I-Class"), 0 },
	{ GEMWARE_C210, 4, _T("GemWave C210"), 0 },
	{ GEMWARE_C220, 4, _T("GemWave C220"), 0 },
	{ GEMWARE_C240, 4, _T("GemWave C240"), 0 },
	{ SR176, 4, _T("SR176"), 0 },
  { ICODE_UID_ICS11, 4, _T("I-Code UID ICS 11"), 0 },
  { ICODE_UID_ICS12, 4, _T("I-Code UID ICS 12"), 0 },
	{ EPC_CLASS0_AUTO_DETECT, 4, _T("EPC Class 0 Auto Detect"), 0 },
	{ SYMBOL_CLASS0_AUTO_DETECT, 4, _T("Symbol Class 0 Auto Detect"), 0 },
	{ MATRICS_CLASS0, 4, _T("Matrics Class 0  (96 bits)"), 0 },
	{ MATRICS_CLASS0_PLUS, 4, _T("Matrics Class 0+  (256 bits)"), 0 },
	{ IMPINJ_CLASS0_AUTO_DETECT, 4, _T("Impinj Class 0 Auto Detect"), 0 },
	{ ZUMA, 4, _T("Zuma (256 bits) (Class 0+)"), 0 },
	{ EPC_CLASS1_GEN1_AUTO_DETECT, 4, _T("EPC Class 1 Gen 1 Auto Detect"), 0 },
	{ ALIEN_C1G1_AUTO_DETECT, 4, _T("Alien C1G1 Auto Detect"), 0 },
	{ QUARK, 4, _T("Quark (64 bits)"), 0 },
	{ OMEGA, 4, _T("Omega (64 bits)"), 0 },
	{ LEPTON, 4, _T("Lepton (96 bits)"), 0 },
	{ ST_MICRO_C1G1_AUTO_DETECT, 4, _T("ST Micro C1G1 Auto Detect"), 0 },
	{ XRA00, 4, _T("XRA00 (64 bits)"), 0 },
	{ ISO_18000_6C_AUTO_DETECT, 4, _T("ISO 18000-6C Auto Detect"), 0 },
	{ IMPINJ_C1G2_AUTO_DETECT, 4, _T("Impinj C1G2 Auto Detect"), 0 },
	{ MONZA, 4, _T("Monza (256 bits)"), 0 },
	{ PHILIPS_C1G2_AUTO_DETECT, 4, _T("Philips C1G2 Auto Detect"), 0 },
	{ UCODE_EPC_G2, 4, _T("UCode EPC G2 (512 bits)"), 0 },
  { MOTOROLA_EPC_G2, 4, _T("Motorola EPC Gen2"), 0 },
	{ ST_C1G2_AUTO_DETECT, 4, _T("ST C1G2 Auto Detect"), 0 },
	{ XRAG2, 4, _T("XRAG2 (432 bits)"), 0 },
  { ALIEN_HIGGS, 4, _T("Alien Higgs"), 0 },
  { EM_C1G2_AUTO, 4, _T("EM C1G2 Auto Detect"), 0 },
  { EM_C1G2_EM4024, 4, _T("EM4024 C1G2"), 0 },
  { EM_C1G2_EM4124, 4, _T("EM4124 C1G2"), 0 },
  { EM_C1G2_EM4224, 4, _T("EM4224 C1G2"), 0 },
  { EM_C1G2_EM4324, 4, _T("EM4324 C1G2"), 0 },
	{ ISO_18000_6B_AUTO_DETECT, 4, _T("ISO 18000-6B Auto Detect"), 0 },
	{ PHILIPS_18000_6B_AUTO_DETECT, 4, _T("Philips 18000-6B Auto Detect"), 0 },
	{ UCODE_1_19, 4, _T("UCode EPC 1.19 (256 bits)"), 0 },
	{ UCODE_HSL, 4, _T("UCode HSL (1728 bits)"), 0 },
  // FUJITSU_ISO180006B_AUTO_DETECT
  { FUJITSU_ISO180006B_AUTO_DETECT, 4, _T("Fujitsu ISO180006B Auto Detect"), 0 },
  { FUJITSU_MB97R8010, 4, _T("Fujitsu MB97R8010"), 0 },
  { FUJITSU_MB97R8020, 4, _T("Fujitsu MB97R8020"), 0 },
	{ ISO_18000_6A_AUTO_DETECT, 4, _T("ISO18000-6A Auto Detect"), 0 },
	{ EM_6A, 4, _T("EM 6A Tag"), 0 },
	{ EM_IPX_AUTO, 4, _T("EM IPX Auto Detect"), 0 },
	{ EM4X22_AUTO, 4, _T("EM4X22 Auto Detect"), 0 },
	{ EM4022, 4, _T("EM4022"), 0 },
 	{ EM4122, 4, _T("EM4122"), 0 },
	{ EM4222, 4, _T("EM4222"), 0 },
	{ EM4X44_AUTO, 4, _T("EM4X44 Auto Detect"), 0 },
	{ EM4044, 4, _T("EM4044"), 0 },
	{ EM4144, 4, _T("EM4144"), 0 },
	{ EM4244, 4, _T("EM4244"), 0 },
	{ EM4344, 4, _T("EM4344"), 0 },
	{ EM4444, 4, _T("EM4444"), 0 },
	{ NXP_G2XL, 4, _T("NXP G2XL"), 0},
	{ NXP_G2XM, 4, _T("NXP G2XM"), 0},
	{ HITACHI_HIBIKI_II, 4, _T("Hitachi Hibiki II"), 0}
};

#define NUM_TAGTYPEDESCS sizeof(TagTypeDescriptions)/sizeof(TAGTYPEDESC)
  
/**
 * Returns count of TAGTYPEDESCS
 */
unsigned int GetTagTypeDescriptionCount(void);

/**
 *  Returns the given TAGTYPEDESC
 *  @param index 0 based index of descriptor
 */
LPTAGTYPEDESC GetTagTypeDescription(unsigned int index);

/**
 * Utility function that returns the number of bytes per block for a given tag type
 * @param type Type of tag
 * @return Number of bytes per block
 */
unsigned char TagBlockToByte(SKYETEK_TAGTYPE type);

#ifdef __cplusplus
}
#endif

#endif
