/* rijndael-alg-ref.c   v2.0   August '99
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
*   program file for the rijndael algorithm
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


#include <stdio.h>
#include <stdlib.h>

#define RANGE extern
#include "aes.h"
#undef RANGE

#include "api_ref.h"  /* that contains alg_ref.h */

#define SC	((BC - 4) >> 1)

#include "boxes_ref.dat"

static AES_MEMORY word8 shifts[3][4][2] =
	{
		0, 0,
		1, 3,
		2, 2,
		3, 1,

		0, 0,
		1, 5,
		2, 4,
		3, 3,

		0, 0,
		1, 7,
		3, 5,
		4, 4
	};


word8 mul ( word8 a, word8 b )
{
	/* multiply two elements of GF(2^m)
	 * needed for MixColumn and InvMixColumn
	 */
	/*TRIALOG decomposition */
	word16 intermed;
	word16 intera, interb;
	if ( a && b )
	{
		intera = Logtable[a];
		interb = Logtable[b];
		intermed = ( intera + interb );//%255;  //<-- 8bits addition !!!!!!
		if ( intermed == 255*2 ) intermed = 0;
		else if ( intermed >= 255 ) intermed -= 255;
		return Alogtable[intermed];
	}
	else return 0;
}

void KeyAddition ( word8 rki )
{
	/* Exor corresponding text input and round key input bytes
	 */
	block[0][0] ^= keyInst.keySched[rki][0][0];
	block[0][1] ^= keyInst.keySched[rki][0][1];
	block[0][2] ^= keyInst.keySched[rki][0][2];
	block[0][3] ^= keyInst.keySched[rki][0][3];

	block[1][0] ^= keyInst.keySched[rki][1][0];
	block[1][1] ^= keyInst.keySched[rki][1][1];
	block[1][2] ^= keyInst.keySched[rki][1][2];
	block[1][3] ^= keyInst.keySched[rki][1][3];

	block[2][0] ^= keyInst.keySched[rki][2][0];
	block[2][1] ^= keyInst.keySched[rki][2][1];
	block[2][2] ^= keyInst.keySched[rki][2][2];
	block[2][3] ^= keyInst.keySched[rki][2][3];

	block[3][0] ^= keyInst.keySched[rki][3][0];
	block[3][1] ^= keyInst.keySched[rki][3][1];
	block[3][2] ^= keyInst.keySched[rki][3][2];
	block[3][3] ^= keyInst.keySched[rki][3][3];

}

void ShiftRow0()
{
	/* Row 0 remains unchanged
	 * The other three rows are shifted a variable amount
	 */
	word8 tmp;
	tmp = block[1][0];
	block[1][0] = block[1][1];
	block[1][1] = block[1][2];
	block[1][2] = block[1][3];
	block[1][3] = tmp;

	tmp = block[2][0];
	block[2][0] = block[2][2];
	block[2][2] = tmp;
	tmp = block[2][1];
	block[2][1] = block[2][3];
	block[2][3] = tmp;

	tmp = block[3][3];
	block[3][3] = block[3][2];
	block[3][2] = block[3][1];
	block[3][1] = block[3][0];
	block[3][0] = tmp;

}

void ShiftRow1()
{
	/* Row 0 remains unchanged
	 * The other three rows are shifted a variable amount
	 */
	word8 tmp;

	tmp = block[1][3];
	block[1][3] = block[1][2];
	block[1][2] = block[1][1];
	block[1][1] = block[1][0];
	block[1][0] = tmp;

	tmp = block[2][0];
	block[2][0] = block[2][2];
	block[2][2] = tmp;
	tmp = block[2][1];
	block[2][1] = block[2][3];
	block[2][3] = tmp;

	tmp = block[3][0];
	block[3][0] = block[3][1];
	block[3][1] = block[3][2];
	block[3][2] = block[3][3];
	block[3][3] = tmp;

}

