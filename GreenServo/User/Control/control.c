#include "control.h"

//电机PWM输出限幅
#define MOTOR_PWM_MAX 7200

//-------------------上位机调参用-------------------
// 调试用pid系数
extern int32_t Kp = -50;     // 比例系数
extern int32_t Ki = 40;      // 积分系数
extern int32_t Kd =0;//- 30;      // 微分系数
extern int32_t Cycle = 1000; // 采样周期
extern int32_t Target = 20;  // 目标值

//---------------速度环(PID)----------------
int32_t Target_Speed = 10;         // 目标速度
int32_t Kp_Speed = 5;            // 速度环比例系数
int32_t Ki_Speed = 40;             // 速度环积分系数
int32_t Kd_Speed = 5;             // 速度环微分系数
extern int32_t Car_Speed_A = 0;    // 电机实际速度（电机A）
extern int32_t Car_Speed_B = 0;    // 电机实际速度（电机B）
int32_t Car_Speed = 0;             // 电机实际速度

//---------------转向环(PD)----------------
int8_t CCD_Zhongzhi= 64;        // CCD中值
int8_t CCD_Yuzhi= 0;   //CCD阈值
int32_t Target_Direction = 0;   // 目标角度
int32_t Kp_Direction = 50;      // 角度环比例系数
int32_t Kd_Direction = 100;     // 角度环微分系数
int32_t Car_Direction = 0;      // 实际角度


// 读取电机速度
void Motor_Speed_Read(void)
{
    // 读取编码器速度(加和)
    Car_Speed_A = Encoder_Value(1) ;
    Car_Speed_B = Encoder_Value(2);
    Car_Speed=Car_Speed_A+Car_Speed_B;
    // 清零编码器计数值
    Encoder_Count_Clear(1);
    Encoder_Count_Clear(2);
}


/**
 * @description: 电机A速度环
 * @param {int32_t} target_speed
 * @param {int32_t} fact_speed
 * @return {*}
 */
int32_t PID_Speed_A(int32_t target_speed, int32_t fact_speed)
{
    static int32_t Speed_PWM_Out = 0, Err_Speed = 0, L_Speed_Err=0, L_L_Speed_Err=0; // 速度环输出
    Err_Speed = target_speed - fact_speed;           // 计算速度误差
    // 死区限制
    if (Err_Speed > -1 && Err_Speed < 1)
    {
        Err_Speed = 0;
    }
    // PID计算(增量)
     Speed_PWM_Out = Speed_PWM_Out+ Kp_Speed * (Err_Speed - L_Speed_Err) + Ki_Speed * Err_Speed + Kd_Speed * (Err_Speed - 2 * L_Speed_Err + L_L_Speed_Err);
    // 调参用
     //Speed_PWM_Out = Speed_PWM_Out +  Kp * (Err_Speed-L_Speed_Err) + Ki * Err_Speed + Kd * (Err_Speed - 2.0*L_Speed_Err + L_L_Speed_Err);
    // 限幅
    if (Speed_PWM_Out > MOTOR_PWM_MAX)
    {
        Speed_PWM_Out = MOTOR_PWM_MAX;
    }
    else if (Speed_PWM_Out < -MOTOR_PWM_MAX)
    {
        Speed_PWM_Out = -MOTOR_PWM_MAX;
    }
    // 保存误差
    L_Speed_Err = Err_Speed;     // 保存上一次的误差
    L_L_Speed_Err = L_Speed_Err; // 保存上上次的误差
    return Speed_PWM_Out;
}


/**
 * @description: 电机B速度环
 * @param {int32_t} target_speed
 * @param {int32_t} fact_speed
 * @return {*}
 */
int32_t PID_Speed_B(int32_t target_speed, int32_t fact_speed)
{
    static int32_t Speed_PWM_Out = 0, Err_Speed = 0, L_Speed_Err=0, L_L_Speed_Err=0; // 速度环输出
    Err_Speed = target_speed - fact_speed;           // 计算速度误差
    // 死区限制
    if (Err_Speed > -1 && Err_Speed < 1)
    {
        Err_Speed = 0;
    }
    // PID计算(增量)
    Speed_PWM_Out = Speed_PWM_Out+ Kp_Speed * (Err_Speed - L_Speed_Err) + Ki_Speed * Err_Speed + Kd_Speed * (Err_Speed - 2 * L_Speed_Err + L_L_Speed_Err);
    // 调参用
     //Speed_PWM_Out = Speed_PWM_Out +  Kp * (Err_Speed-L_Speed_Err) + Ki * Err_Speed + Kd * (Err_Speed - 2.0*L_Speed_Err + L_L_Speed_Err);
    // 限幅
    if (Speed_PWM_Out > MOTOR_PWM_MAX)
    {
        Speed_PWM_Out = MOTOR_PWM_MAX;
    }
    else if (Speed_PWM_Out < -MOTOR_PWM_MAX)
    {
        Speed_PWM_Out = -MOTOR_PWM_MAX;
    }
    // 保存误差
    L_Speed_Err = Err_Speed;     // 保存上一次的误差
    L_L_Speed_Err = L_Speed_Err; // 保存上上次的误差
    return Speed_PWM_Out;
}

