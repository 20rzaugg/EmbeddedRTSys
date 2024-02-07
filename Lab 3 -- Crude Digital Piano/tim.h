#include "stm32l476xx.h"
#include <stdint.h>

#define UPCOUNT 0
#define DOWNCOUNT 1

void useHSI(void);

void enableTimer(TIM_TypeDef *TIMx, uint32_t prescaler, uint32_t aar, uint8_t direction, uint8_t generateInterrupt);
