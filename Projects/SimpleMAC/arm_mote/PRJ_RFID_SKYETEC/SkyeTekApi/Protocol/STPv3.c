/**
 * STPv3.c
 * Copyright ?2006 - 2009 Skyetek, Inc. All Rights Reserved.
 *
 * Implements the SkyeTek Protocol version 2 using
 * the new Transport abstraction layer.
 */
#include "SkyeTekAPI.h" //SKYETEK_STATUS, LPSKYETEK_READER, LPSKYETEK_DATA
#include "Protocol/SkyeTekProtocol.h" //LPSTPV3_REQUEST
#include "Reader/Reader.h"
#include "Tag/TagFactory.h"
#include "Protocol/Protocol.h" //PROTOCOL_TAG_SELECT_CALLBACK
#include "Protocol/STPv3.h"
#include "Utils/CRC.h"
#include "Utils/delay.h"

//Global variables
unsigned char genericID[] = { 0xFF, 0xFF, 0xFF, 0xFF };

// Declareation of private functions
unsigned char getASCIIFromHex(unsigned int c, unsigned char pos);
int getHexFromASCII(unsigned char *ascii, int len);
UINT16 crcBL16(UINT8 *dataP, UINT16 nBytes, UINT16 preset);
UINT16 verifyBLcrc(UINT8 *resp, UINT16 len);
void mset(UINT32 *ptr, UINT8 set, UINT32 size);

#define DELAY() rfid_delay(60)

unsigned char read_rfid_char(unsigned int timeout)
{
    unsigned int to;
    unsigned char chTmp = 0x00;
    to = 0;
    do{
      chTmp = getchar();
      ++to;
      DELAY();
    }while(chTmp == 0xFF && to < timeout);
    return chTmp;
}

