/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-07-27 09-48-56
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-08-02 17-11-18
 * @FilePath: \Project\RedServo\STM32\User\OLED\OLEDGUI\gui.c
 * @Description: 0.96OLED配合的GUI简单GUI界面
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "gui.h"
#include <stdio.h>


//菜单页
extern uint8_t GUI_Menu = 0;
extern int32_t Servo_Kp ;            // 舵机比例系数
extern int32_t Servo_Ki ;            // 舵机积分系数
extern int32_t Servo_Kd ;            // 舵机微分系数

extern uint16_t pwm_A ;
extern uint16_t pwm_B ;

//题目标志位
extern uint8_t Problem_Flag;


//急停标志位
extern uint8_t Stop_Flag;


//消息接收标志位
extern uint8_t K210_Flag;

void Menu_Refresh(void)
{
    uint8_t str_buff1[64]= {0};
    uint8_t str_buff2[64]= {0};
    uint8_t str_buff3[64]= {0};
    uint8_t str_buff4[64]= {0};   
    uint8_t str_buff5[64]= {0};
    uint8_t str_buff6[64]= {0};
    
   //OLED_Clear(); 
   switch(GUI_Menu)
   {
        case 0:
        {
            OLED_ShowString(40,0,"Serv",16);
            sprintf((char *)str_buff1, "Problem_Flag:%1d",Problem_Flag);
            sprintf((char *)str_buff2, "Stop_Flag:%d",Stop_Flag);
            sprintf((char *)str_buff4, "K210_Flag:%d",K210_Flag);
            sprintf((char *)str_buff5, "pwm_A:%5d",pwm_A);
            sprintf((char *)str_buff6, "pwm_B:%5d",pwm_B);      
            break;
        }
        case 1:
        {
            OLED_ShowString(40,0,"Motor",16);
            sprintf((char *)str_buff1, "Problem_Flag:%1d",Problem_Flag);
            sprintf((char *)str_buff2, "Servo_Kp:%3.2f",(float)Servo_Kp);
            sprintf((char *)str_buff3, "Servo_Kp:%3.2f",(float)Servo_Ki);
            sprintf((char *)str_buff4, "Servo_Kp:%3.2f",(float)Servo_Kd);
            sprintf((char *)str_buff5, "K210_Flag:%3.2f",(float)K210_Flag);
            sprintf((char *)str_buff5, "pwm_A:%5d",pwm_A);
            sprintf((char *)str_buff6, "pwm_B:%5d",pwm_B);
            break;
        }
        case 2:
        {
            OLED_ShowString(40,0,"PID  ",16);
            sprintf((char *)str_buff1, "Problem_Flag:%1d",Problem_Flag); 

            break;
        }
        case 3:
        {
            

          
        }
    }
    OLED_ShowString(0, 2,str_buff1, 8);
    OLED_ShowString(0, 3,str_buff2, 8);
    OLED_ShowString(0, 4,str_buff3, 8);
    OLED_ShowString(0, 5,str_buff4, 8);
    OLED_ShowString(0, 6,str_buff5, 8);
    OLED_ShowString(0, 7,str_buff6, 8);
}
