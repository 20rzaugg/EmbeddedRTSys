#include "gpio.h"
#include "tim.h"
#include "dac.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sin.h"
#include "stddef.h" //NULL
#include "queue.h"
#include "uart.h"
#include "sensor.h"

#include <stdio.h>
#include <stdbool.h>

#define PC_MESSAGE_MAX_LENGTH (64)

/////////////////// Types //////////////////////////////////////////////////////

typedef uint8_t volume_t;

/////////////////// External Variables /////////////////////////////////////////

extern const struct notePair note_table[25];

/////////////////// Global Variables ///////////////////////////////////////////

static uart_t uartPc = {
	.usart 			= USART2,
	.port 			= GPIOA,
	.pinRx 			= 3,
	.pinTx 			= 2,
	.apbEnr 		=	&(RCC->APB1ENR1),
	.apbEnrBit 	= RCC_APB1ENR1_USART2EN,
	.irq 				= USART2_IRQn,
	.rxEnable		= false,
	.txEnable		= true,
};

static uart_t uartSensorVolume = {
	.usart 			= USART1,
	.port 			= GPIOA,
	.pinRx 			= 10,
	.pinTx 			= 9,
	.apbEnr 		=	&(RCC->APB2ENR),
	.apbEnrBit 	= RCC_APB2ENR_USART1EN,
	.irq 				= USART1_IRQn,
	.rxEnable		= true,
	.txEnable		= true,
};
static sensor_t sensorVolume;

static uart_t uartSensorPitch = {
	.usart 			= USART3,
	.port 			= GPIOB,
	.pinRx 			= 11,
	.pinTx 			= 10,
	.apbEnr 		=	&(RCC->APB1ENR1),
	.apbEnrBit 	= RCC_APB1ENR1_USART3EN,
	.irq 				= USART3_IRQn,
	.rxEnable		= true,
	.txEnable		= true,
};


/////////////////// Shared Memory Handles //////////////////////////////////////

static QueueHandle_t queuePcMessage;

/////////////////// Task Prototypes ////////////////////////////////////////////

// A period task that polls the ultrasonic sensors. This task does not handle
// receiving or interpreting the data.
void pollSensors_task(void *pvParameters);

// Monitors the theremin's current volume and pitch values. When they change,
// this task reports it to the connected computer over UART.
void sendUartMessages_task(void *pvParameters);

/////////////////// Private Function Prototypes ////////////////////////////////

// Transmits a message over the PC UART.
void transmitPcMessage(const uartCharacter_t *data, int length);

// The PC UART interrupt handler. Responsible for transmitting the next
// character in the message queue.
void USART2_IRQHandler(void);

void USART1_IRQHandler(void);

// The timer interrupt. Runs every time Timer4 times out. Responsible for
// selecting the new value to output to the DAC in order to generate a tone of
// the appropriate pitch and volume.
//void TIM4_IRQHandler(void);

/////////////////// Public Function Bodies /////////////////////////////////////

int main() {
	
	useHSI();
	
	// Create mailboxes
	//mailboxVolume = xQueueCreate(1, sizeof(volume_t));
	//mailboxNote = xQueueCreate(1, sizeof(uint8_t));
	
	queuePcMessage = xQueueCreate(PC_MESSAGE_MAX_LENGTH, sizeof(uartCharacter_t));

	// Initialize peripherals
	uartInitialize(&uartPc);
	
	sensorVolume.uart = uartSensorVolume;
	sensorInitialize(&sensorVolume);
	
	
	// Start Timer4
	//enableTimer(TIM4, 1, 70, UPCOUNT, 1);
	//TIM4->CR1 |= TIM_CR1_ARPE;

	// Initialize DAC
	//DACinit_ch1(DAC_NORMAL_BUFFER_EXTERNAL, DAC_TRIGGER_NONE);

	// Start tasks
	BaseType_t t1 = xTaskCreate(pollSensors_task, "pollSensors", 256, NULL, 1, NULL);
	if (t1 != pdPASS) {
		while(1);
	}
	
	BaseType_t t2 = xTaskCreate(sendUartMessages_task, "sendUSART", 256, NULL, 1, NULL);
	if (t2 != pdPASS) {
		while(1);
	}
	
	// Send hello world message
	uartCharacter_t *helloWorld = "Hello World.\n\r";
	transmitPcMessage(helloWorld, 15);
	
	sensorRequestDistance(&sensorVolume);
	
	vTaskStartScheduler();
	
	while(1);
}

