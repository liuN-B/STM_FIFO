#include "bsp.h"
#include "uart_fifo.h"

/* 串口1的GPIO  PA9, PA10*/
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
static uint8_t g_TxBuf1[1024];		/* 发送缓冲区 */
static uint8_t g_RxBuf1[1024];		/* 接收缓冲区 */

		
static void Uart_FIFO_Init(void);
static void Uart_GPIO_Config(void);


/*
*********************************************************************************************************
*	函 数 名: bsp_InitUart
*	功能说明: 初始化串口硬件，并对全局变量赋初值.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	
	Uart_FIFO_Init();		/* 必须先初始化全局变量,再配置硬件 */
	Uart_GPIO_Config();		/* 配置串口的硬件参数(波特率等) */
}
/*
*********************************************************************************************************
*	函 数 名: Uart_FIFO_Init
*	功能说明: 初始化串口相关的变量
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void Uart_FIFO_Init(void)
{

	g_tUart1.uart = USART1;						/* STM32 串口设备 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* 发送缓冲区指针 */
	g_tUart1.pRxBuf = g_RxBuf1;					/* 接收缓冲区指针 */
	g_tUart1.usTxBufSize = 1024;	   			/* 发送缓冲区大小 */
	g_tUart1.usRxBufSize = 1024;				/* 接收缓冲区大小 */
	g_tUart1.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart1.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart1.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart1.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart1.usRxCount = 0;						/* 接收到的新数据个数*/
	g_tUart1.usTxCount = 0;						/* 待发送的数据个数 */
}



/*
*********************************************************************************************************
*	函 数 名: Uart_GPIO_Config
*	功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-H7开发板
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
UART_HandleTypeDef UartHandle;	
static void Uart_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;	

	/* 使能 GPIO TX/RX 时钟 */
	USART1_TX_GPIO_CLK_ENABLE();
	USART1_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART1_CLK_ENABLE();	

	/* 配置TX引脚 */
	GPIO_InitStruct.Pin       = USART1_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* 配置RX引脚 */
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

    
	__HAL_USART_CLEAR_FLAG(&UartHandle, USART_FLAG_TC);  /* 清除TC发送完成标志 */
	__HAL_USART_CLEAR_FLAG(&UartHandle, USART_FLAG_RXNE);/* 清除RXNE接收标志 */
	__HAL_USART_ENABLE_IT(&UartHandle, USART_IT_RXNE);   /* 使能接收数据寄存器非空中断 */
	
	
	/* 配置NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn); 

}

/*
*********************************************************************************************************
*	函 数 名: comSendChar
*	功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*	形    参: 
*			  _ucByte: 待发送的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void UartSendChar( uint8_t *_ucaBuf)
{	
	uint16_t i;

	/* 如果发送缓冲区已经满了，则等待缓冲区空 */
	while(1)
	{
		__IO uint16_t usCount;

		usCount = g_tUart1.usTxCount;

		if (usCount < g_tUart1.usTxBufSize)
		{
			break;/*如果发送缓冲区没有满就跳出去，让发送缓冲区填满*/
		}
		else if(usCount == g_tUart1.usTxBufSize)/* 数据已填满缓冲区 */
		{
			if(((&g_tUart1)->uart->CR1 & USART_CR1_TXEIE) == 0)
			{
				/*寄存器写法：SET_BIT((&g_tUart1)->uart->CR1, USART_CR1_TXEIE);*/
				__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TXE); /* 使能发送中断（缓冲区空） */
			}  
		}
	}

	/* 将新数据填入发送缓冲区 */
	g_tUart1.pTxBuf[g_tUart1.usTxWrite] = _ucaBuf[i];

	DISABLE_INT();
	if (++g_tUart1.usTxWrite >= g_tUart1.usTxBufSize)
	{
		g_tUart1.usTxWrite = 0;
	}
	g_tUart1.usTxCount++;
	ENABLE_INT();

	/*寄存器写法：SET_BIT((&g_tUart1)->uart->CR1, USART_CR1_TXEIE);	使能发送中断（缓冲区空） */
	__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TXE);/* 使能发送中断（缓冲区空） */	
	
}

