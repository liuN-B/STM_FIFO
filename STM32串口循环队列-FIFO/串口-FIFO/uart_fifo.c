#include "bsp.h"
#include "uart_fifo.h"

/* ����1��GPIO  PA9, PA10*/
#define USART1_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()

#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_AF                     GPIO_AF7_USART1

#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_AF                     GPIO_AF7_USART1

static UART_T g_tUart1;
static uint8_t g_TxBuf1[1024];		/* ���ͻ����� */
static uint8_t g_RxBuf1[1024];		/* ���ջ����� */

		
static void Uart_FIFO_Init(void);
static void Uart_GPIO_Config(void);


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitUart
*	����˵��: ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	
	Uart_FIFO_Init();		/* �����ȳ�ʼ��ȫ�ֱ���,������Ӳ�� */
	Uart_GPIO_Config();		/* ���ô��ڵ�Ӳ������(�����ʵ�) */
}
/*
*********************************************************************************************************
*	�� �� ��: Uart_FIFO_Init
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Uart_FIFO_Init(void)
{

	g_tUart1.uart = USART1;						/* STM32 �����豸 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
	g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
	g_tUart1.usTxBufSize = 1024;	   			/* ���ͻ�������С */
	g_tUart1.usRxBufSize = 1024;				/* ���ջ�������С */
	g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usTxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usRxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxCount = 0;						/* ���յ��������ݸ���*/
	g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
}



/*
*********************************************************************************************************
*	�� �� ��: Uart_GPIO_Config
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32-H7������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
UART_HandleTypeDef UartHandle;	
static void Uart_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;	

	/* ʹ�� GPIO TX/RX ʱ�� */
	USART1_TX_GPIO_CLK_ENABLE();
	USART1_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART1_CLK_ENABLE();	

	/* ����TX���� */
	GPIO_InitStruct.Pin       = USART1_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* ����RX���� */
	GPIO_InitStruct.Pin = USART1_RX_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);		

	UartHandle.Instance        = USART1;
	UartHandle.Init.BaudRate   = 115200;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;	
	HAL_UART_Init(&UartHandle); 

    
	__HAL_USART_CLEAR_FLAG(&UartHandle, USART_FLAG_TC);  /* ���TC������ɱ�־ */
	__HAL_USART_CLEAR_FLAG(&UartHandle, USART_FLAG_RXNE);/* ���RXNE���ձ�־ */
	__HAL_USART_ENABLE_IT(&UartHandle, USART_IT_RXNE);   /* ʹ�ܽ������ݼĴ����ǿ��ж� */
	
	
	/* ����NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn); 

}

/*
*********************************************************************************************************
*	�� �� ��: comSendChar
*	����˵��: �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: 
*			  _ucByte: �����͵�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UartSendChar( uint8_t *_ucaBuf)
{	
	uint16_t i;

	/* ������ͻ������Ѿ����ˣ���ȴ��������� */
	while(1)
	{
		__IO uint16_t usCount;

		usCount = g_tUart1.usTxCount;

		if (usCount < g_tUart1.usTxBufSize)
		{
			break;/*������ͻ�����û����������ȥ���÷��ͻ���������*/
		}
		else if(usCount == g_tUart1.usTxBufSize)/* ���������������� */
		{
			if(((&g_tUart1)->uart->CR1 & USART_CR1_TXEIE) == 0)
			{
				/*�Ĵ���д����SET_BIT((&g_tUart1)->uart->CR1, USART_CR1_TXEIE);*/
				__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TXE); /* ʹ�ܷ����жϣ��������գ� */
			}  
		}
	}

	/* �����������뷢�ͻ����� */
	g_tUart1.pTxBuf[g_tUart1.usTxWrite] = _ucaBuf[i];

	DISABLE_INT();
	if (++g_tUart1.usTxWrite >= g_tUart1.usTxBufSize)
	{
		g_tUart1.usTxWrite = 0;
	}
	g_tUart1.usTxCount++;
	ENABLE_INT();

	/*�Ĵ���д����SET_BIT((&g_tUart1)->uart->CR1, USART_CR1_TXEIE);	ʹ�ܷ����жϣ��������գ� */
	__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TXE);/* ʹ�ܷ����жϣ��������գ� */	
	
}

