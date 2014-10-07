/****************************************************************************
**
** 
** 
** Author:  Rana Basheer
** eMail:   mrbxcf@mst.edu
** Version: 1.0
**
****************************************************************************/

#include "defs/esnl_pub.h"

#if defined(_ENABLE_RTLS_BEACON_)

#include "common_constants.h"
#include "common.h"
#include "sys_func.h"
#include "FEAT_Networking/tsp_common_pub.h"

#include "hal/micro/cortexm3/uart.h"
#include "rtls_beacon.h"
#include "microstrain.h"
#include "FEAT_Scheduler/sch_basic_pub.h"
#include "FEAT_STM32W/STM32W_Radio.h"

#include <string.h>

//constants
#define MAX_RSSI_INFO_SZ 100
#define PYTHON_START_PKT 	0xBE
#define PYTHON_END_PKT		0xEF
#define PYTHON_ESC_PKT		0xAB

//typedefs
typedef struct
{
  uint8_t uid;
  uint8_t rssi;
} rssi_data_t;

typedef enum
{
PYTHON_START_PKT_MODE = 0
,PYTHON_CMD_MODE
,PYTHON_TX_BCAST_ID_MODE
,PYTHON_TX_BCAST_UID_MODE
,PYTHON_TX_BCAST_CNT_MODE
,PYTHON_TX_BCAST_DELAY_MODE
,PYTHON_RX_BCAST_ID_MODE
,PYTHON_RX_BCAST_UID_MODE
,PYTHON_END_PKT_MODE
} python_mode_t;

typedef enum
{
PYTHON_ACK_CMD_PKT = 0
,PYTHON_CMD_RQST_TX_BCAST
,PYTHON_CMD_RESP_TX_BCAST
,PYTHON_CMD_RQST_TX_RSSI
,PYTHON_CMD_RESP_TX_RSSI
} python_cmd_t;   

//Function declarations
static void trigger_bcast();
static void send_bcast_now(uint8_t XDATA *context);
static uint8_t send_rssi(uint8_t base_station_id, uint8_t uid);
static void send_python_rssi_info(uint8_t receiver_id, uint8_t *rssi_data, uint8_t rssi_count);
static void send_python_bcast_info(uint8_t bcast_id, uint8_t bcast_indx, uint8_t bcast_uid, float tx_heading);
static void rtls_receive_beacon(uint8_t src_id, pkt_mod_header_t *mod, uint8_t rssi);
static void python_fsm(uint8_t ser_data);
static void rtls_loop(void);
static void handle_python_cmd(void);
static void send_bcast_rqst();
static void send_rssi_rqst();
static void putescchar(uint8_t chr);
static void putackchar(void);

//Global variables
//static uint8_t report_event_tid;/// Stores ID of the timeout for beacon
static uint8_t 			rtls_pkt_id;
static rssi_data_t 		rssi_info[MAX_RSSI_INFO_SZ];
static uint8_t 			bcast_uid;
static uint8_t 			bcast_current_count;
static uint8_t 			bcast_total_count;
static uint8_t 			bcast_ms_delay;
static python_mode_t	python_mode;
static python_cmd_t 	python_cmd;
static uint8_t			python_bcast_rqst_id;
static uint8_t			python_rssi_rqst_id;
static uint8_t			python_esc_mode;
//static uint32_t			next_bcast_time;

void rtls_beacon_power_up()
{
	//report_event_tid = SCH_NO_TIMEOUT_ID;
	bcast_total_count = 0;
	bcast_current_count = 0xFF;
	bcast_uid = 0;
	bcast_ms_delay = 0;
	rtls_pkt_id = 0xFF;
	python_esc_mode = 1;
	python_mode = PYTHON_START_PKT_MODE;
}

void rtls_beacon_init()
{
	tsp_register_port( PKT_MODULE_TYPE_RTLS_BEACON, rtls_receive_beacon);
	sch_add_loop((sch_loop_func_t)rtls_loop);
	//next_bcast_time = (uint32_t)-1;
}

