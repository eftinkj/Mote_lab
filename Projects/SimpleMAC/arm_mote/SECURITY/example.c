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
*-----------------------------------------------------------------------------
* Description   :
*
*   example of the use of the AES encryption api
*   1. initialisation of the key
*   2. creation of the encryption key
*   3. initialisation of the cipher
*   4. encryption of the block
*   5. creation of the decryption key
*   6. decryption of the block
*
*   see aes.h for the global variables definition
******************************************************************************
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RANGE
#include "aes.h"
#undef RANGE

#include "api_ref.h"
#include "enc_dec.h"

#include <REG52.H>                /* special function register declarations   */
/* for the intended 8051 derivative         */

//#include <stdio.h>                /* prototype declarations for I/O functions */

#ifdef MONITOR51                         /* Debugging with Monitor-51 needs   */
char code reserve [3] _at_ 0x23;         /* space for serial interrupt if     */
#endif                                   /* Stop Exection with Serial Intr.   */
/* is enabled                        */

/*
  BYTE AES_MEMORY blockToEncrypt[BLOCK_SIZE]="tyg123456789123";
  BYTE AES_MEMORY buff1[BLOCK_SIZE];
  BYTE AES_MEMORY buff2[BLOCK_SIZE];
*/


int i, j, l;
char input_text[80];

int main ( void )
{
	/*------------------------------------------------
	Setup the serial port for 1200 baud at 16MHz.
	------------------------------------------------*/
#ifndef MONITOR51
	SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
	TMOD |= 0x20;               /* TMOD: timer 1, mode 2, 8-bit reload        */
	TH1   = 221;                /* TH1:  reload value for 1200 baud @ 16MHz   */
	TR1   = 1;                  /* TR1:  timer 1 run                          */
	TI    = 1;                  /* TI:   set TI to send first char of UART    */
#endif

	/*------------------------------------------------
	Note that an embedded program never exits (because
	there is no operating system to return to).  It
	must loop and execute forever.
	------------------------------------------------*/
	P1 ^= 0x01;     		    /* Toggle P1.0 each time we print */
	//  printf ("Hello World\n");   /* Print "Hello World" */


	strcpy ( input_text, "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF" );



	/************** 1. initialisation of the key ******/
	keyMaterial[0] = 0x0;
	keyMaterial[1] = 0x1;
	keyMaterial[2] = 0x2;
	keyMaterial[3] = 0x3;
	keyMaterial[4] = 0x4;
	keyMaterial[5] = 0x5;
	keyMaterial[6] = 0x6;
	keyMaterial[7] = 0x7;
	keyMaterial[8] = 0x8;
	keyMaterial[9] = 0x9;
	keyMaterial[10] = 0xA;
	keyMaterial[11] = 0xB;
	keyMaterial[12] = 0xC;
	keyMaterial[13] = 0xD;
	keyMaterial[14] = 0xE;
	keyMaterial[15] = 0xF;


	for ( i = 0;i < 5;i++ )
	{
		l = 0;
		for ( j = 0;j < BLOCK_SIZE;j++ )
		{
			blockToEncrypt[l] = input_text[i*16+j];  printf ( "%c", blockToEncrypt[j] );
			l++;
		}
		printf ( "\n" );
		encrypt();

		for ( j = 0;j < BLOCK_SIZE;j++ )
			Encrypted_Message[i*16+j] = blockE[j];
	}

	for ( i = 0;i < MESSAGE_PART;i++ )   printf ( "%c", Encrypted_Message[i] );


	strcpy ( input_text, "00000000000000000000000000000000000000000000000000000000000000000000000000000000" );

	for ( i = 0;i < 5;i++ )
	{
		l = 0;
		for ( j = 0;j < BLOCK_SIZE;j++ )
		{
			blockD[l] = Encrypted_Message[i*16+j];  printf ( "%c", blockToDecrypt[j] );
			l++;
		}
		printf ( "\n" );
		decrypt();

		for ( j = 0;j < BLOCK_SIZE;j++ )
			input_text[i*16+j] = blockD[j];
	}

	for ( i = 0;i < MESSAGE_PART;i++ )   printf ( "%c", input_text[i] );



	return 0;
}


