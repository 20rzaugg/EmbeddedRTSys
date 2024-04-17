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

#define NOTE_PITCH_TIMEOUT_TICKS 10

// Static variables

static QueueHandle_t mailboxVolume;

TaskHandle_t DACManager_task_handle;

extern const struct notePair note_table[25];
static const uint16_t tonePitchInitial = 284;

static volatile uint8_t globalVolume = 0;
// Private function prototypes

void DACManager_task(void *pvParameters);

void issueSensorCommand(void *pvParameters);

void sendTemperatureMessage(void *pvParameters);
void sentDistanceMessage(void *pvParameters);

// Private function bodies

void pollSensors_task(void *pvParameters) {
	while(true) {
		uartSensorRequestDistance(USART3);
		uartSensorRequestDistance(USART1);
		vTaskDelay(20);
	}
}

void DACManager_task(void *pvParameters) {
	static uint8_t volume = 0;
	static uint8_t sine_index1 = 0;
	//static uint8_t sine_index2 = 0;
	//static uint8_t sine_index4 = 0;
	static uint8_t sine_index8 = 0;
	static uint8_t sine_index16 = 0;

	while(true) {
		uint32_t notificationValue = ulTaskNotifyTake(pdTrue, NOTE_PITCH_TIMEOUT_TICKS);
		xQueueReceiveFromISR(mailboxVolume, &volume, NOTE_PITCH_TIMEOUT_TICKS);

		if(notificationValue > 0) {
			sine_index1 = (sine_index1 + 1) % 64;
			//sine_index2 = (sine_index2 + 2) % 64; // high frequency component of the sound
			//sine_index4 = (sine_index4 + 4) % 64;
			sine_index8 = (sine_index8 + 8) % 64;
			sine_index16 = (sine_index16 + 16) % 64;
			//uint8_t lut_value = (uint8_t)(((sinLUT[sine_index1] + sinLUT[sine_index2] + sinLUT[sine_index4] + sinLUT[sine_index8] + sinLUT[sine_index16])) / 5.0);
			uint8_t lut_value = (uint8_t)(((sinLUT[sine_index1] + sinLUT[sine_index8] + sinLUT[sine_index16])) / 3.0);
		}
		if(volume < globalVolume) {
			DAC1->DHR12R1 = (lut_value + 50u) & 0xfff;
		}
	}
}

void sendNoteMessage(void *pvParameters) {
	static uint8_t note_index = 0;
	static char* format = "%s\n\r";
	static char message[PC_MESSAGE_MAX_LENGTH];

	while(1) {
		xQueueReceiveFromISR()
		// Receive the distance value.
		uint8_t new_note;
		xQueueReceive(queueUartNote, &new_note, portMAX_DELAY);

		if(new_note != note_index) {
			note_index = new_note;
			int messageLength = snprintf(message, PC_MESSAGE_MAX_LENGTH, format, note_table[note_index].name);
			uartPCTransmit(message, messageLength);
		}
	}
}

void sendVolumeMessage(void *pvParameters) {
	
	static char* format = "Vol %u\n\r";
	static char message[PC_MESSAGE_MAX_LENGTH];

	while(1) {
		
		// Receive the distance value.
		uint16_t distanceMm;
		xQueueReceive(queueUartSensorDistance, &distanceMm, portMAX_DELAY);
		
		// Convert the mm distance to in
		int distanceIn = (int)(distanceMm / 25.4f);
		
		int messageLength = snprintf(message, PC_MESSAGE_MAX_LENGTH, format, distanceIn);
		
		// Transmit the message
		uartPcTransmit(message, messageLength);
	
	}

}



int TIM4_IRQHandler() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	vTaskNotifyGiveFromISR(DACManager_task_handle, &xHigherPriorityTaskWoken);	

	if(xHigherPriorityTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR();
	}

	TIM4->SR &= ~TIM_SR_UIF;

	return 0;
	//TIM4->ARR = tonePitch; //transplant later :)
}

int TIM3_IRQHandler() {
	globalVolume = (globalVolume + 1) % 8;
	TIM3->SR &= ~TIM_SR_UIF;
}

int main() {
	
	mailboxVolume = xQueueCreate(1, sizeof(uint8_t));
	
	useHSI();
	pinMode(GPIOC, BUTTON, INPUT);
	setPullUpDown(GPIOC, BUTTON, PULLUP); //our button is active low
	
	pinMode(GPIOA, LED, OUTPUT);
	
	uartPcInitialize();
	uartSensorInitialize();
	
	enableTimer(TIM4, 1, 70, UPCOUNT, 1);
	TIM4->CR1 |= TIM_CR1_ARPE;

	enableTimer(TIM3, 0, 10, UPCOUNT, 1);
	TIM3->CR1 |= TIM_CR1_ARPE;

	DACinit_ch1(DAC_NORMAL_BUFFER_EXTERNAL, DAC_TRIGGER_NONE);

	BaseType_t t1 = xTaskCreate(pollSensors_task, "pollSensors", 128, NULL, 0, NULL);
	if (t1 != pdPASS) {
		while(1);
	}
	BaseType_t t2 = xTaskCreate(DACManager_task, "DACManager", 128, NULL, 1, DACManager_task_handle);
	if (t2 != pdPASS) {
		while(1);
	}
	
	BaseType_t t3 = xTaskCreate(changeTonePitch, "changetonePitch", 128, NULL, 1, NULL);
	if (t3 != pdPASS) {
		while(1);
	}
	
	BaseType_t t4 = xTaskCreate(issueSensorCommand, "issueSensorCommand", 128, NULL, 1, NULL);
	if (t4 != pdPASS) {
		while(1);
	}
	
	BaseType_t t5 = xTaskCreate(sendTemperatureMessage, "sendTemperatureMessage", 128, NULL, 1, NULL);
	if (t5 != pdPASS) {
		while(1);
	}
	
	BaseType_t t6 = xTaskCreate(sendDistanceMessage, "sendDistanceMessage", 128, NULL, 1, NULL);
	if (t6 != pdPASS) {
		while(1);
	}
	
	char *helloWorld = "Hello World.\n\r";
	uartPcTransmit(helloWorld, 15);
	
	vTaskStartScheduler();
	
	while(1);
}


