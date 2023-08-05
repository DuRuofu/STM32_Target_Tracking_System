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
//校准确认标志位
extern uint8_t Calibration_Flag =0;
//消息接收标志位
extern uint8_t K210_Flag =0;

//--------------------------舵机参考系---------------------
//手动调整
uint16_t Screen_Width = 310;
uint16_t Screen_Height = 294;
//中心位置处参数
extern uint16_t Centre_A = 1460;
extern uint16_t Centre_B = 1417;
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
    Yuntai_Control();        // 云台初始化
    HAL_TIM_Base_Start_IT(&htim4); // 启动定时器4 开始数据采样,执行控制
    Update_Parameters();
}



//上电与K210通讯拿参数
void Update_Parameters(void)
{
    DEBUG_info("读取配置","开始读取配置信息 \r\n");
    uint8_t data_str[255]={0};
    //通知K210传递参数
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",1,0,0);
    Usart2_SendString(data_str);
    while(K210_Flag == 0)
    {
        HAL_Delay(500);
        DEBUG_info("读取配置","等待读取.... \r\n");
    }
    K210_Flag = 0;
    //更新参数
    left_PWM =K210_data[0];
    up_PWM=K210_data[1];
    right_PWM=K210_data[2];
    down_PWM=K210_data[3];
    K210_left=K210_data[4];
    K210_up=K210_data[5];
    K210_right=K210_data[6];
    K210_down=K210_data[7];
    //计算参数
    Screen_Width =left_PWM - right_PWM;
    Screen_Height = down_PWM - up_PWM;
    //计算中间
    Centre_A =(left_PWM+right_PWM)/2;
    Centre_B =(up_PWM+down_PWM)/2;
    //计算长宽
    K210_Width = K210_right - K210_left;
    K210_Height= K210_down - K210_up;
    DEBUG_info("读取配置","配置信息读取完毕!! \r\n");
    Buzzer_LongBeep();
}

//校准模式(待优化)
void System_Calibration(void)
{ 
     uint8_t data_str[255]={0};
    //移动到左上角
    pwm_A = left_PWM;
    pwm_B= up_PWM;
    DEBUG_info("校准函数","开始手动校准/r/n");
    //进入手动模式
    while(Calibration_Flag !=1)
    {
        if (HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin) == 1) pwm_B -= 1;
        //下
        if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == 1) pwm_B += 1;
        //左
        if (HAL_GPIO_ReadPin(KEY_6_GPIO_Port, KEY_6_Pin) == 1) pwm_A += 1;
        //右
        if (HAL_GPIO_ReadPin(KEY_7_GPIO_Port, KEY_7_Pin) == 1) pwm_A -= 1;
        HAL_Delay(30);
        Yuntai_Control();
    }
    Calibration_Flag = 0;
    //保存当前值为新的左上角参数
    left_PWM = pwm_A;
    up_PWM = pwm_B;
    //向K210确定坐标
    strcpy((char*)data_str, "");
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",2,left_PWM,up_PWM);
    Usart2_SendString(data_str);
    
    while(K210_Flag == 0)HAL_Delay(10);
    
    K210_Flag = 0;
    K210_left = K210_data[0];
    K210_up = K210_data[1];
    
    //移动到右下角
    pwm_A =  left_PWM -Screen_Width;
    pwm_B =  up_PWM + Screen_Height;
    //进入手动模式
    while(Calibration_Flag !=1)
    {
        if (HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin) == 1) pwm_B -= 1;
        //下
        if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == 1) pwm_B += 1;
        //左
        if (HAL_GPIO_ReadPin(KEY_6_GPIO_Port, KEY_6_Pin) == 1) pwm_A += 1;
        //右
        if (HAL_GPIO_ReadPin(KEY_7_GPIO_Port, KEY_7_Pin) == 1) pwm_A -= 1;
        HAL_Delay(30);
        Yuntai_Control();
    }
    //保存当前值为新的右下角参数
    right_PWM = pwm_A;
    down_PWM = pwm_B;
    //向K210确定坐标
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",3,right_PWM,down_PWM);
    Usart2_SendString(data_str);
    
    
    while(K210_Flag == 0)HAL_Delay(10); //等待K210回应
    
    K210_Flag = 0;
    K210_right = K210_data[0];
    K210_down = K210_data[1];
    
    DEBUG_info("校准函数","手动校准结束/r/n");
    
    //---------计算参数--------------------
    
    //---------舵机坐标系
    DEBUG_info ("aa","right_PWM:%d,left_PWM:%d",right_PWM,left_PWM);
    //计算长宽
    Screen_Width =left_PWM - right_PWM;
    Screen_Height = down_PWM - up_PWM;
     
    //计算中间
    Centre_A =(left_PWM+right_PWM)/2;
    Centre_B =(up_PWM+down_PWM)/2;

    //---------K210坐标系
    //计算长宽
    K210_Width = K210_right - K210_left;
    K210_Height= K210_down - K210_up;
    
    //------------保存参数------------------
    sprintf((char*)data_str, "id:%d,left_PWM:%d,up_PWM:%d,right_PWM:%d,down_PWM:%d,K210_left:%d,K210_up:%d,K210_right:%d,K210_down:%d",5,left_PWM,up_PWM,right_PWM,down_PWM,K210_left,K210_up,K210_right,K210_down);
    Usart2_SendString(data_str);
    //while(K210_Flag == 0)HAL_Delay(10); //等待K210回应
    Problem_Flag =1;
}


