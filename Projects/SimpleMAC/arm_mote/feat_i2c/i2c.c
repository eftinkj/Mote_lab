/****************************************************************************
**
** Project: I2C FOR HUMIDITY/TEMP SENSOR
**
****************************************************************************/

// Have to be the first include (before ant other statement or #ifdef
#include "sht_basic.h"

#if defined(_ENABLE_I2C_)

#include "common_constants.h"
//#include "sys_func.h"

#include "FEAT_Scheduler/sch_basic_pub.h"

#define SHT_TIMEOUT_MS 1000


uint8_t sht_event_tid; /// Stores ID of the timeout for reading the tags
//uint8_t report_event_tid;/// Stores ID of the timeout for reporting the inventory

uint16_t response_humi; // last measurment
uint16_t response_temp; // last temp measurment
uint16_t sht_status_reg; // Last read Status Reg value

/**
 * void sht_power_up() - internal initialization
 */
void sht_power_up()
{
	sht_event_tid = SCH_NO_TIMEOUT_ID;
	//report_event_tid = SCH_NO_TIMEOUT_ID;

	SC2_RATELIN=14;
	SC2_RATEEXP=3;//selecting the clock rate as 100 kpbs
	
	//GPIO for SC2 - I2C mode
	//GPIO_PACFGL = ......;	
	GPIO_PACFGL = GPIO_PACFGL & 0xFFFFF00F;
	GPIO_PACFGL = GPIO_PACFGL | 0x00000DD0;
	response_humi = 0;
	sht_status_reg = 255;
	
	SC2_MODE = SC2_MODE_I2C; //3;//selecting TWI conttroller
}


/**
 * void sht_init() - external initialization (schedule tasks/events)
 */
void sht_init()
{
	sht_event_tid = sch_create_timeout( rtc_get_ticks()+ SHT_TIMEOUT_MS, sht_read_status, NULL);
	sht_event_tid = sch_create_timeout( rtc_get_ticks()+ SHT_TIMEOUT_MS+5, sht_read_status, NULL);
	sht_event_tid = sch_create_timeout( rtc_get_ticks()+ SHT_TIMEOUT_MS+10, sht_write_status, NULL);

	sht_event_tid = sch_create_timeout( rtc_get_ticks()+ 2*SHT_TIMEOUT_MS, sht_req_temp, NULL);
//	sht_event_tid = sch_create_timeout( rtc_get_ticks()+ 2*SHT_TIMEOUT_MS, sht_req_humi, NULL);
}

/**
 * void sht_wakeup() - SHT sensor specific START/Wakeup sequence
 *			NOTE: Use always befor new request/cmd tranmsission
 *			but after TWI Start initialization
 */
void sht_wakeup()
{
	GPIO_PACFGL = GPIO_PACFGL & 0xFFFFf77f; // Output
	GPIO_PASET = 0x00000004;
	GPIO_PACLR = 0x00000002;// 1-0
	halCommonDelayMicroseconds(1);
	GPIO_PACLR = 0x00000004;// 0-0
	halCommonDelayMicroseconds(1);
	GPIO_PASET = 0x00000004;// 1-0
	halCommonDelayMicroseconds(1);
	GPIO_PASET = 0x00000002;// 1-1
	
	GPIO_PACFGL = GPIO_PACFGL | 0x00000880; // Alt Output (I2C)
}

/**
 * viod sht_write_status(uint8_t *context) - send request for humidity measurments
 */
void sht_write_status(uint8_t *context)
{
	SC2_TWICTRL1 = SC_TWISTOP;
	while (0 != (SC_TWISTOP & SC2_TWICTRL1));
	SC2_TWICTRL1 = SC_TWISTART;
	while (0 != (SC_TWISTART & SC2_TWICTRL1));

	sht_wakeup();

	SC2_DATA = 0x06; // request Status Byte WRITE/CHANGE
	SC2_TWICTRL1 = SC_TWISEND;//issue transmission start to sensor
	while (0 != (SC_TWISEND & SC2_TWICTRL1));

	SC2_DATA = 0x00; //
	SC2_TWICTRL1 = SC_TWISEND;//issue transmission start to sensor
	while (0 != (SC_TWISEND & SC2_TWICTRL1));

//	SC2_TWICTRL1 = SC_TWISTOP;	// STOP transmission (ACK-Data set to high
	
	// TODO: pass the measurment to SSN module for transmission
	
	
	// Schedule next read operation:
//    sht_event_tid = sch_create_timeout( rtc_get_ticks()+SHT_TIMEOUT_MS-100, request_measument_humi, NULL);
    sht_event_tid = sch_create_timeout( rtc_get_ticks()+1, sht_read_status, NULL);
}

