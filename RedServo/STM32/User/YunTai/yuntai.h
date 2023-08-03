#ifndef __YUNTAI_H
#define __YUNTAI_H

#include "main.h"
#include "pwm.h"
void Yuntai_Init(void);
void Yuntaiz_A_Move(uint16_t pwm_d,int16_t Flow_Coefficient);
void Yuntaiz_B_Move(uint16_t pwm_d,int16_t Flow_Coefficient);
void Yuntaiz_AB_Move(uint16_t pwm_a,uint16_t pwm_b, int16_t Flow_Coefficient);
void Yuntai_A4_Track(uint16_t pwm_a,uint16_t pwm_b, int16_t Flow_Coefficient);
void Yuntai_Control(void);

#endif // !__YUNTAI_H
