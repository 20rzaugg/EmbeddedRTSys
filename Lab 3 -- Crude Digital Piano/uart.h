#ifndef _UART_H
#define _UART_H

#include "stm32l476xx.h"

// Public function prototypes

// Initializes the UART peripheral according to the settings in uart.h
void uart_initialize(void);

// Transmits a byte over the UART.
void uart_transmit(const char *data);
	
// Sets the callback for when a byte of data is received.
void uart_set_callback_data_received(void (*callback)(char));

#endif