// Public functions
SKYETEK_STATUS STPV3_BuildRequest( LPSTPV3_REQUEST req)
{
  unsigned short crc_check;
  unsigned int ix = 0, iy = 0, state = 0, len = 0;
  //int written = 0, totalWritten = 0;

  if( req == NULL )
    return SKYETEK_INVALID_PARAMETER;

  //mset((UINT32*)req->msg, 0, STPV3_MAX_ASCII_REQUEST_SIZE);
  
  /* Write ASCII message */
  if( req->isASCII )
  {
    req->msg[ix++] = STPV3_CR;
    req->msg[ix++] = getASCIIFromHex(req->flags,3);
    req->msg[ix++] = getASCIIFromHex(req->flags,2);
    req->msg[ix++] = getASCIIFromHex(req->flags,1);
    req->msg[ix++] = getASCIIFromHex(req->flags,0);
    req->msg[ix++] = getASCIIFromHex(req->cmd,3);
    req->msg[ix++] = getASCIIFromHex(req->cmd,2);
    req->msg[ix++] = getASCIIFromHex(req->cmd,1);
    req->msg[ix++] = getASCIIFromHex(req->cmd,0);
		if( req->flags & STPV3_RID )
		{
			req->msg[ix++] = getASCIIFromHex(req->rid[0],1);
			req->msg[ix++] = getASCIIFromHex(req->rid[0],0);
			req->msg[ix++] = getASCIIFromHex(req->rid[1],1);
			req->msg[ix++] = getASCIIFromHex(req->rid[1],0);
			req->msg[ix++] = getASCIIFromHex(req->rid[2],1);
			req->msg[ix++] = getASCIIFromHex(req->rid[2],0);
			req->msg[ix++] = getASCIIFromHex(req->rid[3],1);
			req->msg[ix++] = getASCIIFromHex(req->rid[3],0);
		}
		if( (req->cmd >> 8) == 0x01 || (req->cmd >> 8) == 0x02 ||
        (req->cmd >> 8) == 0x03 || (req->cmd >> 8) == 0x04 ||
        (req->cmd >> 8) == 0x05 || (req->cmd >> 8) == 0x06 )
		{
			req->msg[ix++] = getASCIIFromHex(req->tagType,3);
			req->msg[ix++] = getASCIIFromHex(req->tagType,2);
			req->msg[ix++] = getASCIIFromHex(req->tagType,1);
			req->msg[ix++] = getASCIIFromHex(req->tagType,0);
		}
		if( req->flags & STPV3_TID )
		{
			req->msg[ix++] = getASCIIFromHex(req->tidLength,1);
			req->msg[ix++] = getASCIIFromHex(req->tidLength,0);
			for( iy = 0; iy < req->tidLength; iy++ )
			{
				req->msg[ix++] = getASCIIFromHex(req->tid[iy],1);
				req->msg[ix++] = getASCIIFromHex(req->tid[iy],0);
			}
		}
		if( req->flags & STPV3_AFI )
		{
			req->msg[ix++] = getASCIIFromHex(req->afi,1);
			req->msg[ix++] = getASCIIFromHex(req->afi,0);
		}
		if( req->flags & STPV3_SESSION )
		{
			req->msg[ix++] = getASCIIFromHex(req->session,1);
			req->msg[ix++] = getASCIIFromHex(req->session,0);
		}
		state = STPV3_IsAddressOrDataCommand(req->cmd);
		if( state & STPV3_FORMAT_ADDRESS )
		{
			req->msg[ix++] = getASCIIFromHex(req->address[0],1);
			req->msg[ix++] = getASCIIFromHex(req->address[0],0);
			req->msg[ix++] = getASCIIFromHex(req->address[1],1);
			req->msg[ix++] = getASCIIFromHex(req->address[1],0);
		}
		if( state & STPV3_FORMAT_BLOCKS )
		{
			req->msg[ix++] = getASCIIFromHex(req->numBlocks,3);
			req->msg[ix++] = getASCIIFromHex(req->numBlocks,2);
			req->msg[ix++] = getASCIIFromHex(req->numBlocks,1);
			req->msg[ix++] = getASCIIFromHex(req->numBlocks,0);
		}
		if( req->flags & STPV3_DATA )
		{
			req->msg[ix++] = getASCIIFromHex(req->dataLength,3);
			req->msg[ix++] = getASCIIFromHex(req->dataLength,2);
			req->msg[ix++] = getASCIIFromHex(req->dataLength,1);
			req->msg[ix++] = getASCIIFromHex(req->dataLength,0);
			for( iy = 0; iy < req->dataLength; iy++ )
			{
				req->msg[ix++] = getASCIIFromHex(req->data[iy],1);
				req->msg[ix++] = getASCIIFromHex(req->data[iy],0);
			}
		}
                /* Calculate CRC */
		if( req->flags & STPV3_CRC )
		{
		    crc_check = crca16(0x0000, (req->msg)+1, (ix-1));
                    req->msg[ix++] = getASCIIFromHex((crc_check >> 8),1);
                    req->msg[ix++] = getASCIIFromHex((crc_check >> 8),0);
                    req->msg[ix++] = getASCIIFromHex((crc_check & 0x00FF),1);
                    req->msg[ix++] = getASCIIFromHex((crc_check & 0x00FF),0);
		}
          req->msg[ix++] = STPV3_CR;
	  req->msgLength = ix;
  }

  /* Write binary message */
  else
  {
      req->msg[0] = STPV3_STX;
    ////SkyeTek_Debug(_T("STX:\t%02X\r\n"),req->msg[0]);
    ix = 3;
    req->msg[ix++] = req->flags >> 8;
    req->msg[ix++] = req->flags & 0x00FF;
    
//    ////SkyeTek_Debug(_T("FLAGS:\t%02X%02X ( "),req->msg[ix-2],req->msg[ix-1]);
//    if( req->flags & STPV3_LOOP )
//      ////SkyeTek_Debug(_T("LOOP "));
//    if( req->flags & STPV3_INV )
//      ////SkyeTek_Debug(_T("INV "));
//    if( req->flags & STPV3_LOCK )
//      ////SkyeTek_Debug(_T("LOCK "));
//    if( req->flags & STPV3_RF )
//      ////SkyeTek_Debug(_T("RF "));
//    if( req->flags & STPV3_AFI )
//      ////SkyeTek_Debug(_T("AFI "));
//    if( req->flags & STPV3_CRC )
//      ////SkyeTek_Debug(_T("CRC "));
//    if( req->flags & STPV3_TID )
//      ////SkyeTek_Debug(_T("TID "));
//    if( req->flags & STPV3_RID )
//      ////SkyeTek_Debug(_T("RID "));
//    if( req->flags & STPV3_ENCRYPTION )
//      ////SkyeTek_Debug(_T("ENC "));
//    if( req->flags & STPV3_HMAC )
//      ////SkyeTek_Debug(_T("HMAC "));
//    if( req->flags & STPV3_SESSION )
//      ////SkyeTek_Debug(_T("SESS "));
//    if( req->flags & STPV3_DATA )
//      ////SkyeTek_Debug(_T("DATA "));
//    ////SkyeTek_Debug(_T(")\r\n"));


    req->msg[ix++] = req->cmd >> 8;
    req->msg[ix++] = req->cmd & 0x00FF;
    ////SkyeTek_Debug(_T("CMD:\t%02X%02X\r\n"),req->msg[ix-2],req->msg[ix-1]);
		if( req->flags & STPV3_RID )
		{
			req->msg[ix++] = req->rid[0];
			req->msg[ix++] = req->rid[1];
			req->msg[ix++] = req->rid[2];
			req->msg[ix++] = req->rid[3];
      ////SkyeTek_Debug(_T("RID:\t%02X%02X%02X%02X\r\n"),req->msg[ix-4],req->msg[ix-3],req->msg[ix-2],req->msg[ix-1]);
		}
		if( (req->cmd >> 8) == 0x01 || (req->cmd >> 8) == 0x02 ||
        (req->cmd >> 8) == 0x03 || (req->cmd >> 8) == 0x04 ||
        (req->cmd >> 8) == 0x05 || (req->cmd >> 8) == 0x06)
		{
			req->msg[ix++] = req->tagType >> 8;
			req->msg[ix++] = req->tagType & 0x00FF;
      ////SkyeTek_Debug(_T("TTYP:\t%02X%02X\r\n"),req->msg[ix-2],req->msg[ix-1]);
		  if( req->flags & STPV3_TID )
		  {
			  req->msg[ix++] = (unsigned char)req->tidLength;
			  if( req->tidLength > 0 )
        {
          ////SkyeTek_Debug(_T("TID:\t"));
				  for( iy = 0; iy < req->tidLength && iy < 16; iy++ )
          {
					  req->msg[ix++] = req->tid[iy];
            ////SkyeTek_Debug(_T("%02X"),req->msg[ix-1]);
          }
          ////SkyeTek_Debug(_T("\r\n"));
        }
		  }
		}
		if( req->flags & STPV3_AFI )
    {
			req->msg[ix++] = (unsigned char)req->afi;
      ////SkyeTek_Debug(_T("AFI:\t%02X\r\n"),req->msg[ix-1]);
    }
		if( req->flags & STPV3_SESSION )
    {
			req->msg[ix++] = (unsigned char)req->session;
      ////SkyeTek_Debug(_T("SESS:\t%02X\r\n"),req->msg[ix-1]);
    }
		state = STPV3_IsAddressOrDataCommand(req->cmd);
		if( state & STPV3_FORMAT_ADDRESS )
		{
			req->msg[ix++] = req->address[0];
			req->msg[ix++] = req->address[1];
      ////SkyeTek_Debug(_T("ADDR:\t%02X%02X\r\n"),req->msg[ix-2],req->msg[ix-1]);
		}
		if( state & STPV3_FORMAT_BLOCKS )
		{
			req->msg[ix++] = req->numBlocks >> 8;
			req->msg[ix++] = req->numBlocks & 0x00FF;
      ////SkyeTek_Debug(_T("BLKS:\t%02X%02X\r\n"),req->msg[ix-2],req->msg[ix-1]);
		} 
		if( req->flags & STPV3_DATA )
		{
			req->msg[ix++] = req->dataLength >> 8;
			req->msg[ix++] = req->dataLength & 0x00FF;
      ////SkyeTek_Debug(_T("DLEN:\t%02X%02X\r\n"),req->msg[ix-2],req->msg[ix-1]);
			if( req->dataLength > 0 )
			{
        ////SkyeTek_Debug(_T("DATA:\t"));
				for( iy = 0; iy < req->dataLength && iy < 2048; iy++ )
        {
					req->msg[ix++] = req->data[iy];
          ////SkyeTek_Debug(_T("%02X"),req->msg[ix-1]);
        }
        ////SkyeTek_Debug(_T("\r\n"));
			}
		}
    /* Set Length */
		len = ix - 1; /* Minus 3 plus 2 byte CRC */
		req->msg[1] = len >> 8;
		req->msg[2] = len & 0x00FF; 
    ////SkyeTek_Debug(_T("LEN:\t%02X%02X\r\n"),req->msg[1],req->msg[2]);
		/* Calculate CRC */
		crc_check = crc16((unsigned short)0x0000, req->msg+1, (ix-1));
		req->msg[ix++] = crc_check >> 8;
		req->msg[ix++] = crc_check & 0x00FF;
    ////SkyeTek_Debug(_T("CRC:\t%02X%02X\r\n"),req->msg[ix-2],req->msg[ix-1]);
		req->msgLength = ix;
  }
  
  return SKYETEK_SUCCESS;
}

SKYETEK_STATUS STPV3_WriteRequest( LPSKYETEK_DEVICE lpDevice, LPSTPV3_REQUEST req, unsigned int timeout)
{
	int msgLen = 0;
	int i = 0;
	SKYETEK_STATUS status;
  
        if(req == NULL )
          return SKYETEK_INVALID_PARAMETER;

        if( (status = STPV3_BuildRequest(req)) != SKYETEK_SUCCESS )
	  return status;
        
        msgLen = req->msgLength;
        i=0;
        while(msgLen-- > 0)
        {
          // May consider timeout later
          putchar(req->msg[i++]);
        }
  return SKYETEK_SUCCESS;
}

