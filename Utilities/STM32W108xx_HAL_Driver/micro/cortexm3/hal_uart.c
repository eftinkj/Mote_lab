/**
  ******************************************************************************
  * @file    hal_uart.c
  * @author  MCD Application Team
  * @version V2.0.1
  * @date    30-November-2012
  * @brief   STM32W  hal uart drivers, supporting IAR's standard library IO routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include PLATFORM_HEADER
#include "micro-common.h"

#include "hal_uart.h"
#include "hal.h"
#ifdef __GNUC__
#include <sys/stat.h>
#endif /* __GNUC__ */
#ifndef __ICCARM__
#include <stdio.h>
#define _LLIO_STDIN ((int) stdin)
#define _LLIO_STDOUT ((int) stdout)
#define _LLIO_STDERR ((int) stderr)
#define _LLIO_ERROR ((size_t)-1)
#ifndef __GNUC__
#define __write _write
#define __read _read
#endif
#undef putchar
void __io_putchar( char c );
int putchar (int c)
{
  __io_putchar((char) c);
  return c;
}
#endif /* __ICCARM__ */

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef __GNUC__
/* With GCC/RAISONANCE, small scanf calls __io_getchar() */
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* __GNUC__ */

/** @addtogroup HAL
  * @{
  */

/** @defgroup micro common
  * @brief micro common modules
  * @{
  */

#define RECEIVE_QUEUE_SIZE (128)

uint8_t rxQ[RECEIVE_QUEUE_SIZE];
uint16_t rxHead;
uint16_t rxTail;
uint16_t rxUsed;

/** @defgroup  micro_Private_Functions
  * @{
  */

/** @defgroup  UART_Initialization_Functions
  * @{
  */

/**
  * @brief  high level uart configuration.
  * @param  None
  * @retval None
  */
void uartInit(void)
{
  UART_InitTypeDef UART_InitStructure;
  
  /* UARTx configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  */
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_WordLength = UART_WordLength_8b;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_No;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_Disable;
  STM_EVAL_COMInit(COM1, &UART_InitStructure);
  
  /* Enable the SC1_Rx Receive interrupt: this interrupt is generated when the 
  UART receive in data register*/ 
  UART_TriggerEventConfig(SC1_IT, UART_IT_RXNE, SC_TriggerEvent_Level);
  UART_ITConfig(SC1_IT, UART_IT_PE | UART_IT_FE | UART_IT_OVR | UART_IT_RXNE, ENABLE);
  
  
  SC1_IT->ISR = 0xFFFF; /* Clear any stale interrupts */
  NVIC->ISER[0] = NVIC_IxxR_SC1;
}

/**
  * @}
  */

/** @defgroup  UART_Transmit_Functions
  * @{
  */

/**
  * @brief  IAR Standard library hook for serial output
  * @param  handle
  * @param  buffer
  * @param  size buffer size.
  * @retval size_t
  */
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  size_t nChars = 0;

  /* This template only writes to "standard out" and "standard err",
     for all other file handles it returns failure. */
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
    return _LLIO_ERROR;
  }

  if (buffer == 0) {
    /* This means that we should flush internal buffers.  
       spin until TX complete (TX is idle) */
    while ((SC1_UART->UARTSR&SC_UARTSR_IDLE)!=SC_UARTSR_IDLE) {}
    return 0;
  }
  
  /* ensure port is configured for UART */
  if(SC1_UART->CR != SC_Mode_UART) {
    return _LLIO_ERROR;
  }

  while(size--) {
    /* spin until data register has room for more data */
    while ((SC1_UART->UARTSR&SC_UARTSR_TXE)!=SC_UARTSR_TXE) {}
    SC1_UART->DR = *buffer;
    buffer++;

    ++nChars;
  }

  return nChars;
}

/**
  * @brief flush file
  * @param file
  * @retval int
  */
#if (defined (__GNUC__))
int fflush (FILE *f)
 #elif (defined (__CC_ARM))
 int fflush (FILE *f)
#elif (defined (__ICCARM__))
size_t fflush(int handle)
#else
#error "Add fflash for the appropriate compiler"
#endif
{
  return __write(_LLIO_STDOUT, NULL, 0);
}
/**
  * @brief UART Tx handler
  * @param None
  * @retval None
  */
