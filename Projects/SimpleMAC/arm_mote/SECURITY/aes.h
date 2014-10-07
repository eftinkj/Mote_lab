/*
******************************************************************************
* Copyright 2002
*
*     TRIALOG SA
*     25 rue du General Foy
*     F-75008 Paris
*     FRANCE
*     Ph : +33 (0) 1 44 70 61 00
*     www.trialog.com
*
* Module        : aes.h
*
*-----------------------------------------------------------------------------
* Description   : 
* 
*   header file containing all the custom definitions
*   and global variables
******************************************************************************
*/

/* pointer type definition */
#ifdef _DATA_PTR                  
#define AES_MEMORY data
#else
# ifdef _IDATA_PTR
# define AES_MEMORY idata
# else
# define AES_MEMORY xdata
# endif
#endif

/* location of block */
#ifdef _IDATA_BLOCK
#define BLOCK_LOCATION idata
#else
#define BLOCK_LOCATION xdata
#endif

#define     KEY_LENGTH      128  /*  Rijndael limited to 128 bits keys */
#define     BLOCK_LENGTH    128  /*  Rijndael limited to 128 bits blocks */
#define     BLOCK_SIZE      16   /*  Default block size in bytes */
#define     MESSAGE_PART    80   /*Eyad: Message to be encrypted*/
#define     BC              4
#define     KC              4
#define     ROUNDS          10

// ---------> cut from alg_ref.h
typedef unsigned char	 word8;	
typedef unsigned short	 word16;	
typedef unsigned long	 word32;


// ---------> cut from api_ref.h

typedef unsigned char    BYTE;

/*  The structure for key information */
typedef struct {
      BYTE  direction;	/*  Key used for encrypting or decrypting? */
      int   keyLen;	/*  Length of the key  */

      /*  The following parameters are algorithm dependent, replace or
      		add as necessary  */
      int   blockLen;   /* block length */
      word8 keySched[ROUNDS+1][4][BC];	/* key schedule		*/
      } keyInstance;

/*  The structure for cipher information */
typedef struct {
      BYTE  mode;            /* MODE_ECB, MODE_CBC, or MODE_CFB1 */
      BYTE  IV[16]; /* A possible Initialization Vector for 
      					ciphering */
      /*  Add any algorithm specific parameters needed here  */
      int   blockLen;    	/* Sample: Handles non-128 bit block sizes
      					(if available) */
      } cipherInstance;

// end cut



/* global variables  */
RANGE char AES_MEMORY keyMaterial[16];         // the 16 bytes key
RANGE keyInstance AES_MEMORY keyInst;          // the key object
RANGE cipherInstance AES_MEMORY cipherInst;    // the cipher object

RANGE word8 AES_MEMORY k[4][KC];              
RANGE word8 BLOCK_LOCATION block[4][BC];

// these 2 arrays receives (respectively) the data to encrypt and decrypt

// the users writes his data in the array and the result is stored in the
// same array
RANGE BYTE AES_MEMORY blockE[BLOCK_SIZE];
RANGE BYTE AES_MEMORY blockD[BLOCK_SIZE];

RANGE  BYTE AES_MEMORY blockToEncrypt[BLOCK_SIZE];
RANGE  BYTE AES_MEMORY blockToDecrypt[BLOCK_SIZE];
RANGE  BYTE AES_MEMORY buff1[BLOCK_SIZE];
RANGE  BYTE AES_MEMORY buff2[BLOCK_SIZE];
RANGE  BYTE AES_MEMORY Encrypted_Message[MESSAGE_PART];
RANGE  BYTE AES_MEMORY Decrypted_Message[MESSAGE_PART];
  void encrypt();
  void decrypt();


  
