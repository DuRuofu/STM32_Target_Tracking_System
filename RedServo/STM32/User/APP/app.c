#include "app.h"

//题目选择标志位
extern uint8_t Problem_Flag = 0;




extern uint16_t left_PWM = 0;
extern uint16_t right_PWM = 0;
extern uint16_t up_PWM = 0;
extern uint16_t down_PWM = 0;


extern uint16_t pwm_A ;
extern uint16_t pwm_B ;




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
}



//题目1
void Problem1(void){
        //执行一次
        printf("题目1执行");
        Yuntaiz_A(1494);   // 云台丝滑移动
        Yuntaiz_B(1791);   // 云台丝滑移动
}

//题目2
void Problem2(void){
    //执行一次
    uint8_t i = 0;
    if(i == 0)
    {
        Yuntai_Init();   // 复位
        //开始运转 A舵机负责左右  B舵机负责上下
        SERVO_PWMB_Set(up_PWM);   //运动到最上方
        SERVO_PWMA_Set(right_PWM);   //运动到最右方
        HAL_Delay(1000);
        SERVO_PWMB_Set(down_PWM);   //运动到最下方
        SERVO_PWMA_Set(left_PWM);   //运动到最左方
        HAL_Delay(1000);
        SERVO_PWMB_Set(up_PWM);   //运动到最上方
        SERVO_PWMA_Set(right_PWM);   //运动到最右方
        HAL_Delay(1000);
        i = 1;
    }
}


//题目3
void Problem3(void){

}

//题目4
void Problem4(void){

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
            //Problem1();

            last_Flag = Problem_Flag;
        }
        break;
    case 2:
        if(last_Flag != Problem_Flag)
        {
            //Problem2();

            last_Flag = Problem_Flag;
        }
        break;
    case 3:
    
        break;
    case 4:
    
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
            //Yuntai_Init();   // 云台控制 
        }
    }
}


