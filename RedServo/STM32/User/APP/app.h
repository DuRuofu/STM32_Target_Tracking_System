#ifndef __APP_H
#define __APP_H

#include "main.h"
#include "tim.h"
#include "led.h"
#include "oled.h"
#include "gui.h"
#include "control.h"
#include "yuntai.h"
#include "serial_it_config.h"



void App_Init(void);
void App_Task(void);
void MPU6050_Data_Read(void);
void Motor_Speed_Read(void);
void Car_PID_Ctrl(void);
void Update_Parameters(void);
//У׼ģʽ
void System_Calibration(void);
#endif // !__APP_H
