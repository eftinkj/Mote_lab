
#include "STM32W_Radio.h"
#include "phy-library.h"


#ifdef _ARM_

#include "FEAT_Networking/Phy_layer.h"
#include "FEAT_Scheduler/sch_basic_pub.h"
#include "FEAT_Routing/routing.h"
#include "FEAT_Queuing/Queuing.h"
#include "Application.h"
#include "common.h"


//uint8_t dbug_test = 0;

// #ifdef _ARM_
/* buffer for received packet */
//	extern uint8_t rxPacket[128];
/* received packet flag */
//	extern boolean packetReceived;
//	extern int8_t rxPacketRSSI;
// #endif // _ARM_

uint16_t radio_dst = 0;

uint8_t stradio_pending_len_ = 0;
sint8_t *stradio_pending_data_ = NULL;
uint16_t stradio_pending_dst_ = 0;

int8_t stradio_power_ = 3;

// Shortened ADDRESSES
uint8_t	my_addr8_;// = MY_ADDR;
uint16_t	my_addr16_;// = MY_ADDR;


#define STALLED_THRESHOLD	10
uint32_t count_stalled = 0;
uint32_t stalled_reset = STALLED_THRESHOLD;

uint32_t stradio_count_ov_priority_ = 0; /// count how many packets dropped due to overrun (Priority packets only)
		
uint32_t tx_count_ = 0;
uint32_t tx_isr_count_ = 0;

// TX ISR related variables:
uint32_t count_succ_ = 0;
uint32_t count_cca_ = 0;
uint32_t count_nack_ = 0;
uint32_t count_ack_ = 0;
int32_t stradio_resend_ = -1;
uint32_t count_resend_ = 0;
StStatus last_status_ = 0;

uint8_t txTIDX = SCH_NO_TIMEOUT_ID;
boolean stradio_retransmit_req_;
uint32_t stradio_count_failed_retransmissions_ = 0;
char str_stradio_PktTO[] = "STRadio_Pkt_Tx_TO";

StStatus volatile result_1 = 0;

void radio_init()
{
#ifdef EUI64_TRUNK_ID
	{
		uint8_t *eui = ST_RadioGetEui64();
		my_addr8_ = eui[0];
		my_addr16_ = my_addr8_;
	}
#else
	{
		my_addr8_ = MY_ADDR;
		my_addr16_ = MY_ADDR;
	}
#endif
	result_1 = ST_RadioSetPowerMode(0x00);
#ifdef _STM32W_HIGH_POWER_
	// For High Power Module use this
	uint16_t txPowerMode = 0x0001; //USER_TX_POWER_MODE; // Boost and alternate Tx/RX
	result_1 = ST_RadioSetPowerMode(txPowerMode);
	GPIO_PCCFGH = GPIO_PCCFGH & 0xFFFFFF0F; // PC5
	GPIO_PCCFGH = GPIO_PCCFGH | 0x00000090; // PC5 = 0x9 - Special function (TX select)
	// Prepare PB5, PB6, PB7 for controlling the external amplifier
	GPIO_PBCFGH = GPIO_PBCFGH & 0xFFFF000F;
	GPIO_PBCFGH = GPIO_PBCFGH | 0x00001110;
	GPIO_PBCLR = 0x000000E0;
	GPIO_PBSET = 0x000000C0; // 0xC - enable LNA and activate chip, 0x4 - enable chip (exit sleep)
#endif //_STM32W_HIGH_POWER_

	my_rf_channel_ = DEFAULT_RF_CHANNEL;
	my_tx_power_ = DEFAULT_TX_POWER; // configured power level (default for orodinary communication
	stradio_retransmit_req_ = 0;
	ST_RadioSetNodeId(MY_ADDR);
	ST_RadioSetPanId(MY_PAN_ID);
	result_1 = ST_RadioSetChannel(my_rf_channel_);
	
	phy_set_power_level ( my_tx_power_ );
	stradio_pending_len_ = 0;
	stradio_pending_data_ = NULL;
	stradio_pending_dst_ = 0;
	
	sch_add_loop((sch_loop_func_t)radio_loop);
	

//	GPIO_PBOUT =
}


