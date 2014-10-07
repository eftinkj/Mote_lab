
#include "tsp_common_prv.h"
#include "tsp_common_pub.h"

#include "packet.h"
#include "FEAT_Queuing/Queuing.h"
#include "sys_func.h"

#ifdef _ENABLE_ROBOTICS_01_
	#include "PRJ_Robotics_01/truck_basic_control_pub.h"
#endif // _ENABLE_ROBOTICS_01_

#ifdef _ENABLE_ROBOTICS_02_
	#include "PRJ_Robotics_02/bot_basic_control_pub.h"
#endif // _ENABLE_ROBOTICS_02_


#ifdef _ENABLE_ROBOTICS_03_
#include "PRJ_Robotics_03/qrhelo_basic_control_pub.h"
#endif // _ENABLE_ROBOTICS_03_


#ifdef _ENABLE_ROBOTICS_04_
#include "PRJ_Robotics_04/vex_tank_basic_control_pub.h"
#endif // _ENABLE_ROBOTICS_04_


#ifdef _ENABLE_NEM_UTILITIES_01_
	#include "PRJ_NEM_Utilities_01/mon_power_basic_pub.h"
#endif // _ENABLE_NEM_UTILITIES_01_


#ifdef _ENABLE_LOGISTICS_01_
	#include "FEAT_LGS_01/lgs_basic_pub.h"
#endif // _ENABLE_LOGISTICS_01_


#if defined(_ENABLE_CAMT_HEADSET_01_) || defined(_ENABLE_CAMT_HEADSET_02_)
	#include "PRJ_CAMT_Headset_01/hset_basic_pub.h"
	#include "FEAT_XBee_API/API_frames.h"
#endif // _ENABLE_CAMT_HEADSET_01_


#ifdef _ENABLE_FLA_BASIC_
	#include "FEAT_Flash_Access/fla_basic_pub.h"
#endif // _ENABLE_FLA_BASIC_

#ifdef _ENABLE_SSN_BASIC_
	#include "FEAT_SigProc/ssn_basic_pub.h"
#endif // _ENABLE_SSN_BASIC_

#include "FEAT_Queuing/Queuing.h"

#include "FEAT_STM32W/STM32W_Radio.h"

#include "Application.h"
//#include "HW_LIB/adc_0.h"
/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/

#define MAX_PORTS	10


/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Variables definition (PRIVATE)
****************************************************************************/
uint8_t		tsp_callback_port[MAX_PORTS];
tsp_cb_func_t	tsp_callback_funcs[MAX_PORTS];
uint8_t		tsp_cb_count = 0;

/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/

/****************************************************************************
**	Functions declarations / prototypes (PRIVATE)
****************************************************************************/

/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions implementation (PRIVATE)
****************************************************************************/


/**
 * tsp_register_port( port, callback_func) - sets a new listening port
 *			when a message comes, the (callback_func) is executed
 *	RETURNS:
 */
uint8_t tsp_register_port( uint8_t port, tsp_cb_func_t callback_func)
{
	if (tsp_cb_count < MAX_PORTS) {
		tsp_callback_port[tsp_cb_count] = port;
		tsp_callback_funcs[tsp_cb_count] = callback_func;
		tsp_cb_count++;
		return tsp_cb_count-1;
	} else {
		return 0xFF; // failed
	}
}

/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

/**
  * tsp_init() - sets up MOD module
  */
void tsp_init ( void )
{
	tsp_cb_count = 0;
	// DO NOTHING ???
}


/**
  * tsp_receive(p) - handle reception of packet "p"
  */
uint8_t tsp_receive( uint8_t XDATA *p, uint8_t rssi )
{
	pkt_t XDATA *pkt = ( pkt_t XDATA * ) p;
	switch (pkt->flags)
	{
		case FLAG_DATA:
			break;
		case FLAG_DATA_TEST:
			break;
		case FLAG_DATA_ARB:
			break;
		case FLAG_DATA_TEST_V2:		
		case FLAG_DATA_MODULAR:
			tsp_distribute_modules( p, rssi );
			return TSP_HANDLED_PACKET;

			//break;
		default:
			// Drop the packet and forget
			break;
	}
	return TSP_NOT_HANDLED_PACKET;
}



