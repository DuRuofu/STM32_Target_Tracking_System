#ifndef __LED_H
#define __LED_H

#include "gpio.h"

void LED_Init(void);
void LED_On(uint8_t led_num);
void LED_Off(uint8_t led_num);
void LED_Toggle(uint8_t led_num);

#endif // !__LED_H



