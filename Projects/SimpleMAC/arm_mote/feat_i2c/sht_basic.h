// Have to be the first include (before ant other statement or #ifdef
#include "defs/esnl_pub.h"

#if defined(_ENABLE_I2C_)



#define SC_TWI0001 0x00000001
#define SC_TWI0010 0x00000002
#define SC_TWI0100 0x00000004
#define SC_TWI1000 0x00000008

//#define SC_TWIRECV 0x00000001
//#define SC_TWISEND 0x00000002
//#define SC_TWISTART 0x00000004
//#define SC_TWISTOP 0x00000008


#define SHT_TIMEOUT_MS 1000


//extern uint8_t sht_event_tid; /// Stores ID of the timeout for reading the tags
//uint8_t report_event_tid;/// Stores ID of the timeout for reporting the inventory

//extern uint16_t response_humi; // last measurment


/**
 * void sht_power_up() - internal initialization
 */
void sht_power_up();


/**
 * void sht_init() - external initialization (schedule tasks/events)
 */
void sht_init();

/**
 * viod sht_write_status(uint8_t *context) - send request for humidity measurments
 */
void sht_write_status(uint8_t *context);

/**
 * viod request_measument_humi(uint8_t *context) - send request for humidity measurments
 */
void sht_read_status(uint8_t *context);

/**
 * viod sht_req_humi(uint8_t *context) - send request for humidity measurments
 */
void sht_req_humi(uint8_t *context);

/**
 * void sht_read_humi (uint8_t *context) - read the humidity measurment
 */
void sht_read_humi (uint8_t *context);


/**
 * viod sht_req_temp(uint8_t *context) - send request for temperature measurments
 */
void sht_req_temp(uint8_t *context);


/**
 * void sht_read_temp (uint8_t *context) - read the temperature measurment
 */
void sht_read_temp (uint8_t *context);


#endif // _ENABLE_I2C_