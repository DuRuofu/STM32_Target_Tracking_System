#ifndef __APP_H
#define __APP_H

#include "main.h"
#include "tim.h"
#include "led.h"
#include "oled.h"
#include "motor.h"
#include "encoder.h"
#include "serial_it_config.h"
#include "IIC.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"


void App_Init(void);
void App_Task(void);
void MPU6050_Data_Read(void);
void Motor_Speed_Read(void);
void Car_PID_Ctrl(void);


#endif // !__APP_H