//题目1
void Problem1(void){
        //执行一次
        pwm_A =  Centre_A;
        pwm_B =  Centre_B;
        Yuntaiz_A_Move(pwm_A,2);   // 云台丝滑移动
        Yuntaiz_B_Move(pwm_B,2);   // 云台丝滑移动
        HAL_Delay(50    );
        Buzzer_LongBeep();
}

//题目2
void Problem2(void){
    
        Yuntaiz_AB_Move(Centre_A,Centre_B,5);   //运动到中间
        Yuntaiz_AB_Move(left_PWM,up_PWM,5);//斜着移动到最左上角  
        Yuntaiz_A_Move(right_PWM,5);   //运动到最右方
        Yuntaiz_B_Move(down_PWM,5);   //运动到最下方
        Yuntaiz_A_Move(left_PWM,5);   //运动到最左方
        Yuntaiz_B_Move(up_PWM,5);   //
        HAL_Delay(100);
        Yuntaiz_AB_Move(Centre_A,Centre_B,5);   //运动到中间
}

//题目3
void Problem3(void){
    //A4巡线
    Yuntaiz_AB_Move(left_PWM,up_PWM,5);//斜着移动到最左上角  
    uint16_t  A_x,A_y,B_x,B_y,C_x,C_y,D_x,D_y;
    //向K210请求4个点的坐标
    uint8_t data_str[255]={0};
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",4,0,0);
    Usart2_SendString(data_str);
    while(K210_Flag == 0)HAL_Delay(10);  //等待K210回应
    Buzzer_ShortBeep();
    K210_Flag = 0;
    A_x = K210_data[0];
    A_y = K210_data[1];
    B_x = K210_data[2];
    B_y = K210_data[3];
    C_x = K210_data[4];
    C_y = K210_data[5];
    D_x = K210_data[6];
    D_y = K210_data[7];  
    
    
    //计算坐标映射
    float ka = (float)Screen_Width/(float)K210_Width;
    float kb = (float)Screen_Height/(float)K210_Height;
    
    DEBUG_info ("题目3","Screen_Width:%f,K210_Width:%f",(float)Screen_Width,(float)K210_Width);
    DEBUG_info("题目3","Screen_Height:%f,K210_Height:%f",(float)Screen_Height,(float)K210_Height);
    DEBUG_info ("题目3","ka:%f,kb:%f",ka,kb);

    DEBUG_info ("题目3","A_x:%d,K210_left:%d",A_x,K210_left);
    //计算A点坐标
    //计算A点坐标
    A_x = left_PWM -round((A_x - K210_left)*ka);
    A_y = round((A_y - K210_up))*kb + up_PWM;
    

    //计算B点坐标
    B_x = left_PWM- round((B_x - K210_left)*ka);
    B_y = round((B_y - K210_up))*kb + up_PWM;
     
     
    //计算C点坐标
    C_x = left_PWM- round((C_x - K210_left)*ka);
    C_y = round((C_y - K210_up))*kb + up_PWM;

    //计算D点坐标
    D_x = left_PWM-  round((D_x - K210_left)*ka) ;
    D_y = round((D_y - K210_up))*kb + up_PWM;


    //运动到A点
    Yuntaiz_AB_Move(A_x,A_y,10);  
    //运动到B点
    Yuntaiz_AB_Move(B_x,B_y,10);
    //运动到C点
    Yuntaiz_AB_Move(C_x,C_y,10);
    //运动到D点
    Yuntaiz_AB_Move(D_x,D_y,10);
    //运动到A点
    Yuntaiz_AB_Move(A_x,A_y,10);  
        
    //Yuntai_A4_Track();
}

