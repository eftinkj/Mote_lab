
#include "rssi_sensor.h"
#include "sys_func.h"

#include "FEAT_STM32W/STM32W_Radio.h"

#if defined(_ENABLE_RSSI_SENSOR_)

#define	RS_DEFAULT_DST_ADDR	0xFD

IAR_PACKED struct
{
	rssi_one_hop_t RssiOneHop[RSSI_MAX_ONE_HOPS];
} RssiOneHops;


float rs_alpha;

uint16_t	rs_dst_addr;

void rs_power_up()
{
	uint8_t i;
	for(i=0; i< RSSI_MAX_ONE_HOPS; i++)
	{
		RssiOneHops.RssiOneHop[i].ID = NO_NODE_IDX;
		RssiOneHops.RssiOneHop[i].rssi_count = 0;
		RssiOneHops.RssiOneHop[i].aver_rssi = 0;
	}
	rs_alpha = 0.5;
	rs_dst_addr = RS_DST_ADDR;
}

void rs_init()
{
	// Connect to system (loop, timeout, etc)
	rs_dst_addr	= RS_DEFAULT_DST_ADDR;
}


void rs_rx_rssi(uint16_t id, sint8_t rssi)
{
	uint8_t	idx = rs_get_ID_idx(id);
	if ( NO_NODE_IDX == idx )
	{
		rs_add_ID( id, rssi);
	}
	else
	{
		// Should not happen
		rs_update_rssi_IDX(idx, rssi);		
	}
}

void rs_add_ID( uint16_t id, sint8_t rssi)
{
	uint8_t idx = rs_get_free_IDX(id);
	if ( NO_NODE_IDX == idx )
	{
		sys_error(10); // Unable to find free space -> remove unused/old?
	}
	else
	{
		RssiOneHops.RssiOneHop[idx].aver_rssi = rssi;
		rs_update_rssi_IDX(idx, rssi);
	}
}


uint8_t	rs_get_ID_idx(uint16_t id)
{
	uint8_t i;
	for(i=0; i<RSSI_MAX_ONE_HOPS; i++)
	{
		if (id == RssiOneHops.RssiOneHop[i].ID)
		{
			return i;
		}
	}
	return NO_NODE_IDX;
}

uint8_t	rs_get_free_IDX(uint16_t id)
{
	uint8_t i;
	for(i=0; i<RSSI_MAX_ONE_HOPS; i++)
	{
		if (NO_NODE_IDX == RssiOneHops.RssiOneHop[i].ID)
		{
			RssiOneHops.RssiOneHop[i].ID = id;
			RssiOneHops.RssiOneHop[i].rssi_count = 0;
			RssiOneHops.RssiOneHop[i].aver_rssi = 0;
			return i;
		}
	}
	return NO_NODE_IDX;
}

void rs_delete_IDX(uint8_t idx)
{
	if (idx <RSSI_MAX_ONE_HOPS)
		RssiOneHops.RssiOneHop[idx].ID = NO_NODE_IDX;
		RssiOneHops.RssiOneHop[idx].rssi_count = 0;
		RssiOneHops.RssiOneHop[idx].aver_rssi = 0;
}


void rs_update_rssi_IDX(uint8_t idx, sint8_t rssi)
{
	RssiOneHops.RssiOneHop[idx].rssi_history
		[RssiOneHops.RssiOneHop[idx].rssi_count++] = rssi;
	if (RssiOneHops.RssiOneHop[idx].rssi_count)
	{
		rs_sent_report(idx);	// TODO change to event/message driven to offload packet reception callback function??
		// WHO SHOULD ZERO THE COUNT??
		RssiOneHops.RssiOneHop[idx].rssi_count = 0;
	}
	RssiOneHops.RssiOneHop[idx].aver_rssi =
		rs_alpha * RssiOneHops.RssiOneHop[idx].aver_rssi + (1-rs_alpha)*rssi;
}

#include "common_constants.h"
#include "FEAT_Networking/tsp_common_pub.h"
void rs_sent_report(uint8_t idx)
{
	uint8_t * ptr_pkt_id = NULL;
	sint8_t * ptr_samples = NULL;
	uint8_t data_size = RS_MODULE_HEADER_LEN + RssiOneHops.RssiOneHop[idx].rssi_count;
	rs_module_t *ptr_module = (rs_module_t*)tsp_new_module( PKT_MODULE_TYPE_RS_RSSI_REPORT, data_size, ptr_pkt_id, rs_dst_addr );
	if (0 == ptr_module)
	{
		sys_error(11);
		return;
	}
	ptr_module -> reporter_addr = MY_ADDR;
	ptr_module -> node_addr = RssiOneHops.RssiOneHop[idx].ID;
	ptr_module -> rss_aver = RssiOneHops.RssiOneHop[idx].aver_rssi;
	ptr_module -> rssi_count = RssiOneHops.RssiOneHop[idx].rssi_count;
	ptr_samples = &(ptr_module -> rssi_samples) ;
	for (uint8_t i=0; i < RssiOneHops.RssiOneHop[idx].rssi_count; i++)
	{
		ptr_samples[i] = RssiOneHops.RssiOneHop[idx].rssi_history[i];
	}
	if (MY_ADDR == RS_DST_ADDR)
	{
		rs_receive_report((uint8_t*)ptr_module);
		tsp_release_packet(*ptr_pkt_id);
	}
	else if (0 == tsp_send_from_modules( *ptr_pkt_id ))
	{
		sys_error(12);
	}
	return;
}

void rs_receive_report(uint8_t *module)
{
	sint8_t * ptr_samples = NULL;
	rs_module_t *ptr_module = (rs_module_t*)module;
	//ptr_module -> reporter_addr;
	//ptr_module -> node_addr;
	//ptr_module -> rss_aver;
	ptr_samples = &(ptr_module ->rssi_samples);
	for (uint8_t i=0; i < ptr_module -> rssi_count; i++)
	{
		printf ( "From %d about %d rssi=%d\r", ptr_module -> reporter_addr, ptr_module -> node_addr, ptr_samples[i] );
	}
}

#endif // defined (_ENABLE_RSSI_SENSOR_)
