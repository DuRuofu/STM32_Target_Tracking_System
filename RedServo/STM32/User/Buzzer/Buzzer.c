/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-08-02 12-02-17
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-03 15-30-02
 * @FilePath: \Project\RedServo\STM32\User\Buzzer\Buzzer.c
 * @Description: 蜂鸣器模块，包含蜂鸣器初始化，蜂鸣器控制，蜂鸣器短响，蜂鸣器长响
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "Buzzer.h"

//在这里修改蜂鸣器引脚 ，或者使用cubemx生成的宏定义（引脚命名为Buzzer_Pin）
// #define Buzzer_Pin_Pin GPIO_PIN_11
// #define Buzzer_Pin_GPIO_Port GPIOB

/**
  * @brief  蜂鸣器初始化
  * @param  无
  * @retval 无
  */
void Buzzer_Init(void)
{
    //引脚初始化
}


/**
  * @brief  蜂鸣器控制
  * @param  无
  * @retval 无
  */
void Buzzer_Control(uint8_t status)
{
    //蜂鸣器控制
    if (status == 0)
    {
        HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_SET);
    }
    else if (status == 1)
    {
        HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_RESET);
    }
    
}


//蜂鸣器短响
void Buzzer_ShortBeep(void)
{
    Buzzer_Control(1);
    HAL_Delay(30);
    Buzzer_Control(0);
}


//蜂鸣器长响
void Buzzer_LongBeep(void)
{
    Buzzer_Control(1);
    HAL_Delay(1000);
    Buzzer_Control(0);
}