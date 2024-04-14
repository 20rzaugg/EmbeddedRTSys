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

#define LED 5
#define BUTTON 13

// Static variables

volatile uint8_t sine_index1 = 0;
volatile uint8_t sine_index2 = 0;
volatile uint8_t sine_index4 = 0;
volatile uint8_t sine_index8 = 0;
volatile uint8_t sine_index16 = 0;

static QueueHandle_t mailboxLedState;
static QueueHandle_t mailboxToneState;
static QueueHandle_t mailboxTonePitch;

extern const uint16_t note_table[8];
static const uint16_t tonePitchInitial = 284;

// Private function prototypes

void checkButton(void *pvParameters);
void controlLED(void *pvParameters);

void changeTonePitch(void *pvParameters);

void issueSensorCommand(void *pvParameters);

void sendTemperatureMessage(void *pvParameters);
void sentDistanceMessage(void *pvParameters);

// Private function bodies

void updateTimers_task(void *pvParameters) {
	
	static char note;
	
	while(1) {
	
		// Check the note queue.
		xQueueReceive(queueUartNote, &note, portMAX_DELAY);

		// Check if the received character was a valid note name.
		if (
			(note >= 'A' && note <= 'H') ||
			(note >= 'a' && note <= 'h')
		) {
			
			// Determine the note's pitch.
			uint16_t newPitch;
			if (note >= 'A' && note <= 'H') {
				newPitch = note_table[note - 65];
			} else {
				newPitch = note_table[note - 97];
			}
			
			xQueueSendToFrontFromISR(mailboxTonePitch, &newPitch, NULL);
		}		
	}

}

void pollSensors_task(void *pvParameters) {
	while(true) {
		
		uartSensorRequestDistance(USART3);
		uartSensorRequestDistance(USART1);
		vTaskDelay(15);
	}
}

void sendTemperatureMessage(void *pvParameters) {

	static char* format = "%u deg F\n\r";
	static char message[PC_MESSAGE_MAX_LENGTH];
	
	while(1) {
		
		// Receive the temperature value.
		uint8_t temperatureC;
		xQueueReceive(queueUartSensorTemperature, &temperatureC, portMAX_DELAY);
		
		// Convert the C temperature to F
		int temperatureF = (temperatureC - 45) * 9 / 5 + 32;
		
		// Create the message string
		int messageLength = snprintf(message, PC_MESSAGE_MAX_LENGTH, format, temperatureC);
		
		// Transmit the message
		uartPcTransmit(message, messageLength);
	}

}

void sendDistanceMessage(void *pvParameters) {
	
	static char* format = "%u inches\n\r";
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
	
	static uint8_t toneState = 0;
	static uint16_t tonePitch = tonePitchInitial;
	
	xQueueReceiveFromISR(mailboxToneState, &toneState, NULL);
	xQueueReceiveFromISR(mailboxTonePitch, &tonePitch, NULL);	

	if(toneState) {
		sine_index1 = (sine_index1 + 1) % 64;
		sine_index2 = (sine_index2 + 2) % 64; // high frequency component of the sound
		sine_index4 = (sine_index4 + 4) % 64;
		sine_index8 = (sine_index8 + 8) % 64;
		sine_index16 = (sine_index16 + 16) % 64;
		uint8_t lut_value = (uint8_t)(((sinLUT[sine_index1] + sinLUT[sine_index2] + sinLUT[sine_index4] + sinLUT[sine_index8] + sinLUT[sine_index16])) / 5.0);
		DAC1->DHR12R1 = (lut_value + 50u) & 0xfff;
	}
	
	TIM4->ARR = tonePitch;
	
	TIM4->SR &= ~TIM_SR_UIF;
	return 0;
}

int main() {
	
	mailboxLedState = xQueueCreate(1, sizeof(uint8_t));
	mailboxToneState = xQueueCreate(1, sizeof(uint8_t));
	mailboxTonePitch = xQueueCreate(1, sizeof(uint16_t));
	
	useHSI();
	pinMode(GPIOC, BUTTON, INPUT);
	setPullUpDown(GPIOC, BUTTON, PULLUP); //our button is active low
	
	pinMode(GPIOA, LED, OUTPUT);
	
	uartPcInitialize();
	uartSensorInitialize();
	
	enableTimer(TIM4, 1, 70, UPCOUNT, 1);
	TIM4->CR1 |= TIM_CR1_ARPE;

	DACinit_ch1(DAC_NORMAL_BUFFER_EXTERNAL, DAC_TRIGGER_NONE);

	BaseType_t t1 = xTaskCreate(checkButton, "checkButton", 128, NULL, 1, NULL);
	if (t1 != pdPASS) {
		while(1);
	}
	BaseType_t t2 = xTaskCreate(controlLED, "controlLED", 128, NULL, 1, NULL);
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