/**
 * viod sht_read_status(uint8_t *context) - send request for Status Byte Read
 */
void sht_read_status(uint8_t *context)
{
	SC2_TWICTRL1 = SC_TWISTOP;
	while (0 != (SC_TWISTOP & SC2_TWICTRL1));
	SC2_TWICTRL1 = SC_TWISTART;
	while (0 != (SC_TWISTART & SC2_TWICTRL1));

	sht_wakeup();

	SC2_DATA = 0x07; // request Humidity measurment - bits: 00000101
	SC2_TWICTRL1 = SC_TWISEND;//issue transmission start to sensor
	while (0 != (SC_TWISEND & SC2_TWICTRL1));

	SC2_TWICTRL1 = SC_TWIRECV; // request read of a byte	
	while (0 != (SC2_TWICTRL1 & SC_TWIRECV)); // wait until byte read
	sht_status_reg = SC2_DATA; //read 1st byte;
	sht_status_reg = sht_status_reg<<8; // This is high byte
	SC2_TWICTRL1 = SC_TWIRECV; // request read of a byte	
	while (0 != (SC2_TWICTRL1 & SC_TWIRECV)); // wait until byte read
	sht_status_reg += SC2_DATA; //read 2nd byte and add to the high byte;
	SC2_TWICTRL1 = SC_TWISTOP;	// STOP transmission (ACK-Data set to high
	
	// TODO: pass the measurment to SSN module for transmission
	
	
	// Schedule next read operation:
//    sht_event_tid = sch_create_timeout( rtc_get_ticks()+SHT_TIMEOUT_MS-100, request_measument_humi, NULL);
//    sht_event_tid = sch_create_timeout( rtc_get_ticks()+1, sht_write_status, NULL);
}

/**
 * viod sht_req_humi(uint8_t *context) - send request for humidity measurments
 */
void sht_req_humi(uint8_t *context)
{
	SC2_TWICTRL1 = SC_TWISTOP;
	while (0 != (SC_TWISTOP & SC2_TWICTRL1));
	SC2_TWICTRL1 = SC_TWISTART;
	while (0 != (SC_TWISTART & SC2_TWICTRL1));

	sht_wakeup();

	SC2_DATA = 0x05; // request Humidity measurment - bits: 00000101
	SC2_TWICTRL1 = SC_TWISEND;//issue transmission start to sensor
	while (0 != (SC_TWISEND & SC2_TWICTRL1));

	SC2_TWICTRL1 = SC_TWISTOP;
	while (0 != (SC_TWISTOP & SC2_TWICTRL1));

	sht_event_tid = sch_create_timeout( rtc_get_ticks()+ 21, sht_read_humi, NULL);
}



/**
 * void sht_read_humi (uint8_t *context) - read the humidity measurment
 */
void sht_read_humi (uint8_t *context)
{
	uint8_t temp;
	SC2_TWICTRL1 = SC_TWIRECV; // request read of a byte	
	while (0 != (SC2_TWICTRL1 & SC_TWIRECV)); // wait until byte read
	response_humi = SC2_DATA; //read 1st byte;
	response_humi = response_humi<<8; // This is high byte
	SC2_TWICTRL1 = SC_TWIRECV; // request read of a byte	
	while (0 != (SC2_TWICTRL1 & SC_TWIRECV)); // wait until byte read
	response_humi += SC2_DATA; //read 2nd byte and add to the high byte;

	SC2_TWICTRL1 = SC_TWIRECV; // request read of a byte	
	while (0 != (SC2_TWICTRL1 & SC_TWIRECV)); // wait until byte read
	temp = SC2_DATA; //read 3nd byte (checksum)

	SC2_TWICTRL1 = SC_TWISTOP;	// STOP transmission (ACK-Data set to high
	
	// TODO: pass the measurment to SSN module for transmission
	
	
	// Schedule next read operation:
//    sht_event_tid = sch_create_timeout( rtc_get_ticks()+SHT_TIMEOUT_MS-100, request_measument_humi, NULL);
    sht_event_tid = sch_create_timeout( rtc_get_ticks()+1, sht_req_humi, NULL);
}

