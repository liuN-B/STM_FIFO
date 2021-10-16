#include "bsp.h"
#include "usart.h"	

struct rt_ringbuffer ring_buf;      //����1���ջ��ζ��� 
static uint8_t 	USART1_Len = 0;     //��ŵ�ǰ���ڽ������ݴ�ŵ�λ��
static uint8_t  s_USART1_RxBuf[256];//���ڽ��ջ�����


UART_HandleTypeDef huart1;

void uart_init(u32 bound){

	rt_ringbuffer_init(&ring_buf,s_USART1_RxBuf,256);	

	huart1.Instance=USART1;					    //USART1
	huart1.Init.BaudRate=bound;				    //������
	huart1.Init.WordLength=UART_WORDLENGTH_8B;  //�ֳ�Ϊ8λ���ݸ�ʽ
	huart1.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	huart1.Init.Parity=UART_PARITY_NONE;		 //����żУ��λ
	huart1.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	huart1.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&huart1);					    //HAL_UART_Init()��ʹ��UART1	
	
	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);/*ʹ�ܴ��ڽ��շǿ��ж� */	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE); /*ʹ�ܴ��ڽ��տ����ж� */
	
}



void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStruct;
	
	if(huart->Instance==USART1)//����Ǵ���1�����д���1 MSP��ʼ��
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
	
	if((__HAL_USART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)) //���շǿ��ж�
    {
		HAL_UART_Receive(&huart1, &receive_char, 1, 1000);//���յ������ݷ���receive_char		
		rt_ringbuffer_put(&ring_buf,&receive_char,1);//��receive_charѹ�뵽ring_buf��
		USART1_Len++;		
		__HAL_USART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);//������շǿ��ж�
	}
	
	if((__HAL_USART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)) //���տ����ж�
    {
	
		rt_ringbuffer_get(&ring_buf,s_USART1_RxBuf, USART1_Len);//��rring_buf�����ݶ�������s_USART1_RxBuf��
		s_USART1_RxBuf[USART1_Len] = '\0';
		printf("%s\r\n",s_USART1_RxBuf);
		USART1_Len = 0 ;//����֡���ȸ�λ
		__HAL_USART_CLEAR_IDLEFLAG(&huart1);
	}
}

//�ض���fputc���� 
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

/***************************** �ǹ�о www.zhiguoxin.cn (END OF FILE) *********************************/
