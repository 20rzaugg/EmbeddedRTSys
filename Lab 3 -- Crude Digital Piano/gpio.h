#include "stm32l476xx.h"

#define ANALOG 3u
#define SPECIAL 2u
#define OUTPUT 1u
#define INPUT 0u

#define PUSHPULL 0u
#define OPENDRAIN 1u

#define NONE 0u
#define PULLUP 1u
#define PULLDOWN 2u

#define LOW 0u
#define HIGH 1u

#define LOWSPEED 0u
#define MEDIUMSPEED 1u
#define HIGHSPEED 2u
#define VERYHIGHSPEED 3u

void pinMode(GPIO_TypeDef *port, unsigned int pin, unsigned int mode);

void setOutputType(GPIO_TypeDef *port, unsigned int pin, unsigned int type);

void setPullUpDown(GPIO_TypeDef *port, unsigned int pin, unsigned int pupd);

void setSpeed(GPIO_TypeDef *port, unsigned int pin, unsigned int speed);

void digitalWrite(GPIO_TypeDef *port, unsigned int pin, unsigned int value);

int digitalRead(GPIO_TypeDef *port, unsigned int pin);