/**
 * @description: 转向环(待修改)
 * @param {int32_t} target_direction
 * @param {int32_t} fact_direction
 * @return {*}
 */
int32_t PID_Direction(int32_t target_direction, int32_t fact_direction)
{
    DEBUG_info("","PID_Direction");
    static int32_t Direction_PWM_Out = 0, Err_Direction = 0,L_Direction_Err=0; // 转向环输出
    Err_Direction = target_direction - fact_direction;       // 计算转向误差
    DEBUG_info("","Err_Direction = %d", Err_Direction);
    // 死区限制
    if (Err_Direction > -1 && Err_Direction < 1)
    {
        Err_Direction = 0;
    }
    Direction_PWM_Out = Kp * Err_Direction + Kd * (Err_Direction - L_Direction_Err);
    //Direction_PWM_Out = Kp_Direction * Err_Direction + Kd_Direction * (Err_Direction - L_Direction_Err)

    if (Direction_PWM_Out > MOTOR_PWM_MAX)
    {
        Direction_PWM_Out = MOTOR_PWM_MAX;
    }
    else if (Direction_PWM_Out < -MOTOR_PWM_MAX)
    {
        Direction_PWM_Out = -MOTOR_PWM_MAX;
    }
    // 保存误差
    L_Direction_Err = Err_Direction;     // 保存上一次的误差
    DEBUG_info("","Direction_PWM_Out = %d", Direction_PWM_Out);
    return Direction_PWM_Out;
}



/**
 * @description: 小车PID总输出
 * @return {*}
 */
void Car_PID_Ctrl(void)
{
    // 速度环
    int32_t PWM_Out_Speed_A = PID_Speed_A(Target_Speed, Car_Speed_A);
    int32_t PWM_Out_Speed_B = PID_Speed_B(Target_Speed, Car_Speed_B);

    // 转向环
    int32_t PWM_Out_Direction = PID_Direction(64, CCD_Zhongzhi);

    // 最终输出PWM
    int32_t PWM_Output_A = PWM_Out_Speed_A + PWM_Out_Direction;
    int32_t PWM_Output_B = PWM_Out_Speed_B - PWM_Out_Direction;

    // 限幅
    if (PWM_Output_A >= MOTOR_PWM_MAX)
    {
        PWM_Output_A = MOTOR_PWM_MAX;
    }
    else if (PWM_Output_A <= -MOTOR_PWM_MAX)
    {
        PWM_Output_A = -MOTOR_PWM_MAX;
    }
    else if (PWM_Output_B >= MOTOR_PWM_MAX)
    {
        PWM_Output_B = MOTOR_PWM_MAX;
    }
    else if (PWM_Output_B <= -MOTOR_PWM_MAX)
    {
        PWM_Output_B = -MOTOR_PWM_MAX;
    }
    //作用到电机
    Motor_Ctrl(PWM_Output_A, 1); // 作用到电机
    Motor_Ctrl(PWM_Output_B, 2); // 作用到电机
}




// 函数功能：线性CCD取中值
// 入口参数：无
// 返回  值：无
void  Find_CCD_Zhongzhi(uint16_t *ADV)
{ 
	static uint16_t i,j,Left,Right,Last_CCD_Zhongzhi;
	static uint16_t value1_max,value1_min;
	
	 value1_max=ADV[0];  //动态阈值算法，读取最大和最小值
    for(i=5;i<123;i++)   //两边各去掉5个点
    {
    if(value1_max<=ADV[i])
    value1_max=ADV[i];
    }
    value1_min=ADV[0];  //最小值
    for(i=5;i<123;i++) 
    {
    if(value1_min>=ADV[i])
    value1_min=ADV[i];
    }
   CCD_Yuzhi=(value1_max+value1_min)/2;	  //计算出本次中线提取的阈值
	for(i = 5;i<118; i++)   //寻找左边跳变沿
	{
		if(ADV[i]>CCD_Yuzhi&&ADV[i+1]>CCD_Yuzhi&&ADV[i+2]>CCD_Yuzhi&&ADV[i+3]<CCD_Yuzhi&&ADV[i+4]<CCD_Yuzhi&&ADV[i+5]<CCD_Yuzhi)
		{	
			Left=i;
			break;	
		}
	}
	for(j = 118;j>5; j--)//寻找右边跳变沿
  {
		if(ADV[j]<CCD_Yuzhi&&ADV[j+1]<CCD_Yuzhi&&ADV[j+2]<CCD_Yuzhi&&ADV[j+3]>CCD_Yuzhi&&ADV[j+4]>CCD_Yuzhi&&ADV[j+5]>CCD_Yuzhi)
		{	
		  Right=j;
		  break;	
		}
  }
	CCD_Zhongzhi=(Right+Left)/2;//计算中线位置
	if(myabs(CCD_Zhongzhi-Last_CCD_Zhongzhi)>70)   //计算中线的偏差，如果太大
    CCD_Zhongzhi=Last_CCD_Zhongzhi;    //则取上一次的值
    Last_CCD_Zhongzhi=CCD_Zhongzhi;  //保存上一次的偏差
}

int myabs(int a)
{ 		   
	  int temp;
		if(a<0) temp=-a;  
	  else temp=a;
	  return temp;
}


