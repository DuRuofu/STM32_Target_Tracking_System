/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-08-02 12-02-17
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-03 15-29-16
 * @FilePath: \Project\RedServo\STM32\User\USART\serial_it_config.c
 * @Description: 串口模块汇总，包含串口初始化，串口接收中断初始化，串口接收完成回调函数，串口错误回调函数
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "serial_it_config.h"

/**
 * @description: 串口接收中断初始化(总)
 * @return {*}Debug_Init
 */
void USART_IT_Config(void)
{
    //串口1接收中断初始化
    Debug_Init();
    //串口2接收中断初始化
    USART2_Init();
    //串口3接收中断初始化
}

//串口接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    
{
    if(huart->Instance == USART1)
    {
        UART1_RxCpltCallback(huart);
    }
    else if(huart->Instance == USART2)
    {
        UART2_RxCpltCallback(huart);
    }
    else if(huart->Instance == USART3)
    {
        //UART2_RxCpltCallback();
    }
    }


//错误回调  
 void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)   
{       
    if(huart->Instance == USART1)
    {
        UART1_ErrorCallback(huart);
    }
    else if(huart->Instance == USART2)
    {
        UART2_ErrorCallback(huart);
    }
    else if(huart->Instance == USART3)
    {
        //UART3_ErrorCallback();
    } 
}

