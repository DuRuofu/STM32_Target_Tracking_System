#ifndef I2C_LIBRARY_H
#define I2C_LIBRARY_H

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

void delay_us(uint32_t nus);

void IIC_Start_1(void);
void IIC_Stop_1(void);
void IIC_Ack(void);
void IIC_NAck(void);
bool IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);

bool I2C_Write(uint8_t devAdd, uint8_t addr, uint8_t data[], int len);
bool I2C_Read(uint8_t devAdd, uint8_t nAddr, uint8_t data[], uint8_t len);

#endif /* I2C_LIBRARY_H */