/*
*********************************************************************************************************
*	�� �� ��: comGetChar
*	����˵��: �ӽ��ջ�������ȡ1�ֽڣ��������������������ݾ��������ء�
*	��    ��: 
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
*********************************************************************************************************
*/
uint8_t UartGetChar(uint8_t *_pByte)
{
	
	uint16_t usCount;

	/* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ������� */
	DISABLE_INT();
	usCount = g_tUart1.usRxCount;
	ENABLE_INT();

	/* �������д������ͬ���򷵻�0 */
	if (usCount == 0)	/* �Ѿ�û������ */
	{
		return 0;
	}
	else
	{
		*_pByte = g_tUart1.pRxBuf[g_tUart1.usRxRead];/* �Ӵ��ڽ���FIFOȡ1������ */

		/* ��дFIFO������ */
		DISABLE_INT();
		if (++g_tUart1.usRxRead >=g_tUart1.usRxBufSize)
		{
			g_tUart1.usRxRead = 0;
		}
		g_tUart1.usRxCount--;
		ENABLE_INT();
		return 1;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: UartSend
*	����˵��: ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�����������Ϻ��Զ��رշ����ж�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* ������ͻ������Ѿ����ˣ���ȴ��������� */
		while (1)
		{
			__IO uint16_t usCount;

			usCount = g_tUart1.usTxCount;

			if (usCount < g_tUart1.usTxBufSize)
			{
				break;/*������ͻ�����û����������ȥ���÷��ͻ���������*/
			}
			else if(usCount == g_tUart1.usTxBufSize)/* ���������������� */
			{
				if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_TXE)== RESET)
				{
					__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TXE);  /* ʹ�ܷ����жϣ��������գ� */
				}  
			}
		}

		/* �����������뷢�ͻ����� */
		g_tUart1.pTxBuf[g_tUart1.usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++g_tUart1.usTxWrite >= g_tUart1.usTxBufSize)
		{
			g_tUart1.usTxWrite = 0;
		}
		g_tUart1.usTxCount++;
		ENABLE_INT();
	}

	__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TXE); /* ʹ�ܷ����жϣ��������գ� */
}


void USART1_IRQHandler(void)
{	
	
	/* ������ջ������ж�  */
	if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_RXNE)!= RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		uint8_t ch;

		ch = READ_REG(g_tUart1.uart->DR);
		g_tUart1.pRxBuf[g_tUart1.usRxWrite] = ch;
		if (++g_tUart1.usRxWrite >= g_tUart1.usRxBufSize)
		{
			g_tUart1.usRxWrite = 0;
		}
		if (g_tUart1.usRxCount < g_tUart1.usRxBufSize)
		{
			g_tUart1.usRxCount++;
		}
	}

	/* �����ͻ��������ж� */
	if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_TXE)!= RESET)
	{

		if (g_tUart1.usTxCount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			CLEAR_BIT(g_tUart1.uart->CR1, USART_CR1_TXEIE);

			/* ʹ�����ݷ�������ж� */
			__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TC);/* ʹ�����ݷ�������ж� */
			
		}
		else
		{			
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			g_tUart1.uart->DR = g_tUart1.pTxBuf[g_tUart1.usTxRead];
			if (++g_tUart1.usTxRead >= g_tUart1.usTxBufSize)
			{
				g_tUart1.usTxRead = 0;
			}
			g_tUart1.usTxCount--;
		}

	}
	/* ����bitλȫ��������ϵ��ж� */
	if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_TC)!= RESET)
	{

		if (g_tUart1.usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			__HAL_USART_CLEAR_FLAG(&UartHandle,USART_FLAG_TC);/*��������ж�*/
		}
		else
		{
			/* ��������£��������˷�֧ */
			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
			g_tUart1.uart->DR =g_tUart1.pTxBuf[g_tUart1.usTxRead];
			if (++g_tUart1.usTxRead >= g_tUart1.usTxBufSize)
			{
				g_tUart1.usTxRead = 0;
			}
			g_tUart1.usTxCount--;
		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xffff);
	return ch;
}

/*
*********************************************************************************************************
*	�� �� ��: fgetc
*	����˵��: �ض���getc��������������ʹ��getchar�����Ӵ���1��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fgetc(FILE *f)
{

	uint8_t ch;
	HAL_UART_Receive(&UartHandle,&ch, 1, 0xffff);
	return ch;
}

/*
*********************************************************************************************************
*	�� �� ��: comClearTxFifo
*	����˵��: ���㴮�ڷ��ͻ�����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comClearTxFifo()
{
	g_tUart1.usTxWrite = 0;
	g_tUart1.usTxRead = 0;
	g_tUart1.usTxCount = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: comClearRxFifo
*	����˵��: ���㴮�ڽ��ջ�����
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comClearRxFifo(void)
{

	g_tUart1.usRxWrite = 0;
	g_tUart1.usRxRead  = 0;
	g_tUart1.usRxCount = 0;
}

/***************************** �ǹ�о www.zhiguoxin.cn (END OF FILE) *********************************/


