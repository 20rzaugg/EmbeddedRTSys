#ifndef _UART_H
#define _UART_H

#include <stdbool.h>

#include "stm32l476xx.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

// Public defines


/////////////////// Public Types ///////////////////////////////////////////////

// A character than can be trasnmitted over a UART.
typedef uint8_t uartCharacter_t;

// A struct used to organize information related to a USART peripheral.
struct uartStruct {
	USART_TypeDef			*usart;			// The USART this struct represents.
	GPIO_TypeDef			*port;			// The GPIO port to which this is connected.
	unsigned int			pinRx;			// The index of this USART's RX pin.
	unsigned int			pinTx;			// The index of this USART's TX pin.
	volatile uint32_t	*apbEnr;		// The clock enable register.
	uint32_t					apbEnrBit;	// The bit that should be set in the above.
	IRQn_Type					irq;				// The interrupt vector of this uart.
	bool							rxEnable;		// Whether to enable the RX interrupt.
	bool							txEnable;		// Whether to enable the Tx interrupt.
};
typedef struct uartStruct uart_t;

/////////////////// Public Function Prototypes /////////////////////////////////

// Initialize a USART with the usual settings, including any pin configuration.
void uartInitialize(uart_t *uart);

// Transmit a single character over a UART.
void uartTransmitCharacter(uart_t *uart, uartCharacter_t character); 

#endif
