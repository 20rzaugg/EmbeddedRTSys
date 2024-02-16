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

void changeTonePitch(char note);

// Private function bodies

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

void changeTonePitch(char note) {

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
		uint8_t lut_value = (uint8_t)(((sinLUT[sin_index1] + sinLUT[sine_index2] + sinLUT[sine_index4] + sinLUT[sine_index8] + sinLUT[sine_index16])) / 5.0);
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
	
	uart_initialize();
	uart_set_callback_data_received(changeTonePitch);
	
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
	vTaskStartScheduler();
	while(1);
}
