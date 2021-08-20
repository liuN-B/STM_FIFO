#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define EN_ALIENTEK_USART1_IRQHandler    0 //����ԭ��STM32F103RCT6
#define EN_yeFire_USART1_IRQHandler      1 //Ұ��Ե���STM32F103ZET6
#define EN_JiXiaoXin_USART1_IRQHandler   0 //��С��STM32F103ZET6

#if EN_ALIENTEK_USART1_IRQHandler
#define USART_REC_LEN  			200  	//�����������ֽ��� 200	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
#endif

#if EN_JiXiaoXin_USART1_IRQHandler
extern u8 aRxBuffer[100];
extern u8 RxCounter;
extern u8 ReceiveState;
#endif

void uart_init(u32 bound);

#endif
