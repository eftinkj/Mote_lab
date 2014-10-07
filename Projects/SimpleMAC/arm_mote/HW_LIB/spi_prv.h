/****************************************************************************
**
** Copyright (C) 2006-2005 Maciej Zawodniok, James W. Fonda. All rights reserved.
**
** This file is part of the documentation of the UMR Mote Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef _SPI_PRV_H_
#define _SPI_PRV_H_

#include "spi_pub.h"

#ifdef _ENABLE_SPI_MODULE_

/****************************************************************************
**	Constants, definies and typedefs  (PRIVATE)
****************************************************************************/
#define SPI_CLEAR_BUFFER_VALUE 0xCC



#define SPI_BUF_MEM	idata
#define SPI_IDX_MEM	idata

#define SPI_BUFFER_SIZE	16
#define SPI_BUFFER_MASK	SPI_BUFFER_SIZE-1

#define SPI_PREAMBLE_OFF	0x00
/****************************************************************************
**	Variables declaration  (PRIVATE)
****************************************************************************/
/*
extern spi_buf_len_t SPI_IDX_MEM spi_curr_idx;
extern spi_buf_len_t SPI_IDX_MEM spi_tx_idx; // last byte to TX
extern spi_buf_len_t SPI_IDX_MEM spi_rx_idx; // first byte from RX (to be read by app)
*/
extern spi_buf_t SPI_BUF_MEM spi_buffer[SPI_BUFFER_SIZE];

extern spi_state_t SPI_IDX_MEM spi_state;

extern bit spi_tx_tick_; // Watchdog for UART0 interrupt

// Mutex for accessing
extern spi_app_t SPI_IDX_MEM spi_app_mutex;


extern spi_event_t *spi_event_flag;
extern spi_event_t spi_event_flag_mask;
extern rtc_tick_t spi_rtc_timeout;


extern spi_buf_t spi_preamble_char;
extern spi_event_t spi_old_event_mask;
extern uint8_t spi_max_retries;

/****************************************************************************
**	Functions declaration (PRIVATE)
****************************************************************************/

/**
* spi_purge_task( app ) - stops current SPI task (operation)
*	RETURN: 1 (SPI_R_OK) - if request has been fulfilled (SPI is freed by app)
*			0 (SPI_R_FAIL) - if unsucceflul (e.g. used by another app or free)
*/
void spi_purge_task( spi_app_t app );



/**
  * spi_jump_start () - restarts the SPI transmission (Interrupt)
  */
void spi_jump_start ( void );


#endif // _ENABLE_SPI_MODULE_

#endif

