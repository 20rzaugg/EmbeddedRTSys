#include "stm32l476xx.h"

#define SPECIAL 3
#define ANALOG 2
#define OUTPUT 1
#define INPUT 0

#define PUSHPULL 0;
#define OPENDRAIN 1;

#define NONE 0
#define PULLUP 1
#define PULLDOWN 2

#define LOW 0
#define HIGH 1

void pinMode(GPIO_TypeDef *port, unsigned int pin, unsigned int mode);

void setOutputType(GPIO_TypeDef *port, unsigned int pin, unsigned int type);

void setPullUpDown(GPIO_TypeDef *port, unsigned int pin, unsigned int pupd);

void digitalWrite(GPIO_TypeDef *port, unsigned int pin, unsigned int value);

int digitalRead(GPIO_TypeDef *port, unsigned int pin);