void SubstitutionS()
{
	/* Replace every byte of the input by the byte at that place
	 * in the nonlinear S-box
	 */
	block[0][0] = S[block[0][0]] ;
	block[0][1] = S[block[0][1]] ;
	block[0][2] = S[block[0][2]] ;
	block[0][3] = S[block[0][3]] ;

	block[1][0] = S[block[1][0]] ;
	block[1][1] = S[block[1][1]] ;
	block[1][2] = S[block[1][2]] ;
	block[1][3] = S[block[1][3]] ;


	block[2][0] = S[block[2][0]] ;
	block[2][1] = S[block[2][1]] ;
	block[2][2] = S[block[2][2]] ;
	block[2][3] = S[block[2][3]] ;

	block[3][0] = S[block[3][0]] ;
	block[3][1] = S[block[3][1]] ;
	block[3][2] = S[block[3][2]] ;
	block[3][3] = S[block[3][3]] ;

}

void SubstitutionSi()
{
	/* Replace every byte of the input by the byte at that place
	 * in the nonlinear S-box
	 */
	block[0][0] = Si[block[0][0]] ;
	block[0][1] = Si[block[0][1]] ;
	block[0][2] = Si[block[0][2]] ;
	block[0][3] = Si[block[0][3]] ;

	block[1][0] = Si[block[1][0]] ;
	block[1][1] = Si[block[1][1]] ;
	block[1][2] = Si[block[1][2]] ;
	block[1][3] = Si[block[1][3]] ;


	block[2][0] = Si[block[2][0]] ;
	block[2][1] = Si[block[2][1]] ;
	block[2][2] = Si[block[2][2]] ;
	block[2][3] = Si[block[2][3]] ;

	block[3][0] = Si[block[3][0]] ;
	block[3][1] = Si[block[3][1]] ;
	block[3][2] = Si[block[3][2]] ;
	block[3][3] = Si[block[3][3]] ;

}

