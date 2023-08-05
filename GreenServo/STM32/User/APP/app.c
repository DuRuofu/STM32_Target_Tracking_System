/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-08-02 12-02-17
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-04 15-50-04
 * @FilePath: \Project\RedServo\STM32\User\APP\app.c
 * @Description: 主程序入口，包含主循环和中断回调函数，以及一些全局变量的定义
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "app.h"
#include "Buzzer.h"
#include "at24c02.h"
#include "string.h"
#include <math.h>

//题目选择标志位
extern uint8_t Problem_Flag = 0;
//消息接收标志位
extern uint8_t K210_Flag =0;

//--------------------------舵机参考系---------------------
//手动调整
uint16_t Screen_Width = 310;
uint16_t Screen_Height = 294;
//中心位置处参数
extern uint16_t Centre_A = 1460;
extern uint16_t Centre_B = 1457;
// 舵机（左上位置）
extern uint16_t left_PWM = 1613;
extern uint16_t up_PWM =1270;
// 舵机B （右下位置）
extern uint16_t right_PWM = 1307;
extern uint16_t down_PWM = 1565;


//--------------------------K210参考系---------------------
//左上角坐标
extern uint16_t K210_left = 0;
extern uint16_t K210_up= 0;
//右下角坐标
extern uint16_t K210_right = 0;
extern uint16_t K210_down= 0;
//宽度长度
extern uint16_t K210_Width = 0;
extern uint16_t K210_Height= 0;

extern uint16_t K210_data[8];

//实时位置
extern uint16_t pwm_A ;
extern uint16_t pwm_B ;

#define DEVICE_ADDRESS 0x50

/**
 * @description: 系统应用初始化
 * @return {*}
 */
void App_Init(void)
{
    OLED_Init();          // 0.96oled初始化
    OLED_Clear();
    USART_IT_Config();    // 总串口接收初始化
    PWM_Init();
    //Yuntai_Control();        // 云台初始化
    HAL_TIM_Base_Start_IT(&htim4); // 启动定时器4 开始数据采样,执行控制
    //Yuntaiz_B_Move(Centre_B,0);
    //SearchTheScreen();
        pwm_A =1500;
    pwm_B =1628;
    Yuntaiz_A_Move(pwm_A,0);
    Yuntaiz_B_Move(pwm_B,0);
}



//上电搜索屏幕
void SearchTheScreen(void)
{
    Yuntaiz_A_Move(2000,0);
    pwm_B =1628;
    Yuntaiz_B_Move(pwm_B,0);
    DEBUG_info("搜索屏幕","开始搜索屏幕 \r\n");
    uint8_t data_str[255]={0};
    //通知K210传递参数
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",1,0,0);
    Usart2_SendString(data_str);


    for(;;)
    {
            for(pwm_A = 2000;pwm_A>=1000;pwm_A=pwm_A-100)
        {
            Yuntaiz_A_Move(pwm_A,0);
            HAL_Delay(100);
            if(K210_Flag == 1)
            {
                DEBUG_info("搜索屏幕","搜索到屏幕!\r\n");
                    printf("pwm_A:%d,pwm_B:%d\r\n",pwm_A,pwm_B);
                Buzzer_LongBeep();
                K210_Flag = 0;
                return;
            }
        }
            for(pwm_A = 1000;pwm_A<=2000;pwm_A=pwm_A+100)
        {
            Yuntaiz_A_Move(pwm_A,0);
            HAL_Delay(100);
            if(K210_Flag == 1)
            {
                DEBUG_info("搜索屏幕","搜索到屏幕!\r\n");
                    printf("pwm_A:%d,pwm_B:%d\r\n",pwm_A,pwm_B);
                Buzzer_LongBeep();
                K210_Flag = 0;
                return;
            }
        }
    }

}


//题目1
void Problem1(void){

    //运动目标位置复位，一键启动自动追踪系统
    uint8_t data_str[255]={0};
    //通知K210传递参数
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",2,0,0);
    Usart2_SendString(data_str);
    while(1)
    {
        if (K210_Flag == 1)
        {
            printf("进行计算");
            //进行pid控制
            uint16_t red_x=K210_data[0] ,red_y=K210_data[1],green_x=K210_data[2],green_y=K210_data[3];
            if(red_x > 2000)
            {
                red_x -=2000;
            }
            int16_t x_err =  red_x - green_x, y_err = red_y -green_y;
            if(x_err <=2  && y_err<=2)
            {
                 //声光提示
                Buzzer_LongBeep();
                LED_Toggle(2);
                HAL_Delay(500);
                LED_Toggle(2);
                HAL_Delay(500);
                LED_Toggle(2);
            }
            else{
                   Yuntai_PID(x_err,y_err);
            }

            K210_Flag = 0;
        }
    }

}


//题目2
void Problem2(void){
    

}

//题目3
void Problem3(void){
  
  
}




/**
 * @description: 系统应用循环任务
 * @return {*}
 */
void App_Task(void)
{
    static uint8_t last_Flag =0;
    Menu_Refresh(); // 刷新菜单
    switch (Problem_Flag)
    {
    case 1:
        if(last_Flag != Problem_Flag)
        {
            Problem1();
            last_Flag = Problem_Flag;
        }
        break;
    case 2:
        if(last_Flag != Problem_Flag)
        {
            Problem2();
            last_Flag = Problem_Flag;
        }
        break;
    case 3:
        if(last_Flag != Problem_Flag)
        {
            Problem3();
            last_Flag = Problem_Flag;
        }
        break;
    default:
        break;
    }
}




// 定时器中断回调函数(1ms一次)
uint8_t LED_Heartbeat = 0; // LED心跳
uint16_t encoder_count = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim4) // 判断中断是否来自于定时器4
    {
        // 心跳(50ms一次)
        LED_Heartbeat++;
        if (LED_Heartbeat == 30)
        {
            LED_Toggle(1);
        }
    }
}

 
