#ifndef _USART_FIFO_H_
#define _USART_FIFO_H_

#include "bsp.h"

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	
	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;	/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;	/* 还未读取的新数据个数 */

}UART_T;

extern UART_T g_tUart1;

void bsp_InitUart(void);
uint8_t UartGetChar(uint8_t *_pByte);
void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen);


#endif

/***************************** 智果芯 www.zhiguoxin.cn (END OF FILE) *********************************/