SKYETEK_STATUS STPV3_ReadResponseImpl(
  LPSKYETEK_DEVICE      lpDevice, 
  LPSTPV3_REQUEST       req, 
  LPSTPV3_RESPONSE      resp,
  unsigned int          timeout
  )
{
  int msgLen = 0;
  unsigned char chTmp;
  int i = 0;
  int j = 0;
  unsigned int length = 0, ix = 0, iy=0;

  //mset((UINT32*)resp, 0, sizeof(STPV3_RESPONSE));

  if(resp == NULL )
    return SKYETEK_INVALID_PARAMETER;
  
  if( req->isASCII )
  {
    /* Read LF  */
    //unsigned char tmp;
    resp->msg[0] = read_rfid_char(timeout);
    
    /* Check response */
    if( resp->msg[0] != STPV3_LF )
    {
      return SKYETEK_READER_PROTOCOL_ERROR;
    }

    msgLen = 1;
    do{
      chTmp = read_rfid_char(timeout);     
      resp->msg[msgLen]=chTmp;
      msgLen++;
      if(chTmp == 0xFF) return SKYETEK_READER_PROTOCOL_ERROR;
    }while(chTmp != STPV3_LF);
    resp->msgLength = msgLen - 2;

    /* Copy over */
    i = 1;
    resp->code = getHexFromASCII(&resp->msg[i],4);
    i += 4;

    /* Check for error code */
    if( STPV3_IsErrorResponse(resp->code) )
    {
        if( req->flags & STPV3_CRC && resp->msgLength > 3 ) 
        {
                if(!verifyacrc((resp->msg)+1, resp->msgLength-4, true))
                {
                        return SKYETEK_INVALID_CRC;
                }
        }
	return SKYETEK_SUCCESS;
    }
		
    /* RID */		
    if( req->flags & STPV3_RID )
    {
      resp->rid[0] = getHexFromASCII(&resp->msg[i],2);
      i += 2;
      resp->rid[1] = getHexFromASCII(&resp->msg[i],2);
      i += 2;
      resp->rid[2] = getHexFromASCII(&resp->msg[i],2);
      i += 2;
      resp->rid[3] = getHexFromASCII(&resp->msg[i],2);
      i += 2;
    }
    
    if( resp->code == STPV3_RESP_SELECT_TAG_PASS && !(req->tagType & 0x0000000F) /* auto-detect */ )
    {
      resp->tagType = getHexFromASCII(&resp->msg[i],4);
      i += 4;
    }
    
    msgLen = resp->msgLength-1;
    
    if( req->flags & STPV3_CRC )
      msgLen -= 4;
    
    if( msgLen > i )
    {
      resp->dataLength = getHexFromASCII(&resp->msg[i],4);
      resp->dataLength *= 2; /* ASCII is twice binary */
      if( resp->dataLength > resp->msgLength-i || resp->dataLength > 2048 )
      {
        resp->dataLength = 0;
	return SKYETEK_READER_PROTOCOL_ERROR;
      }
      i += 4;
      if( resp->dataLength > 0 )
      {
        for( j = 0; j < resp->dataLength; j++ )
	  resp->data[j] = resp->msg[i++];
      }
    }
		
    /* Verify CRC */
    if( req->flags & STPV3_CRC && resp->msgLength > 3 ) 
    {
      if(!verifyacrc((resp->msg)+1, resp->msgLength-3, 1))
      {
	return SKYETEK_INVALID_CRC;
      }
    }
  }//if( req->isASCII )

  /* Binary mode */
  else
  {
    /* Read STX and message length */
    do{
      resp->msg[0] = getchar();
      //tmp = getchar();
    }while(resp->msg[0] == 0xFF);
    resp->msg[1] = getchar();
    resp->msg[2] = getchar();

  
		/* Check response */
		if( resp->msg[0] != STPV3_STX )
		{
			//STP_DebugMsg(_T("response"), resp->msg, 3, req->isASCII);
			if( resp->msg[0] == STPV3_NACK )
				return SKYETEK_READER_IN_BOOT_LOAD_MODE;
			else
				return SKYETEK_READER_PROTOCOL_ERROR;
		}

		/* Check message length */
		length = (resp->msg[1] << 8) | resp->msg[2];
		if( length > STPV3_MAX_ASCII_RESPONSE_SIZE )
		{
			//STP_DebugMsg(_T("response"), resp->msg, 3, req->isASCII);
			return SKYETEK_READER_PROTOCOL_ERROR;
		}

		/* Read in rest of message */
                
                for(i = 0; i<length; i++)
                {
                  do{
                    chTmp = getchar();
                  }while(chTmp == 0xFF);
                }
                
		resp->msgLength = length + 3;

		if( resp->msgLength > STPV3_MAX_ASCII_RESPONSE_SIZE )
		{
                        resp->msgLength = 0;
			//STP_DebugMsg(_T("response"), resp->msg, 3, req->isASCII);
			return SKYETEK_READER_PROTOCOL_ERROR;
		}

		/* Get code */
		ix = 3;
		resp->code = (resp->msg[ix] << 8) | resp->msg[ix+1];
		ix += 2;

		//STP_DebugMsg(_T("response"), resp->msg, resp->msgLength, req->isASCII);
		//SkyeTek_Debug(_T("code: %s\r\n"), STPV3_LookupResponse(resp->code));

		/* Check for error code */
		if( STPV3_IsErrorResponse(resp->code) )
		{
			resp->msgLength = ix + 2; /* for CRC */
			if(!verifycrc((resp->msg)+1, length, 1))
				return SKYETEK_INVALID_CRC;
			else
				return SKYETEK_SUCCESS;
		}

		/* Otherwise, process response */
		if( req->flags & STPV3_RID )
		{
			for( iy = 0; iy < 4; iy++ )
				resp->rid[iy] = resp->msg[ix++];
		}
		if( resp->code == STPV3_RESP_SELECT_TAG_PASS && !(req->tagType & 0x0000000F) /* auto-detect */ )
		{
			resp->tagType = (SKYETEK_TAGTYPE)((resp->msg[ix] << 8) | resp->msg[ix+1]);
			ix += 2;
		}
		if( length + 1 > ix )
		{
			resp->dataLength = (resp->msg[ix] << 8) | resp->msg[ix+1];
			if( resp->dataLength > length || resp->dataLength > sizeof(resp->data) )
                        {
                          resp->dataLength = 0;
			  return SKYETEK_READER_PROTOCOL_ERROR;
                        }
			ix += 2;
			if( resp->dataLength > 0 )
			{
				for( iy = 0; iy < resp->dataLength; iy++ )
					resp->data[iy] = resp->msg[ix++];
			}
		}
		resp->msgLength = ix + 2; /* for CRC */

		/* Verify CRC */
		if(!verifycrc((resp->msg)+1, length, 1))
			return SKYETEK_INVALID_CRC;

  }
	
    return SKYETEK_SUCCESS;
} 

