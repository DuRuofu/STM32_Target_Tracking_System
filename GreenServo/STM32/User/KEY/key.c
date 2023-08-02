#include "key.h"
#include "led.h"
#include "gui.h"
#include "yuntai.h"
#include "Buzzer.h"

#define KEY1_Pin KEY_1_Pin
#define KEY2_Pin KEY_2_Pin
#define KEY3_Pin KEY_3_Pin
#define KEY4_Pin KEY_4_Pin

extern uint8_t GUI_Menu;


/**
 * @description: 按键初始化 (使用CubeMX自动生成的宏定义，就不用写这个函数了)
 * @return {*}
 */
void Key_Init(void)
{

}

void Key_1_Callback(void)
{


    OLED_Clear(); 
    // 按键1按下的处理代码
    if (GUI_Menu < 3)
    {
        GUI_Menu++;
    }
    else
    {
        GUI_Menu = 0;
    }
    
}


void Key_2_Callback(void)
{
    OLED_Clear(); 
    // 按键2按下的处理代码
    if (GUI_Menu > 0)
    {
        GUI_Menu--;
    }
    else
    {
        GUI_Menu = 3;
    }
}


void Key_3_Callback(void)
{
    LED_Toggle(1);

}

void Key_4_Callback(void)
{

    LED_Toggle(1);
}




//
/**
 * @description: 按键检测，外部中断回调函数
 * @param {uint16_t} GPIO_Pin
 * @return {*}
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
       Buzzer_ShortBeep();
    /* Prevent unused argument(s) compilation warning */
    UNUSED(GPIO_Pin);
    /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_GPIO_EXTI_Callback could be implemented in the user file
    */
    // 按键按下
    if(GPIO_Pin == KEY1_Pin)
    {
        // 按键1按下的处理代码
        Key_1_Callback();
    }
    else if(GPIO_Pin == KEY2_Pin)
    {
        // 按键2按下的处理代码
        Key_2_Callback();
    }
    else if(GPIO_Pin == KEY3_Pin)
    {
        // 按键2按下的处理代码
        Key_3_Callback();
    }
     else if(GPIO_Pin == KEY4_Pin)
    {
        // 按键2按下的处理代码
        Key_4_Callback();
    }
}