static void halInternalUart1TxIsr(void)
{
  /* Nothing for now, as ouput is blocking from the __write function */
}

/**
  * @brief IO putchar prototype
  * @param c char to transmit
  * @retval None
  */
void __io_putchar( char c )
{
  __write(_LLIO_STDOUT, (unsigned char *)&c, 1);
}

/**
  * @brief IO flush
  * @param None
  * @retval None
  */
void __io_flush( void )
{
  __write(_LLIO_STDOUT, NULL, 0);
}

/**
  * @}
  */

/** @defgroup  UART_Receive_Functions
  * @{
  */

/**
  * @brief  IAR Standard library hook for serial input
  * @param  handle
  * @param  buffer
  * @param  size buffer size.
  * @retval size_t
  */
size_t __read(int handle, unsigned char * buffer, size_t size)
{
  int nChars = 0;

  /* This template only reads from "standard in", for all other file
     handles it returns failure. */
  if (handle != _LLIO_STDIN)
  {
    return _LLIO_ERROR;
  }

  for(nChars = 0; (rxUsed>0) && (nChars < size); nChars++) {
    ATOMIC(
      *buffer++ = rxQ[rxTail];
      rxTail = (rxTail+1) % RECEIVE_QUEUE_SIZE;
      rxUsed--;
    )
  }

  return nChars;
}

/**
  * @brief UART Rx handler
  * @param None
  * @retval None
  */
static void halInternalUart1RxIsr(void)
{
  /* At present we really don't care which interrupt(s)
     occurred, just that one did.  Loop reading RXVALID
     data, processing any errors noted
     along the way. */
  while ( SC1_UART->UARTSR & SC_UARTSR_RXNE ) {
    uint8_t errors = SC1_UART->UARTSR & (SC_UARTSR_FE |
                                   SC_UARTSR_OVR  |
                                   SC_UARTSR_PE );
    uint8_t incoming = (uint8_t) SC1_UART->DR;

    if ( (errors == 0) && (rxUsed < (RECEIVE_QUEUE_SIZE-1)) ) {
      rxQ[rxHead] = incoming;
      rxHead = (rxHead+1) % RECEIVE_QUEUE_SIZE;
      rxUsed++;
    } else {
      /* IAR standard library doesn't allow for any error handling in the 
         case of rx errors or not having space in the receive queue, so the
         errors are just silently dropped.*/
    }
  } /* end of while ( SC1_UART->UARTSR & SC1_UARTRXVAL ) */
}

/**
  * @brief Non blocking read
  * @param dataByte: buffer containing the read byte if any
  * @retval TRUE if there is a data, FALSE otherwise
  */
boolean __io_getcharNonBlocking(uint8_t *data)
{
  if (__read(_LLIO_STDIN,data,1))
    return TRUE;
  else
    return FALSE;
}/* end serialReadByte() */

/**
  * @brief IO getchar prototype 
  * @param None
  * @retval int: data received
  */
int __io_getchar()
{
  unsigned char c = 0;
  __read(_LLIO_STDIN, &c, 1);
  return (int)(c);
}

/**
  * @}
  */

/** @defgroup  UART_Interrupts_Functions
  * @{
  */

/**
  * @brief SC1 IRQ handler
  * @param None
  * @retval None
  */
void halSc1Isr(void)
{
  uint32_t interrupt;

  /* this read and mask is performed in two steps otherwise the compiler
     will complain about undefined order of __IO access */
  interrupt = SC1_IT->ISR;
  interrupt &= SC1_IT->IER;
  
  while (interrupt != 0) {
  
    SC1_IT->ISR = interrupt; /* acknowledge the interrupts early */

    /* RX events */
    if ( interrupt & (SC_IER_RXNEIE   | /* RX has data */
                      SC_IER_OVRIE   | /* RX Overrun error */
                      SC_IER_FEIE | /* RX Frame error */
                      SC_IER_PEIE ) /* RX Parity error */
       ) {
      halInternalUart1RxIsr();
    }
    
    /* TX events */
    if ( interrupt & (SC_IER_TXEIE | /* TX has room */
                      SC_IER_IDLEIE ) /* TX idle (more room) */
       ) {
      halInternalUart1TxIsr();
    }

    interrupt = SC1_IT->ISR;
    interrupt &= SC1_IT->IER;
  }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
