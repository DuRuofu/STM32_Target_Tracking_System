/*
 * @Author: DuRuofu duruofu@qq.com
 * @Date: 2023-07-27 09-48-56
 * @LastEditors: DuRuofu
 * @LastEditTime: 2023-07-29 12-30-15
 * @FilePath: \MDK-ARMd:\duruofu\Project\Avoidance_Car\project\STM32ZET6\Users\OLED\OLEDGUI\gui.c
 * @Description: 0.96OLED配合的GUI简单GUI界面
 * Copyright (c) 2023 by duruofu@foxmail.com All Rights Reserved. 
 */

#include "gui.h"
#include <stdio.h>

extern uint8_t GUI_Menu = 0;
extern int32_t Car_Speed_A ;    // 电机实际速度（电机A）
extern int32_t Car_Speed_B ;    // 电机实际速度（电机B）
extern uint16_t CCD_Value[128];

// PID值
extern int32_t Kp;     // 比例系数
extern int32_t Ki;      // 积分系数
extern int32_t Kd;      // 微分系数
extern int32_t Target;  // 目标值

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
            OLED_ShowString(40,0,"GUI  ",16);
            sprintf((char *)str_buff1, "GUI_Menu:%5d",GUI_Menu); 
            break;
        }
        case 1:
        {
            OLED_ShowString(40,0,"Motor",16);
            sprintf((char *)str_buff1, "GUI_Menu:%5d",GUI_Menu); 
            sprintf((char *)str_buff2, "Speed_A:%3d",Car_Speed_A);
            sprintf((char *)str_buff3, "Speed_B:%3d",Car_Speed_B);
            break;
        }
        case 2:
        {
            OLED_ShowString(40,0,"PID  ",16);
            sprintf((char *)str_buff1, "GUI_Menu:%5d",GUI_Menu); 
            sprintf((char *)str_buff2, "Kp:%5d",Kp);
            sprintf((char *)str_buff3, "Ki:%5d",Ki);
            sprintf((char *)str_buff4, "Kd:%5d",Kd);
            sprintf((char *)str_buff5, "Target:%5d",Target);
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
