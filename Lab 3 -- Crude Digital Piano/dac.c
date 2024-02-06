#include "dac.h"

void DACinit_ch1(uint32_t mode, uint32_t trigger) {
    //enable DAC clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
    //Disable DAC channel 1
    DAC->CR &= ~DAC_CR_EN1;
    
    //set trigger mode
    if(trigger >= DAC_TRIGGER_NONE) {
        DAC->CR &= ~DAC_CR_TEN1;
    } else {
        DAC->CR |= DAC_CR_TEN1;
        DAC->CR &= ~(7u << 3);
        DAC->CR |= (trigger << 3);
    }
    
    //set mode
    DAC->MCR &= ~(7u);
    DAC->MCR |= (mode);

    //enable DAC channel 1
    DAC->CR |= DAC_CR_EN1;
		
		//set GPIOA 4 to analog mode
    pinMode(GPIOA, 4, ANALOG);
}

void DACinit_ch2(uint32_t mode, uint32_t trigger) {
    //enable DAC clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
    //Disable DAC channel 2
    DAC->CR &= ~DAC_CR_EN2;
    //set GPIOA 5 to analog mode
    pinMode(GPIOA, 5, ANALOG);
    
    //set trigger mode
    if(trigger >= DAC_TRIGGER_NONE) {
        DAC->CR &= ~DAC_CR_TEN2;
    } else {
        DAC->CR |= DAC_CR_TEN2;
        DAC->CR &= ~(7u << 19);
        DAC->CR |= (trigger << 19);
    }
    
    //set mode
    DAC->MCR &= ~(7u << 16);
    DAC->MCR |= (mode << 16);

    //enable DAC channel 2
    DAC->CR |= DAC_CR_EN2;
}