void MixColumn()
{
	/* Mix the four bytes of every column in a linear way
	 */
	word8 AES_MEMORY b[4][MAXBC];
	/* TRIALOG decomposition of operation */
	b[0][0] = mul ( 2, block[0][0] ) ^ mul ( 3, block[ ( 0 + 1 ) % 4][0] ) ^ block[ ( 0 + 2 ) % 4][0] ^ block[ ( 0 + 3 ) % 4][0];
	b[0][1] = mul ( 2, block[0][1] ) ^ mul ( 3, block[ ( 0 + 1 ) % 4][1] ) ^ block[ ( 0 + 2 ) % 4][1] ^ block[ ( 0 + 3 ) % 4][1];
	b[0][2] = mul ( 2, block[0][2] ) ^ mul ( 3, block[ ( 0 + 1 ) % 4][2] ) ^ block[ ( 0 + 2 ) % 4][2] ^ block[ ( 0 + 3 ) % 4][2];
	b[0][3] = mul ( 2, block[0][3] ) ^ mul ( 3, block[ ( 0 + 1 ) % 4][3] ) ^ block[ ( 0 + 2 ) % 4][3] ^ block[ ( 0 + 3 ) % 4][3];

	b[1][0] = mul ( 2, block[1][0] ) ^ mul ( 3, block[ ( 1 + 1 ) % 4][0] ) ^ block[ ( 1 + 2 ) % 4][0] ^ block[ ( 1 + 3 ) % 4][0];
	b[1][1] = mul ( 2, block[1][1] ) ^ mul ( 3, block[ ( 1 + 1 ) % 4][1] ) ^ block[ ( 1 + 2 ) % 4][1] ^ block[ ( 1 + 3 ) % 4][1];
	b[1][2] = mul ( 2, block[1][2] ) ^ mul ( 3, block[ ( 1 + 1 ) % 4][2] ) ^ block[ ( 1 + 2 ) % 4][2] ^ block[ ( 1 + 3 ) % 4][2];
	b[1][3] = mul ( 2, block[1][3] ) ^ mul ( 3, block[ ( 1 + 1 ) % 4][3] ) ^ block[ ( 1 + 2 ) % 4][3] ^ block[ ( 1 + 3 ) % 4][3];

	b[2][0] = mul ( 2, block[2][0] ) ^ mul ( 3, block[ ( 2 + 1 ) % 4][0] ) ^ block[ ( 2 + 2 ) % 4][0] ^ block[ ( 2 + 3 ) % 4][0];
	b[2][1] = mul ( 2, block[2][1] ) ^ mul ( 3, block[ ( 2 + 1 ) % 4][1] ) ^ block[ ( 2 + 2 ) % 4][1] ^ block[ ( 2 + 3 ) % 4][1];
	b[2][2] = mul ( 2, block[2][2] ) ^ mul ( 3, block[ ( 2 + 1 ) % 4][2] ) ^ block[ ( 2 + 2 ) % 4][2] ^ block[ ( 2 + 3 ) % 4][2];
	b[2][3] = mul ( 2, block[2][3] ) ^ mul ( 3, block[ ( 2 + 1 ) % 4][3] ) ^ block[ ( 2 + 2 ) % 4][3] ^ block[ ( 2 + 3 ) % 4][3];

	b[3][0] = mul ( 2, block[3][0] ) ^ mul ( 3, block[ ( 3 + 1 ) % 4][0] ) ^ block[ ( 3 + 2 ) % 4][0] ^ block[ ( 3 + 3 ) % 4][0];
	b[3][1] = mul ( 2, block[3][1] ) ^ mul ( 3, block[ ( 3 + 1 ) % 4][1] ) ^ block[ ( 3 + 2 ) % 4][1] ^ block[ ( 3 + 3 ) % 4][1];
	b[3][2] = mul ( 2, block[3][2] ) ^ mul ( 3, block[ ( 3 + 1 ) % 4][2] ) ^ block[ ( 3 + 2 ) % 4][2] ^ block[ ( 3 + 3 ) % 4][2];
	b[3][3] = mul ( 2, block[3][3] ) ^ mul ( 3, block[ ( 3 + 1 ) % 4][3] ) ^ block[ ( 3 + 2 ) % 4][3] ^ block[ ( 3 + 3 ) % 4][3];

	block[0][0] = b[0][0];
	block[0][1] = b[0][1];
	block[0][2] = b[0][2];
	block[0][3] = b[0][3];

	block[1][0] = b[1][0];
	block[1][1] = b[1][1];
	block[1][2] = b[1][2];
	block[1][3] = b[1][3];

	block[2][0] = b[2][0];
	block[2][1] = b[2][1];
	block[2][2] = b[2][2];
	block[2][3] = b[2][3];

	block[3][0] = b[3][0];
	block[3][1] = b[3][1];
	block[3][2] = b[3][2];
	block[3][3] = b[3][3];
}


