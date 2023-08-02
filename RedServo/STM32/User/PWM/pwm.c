/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-07-07 15-49-11
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-07-26 21-36-15
 * @FilePath: \MDK-ARMd:\duruofu\Project\Avoidance_Car\project\STM32ZET6\Users\PWM\pwm.c
 * @Description: PWM模块(驱动电机使用,双路同频)
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "pwm.h"

//在此设置使用的定时器和对应A,B两路PWM的通道

#define TIM_1  htim1


#define PWM_SERVO_CHANNEL_A TIM_CHANNEL_1
#define PWM_SERVO_CHANNEL_B TIM_CHANNEL_4


//设定占空比最大值(Counter Period计数值)
#define PWM2_D 20000


//PWM初始化
void PWM_Init(void)
{
    //-----------舵机的PWM初始化-----------
    // 使能定时器的通道
    HAL_TIM_PWM_Start(&TIM_1,PWM_SERVO_CHANNEL_A);
    HAL_TIM_PWM_Start(&TIM_1,PWM_SERVO_CHANNEL_B);
    // 设置初始占空比为0
    __HAL_TIM_SET_COMPARE(&TIM_1, PWM_SERVO_CHANNEL_A, 0);
    __HAL_TIM_SET_COMPARE(&TIM_1, PWM_SERVO_CHANNEL_B, 0);
} 



//--------------------舵机PWM调节--------------------

void SERVO_PWMA_Set(uint16_t pwm_d)
{
    if(pwm_d > PWM2_D)
    {
        pwm_d = PWM2_D;
    }
    __HAL_TIM_SET_COMPARE(&TIM_1, PWM_SERVO_CHANNEL_A, pwm_d);
}


void SERVO_PWMB_Set(uint16_t pwm_d)
{
    if(pwm_d > PWM2_D)
    {
        pwm_d = PWM2_D;
    }
    __HAL_TIM_SET_COMPARE(&TIM_1, PWM_SERVO_CHANNEL_B, pwm_d);
}


