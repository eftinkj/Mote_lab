#ifndef _RFID_SKYETEK_H_
#define _RFID_SKYETEK_H_

#include "defs/esnl_pub.h"


#define RFID_READ_TIMEOUT_MS 500
#define RFID_REPORT_TIMEOUT_MS 5750

typedef struct RFID_TAGS_ID
{
  unsigned char id[25];
} RFID_TAGS_ID;

typedef struct RFID_TAGS
{
  unsigned int number;
  RFID_TAGS_ID IDs[10];
} RFID_TAGS;

extern RFID_TAGS rfid_tags;

void rfid_skyetek_init();
void rfid_skyetek_GetTags(uint8_t * context);

void rfid_send_inventory(uint8_t *context);
/**
* uint8_t rfid_create_report () - creates and fills out the report packet
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t rfid_create_report (uint8_t count, uint8_t start_index);
uint8_t rfid_create_report_num (unsigned short n);

#endif //#ifndef _RFID_SKYETEK_H_