/**
  * tsp_distribute_modules(p) - handle distribution of modules from packet "p"
  *			to the applications
  *			where the data in "p" is in new MODULAR format
  *		TODO: Make the distribution flexible (configured online via functions)
  */
void tsp_distribute_modules(uint8_t XDATA *p, uint8_t rssi )
{
	pkt_v2_t XDATA *pkt = ( pkt_v2_t XDATA * )p ;

	// NOTE: Initially only handling of the first module is suported
	// TODO: Enable multiple modules in one packet
	uint8_t offset = PKT_V2_MODULES_OFFSET;
	uint8_t len = pkt->length + PKT_HEADER_LENGTH;
	pkt_mod_header_t XDATA *mod = (pkt_mod_header_t XDATA*) &(p[offset]);
	uint8_t src_id = pkt->src_id;
//	while ( (offset < (pkt->length + PKT_HEADER_LENGTH)) && (offset < MAX_PKT_SIZE) )
	while ( (offset < len) && (offset < MAX_PKT_SIZE) )
	{
		mod = (pkt_mod_header_t XDATA *) &(p[offset]);
//		switch (pkt->data_modules)
		switch (mod->module_type)
		{
			case PKT_MODULE_TYPE_ADFS:
				break;
			case PKT_MODULE_TYPE_PONG:
				break;
			case PKT_MODULE_TYPE_HOPLIST:
				break;
			case PKT_MODULE_TYPE_PCC_UPSTREAM:
				break;
			//case PKT_MODULE_TYPE_CS_V1:
			//	break;
			case PKT_MODULE_TYPE_RAW_CHANNEL:
				break;
			case PKT_MODULE_TYPE_RAW_CHANNEL_V1:
				break;
			case PKT_MODULE_TYPE_RAW_CHANNEL_V2:
				break;
			case PKT_MODULE_TYPE_RAW_CHANNEL_V3:
				break;
#ifdef _ENABLE_ROBOTICS_01_
			case PKT_MODULE_TYPE_TRUCK_CMD_V1:
				truck_execute_command( mod );
				break;
#endif // _ENABLE_ROBOTICS_01_

#ifdef _ENABLE_ROBOTICS_02_
			case PKT_MODULE_TYPE_BOT_CMD_V1:
				bot_execute_command( mod );
				break;
#endif // _ENABLE_ROBOTICS_02_

#ifdef _ENABLE_ROBOTICS_03_
			case PKT_MODULE_TYPE_QRHELO_CMD_V1:
				qrhelo_execute_command( mod );
				break;
#endif // _ENABLE_ROBOTICS_03_


#ifdef _ENABLE_ROBOTICS_04_
			case PKT_MODULE_TYPE_VEX_TANK_CMD_V1:
				vex_tank_execute_command( mod );
				break;
#endif // _ENABLE_ROBOTICS_04_


#ifdef _ENABLE_NEM_UTILITIES_01_
			case PKT_MODULE_TYPE_MON_POWER_CMD_V1:
			case PKT_MODULE_TYPE_MON_POWER_REPORT_V1:
				mon_power_execute_command( mod );
				break;
#endif // _ENABLE_NEM_UTILITIES_01_

#ifdef _ENABLE_LOGISTICS_01_
			case PKT_MODULE_TYPE_LGS_MOTE_REPORT:
				lgs_recv_command( mod, rssi );
				break;
	#ifdef _ENABLE_RFID_READER_DETECTION_
			case PKT_MODULE_TYPE_RFID_READER_V1:
				logistics_rfid_read( mod, rssi );
				break;
	#endif //_ENABLE_RFID_READER_DETECTION_

	#ifdef _ENABLE_DLS_TRUCK_MOTE_
			case PKT_MODULE_TYPE_LGS_GATEMOTE_BEACON:
//				logistics_gate_read( mod, rssi );
				break;
	#endif // _ENABLE_DLS_TRUCK_MOTE_


	#ifdef _ENABLE_DLS_GATE_MOTE_
			case PKT_MODULE_TYPE_RFID_READER_V1:
//				logistics_recv_gatemote(mod, rssi );
				break;
	#endif // _ENABLE_DLS_GATE_MOTE_

//PKT_MODULE_TYPE_LGS_RFIDR_DETECT_REPORT
// PKT_MODULE_TYPE_LGS_INVENTORY_REPORT
//			case PKT_MODULE_TYPE_LGS_MOTE_REPORT:
//				break;

			case PKT_MODULE_TYPE_LGS_MOTE_BEACON:
//	#ifdef _ENABLE_RFID_READER_DETECTION_
				lgs_recv_mote_beacon( mod, rssi);
//	#endif // _ENABLE_RFID_READER_DETECTION_
				break;
			case PKT_MODULE_TYPE_LGS_MOTE_ASSOC_REQ:
				lgs_recv_assoc_req(mod, rssi);
				break;
			case PKT_MODULE_TYPE_LGS_MOTE_QUERY:
				break;

#endif // _ENABLE_LOGISTICS_01_

#ifdef _ENABLE_FLA_BASIC_
//#ifdef _ENABLE_RFID_READER_DETECTION_
			case PKT_MODULE_TYPE_FLA_BASIC:
				fla_recv_cmd( mod, rssi );
				break;
#endif // _ENABLE_FLA_BASIC_

#ifdef _ENABLE_SSN_BASIC_
			case PKT_MODULE_TYPE_SSN_V1:
				ssn_execute_command ( (uint8_t XDATA *)mod );
				break;
#endif // _ENABLE_SSN_BASIC_

#ifdef FEAT_ENABLE_CH_SWITCHING
			case PKT_MODULE_TYPE_CS_V1:
				cs_recv_switching_request( (char *)mod );
				break;
#endif // FEAT_ENABLE_CH_SWITCHING
				
#if defined(_ENABLE_CAMT_HEADSET_01_) || defined(_ENABLE_CAMT_HEADSET_02_)
			case PKT_MODULE_TYPE_HSET_REPORT_V1:
			case PKT_MODULE_TYPE_HSET_SAMPLES_V1:
				hset_execute_command(&(pkt->data_modules));
				break;
#endif // _ENABLE_CAMT_HEADSET_01_

#if defined(_ENABLE_RSSI_SENSOR_)
		  	case PKT_MODULE_TYPE_RS_RSSI_REPORT:
				rs_receive_report((uint8_t XDATA *)mod );
			 	break;
#endif // #if defined(_ENABLE_RSSI_SENSOR_)
			case PKT_MODULE_TYPE_END:
			case PKT_MODULE_TYPE_DUMMY:
			default:
				//TODO: Add dynamic port assignment/handling
			{
				uint8_t i;
				for(i=0; i<tsp_cb_count; i++)
				{
					if (tsp_callback_port[i] == mod->module_type)
					{
						(tsp_callback_funcs[i])(src_id, mod, rssi) ;
					}
				}
			}
				return;
				//break;
		}
		offset += (mod->module_length + 2);
	} // while end
}