/*
*********************************************************************************************************
*	函 数 名: comGetChar
*	功能说明: 从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
*	形    参: 
*			  _pByte: 接收到的数据存放在这个地址
*	返 回 值: 0 表示无数据, 1 表示读取到有效字节
*********************************************************************************************************
*/
uint8_t UartGetChar(uint8_t *_pByte)
{
	
	uint16_t usCount;

	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
	DISABLE_INT();
	usCount = g_tUart1.usRxCount;
	ENABLE_INT();

	/* 如果读和写索引相同，则返回0 */
	if (usCount == 0)	/* 已经没有数据 */
	{
		return 0;
	}
	else
	{
		*_pByte = g_tUart1.pRxBuf[g_tUart1.usRxRead];/* 从串口接收FIFO取1个数据 */

		/* 改写FIFO读索引 */
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
*	函 数 名: UartSend
*	功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void UartSendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
		while (1)
		{
			__IO uint16_t usCount;

			usCount = g_tUart1.usTxCount;

			if (usCount < g_tUart1.usTxBufSize)
			{
				break;/*如果发送缓冲区没有满就跳出去，让发送缓冲区填满*/
			}
			else if(usCount == g_tUart1.usTxBufSize)/* 数据已填满缓冲区 */
			{
				if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_TXE)== RESET)
				{
					__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TXE);  /* 使能发送中断（缓冲区空） */
				}  
			}
		}

		/* 将新数据填入发送缓冲区 */
		g_tUart1.pTxBuf[g_tUart1.usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++g_tUart1.usTxWrite >= g_tUart1.usTxBufSize)
		{
			g_tUart1.usTxWrite = 0;
		}
		g_tUart1.usTxCount++;
		ENABLE_INT();
	}

	__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TXE); /* 使能发送中断（缓冲区空） */
}


void USART1_IRQHandler(void)
{	
	
	/* 处理接收缓冲区中断  */
	if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_RXNE)!= RESET)
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
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

	/* 处理发送缓冲区空中断 */
	if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_TXE)!= RESET)
	{

		if (g_tUart1.usTxCount == 0)
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			CLEAR_BIT(g_tUart1.uart->CR1, USART_CR1_TXEIE);

			/* 使能数据发送完毕中断 */
			__HAL_UART_ENABLE_IT(&UartHandle,UART_IT_TC);/* 使能数据发送完毕中断 */
			
		}
		else
		{			
			/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
			g_tUart1.uart->DR = g_tUart1.pTxBuf[g_tUart1.usTxRead];
			if (++g_tUart1.usTxRead >= g_tUart1.usTxBufSize)
			{
				g_tUart1.usTxRead = 0;
			}
			g_tUart1.usTxCount--;
		}

	}
	/* 数据bit位全部发送完毕的中断 */
	if(__HAL_UART_GET_FLAG(&UartHandle,UART_FLAG_TC)!= RESET)
	{

		if (g_tUart1.usTxCount == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			__HAL_USART_CLEAR_FLAG(&UartHandle,USART_FLAG_TC);/*发送完毕中断*/
		}
		else
		{
			/* 正常情况下，不会进入此分支 */
			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
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
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xffff);
	return ch;
}

/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: comClearTxFifo
*	功能说明: 清零串口发送缓冲区
*	形    参: 
*	返 回 值: 无
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
*	函 数 名: comClearRxFifo
*	功能说明: 清零串口接收缓冲区
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void comClearRxFifo(void)
{

	g_tUart1.usRxWrite = 0;
	g_tUart1.usRxRead  = 0;
	g_tUart1.usRxCount = 0;
}

/***************************** 智果芯 www.zhiguoxin.cn (END OF FILE) *********************************/