static void rtls_receive_beacon(uint8_t src_id, pkt_mod_header_t *mod, uint8_t rssi)
{
    //mod->moule_type ==;
    //mod->module_length;
    //mod->content // is the first byte;
	uint8_t *content = &(mod->content);
	uint8_t rtls_cmd = *content; content++;
	// printf (
	switch(rtls_cmd)
	{
	case RTLS_BCAST_RQST:
	  {
	  //This message is only received by a mote acting as the transmitter
	  //A request came from the base station asking to send certain number of broadcast messages
	  bcast_uid = *content; content++;
	  bcast_total_count = *content; content++;
	  bcast_ms_delay = *content; content++;
	  bcast_current_count = 0;
	  trigger_bcast();	  
	  }  
	break;
	
	case RTLS_BCAST:
	  {
	  //This is a message that either a receiver or a base station will receive
	  uint8_t bcast_indx = *content-1; content++; 
	  uint8_t bcast_uid  = *content; content++;
	  float tx_heading = *((float *)content);
	  if(bcast_indx < MAX_RSSI_INFO_SZ)
	  	{
		rssi_info[bcast_indx].uid = bcast_uid;
		rssi_info[bcast_indx].rssi = rssi;
	  	send_python_bcast_info(src_id, bcast_indx, bcast_uid, tx_heading);
		}
	  }
	  break;
	  
	case RTLS_RSSI_RQST:
	  {
	  //this message is only received  by a mote acting as the receiver
	  //we are sending the RSSI information collected from the transmitter broadcast to the base station
	  uint8_t bcast_uid = *content;
	  send_rssi(src_id, bcast_uid);
	  }
	  break;
	
	case RTLS_RSSI_RESP:
	  {
	  //This message is only send from the receiver to the base station
	  //we have to send to the python the current heading that is obtained from the broadcast
      //message and also the rssi info obtained from the receiver
	  uint8_t rssi_count = mod->module_length-1;
	  uint8_t *rssi_data = content;
	  send_python_rssi_info(src_id, rssi_data, rssi_count);
	  }
	  break;
	}
}

/*
This function is executed by the transmitter on the request of the base station
we have to keep on sending multiple messages out
*/
static void trigger_bcast()
{
//we create a timer and when the timer expires a broadcast message will be sent out
sch_create_timeout( rtc_get_ticks()+bcast_ms_delay, send_bcast_now, NULL);
}

static void send_bcast_now(uint8_t XDATA *context)
{
  // 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
	uint8_t module_size = sizeof(float)+3*sizeof(uint8_t); // nothing to transmit ???
	uint8_t *content = NULL;
//	sprintf(buff, "\\%s", "");  
	content = tsp_new_module( PKT_MODULE_TYPE_RTLS_BEACON, 
                          module_size, 
                          &rtls_pkt_id, 
                          MAC_BROADCAST);

	if (0 == content)
	{
		// Error
		rtls_pkt_id = 0xFF;
		sys_error(SYS_ERROR_MEM_FULL);
	}
	else
	{
	    (*content) = RTLS_BCAST; content++;
	  	(*content) = bcast_current_count; content++; 
	  	(*content) = bcast_uid; content++; 
		*((float *)content) = microstrain_get_yaw();
		bcast_current_count++;
		tsp_send_from_modules(rtls_pkt_id);
	}
	if(bcast_current_count <= bcast_total_count)
		{
		trigger_bcast(); //retrigger the broadcast	
		}
}
/*
This is a command that is executed at the receiver. This will send the rssi information 
collected by this receiver to the base station
*/
static uint8_t send_rssi(uint8_t base_station_id, uint8_t uid)
{
	// 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
	uint8_t *content = NULL;
	uint8_t i;
	uint8_t rssi_info_count = 1; //we start from 1 for the command
	//Find out how many rssi packets are there in our array that corresponds
	//to the UID requested by the base station
	for(i = 0; i < MAX_RSSI_INFO_SZ; ++i)
		{
		if(rssi_info[i].uid == uid)
		  rssi_info_count++;
		}
	if(rssi_info_count == 0)
	  return 1;
	
	//We reserve a buffer space to send that rssi information
	content = tsp_new_module( PKT_MODULE_TYPE_RTLS_BEACON, 
                          rssi_info_count, 
                          &rtls_pkt_id, 
                          base_station_id);

	if (0 == content)
	{
		// Error
		rtls_pkt_id = 0xFF;
		sys_error(SYS_ERROR_MEM_FULL);
		return 0;
	}
	else
	{	
	  	*content = RTLS_RSSI_RESP; ++content;
	  	//copy the RSSI information to the buffer space that we reserved previously
		for(i = 0; i < MAX_RSSI_INFO_SZ; ++i)
			{
			if(rssi_info[i].uid == uid)
			  *content = rssi_info[i].rssi; ++content;
			}
		//off we go
		tsp_send_from_modules(rtls_pkt_id);
	}
	return 1;
}

