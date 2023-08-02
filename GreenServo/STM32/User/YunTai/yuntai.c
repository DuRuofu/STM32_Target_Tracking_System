#include "yuntai.h"
#include "debug.h"


#define Servo_middle_A 1500  //舵机中值
#define Servo_middle_B 1800
#define Servo_MAX_A 2500    //最大位置
#define Servo_MAX_B 2200
#define Servo_MIN_A 500     //最小位置
#define Servo_MIN_B 800

// 初始位置
#define Servo_middle_A 1500
#define Servo_middle_B 1800



extern int32_t Servo_Kp = 0.5;            // 舵机比例系数
extern int32_t Servo_Ki = 0.8;            // 舵机积分系数
extern int32_t Servo_Kd = 0;            // 舵机微分系数



extern uint16_t pwm_A = Servo_middle_A;
extern uint16_t pwm_B = Servo_middle_B; 



extern int8_t Position_error[2];






void Yuntai_Init()
{
    PWM_Init();
    SERVO_PWMA_Set(Servo_middle_A);
    SERVO_PWMB_Set(Servo_middle_B);
}

void Yuntai_PID()
{
    static int16_t L_x_err=0,L_L_x_err=0,L_y_err=0,L_L_y_err=0;

    int8_t x_err = Position_error[0];
    int8_t y_err = Position_error[1];
    printf("x_err:%d,y_err:%d\r\n",x_err,y_err);

    // 死区限制，防止抖动
    if (x_err > -8 && x_err < 8) x_err = 0;
    if (y_err > -8 && y_err < 8) y_err = 0;


    // 进行PID计算
    // -0.2 -0.8 -0.1
    // 0.2 0.8 0.1
    pwm_A += -0 * (x_err-L_x_err)+ (-0.7)*x_err+ (40.15) * (x_err - 2 * L_x_err + L_L_x_err);
    pwm_B += 0.1 * (y_err-L_y_err) + (0.7)*y_err + 0.15* (y_err - 2 * L_y_err + L_L_y_err);
    
    //保存误差值
    L_x_err = x_err;
    L_L_x_err = L_x_err;
    
    L_y_err = y_err;
    L_L_x_err = L_y_err;

    //输出限幅
    if (pwm_A > Servo_MAX_A) pwm_A = Servo_MAX_A;
    if (pwm_A < Servo_MIN_A) pwm_A = Servo_MIN_A;
    if (pwm_B > Servo_MAX_B) pwm_B = Servo_MAX_B;
    if (pwm_B < Servo_MIN_B) pwm_B = Servo_MIN_B;

    // 进行控制
    SERVO_PWMA_Set(pwm_A);
    SERVO_PWMB_Set(pwm_B);
}


