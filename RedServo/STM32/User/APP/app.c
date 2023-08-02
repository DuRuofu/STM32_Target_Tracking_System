#include "app.h"
#include "Buzzer.h"
//题目选择标志位
extern uint8_t Problem_Flag = 0;


//中心位置处参数
extern uint16_t Centre_A = 1494;
extern uint16_t Centre_B = 1796;

// 舵机A（左右边框）
extern uint16_t left_PWM = 1594;
extern uint16_t right_PWM = 1389;

// 舵机B （上下边框参数）
extern uint16_t up_PWM = 1696;
extern uint16_t down_PWM = 1891;


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
        pwm_A =  1494;
        pwm_B =  1796;
           
        Yuntaiz_A(pwm_A,1);   // 云台丝滑移动
        Yuntaiz_B(pwm_B,1);   // 云台丝滑移动
}

//题目2
void Problem2(void){
    //执行一次
        //Yuntai_Init();   // 复位
        //开始运转 A舵机负责左右  B舵机负责上下
        Yuntaiz_B(up_PWM,2);   //运动到最上方
        HAL_Delay(2000);
        Yuntaiz_A(right_PWM,2);   //运动到最右方
        HAL_Delay(2000);
        Yuntaiz_B(down_PWM,2);   //运动到最下方
        HAL_Delay(2000);
        Yuntaiz_A(left_PWM,2);   //运动到最左方
        HAL_Delay(2000);
        Yuntaiz_B(up_PWM,2);   //运动到最上方
        HAL_Delay(2000);
        Yuntaiz_A(right_PWM,2);   //运动到最右方
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
            Problem1();
            Buzzer_LongBeep();
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
        }
    }
}