/*
This is needed to escape control characters in the data
*/
static void putescchar(uint8_t chr)
{
if(chr == PYTHON_START_PKT || chr == PYTHON_END_PKT || chr == PYTHON_ESC_PKT)
	putchar(PYTHON_ESC_PKT);
putchar(chr);
}
/*
*/
static void send_python_bcast_info(uint8_t bcast_id, uint8_t bcast_indx, uint8_t bcast_uid, float tx_heading)
{
uint8_t *hdg_ptr = (uint8_t *)&tx_heading;
putchar(PYTHON_START_PKT);
putescchar(PYTHON_CMD_RESP_TX_BCAST);
putescchar(bcast_id);
putescchar(bcast_indx);
putescchar(bcast_uid);
putescchar(hdg_ptr[3]);
putescchar(hdg_ptr[2]);
putescchar(hdg_ptr[1]);
putescchar(hdg_ptr[0]);
putchar(PYTHON_END_PKT);
}

static void putackchar()
{
putchar(PYTHON_START_PKT);
putchar(PYTHON_ACK_CMD_PKT);
putchar(PYTHON_END_PKT);
}

/*
This function sends the heading, rssi information that the receiver collected to the 
python
*/
static void send_python_rssi_info(uint8_t receiver_id, uint8_t *rssi_data, uint8_t rssi_count)
{
uint8_t i=0;
putchar(PYTHON_START_PKT);
putescchar(PYTHON_CMD_RESP_TX_RSSI);
putescchar(receiver_id);
//putescchar(rssi_count);
for(i = 0; i < rssi_count; ++i)
	{
	putescchar(rssi_data[i]);	
	}
putchar(PYTHON_END_PKT);
}

/*
This function reads the serial port if it is not being used by the microstrain
*/
static void rtls_loop(void)
{
if(!is_microstrain_active())
	{
  	uint8_t ser_data;
	if ( serialReadByte( &ser_data ) )
		{
		python_fsm(ser_data);
		}
	}
}

/*
This is used to handle the control characters appearing in the data stream
*/
#define ESC_PYTHON_MODE()						\
	  	if(python_esc_mode)						\
			{									\
			if(ser_data == PYTHON_ESC_PKT)		\
				{								\
				python_esc_mode = 0;			\
				break;							\
				}								\
			}									\
		else									\
			{									\
			python_esc_mode = 1;				\
			}									\

