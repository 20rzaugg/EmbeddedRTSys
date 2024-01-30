#include "tim.h"

void useHSI() {
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while(!(RCC->CFGR & RCC_CFGR_SWS_HSI));
    SystemCoreClockUpdate();
}

void enableTimer(TIM_TypeDef *TIMx, uint32_t prescaler, uint32_t aar, uint8_t direction, uint8_t generateInterrupt) {
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

    //disable counter
    TIMx->CR1 &= ~TIM_CR1_CEN;
    //edge aligned mode
    TIMx->CR1 &= ~TIM_CR1_CMS;
    //Master mode
    TIMx->CR2 &= ~TIM_CR2_MMS;
    TIMx->CR2 |= TIM_CR2_MMS_2; //100 update trigger as TRGO
    
    if(generateInterrupt) {
        //Trigger interrupt on update
        TIMx->DIER |= TIM_DIER_TIE;
        //Enable interrupt
        TIMx->DIER |= TIM_DIER_UIE;
    }
    
    //output compare mode
    TIMx->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIMx->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); //0110 PWM mode 1

    //set output polarity
    TIMx->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
    //enable output
    TIMx->CCER |= TIM_CCER_CC1E;

    //compute arr from prescaler and SystemCoreClock
    TIMx->PSC = prescaler;
    TIMx->ARR = aar;
    TIMx->CCR1 = 30;
    //set direction
    if(direction == UPCOUNT) {
        TIMx->CR1 &= ~TIM_CR1_DIR;
    } else if(direction == DOWNCOUNT) {
        TIMx->CR1 |= TIM_CR1_DIR;
    }
    //enable interrupt
    if(TIMx == TIM2 && generateInterrupt) {
        NVIC_EnableIRQ(TIM2_IRQn);
    } else if(TIMx == TIM3 && generateInterrupt) {
        NVIC_EnableIRQ(TIM3_IRQn);
    } else if(TIMx == TIM4 && generateInterrupt) {
        NVIC_EnableIRQ(TIM4_IRQn);
    } else if(TIMx == TIM5 && generateInterrupt) {
        NVIC_EnableIRQ(TIM5_IRQn);
    } else if(TIMx == TIM7 && generateInterrupt) {
        NVIC_EnableIRQ(TIM7_IRQn);
    }
    //enable counter
    TIMx->CR1 |= TIM_CR1_CEN;
}