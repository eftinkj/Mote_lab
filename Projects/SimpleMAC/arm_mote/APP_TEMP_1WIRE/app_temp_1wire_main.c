

#include "HW_LIB/OneWire.h"
#include "app_temp_1wire_main.h"
#include "FEAT_Scheduler/sch_basic_pub.h"


OWire OneWire;
unsigned char addr[8] = {0x28, 0xEC, 0x21, 0xEA, 0x03, 0x00, 0x00, 0xC9};
unsigned int DSState = 0;

unsigned char data[2];
float Ta = 0.0;


/*******************************************************************************
* Function Name  : app_temp_1wire_init.
* Description    : Initializes OneWire interface and setups periodic temperature
*					readings.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void app_temp_1wire_init()
{
  OWInit(&OneWire, GPIOA, GPIO_Pin_7);
  
  OWSearch(&OneWire, addr);
  /*sprintf(test, "%02X %02X %02X %02X %02X %02X %02X %02X",
  addr[7],addr[6],
  addr[5],addr[4],
  addr[3],addr[2],
  addr[1],addr[0]);
  ST7565_drawstring(6, 6, test);
  */
  
  sch_add_loop(app_temp_1wire_loop); 
}


/*******************************************************************************
* Function Name  : app_temp_1wire_loop
* Description    : Reads the temperature in two stages (conversion + read)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void app_temp_1wire_loop()
{
  
  //onewire
  switch (DSState){
  case 0:
	  	lcd_setCursor ( 0 , 0 ) ;
		lcd_printStr ( "|" ) ;
	OWReset(&OneWire);
	OWWrite(&OneWire, 0xCC); // skip ROM
	OWWrite(&OneWire, 0x44); // start conversion
	//DSTimer = millis();
	DSState++;
	break;
  case 1:
	//if((millis() - DSTimer) >= 1000){
	  	  	lcd_setCursor ( 0 , 0 ) ;
			lcd_printStr ( "F=" ) ;
	  OWReset(&OneWire);
	  OWSelect(&OneWire, addr);
	  OWWrite(&OneWire, 0xBE); // Read Scratchpad
	  data[0] = OWRead(&OneWire);
	  data[1] = OWRead(&OneWire);
	  Ta = ((data[1] << 8) | data[0]) / 16.0;
		char buf[15];
	  sprintf(buf, "%f", Ta);
	  lcd_printStr ( buf ) ;
	  DSState = 0;
	//}
	break;
  }
}
