#include "gpio.h"
#include "tim.h"
#include "dac.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sin.h"
#include "stddef.h" //NULL
#include "queue.h"
#include "uart.h"

#define LED 5
#define BUTTON 13

volatile uint8_t sin_index = 0;

QueueHandle_t mailboxLedState;
QueueHandle_t mailboxToneState;

void checkButton(void *pvParameters) {
	static uint8_t buttonStateLast = 1;
	static uint8_t ledState = 0;
	while(1) {
		uint8_t buttonState = digitalRead(GPIOC, BUTTON);
		if (buttonState == 1 && buttonStateLast == 0) { //change on button release
			ledState = !ledState;
			xQueueSendToBack(mailboxLedState, &ledState, 0);
			xQueueSendToBack(mailboxToneState, &ledState, 0);
		}
		buttonStateLast = buttonState;
		vTaskDelay(10);
	}
}

void controlLED(void *pvParameters) {
	uint8_t ledState = 0; 
	while(1) {
		xQueueReceive(mailboxLedState, &ledState, portMAX_DELAY);
		digitalWrite(GPIOA, LED, ledState);
		vTaskDelay(10);
	}
}

int TIM4_IRQHandler() {
	
	static uint8_t toneState = 0;
	
	xQueueReceiveFromISR(mailboxToneState, &toneState, NULL);
	
	if(toneState) {
		sin_index++;
		if(sin_index >= 64) {
			sin_index = 0;
		}
		DAC1->DHR12R1 = (sinLUT[sin_index] + 50u) & 0xfff;
	}
	TIM4->SR &= ~TIM_SR_UIF;
	return 0;
}

int main() {
	
	mailboxLedState = xQueueCreate(1, sizeof(uint8_t));
	mailboxToneState = xQueueCreate(1, sizeof(uint8_t));
	
	useHSI();
	pinMode(GPIOC, BUTTON, INPUT);
	setPullUpDown(GPIOC, BUTTON, PULLUP); //our button is active low
	
	pinMode(GPIOA, LED, OUTPUT);
	
	enableTimer(TIM4, 7, 70, UPCOUNT, 1);

	DACinit_ch1(DAC_NORMAL_BUFFER_EXTERNAL, DAC_TRIGGER_NONE);

	BaseType_t t1 = xTaskCreate(checkButton, "checkButton", 128, NULL, 1, NULL);
	if (t1 != pdPASS) {
		while(1);
	}
	BaseType_t t2 = xTaskCreate(controlLED, "controlLED", 128, NULL, 1, NULL);
	if (t2 != pdPASS) {
		while(1);
	}
	vTaskStartScheduler();
	while(1);
}
