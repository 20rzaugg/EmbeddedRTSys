#include "dac.h"

void DACinit_ch1() {
    //enable DAC clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
    //Disable DAC channel 1
    DAC->CR &= ~DAC_CR_EN1;
    //set GPIOA 4 to analog mode
    pinMode(GPIOA, 4, ANALOG);
    

}

void DACinit_ch2() {

}