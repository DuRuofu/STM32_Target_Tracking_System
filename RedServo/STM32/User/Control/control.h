#ifndef __CONTROL_H
#define __CONTROL_H
#include "main.h"

#include "debug.h"

void Motor_Speed_Read(void);
void Car_PID_Ctrl(void);
void Set_PID_Target(float temp_val);
void Set_PID(int32_t kp, int32_t ki, int32_t kd);
void  Find_CCD_Zhongzhi(uint16_t *ADV);


#endif // !__CONTROL_H
