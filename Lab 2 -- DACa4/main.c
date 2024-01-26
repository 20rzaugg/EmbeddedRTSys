#include "gpio.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sin.h"
#include "stddef.h" //NULL

#define LED 5
#define BUTTON 13

volatile uint8_t led_state = 0;

void checkButton(void *pvParameters) {
	static uint8_t last_state = 1;
	while(1) {
		uint8_t button_state = digitalRead(GPIOC, BUTTON);
		if (button_state == 1 && last_state == 0) { //change on button release
			led_state = !led_state;
		}
		last_state = button_state;
		vTaskDelay(10);
	}
}

void controlLED(void *pvParameters) {
	while(1) {
		digitalWrite(GPIOA, LED, led_state);
		vTaskDelay(10);
	}
}

int main() {
	useHSI();
	pinMode(GPIOC, BUTTON, INPUT);
	setPullUpDown(GPIOC, BUTTON, PULLUP); //our button is active low
	
	pinMode(GPIOA, LED, OUTPUT);
	
	// 85% confident that we want 440*44 = 19360 changes per second
	// 82% confident that a prescaler of 14 gives us a number very close to a whole number
	// 30% confident this will work 
	enableTimer(TIM4, 14, 19360, UPCOUNT, 1);

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