SKYETEK_API SKYETEK_STATUS STPV3_ReadResponse(
  LPSKYETEK_DEVICE      lpDevice, 
  LPSTPV3_REQUEST       req, 
  LPSTPV3_RESPONSE      resp,
  unsigned int          timeout
  )
{
  SKYETEK_STATUS st;
  unsigned int count = 0;
  
read:
  st = STPV3_ReadResponseImpl(lpDevice, req, resp,timeout);
  count++;
  if( resp->code == 0 || resp->code & 0x00008000 )
  {
    return st;
  }
  else if( req->cmd == STPV3_CMD_SELECT_TAG &&
           resp->code == STPV3_RESP_SELECT_TAG_LOOP_ON )
  {
    return st;
  }
  else if( (resp->code & 0x00007FFF) == (req->cmd & 0x00007FFF) )
  {
    return st;
  }
  else if( req->anyResponse )
  {
    return st;
  }
  else
  {
    //////SkyeTek_Debug(_T("error: response code 0x%X doesn't match request 0x%X\r\n"), resp->code, req->cmd);
    count++;
    if( count > 10 )
      return st;
    goto read;
  }
}

SKYETEK_STATUS 
STPV3_SendCommand(
    LPSKYETEK_READER     lpReader,
    unsigned int         cmd,
    unsigned int         flags,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
  
	/* Send request */
	//mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));

	req.cmd = cmd;
	req.flags = STPV3_CRC;  // NO CRC
        req.flags |= flags;
  
        lpri = (LPREADER_IMPL)lpReader->internal;
        if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
        {
          lpri->CopyRIDToBuffer(lpReader,req.rid);
          req.flags |= STPV3_RID;
        }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice,&req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
        
	status = STPV3_ReadResponse(lpReader->lpDevice,&req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
    
  return SKYETEK_SUCCESS; 

}

SKYETEK_STATUS 
STPV3_SendGetCommand(
    LPSKYETEK_READER     lpReader,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       *lpData,
    unsigned int         timeout
    )
{
  STPV3_REQUEST req;
  STPV3_RESPONSE resp;
  SKYETEK_STATUS status;
  LPREADER_IMPL lpri;
  
  if(lpData == NULL )
    return SKYETEK_INVALID_PARAMETER;
  
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC; // No CRC
  req.flags |= flags;
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
  status = STPV3_WriteRequest(lpReader->lpDevice,&req, timeout);
  if( status != SKYETEK_SUCCESS )
	return status;

  //mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
  status = STPV3_ReadResponse(lpReader->lpDevice,&req, &resp, timeout);
  if( status != SKYETEK_SUCCESS )
	return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
  
  //*lpData = SkyeTek_AllocateData(resp.dataLength); // Super function should give a MAX size data buffer for lpData
  if( *lpData == NULL )
    return SKYETEK_OUT_OF_MEMORY;
  return SkyeTek_CopyBuffer(*lpData,resp.data,resp.dataLength);
}

SKYETEK_STATUS 
STPV3_SendSetCommand(
    LPSKYETEK_READER     lpReader,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       lpData,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if( lpData == NULL || lpData->data == NULL || lpData->size > 2048 )
    return SKYETEK_INVALID_PARAMETER;
   
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC | STPV3_DATA;
  req.flags |= flags;
	req.dataLength = lpData->size;
  if( req.dataLength > 2048 )
    req.dataLength = 2048;
	for(iy = 0; iy < req.dataLength; iy++)
		req.data[iy] = lpData->data[iy];
 
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }
 
writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;


	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
    
  return SKYETEK_SUCCESS; 

}

SKYETEK_STATUS 
STPV3_SendAddrGetCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_ADDRESS    lpAddr,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       *lpData,
    unsigned int         timeout
    )
{
  STPV3_REQUEST req;
  STPV3_RESPONSE resp;
  SKYETEK_STATUS status;
  LPREADER_IMPL lpri;
  
  if(lpAddr == NULL || lpData == NULL )
    return SKYETEK_INVALID_PARAMETER;
  
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC;
  req.flags |= flags;
  req.address[0] = lpAddr->start >> 8;
  req.address[1] = lpAddr->start & 0x00FF;
  req.numBlocks = lpAddr->blocks; 
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice,&req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice,&req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
  
//  *lpData = SkyeTek_AllocateData(resp.dataLength);// Super function should give a MAX size data buffer for lpData
  if( *lpData == NULL )
    return SKYETEK_OUT_OF_MEMORY;
  return SkyeTek_CopyBuffer(*lpData,resp.data,resp.dataLength);
}

SKYETEK_STATUS 
STPV3_SendAddrSetCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_ADDRESS    lpAddr,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       lpData,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if(lpAddr == NULL || lpData == NULL ||
    lpData->data == NULL || lpData->size > 2048 )
    return SKYETEK_INVALID_PARAMETER;
   
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC | STPV3_DATA;
  req.flags |= flags;
  req.address[0] = lpAddr->start >> 8;
  req.address[1] = lpAddr->start & 0x00FF;
  req.numBlocks = lpAddr->blocks;
  req.dataLength = lpData->size;
  if( req.dataLength > 2048 )
    req.dataLength = 2048;
  for(iy = 0; iy < req.dataLength; iy++)
    req.data[iy] = lpData->data[iy];
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
    
  return SKYETEK_SUCCESS; 
}

SKYETEK_STATUS 
STPV3_SendTagCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    unsigned int         cmd,
    unsigned int         flags,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if(lpTag == NULL )
    return SKYETEK_INVALID_PARAMETER;

  /* Build request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC;
  req.flags |= flags;
  req.session = lpTag->session;
  req.afi = lpTag->afi;
  req.tagType = lpTag->type;
  if( (flags & STPV3_TID) && lpTag->id != NULL && lpTag->id->id != NULL )
  {
    req.tidLength = lpTag->id->length;
    if( req.tidLength > 16 )
      req.tidLength = 16;
    for(iy = 0; iy < req.tidLength; iy++)
      req.tid[iy] = lpTag->id->id[iy];
  }

  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	/* Send request */
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

	/* Read response */
	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
  
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
  
  return SKYETEK_SUCCESS;	
}

SKYETEK_STATUS 
STPV3_SendGetTagCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       *lpData,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if(lpTag == NULL || lpData == NULL )
    return SKYETEK_INVALID_PARAMETER;

  
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC;
  req.flags |= flags;
  req.session = lpTag->session;
  req.afi = lpTag->afi;
  req.tagType = lpTag->type;
  
  if( (flags & STPV3_TID) && lpTag->id != NULL && lpTag->id->id != NULL )
  {
    req.tidLength = lpTag->id->length;
    if( req.tidLength > 16 )
      req.tidLength = 16;
    for(iy = 0; iy < req.tidLength; iy++)
      req.tid[iy] = lpTag->id->id[iy];
  }
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
  
  //*lpData = SkyeTek_AllocateData(resp.dataLength); // Super function should give a MAX size data buffer for lpData
  if( *lpData == NULL )
    return SKYETEK_OUT_OF_MEMORY;
  return SkyeTek_CopyBuffer(*lpData,resp.data,resp.dataLength);

}

