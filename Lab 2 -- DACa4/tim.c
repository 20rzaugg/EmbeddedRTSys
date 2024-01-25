#include "tim.h"

void useHSI() {
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while(!(RCC->CFGR & RCC_CFGR_SWS_HSI));

}

void enableTimer(TIM_TypeDef *TIMx, uint32_t prescaler, uint32_t period, uint8_t direction, uint8_t generateInterrupt) {
    if(TIMx == TIM1) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    } 
    else if(TIMx == TIM2) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    } 
    else if(TIMx == TIM3) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
    } 
    else if(TIMx == TIM4) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
    } 
    else if(TIMx == TIM5) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
    } 
    else if(TIMx == TIM6) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    } 
    else if(TIMx == TIM7) {
        RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;
    } 
    else if(TIMx == TIM8) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
    } 
    else if(TIMx == TIM15) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    } 
    else if(TIMx == TIM16) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
    } 
    else if(TIMx == TIM17) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
    } 
    else {
        return;
    }
    //compute arr from prescaler and SystemCoreClock

    arr = (SystemCoreClock / (prescaler + 1)) * period - 1;
    TIMx->PSC = prescaler;
    TIMx->ARR = period;
}