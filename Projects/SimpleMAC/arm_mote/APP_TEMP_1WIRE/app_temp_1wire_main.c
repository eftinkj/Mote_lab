

#include "HW_LIB/OneWire.h"
#include "app_temp_1wire_main.h"
#include "FEAT_Scheduler/sch_basic_pub.h"

extern void app_temp_lcd_disp(uint8_t * context);

OWire OneWire;
unsigned char addr[8] = {0x28, 0xEC, 0x21, 0xEA, 0x03, 0x00, 0x00, 0xC9};
unsigned int DSState = 0;

unsigned char data[2];
float Ta = 0.0;

#define TEMP_LCD_INTERVAL 1000

uint8_t lcd_timeout = SCH_NO_TIMEOUT_ID;
uint8_t temp_sensor_timeout = SCH_NO_TIMEOUT_ID;
int32_t temp_cC = 0; // Temperature in centy-Celcius (0.01 C per unit)

char temp_task_name[]="TEMP";
char lcd_task_name[]="LCD";

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
  OWInit(&OneWire, GPIOC, GPIO_Pin_7);
  
  OWSearch(&OneWire, addr);
  /*sprintf(test, "%02X %02X %02X %02X %02X %02X %02X %02X",
  addr[7],addr[6],
  addr[5],addr[4],
  addr[3],addr[2],
  addr[1],addr[0]);
  ST7565_drawstring(6, 6, test);
  */
  
  //sch_add_loop(app_temp_1wire_loop); 
  lcd_timeout = sch_create_timeout( rtc_get_ticks()+TEMP_LCD_INTERVAL, app_temp_lcd_disp, (uint8_t*) 0, lcd_task_name);
  temp_sensor_timeout = sch_create_timeout( rtc_get_ticks()+TEMP_LCD_INTERVAL*2, app_temp_1wire_loop, (uint8_t*) 0, temp_task_name);

}



/*******************************************************************************
* Function Name  : app_temp_1wire_loop
* Description    : Reads the temperature in two stages (conversion + read)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void app_temp_1wire_loop(uint8_t *context) //__align(8)
{
  
  //onewire
  switch (DSState){
  case 0:
	OWReset(&OneWire);
	OWWrite(&OneWire, 0xCC); // skip ROM
	OWWrite(&OneWire, 0x44); // start conversion
	//DSTimer = millis();
	DSState++;
	break;
  case 1:
	//if((millis() - DSTimer) >= 1000){
	  OWReset(&OneWire);
	  //OWSelect(&OneWire, addr);
		OWWrite(&OneWire, 0xCC); // skip ROM
	  OWWrite(&OneWire, 0xBE); // Read Scratchpad
	  data[0] = OWRead(&OneWire);
	  data[1] = OWRead(&OneWire);
	  temp_cC = (((data[1] << 8) | data[0] )*100) / 16;
	//  int32_t total_100=((((data[1] << 8) | data[0] )*10*9) / 8) ;//+ 3200;
	//	int32_t i=(total_100/100)+32;
	//	int32_t f=total_100%100;
		//i+=32;
	  DSState = 0;
	//}
	break;
  }
    temp_sensor_timeout = sch_create_timeout( rtc_get_ticks()+TEMP_LCD_INTERVAL*2, app_temp_1wire_loop, (uint8_t*) 0, temp_task_name);
	if ( SCH_NO_TIMEOUT_ID == lcd_timeout ) printf( "TEMP timer out %l\n", rtc_get_ticks() );
}



int lcd_count = 0;
void app_temp_lcd_disp(uint8_t * context)
{	
	char buf[15];
	int32_t total_100 = (temp_cC * 9)/5 ;//+ 3200;
	int32_t i=(total_100/100)+32;
	int32_t f=total_100%100;
		//i+=32;
	sprintf(buf, "%d.%d", i, f);
	//sfprintf(buf, "%f", __align(8) Ta);
  	lcd_setCursor ( 0 , 0 ) ;
	switch (lcd_count++)
	{
		case 0: lcd_printStr ( "0F=" ) ; break;
		case 1: lcd_printStr ( "1F=" ) ; break;
		case 2: lcd_printStr ( "2F=" ) ; break;
		case 3: lcd_printStr ( "3F=" ) ; break;
		case 4: lcd_printStr ( "4F=" ) ; break;
		case 5: lcd_printStr ( "5F=" ) ; lcd_count=0; break;
	}
	lcd_printStr ( buf ) ;
	lcd_timeout = sch_create_timeout( rtc_get_ticks()+TEMP_LCD_INTERVAL, app_temp_lcd_disp, (uint8_t*) 0, lcd_task_name);
	if (SCH_NO_TIMEOUT_ID == temp_sensor_timeout) printf("LCD timer OUT %l\n", rtc_get_ticks());
}