void InvMixColumn()
{
	/* Mix the four bytes of every column in a linear way
	 * This is the opposite operation of Mixcolumn
	 */
	word8 AES_MEMORY b[4][MAXBC];
	b[0][0] = mul ( 0xe, block[0][0] ) ^ mul ( 0xb, block[ ( 0+1 ) %4][0] ) ^ mul ( 0xd, block[ ( 0+2 ) %4][0] ) ^ mul ( 0x9, block[ ( 0+3 ) %4][0] );
	b[1][0] = mul ( 0xe, block[1][0] ) ^ mul ( 0xb, block[ ( 1+1 ) %4][0] ) ^ mul ( 0xd, block[ ( 1+2 ) %4][0] ) ^ mul ( 0x9, block[ ( 1+3 ) %4][0] );
	b[2][0] = mul ( 0xe, block[2][0] ) ^ mul ( 0xb, block[ ( 2+1 ) %4][0] ) ^ mul ( 0xd, block[ ( 2+2 ) %4][0] ) ^ mul ( 0x9, block[ ( 2+3 ) %4][0] );
	b[3][0] = mul ( 0xe, block[3][0] ) ^ mul ( 0xb, block[ ( 3+1 ) %4][0] ) ^ mul ( 0xd, block[ ( 3+2 ) %4][0] ) ^ mul ( 0x9, block[ ( 3+3 ) %4][0] );

	b[0][1] = mul ( 0xe, block[0][1] ) ^ mul ( 0xb, block[ ( 0+1 ) %4][1] ) ^ mul ( 0xd, block[ ( 0+2 ) %4][1] ) ^ mul ( 0x9, block[ ( 0+3 ) %4][1] );
	b[1][1] = mul ( 0xe, block[1][1] ) ^ mul ( 0xb, block[ ( 1+1 ) %4][1] ) ^ mul ( 0xd, block[ ( 1+2 ) %4][1] ) ^ mul ( 0x9, block[ ( 1+3 ) %4][1] );
	b[2][1] = mul ( 0xe, block[2][1] ) ^ mul ( 0xb, block[ ( 2+1 ) %4][1] ) ^ mul ( 0xd, block[ ( 2+2 ) %4][1] ) ^ mul ( 0x9, block[ ( 2+3 ) %4][1] );
	b[3][1] = mul ( 0xe, block[3][1] ) ^ mul ( 0xb, block[ ( 3+1 ) %4][1] ) ^ mul ( 0xd, block[ ( 3+2 ) %4][1] ) ^ mul ( 0x9, block[ ( 3+3 ) %4][1] );

	b[0][2] = mul ( 0xe, block[0][2] ) ^ mul ( 0xb, block[ ( 0+1 ) %4][2] ) ^ mul ( 0xd, block[ ( 0+2 ) %4][2] ) ^ mul ( 0x9, block[ ( 0+3 ) %4][2] );
	b[1][2] = mul ( 0xe, block[1][2] ) ^ mul ( 0xb, block[ ( 1+1 ) %4][2] ) ^ mul ( 0xd, block[ ( 1+2 ) %4][2] ) ^ mul ( 0x9, block[ ( 1+3 ) %4][2] );
	b[2][2] = mul ( 0xe, block[2][2] ) ^ mul ( 0xb, block[ ( 2+1 ) %4][2] ) ^ mul ( 0xd, block[ ( 2+2 ) %4][2] ) ^ mul ( 0x9, block[ ( 2+3 ) %4][2] );
	b[3][2] = mul ( 0xe, block[3][2] ) ^ mul ( 0xb, block[ ( 3+1 ) %4][2] ) ^ mul ( 0xd, block[ ( 3+2 ) %4][2] ) ^ mul ( 0x9, block[ ( 3+3 ) %4][2] );

	b[0][3] = mul ( 0xe, block[0][3] ) ^ mul ( 0xb, block[ ( 0+1 ) %4][3] ) ^ mul ( 0xd, block[ ( 0+2 ) %4][3] ) ^ mul ( 0x9, block[ ( 0+3 ) %4][3] );
	b[1][3] = mul ( 0xe, block[1][3] ) ^ mul ( 0xb, block[ ( 1+1 ) %4][3] ) ^ mul ( 0xd, block[ ( 1+2 ) %4][3] ) ^ mul ( 0x9, block[ ( 1+3 ) %4][3] );
	b[2][3] = mul ( 0xe, block[2][3] ) ^ mul ( 0xb, block[ ( 2+1 ) %4][3] ) ^ mul ( 0xd, block[ ( 2+2 ) %4][3] ) ^ mul ( 0x9, block[ ( 2+3 ) %4][3] );
	b[3][3] = mul ( 0xe, block[3][3] ) ^ mul ( 0xb, block[ ( 3+1 ) %4][3] ) ^ mul ( 0xd, block[ ( 3+2 ) %4][3] ) ^ mul ( 0x9, block[ ( 3+3 ) %4][3] );

	block[0][0] = b[0][0];
	block[0][1] = b[0][1];
	block[0][2] = b[0][2];
	block[0][3] = b[0][3];

	block[1][0] = b[1][0];
	block[1][1] = b[1][1];
	block[1][2] = b[1][2];
	block[1][3] = b[1][3];

	block[2][0] = b[2][0];
	block[2][1] = b[2][1];
	block[2][2] = b[2][2];
	block[2][3] = b[2][3];

	block[3][0] = b[3][0];
	block[3][1] = b[3][1];
	block[3][2] = b[3][2];
	block[3][3] = b[3][3];
}


