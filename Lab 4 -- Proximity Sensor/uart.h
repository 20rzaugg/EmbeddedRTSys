#ifndef _UART_H
#define _UART_H

#include "stm32l476xx.h"

#include "FreeRTOS.h"
#include "queue.h"

// Public defines
#define PC_MESSAGE_MAX_LENGTH (64)

// Public global variables

// Note names, in the form of chars, go in this queue.
extern QueueHandle_t queueUartNote;

// t and p go in this queue.
extern QueueHandle_t queueUartSensorCommand;

// Public function prototypes

// Initialize the UART connected to the PC, including any pin configuration.
void uartPcInitialize(void);

// Initialize the UART connected to the ultrasonic sensor, including any pin configuration.
void uartSensorInitialize(void);

// Transmit a message of arbitrary length to the connected PC.
void uartPcTransmit(const char *data, int length);

// Transmit a message of exactly one byte to the ultrasonic sensor.
void uartSensorTransmit(char data);

#endif
