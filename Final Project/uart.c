#include "FreeRTOS.h"
#include "queue.h"

#include "uart.h"
#include "gpio.h"

#define UART_SENSOR_COMMAND_TEMPERATURE 0x50
#define UART_SENSOR_COMMAND_DISTANCE 0x55

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
void USART1_IRQHandler(void);

// Interrupt handler for USART3.
void USART3_IRQHandler(void);

//////////////////// Private global variables //////////////////////////////////

// A queue that contains all the chars to transmit to the PC
static QueueHandle_t queuePcMessage;

enum uartSensorStateEnum {
	IDLE,
	WAITING_FOR_DISTANCE_FIRST_BYTE,
	WAITING_FOR_DISTANCE_SECOND_BYTE
};
typedef enum uartSensorStateEnum uartSensorState_t;
static uartSensorState_t uart1SensorState;
static uartSensorState_t uart3SensorState;

//////////////////// Public Function Bodies ////////////////////////////////////

// Initialize the UART connected to the PC, including any pin configuration.
void uartPcInitialize(void) {
	
	// Initialize the queues.
	queuePcMessage 			= xQueueCreate(PC_MESSAGE_MAX_LENGTH, sizeof(char));
	queueUartNote 			= xQueueCreate(1, sizeof(char));
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
	USART2->CR1 |= USART_CR1_RXNEIE; 		// Enables the Receive Register Not Empty interrupt.
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

// Initialize the USART3 connected to the ultrasonic sensor, including any pin configuration.
// TX is on PB10, RX is on PB11.
void uartSensor1Initialize(void) {

	uartSensorState = IDLE;
	
	// Initialize the queues
	queueUartSensorTemperature = xQueueCreate(1, sizeof(uint8_t));
	queueUartSensorDistance = xQueueCreate(1, sizeof(uint16_t));
	
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

// Initialize the USART1 connected to the ultrasonic sensor, including any pin configuration.
// TX is on PA9, RX is on PA10.
void uartSensor2Initialize(void) {

	uartSensorState = IDLE;
	
	// Initialize the queues
	queueUartSensorTemperature = xQueueCreate(1, sizeof(uint8_t));
	queueUartSensorDistance = xQueueCreate(1, sizeof(uint16_t));
	
	// Configure the USART1 I/O.
	pinMode(GPIOA, 9, SPECIAL);
	pinMode(GPIOA, 10, SPECIAL);
	setOutputType(GPIOA, 9, PUSHPULL);
	setOutputType(GPIOA, 10, PUSHPULL);
	setSpeed(GPIOA, 9, VERYHIGHSPEED);
	setSpeed(GPIOA, 10, VERYHIGHSPEED);
	setPullUpDown(GPIOA, 9, PULLUP);
	setPullUpDown(GPIOA, 10, PULLUP);
	GPIOA->AFR[1] |= 0x77 << 4;
	
	// Enables the USART1 peripheral clock.
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART1EN;
	
	// Disables the USART so the control registers can be set.
	USART1->CR1 &= ~USART_CR1_UE;
	
	// Configures Control Register 1.
	USART1->CR1 = 0;
	USART1->CR1 |= USART_CR1_RXNEIE; 		// Enables the Receive Register Not Empty interrupt.
	USART1->CR1 |= USART_CR1_TE;			// Enables the transmitter.
	USART1->CR1 |= USART_CR1_RE;			// Enables the receiver.
	
	// Configures the Control Register 2.
	// No bits to set in CR2.
	
	// Configures the Control Register 3.
	// No bits to set the CR3.
	
	// Configures the baud rate.
	USART1->BRR = 1667; // Sets a Baud rate of 9600 (assuming a clock rate of 16MHz.
	
	// Enables the USART.
	USART1->CR1 |= USART_CR1_UE;
	
	// Enables USART3 interrupts.
	NVIC_EnableIRQ(USART1_IRQn);
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

// Request a distance reading from the uart sensor
void uartSensorRequestDistance(USART_TypeDef *USARTx) {
	if (USARTx == USART1) {
		uart1SensorState = WAITING_FOR_DISTANCE_FIRST_BYTE;
	}
	else if(USARTx == USART3) {
		uart3SensorState = WAITING_FOR_DISTANCE_FIRST_BYTE;
	}
	USARTx->TDR = UART_SENSOR_COMMAND_DISTANCE;
}

//////////////////// Private Function Bodies ///////////////////////////////////

/*
Queue not set up, USART1 should handle the volume of the tone by sending the volume to the DACManagerTask in main.c
It should also pass the volume/distance into the SendVolumeMessage task in main.c
I think it makes sense for volume to be highest when you're close to the sensor and lowest when you're far away
This makes it so if you're not intentionally blocking the sensor, the tone will shut off
*/
void USART1_IRQHandler(void) {

	static uint16_t distance = 0;

	// Determine the type of interrupt.
	volatile unsigned int interrupt_status = USART1->ISR;
	
	// Data has been received over the UART.
	if ((interrupt_status & USART_ISR_RXNE) != 0) {
	
		// Receive the data.
		volatile unsigned int receivedData = (char)(USART1->RDR & USART_RDR_RDR);
		
		switch (uart1SensorState) {
			case WAITING_FOR_DISTANCE_FIRST_BYTE:
				distance = (uint16_t)(receivedData << 8);
				uart1SensorState = WAITING_FOR_DISTANCE_SECOND_BYTE;
				break;
			case WAITING_FOR_DISTANCE_SECOND_BYTE:
				distance |= (uint16_t)(receivedData);
				uart1SensorState = IDLE;
				xQueueSendToBackFromISR(queueUartSensorDistance, &distance, NULL);
				break;
			case IDLE:
				return;
		}
	}
}

/*
Queue not set up, USART3 should handle the pitch of the note by directly modifying the TIM4 ARR
It should also pass the note/distance into the SendNoteMessage task in main.c
High pitch equals far away, low pitch equals close. We did some sort of equation with the highest note being some 610 mm away (2 feet)
with 20 mm increments per note. Something like | note_index = (610-SensorValue)/20 |

*/

void USART3_IRQHandler(void) {

	static uint16_t distance = 0;

	// Determine the type of interrupt.
	volatile unsigned int interrupt_status = USART3->ISR;
	
	// Data has been received over the UART.
	if ((interrupt_status & USART_ISR_RXNE) != 0) {
	
		// Receive the data.
		volatile unsigned int receivedData = (char)(USART3->RDR & USART_RDR_RDR);
		
		switch (uart3SensorState) {
			case WAITING_FOR_DISTANCE_FIRST_BYTE:
				distance = (uint16_t)(receivedData << 8);
				uart3SensorState = WAITING_FOR_DISTANCE_SECOND_BYTE;
				break;
			case WAITING_FOR_DISTANCE_SECOND_BYTE:
				distance |= (uint16_t)(receivedData);
				uart3SensorState = IDLE;
				xQueueSendToBackFromISR(queueUartSensorDistance, &distance, NULL);
				break;
			case IDLE:
				return;
		}
	}
}
