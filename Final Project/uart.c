#include "FreeRTOS.h"
#include "queue.h"

#include "uart.h"
#include "gpio.h"

//////////////////// Private function prototypes ///////////////////////////////

// Interrupt handler for USART1.
void USART1_IRQHandler(void);

// Interrupt handler for USART3.
void USART3_IRQHandler(void);

//////////////////// Private types /////////////////////////////////////////////

//////////////////// Private global variables //////////////////////////////////

// A queue that contains all the chars to transmit to the PC
static QueueHandle_t queuePcMessage;

static QueueHandle_t queuePcMessage;

//////////////////// Public Function Bodies ////////////////////////////////////

void uartInitialize(uart_t *uart) {
	
	pinMode(uart->port, uart->pinRx, SPECIAL);
	pinMode(uart->port, uart->pinTx, SPECIAL);
	setOutputType(uart->port, uart->pinRx, PUSHPULL);
	setOutputType(uart->port, uart->pinTx, PUSHPULL);
	setSpeed(uart->port, uart->pinRx, VERYHIGHSPEED);
	setSpeed(uart->port, uart->pinTx, VERYHIGHSPEED);
	setPullUpDown(uart->port, uart->pinRx, PULLUP);
	setPullUpDown(uart->port, uart->pinTx, PULLUP);
	setAlternateFunction(uart->port, uart->pinRx, 0x7u);
	setAlternateFunction(uart->port, uart->pinTx, 0x7u);
	
	// Enables the USART's peripheral clock.
	*(uart->apbEnr) |= uart->apbEnrBit;
	
	// Disables the USART so the control registers can be set.
	uart->usart->CR1 &= ~USART_CR1_UE;
	
	// Configures Control Register 1.
	uart->usart->CR1 = 0;
	if (uart->rxEnable) {
		//uart->usart->CR1 |= USART_CR1_RXNEIE;
		uart->usart->CR1 |= USART_CR1_RE;
	}
	if (uart->txEnable) {
		//uart->usart->CR1 |= USART_CR1_TXEIE;
		uart->usart->CR1 |= USART_CR1_TE;
	}
	
	// Configures the Control Register 2.
	// No bits to set in CR2.
	
	// Configures the Control Register 3.
	// No bits to set the CR3.
	
	// Configures the baud rate.
	uart->usart->BRR = 1667; // Sets a Baud rate of 9600 (assuming a clock rate of 16MHz.
	
	// Enables the USART.
	uart->usart->CR1 |= USART_CR1_UE;
	
	// Enables USART interrupts.
	if (uart->rxEnable || uart->txEnable) {
		NVIC_EnableIRQ(uart->irq);
	}
	
}

void uartTransmitCharacter(uart_t *uart, uartCharacter_t character) {
	uart->usart->TDR = character;
}

/*
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
*/

/*
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
*/

//////////////////// Private Function Bodies ///////////////////////////////////

/*
Queue not set up, USART1 should handle the volume of the tone by sending the volume to the DACManagerTask in main.c
It should also pass the volume/distance into the SendVolumeMessage task in main.c
I think it makes sense for volume to be highest when you're close to the sensor and lowest when you're far away
This makes it so if you're not intentionally blocking the sensor, the tone will shut off
*/
/*
void USART1_IRQHandler(void) {

	static sensorDistance_t distance = 0;
	static volume_t volume = 0;

	// Determine the type of interrupt.
	volatile unsigned int interrupt_status = USART1->ISR;
	
	// Data has been received over the UART.
	if ((interrupt_status & USART_ISR_RXNE) != 0) {
	
		// Receive the data.
		volatile unsigned int receivedData = (char)(USART1->RDR & USART_RDR_RDR);
		
		switch (uart1SensorState) {
			case WAITING_FOR_DISTANCE_FIRST_BYTE:
				distance = (sensorDistance_t)(receivedData << 8);
				uart1SensorState = WAITING_FOR_DISTANCE_SECOND_BYTE;
				break;
			case WAITING_FOR_DISTANCE_SECOND_BYTE:
				distance |= (sensorDistance_t)(receivedData);
				uart1SensorState = IDLE;
				if (distance > 600) distance = 600; // 600 mm is 2 feet (the max distance we want to measure
				volume = (volume_t)((600-distance)/75);
				xQueueOverwriteFromISR(mailboxVolume, &volume, NULL);
				break;
			case IDLE:
				return;
		}
	}
}

void USART2_IRQHandler(void) {
	volatile unsigned int interrupt_status = USART2->ISR;
	if((interrupt_status & USART_ISR_TXE) != 0) {
		char data;
		BaseType_t characterRemovedFromQueue;
		characterRemovedFromQueue = xQueueReceiveFromISR(queuePcMessage, &data, NULL);
		if (characterRemovedFromQueue == pdTRUE) {
			USART2->TDR = data;
		}
		else {
			USART2->CR1 &= ~USART_CR1_TXEIE;
		}
	}
}
*/

/*
Queue not set up, USART3 should handle the pitch of the note by directly modifying the TIM4 ARR
It should also pass the note/distance into the SendNoteMessage task in main.c
High pitch equals far away, low pitch equals close. We did some sort of equation with the highest note being some 610 mm away (2 feet)
with 20 mm increments per note. Something like | note_index = (610-SensorValue)/20 |

*/

/*
void USART3_IRQHandler(void) {

	static uint16_t distance = 0;
	static uint8_t note_index = 0;
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
				if (distance > 610) distance = 610; // 610 mm is 2 feet (the max distance we want to measure
				note_index = (uint8_t)((610 - distance) / 20);
				xQueueSendToFrontFromISR(mailboxNote, &note_index, NULL);
				break;
			case IDLE:
				return;
		}
	}
}
*/
