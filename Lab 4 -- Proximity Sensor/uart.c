#include "FreeRTOS.h"
#include "queue.h"

#include "uart.h"
#include "gpio.h"

#define UART_SENSOR_COMMAND_TEMPERATURE 0x50
#define UART_SENSOR_COMMAND_DISTANCE		0x55

//////////////////// Public global variables ///////////////////////////////////

// Note names, in the form of chars, go in this queue.
QueueHandle_t queueUartNote;

// t and p go in this queue.
QueueHandle_t queueUartSensorCommand;

// The raw distance values received from the sensor go in this queue.
QueueHandle_t queueUartSensorDistance;

// The raw temperature values received from the sensor go in this queue.
QueueHandle_t queueUartSensorTemperature;

//////////////////// Private function prototypes ///////////////////////////////

// Interrupt handler for USART2.
void USART2_IRQHandler(void);

// Interrupt handler for USART3.
void USART3_IRQHandler(void);

//////////////////// Private global variables //////////////////////////////////

// A queue that contains all the chars to transmit to the PC
static QueueHandle_t queuePcMessage;

enum uartSensorStateEnum {
	IDLE,
	WAITING_FOR_DISTANCE_FIRST_BYTE,
	WAITING_FOR_DISTANCE_SECOND_BYTE,
	WAITING_FOR_TEMPERATURE
};
typedef enum uartSensorStateEnum uartSensorState_t;
static uartSensorState_t uartSensorState;

//////////////////// Public Function Bodies ////////////////////////////////////

// Initialize the UART connected to the PC, including any pin configuration.
void uartPcInitialize(void) {
	
	// Initialize the queues.
	queuePcMessage 					= xQueueCreate(PC_MESSAGE_MAX_LENGTH, sizeof(char));
	queueUartNote 					= xQueueCreate(1, sizeof(char));
	queueUartSensorCommand 	= xQueueCreate(1, sizeof(char));

	// Configure the USART2 I/O.
	pinMode(GPIOA, 2, SPECIAL);
	pinMode(GPIOA, 3, SPECIAL);
	setOutputType(GPIOA, 2, PUSHPULL);
	setOutputType(GPIOA, 3, PUSHPULL);
	setSpeed(GPIOA, 2, VERYHIGHSPEED);
	setSpeed(GPIOA, 3, VERYHIGHSPEED);
	setPullUpDown(GPIOA, 2, PULLUP);
	setPullUpDown(GPIOA, 3, PULLUP);
	GPIOA->AFR[0] |= 0x77 << 8;
	
	// Enables the USART2 peripheral clock.
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	
	// Disables the USART so the control registers can be set.
	USART2->CR1 &= ~USART_CR1_UE;
	
	// Configures Control Register 1.
	USART2->CR1 = 0;
	USART2->CR1 |= USART_CR1_RXNEIE; 	// Enables the Receive Register Not Empty interrupt.
	USART2->CR1 |= USART_CR1_TE;			// Enables the transmitter.
	USART2->CR1 |= USART_CR1_RE;			// Enables the receiver.
	
	// Configures the Control Register 2.
	// No bits to set in CR2.
	
	// Configures the Control Register 3.
	// Not bits to set the CR3.
	
	// Configures the baud rate.
	USART2->BRR = 1667; // Sets a Baud rate of 9600 (assuming a clock rate of 16MHz.
	
	// Enables the USART.
	USART2->CR1 |= USART_CR1_UE;
	
	// Enables USART2 interrupts.
	NVIC_EnableIRQ(USART2_IRQn);

}

// Initialize the UART connected to the ultrasonic sensor, including any pin configuration.
void uartSensorInitialize(void) {

	uartSensorState = IDLE;
	
	// Initialize the queues
	queueUartSensorTemperature = xQueueCreate(1, sizeof(uint16_t));
	queueUartSensorDistance = xQueueCreate(1, sizeof(uint8_t));
	
	// Configure the USART3 I/O.
	pinMode(GPIOB, 10, SPECIAL);
	pinMode(GPIOB, 11, SPECIAL);
	setOutputType(GPIOB, 10, PUSHPULL);
	setOutputType(GPIOB, 11, PUSHPULL);
	setSpeed(GPIOB, 10, VERYHIGHSPEED);
	setSpeed(GPIOB, 11, VERYHIGHSPEED);
	setPullUpDown(GPIOB, 10, PULLUP);
	setPullUpDown(GPIOB, 11, PULLUP);
	GPIOB->AFR[1] |= 0x77 << 8;
	
	// Enables the USART3 peripheral clock.
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
	
	// Disables the USART so the control registers can be set.
	USART3->CR1 &= ~USART_CR1_UE;
	
	// Configures Control Register 1.
	USART3->CR1 = 0;
	USART3->CR1 |= USART_CR1_RXNEIE; 	// Enables the Receive Register Not Empty interrupt.
	USART3->CR1 |= USART_CR1_TE;			// Enables the transmitter.
	USART3->CR1 |= USART_CR1_RE;			// Enables the receiver.
	
	// Configures the Control Register 2.
	// No bits to set in CR2.
	
	// Configures the Control Register 3.
	// No bits to set the CR3.
	
	// Configures the baud rate.
	USART3->BRR = 1667; // Sets a Baud rate of 9600 (assuming a clock rate of 16MHz.
	
	// Enables the USART.
	USART3->CR1 |= USART_CR1_UE;
	
	// Enables USART3 interrupts.
	NVIC_EnableIRQ(USART3_IRQn);

}

