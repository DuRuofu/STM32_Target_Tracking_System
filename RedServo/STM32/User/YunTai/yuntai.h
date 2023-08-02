#ifndef __YUNTAI_H
#define __YUNTAI_H

#include "main.h"
#include "pwm.h"
void Yuntai_Init(void);
void Yuntaiz_A(uint16_t pwm_d,int16_t Flow_Coefficient);
void Yuntaiz_B(uint16_t pwm_d,int16_t Flow_Coefficient);
void Yuntai_Control(void);
#endif // !__YUNTAI_H
