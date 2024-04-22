#include "gpio.h"
#include "tim.h"
#include "dac.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sin.h"
#include "stddef.h" //NULL
#include "queue.h"
#include "uart.h"

#include <stdio.h>

// Static variables

QueueHandle_t mailboxVolume;
QueueHandle_t mailboxNote;

extern const struct notePair note_table[25];
static const uint16_t tonePitchInitial = 284;

// Private function prototypes

void issueSensorCommand(void *pvParameters);
void sendUSARTMessages(void *pvParameters);

// Private function bodies

void pollSensors_task(void *pvParameters) {
	while(1) {
		uartSensorRequestDistance(USART3);
		uartSensorRequestDistance(USART1);
		vTaskDelay(200);
	}
}

void sendUSARTMessages_task(void *pvParameters) {
	static uint8_t note_index = 0;
	static uint8_t volume = 0;
	static uint8_t new_note_index;
	static uint8_t new_volume;
	static char* noteformat = "%s\n\r";
	static char* volformat = "Vol %u\n\r";
	static char message[PC_MESSAGE_MAX_LENGTH];

	while(1) {
		xQueuePeek(mailboxNote, &new_note_index, portMAX_DELAY);

		if(new_note_index != note_index) {
			note_index = new_note_index;
			int messageLength = snprintf(message, PC_MESSAGE_MAX_LENGTH, noteformat, note_table[note_index].name);
			uartPcTransmit(message, messageLength);
		}
		xQueuePeek(mailboxVolume, &new_volume, portMAX_DELAY);

		if(new_volume != volume) {
			volume = new_volume;
			int messageLength = snprintf(message, PC_MESSAGE_MAX_LENGTH, volformat, volume);
			uartPcTransmit(message, messageLength);
		}
	}
}

// Sends notification to the DACManager to move the DAC to the next SIN value
void TIM4_IRQHandler() {
	
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

// I've barely touched main, it's mostly the same as Lab 4
int main() {
	
	useHSI();
	
	mailboxVolume = xQueueCreate(1, sizeof(uint8_t));
	mailboxNote = xQueueCreate(1, sizeof(uint8_t));

	uartPcInitialize();
	uartSensor1Initialize();
	uartSensor3Initialize();
	
	enableTimer(TIM4, 1, 70, UPCOUNT, 1);
	TIM4->CR1 |= TIM_CR1_ARPE;

	DACinit_ch1(DAC_NORMAL_BUFFER_EXTERNAL, DAC_TRIGGER_NONE);

	BaseType_t t1 = xTaskCreate(pollSensors_task, "pollSensors", 256, NULL, 1, NULL);
	if (t1 != pdPASS) {
		while(1);
	}
	BaseType_t t2 = xTaskCreate(sendUSARTMessages_task, "sendUSART", 256, NULL, 1, NULL);
	if (t2 != pdPASS) {
		while(1);
	}
	
	char *helloWorld = "Hello World.\n\r";
	uartPcTransmit(helloWorld, 15);
	
	vTaskStartScheduler();
	
	while(1);
}


