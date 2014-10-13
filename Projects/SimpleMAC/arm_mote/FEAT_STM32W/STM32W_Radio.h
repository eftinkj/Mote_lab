#ifndef _STM32W_RADIO_H_
#define _STM32W_RADIO_H_

#include "defs/esnl_pub.h"
#include "phy-library.h"


// #ifdef _ARM_
	/* buffer for received packet */
	extern uint8_t rxPacket[128];
	/* received packet flag */
	extern boolean packetReceived;
	extern uint8_t rxPacketRSSI;
// #endif // _ARM_
	
#pragma align txPacket
extern uint8_t txPacket[128];
extern boolean txComplete;
extern unsigned char XDATA sent_DATA_;

extern int8_t stradio_power_; // stores the power value for STM32W radio


extern uint8_t	my_addr8_;
extern uint16_t	my_addr16_;


typedef IAR_PACKED struct {
	uint16_t	mac_type	:3; // MAC frame type: 0-beacon, 1-data, 2-ack, 3-MAC cmd, 4-reserved, 5-reserved
	uint16_t security	:1;	// 1 - MAC-level cryptography: 1-enabled; 0-disabled
	uint16_t pending		:1;	// 1 - next data packet pending
	uint16_t ack_req		:1;	// 1 - ACK is requested; 0 - noACK
	uint16_t intra_pan	:1; // 1->inter-PAN; 0->intra-PAN
	uint16_t reserved1	:3;
	uint16_t dst_mode	:2;	// dest. addr. mode (0-no PAN and addr; 1-reserved; 2-16bit address; 3-64bit address)
	uint16_t reserved2	:2;
	uint16_t src_mode	:2;	// source addr. mode (0-no PAN and addr; 1-reserved; 2-16bit address; 3-64bit address)
}PACKED fcf_t;

typedef IAR_PACKED struct {
	uint8_t		len;
	fcf_t	fcf;
	uint8_t		seq;
	uint16_t		dst_pan;
	uint16_t		dst_addr;
	uint16_t		src_pan;
	uint16_t		src_addr;
	uint8_t		data;
} PACKED stm32_pkt16_t;

#define  MAC_ADDR_MODE_NONE		0
#define  MAC_ADDR_MODE_RESERVED	1
#define  MAC_ADDR_MODE_16BIT	2
#define  MAC_ADDR_MODE_64BIT	3

#define STRADIO_DEFAULT_RETRANSMISSION_COUNT	10

#define STRADIO_GET_SRC_ADDR(p)	((uint16_t)(((stm32_pkt16_t *)p)->src_addr))

void radio_init();


/*******************************************************************************
* Function Name  : radio_loop
* Description    : When Radio is idle then checks for packets to be transmitted
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void radio_loop();

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
boolean sendPacketData(uint8_t length, sint8_t *data, uint16_t dst);




void sendPriorityPacket(uint8_t length, sint8_t *data, uint16_t dst);



// Handles Timeout after transmission attempt
//  - if Tx is not acknowledged/ended then force cleanup
void hPacketTimeout(uint8_t *tidx);



#endif // _STM32W_RADIO_H_