int stradio_retransmision_result = 0;
/*******************************************************************************
* Function Name  : radio_loop
* Description    : When Radio is idle then checks for packets to be transmitted
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void radio_loop()
{
	// if the TX is idle then dequeue next packet and start TX
	CLEAR_LED(RLED);
	if ( 1 == stradio_retransmit_req_)
	{
		#if defined(_ENABLE_XBEE_COMPAT_4BS_TX_) || defined(_FORCE_XBEE_COMPAT_TX_)
			#ifdef _FORCE_XBEE_COMPAT_TX_
				if (1)
			#else
         	   if (BS_ADDR == txPacket[6])
			#endif //_FORCE_XBEE_COMPAT_TX_
         	   {
					//txPacket[10]++;// = txPacket[3]; /* XBee COMPATIBILITY - increment sequence number */
					txPacket[3]++;
				}
		#endif // defined(_ENABLE_XBEE_COMPAT_4BS_TX_) || defined(_FORCE_XBEE_COMPAT_TX_)
		int temp_ret = ST_RadioTransmit(txPacket);
		if (ST_SUCCESS != temp_ret)
		{
			txComplete = TRUE; // FAILED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			stradio_retransmision_result = temp_ret;
		}
		else
		{
			txComplete = FALSE;
			stradio_count_failed_retransmissions_++;
		}
		stradio_retransmit_req_ = 0;
	} else
	if (FALSE == txComplete)
	{
		SET_LED(RLED);
		count_stalled++;
		if (count_stalled > stalled_reset)
		{
			//stalled_reset = count_stalled + STALLED_THRESHOLD;
			//txComplete = TRUE;
		}
	} else if ( 0 != stradio_pending_len_ )
	{
		sendPacketData( stradio_pending_len_, stradio_pending_data_, stradio_pending_dst_);
		stradio_pending_len_ = 0;
	} else if ( ( 0 == pkt_to_sent_len ) && ( 0xFF == pkt_to_sent_id ) )
	{
		stalled_reset++;
		pkt_to_sent_id = que_deQpackets();
		if ( 0xFF != pkt_to_sent_id )
		{
			char routed = 0;
			unsigned int base = QBUFF_BASE ( pkt_to_sent_id );
			// fill the TX-related variables
			pkt_to_sent_len = PAK_GET_TOTAL_LENGTH ( pkt_to_sent_id );
			// routing decisions for the packet
			routed = routing_send_DATA_base ( QBUFF_BASE ( pkt_to_sent_id ) );
			// If routing OK then start transmission process -> backoff
			if ( 1 == routed )
			{
				unsigned int mac_d = get_dst_mac_base ( base );
				sent_DATA_ = 1;
				sendPacketData(pkt_to_sent_len, (sint8_t*)&(QBUFF_ACCESS(base, 0)) , mac_d ); // send via the backoff implementation
				phy_sent_timeout_ = rtc_get_ticks() + my_tx_timeout_;
				SET_LED(YLED);
			}
			else
			{
				if (ROUTING_BEGAN_ROUTE_DISCOVERY == routed)
				{
					// re-enqueue the packet
					if (0 == que_enQpacket (pkt_to_sent_id))
					{
						release_pkt_in_tx();
					}
					else
					{
						pkt_to_sent_len = 0;
						pkt_to_sent_id = 0xFF;
					}
				}
				else
				{
#ifdef _ENABLE_APP_MOD_
					app_drop_pkt ( pkt_to_sent_id, MODULE_RTR, REASON_NOROUTE, EVENT_DSEND );
#endif // _ENABLE_APP_MOD_
					// drop packet if not routable
					release_pkt_in_tx();
				}
			}
		}
	}
}