SKYETEK_STATUS 
STPV3_SendSetTagCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       lpData,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if(lpTag == NULL || lpData == NULL ||
    lpData->data == NULL || lpData->size > 2048 )
    return SKYETEK_INVALID_PARAMETER;
  
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC | STPV3_DATA; 
  req.flags |= flags;
  req.session = lpTag->session;
  req.afi = lpTag->afi;
  req.tagType = lpTag->type;
  
  if( (flags & STPV3_TID) && lpTag->id != NULL && lpTag->id->id != NULL )
  {
    req.tidLength = lpTag->id->length;
    if( req.tidLength > 16 )
      req.tidLength = 16;
    for(iy = 0; iy < req.tidLength; iy++)
      req.tid[iy] = lpTag->id->id[iy];
  }
  
  req.dataLength = lpData->size;
  if( req.dataLength > 2048 )
    req.dataLength = 2048;
  for(iy = 0; iy < req.dataLength; iy++)
    req.data[iy] = lpData->data[iy];
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

        //mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
    
  return SKYETEK_SUCCESS; 

}

SKYETEK_STATUS 
STPV3_SendGetSetTagCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       lpSendData,
    LPSKYETEK_DATA       *lpRecvData,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if(lpTag == NULL || lpSendData == NULL ||
    lpSendData->data == NULL || lpSendData->size > 2048 || lpRecvData == NULL )
    return SKYETEK_INVALID_PARAMETER;
  
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC | STPV3_DATA;
  req.flags |= flags;
  req.session = lpTag->session;
  req.afi = lpTag->afi;
  req.tagType = lpTag->type;
  if( (flags & STPV3_TID) && lpTag->id != NULL && lpTag->id->id != NULL )
  {
    req.tidLength = lpTag->id->length;
    if( req.tidLength > 16 )
      req.tidLength = 16;
    for(iy = 0; iy < req.tidLength; iy++)
      req.tid[iy] = lpTag->id->id[iy];
  }
  req.dataLength = lpSendData->size;
  if( req.dataLength > 2048 )
    req.dataLength = 2048;
  for(iy = 0; iy < req.dataLength; iy++)
    req.data[iy] = lpSendData->data[iy];
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
    
  //*lpRecvData = SkyeTek_AllocateData(resp.dataLength); // Super function should give a MAX size data buffer for lpData
  if( *lpRecvData == NULL )
    return SKYETEK_OUT_OF_MEMORY;
  return SkyeTek_CopyBuffer(*lpRecvData,resp.data,resp.dataLength);
}

SKYETEK_STATUS 
STPV3_SendAddrGetTagCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ADDRESS    lpAddr,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       *lpData,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if(lpTag == NULL || lpAddr == NULL || lpData == NULL )
    return SKYETEK_INVALID_PARAMETER;
  
  
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC;
  req.flags |= flags;
  req.session = lpTag->session;
  req.afi = lpTag->afi;
  req.tagType = lpTag->type;
  if( (flags & STPV3_TID) && lpTag->id != NULL && lpTag->id->id != NULL )
  {
    req.tidLength = lpTag->id->length;
    if( req.tidLength > 16 )
      req.tidLength = 16;
    for(iy = 0; iy < req.tidLength; iy++)
      req.tid[iy] = lpTag->id->id[iy];
  }
  req.address[0] = lpAddr->start >> 8;
  req.address[1] = lpAddr->start & 0x00FF;
  req.numBlocks = lpAddr->blocks;
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
  
  //*lpData = SkyeTek_AllocateData(resp.dataLength); // Super function should give a MAX size data buffer for lpData
  if( *lpData == NULL )
    return SKYETEK_OUT_OF_MEMORY;
  return SkyeTek_CopyBuffer(*lpData,resp.data,resp.dataLength);

}

SKYETEK_STATUS 
STPV3_SendAddrSetTagCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ADDRESS    lpAddr,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       lpData,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if(lpTag == NULL || lpAddr == NULL || 
      lpData == NULL || lpData->size > 2048 )
    return SKYETEK_INVALID_PARAMETER;

  /* Build request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC | STPV3_DATA;
  req.flags |= flags;
  req.session = lpTag->session;
  req.afi = lpTag->afi;
  req.tagType = lpTag->type;
  if( (flags & STPV3_TID) && lpTag->id != NULL && lpTag->id->id != NULL )
  {
    req.tidLength = lpTag->id->length;
    if( req.tidLength > 16 )
      req.tidLength = 16;
    for(iy = 0; iy < req.tidLength; iy++)
      req.tid[iy] = lpTag->id->id[iy];
  }
  req.address[0] = lpAddr->start >> 8;
  req.address[1] = lpAddr->start & 0x00FF;
  req.numBlocks = lpAddr->blocks;
  req.dataLength = lpData->size;
  if( req.dataLength > 2048 )
    req.dataLength = 2048;
  for(iy = 0; iy < req.dataLength; iy++)
    req.data[iy] = lpData->data[iy];
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;  

	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
  
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
  {
	  // hack around firmware returning same status as inventory done
	  if( resp.code == STPV3_RESP_SELECT_TAG_INVENTORY_DONE )
		  return SKYETEK_FAILURE;
	  else
		return STPV3_GetStatus(resp.code);
  }
  
  return SKYETEK_SUCCESS;	
}

SKYETEK_STATUS 
STPV3_SendAddrGetSetTagCommand(
    LPSKYETEK_READER     lpReader,
    LPSKYETEK_TAG        lpTag,
    LPSKYETEK_ADDRESS    lpAddr,
    unsigned int         cmd,
    unsigned int         flags,
    LPSKYETEK_DATA       lpSendData,
    LPSKYETEK_DATA       *lpRecvData,
    unsigned int         timeout
    )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	unsigned int iy = 0;

  if(lpTag == NULL || lpSendData == NULL ||
    lpSendData->data == NULL || lpSendData->size > 2048 || lpRecvData == NULL )
    return SKYETEK_INVALID_PARAMETER;
  
  /* Send request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = cmd;
  req.flags = STPV3_CRC | STPV3_DATA;
  req.flags |= flags;
  req.session = lpTag->session;
  req.afi = lpTag->afi;
  req.tagType = lpTag->type;
  if( (flags & STPV3_TID) && lpTag->id != NULL && lpTag->id->id != NULL )
  {
    req.tidLength = lpTag->id->length;
    if( req.tidLength > 16 )
      req.tidLength = 16;    
    for(iy = 0; iy < req.tidLength; iy++)
      req.tid[iy] = lpTag->id->id[iy];
  }
  req.address[0] = lpAddr->start >> 8;
  req.address[1] = lpAddr->start & 0x00FF;
  req.numBlocks = lpAddr->blocks;
  req.dataLength = lpSendData->size;
  if( req.dataLength > 2048 )
    req.dataLength = 2048;
  for(iy = 0; iy < req.dataLength; iy++)
    req.data[iy] = lpSendData->data[iy];
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }

writeCommand:
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;

	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
	
  if(resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF)
    goto writeCommand;
  else if(resp.code != cmd)
    return STPV3_GetStatus(resp.code);
    
  //*lpRecvData = SkyeTek_AllocateData(resp.dataLength); // Super function should give a MAX size data buffer for lpData
  if( *lpRecvData == NULL )
    return SKYETEK_OUT_OF_MEMORY;
  return SkyeTek_CopyBuffer(*lpRecvData,resp.data,resp.dataLength);
}

/* Reader Functions */
SKYETEK_STATUS 
STPV3_StopSelectLoop(
  LPSKYETEK_READER      lpReader,
  unsigned int          timeout
  )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
        
  if(lpReader == NULL)
    return SKYETEK_INVALID_PARAMETER;

  /* Build request */
  //mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
  req.cmd = STPV3_CMD_SELECT_TAG;
  req.flags = STPV3_CRC;
  
  lpri = (LPREADER_IMPL)lpReader->internal;
  if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
  {
    lpri->CopyRIDToBuffer(lpReader,req.rid);
    req.flags |= STPV3_RID;
  }
	
  /* Send request */
  status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
  if( status != SKYETEK_SUCCESS )
    return status;

	/* Read response */
