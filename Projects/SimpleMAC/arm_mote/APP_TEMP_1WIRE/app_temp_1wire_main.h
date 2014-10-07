#ifndef _APP_TEMP_1WIRE_MAIN_
#define _APP_TEMP_1WIRE_MAIN_

#include "defs/esnl_pub.h"


/*******************************************************************************
* Function Name  : app_temp_1wire_init.
* Description    : Initializes OneWire interface and setups periodic temperature
*					readings.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void app_temp_1wire_init();



/*******************************************************************************
* Function Name  : app_temp_1wire_loop
* Description    : Reads the temperature in two stages (conversion + read)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void app_temp_1wire_loop();

#endif
