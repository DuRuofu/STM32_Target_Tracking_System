#include "key.h"
#include "led.h"
#include "gui.h"
#include "yuntai.h"
#include "Buzzer.h"

#define KEY1_Pin KEY_1_Pin
#define KEY2_Pin KEY_2_Pin
#define KEY3_Pin KEY_3_Pin
#define KEY4_Pin KEY_4_Pin
#define KEY5_Pin KEY_5_Pin
#define KEY6_Pin KEY_6_Pin
#define KEY7_Pin KEY_7_Pin
#define KEY8_Pin KEY_8_Pin


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

        //OLED_Clear();
        // // 按键1按下的处理代码
        // if (GUI_Menu < 3)
        // {
        //     GUI_Menu++;
        // }
        // else
        // {
        //     GUI_Menu = 0;
        // }

        if(Problem_Flag == 9 )return;
        //更换题目
        if (Problem_Flag < 4)
        {
            Problem_Flag++;
        }
        else
        {
            Problem_Flag = 0;
        }
        
    }



void Key_2_Callback(void)
{

}


//设置舵机调试模式的按钮
void Key_3_Callback(void)
{
    if (Problem_Flag==9)
    {
        Problem_Flag=0;
    }else
    {
        Problem_Flag=9;
    }
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

//急停
void Key_8_Callback(void){
    if(Stop_Flag == 1)
    {
        Stop_Flag=0;
    }
    else{
        Stop_Flag=1;
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
    else if(GPIO_Pin == KEY3_Pin)
    {

        // 按键3按下的处理代码
        Debounce(GPIO_Pin, Key_3_Callback);
    }
     else if(GPIO_Pin == KEY4_Pin)
    {
        // 按键4按下的处理代码
        Debounce(GPIO_Pin, Key_4_Callback);
    }
     else if(GPIO_Pin == KEY5_Pin)
    {
        // 按键5按下的处理代码
        Debounce(GPIO_Pin, Key_5_Callback);
    }
     else if(GPIO_Pin == KEY6_Pin)
    {
        // 按键6按下的处理代码
        Debounce(GPIO_Pin, Key_6_Callback);
    }
     else if(GPIO_Pin == KEY7_Pin)
    {
        // 按键7按下的处理代码
        Debounce(GPIO_Pin, Key_7_Callback);
    }
     else if(GPIO_Pin == KEY8_Pin)
    {
        DEBUG_printf("按键8");
        // 按键8按下的处理代码
        Debounce(GPIO_Pin, Key_8_Callback);
        
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