/**
 * viod sht_req_temp(uint8_t *context) - send request for temperature measurments
 */
void sht_req_temp(uint8_t *context)
{
	SC2_TWICTRL1 = SC_TWISTOP;
	while (0 != (SC_TWISTOP & SC2_TWICTRL1));
	SC2_TWICTRL1 = SC_TWISTART;
	while (0 != (SC_TWISTART & SC2_TWICTRL1));

	sht_wakeup();

	SC2_DATA = 0x03; // request temperature measurment - bits: 00000101
	SC2_TWICTRL1 = SC_TWISEND;//issue transmission start to sensor
	while (0 != (SC_TWISEND & SC2_TWICTRL1));

	SC2_TWICTRL1 = SC_TWISTOP;
	while (0 != (SC_TWISTOP & SC2_TWICTRL1));

	sht_event_tid = sch_create_timeout( rtc_get_ticks()+ 240, sht_read_temp, NULL);
}

/*
VDD d1 (°C) d1 (°F)
5V -40.1 -40.2
4V -39.8 -39.6
3.5V -39.7 -39.5
3V -39.6 -39.3
2.5V -39.4 -38.9

SOT d2 (°C) d2 (°F)
14bit 0.01 0.018
12bit 0.04 0.072
*/
//#define d1	-39.3
#define d1	-200.3
//#define d2	0.072
#define d2	0.018
float temp_F = 0.0;
sint16_t temp_F_int = 0;

/**
 * void sht_read_temp (uint8_t *context) - read the temperature measurment
 */
void sht_read_temp (uint8_t *context)
{
	uint8_t temp;
	SC2_TWICTRL1 = SC_TWISTART;
	while (0 != (SC_TWISTART & SC2_TWICTRL1));

	SC2_TWICTRL1 = SC_TWIRECV; // request read of a byte	
	while (0 != (SC2_TWICTRL1 & SC_TWIRECV)); // wait until byte read
	response_temp = SC2_DATA; //read 1st byte;
	response_temp = response_temp << 8; // This is high byte
	SC2_TWICTRL1 = SC_TWIRECV; // request read of a byte	
	while (0 != (SC2_TWICTRL1 & SC_TWIRECV)); // wait until byte read
	response_temp += SC2_DATA; //read 2nd byte and add to the high byte;

	SC2_TWICTRL1 = SC_TWIRECV; // request read of a byte	
	while (0 != (SC2_TWICTRL1 & SC_TWIRECV)); // wait until byte read
	temp = SC2_DATA; //read 3nd byte (checksum)

	SC2_TWICTRL1 = SC_TWISTOP;	// STOP transmission (ACK-Data set to high
	
	// TODO: pass the measurment to SSN module for transmission
	temp_F = (d1 + d2 * response_temp);
	// 209 -> 70F ; 270-> (??)90F
	temp_F_int = (temp_F * 100);
	ssn_recv_sample(0,temp_F_int); // It will be divided by 100 in SSN
	ssn_recv_sample(0,temp_F_int);
	ssn_recv_sample(0,temp_F_int);

	ssn_recv_sample(0,temp_F_int);
	ssn_recv_sample(0,temp_F_int);

	// Schedule next read operation:
//    sht_event_tid = sch_create_timeout( rtc_get_ticks()+SHT_TIMEOUT_MS-100, request_measument_humi, NULL);
    sht_event_tid = sch_create_timeout( rtc_get_ticks()+1, sht_req_temp, NULL);
}



#endif