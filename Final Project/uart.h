#ifndef _UART_H
#define _UART_H

#include "stm32l476xx.h"

#include "FreeRTOS.h"
#include "queue.h"

// Public defines
#define PC_MESSAGE_MAX_LENGTH (64)

// Public global variables

extern QueueHandle_t mailboxVolume;

extern QueueHandle_t mailboxNote;

// Public function prototypes

// Initialize the UART connected to the PC, including any pin configuration.
void uartPcInitialize(void);

// Initialize the UART connected to the ultrasonic sensor, including any pin configuration.
void uartSensor1Initialize(void);
void uartSensor3Initialize(void);

// Transmit a message of arbitrary length to the connected PC.
void uartPcTransmit(const char *data, int length);

// Request a temperature reading from the uart sensor
void uartSensorRequestTemperature(void);

// Request a distance reading from the uart sensor
void uartSensorRequestDistance(USART_TypeDef *USARTx);

#endif
