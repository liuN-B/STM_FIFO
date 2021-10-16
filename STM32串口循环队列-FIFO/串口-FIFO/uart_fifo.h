#ifndef _USART_FIFO_H_
#define _USART_FIFO_H_

#include "bsp.h"

/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */
	
	__IO uint16_t usTxWrite;	/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;		/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;	/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;	/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;		/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;	/* ��δ��ȡ�������ݸ��� */

}UART_T;

extern UART_T g_tUart1;

void bsp_InitUart(void);
uint8_t UartGetChar(uint8_t *_pByte);
void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen);


#endif

/***************************** �ǹ�о www.zhiguoxin.cn (END OF FILE) *********************************/
