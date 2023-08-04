#include "at24c02.h"


// #define AT24C02_SDA_Pin GPIO_PIN_10
// #define AT24C02_SDA_GPIO_Port GPIOB
// #define AT24C02_SCL_Pin GPIO_PIN_11
// #define AT24C02_SCL_GPIO_Port GPIOB

#define I2C_SCL_SET AT24C02_SCL_GPIO_Port->BSRR = AT24C02_SCL_Pin
#define I2C_SCL_CLR AT24C02_SCL_GPIO_Port->BRR = AT24C02_SCL_Pin
 
#define I2C_SDA_SET AT24C02_SDA_GPIO_Port->BSRR = AT24C02_SDA_Pin
#define I2C_SDA_CLR AT24C02_SDA_GPIO_Port->BRR = AT24C02_SDA_Pin
#define I2C_SDA_IN AT24C02_SDA_GPIO_Port->IDR &AT24C02_SDA_Pin
 
#define pr_err(...)
#define pr_dbg(...)
 
#define I2C_TIME 6
 
 
 


void delay_us(uint32_t nus)
{
 uint32_t temp;
 SysTick->LOAD = 9*nus;
 SysTick->VAL=0X00;//清空计数器
 SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
 do
 {
  temp=SysTick->CTRL;//读取当前倒计数值
 }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
     SysTick->CTRL=0x00; //关闭计数器
    SysTick->VAL =0X00; //清空计数器
}

 
//产生IIC起始信号
void IIC_Start_1(void)
{
    if (!(I2C_SDA_IN)) //释放SDA
    {
        I2C_SCL_CLR;
        delay_us(I2C_TIME / 2);
        I2C_SDA_SET;
        delay_us(I2C_TIME / 2); //保证起始信号为高电平
    }
 
    I2C_SCL_SET; //保证为高电平
    delay_us(I2C_TIME);
 
    I2C_SDA_CLR; // START:when CLK is high,DATA change form high to low
    delay_us(I2C_TIME);
    I2C_SCL_CLR;
    delay_us(I2C_TIME / 2);
}
//产生IIC停止信号
void IIC_Stop_1(void)
{
    delay_us(I2C_TIME / 2);
    I2C_SCL_CLR;
    delay_us(I2C_TIME / 2);
    I2C_SDA_CLR; // STOP:when CLK is high DATA change form low to high
    delay_us(I2C_TIME);
 
    I2C_SCL_SET;
    delay_us(I2C_TIME / 2);
    I2C_SDA_SET; //发送I2C总线结束信号
    delay_us(I2C_TIME / 2);
}
 
//产生ACK应答
void IIC_Ack(void)
{
    I2C_SDA_CLR;
    delay_us(I2C_TIME / 2);
    I2C_SCL_SET;
    delay_us(I2C_TIME);
    I2C_SCL_CLR;
    delay_us(I2C_TIME / 2);
}
//不产生ACK应答
void IIC_NAck(void)
{
    I2C_SDA_SET;
    delay_us(I2C_TIME / 2);
    I2C_SCL_SET;
    delay_us(I2C_TIME);
    I2C_SCL_CLR;
    delay_us(I2C_TIME / 2);
}
// IIC发送一个字节
//返回从机有无应答
// 1，有应答
// 0，无应答  CLK结束为低电平
bool IIC_Send_Byte(uint8_t txd)
{
    uint8_t t, ack;
    delay_us(I2C_TIME / 2); //
    I2C_SCL_CLR;            //拉低时钟开始数据传输
    delay_us(I2C_TIME / 2); //
    for (t = 0; t < 8; t++)
    {
        delay_us(I2C_TIME / 2);
        if (txd & 0x80)
        {
            I2C_SDA_SET;
        }
        else
        {
            I2C_SDA_CLR;
        }
        txd <<= 1;
        delay_us(I2C_TIME / 2); //对TEA5767这三个延时都是必须的
        I2C_SCL_SET;
        delay_us(I2C_TIME);
        I2C_SCL_CLR; //发出数据
    }
    //释放SDA
    delay_us(I2C_TIME / 2);
    I2C_SDA_SET;
    delay_us(I2C_TIME / 2);
    I2C_SCL_SET;
    delay_us(I2C_TIME); //等待应答周期
    ack = I2C_SDA_IN;
    I2C_SCL_CLR;
    delay_us(I2C_TIME / 2); //
    if (!ack)
    {
        return true;
    }
    else
    {
        return false; //应答失败
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    delay_us(I2C_TIME / 2);
    I2C_SCL_CLR;
 
    for (i = 0; i < 8; i++)
    {
        delay_us(I2C_TIME / 2);
        I2C_SCL_SET;
        delay_us(I2C_TIME / 2);
        receive <<= 1;
        if (I2C_SDA_IN)
            receive++;
        delay_us(I2C_TIME / 2);
        I2C_SCL_CLR;
        delay_us(I2C_TIME / 2);
    }
    if (!ack)
        IIC_NAck(); //发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}
 
#define CHECK_ACK(X)                                \
    ack = X;                                        \
    if (ack == false)                               \
    {                                               \
       ; \
        break;                                      \
    }
 
// addr 地址8字节对齐 一次 一次写入不得超过8字节 写入间隔不得小于10ms
bool I2C_Write(uint8_t devAdd, uint8_t addr, uint8_t data[], int len)
{
    bool ack = false;
    IIC_Start_1();
    do
    {
        CHECK_ACK(IIC_Send_Byte(devAdd));
 
        CHECK_ACK(IIC_Send_Byte(addr)); //发送低地址
 
        for (int i = 0; i < len; i++)
        {
            CHECK_ACK(IIC_Send_Byte(data[i])); //发送字节
        }
    } while (0);
    IIC_Stop_1(); //产生一个停止条件
    return ack;
}
 
bool I2C_Read(uint8_t devAdd, uint8_t nAddr, uint8_t data[], uint8_t len)
{
    bool ack = false;
    IIC_Start_1();
    do
    {
        CHECK_ACK(IIC_Send_Byte(devAdd)); //发送器件地址0XA0,写数据
 
        CHECK_ACK(IIC_Send_Byte(nAddr));
 
        IIC_Start_1();
        CHECK_ACK(IIC_Send_Byte(devAdd + 1)); //进入接收模式
 
        for (int i = 0; i < len - 1; i++)
        {
            data[i] = IIC_Read_Byte(1);
        }
        data[len - 1] = IIC_Read_Byte(0);
    } while (0);
    IIC_Stop_1(); //产生一个停止条件
    return ack;
}