/////////////////// Task Bodies ////////////////////////////////////////////////

void pollSensors_task(void *pvParameters) {
	while(1) {
		sensorRequestDistance(&sensorVolume);
		// sensorRequestDistance(&sensorPitch);
		vTaskDelay(200);
	}
}

void sendUartMessages_task(void *pvParameters) {
	static uint8_t note_index = 0;
	static volume_t volume = 0;
	static uint8_t new_note_index;
	static volume_t new_volume;
	static char* noteformat = "%s\n\r";
	static char* volformat = "Vol %u\n\r";
	static char message[PC_MESSAGE_MAX_LENGTH];

	while(1) {
		//xQueuePeek(mailboxNote, &new_note_index, portMAX_DELAY);

		/*
		if (new_note_index != note_index) {
			note_index = new_note_index;
			int messageLength = snprintf(message, PC_MESSAGE_MAX_LENGTH, noteformat, note_table[note_index].name);
			uartPcTransmit(message, messageLength);
		}
		*/
		
		xQueuePeek(sensorVolume.mailboxDistance, &new_volume, portMAX_DELAY);
		if (new_volume != volume) {
			volume = new_volume;
			int messageLength = snprintf(message, PC_MESSAGE_MAX_LENGTH, volformat, volume);
			transmitPcMessage(message, messageLength);
		}
		
		vTaskDelay(200);
	}
}

/////////////////// Private Function Bodies ////////////////////////////////////

void transmitPcMessage(const uartCharacter_t *data, int length) {

	BaseType_t characterAddedToQueue;
	int i;
	for (i = 0; i < length; i++) {
		characterAddedToQueue = xQueueSendToBack(queuePcMessage, data + i, 0);
		if (characterAddedToQueue != pdTRUE) {
			break;
		}
	}
	// Enable the interrupt.
	uartPc.usart->CR1 |= USART_CR1_TXEIE;

}

void USART2_IRQHandler(void) {
	volatile unsigned int interruptStatus = uartPc.usart->ISR;
	if ((interruptStatus & USART_ISR_TXE) != 0) {
		uartCharacter_t data;
		BaseType_t characterRemovedFromQueue;
		characterRemovedFromQueue = xQueueReceiveFromISR(queuePcMessage, &data, NULL);
		if (characterRemovedFromQueue == pdTRUE) {
			uartTransmitCharacter(&uartPc, data);
		} else {
			uartPc.usart->CR1 &= ~USART_CR1_TXEIE;
		}
	}
}

void USART1_IRQHandler(void) {

	// Determine the type of interrupt.
	volatile unsigned int interrupt_status = USART1->ISR;
	if ((interrupt_status & USART_ISR_RXNE) != 0) {
		sensorReceiveCharacter(&sensorVolume);
	}
	
}

/*
// Sends notification to the DACManager to move the DAC to the next SIN value
void TIM4_IRQHandler(void) {
	
	static uint8_t note_index = 0;
	static uint8_t volume = 0;
	
	static uint8_t sine_index1 = 0;
	//static uint8_t sine_index2 = 0;
	//static uint8_t sine_index4 = 0;
	static uint8_t sine_index8 = 0;
	static uint8_t sine_index16 = 0;
	uartSensorRequestDistance(USART3);
	uartSensorRequestDistance(USART1);
	xQueuePeekFromISR(mailboxNote, &note_index);
	xQueuePeekFromISR(mailboxVolume, &volume);

	sine_index1 = (sine_index1 + 1) % 64;

	//wanted to test different frequency combinations to make a cool sound
	
	//sine_index2 = (sine_index2 + 2) % 64; // high frequency component of the sound
	//sine_index4 = (sine_index4 + 4) % 64;
	sine_index8 = (sine_index8 + 8) % 64;
	sine_index16 = (sine_index16 + 16) % 64;
	//uint8_t lut_value = (uint8_t)(((sinLUT[sine_index1] + sinLUT[sine_index2] + sinLUT[sine_index4] + sinLUT[sine_index8] + sinLUT[sine_index16])) / 5.0);
	uint8_t lut_value = (uint8_t)(((sinLUT[sine_index1] + sinLUT[sine_index8] + sinLUT[sine_index16])*volume) / 24.0); //3x8 (3 sine indexes, 8 volumes)
	DAC1->DHR12R1 = (lut_value + 50u) & 0xfff;
	
	TIM4->ARR = note_table[note_index].pitch;
	
	TIM4->SR &= ~TIM_SR_UIF;
}
*/