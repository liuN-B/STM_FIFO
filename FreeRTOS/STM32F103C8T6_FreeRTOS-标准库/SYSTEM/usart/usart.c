#include "sys.h"
#include "usart.h"

/*
�����ڲ���������ʱTC��־λΪ0����TC��������ɣ�����λ����λΪ1��
��ʾ USART_DR�ڵ������Ѿ�����������ˡ�������������λ���жϣ�
�������жϡ���λҲ���������㷽ʽ��
	1���� USART_SR��дUSART_DR��
	2��ֱ�����λд 0��
*/
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	 
#pragma import(__use_no_semihosting)                
struct __FILE { int handle; }; 
FILE __stdout; 
void _sys_exit(int x) {	x = x; } 
//�ض���fputc������ʹprintf���������fputc�����UART,  ����ʹ�ô���1(USART1)
int fputc(int ch, FILE *f) 
{
	//��SR�Ĵ����е�TC��־
	USART_ClearFlag(USART1,USART_FLAG_TC);//�����벻ҪҲ����
	//����һ���ֽ����ݵ�����
	USART_SendData(USART1, (uint8_t) ch);//�ȼ��� USART1->DR = (u8) ch;		
	//ѭ������,ֱ��������ϡ�while (0);����ѭ����while(1);���벻������ִ��
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//�ȼ���while((USART1->SR&0X40)==0);
	return (ch);
}

void uart_init(u32 bound)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//ʹ��USART1ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE); //ʹ�ܴ���1 

}

#if EN_ALIENTEK_USART1_IRQHandler
/*RXNE�������ݼĴ����ǿգ���Ĭ�������Ҳ����û�����ݽ���ʱ���üĴ���ֵΪ0
�������ݱ����յ�ʱ����λ�ᱻ�Զ���1���Ϳ��Զ������ˡ���ʱ������Ҫ���ľ�
�Ǿ���ȥ��ȡUSART_DR��ͨ����USART_DR���Խ���λ���㣬Ҳ�������λд0��ֱ�������*/	
//һ��������2���ֽ�,һ��Ӣ����ĸ(���ִ�Сд)��һ���ֽ�,���ı��ռ�����ֽ�, Ӣ�ı��ռһ���ֽ�.
u8 USART_RX_BUF[USART_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�. 
u16 USART_RX_STA=0;//����״̬���	
/*
�����յ��ӵ��Է����������ݣ��ѽ��յ������ݱ����� USART_RX_BUF �У�ͬʱ��
����״̬�Ĵ�����USART_RX_STA���м������յ�����Ч���ݸ��������յ��س����س���
��ʾ�� 2 ���ֽ���ɣ�0X0D �� 0X0A���ĵ�һ���ֽ� 0X0D ʱ�����������������ӣ��ȴ�
0X0A �ĵ���������� 0X0A û������������Ϊ��ν���ʧ�ܣ����¿�ʼ��һ�ν��ա���
��˳�����յ� 0X0A������ USART_RX_STA �ĵ� 15 λ���������һ�ν��գ����ȴ���
λ����������������Ӷ���ʼ��һ�εĽ��գ�������ٳ�û���յ� 0X0D����ô�ڽ�����
�ݳ��� USART_REC_LEN ��ʱ����ᶪ��ǰ������ݣ����½��ա�
����������������������������������������������������������������������������������������������������������������������������������
|						USART_RX_STA
����������������������������������������������������������������������������������������������������������������������������������
|		bit15   	|		bit14  		|		bit13~0		 	|
����������������������������������������������������������������������������������������������������������������������������������
|	������ɱ�־	|	���յ�0x0D��־	|	���յ���Ч���ݵĸ���|
����������������������������������������������������������������������������������������������������������������������������������
*/
void USART1_IRQHandler(void) //����1�жϷ������
	{
	u8 ucTemp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE)!= RESET) //��������˽����ж�RXNE���Զ���λΪ1  (���յ������ݱ�����0x0d 0x0a��β)
	{
		ucTemp =USART_ReceiveData(USART1);//��ȡ���յ�������		
		if((USART_RX_STA&0x8000)==0)//����δ��ɣ�ÿ�ν����жϷ�������Ҫ�ж�bit15������ɱ�־λ�Ƿ���λΪ1��
		{
			if(USART_RX_STA&0x4000)//���յ��˻س�����Ҳ����0x0D �����bit15������ɱ�־λδ��λΪ1����Ҫ�ж��Ƿ���յ��˻س����ĵ�һ���ֽڡ�USART_RX_STA��0����1��
			{
				if(ucTemp!=0x0A)//û�н��յ��س����ĵڶ����ֽ�0x0A(��ȷ�����յ�0x0d��Ҫ�ж��Ƿ���ܵ���0x0a������ȷ��������ȷ)
					USART_RX_STA=0;//���մ���,���¿�ʼ
				else 
					USART_RX_STA|=0x8000;//��������ˣ���������������λ�ģ����յ�0x0dҲ���յ���0x0a˵��������ȷ������ɣ���bit15������ɱ�־λ��1�� 
			}
			else //û�н��յ��˻س�����Ҳ����û���յ�0x0D,�ж����˼�����������
			{	
				if(ucTemp==0x0D)//�ж����˼����������ݣ�ҲҪ���ж�һ���Ƿ���ܵ��˽��յ��س����ĵ�һ���ֽ�
					USART_RX_STA|=0x4000;//������յ��س����ĵ�һ���ֽ�0x0D,��Ҫ�����յ��س���bit14λ0x0D��־λ��1
				else//�ж����˼����������ݣ������û�н��յ��س����ĵ�һ���ֽڣ���Ҫ�ѽ��յ������ݴ浽������USART_RX_BUF[]
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=ucTemp;//�ѼĴ���USART_RX_STA��bit13~0�����㴦��������Ž��յ�������ucTemp
					USART_RX_STA++;//ÿ�ν��յ�һ�������ݾ�Ҫ��USART_RX_STA��ֵ��1
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;//�������ݴ���,���¿�ʼ����
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
		ucTemp = USART_ReceiveData(USART1);//��ȡ���յ������ݸ�ucTemp
		USART_SendData(USART1,ucTemp);//����ȡ�������ݷ��͸�PC��λ��  
	}	 
}
#endif

#if EN_JiXiaoXin_USART1_IRQHandler

u8 aRxBuffer[100];
u8 RxCounter;
u8 ReceiveState;

void USART1_IRQHandler(void)
{
	u8 Clear=Clear;//���ֶ��巽��������������������"û���õ�"����	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // ������յ�1���ֽ�
	{ 	
		aRxBuffer[RxCounter++] = USART_ReceiveData(USART1);// �ѽ��յ����ֽڱ��棬�����ַ��1
	} 
	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)// ������յ�1֡����
	{
		Clear=USART1->SR;// ��SR�Ĵ���
		Clear=USART_ReceiveData(USART1);// ��DR�Ĵ���(�ȶ�SR�ٶ�DR������Ϊ�����IDLE�ж�)
		ReceiveState=1;// ��ǽ��յ���1֡����
	}
}
#endif

