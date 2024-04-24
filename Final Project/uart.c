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

//////////////////// Private Function Bodies ///////////////////////////////////

/*
Queue not set up, USART3 should handle the pitch of the note by directly modifying the TIM4 ARR
It should also pass the note/distance into the SendNoteMessage task in main.c
High pitch equals far away, low pitch equals close. We did some sort of equation with the highest note being some 610 mm away (2 feet)
with 20 mm increments per note. Something like | note_index = (610-SensorValue)/20 |

*/