/**
 * tsp_release_packet(pkt_id) - releases the packet buffer (in queue)
 *			this should be used when the packet was reserved
 *			and will not be sent out nor used anymore
 */
void tsp_release_packet(uint8_t pkt_id)
{
	que_make_empty_buffer ( pkt_id );
}



/**
  * tsp_send_from_modules(pkt_id) - handle transmission of the new packet
  *			from the applications (actually enquing the packet)
  *			where the data in "p" is in new MODULAR format
  *		RETURNs: 1 - when succesful, 0 - when failed (queue full)
  */
uint8_t tsp_send_from_modules(uint8_t pkt_id)
{
	// Enqueue packet
	if ( 0 == que_enQpacket ( pkt_id ) )
	{
		// Failed -> free the packet
		app_drop_pkt ( pkt_id, MODULE_APP, REASON_QUEUE_FULL, EVENT_DGEN );
		que_make_empty_buffer ( pkt_id );
		return 0;
		// Handle the lack of space in queue
	}
	return 1;
}



/**
  * tsp_reserve_packet ( data_size, *pkt_base, dest_id ) - reserves a new packet
  *			and fills out the main fields and modules
  *			The reserved space for user/app module is marked as DUMMY module
  *		RETURNs: total length of the packet OR ZERO (0) if failed
  */
