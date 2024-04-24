#ifndef _SENSOR_H
#define _SENSOR_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "uart.h"

/////////////////// Public Types ///////////////////////////////////////////////

// The state of a sensor.
enum sensorStateEnum {
	IDLE,		// The sensor is not currently waiting for a byte.
	WAITING	// The sensor has received the first byte of a distance measurement
					// and is waiting for the second byte.
};
typedef enum sensorStateEnum sensorState_t;

typedef uint16_t sensorDistance_t;

// A struct used to organize all the information related to a particular sensor.
struct sensorStruct {
	uart_t						uart;							// The UART this sensor is driven by.
	sensorState_t 		state;						// The current state of the sensor.
	SemaphoreHandle_t semaphoreGo;			// A binary semaphore. Should be raised
																			// whenever a distance measurement is
																			// requested.
	QueueHandle_t			mailboxDistance;	// A mailbox containing the last distance
																			// reading of the sensor.
	uartCharacter_t		firstCharacter;		// The first character of the two distance
																			// characters that has been received.
};
typedef struct sensorStruct sensor_t;

/////////////////// Public Function Prototypes /////////////////////////////////

void sensorInitialize(sensor_t *sensor);

void sensorRequestDistance(sensor_t *sensor);

void sensorReceiveCharacter(sensor_t *sensor);

#endif