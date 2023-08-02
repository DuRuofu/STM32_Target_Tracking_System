#include "Buzzer.h"

//在这里修改蜂鸣器引脚 ，或者使用cubemx生成的宏定义（引脚命名为Buzzer_Pin）
// #define Buzzer_Pin_Pin GPIO_PIN_11
// #define Buzzer_Pin_GPIO_Port GPIOB

/**
  * @brief  蜂鸣器初始化
  * @param  无
  * @retval 无
  */
void Buzzer_Init(void)
{
    //引脚初始化
}


/**
  * @brief  蜂鸣器控制
  * @param  无
  * @retval 无
  */
void Buzzer_Control(uint8_t status)
{
    //蜂鸣器控制
    if (status == 0)
    {
        HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_SET);
    }
    else if (status == 1)
    {
        HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_RESET);
    }
    
}


//蜂鸣器短响
void Buzzer_ShortBeep(void)
{
    Buzzer_Control(1);
    HAL_Delay(30);
    Buzzer_Control(0);
}


