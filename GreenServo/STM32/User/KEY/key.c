/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-08-02 12-02-17
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-03 15-28-12
 * @FilePath: \Project\RedServo\STM32\User\KEY\key.c
 * @Description: 键盘扫描
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "key.h"
#include "led.h"
#include "gui.h"
#include "yuntai.h"
#include "Buzzer.h"
#include "app.h"


#define KEY1_Pin KEY_1_Pin
#define KEY2_Pin KEY_2_Pin
#define KEY3_Pin KEY_3_Pin
#define KEY4_Pin KEY_4_Pin
#define KEY5_Pin KEY_5_Pin
#define KEY6_Pin KEY_6_Pin
#define KEY7_Pin KEY_7_Pin
#define KEY8_Pin KEY_8_Pin
#define KEY9_Pin KEY_9_Pin
#define KEY10_Pin KEY_10_Pin
#define KEY11_Pin KEY_11_Pin



#define DEBOUNCE_DELAY 250 // 设置消抖延时为200毫秒



//GUI菜单标志位
extern uint8_t GUI_Menu;

//题目标志位
extern uint8_t Problem_Flag;


//急停标志位
extern uint8_t Stop_Flag;

// 初始位置  //舵机中值
extern uint16_t pwm_A ;
extern uint16_t pwm_B ;


/**
 * @description: 按键初始化 (使用CubeMX自动生成的宏定义，就不用写这个函数了)
 * @return {*}
 */
void Key_Init(void)
{

}



//题目切换按钮
void Key_1_Callback(void)
{
    
    //更换题目
    if(Problem_Flag < 3)
    {
        Problem_Flag++;
    }
    else 
    {
        Problem_Flag = 0;
    }
}



// 急停按键
void Key_2_Callback(void)
{
    if(Stop_Flag == 1)
    {
        Stop_Flag=0;
    }
    else{
        Stop_Flag=1;
    }

}


//设置舵机调试模式的按钮
void Key_3_Callback(void)
{

}


//上
void Key_4_Callback(void)
{

}

//下
void Key_5_Callback(void){

}

//左
void Key_6_Callback(void){

}

//右
void Key_7_Callback(void){

}

//题目1 //复位按键
void Key_8_Callback(void){

}


void Key_9_Callback(void){
    //更换题目
    if(Problem_Flag == 2)
    {
        Problem_Flag =0;
    }
    else 
    {
        Problem_Flag = 2;
    }
}
void Key_10_Callback(void){
    //更换题目
    if(Problem_Flag == 3)
    {
        Problem_Flag =0;
    }
    else 
    {
        Problem_Flag = 3;
    }
}
void Key_11_Callback(void){
    //更换题目
    if(Problem_Flag == 4)
    {
        Problem_Flag =0;
    }
    else 
    {
        Problem_Flag = 4;
    }
}

//
/**
 * @description: 按键检测，外部中断回调函数
 * @param {uint16_t} GPIO_Pin
 * @return {*}
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

    /* Prevent unused argument(s) compilation warning */
    UNUSED(GPIO_Pin);
    /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_GPIO_EXTI_Callback could be implemented in the user file
    */
    // 按键按下
    if(GPIO_Pin == KEY1_Pin)
    {
        //printf("按键1");
        Debounce(GPIO_Pin, Key_1_Callback);
       
    }
    else if(GPIO_Pin == KEY2_Pin)
    {

        // 按键2按下的处理代码
        Debounce(GPIO_Pin, Key_2_Callback);
    }
}




// 通用的按键消抖函数
void Debounce(uint16_t GPIO_Pin, void (*callback)(void))
{
    static uint32_t lastTriggerTime = 0;
    uint32_t currentTime = HAL_GetTick(); // 获取当前时间戳

    if (currentTime - lastTriggerTime >= DEBOUNCE_DELAY)
    {
        Buzzer_ShortBeep();
        callback(); // 调用传入的回调函数
        lastTriggerTime = currentTime; // 更新上一次触发的时间戳
    }
}