readResponse:
	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
  if( status != SKYETEK_SUCCESS )
    return status;
  if( resp.code != STPV3_RESP_SELECT_TAG_LOOP_OFF )
    goto readResponse;
  return status;
}

//SKYETEK_STATUS 
//STPV3_SelectTags(
//  SKYETEK_TAGTYPE              tagType, 
//  PROTOCOL_TAG_SELECT_CALLBACK callback,
//  PROTOCOL_FLAGS               flags,
//  void                         *user,
//  unsigned int                 timeout
//  )
//{
//	STPV3_REQUEST req;
//	STPV3_RESPONSE resp;
//	SKYETEK_STATUS status;
//        SKYETEK_DATA data;
//        unsigned char buffer[STPV3_MAX_ASCII_RESPONSE_SIZE];
//        data.data = &buffer[0];
//        
//        mset((UINT8*)&buffer[0], 0, STPV3_MAX_ASCII_RESPONSE_SIZE);
//	int ix = 0, iy = 0;
//
//  if(callback == 0)
//    return SKYETEK_INVALID_PARAMETER;
//
//  /* Build request */
//  mset(&req,0,sizeof(STPV3_REQUEST));
//  req.cmd = STPV3_CMD_SELECT_TAG;
//  req.flags = (flags.isInventory == 1) ? STPV3_INV : 0;//No CRC, req.flags = STPV3_CRC | ((flags.isInventory == 1) ? STPV3_INV : 0);
//  req.flags = req.flags | ((flags.isLoop == 1) ? STPV3_LOOP : 0);
//  req.tagType = tagType;
//  
//  // NO RID
//
//	/* Send request */
//	status = STPV3_WriteRequest(&req, timeout);
//	if( status != SKYETEK_SUCCESS )
//		return status;
//
//readResponse:
//  mset(&resp,0,sizeof(STPV3_RESPONSE));
//  status = STPV3_ReadResponse(&req, &resp, timeout);
//  if( status == SKYETEK_TIMEOUT )
//  {
//    if(!callback(tagType, NULL, user))
//    {
//      STPV3_StopSelectLoop(timeout);
//      return SKYETEK_SUCCESS;
//    }
//    goto readResponse;
//  }
//	if( status != SKYETEK_SUCCESS )
//		return status;
//
//	if( resp.code == STPV3_RESP_SELECT_TAG_LOOP_ON )
//		goto readResponse;
//
//	if( resp.code == STPV3_RESP_SELECT_TAG_FAIL || resp.code == STPV3_RESP_SELECT_TAG_LOOP_OFF )
//                return SKYETEK_SUCCESS;
//
//	if( resp.code == STPV3_RESP_SELECT_TAG_PASS )
//	{
//	  /* Copy over tag information */
//          if( resp.tagType != 0 )
//            tagType = (SKYETEK_TAGTYPE)resp.tagType;
//          else
//            tagType = (SKYETEK_TAGTYPE)req.tagType;
//
//    //lpd is subtituded by data, lpd = SkyeTek_AllocateData(resp.dataLength);
//    SkyeTek_CopyBuffer(&data,resp.data,resp.dataLength);
//  
//    /* Call the callback */
//    if(!callback(tagType, lpd, user))
//    {
//      //SkyeTek_FreeData(lpd);
//      STPV3_StopSelectLoop(timeout);
//      return SKYETEK_SUCCESS;
//    }
//    //SkyeTek_FreeData(lpd);
//
//    /* Check for bail */
//    if(!flags.isInventory && !flags.isLoop)
//      return SKYETEK_SUCCESS;
//		
//    /* Keep reading */
//    goto readResponse;
//  }
//  return status;
//}

SKYETEK_STATUS 
STPV3_GetTags(
  LPSKYETEK_READER   lpReader, 
  SKYETEK_TAGTYPE    tagType, 
  LPTAGTYPE_ARRAY    tagTypes, 
  LPSKYETEK_DATA     lpData, 
  unsigned int       *count,
  unsigned int       timeout
  )
{
	STPV3_REQUEST req;
	STPV3_RESPONSE resp;
	SKYETEK_STATUS status;
        LPREADER_IMPL lpri;
	
        //unsigned int num = 0;
        *count = 0;

        if(lpReader == NULL || lpData == NULL )
        {
          return SKYETEK_INVALID_PARAMETER;
        }
  
        if( lpReader->internal == NULL)
        {
          return SKYETEK_INVALID_PARAMETER;
        }

	/* Build request */
	//mset((UINT32*)&req,0,sizeof(STPV3_REQUEST));
               
	req.cmd = STPV3_CMD_SELECT_TAG;
        req.tagType = tagType;
	req.flags = STPV3_INV;//STPV3_CRC | STPV3_INV;
        
        req.isASCII = true;
	
        
        lpri = (LPREADER_IMPL)lpReader->internal;
        if( lpReader->sendRID || !lpri->DoesRIDMatch(lpReader,genericID) )
        {
          lpri->CopyRIDToBuffer(lpReader,req.rid);
          req.flags |= STPV3_RID;
        }

	/* Send request */
	status = STPV3_WriteRequest(lpReader->lpDevice, &req, timeout);
	if( status != SKYETEK_SUCCESS )
		return status;
  
readResponse:
	/* Read response */
	//mset((UINT32*)&resp,0,sizeof(STPV3_RESPONSE));
	status = STPV3_ReadResponse(lpReader->lpDevice, &req, &resp, timeout);
	if( status != SKYETEK_SUCCESS )
          return status;//goto failure; /* timeout or error */

	if( resp.code == STPV3_RESP_SELECT_TAG_FAIL || resp.code == STPV3_RESP_SELECT_TAG_INVENTORY_DONE )
		goto success;

	if( resp.code == STPV3_RESP_SELECT_TAG_PASS )
	{
//          (*tagTypes)[num] = (LPTAGTYPE_ARRAY)malloc(sizeof(TAGTYPE_ARRAY));
            if( resp.tagType != 0 )
              tagTypes[(*count)].type = (SKYETEK_TAGTYPE)resp.tagType;
            else
              tagTypes[(*count)].type = (SKYETEK_TAGTYPE)req.tagType;
//		(*lpData)[num] = SkyeTek_AllocateData(resp.dataLength);

            SkyeTek_CopyBuffer(&lpData[(*count)],resp.data,resp.dataLength);
	    (*count)++;//num++;

		/* Keep reading */
		goto readResponse;
	}
success:
  /* Unknown code? */
//  ////SkyeTek_Debug(_T("Unknown response code: 0x%X\r\n"), resp.code);
//  *count = num;
  return SKYETEK_SUCCESS;
}

