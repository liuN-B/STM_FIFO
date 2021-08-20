#include "sys.h"
#include "usart.h"

/*
当串口不发送数据时TC标志位为0。当TC（发送完成），该位被置位为1，
表示 USART_DR内的数据已经被发送完成了。如果设置了这个位的中断，
则会产生中断。该位也有两种清零方式：
	1）读 USART_SR，写USART_DR。
	2）直接向该位写 0。
*/
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	 
#pragma import(__use_no_semihosting)                
struct __FILE { int handle; }; 
FILE __stdout; 
void _sys_exit(int x) {	x = x; } 
//重定向fputc函数，使printf的输出，由fputc输出到UART,  这里使用串口1(USART1)
int fputc(int ch, FILE *f) 
{
	//清SR寄存器中的TC标志
	USART_ClearFlag(USART1,USART_FLAG_TC);//这句代码不要也可以
	//发送一个字节数据到串口
	USART_SendData(USART1, (uint8_t) ch);//等价于 USART1->DR = (u8) ch;		
	//循环发送,直到发送完毕。while (0);跳出循环，while(1);代码不再向下执行
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//等价于while((USART1->SR&0X40)==0);
	return (ch);
}

void uart_init(u32 bound)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接收中断
	USART_Cmd(USART1, ENABLE); //使能串口1 

}

#if EN_ALIENTEK_USART1_IRQHandler
/*RXNE（读数据寄存器非空），默认情况下也就是没有数据接收时，该寄存器值为0
当有数据被接收到时，该位会被自动置1，就可以读出来了。这时候我们要做的就
是尽快去读取USART_DR，通过读USART_DR可以将该位清零，也可以向该位写0，直接清除。*/	
//一个汉字是2个字节,一个英文字母(不分大小写)是一个字节,中文标点占三个字节, 英文标点占一个字节.
u8 USART_RX_BUF[USART_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节. 
u16 USART_RX_STA=0;//接收状态标记	
/*
当接收到从电脑发过来的数据，把接收到的数据保存在 USART_RX_BUF 中，同时在
接收状态寄存器（USART_RX_STA）中计数接收到的有效数据个数，当收到回车（回车的
表示由 2 个字节组成：0X0D 和 0X0A）的第一个字节 0X0D 时，计数器将不再增加，等待
0X0A 的到来，而如果 0X0A 没有来到，则认为这次接收失败，重新开始下一次接收。如
果顺利接收到 0X0A，则标记 USART_RX_STA 的第 15 位，这样完成一次接收，并等待该
位被其他程序清除，从而开始下一次的接收，而如果迟迟没有收到 0X0D，那么在接收数
据超过 USART_REC_LEN 的时候，则会丢弃前面的数据，重新接收。
—————————————————————————————————————————————————————————————————
|						USART_RX_STA
—————————————————————————————————————————————————————————————————
|		bit15   	|		bit14  		|		bit13~0		 	|
—————————————————————————————————————————————————————————————————
|	接收完成标志	|	接收到0x0D标志	|	接收到有效数据的个数|
—————————————————————————————————————————————————————————————————
*/
void USART1_IRQHandler(void) //串口1中断服务程序
	{
	u8 ucTemp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE)!= RESET) //如果发生了接收中断RXNE会自动置位为1  (接收到的数据必须是0x0d 0x0a结尾)
	{
		ucTemp =USART_ReceiveData(USART1);//读取接收到的数据		
		if((USART_RX_STA&0x8000)==0)//接收未完成（每次进入中断服务函数都要判断bit15接收完成标志位是否置位为1）
		{
			if(USART_RX_STA&0x4000)//接收到了回车符，也就是0x0D （如果bit15接收完成标志位未置位为1，就要判断是否接收到了回车符的第一个字节。USART_RX_STA是0还是1）
			{
				if(ucTemp!=0x0A)//没有接收到回车符的第二个字节0x0A(在确定接收到0x0d后要判断是否接受到了0x0a，才能确定接收正确)
					USART_RX_STA=0;//接收错误,重新开始
				else 
					USART_RX_STA|=0x8000;//接收完成了（或运算是用来置位的，接收到0x0d也就收到了0x0a说明接收正确接收完成，把bit15接收完成标志位置1） 
			}
			else //没有接收到了回车符，也就是没有收到0x0D,中断来了继续接收数据
			{	
				if(ucTemp==0x0D)//中断来了继续接收数据，也要先判断一下是否接受到了接收到回车符的第一个字节
					USART_RX_STA|=0x4000;//如果接收到回车符的第一个字节0x0D,就要将接收到回车的bit14位0x0D标志位置1
				else//中断来了继续接收数据，如果还没有接收到回车符的第一个字节，就要把接收到的数据存到缓冲区USART_RX_BUF[]
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=ucTemp;//把寄存器USART_RX_STA的bit13~0做清零处理，用来存放接收到的数据ucTemp
					USART_RX_STA++;//每次接收到一个新数据就要把USART_RX_STA的值加1
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;//接收数据错误,重新开始接收
				}		 
			}
		}   		 
     } 
}
#endif

#if EN_yeFire_USART1_IRQHandler 
void USART1_IRQHandler(void)
{
	uint8_t ucTemp;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData(USART1);//读取接收到的数据给ucTemp
		USART_SendData(USART1,ucTemp);//将读取到的数据发送给PC上位机  
	}	 
}
#endif

#if EN_JiXiaoXin_USART1_IRQHandler

u8 aRxBuffer[100];
u8 RxCounter;
u8 ReceiveState;

void USART1_IRQHandler(void)
{
	u8 Clear=Clear;//这种定义方法，用来消除编译器的"没有用到"提醒	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 如果接收到1个字节
	{ 	
		aRxBuffer[RxCounter++] = USART_ReceiveData(USART1);// 把接收到的字节保存，数组地址加1
	} 
	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)// 如果接收到1帧数据
	{
		Clear=USART1->SR;// 读SR寄存器
		Clear=USART_ReceiveData(USART1);// 读DR寄存器(先读SR再读DR，就是为了清除IDLE中断)
		ReceiveState=1;// 标记接收到了1帧数据
	}
}
#endif

