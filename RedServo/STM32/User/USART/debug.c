#include "debug.h"
#include "string.h"
#include "yuntai.h"
#define RXBUFFERSIZE  256     //最大接收字节数

//定义串口句柄,使用串口1
#define UART_HANDLE huart1

//定义数据缓冲区
uint8_t RxBuffer[RXBUFFERSIZE];
uint8_t Uart_RxBuffer;      //接收中断缓冲
uint8_t Uart_Rx_Cnt = 0;     //接收缓冲计数

uint16_t data[4]={0};
  // 初始位置  //舵机中值
extern uint16_t pwm_A ;
extern uint16_t pwm_B ;



void Debug_Init(void)
{
  /*串口硬件配置代码(使用cudeMX则不需要此部分)
  Init the GPIO of USART1
  */
  //使能 USART1 的接收中断
  __HAL_UART_ENABLE_IT(&UART_HANDLE,UART_IT_RXNE);
  //开启 USART1 的连续接收中断，并指定接收缓冲区的地址和长度
  HAL_UART_Receive_IT(&UART_HANDLE,&Uart_RxBuffer,1);
}

//串口1接收完成回调函数
void UART1_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
 
	if(Uart_Rx_Cnt >= 255)  //溢出判断
	{
		Uart_Rx_Cnt = 0;
		memset(RxBuffer,0x00,sizeof(RxBuffer));
		HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)"数据溢出", 10,0xFFFF); 	
	}
	else
	{
		RxBuffer[Uart_Rx_Cnt++] = Uart_RxBuffer;   

		if((RxBuffer[Uart_Rx_Cnt-1] == 0x0A)&&(RxBuffer[Uart_Rx_Cnt-2] == 0x0D)) //判断结束位
		{
      //这里可以写多字节消息的判断
     // 解析k210数据
      sscanf((const char *)RxBuffer, "%d,%d\r\n", &data[0], &data[1]);
      pwm_A =data[0];
      pwm_B =data[1];
      //printf("%d,%d",Position_error[0],Position_error[1]);


      //复位
			Uart_Rx_Cnt = 0;
			memset(RxBuffer,0x00,sizeof(RxBuffer)); //清空数组
		}
	}
	
	HAL_UART_Receive_IT(&UART_HANDLE, (uint8_t *)&Uart_RxBuffer, 1);   //因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用

}

//串口1错误回调函数(主要用来清除溢出中断)
void UART1_ErrorCallback(UART_HandleTypeDef *huart)
{
  if(HAL_UART_ERROR_ORE)
  {
     uint32_t temp = huart->Instance->SR;
     temp = huart->Instance->DR;
  }
}

/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UART_HANDLE,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
   
}

/**
  * 函数功能: 重定向c库函数printf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&UART_HANDLE, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
 
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&UART_HANDLE, &ch, 1, 0xffff);
  return ch;
}