/*
This function talks to the uart port to see if there is any command from the python host
*/
static void python_fsm(uint8_t ser_data)
{
switch(python_mode)
	{
	default:
	  python_mode = PYTHON_START_PKT_MODE;
	  break;
	  
	case PYTHON_START_PKT_MODE:
	  	python_mode = (ser_data == PYTHON_START_PKT)?PYTHON_CMD_MODE:PYTHON_START_PKT_MODE;
		python_esc_mode = 1;
		break;
				
	case PYTHON_CMD_MODE:
		ESC_PYTHON_MODE();
	  	python_cmd = (python_cmd_t)ser_data;
		switch(python_cmd)
			{
			case PYTHON_CMD_RQST_TX_BCAST:
			  python_mode = PYTHON_TX_BCAST_ID_MODE;
			  break;
			case PYTHON_CMD_RQST_TX_RSSI:
			  python_mode = PYTHON_RX_BCAST_ID_MODE;
			  break;
			default:
			  python_mode = PYTHON_START_PKT_MODE;
			  break;
			}
	  	break;
		
	case PYTHON_TX_BCAST_ID_MODE:
	  	ESC_PYTHON_MODE();
	  	python_bcast_rqst_id = ser_data;
		python_mode = PYTHON_TX_BCAST_UID_MODE;
	  	break;
	
	case PYTHON_TX_BCAST_UID_MODE:
	  	ESC_PYTHON_MODE();
	  	bcast_uid = ser_data;
		python_mode = PYTHON_TX_BCAST_CNT_MODE;	  	
	  	break;
	  
	case PYTHON_TX_BCAST_CNT_MODE:
	  	ESC_PYTHON_MODE();
		bcast_total_count = ser_data;
		python_mode = PYTHON_TX_BCAST_DELAY_MODE;
		break;
	
	case PYTHON_TX_BCAST_DELAY_MODE:
	  	ESC_PYTHON_MODE();
	  	bcast_ms_delay = ser_data;
		python_mode = PYTHON_END_PKT_MODE;
		break;
	
	case PYTHON_RX_BCAST_ID_MODE:
	  	ESC_PYTHON_MODE();
	  	python_rssi_rqst_id = ser_data;
		python_mode = PYTHON_RX_BCAST_UID_MODE;
		break;

	case PYTHON_RX_BCAST_UID_MODE:
	  	ESC_PYTHON_MODE();
	  	bcast_uid = ser_data;
		python_mode = PYTHON_END_PKT_MODE;	  	
	  	break;
		
	case PYTHON_END_PKT_MODE:	  	
	  	if(ser_data == PYTHON_END_PKT)
	    	handle_python_cmd();
		python_mode = PYTHON_START_PKT_MODE;
	  	break;
	}
}	  

static void handle_python_cmd(void)
{
switch(python_cmd)
	{
	case PYTHON_CMD_RQST_TX_BCAST:
	  send_bcast_rqst();
	  break;
	case PYTHON_CMD_RQST_TX_RSSI:
	  send_rssi_rqst();
	  break;
	default:
	  break;	 
	}
}

static void send_bcast_rqst()
{
  // 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
	uint8_t module_size = 4*sizeof(uint8_t); // nothing to transmit ???
	uint8_t *content = NULL;
//	sprintf(buff, "\\%s", "");  
	content = tsp_new_module( PKT_MODULE_TYPE_RTLS_BEACON, 
                          module_size, 
                          &rtls_pkt_id, 
                          python_bcast_rqst_id);

	if (0 == content)
	{
		// Error
		rtls_pkt_id = 0xFF;
		sys_error(SYS_ERROR_MEM_FULL);
	}
	else
	{
	  	(*content) = RTLS_BCAST_RQST; content++;
	  	(*content) = bcast_uid; content++; 
	  	(*content) = bcast_total_count; content++; 
	  	(*content) = bcast_ms_delay; content++; 
		tsp_send_from_modules(rtls_pkt_id);
		putackchar();
	}
}

static void send_rssi_rqst()
{
  // 1) Reserve a packet in a queue buffer (NOT QUEUED YET) and check if successful
	// 2) Get a pointer to the reserved MODULE inside the packet (set as DUMMY)
	uint8_t module_size = 2*sizeof(uint8_t); // nothing to transmit ???
	uint8_t *content = NULL;
//	sprintf(buff, "\\%s", "");  
	content = tsp_new_module( PKT_MODULE_TYPE_RTLS_BEACON, 
                          module_size, 
                          &rtls_pkt_id, 
                          python_rssi_rqst_id);

	if (0 == content)
	{
		// Error
		rtls_pkt_id = 0xFF;
		sys_error(SYS_ERROR_MEM_FULL);
	}
	else
	{
	  	(*content) = RTLS_RSSI_RQST; content++;
	  	(*content) = bcast_uid; content++; 
		tsp_send_from_modules(rtls_pkt_id);
		putackchar();
	}
}


#endif // defined(_ENABLE_RTLS_BEACON_)