#include "app.h"

uint8_t mpu6050_data_flag; // 是否正常读取数据

// 编码器计数值
int16_t car_speed_1 = 0;
int16_t car_speed_2 = 0;

//*********直立环参数*********
int32_t Car_zero = -5.5f;    // 小车机械零点
int32_t Upright_Kp = -700.0f *0.6f;  // 直立环PID参数Kp
int32_t Upright_Kd = 1.1f   *0.6f;// 直立环PID参数Kd
int32_t PWM_Upright = 0.0f; // 直立环PWM
//*********速度环参数*********
int32_t Speed_Kp = 1.0f;  // 速度环PID参数
int32_t Speed_Ki = 0.005f;  // 速度环PID参数
int32_t PWM_Speed = 0.0f; // 速度环PWM

//*********转向环参数*********

//*********最终输出PWM*********
int16_t PWM_Min = 0;  //电机死区
int16_t PWM_MAX = 7200;   // PWM最大值(限幅)
int16_t PWM_Output_1 = 0; // 最终输出的PWM值(电机1)
int16_t PWM_Output_2 = 0; // 最终输出的PWM值(电机2)

float pitch, roll, yaw;    // 欧拉角
short gyrox, gyroy, gyroz; // 陀螺仪原始数据

/**
 * @description: 系统应用初始化
 * @return {*}
 */
void App_Init(void)
{
    OLED_Init();          // 0.96oled初始化
    Motor_Init();         // 电机初始化
    USART_IT_Config();    // 总串口接收初始化
    Encoder_Count_Init(); // 编码器计数初始化

    // MPU_Init();//MPU6050初始化
    while (MPU_Init() != 0)
    {
        printf("MPU初始化\r\n");
    } // DMP库初始化
    while (mpu_dmp_init() != 0)
    {
        printf("DMP库初始化:%d\r\n", mpu_dmp_init());
    } // DMP库初始化

    // 开始数据采样
    HAL_TIM_Base_Start_IT(&htim4); // 启动定时器4

    // 电机控制
    Motor_Ctrl(400, 1);

}


/**
 * @description: 系统应用循环任务
 * @return {*}
 */
void App_Task(void)
{
    LED_Toggle(); // 测试LED
    // HAL_Delay(100);
    // 测试串口
    // 显示电机速度
    OLED_ShowSignedNum(1, 1, car_speed_1, 5);
    OLED_ShowSignedNum(1, 8, car_speed_2, 5);
    // MPU6050值
    OLED_ShowSignedNum(2, 1, roll, 5);
    OLED_ShowSignedNum(2, 6, pitch, 5);
    OLED_ShowSignedNum(2, 11, yaw, 5);

    //debug
    printf("X:%.1f°  Y:%.1f°  Z:%.1f° \r\n", roll, pitch, yaw); // 串口1输出采集信息
    
    printf("PWM1:%d  PWM2:%d \r\n", PWM_Output_1, PWM_Output_2); // 串口1输出采集信息
    HAL_Delay(50);
}


// 定时器中断回调函数(1ms一次)
uint16_t encoder_count = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim4) // 判断中断是否来自于定时器4
    {
        // 读取MPU6050数据任务
        //MPU6050_Data_Read();
        // 编码器测速任务
        encoder_count++;
        if (encoder_count == 5)
        {
            // 计数值清零
            encoder_count = 0;
            // 读取编码器速度
            Motor_Speed_Read();
        }
        // 小车运动任务
        //Car_PID_Ctrl();
    }
}


/**
 * @description: 读取MPU6050数据
 * @return {*}
 */
void MPU6050_Data_Read(void)
{
    mpu6050_data_flag = mpu_dmp_get_data(&pitch, &roll, &yaw); // 得到角度数据
    // MPU_Get_Accelerometer(&aacx,&aacy,&aacz); //得到加速度传感器数据
    MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz); // 得到陀螺仪数据
}

//读取电机速度
void Motor_Speed_Read(void)
{
    // 读取编码器速度
    car_speed_1 = Encoder_Value(1);
    car_speed_2 = Encoder_Value(2);
    // 放大10倍
    car_speed_1 = car_speed_1 * 10;
    car_speed_2 = car_speed_2 * 10;
    // 清零编码器计数值
    Encoder_Count_Clear(1);
    Encoder_Count_Clear(2);
}

// pid部分

/**
 * @description: 直立环 , 控制小车保持直立
 * @param {float} Angle  采集到的实际角度值
 * @param {float} Gyro   采集到的实际角速度值
 * @return {*}
 */
int PID_Upright(float Angle, float Gyro)
{
    float err;
    int16_t PWM_Upright;
    err = Car_zero - Angle;                             // 期望值-实际值，这里期望小车平衡，因此期望值就是机械中值
    PWM_Upright = Upright_Kp * err + Gyro * Upright_Kd; // 计算平衡控制的电机PWM
    return PWM_Upright;
}

/**
 * @description: 速度环
 * @param {int16_t} car_speed_1
 * @param {int16_t} car_speed_2
 * @return {*}
 */
int16_t PID_Speed(int16_t car_speed_1, int16_t car_speed_2)
{
    static float Velocity,Encoder_Least,Encoder,Movement;
    static float Encoder_Integral;
   //=============速度PI控制器=======================//  
    Encoder_Least =(car_speed_1+car_speed_2)-0;      
    //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零） 
    Encoder *= 0.7;          //===一阶低通滤波器       
    Encoder += Encoder_Least*0.3;   //===一阶低通滤波器    
    Encoder_Integral +=Encoder; //===积分出位移 积分时间：10ms
    if(Encoder_Integral>10000)    Encoder_Integral=10000;   
    //===积分限幅
    if(Encoder_Integral<-10000)    Encoder_Integral=-10000;   
    //===积分限幅  
    Velocity=Encoder*Speed_Kp+Encoder_Integral*Speed_Ki;  
    //===速度控制  
    if(pitch<-40||pitch>40)   Encoder_Integral=0;   
    //===电机关闭后清除积分
    return Velocity;
}

// 转向环

/**
 * @description: 小车PID总输出
 * @return {*}
 */
void Car_PID_Ctrl(void)
{
    // 直立环
    PWM_Upright = PID_Upright(pitch, gyroy);
    // 速度环
    PWM_Speed = PID_Speed(car_speed_1, car_speed_2);

    // 转向环

    // 最终输出PWM
    PWM_Output_1 = PWM_Upright+PWM_Speed;
    PWM_Output_2 = PWM_Upright+PWM_Speed;

    // 限幅
    if (PWM_Output_1 >= PWM_MAX)
    {
        PWM_Output_1 = PWM_MAX;
    }
    if (PWM_Output_1 <= -PWM_MAX)
    {
        PWM_Output_1 = -PWM_MAX;
    }
    if (PWM_Output_2 >= PWM_MAX)
    {
        PWM_Output_2 = PWM_MAX;
    }
    if (PWM_Output_2 <= -PWM_MAX)
    {
        PWM_Output_2 = -PWM_MAX;
    }
    if ((pitch >= 80) || (pitch <= -80))
    {
        PWM_Output_1 = 0;
        PWM_Output_2 = 0;
    }
    // 作用到电机
    Motor_Ctrl(PWM_Output_1, 1); // 作用到电机
    Motor_Ctrl(PWM_Output_2, 2); // 作用到电机
}