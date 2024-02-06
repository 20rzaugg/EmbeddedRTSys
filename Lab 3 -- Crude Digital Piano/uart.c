#include "stm32l476xx.h"
#include "gpio.h"

void uart_configure_io(void) {

	pinMode(GPIOA, 2, SPECIAL);
    pinMode(GPIOA, 3, SPECIAL);

    setOutputType(GPIOA, 2, PUSHPULL);
    setOutputType(GPIOA, 3, PUSHPULL);

    setSpeed(GPIOA, 2, VERYHIGHSPEED);
    setSpeed(GPIOA, 3, VERYHIGHSPEED);

    setPullUpDown(GPIOA, 2, PULLUP);
    setPullUpDown(GPIOA, 3, PULLUP);
    
    GPIOA->AFR[0] |= 0x77 << 8;

}