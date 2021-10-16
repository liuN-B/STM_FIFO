#include "bsp.h"
#include "usart.h"	

struct rt_ringbuffer ring_buf;      //串口1接收环形队列 
static uint8_t 	USART1_Len = 0;     //存放当前串口接收数据存放的位置
static uint8_t  s_USART1_RxBuf[256];//串口接收缓冲区


UART_HandleTypeDef huart1;

void uart_init(u32 bound){

	rt_ringbuffer_init(&ring_buf,s_USART1_RxBuf,256);	

	huart1.Instance=USART1;					    //USART1
	huart1.Init.BaudRate=bound;				    //波特率
	huart1.Init.WordLength=UART_WORDLENGTH_8B;  //字长为8位数据格式
	huart1.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	huart1.Init.Parity=UART_PARITY_NONE;		 //无奇偶校验位
	huart1.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	huart1.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&huart1);					    //HAL_UART_Init()会使能UART1	
	
	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);/*使能串口接收非空中断 */	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE); /*使能串口接收空闲中断 */
	
}



void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStruct;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_USART1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

	}
}



void USART1_IRQHandler(void)                	
{ 
	uint8_t receive_char;
	
	if((__HAL_USART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)) //接收非空中断
    {
		HAL_UART_Receive(&huart1, &receive_char, 1, 1000);//将收到的数据放入receive_char		
		rt_ringbuffer_put(&ring_buf,&receive_char,1);//将receive_char压入到ring_buf中
		USART1_Len++;		
		__HAL_USART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);//清除接收非空中断
	}
	
	if((__HAL_USART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)) //接收空闲中断
    {
	
		rt_ringbuffer_get(&ring_buf,s_USART1_RxBuf, USART1_Len);//将rring_buf的数据读出到到s_USART1_RxBuf中
		s_USART1_RxBuf[USART1_Len] = '\0';
		printf("%s\r\n",s_USART1_RxBuf);
		USART1_Len = 0 ;//数据帧长度复位
		__HAL_USART_CLEAR_IDLEFLAG(&huart1);
	}
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1,&ch, 1, 0xffff);
  return ch;
}

/***************************** 智果芯 www.zhiguoxin.cn (END OF FILE) *********************************/