uint8_t tsp_reserve_packet ( uint8_t data_size, uint8_t *pkt_id, uint8_t dst_id )
{

	int16_t len = PKT_HEADER_LENGTH; // size of the packet header (start + flag + dst(2) + src(2) + len)
	int16_t msg_i;
	uint16_t	pkt_base;
	// SDCC:
	pkt_v2_t * XDATA pkt = NULL;
	uint8_t * XDATA msg;
	int i;

	{
		// reserve packet
		(*pkt_id) = que_get_empty_buffer_location();
		if ( 0xFF == (*pkt_id) )
		{
			// Error with packet buffer allocation!!
			// TODO: Handle the lack of space for new packet
			app_drop_pkt ( 0xFFFF, MODULE_APP, REASON_MEMORY_FULL, EVENT_DGEN );
			return 0;
		}
	}
	pkt_base = ( uint16_t ) QBUFF_BASE ( (*pkt_id) );
	pkt = ( pkt_v2_t* ) ( & ( buffer0[pkt_base] ) ); //&(QBUFF_ACCESS(base,0));

	pkt->start = START_BYTE;
	pkt->flag = FLAG_DATA_TEST_V2;

	pkt->mac_dst = SWAP16((uint16_t)(0xFF==dst_id?0xFFFF:dst_id));//0xFFFF);
	pkt->mac_src = SWAP16(MY_ADDR);

	msg = & ( pkt->dst_id );

	msg_i = 0;

	// FILL END-TO-END ADDRESSING
	msg[msg_i] = dst_id; // BS_ADDR;
	msg[msg_i+1] = MY_ADDR;

	len = len + 2;
	msg_i = msg_i + 2;

#ifdef FEAT_ENABLE_SFQ
	{
		pkt_mod_header_t *ph = ( pkt_mod_header_t * ) ( & ( msg[msg_i] ) );
		pkt_mod_adfs_t *p_adfs = ( pkt_mod_adfs_t* ) ( &msg[msg_i + PKT_MODULE_LEN_HEADER] );

		ph->module_type = PKT_MODULE_TYPE_ADFS;
		ph->module_length = PKT_MODULE_LEN_ADFS;

		p_adfs->weight = my_source_weight_;
		p_adfs ->ticker = rtc_get_rticks();

		len = len + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_ADFS;
		msg_i = msg_i + PKT_MODULE_LEN_HEADER + PKT_MODULE_LEN_ADFS;
	}
#endif // FEAT_ENABLE_SFQ


	{
		pkt_mod_header_t *ph = ( pkt_mod_header_t * ) ( & ( msg[msg_i] ) );
		ph->module_type = PKT_MODULE_TYPE_DUMMY; // Data portion type

		len += PKT_MODULE_LEN_HEADER;
		msg_i += PKT_MODULE_LEN_HEADER;

		// FILL THE REST OF 100byte packet (90 bytes of samples)
		// MZ: 2011/08/05 - does not belong here:
		//		if ( SAMPLES_PER_PACKET < data_size )
		//			data_size = SAMPLES_PER_PACKET;
		if ( MAX_PKT_SIZE < len + data_size )
			data_size = MAX_PKT_SIZE - len;

		ph->module_length = data_size; // Data portion length

		for ( i = 0; i < data_size; i++ )
		{
			msg[msg_i] = sequence_no_;
			msg_i++;
			len++;
		}
		sequence_no_++;	// increase for next packet
	}

	msg[msg_i] = PKT_MODULE_TYPE_END; // End of modules

	len++;
	msg_i++;

	pkt->length = msg_i; // of the packet'd data

	return len;
}