SKYETEK_STATUS 
STPV3_ReadTagData(    
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    PROTOCOL_FLAGS       flags,
    LPSKYETEK_ADDRESS    lpAddr,
    LPSKYETEK_DATA       *lpData,
    unsigned int         timeout
    )
{
  unsigned int f = 0;
  if( lpTag->id != NULL && lpTag->id->id != NULL && lpTag->id->length > 0 )
    f |= STPV3_TID;
  if( flags.isEncrypt )
    f |= STPV3_ENCRYPTION;
  if( flags.isHMAC )
    f |= STPV3_HMAC;
  f |= (lpTag->rf > 0 ? STPV3_RF : 0);
  f |= (lpTag->session > 0 ? STPV3_SESSION : 0);
  return STPV3_SendAddrGetTagCommand(lpReader,lpTag,lpAddr,STPV3_CMD_READ_TAG,f,lpData,timeout);
}

SKYETEK_STATUS 
STPV3_WriteTagData(
    LPSKYETEK_READER     lpReader, 
    LPSKYETEK_TAG        lpTag,
    PROTOCOL_FLAGS       flags,
    LPSKYETEK_ADDRESS    lpAddr,
    LPSKYETEK_DATA       lpData,
    unsigned int         timeout
    )
{
  unsigned int f = 0;
  if( flags.isEncrypt )
    f |= STPV3_ENCRYPTION;
  if( flags.isHMAC )
    f |= STPV3_HMAC;
  if( flags.isLock )
    f |= STPV3_LOCK;
  if( lpTag->id != NULL && lpTag->id->id != NULL && lpTag->id->length > 0 )
    f |= STPV3_TID;
  f |= (lpTag->rf > 0 ? STPV3_RF : 0);
  f |= (lpTag->session > 0 ? STPV3_SESSION : 0);
  return STPV3_SendAddrSetTagCommand(lpReader,lpTag,lpAddr,STPV3_CMD_WRITE_TAG,f,lpData,timeout);
}

PROTOCOLIMPL STPV3Impl = {
  3,
//  STPV3_SelectTags,
  STPV3_GetTags,
//  STPV3_GetTagsWithMask,
//  STPV3_StoreKey,
//  STPV3_LoadKey,
//  STPV3_LoadDefaults,
//  STPV3_ResetDevice,
//  STPV3_Bootload,
//  STPV3_GetSystemParameter,
//  STPV3_SetSystemParameter,
//  STPV3_GetDefaultSystemParameter,
//  STPV3_SetDefaultSystemParameter,
//  STPV3_AuthenticateReader,
//  STPV3_EnableDebug,
//  STPV3_DisableDebug,
//  STPV3_UploadFirmware,
//  STPV3_EnterPaymentScanMode,
//  STPV3_ScanPayments,
//  STPV3_SelectTag,
 // STPV3_ReadTagData,    
 // STPV3_WriteTagData,
//  STPV3_ReadTagConfig,    
//  STPV3_WriteTagConfig,
//  STPV3_ActivateTagType,
//  STPV3_DeactivateTagType,
//  STPV3_SetTagBitRate,
//  STPV3_GetTagInfo,
//  STPV3_GetLockStatus,
//  STPV3_KillTag,
//  STPV3_ReviveTag,
//  STPV3_EraseTag,
//  STPV3_FormatTag,
//  STPV3_DeselectTag,
//  STPV3_AuthenticateTag,
//  STPV3_SendTagPassword,
//  STPV3_GetApplicationIDs,
//  STPV3_SelectApplication,
//  STPV3_CreateApplication,
//  STPV3_DeleteApplication,
//  STPV3_GetFileIDs,
//  STPV3_SelectFile,
//  STPV3_CreateFile,
//  STPV3_GetFileSettings,
//  STPV3_ChangeFileSettings,
//  STPV3_ReadFile,
//  STPV3_WriteFile,
//  STPV3_DeleteFile,
//  STPV3_ClearFile,
//  STPV3_CreditValueFile,
//  STPV3_DebitValueFile,
//  STPV3_LimitedCreditValueFile,
//  STPV3_GetValue,
//  STPV3_ReadRecords,
//  STPV3_WriteRecord,
//  STPV3_CommitTransaction,
//  STPV3_AbortTransaction,
//  STPV3_EnableEAS,
//  STPV3_DisableEAS,
//  STPV3_ScanEAS,
//  STPV3_ReadAFI,
//  STPV3_WriteAFI,
//  STPV3_ReadDSFID,
//  STPV3_WriteDSFID,
//  STPV3_GetKeyVersion,
//  STPV3_ChangeKey,
//  STPV3_GetKeySettings,
//  STPV3_ChangeKeySettings,
//  STPV3_InitializeSecureMemoryTag,
//  STPV3_SetupSecureMemoryTag,
//  STPV3_InterfaceSend,
//  STPV3_TransportSend,
//  STPV3_InitiatePayment,
//  STPV3_ComputePayment,
//  STPV3_GetDebugMessages
};

SKYETEK_STATUS STPV3_GetStatus(unsigned int code)
{
	int ix;
	for( ix = 0; ix < STPV3_STATUS_LOOKUPS_COUNT; ix++ )
	{
		if( stpv3Statuses[ix].cmd == code )
			return stpv3Statuses[ix].status;
	}
	return SKYETEK_FAILURE;
}

unsigned char STPV3_IsErrorResponse(unsigned int code)
{
	return( ((code & 0x0000F000) == 0x00008000) || ((code & 0x0000F000) == 0x00009000));
}