// Transmit a message of arbitrary length to the connected PC.
void uartPcTransmit(const char *data, int length) {

	BaseType_t characterAddedToQueue;
	int i;
	for (i = 0; i < length; i++) {
		characterAddedToQueue = xQueueSendToBack(queuePcMessage, data + i, 0);
		if (characterAddedToQueue != pdTRUE) {
			break;
		}
	}
	// Enable the interrupt.
	USART2->CR1 |= USART_CR1_TXEIE;
	
}

// Request a temperature reading from the uart sensor
void uartSensorRequestTemperature(void) {
	
	// Nothing can be requested if we're already waiting for something back.
	/*if (uartSensorState != IDLE) {
		return;
	}*/
	
	uartSensorState = WAITING_FOR_TEMPERATURE;
	
	// Transmit the byte over USART3.
	USART3->TDR = UART_SENSOR_COMMAND_TEMPERATURE;
	
}

// Request a distance reading from the uart sensor
void uartSensorRequestDistance(void) {

	// Nothing can be requested if we're already waiting for something back.
	/*if (uartSensorState != IDLE) {
		return;
	}*/
	
	uartSensorState = WAITING_FOR_DISTANCE_FIRST_BYTE;
	
	// Transmit the byte over USART3.
	USART3->TDR = UART_SENSOR_COMMAND_DISTANCE;

}

//////////////////// Private Function Bodies ///////////////////////////////////

// Interrupt handler for USART2.
void USART2_IRQHandler(void) {
	
	// Determine the type of interrupt.
	volatile unsigned int interrupt_status = USART2->ISR;
	
	// Data has been received over the UART.
	if ((interrupt_status & USART_ISR_RXNE) != 0) {
		
		// Receive the data.
		char receivedData = (char)(USART2->RDR & USART_RDR_RDR);
		
		// Determine which queue the needs to go to.
		switch (receivedData) {
			case 'a':
			case 'A':
			case 'b':
			case 'B':
			case 'c':
			case 'C':
			case 'd':
			case 'D':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
			case 'h':
			case 'H':
				// Send it to the note queue.
				xQueueSendToBackFromISR(queueUartNote, &receivedData, NULL);
				break;
			case 't':
			case 'T':
			case 'p':
			case 'P':
				// Send it to the sensor command queue.
				xQueueSendToBackFromISR(queueUartSensorCommand, &receivedData, NULL);
				break;
		}
	}
	
	// Next bit is ready to transmit.
	else if ((interrupt_status & USART_ISR_TXE) != 0) {
		// This interrupt will only be raised if a message is currently being transmitted.
		
		// Retrieve the next character to transmit.
		char nextCharacter;
		BaseType_t nextCharacterWasReceived;
		nextCharacterWasReceived = xQueueReceiveFromISR(queuePcMessage, &nextCharacter, NULL);
		if (nextCharacterWasReceived != pdTRUE) {
			// If there are no more characters to transmit, disable the interrupt.
			// The interrupt will be enabled again when a new message is ready.
			USART2->CR1 &= ~USART_CR1_TXEIE;
		} else {
			USART2->TDR = nextCharacter;
		}
	}

}

void USART3_IRQHandler(void) {
	
	static uint16_t distance = 0;

	// Determine the type of interrupt.
	volatile unsigned int interrupt_status = USART3->ISR;
	
	// Data has been received over the UART.
	if ((interrupt_status & USART_ISR_RXNE) != 0) {
	
		// Receive the data.
		char receivedData = (char)(USART3->RDR & USART_RDR_RDR);
		
		switch (uartSensorState) {
			case WAITING_FOR_TEMPERATURE:
				xQueueSendToBackFromISR(queueUartSensorTemperature, &receivedData, NULL);
				uartSensorState = IDLE;
				break;
			case WAITING_FOR_DISTANCE_FIRST_BYTE:
				distance = receivedData << 8;
				uartSensorState = WAITING_FOR_DISTANCE_SECOND_BYTE;
				break;
			case WAITING_FOR_DISTANCE_SECOND_BYTE:
				distance |= receivedData;
				uartSensorState = IDLE;
				xQueueSendToBackFromISR(queueUartSensorDistance, &distance, NULL);
				break;
			case IDLE:
				return;
		}
	}

}
