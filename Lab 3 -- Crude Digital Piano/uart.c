#include "uart.h"

#include "stm32l476xx.h"

// Private function prototypes

// Interrupt handler for USART2.
void USART2_IRQHandler(void);

// Configures the I/O used by the USART.
void uart_configure_io(void);

// Private global variables

// A function pointer to the function that handles incoming data
static void (*uart_callback_data_received)(char);

// Initializes the UART peripheral.
void uart_initialize(void) {
	
	// Configures the USART2 I/O.
	uart_configure_io();
	
	// Enables the USART2 peripheral clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	
	// Disables the USART so all the control registers can be set.
	USART2->CR1 &= ~USART_CR1_UE;
	
	// Configures Control Register 1
	USART2->CR1 = 0;
	USART2->CR1 |= USART_CR1_RXNEIE; 	// Enables the Receive Register Not Empty interrupt.
	USART2->CR1 |= USART_CR1_TE;			// Enables the transmitter.
	USART2->CR1 |= USART_CR1_RE;			// Enables the receiver.
	
	// Configures the Control Register 2.
	USART2->CR2 = 0;
	// No bits to set in CR2.
	
	// Configures the Control Register 3.
	USART2->CR3 = 0;
	// No bits to set in CR3.
	
	// Configures the baud rate
	USART2->BRR = 0x833;	// Sets a Baud rate of 9600 (assuming a clock rate of 8MHz)

	// Waits until the transmitter enable has been acknowledged.
	// while((USART2->ISR & USART_ISR_TEACK) == 0);
	
	// Waits until the receiver enable has been acknowledged.
	// while((USART2->ISR & USART_ISR_REACK) == 0);
	
	// Enables the USART
	USART2->CR1 |= USART_CR1_UE;
	
	// Enables USART2 interrupts
	NVIC_EnableIRQ(USART2_IRQn);
	
}

void uart_set_callback_data_received(void (*callback)(char)) {
	uart_callback_data_received = callback;
}

// Transmits a byte over the UART.
void uart_transmit(const char *data) {
	
	// Waits until the USART is ready to transmit.
	while((USART2->ISR & USART_ISR_TXE) == 0);
	
	// Transfers the data to the transmit register
	USART2->TDR = *data | USART_TDR_TDR;
	
}

// Interrupt handler for USART2.
void USART2_IRQHandler(void) {
	
	// Determine the type of interrupt.
	volatile unsigned int interrupt_status = USART2->ISR;
	
	if ((interrupt_status & USART_ISR_RXNE) != 0) {
		// Data has been received over the UART.
		char received_data = (char)(USART2->RDR | USART_RDR_RDR);
		uart_callback_data_received(received_data);
		return;
	}

}

// Configures the I/O used by the USART.
void uart_configure_io(void) {

	// Enable the clock to I/O Bank A.
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// Configure alternate function for pins PA2 (Tx) and PA3 (Rx).
	GPIOA->MODER |= GPIO_MODER_MODE2_1;
	GPIOA->MODER |= GPIO_MODER_MODE3_1;

}