unsigned char STPV3_IsAddressOrDataCommand( unsigned int cmd )
{
  /*
  #define STPV3_FORMAT_NOTHING  0x00
  #define STPV3_FORMAT_ADDRESS  0x01,
  #define STPV3_FORMAT_BLOCKS   0x02,
  #define STPV3_FORMAT_DATA     0x04,
  */
  switch(cmd)
    {
		/* address and blocks - no data */
    case STPV3_CMD_READ_TAG:
    case STPV3_CMD_READ_TAG_CONFIG:
    case STPV3_CMD_READ_SYSTEM_PARAMETER:
    case STPV3_CMD_RETRIEVE_DEFAULT_SYSTEM_PARAMETER:
			return (STPV3_FORMAT_ADDRESS | STPV3_FORMAT_BLOCKS);
			break;
		/* data only - no address */
    case STPV3_CMD_SET_TAG_BIT_RATE:
    case STPV3_CMD_KILL_TAG:
    case STPV3_CMD_AUTHENTICATE_READER:
    case STPV3_CMD_REVIVE_TAG:
    case STPV3_CMD_WRITE_AFI:
    case STPV3_CMD_WRITE_DSFID:
		case STPV3_CMD_CREDIT_VALUE_FILE:
		case STPV3_CMD_DEBIT_VALUE_FILE:
    case STPV3_CMD_SELECT_APPLICATION:
    case STPV3_CMD_SEND_TAG_PASSWORD:
    case STPV3_CMD_INIT_SECURE_MEMORY:
    case STPV3_CMD_SETUP_SECURE_MEMORY:
    case STPV3_CMD_CREATE_APPLICATION:
    case STPV3_CMD_DELETE_APPLICATION:
    case STPV3_CMD_GET_FILE_IDS:
    case STPV3_CMD_SELECT_FILE:
    case STPV3_CMD_CREATE_FILE:
    case STPV3_CMD_GET_FILE_SETTINGS:
    case STPV3_CMD_CHANGE_FILE_SETTINGS:
    case STPV3_CMD_READ_FILE:
    case STPV3_CMD_WRITE_FILE:
    case STPV3_CMD_DELETE_FILE:
    case STPV3_CMD_CLEAR_FILE:
    case STPV3_CMD_LIMITED_CREDIT_VALUE_FILE:
    case STPV3_CMD_GET_VALUE:
    case STPV3_CMD_READ_RECORDS:
    case STPV3_CMD_WRITE_RECORD:
    case STPV3_CMD_GET_KEY_VERSION:
    case STPV3_CMD_CHANGE_KEY:
    case STPV3_CMD_CHANGE_KEY_SETTINGS:
    case STPV3_CMD_GET_KEY_SETTINGS:
    case STPV3_CMD_INTERFACE_SEND:
    case STPV3_CMD_TRANSPORT_SEND:
    case STPV3_CMD_INITIATE_PAYMENT:
    case STPV3_CMD_COMPUTE_PAYMENT:
			return (STPV3_FORMAT_DATA);
			break;
		/* address, blocks and data */
    case STPV3_CMD_WRITE_TAG:
    case STPV3_CMD_WRITE_TAG_CONFIG:
    case STPV3_CMD_WRITE_SYSTEM_PARAMETER:
    case STPV3_CMD_STORE_DEFAULT_SYSTEM_PARAMETER:
			return (STPV3_FORMAT_ADDRESS | STPV3_FORMAT_BLOCKS | STPV3_FORMAT_DATA);
			break;
		/* address but no numblocks or data */
    case STPV3_CMD_ERASE_TAG:
    case STPV3_CMD_GET_LOCK_STATUS:
    case STPV3_CMD_LOAD_KEY:
			return (STPV3_FORMAT_ADDRESS);
			break;
    /* adress with no numblocks but with data */
    case STPV3_CMD_AUTHENTICATE_TAG:
    case STPV3_CMD_STORE_KEY:
      return (STPV3_FORMAT_ADDRESS | STPV3_FORMAT_DATA);
      break;
    default:
      return STPV3_FORMAT_NOTHING;
    }
	//return STPV3_FORMAT_NOTHING;
}

// private function block
void mset(UINT32 *ptr, UINT8 set, UINT32 size)
{
  UINT8* p = (UINT8*)ptr;
  while(size>0)
  {
    *(p++) = set;
  }
}

UINT16 crcBL16(UINT8 *dataP, UINT16 nBytes, UINT16 preset)
{
    UINT16 i, j;
	UINT8 mBits = 8;

  	UINT16 crc_16 = preset;

	for( i=0; i<nBytes; i++ )
	{
	 	crc_16 ^= *dataP++;
		
		for( j=0; j<mBits; j++ )
		{
			if( crc_16 & 0x0001 )
			{
				crc_16 >>= 1;
				crc_16 ^= 0x8408;  // Polynomial (x^16 + x^12 + x^5 + 1)
			}
		    else
			{
				crc_16 >>= 1;
			}
		}
	}

	return( crc_16 );
}

UINT16 verifyBLcrc(UINT8 *resp, UINT16 len)
{
  UINT16 crc_check;
  
	crc_check = crcBL16(resp, len-2, 0x0000);
	if(resp[len-2] == (crc_check >> 8) &&
     resp[len-1] == (crc_check & 0x00FF))
		return 1;
	else
		return 0;
}

unsigned char getASCIIFromHex(unsigned int c, unsigned char pos)
{
	unsigned char byte;
	if( pos == 0)
		byte = (c & 0x000F);
	else if( pos == 1 )
		byte = (c & 0x00F0) >> 4;
	else if( pos == 2 )
		byte = (c & 0x0F00) >> 8;
	else if( pos == 3 )
		byte = (c & 0xF000) >> 12;
//	if( byte >= 0 && byte <= 9 )
//		byte += 48;
        if( byte <= 9 )
		byte += 48;
	else if( byte >= 10 && byte <= 15 )
		byte += 55;
	else 
		byte = 48;
	return byte;
}

int getHexFromASCIIBit(unsigned char c)
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

int getHexFromASCII(unsigned char *ascii, int len)
{
	int ret = 0;
	if( len == 4 )
		ret = (getHexFromASCIIBit(ascii[0]) << 12) | (getHexFromASCIIBit(ascii[1]) << 8) | (getHexFromASCIIBit(ascii[2]) << 4) | getHexFromASCIIBit(ascii[3]);
	else if( len == 3 )
		ret = (getHexFromASCIIBit(ascii[0]) << 8) | (getHexFromASCIIBit(ascii[1]) << 4) | getHexFromASCIIBit(ascii[2]);
	else if( len == 2 )
		ret = (getHexFromASCIIBit(ascii[0]) << 4) | getHexFromASCIIBit(ascii[1]);
	else if( len == 1 )
		ret = getHexFromASCIIBit(ascii[0]);
	return ret;
}

SKYETEK_STATUS 
SkyeTek_CopyBuffer(
    LPSKYETEK_DATA     lpData,
    unsigned char      *buffer,
    unsigned int       size
    )
{
  unsigned int ix;
  //unsigned int max = 0;
  
  if( buffer == NULL || lpData == NULL || lpData->data == NULL )
    return SKYETEK_INVALID_PARAMETER;
  //max = (size > lpData->size ? lpData->size : size);
  for( ix = 0; ix < size; ix++ )
    lpData->data[ix] = buffer[ix];
  lpData->size = size;
  return SKYETEK_SUCCESS;
}

