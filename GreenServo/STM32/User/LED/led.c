/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-07-07 09-36-03
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-07-26 20-30-03
 * @FilePath: \MDK-ARMd:\duruofu\Project\Avoidance_Car\project\STM32ZET6\Users\LED\led.c
 * @Description: 控制一个led灯
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "led.h"

/**
  * @brief  LED初始化
  * @param  无
  * @retval 使用cubeMX生成只需要将引脚用户标签命名为LED_1即可，无需再次调用此函数
  */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : LED_Pin */
    GPIO_InitStruct.Pin = LED_1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_1_GPIO_Port, &GPIO_InitStruct);
}


/**
  * @brief  LED亮
  * @param  无
  * @retval 无
  */
void LED_On(uint8_t led_num)
{
    if (led_num == 1)
    {
      HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
    }
    else if(led_num == 2)
    {
      HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
    }
}


/**
  * @brief  LED灭
  * @param  无
  * @retval 无
  */
void LED_Off(uint8_t led_num)
{
    if (led_num == 1)
    {
      HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
    }
    else if(led_num == 2)
    {
      HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
    }   
}


/**
  * @brief  LED翻转
  * @param  无
  * @retval 无
  */
void  LED_Toggle(uint8_t led_num)
{
    
    if (led_num == 1)
    {
     HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
    }
    else if(led_num == 2)
    {
      HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
    }  
}


