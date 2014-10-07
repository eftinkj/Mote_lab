/****************************************************************************
**
** Project: Read data from RFID reader (SKYETEC)
** 
** Author:  Wang, Lei
** eMail:   lw3r6@mst.edu
** Version: 1.0
**
****************************************************************************/

#include "PRJ_RFID_SKYETEC/SkyeTekAPI/SkyeTekAPI.h"

#if defined(_ENABLE_RFID_SKYETEK_)

#include "common_constants.h"
#include "sys_func.h"
#include "FEAT_Networking/tsp_common_pub.h"

#include "hal/micro/cortexm3/uart.h"
#include "rfid_skyetek.h"
#include "FEAT_Scheduler/sch_basic_pub.h"

#include <string.h>

// Global Table
RFID_TAGS rfid_tags;

// needed for reader
SKYETEK_ID ReaderID; // Empty left for reading from Reader
SKYETEK_PROTOCOL STPV3 = { 3,
                            .internal = &STPV3Impl
                         };
SKYETEK_READER Reader = { .id = &ReaderID, 
                          .lpProtocol = &STPV3,
                          .internal = &SkyetekReaderImpl
                            // "lpDevice" is serial port
                        };
SKYETEK_TAG tagTAGs[MOTE_RFID_NUM_MAX_TAGS];

char buff[80]; // for package

uint8_t read_event_tid; /// Stores ID of the timeout for reading the tags
uint8_t report_event_tid;/// Stores ID of the timeout for reporting the inventory

uint8_t rfid_pkt_id = 0xFF;

unsigned short usNumTotalTags = 0; /// Tag count from last read

// Declare of local functions
void rfid_clear_buffers();
void rfid_clear_pkg_buff();

void rfid_skyetek_power_up()
{
	read_event_tid = SCH_NO_TIMEOUT_ID;
	report_event_tid = SCH_NO_TIMEOUT_ID;
	usNumTotalTags = 0;
	
	rfid_clear_buffers();
}


void rfid_skyetek_init()
{
  
//  uartInit( 38400, 8, PARITY_NONE, 1 );//38400//9600//19200//57600
  
  read_event_tid = sch_create_timeout( rtc_get_ticks()+ RFID_READ_TIMEOUT_MS, rfid_skyetek_GetTags, NULL);
  report_event_tid = sch_create_timeout( rtc_get_ticks()+ RFID_REPORT_TIMEOUT_MS, rfid_send_inventory, NULL);
}

void rfid_skyetek_GetTags(uint8_t * context)
{

	unsigned short usNumTags = 0;
	SKYETEK_STATUS ret;
	unsigned char bExisted;
  
   ret = SkyeTek_GetTags(&Reader, AUTO_DETECT, &tagTAGs[0], &usNumTags);
	  
   if (ret == SKYETEK_SUCCESS && usNumTotalTags < 10);
   {
		// CHECK IF THE TAG ALREADY IN LIST
		// IF NOT ADD AT THE END
		for(int i=0; i<usNumTags; i++)
		{
			bExisted = 0;
			// See whether existed already
			for(int j = 0; j<10; j++)
			{
				if( 0 == strcmp((char const*)&(rfid_tags.IDs[i].id[0]), (char const*)&(tagTAGs[i].friendly[0])) 
				   && rfid_tags.IDs[i].id[0] != 0x00)
				{
					bExisted = 1;
				}
			}
			if(bExisted == 0)
			{
				//for(int j =0; j<24; j++)
				//{
				//rfid_tags.IDs[i].id[j] = tagTAGs[i].friendly[j];        
				//}
		  		strcpy((char*)&(rfid_tags.IDs[usNumTotalTags++].id[0]), (char*)&(tagTAGs[i].friendly[0]));
			}
		}
	
		rfid_tags.number = usNumTotalTags;
  }
  
    read_event_tid = sch_create_timeout( rtc_get_ticks()+RFID_READ_TIMEOUT_MS, rfid_skyetek_GetTags, NULL);
}


void rfid_send_inventory(uint8_t *context)
{
	uint8_t i;
	uint8_t reminder = rfid_tags.number % 3;
	
	uint8_t result = rfid_create_report_num(rfid_tags.number);
	
	result = rfid_create_report(reminder, 0);
	for( i = reminder; i< rfid_tags.number; i+=3)
	{
		result = rfid_create_report(3, i);
	}
	
	// clear counter for next counting
	usNumTotalTags = 0;
	rfid_clear_buffers();
	report_event_tid = sch_create_timeout( rtc_get_ticks()+RFID_REPORT_TIMEOUT_MS, rfid_send_inventory, NULL);
}

/**
* uint8_t rfid_create_report () - creates and fills out the report packet
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t rfid_create_report (uint8_t count, uint8_t start_index)
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
	uint8_t module_size = 0;
	uint8_t *mod = NULL;
	
	rfid_clear_pkg_buff();
	
	if (count >=3) {
		sprintf(buff, "\\%24s\\%s\\%s\\", rfid_tags.IDs[start_index].id
				, rfid_tags.IDs[start_index+1].id, rfid_tags.IDs[start_index+2].id);  
	} else if (count == 2) {
		sprintf(buff, "\\%s\\%s\\", rfid_tags.IDs[start_index].id, rfid_tags.IDs[start_index+1].id);  
	} else if (count == 1) {
		sprintf(buff, "%s\\", rfid_tags.IDs[start_index].id);  
	} else
	  return 0;

	module_size = strlen(buff);
	mod = tsp_new_module( PKT_MODULE_TYPE_HSET_SAMPLES_V1, 
                          module_size, 
                          &rfid_pkt_id, 
                          HSET_DST);

	if (0 == mod)
	{
		// Error
		rfid_pkt_id = 0xFF;
		sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	else
	{
		memcpy(mod,buff,module_size);
		tsp_send_from_modules(rfid_pkt_id);
	}
	return 1;
}

uint8_t rfid_create_report_num (unsigned short n)
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
	uint8_t module_size = 0;
	uint8_t *mod = NULL;
	
	rfid_clear_pkg_buff();
	
	sprintf(buff, "<<%ho>", n);  

	module_size = strlen(buff);
	mod = tsp_new_module( PKT_MODULE_TYPE_HSET_SAMPLES_V1, 
                          module_size, 
                          &rfid_pkt_id, 
                          HSET_DST);

	if (0 == mod)
	{
		// Error
		rfid_pkt_id = 0xFF;
		sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	else
	{
		memcpy(mod,buff,module_size);
		tsp_send_from_modules(rfid_pkt_id);
	}
	return 1;
}

void rfid_clear_buffers()
{
	for(int i=0; i<10; i++)
    {
		memset(&(rfid_tags.IDs[i].id[0]), 0, (25*sizeof(unsigned char)));
		rfid_tags.IDs[i].id[24] = '\0';        
	}
	for(int i=0; i<MOTE_RFID_NUM_MAX_TAGS; i++)
    {
		memset(&(tagTAGs[i].friendly[0]), 0, (32*sizeof(unsigned char)));
		tagTAGs[i].friendly[24] = '\0';        
	}
}

void rfid_clear_pkg_buff()
{
	for(int i=0; i<80; i++)
    {
		memset(&(buff[0]), 0, (80*sizeof(char)));        
	}
}
#endif // defined(_ENABLE_RFID_SKYETEK_)