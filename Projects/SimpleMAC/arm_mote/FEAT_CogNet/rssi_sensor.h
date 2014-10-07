
#ifndef _RSSI_SENSOR_H_


#include "defs/esnl_pub.h"


#define NO_NODE_IDX	0xFF
#define NO_NODE_ADDR	0xFFFF
#define RSSI_MAX_ONE_HOPS	20

#define RSSI_MAX_HISTORY_RSSI 20

#define RS_DST_ADDR	0xFA

typedef IAR_PACKED struct
{
	uint16_t 	ID;
	uint8_t		rssi_count; // last rssi
	sint8_t		rssi_history[RSSI_MAX_HISTORY_RSSI]; // last X rssi values
	float		aver_rssi; // moving average RSSI
} PACKED rssi_one_hop_t;


typedef IAR_PACKED struct {
	uint16_t	reporter_addr;	/// Address of the reporting node
	uint16_t	node_addr;	/// Address of the node that is being reported
	float		rss_aver;	/// Average RSSI
	uint8_t		rssi_count;
	sint8_t		rssi_samples;	/// First RSSI sample
} PACKED rs_module_t;

// Size of header (without samples)
#define	RS_MODULE_HEADER_LEN	(sizeof(rs_module_t)-1)

/*
IAR_PACKED struct
{
	rssi_one_hop_t RssiOneHop[RSSI_MAX_ONE_HOPS];
} RssiOneHops;

float rs_alpha;
*/

void rs_power_up();

void rs_init();

void rs_rx_rssi(uint16_t id, sint8_t rssi);

void rs_add_ID( uint16_t id, sint8_t rssi);

uint8_t	rs_get_ID_idx(uint16_t id);

uint8_t	rs_get_free_IDX(uint16_t id);

void rs_delete_IDX(uint8_t idx);

void rs_update_rssi_IDX(uint8_t idx, sint8_t rssi);


void rs_sent_report(uint8_t idx);

void rs_receive_report(uint8_t *module);

#endif //_RSSI_SENSOR_H_

