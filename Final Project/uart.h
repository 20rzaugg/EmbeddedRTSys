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

// The state of a sensor.
enum sensorStateEnum {
	IDLE,		// The sensor is not currently waiting for a byte.
	WAITING	// The sensor has received the first byte of a distance measurement and
					// is waiting for the second byte.
};
typedef enum sensorStateEnum sensorState_t;

typedef uint8_t sensorDistance_t;

// A struct used to organize all the information related to a particular sensor.
struct sensorStruct {
	uart_t						UARTx;						// The UART this sensor is driven by.
	sensorState_t 		state;						// The current state of the sensor.
	SemaphoreHandle_t semaphoreGo;			// A binary semaphore. Should be raised
																			// whenever a distance measurement is
																			// requested.
	QueueHandle_t			mailboxDistance;	// A mailbox containing the last distance
																			// reading of the sensor.
};
typedef struct sensorStruct sensor_t;

// Public global variables

extern QueueHandle_t mailboxVolume;

extern QueueHandle_t mailboxNote;

// Public function prototypes

// Initialize a USART with the usual settings, including any pin configuration.
void uartInitialize(uart_t *uart);

// Transmit a single character over a UART.
void uartTransmitCharacter(uart_t *uart, uartCharacter_t character); 

// Transmit a message of arbitrary length to the connected PC.
void uartPcTransmit(const char *data, int length);

// Request a temperature reading from the uart sensor
void uartSensorRequestTemperature(void);

// Request a distance reading from the uart sensor
void uartSensorRequestDistance(USART_TypeDef *USARTx);

#endif