char rijndaelKeySched ()
{
	/* Calculate the necessary round keys
	 * The number of calculations depends on keyBits and blockBits
	 */
	char i, j, t, rconpointer = 0;
	word8 AES_MEMORY tk[4][MAXKC];


	for ( j = 0; j < KC; j++ )
		for ( i = 0; i < 4; i++ )
			tk[i][j] = k[i][j];
	t = 0;
	/* copy values into round key array */
	for ( j = 0; ( j < KC ) && ( t < ( ROUNDS + 1 ) *BC ); j++, t++ )
		for ( i = 0; i < 4; i++ ) keyInst.keySched[t / BC][i][t % BC] = tk[i][j];

	while ( t < ( ROUNDS + 1 ) *BC )
	{ /* while not enough round key material calculated */
		/* calculate new values */
		for ( i = 0; i < 4; i++ )
			tk[i][0] ^= S[tk[ ( i+1 ) %4][KC-1]];
		tk[0][0] ^= rcon[rconpointer++];

		if ( KC != 8 )
			for ( j = 1; j < KC; j++ )
				for ( i = 0; i < 4; i++ ) tk[i][j] ^= tk[i][j-1];
		else
		{
			for ( j = 1; j < KC / 2; j++ )
				for ( i = 0; i < 4; i++ ) tk[i][j] ^= tk[i][j-1];
			for ( i = 0; i < 4; i++ ) tk[i][KC/2] ^= S[tk[i][KC/2 - 1]];
			for ( j = KC / 2 + 1; j < KC; j++ )
				for ( i = 0; i < 4; i++ ) tk[i][j] ^= tk[i][j-1];
		}
		/* copy values into round key array */
		for ( j = 0; ( j < KC ) && ( t < ( ROUNDS + 1 ) *BC ); j++, t++ )
			for ( i = 0; i < 4; i++ ) keyInst.keySched[t / BC][i][t % BC] = tk[i][j];
	}

	return 0;
}

char rijndaelEncrypt ()
{
	/* Encryption of one block.
	 */
	char r;

	/* begin with a key addition
	*/
	KeyAddition ( 0 );

	/* ROUNDS-1 ordinary rounds
	*/
	for ( r = 1; r < ROUNDS; r++ )
	{
		SubstitutionS();
		ShiftRow0();
		MixColumn();
		KeyAddition ( r );
	}

	/* Last round is special: there is no MixColumn
	*/
	SubstitutionS();
	ShiftRow0();
	KeyAddition ( ROUNDS );

	return 0;
}



char rijndaelDecrypt ()
{
	char r;

	/* To decrypt: apply the inverse operations of the encrypt routine,
	 *             in opposite order
	 *
	 * (KeyAddition is an involution: it 's equal to its inverse)
	 * (the inverse of Substitution with table S is Substitution with the inverse table of S)
	 * (the inverse of Shiftrow is Shiftrow over a suitable distance)
	 */

	/* First the special round:
	 *   without InvMixColumn
	 *   with extra KeyAddition
	 */
	KeyAddition ( ROUNDS );
	SubstitutionSi();
	ShiftRow1();

	/* ROUNDS-1 ordinary rounds
	 */
	for ( r = ROUNDS - 1; r > 0; r-- )
	{
		KeyAddition ( r );
		InvMixColumn();
		SubstitutionSi();
		ShiftRow1();
	}

	/* End with the extra key addition
	 */

	KeyAddition ( 0 );

	return 0;
}
