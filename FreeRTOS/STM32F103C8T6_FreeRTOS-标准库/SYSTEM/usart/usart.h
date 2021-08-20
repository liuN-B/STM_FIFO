#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define EN_ALIENTEK_USART1_IRQHandler    0 //正点原子STM32F103RCT6
#define EN_yeFire_USART1_IRQHandler      1 //野火霸道者STM32F103ZET6
#define EN_JiXiaoXin_USART1_IRQHandler   0 //计小新STM32F103ZET6

#if EN_ALIENTEK_USART1_IRQHandler
#define USART_REC_LEN  			200  	//定义最大接收字节数 200	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
#endif

#if EN_JiXiaoXin_USART1_IRQHandler
extern u8 aRxBuffer[100];
extern u8 RxCounter;
extern u8 ReceiveState;
#endif

void uart_init(u32 bound);

#endif
