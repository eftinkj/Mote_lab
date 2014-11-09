
#include "defs/esnl_pub.h"

#ifdef FEAT_BASE_STATION

#include "bs_base.h"


//******************
// FUNCTION: bs_power_up
//
// DESCRIPTION:
//  Setup local variables for Base Station
//
// OUTPUT:
//  void
//
// ARGUMENTS:
//  void
//
// CREATED: 9/4/2011, by mjzx9c
// FILE: bs_base.c
//
//void bs_power_up ( void ); // For Header File!
void bs_power_up ( void )
{
}

///////////////////////////////////////////////
/**
* FUNCTION:	 bs_init
* DESCRIPTION: Initialize the Base Station code - connect to OS
* OUTPUT: 	void
* ARGUMENTS:	void
* CREATED:	9/6/2011, by mjzx9c
*/
//void bs_init ( void ); // For Header File!
void bs_init ( void )
{
	
}

#define TEMP_FORMAT_F	1
#define TEMP_FORMAT_C	2
#define TEMP_FORMAT_cF	3
#define TEMP_FORMAT_cC	4

IAR_PACKED struct temp_mod_contet {
	uint16_t temp;
	unit8_t temp_format;
	uint8_t sensor_id;
	uint8_t content;
}  PACKED;
typedef struct temp_mod_contet temp_mod_content_t;

void bs_temp_callback(uint8_t scr_id, pkt_mod_header_t *module, uint8_t rssi)
{
	temp_mod_content_t *m = &(module->module_content);
//		uint8_t module_type;
//		uint8_t module_length;
//		uint8_t content;
	//temp_convert(m->temp_format, m->temp, TEMP_FORMAT_cF);
	printf("ID=%d
	m->temp;
}

#endif /*FEAT_BASE_STATION */