/*******************************************************************************
* Function Name  : sendPacketData
* Description    : It allows to transmit the data
* Input          : - lenght of the data
*                  - data to be transmitted
*                  - 16bit destination address
* Output         : None
* Return         : (bool) (1) if successfuly requested TX
*					(0) if failed to TX (e.g. due to ongoing tx)
*******************************************************************************/
boolean sendPacketData(uint8_t length, sint8_t *data, uint16_t dst)
{
	ATOMIC(
		if (FALSE == txComplete) return 0;
		if (txComplete == TRUE)  { txComplete = FALSE; }
	)
		if (txComplete == FALSE)
		{
			stm32_pkt16_t *p = (stm32_pkt16_t *)txPacket;
			p->fcf.src_mode = MAC_ADDR_MODE_16BIT;
			p->fcf.dst_mode = MAC_ADDR_MODE_16BIT;
			p->dst_addr = dst;
			radio_dst = dst;
#if defined(_ENABLE_XBEE_COMPAT_4BS_TX_) || defined(_FORCE_XBEE_COMPAT_TX_)
	#ifdef _FORCE_XBEE_COMPAT_TX_
			if (1)
	#else
            if (BS_ADDR == dst)
	#endif //_FORCE_XBEE_COMPAT_TX_
            {
				txPacket[10] = txPacket[3]; /* XBee COMPATIBILITY - increment sequence number */
				txPacket[11] = 0x80; //??
//				p->src_addr = MY_ADDR; 	// used for XBee repeat filtering
				p->src_pan = MY_ADDR;	// this is the actual ADDRESS of the source;
			}
			else
#endif // defined(_ENABLE_XBEE_COMPAT_4BS_TX_) || defined(_FORCE_XBEE_COMPAT_TX_)
			{
				p->src_addr = MY_ADDR;
				p->src_pan = MY_ADDR;
			}
			halCommonMemCopy(txPacket+12, data, length);
			txPacket[0] = length + 2 + 11;
			txPacket[3]++; /* increment sequence number */
			tx_count_++;
			if (ST_SUCCESS != ST_RadioTransmit(txPacket))
			{
				txComplete = TRUE; // FAILED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			}
			else
			{
				stradio_resend_ = STRADIO_DEFAULT_RETRANSMISSION_COUNT;
				if(stradio_resend_ > 0)
				{
					txTIDX = sch_create_timeout(rtc_get_ticks()+my_tx_timeout_, hPacketTimeout, &txTIDX, str_stradio_PktTO);
				}
			}
		}
	return (~txComplete);
}/* end sendPacketData() */




void sendPriorityPacket(uint8_t length, sint8_t *data, uint16_t dst)
{
	if (0)//(TRUE == txComplete)
	{
		sendPacketData( length, data, dst);
	}
	else
	{
		if (0 != stradio_pending_len_)
		{
			stradio_count_ov_priority_++;
		}
		//str_pending_ = 1;
		stradio_pending_len_ = length;
		stradio_pending_data_ = data;
		stradio_pending_dst_ = dst;
	}
}

uint32_t stradio_count_tx_timeouts_=0;
void hPacketTimeout(uint8_t* tidx)
{
	ATOMIC(
	if (FALSE == txComplete)
	{
#ifdef _ENABLE_APP_MOD_
			app_drop_pkt ( pkt_to_sent_id, MODULE_PHY, REASON_RADIO_FAILURE, EVENT_DSEND );
#endif // _ENABLE_APP_MOD_
		// drop packet if not routable
		release_pkt_in_tx();
		sent_DATA_ = 0;
		// Not needed since it is cleared imediatelly after TX ??
		//stradio_pending_len_ = 0;
		txComplete = TRUE;
	}
	)
	stradio_count_tx_timeouts_++;
}




/*******************************************************************************
* Function Name  : ST_RadioTransmitCompleteIsrCallback
* Description    : Radio Transmit callback function
* Input          : - status: status of the packet transmission
*                  - sfdSentTime: MAC timer when the SFD was sent
*                  - framePending: TRUE if the received ACK indicates that data
*                    is pending for this node
* Output         : None
* Return         : None
*******************************************************************************/
/* ::ST_SUCCESS - the last byte of the non-ACK-request packet has been
 * transmitted.
 * ::ST_PHY_ACK_RECEIVED - the requested ACK was received.
 * ::ST_MAC_NO_ACK_RECEIVED - the requested ACK was not received in time.
 * ::ST_PHY_TX_CCA_FAIL - unable to transmit due to lack of clear channel on
 * all attempts.
 * ::ST_PHY_TX_UNDERFLOW - DMA underflow occurred while transmitting.  Should
 * never happen.
 * ::ST_PHY_TX_INCOMPLETE - The PLL synthesizer failed to lock while
 * transmitting.  Should never happen.
*/	

void ST_RadioTransmitCompleteIsrCallback(StStatus status,
		uint32_t sfdSentTime,
		boolean framePending)
{
	tx_isr_count_++;
	switch(status) {
	case ST_SUCCESS:
//	  	if (MAC_BROADCAST == radio_dst)
//			txComplete = TRUE;
//		break;
		count_succ_++;
		break;
	case ST_PHY_TX_CCA_FAIL:
		count_cca_++;
		break;
	case ST_MAC_NO_ACK_RECEIVED:
		count_nack_++;
					//dbug_test++;
		if (0 < stradio_resend_--)
		{
			count_resend_++;
			stradio_retransmit_req_ = 1;
			//ATOMIC (txComplete = TRUE;)
					//dbug_test++;

			return;
		} // else -> drop packet -> done below
		else
		{
			// Drop link -> should I check if the failures were all for the same link?
			rtr_dropped_link(radio_dst);
			//	if ( NUMQBUFFS >= pkt_to_sent_id)
			//		rtr_dropped_link(STRADIO_GET_SRC_ADDR( ( (sint8_t*)&(buffer0[pkt_to_sent_id*SIZE_Q_BUFF]) ) ));
		}
					//dbug_test++;

		break;
	case ST_PHY_ACK_RECEIVED:
		count_ack_++;
		break;
	default:
		break;
	}

	last_status_ = status;
	if (1==sent_DATA_)
	{
		release_pkt_in_tx(); // release packet that was sent
	}
	sent_DATA_ = 0;
	sch_remove_timeout(txTIDX, str_stradio_PktTO);
	txTIDX = SCH_NO_TIMEOUT_ID;
	ATOMIC (txComplete = TRUE;)
}/* end ST_RadioTransmitCompleteIsrCallback() */

#endif // _ARM_
