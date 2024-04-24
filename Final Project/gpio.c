#include "gpio.h"

void pinMode(GPIO_TypeDef *port, unsigned int pin, unsigned int mode) {
	//enable appropriate GPIO clock for the pin we're setting up  
	if (port == GPIOA) {
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	}
	else if (port == GPIOB) {
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	}
	else if (port == GPIOC) {
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	}
	else {
		//if not gpio A, B, or C, return. No need to finish function
		return;
	}
	port->MODER = (port->MODER & ~(0x3u << (pin * 2))) | (mode << (pin * 2));
}

void setOutputType(GPIO_TypeDef *port, unsigned int pin, unsigned int type) {
    port->OTYPER = (port->OTYPER & ~(0x1 << pin)) | (type << pin);
}

void setPullUpDown(GPIO_TypeDef *port, unsigned int pin, unsigned int pupd) {
    port->PUPDR = (port->PUPDR & ~(0x3u << (pin * 2))) | (pupd << (pin * 2));
}

void setSpeed(GPIO_TypeDef *port, unsigned int pin, unsigned int speed) {
	port->OSPEEDR = (port->OSPEEDR & ~(0x3u << (pin * 2))) | (speed << (pin * 2));
}

void setAlternateFunction(GPIO_TypeDef *port, unsigned int pin, unsigned int alternateFunction) {
	if (pin < 8) {
		port->AFR[0] |= (alternateFunction & 0xFu) << (pin * 4);
	} else {
		port->AFR[1] |= (alternateFunction & 0xFu) << ((pin - 8) * 4);
	}
}

void digitalWrite(GPIO_TypeDef *port, unsigned int pin, unsigned int value) {
    port->ODR = (port->ODR & ~(0x3u << pin)) | (value << pin);
}

int digitalRead(GPIO_TypeDef *port, unsigned int pin) {
    return (port->IDR >> pin) & 0x1;
}
