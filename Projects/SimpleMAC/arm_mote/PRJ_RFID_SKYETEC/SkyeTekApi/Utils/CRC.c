/**
 * CRC.c
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * CRC funtions.
 */
#include "CRC.h"

#define MAX_BUFFER_SIZE 512 //STPV3_MAX_ASCII_REQUEST_SIZE

unsigned char crcGetASCIIFromHex(unsigned int c, unsigned char pos)
{
	unsigned char crcbyte;
	if( pos == 0)
		crcbyte = (c & 0x000F);
	else if( pos == 1 )
		crcbyte = (c & 0x00F0) >> 4;
	else if( pos == 2 )
		crcbyte = (c & 0x0F00) >> 8;
	else if( pos == 3 )
		crcbyte = (c & 0xF000) >> 12;
	if( crcbyte <= 9 )//if( crcbyte >= 0 && crcbyte <= 9 )
		crcbyte += 48;
	else if( crcbyte >= 10 && crcbyte <= 15 )
		crcbyte += 55;
	else 
		crcbyte = 48;
	return crcbyte;
}

int crcGetHexFromASCIIBit(unsigned char c)
{
	if( c >= 65 && c <= 70 ) /* uppercase letters A-F */
		return ((int)c-55);
	else if( c >= 97 && c <= 102 ) /* lowercase letters a-f */
		return ((int)c-87);
	else if( c >= 48 && c <= 57 ) /* 1-9 */
		return ((int)c-48);
	else 
		return 0;
}

int crcGetHexFromASCII(unsigned char *ascii, int len)
{
	int ret = 0;
	if( len == 4 )
		ret = (crcGetHexFromASCIIBit(ascii[0]) << 12) | (crcGetHexFromASCIIBit(ascii[1]) << 8) | (crcGetHexFromASCIIBit(ascii[2]) << 4) | crcGetHexFromASCIIBit(ascii[3]);
	else if( len == 3 )
		ret = (crcGetHexFromASCIIBit(ascii[0]) << 8) | (crcGetHexFromASCIIBit(ascii[1]) << 4) | crcGetHexFromASCIIBit(ascii[2]);
	else if( len == 2 )
		ret = (crcGetHexFromASCIIBit(ascii[0]) << 4) | crcGetHexFromASCIIBit(ascii[1]);
	else if( len == 1 )
		ret = crcGetHexFromASCIIBit(ascii[0]);
	return ret;
}


unsigned short crc16OneByte(unsigned short crc, unsigned char data)
{
  unsigned char j;    // bit counter 
  crc ^= data;
  
  for(j = 0; j < 8; j++) /* test each bit in the byte */
    {
      if(crc & 0x0001)
        {
          crc >>= 1;
          crc ^= 0x8408;
        }
      else
        crc >>= 1;
    
    }
  
  return crc;  /* returns calculated crc (16 bits) */
}

unsigned short crc16(unsigned short preset, unsigned char *dataP, unsigned short n)
{
  unsigned short i;	// byte counter 
  unsigned char j;    // bit counter 
  unsigned short crc_16;    /* calculation */
  
  crc_16 = preset;         /* PRESET value */
  
  for(i = 0; i < n; i++)
    {
      crc_16 ^= *dataP++;
    
      for(j = 0; j < 8; j++) /* test each bit in the byte */
        {
          if(crc_16 & 0x0001)
            {
              crc_16 >>= 1;
              crc_16 ^= 0x8408;
            }
          else
            crc_16 >>= 1;
      
        }
    
    }
  
  return(crc_16);  /* returns calculated crc (16 bits) */
}

unsigned short crca16(unsigned short preset, unsigned char *dataP, unsigned short n)
{
	unsigned short checkLen;
	unsigned char check[MAX_BUFFER_SIZE];
	unsigned int ix, iy;
	unsigned short ret;

	if( n < 1 ) 
		return 0;
	
	iy = 0;
	checkLen = n/2;
	//check = (unsigned char *)malloc(checkLen *sizeof(unsigned char));
	for( ix = 0; ix < checkLen; ix++ )
	{
		check[ix] = crcGetHexFromASCII(&dataP[iy],2);
		iy += 2;
	}
	ret = crc16(preset,&check[0],checkLen);
	//free(check);
	return ret;
}


/**
* Subversion tutorial
*/
unsigned char verifycrc(unsigned char *resp, unsigned short len, unsigned char isV3)
{
  unsigned short crc_check;
	unsigned short part1;
	unsigned short part2;
  
	if( isV3 )
		crc_check = crc16(0x0000, resp, len);
	else
		crc_check = crc16(0x0000, resp, len-1);
	part1 = (crc_check >> 8);
	part2 = (crc_check & 0x00FF);
	if( isV3 )
	{
		if((resp[len] == part1) && (resp[len+1] == part2))
			return 1;
		else
			return 0;
	} 
	else
	{
		if((resp[len-1] == part1) && (resp[len] == part2))
			return 1;
		else
			return 0;
	}
}

unsigned char verifyacrc(unsigned char *resp, unsigned short len, unsigned char isV3)
{
	unsigned short crc_check;
	unsigned short part1;
	unsigned short part2;
	unsigned short checkLen;
	unsigned char check[MAX_BUFFER_SIZE];
	unsigned int ix, iy;
	unsigned char ret;

	if( len < 1 ) 
		return 0;
	
	iy = 0;
	checkLen = len/2;
	//check = (unsigned char *)malloc(checkLen *sizeof(unsigned char));
	for( ix = 0; ix < checkLen; ix++ )
	{
		check[ix] = crcGetHexFromASCII(&resp[iy],2);
		iy += 2;
	}
        
	if( isV3  )
        {
		crc_check = crc16(0x0000, check, checkLen-2);
        }
	else
        {
		crc_check = crc16(0x0000, check, checkLen-1);
        }
        
	part1 = (crc_check >> 8);
	part2 = (crc_check & 0x00FF);
	if( isV3 )
	{
		if((check[checkLen-2] == part1) && (check[checkLen-1] == part2))
			ret = 1;
		else
			ret = 0;
	} 
	else
	{
		if((check[len-1] == part1) && (check[len] == part2))
			ret = 1;
		else
			ret = 0;
	}
	//free(check);
	return ret;
}