//题目4
void Problem4(void){
    Yuntaiz_AB_Move(left_PWM,up_PWM,5);//斜着移动到最左上角  
    uint16_t  A_x,A_y,B_x,B_y,C_x,C_y,D_x,D_y;
    //向K210请求4个点的坐标
    uint8_t data_str[255]={0};
    sprintf((char*)data_str, "id:%d,x:%d,y:%d",6,0,0);
    Usart2_SendString(data_str);
    while(K210_Flag == 0)HAL_Delay(10);  //等待K210回应
    Buzzer_ShortBeep();
    K210_Flag = 0;
    A_x = K210_data[0];
    A_y = K210_data[1];
    B_x = K210_data[2];
    B_y = K210_data[3];
    C_x = K210_data[4];
    C_y = K210_data[5];
    D_x = K210_data[6];
    D_y = K210_data[7];  
    
    
    //计算坐标映射
    float ka = (float)Screen_Width/(float)K210_Width;
    float kb = (float)Screen_Height/(float)K210_Height;
    
    //计算A点坐标
    A_x = left_PWM -round((A_x - K210_left)*ka);
    A_y = round((A_y - K210_up))*kb + up_PWM;
    

    //计算B点坐标
    B_x = left_PWM- round((B_x - K210_left)*ka);
    B_y = round((B_y - K210_up))*kb + up_PWM;
     
     
    //计算C点坐标
    C_x = left_PWM- round((C_x - K210_left)*ka);
    C_y = round((C_y - K210_up))*kb + up_PWM;

    //计算D点坐标
    D_x = left_PWM-  round((D_x - K210_left)*ka) ;
    D_y = round((D_y - K210_up))*kb + up_PWM;
    
    
    int time =400;
    //运动到A点
    Yuntaiz_AB_Move_2(A_x,A_y,300);  
    HAL_Delay(100);
    //运动到B点
    Yuntaiz_AB_Move_2(B_x,B_y,time);
    HAL_Delay(100);
    //运动到C点
    Yuntaiz_AB_Move_2(C_x,C_y,time);
    HAL_Delay(100);
    //运动到D点
    Yuntaiz_AB_Move_2(D_x,D_y,time);
    HAL_Delay(100);
    //运动到A点
    Yuntaiz_AB_Move_2(A_x,A_y,time);  
    

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
    case 0:
            last_Flag = Problem_Flag;
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
        
    case 4:
         if(last_Flag != Problem_Flag)
        {
            Problem4();
            last_Flag = Problem_Flag;
        }
        break;
    case 9:
        //未校准
        if(Calibration_Flag ==0 )
        {
            //进行校准
            System_Calibration();
        }
        else
        {
            if (HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin) == 1) pwm_B -= 1;
            //下
            if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == 1) pwm_B += 1;
            //左
            if (HAL_GPIO_ReadPin(KEY_6_GPIO_Port, KEY_6_Pin) == 1) pwm_A += 1;
            //右
            if (HAL_GPIO_ReadPin(KEY_7_GPIO_Port, KEY_7_Pin) == 1) pwm_A -= 1;
            HAL_Delay(1);
            Yuntai_Control();
        }
        //云台手动移动模式
        // 上

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

 