/**
*	tsp_get_dummy(pkt_base) - finds the DUMMY module in the given packet
*		RETURNS: pointer (*) to the DUMMY module OR NULL value if failed
*/
uint8_t XDATA *tsp_get_dummy(uint8_t pkt_id)
{
	uint16_t pkt_base = ( uint16_t ) QBUFF_BASE ( pkt_id );
	pkt_v2_t *pkt_v2  = ( pkt_v2_t* ) ( & ( buffer0[pkt_base] ) );
	pkt_mod_header_t *ph = ( pkt_mod_header_t * ) ( & ( pkt_v2->data_modules ) );
	uint8_t mod_len = 0;
	uint8_t pkt_len = PKT_HEADER_LENGTH + 2;
	if (pkt_id >= NUMQBUFFS)
	{
		sys_error(SYS_ERROR_TSP_WRONG_PKT_IDX);
		return NULL;
	}

	while (( PKT_MODULE_TYPE_END != ph->module_type ) && ( PKT_MODULE_TYPE_DUMMY != ph->module_type ))
	{
		if (MAX_PKT_SIZE < pkt_len + PKT_MODULE_LEN_HEADER )
		{
			return NULL;
		}
		mod_len = ph->module_length;
		ph = ( ( pkt_mod_header_t * ) ( ( ( char* ) ( ph ) ) + mod_len + PKT_MODULE_LEN_HEADER ) );
	} //while
	if (PKT_MODULE_TYPE_DUMMY == ph->module_type )
		return (uint8_t XDATA*)ph;
	else
		return NULL;
}

/**
* tsp_new_module( module_type, module_size, *pkt_id, dst_addr)
* DESC:		Reserves packet and fills main fields
* PARAMS:	module_type - Uniques module/socket ID
*			module_size - Size of the module content
*			pkt_id		- pointer to the reference ID of the entire packet
*			dst_addr	- intended final destination address for the packet/module
* RETURN:	0 (NULL) when not successful, pointer to the module content otherwise
*/
uint8_t *tsp_new_module( uint8_t module_type, uint8_t module_size, uint8_t* ptr_pkt_id, uint16_t dst_addr)
{
	if (0 == tsp_reserve_packet( module_size, ptr_pkt_id, dst_addr ))
	{
		sys_error(11);
		return NULL;
	}
	{
		uint8_t *mod = (uint8_t *)tsp_get_dummy(*ptr_pkt_id);
		pkt_mod_header_t *ph = ( pkt_mod_header_t * )mod ;
		ph->module_type = module_type;
		ph->module_length = module_size;
		return &(ph->content);
	}
}



/**
*	tsp_get_module(pkt_base, mod_type) - finds the "mod_type" module in the given packet
*		RETURNS: pointer (*) to the DUMMY module OR NULL value if failed
*/
uint8_t *tsp_get_module(uint8_t pkt_id, uint8_t mod_type)
{
	uint16_t pkt_base = ( uint16_t ) QBUFF_BASE ( pkt_id );
	pkt_v2_t *pkt_v2  = ( pkt_v2_t* ) ( & ( buffer0[pkt_base] ) );
	pkt_mod_header_t *ph = ( pkt_mod_header_t * ) ( & ( pkt_v2->data_modules ) );
	uint8_t mod_len = 0;
	uint8_t pkt_len = PKT_HEADER_LENGTH + 2;

	while (( PKT_MODULE_TYPE_END != ph->module_type ) && ( mod_type != ph->module_type ))
	{
		if (MAX_PKT_SIZE < pkt_len + PKT_MODULE_LEN_HEADER )
		{
			return NULL;
		}
		mod_len = ph->module_length;
		ph = ( ( pkt_mod_header_t * ) ( ( ( char* ) ( ph ) ) + mod_len + PKT_MODULE_LEN_HEADER ) );
	} //while
	if (mod_type == ph->module_type )
		return (uint8_t*)ph;
	else
		return NULL;
}
// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################


