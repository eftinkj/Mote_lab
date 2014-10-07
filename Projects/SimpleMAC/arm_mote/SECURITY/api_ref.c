/* rijndael-api-ref.c   v2.0   August '99
 * Reference ANSI C code
 * authors: Paulo Barreto
 *          Vincent Rijmen
 */

/* MODIFIED BY :
******************************************************************************
* Copyright 2002
*
*     TRIALOG SA
*     25 rue du General Foy
*     F-75008 Paris
*     FRANCE
*     Ph : +33 (0) 1 44 70 61 00
*     www.trialog.com
*-----------------------------------------------------------------------------
* Description   : 
* 
*   api file for the rijndael algorithm
******************************************************************************
*/

/*
                        ------------------------------
                        Rijndael ANSI C Reference Code
                        ------------------------------

                                October 24, 2000

                                  Disclaimer


This software package was submitted to the National Institute of Standards and
Technology (NIST) during the Advanced Encryption Standard (AES) development
effort by Joan Daemen and Vincent Rijmen, the developers of the Rijndael algorithm.

This software is distributed in compliance with export regulations (see below), and
it is intended for non-commercial use, only.   NIST does not support this software 
and does not provide any guarantees or warranties as to its performance, fitness 
for any particular application, or validation under the Cryptographic Module
Validation Program (CMVP) <http://csrc.nist.gov/cryptval/>.  NIST does not accept 
any liability associated with its use or misuse.  This software is provided as-is. 
By accepting this software the user agrees to the terms stated herein.

                            -----------------------

                              Export Restrictions


Implementations of cryptography are subject to United States Federal
Government export controls.  Export controls on commercial encryption products 
are administered by the Bureau of Export Administration (BXA) 
<http://www.bxa.doc.gov/Encryption/> in the U.S. Department of Commerce. 
Regulations governing exports of encryption are found in the Export 
Administration Regulations (EAR), 15 C.F.R. Parts 730-774.
*/

#include <stdlib.h>
#include <string.h>

#include "alg_ref.h"

#define RANGE extern
#include "aes.h"
#undef RANGE

#include "api_ref.h"


int makeKey(BYTE direction)
{
  char i;
	
  if ((direction == DIR_ENCRYPT) || (direction == DIR_DECRYPT)) {
    keyInst.direction = direction;
  } else {
    return BAD_KEY_DIR;
  }

  keyInst.keyLen = KEY_LENGTH;


  /*** ->TRIALOG ***/
  /* keyMaterial given in bytes */

  /* initialize key schedule: */ 
  for(i = 0; i < KEY_LENGTH/8; i++) {
    k[i % 4][i / 4] = (word8) keyMaterial[i];
  }	
  /*** TRIALOG ***/

  rijndaelKeySched ();
	
  return TRUE;
}

int cipherInit(BYTE mode, char *IV)
{
  char i, j, t;
	
  if ((mode == MODE_ECB) || (mode == MODE_CBC) || (mode == MODE_CFB1)) {
    cipherInst.mode = mode;
  } else {
    return BAD_CIPHER_MODE;
  }
	
  if (IV != NULL) {
    for(i = 0; i < BLOCK_LENGTH/8; i++) {		
      t = IV[2*i];
      if ((t >= '0') && (t <= '9')) j = (t - '0') << 4;
      else if ((t >= 'a') && (t <= 'f')) j = (t - 'a' + 10) << 4; 
      else if ((t >= 'A') && (t <= 'F')) j = (t - 'A' + 10) << 4; 
      else return BAD_CIPHER_INSTANCE;
		
      t = IV[2*i+1];
      if ((t >= '0') && (t <= '9')) j ^= (t - '0');
      else if ((t >= 'a') && (t <= 'f')) j ^= (t - 'a' + 10); 
      else if ((t >= 'A') && (t <= 'F')) j ^= (t - 'A' + 10); 
      else return BAD_CIPHER_INSTANCE;
			
      cipherInst.IV[i] = (BYTE) j;
    } 
  }

  return TRUE;
}


int blockEncrypt()
{
  char j, t;
	
  switch (cipherInst.mode) {
  case MODE_ECB: 
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
      for(t = 0; t < 4; t++)
				/* parse input stream into rectangular array */
        block[t][j] = blockE[4*j+t] & 0xFF;
    }
    rijndaelEncrypt ();
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
				/* parse rectangular array into output ciphertext bytes */
      for(t = 0; t < 4; t++)
        blockE[4*j+t] = (BYTE) block[t][j];
    }
    break;
		
  case MODE_CBC:
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
      for(t = 0; t < 4; t++)
        /* parse initial value into rectangular array */
        block[t][j] = cipherInst.IV[t+4*j] & 0xFF;
    }
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
      for(t = 0; t < 4; t++)
				/* parse input stream into rectangular array and exor with 
				   IV or the previous ciphertext */
        block[t][j] ^= blockE[4*j+t] & 0xFF;
    }
    rijndaelEncrypt ();
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
				/* parse rectangular array into output ciphertext bytes */
      for(t = 0; t < 4; t++)
        blockE[4*j+t] = (BYTE) block[t][j];
    }
    break;
	
  default: return BAD_CIPHER_STATE;
  }
	
  return BLOCK_LENGTH;
}

int blockDecrypt()
{
  char j, t;
	
  switch (cipherInst.mode) {
  case MODE_ECB: 
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
      for(t = 0; t < 4; t++)
				/* parse input stream into rectangular array */
        block[t][j] = blockD[4*j+t] & 0xFF;
    }
    rijndaelDecrypt ();
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
				/* parse rectangular array into output ciphertext bytes */
      for(t = 0; t < 4; t++)
        blockD[4*j+t] = (BYTE) block[t][j];
    }
    break;
		
  case MODE_CBC:
    /* first block */
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
      for(t = 0; t < 4; t++)
        /* parse input stream into rectangular array */
        block[t][j] = blockD[4*j+t] & 0xFF;
    }
    rijndaelDecrypt ();
          
    for (j = 0; j < BLOCK_LENGTH/32; j++) {
      /* exor the IV and parse rectangular array into output ciphertext bytes */
      for(t = 0; t < 4; t++)
        blockD[4*j+t] = (BYTE) (block[t][j] ^ cipherInst.IV[t+4*j]);
    }
    break;
	
  default: return BAD_CIPHER_STATE;
  }
	
  return BLOCK_LENGTH;
}
