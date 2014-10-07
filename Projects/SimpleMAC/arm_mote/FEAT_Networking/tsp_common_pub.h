#ifndef _TSP_COMMON_PUB_H_
#define _TSP_COMMON_PUB_H_




/****************************************************************************
**	Includes (PUBLIC)
****************************************************************************/
#include "defs/esnl_pub.h"
#include "packet.h"
/****************************************************************************
**	Constants, definies and typedefs  (PUBLIC)
****************************************************************************/

#define TSP_HANDLED_PACKET		1
#define TSP_NOT_HANDLED_PACKET	0

#ifdef __SDCC__
	typedef CODE void ( * tsp_cb_func_t)(uint8_t, pkt_mod_header_t *, uint8_t);// TASK_FUNC;
#else
	typedef void ( * code tsp_cb_func_t)(uint8_t, pkt_mod_header_t *, uint8_t) TASK_FUNC;
#endif // not _SDCC_
									 
/****************************************************************************
**	Variables definition (PUBLIC)
****************************************************************************/


/****************************************************************************
**	Functions declarations / prototypes (PUBLIC)
****************************************************************************/

/**
 * tsp_register_port( port, callback_func) - sets a new listening port
 *			when a message comes, the (callback_func) is executed
 *	RETURNS: 
 */
uint8_t tsp_register_port( uint8_t port, tsp_cb_func_t callback_func);

/****************************************************************************
**	Functions implementation (PUBLIC)
****************************************************************************/

/**
  * tsp_init() - sets up MOD module
  */
void tsp_init ( void );


/**
  * tsp_receive(p) - handle reception of packet "p"
  */
uint8_t tsp_receive( uint8_t XDATA *p, uint8_t rssi );


/**
  * tsp_release_packet(pkt_id) - releases the packet buffer (in queue)
  *			this should be used when the packet was reserved
  *			and will not be sent out nor used anymore
  */
void tsp_release_packet(uint8_t pkt_id);


/**
*	tsp_get_dummy(pkt_base) - finds the DUMMY module in the given packet
*		RETURNS: pointer (*) to the DUMMY module OR NULL value if failed
*/
uint8_t XDATA*tsp_get_dummy(uint8_t pkt_id);


/**
  * tsp_send_from_modules(pkt_id) - handle transmission of the new packet
  *			from the applications (actually enquing the packet)
  *			where the data in "p" is in new MODULAR format
  *		RETURNs: 1 - when succesful, 0 - when failed (queue full)
  */
uint8_t tsp_send_from_modules(uint8_t pkt_id);


/**
  * tsp_reserve_packet ( data_size, *pkt_base, dest_id ) - reserves a new packet
  *			and fills out the main fields and modules
  *			The reserved space for user/app module is marked as DUMMY module
  *		RETURNs: total length of the packet OR NULL if failed
  */
uint8_t tsp_reserve_packet ( uint8_t data_size, uint8_t *pkt_id, uint8_t dst_id );


/**
*	tsp_get_dummy(pkt_base) - finds the DUMMY module in the given packet
*		RETURNS: pointer (*) to the DUMMY module OR NULL value if failed
*/
uint8_t XDATA*tsp_get_dummy(uint8_t pkt_id);


/**
* tsp_new_module( module_type, module_size, *pkt_id, dst_addr)
* DESC:		Reserves packet and fills main fields
* PARAMS:	module_type - Uniques module/socket ID
*			module_size - Size of the module content
*			pkt_id		- pointer to the reference ID of the entire packet
*			dst_addr	- intended final destination address for the packet/module
* RETURN:	0 (NULL) when not successful, pointer to the module content otherwise
*/
uint8_t *tsp_new_module( uint8_t module_type, uint8_t module_size, uint8_t* pkt_id, uint16_t dst_addr);


/**
*	tsp_get_module(pkt_base, mod_type) - finds the "mod_type" module in the given packet
*		RETURNS: pointer (*) to the DUMMY module OR NULL value if failed
*/
uint8_t *tsp_get_module(uint8_t pkt_id, uint8_t mod_type);

// ############################################################################
// ############################################################################
// Spacers
// ############################################################################
// ############################################################################

#endif // _TSP_COMMON_PUB_H_

