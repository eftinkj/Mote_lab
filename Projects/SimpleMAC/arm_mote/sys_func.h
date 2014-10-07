#ifndef _SYS_FUNC_H_
#define _SYS_FUNC_H_


#include "defs/esnl_pub.h"


typedef uint8_t error_t;

#define SYS_ERROR_NONE	0
#define SYS_ERROR_SPI	1
#define SYS_ERROR_UART0	2
#define SYS_ERROR_UART1	3
#define SYS_ERROR_XBEE_INIT	4
#define SYS_ERROR_XBEE_TX	5
#define SYS_ERROR_XBEE_RX	6
#define SYS_ERROR_7		7
#define SYS_ERROR_8		8
#define SYS_ERROR_9		9
#define SYS_ERROR_XBEE_INIT_TOTAL 10
#define SYS_ERROR_SPI_INCORRECT_STATE	11
#define SYS_ERROR_SPI_COLLISION		12
#define SYS_ERROR_SPI_OVERRUN		12
#define SYS_ERROR_SPI_WRONG_APP		13
#define SYS_ERROR_14		14
#define SYS_ERROR_15		15
#define SYS_ERROR_PWM8_WRONG_PWM_INDEX		16
#define SYS_ERROR_PWM8_WRONG_CONFIG		17
#define SYS_ERROR_18		18
#define SYS_ERROR_19		19
#define SYS_ERROR_LGS_WRONG_TAG_IDX		20
#define SYS_ERROR_LGS_TAG_MEM_FULL		21
#define SYS_ERROR_22		22
#define SYS_ERROR_23		23
#define SYS_ERROR_MEM_FULL		24
#define SYS_ERROR_QUEUE_FULL		25
#define SYS_ERROR_TSP_WRONG_PKT_IDX		26
#define SYS_ERROR_27		27
#define SYS_ERROR_28		28
#define SYS_ERROR_29		29
#define SYS_ERROR_31		30




extern unsigned char XDATA SWRSEF;	// software reset


/**
* sys_init(config) - handles initialization of system functions and variables
*/
void sys_init(void *config);


/**
* sys_reset() - resets the mote
*/
void sys_reset(void);


/**
* sys_error(e) - handles error anouncing
*/
void sys_error(error_t e);


#define LED_LONG_DELAY_TIME 40000

/**
* sys_error_led(repeat) - emits "repeat" number of LED blink series 
*    (with decreasing delay between blinks)
*		NOTE: blocks CPU until finished (except for interrupts)
*/
void sys_error_led ( uint8_t repeat );


#define HW_STOP_INTR() {EA = 0; EA = 0;}
#define HW_START_INTR() (EA = 1)


#endif
