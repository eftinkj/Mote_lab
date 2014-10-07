#ifndef _MON_POWER_BASIC_PRV_H_
#define _MON_POWER_BASIC_PRV_H_

/****************************************************************************
**	Includes (PRIVATE)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "rtc.h"
#include "spi_pub.h"

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/


#define	ADE_TEST_VER
//#undef	ADE_TEST_VER

//#define ADE_TEST_READ_WRITE
#undef	ADE_TEST_READ_WRITE


//#define MON_POWER_DEFAULT_DELAY	250
//#define MON_POWER_CMD_TIMEOUT	250

// Read power (Watt-Hour) every 250ms
#define MON_POWER_READ_POWER_DEFAULT_TIMEOUT	250

// Default delay between read request and reading of the ADE register
#define MIN_DELAY_ADE_REG_READ	5



#define	MON_POWER_REPORT_V1_SIZE	30
#define	MON_POWER_REPORT_V2_SIZE	65

// How many points of temperature and acceleration should be averaged
#define MON_POWER_AVERAGE_COUNT		50

// ADC settings
#define MON_POWER_ADC_NO_CHANNELS	4
#define MON_POWER_ADC_AVERAGER	10
#define MON_POWER_ADC_DIVIDER	10

#define MON_POWER_ADC_G_X	0
#define MON_POWER_ADC_G_Y	1
#define MON_POWER_ADC_G_Z	2
#define MON_POWER_ADC_TEMP	3

// ADE Registers
#define ADE_ILLEGAL_REG_32	0xFFFFFFFF

#define MON_POWER_ADE_WATT_HOUR_A	0x01
#define MON_POWER_ADE_WATT_HOUR_B	0x02
#define MON_POWER_ADE_WATT_HOUR_C	0x03

#define MON_POWER_ADE_VAR_HOUR_A	0x04
#define MON_POWER_ADE_VAR_HOUR_B	0x05
#define MON_POWER_ADE_VAR_HOUR_C	0x06

#define MON_POWER_ADE_VA_HOUR_A		0x07
#define MON_POWER_ADE_VA_HOUR_B		0x08
#define MON_POWER_ADE_VA_HOUR_C		0x09

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/

//extern uint8_t		truck_state_test;
extern rtc_tick_t	mon_power_timeout;
extern rtc_tick_t	mon_power_read_power_timeout;
//extern rtc_tick_t	truck_command_timeout;
extern uint8_t		mon_power_read_power_enabled;

extern uint16_t mon_power_sampling_interval;

extern spi_event_t mon_power_event_flag;
extern spi_event_t mon_power_even_mask;

sbit CS_ADE	= P3^1;


extern	int16_t	mon_acc_watt_a;
extern	int16_t	mon_acc_watt_b;
extern	int16_t	mon_acc_watt_c;

extern	int16_t	mon_acc_var_a;
extern	int16_t	mon_acc_var_b;
extern	int16_t	mon_acc_var_c;

extern	uint16_t	mon_acc_va_a;
extern	uint16_t	mon_acc_va_b;
extern	uint16_t	mon_acc_va_c;

#ifdef MON_POWER_EXTENDED_REPORT
	extern	uint32_t	v_a;
	extern	uint32_t	v_b;
	extern	uint32_t	v_c;

	extern	uint32_t	i_a;
	extern	uint32_t	i_b;
	extern	uint32_t	i_c;

	extern	uint16_t	ade_freq;
	extern	sint8_t		ade_temp;
	extern	uint32_t	ade_wave;
#endif
/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

#define inter_spi_delay() { \
	delay(MIN_DELAY_ADE_REG_READ); \
	CS_ADE = MON_POWER_ADE_CS_DISABLE; \
	delay(MIN_DELAY_ADE_REG_READ); \
	CS_ADE = MON_POWER_ADE_CS_ENABLE; \
	delay(MIN_DELAY_ADE_REG_READ); \
	}

//	delay(5); \

/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/



/**
*  mon_power_init_ade () - sets ADE chip params
*/
void mon_power_init_ade();


/**
* mon_power_get_ver() - reads version register
*/
uint8_t mon_power_get_ver();

/**
* uint8_t mon_power_create_report () - creates and fills out the report packet
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t mon_power_create_report ();


/**
* mon_power_read_power_all() - reads all Accum. registers (Watt-Hour, VAR, VA)
*/
uint8_t mon_power_read_power_all();



void mon_power_cmd(char* packet);

/**
*  mon_power_query_params (packet) - creates and fills out the report packet
*		which is queued for transmission in the end
* RETURN: 0 - when failed, 1 - otherwise
*/
uint8_t mon_power_query_params(char* packet);


/**
*  mon_power_change_params (packet) - changes ADE chip params
*/
void mon_power_change_params(char* packet);


uint8_t mon_power_param_report (uint8_t param_count, uint8_t *param_list );


/**
* mon_power_write_u32_reg( reg, val, write size) - 
* 		writes a value "val" to register "reg" of ADE sending the "write_size"
*		most significant bytes (octets) of the "val" 32-bit parameter
*/
void mon_power_write_u32_reg(uint8_t reg, uint32_t val, uint8_t write_size);


/**
* mon_power_read_u24_reg() - reads a 24-bit (signed) register
*/
uint32_t mon_power_read_u24_reg(uint8_t reg);

/**
* mon_power_read_s16_reg() - reads a 16-bit (signed) register
*/
int16_t mon_power_read_s16_reg(uint8_t reg);

/**
* mon_power_read_u16_reg() - reads a 16-bit (unsigned) register
*/
uint16_t mon_power_read_u16_reg(uint8_t reg);

/**
* mon_power_read_s8_reg() - reads a 8-bit (signed) register
*/
int8_t mon_power_read_s8_reg(uint8_t reg);

/**
* mon_power_read_u8_reg() - reads a 8-bit (unsigned) register
*/
uint8_t mon_power_read_u8_reg(uint8_t reg);


// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################





#endif // _MON_POWER_BASIC_PRV_H_