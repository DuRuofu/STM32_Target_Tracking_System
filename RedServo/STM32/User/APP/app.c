#include "app.h"
#include "Buzzer.h"
#include "at24c02.h"

//题目选择标志位
extern uint8_t Problem_Flag = 0;


//中心位置处参数
extern uint16_t Centre_A = 1489;
extern uint16_t Centre_B = 1574;

// 舵机（左上位置）
extern uint16_t left_PWM = 1638;
extern uint16_t up_PWM =1424;



// 舵机B （右下位置）
extern uint16_t right_PWM = 1346;
extern uint16_t down_PWM = 1722;


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
}



//题目1
void Problem1(void){
        //执行一次
        pwm_A =  Centre_A;
        pwm_B =  Centre_B;
           
        Yuntaiz_A_Move(pwm_A,1);   // 云台丝滑移动
        Yuntaiz_B_Move(pwm_B,1);   // 云台丝滑移动
}

//题目2
void Problem2(void){
    
        Yuntaiz_AB_Move(Centre_A,Centre_B,2);   //运动到中间
        Yuntaiz_AB_Move(left_PWM,up_PWM,2);//斜着移动到最左上角  
        Yuntaiz_A_Move(right_PWM,2);   //运动到最右方
        Yuntaiz_B_Move(down_PWM,2);   //运动到最下方
        Yuntaiz_A_Move(left_PWM,2);   //运动到最左方
        Yuntaiz_B_Move(up_PWM,2);   //运动到最上方
        Yuntaiz_A_Move(right_PWM,2);   //运动到最右方
        Yuntaiz_AB_Move(Centre_A,Centre_B,2);   //运动到中间
   
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
        if(last_Flag != Problem_Flag)
        {
            //Problem3();
            //Problem4();
            last_Flag = Problem_Flag;
        }
        break;
        
    case 4:
         if(last_Flag != Problem_Flag)
        {
            
            last_Flag = Problem_Flag;
        }
        break;
    case 9:
        //舵机移动模式
        // 上
        if (HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin) == 1) pwm_B -= 1;
        //下
        if (HAL_GPIO_ReadPin(KEY_5_GPIO_Port, KEY_5_Pin) == 1) pwm_B += 1;
        //左
        if (HAL_GPIO_ReadPin(KEY_6_GPIO_Port, KEY_6_Pin) == 1) pwm_A += 1;
        //右
        if (HAL_GPIO_ReadPin(KEY_7_GPIO_Port, KEY_7_Pin) == 1) pwm_A -= 1;
        HAL_Delay(1);
        Yuntai_Control();
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


