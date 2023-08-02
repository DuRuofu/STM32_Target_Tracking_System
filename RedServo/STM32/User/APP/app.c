#include "app.h"



/**
 * @description: 系统应用初始化
 * @return {*}
 */
void App_Init(void)
{
    OLED_Init();          // 0.96oled初始化
    OLED_Clear();
    USART_IT_Config();    // 总串口接收初始化
    Yuntai_Init();        // 云台初始化


    HAL_TIM_Base_Start_IT(&htim4); // 启动定时器4 开始数据采样,执行控制
}


/**
 * @description: 系统应用循环任务
 * @return {*}
 */
void App_Task(void)
{
    Menu_Refresh(); // 刷新菜单
    


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


