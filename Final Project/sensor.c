#include "sensor.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "uart.h"

#define SENSOR_COMMAND_DISTANCE 0x55

/////////////////// Public Function Bodies /////////////////////////////////////

void sensorInitialize(sensor_t *sensor) {
	
	uartInitialize(&(sensor->uart));

	sensor->state = IDLE;
	
	sensor->semaphoreGo = 		xSemaphoreCreateBinary();
	
	sensor->mailboxDistance = xQueueCreate(1, sizeof(sensorDistance_t));
	
}

void sensorRequestDistance(sensor_t *sensor) {
	xSemaphoreGive(sensor->semaphoreGo);
	sensor->uart.usart->TDR = SENSOR_COMMAND_DISTANCE;
	sensor->uart.usart->CR1 |= USART_CR1_RXNEIE;
}

void sensorReceiveCharacter(sensor_t *sensor) {
	
	BaseType_t readingRequested;
	
	volatile uartCharacter_t receivedCharacter;
	sensorDistance_t distance;

	switch (sensor->state) {
	
		case IDLE:
			// Check if a reading has been requested from the sensor.
			readingRequested = xSemaphoreTakeFromISR(sensor->semaphoreGo, NULL);
			if (readingRequested == pdFALSE) {
				// The program has gotten into an error state.
			} else {
				// Read in the data
				receivedCharacter = (sensor->uart.usart->RDR & USART_RDR_RDR);
				sensor->firstCharacter = (sensorDistance_t)(receivedCharacter);
				sensor->state = WAITING;
			}
			break;
		
		case WAITING:
			receivedCharacter = (sensor->uart.usart->RDR & USART_RDR_RDR);
			distance = (sensor->firstCharacter << 8) | receivedCharacter;
			xQueueOverwriteFromISR(sensor->mailboxDistance, &distance, NULL);
			xSemaphoreTakeFromISR(sensor->semaphoreGo, NULL);
			sensor->state = IDLE;
			sensor->uart.usart->CR1 &= ~USART_CR1_RXNEIE;
			break;
		
	}

}