#include "gpio.h"

#define LED GPIOA, 5
#define BUTTON GPIOC, 13

int main() {
	pinMode(BUTTON, INPUT);
	setPullUpDown(BUTTON, PULLUP); //our button is active low
	
	pinMode(LED, OUTPUT);
	
	//int button = digitalRead(BUTTON);
	//digitalWrite(LED, HIGH); 
}