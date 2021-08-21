#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"


//�ⲿ�жϳ�ʼ�� SRAMоƬ��INT���Ž���PA6����
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();               //����GPIOAʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_6;                //PA0 ���ǰ���WK_UP
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //�����ش���
    GPIO_Initure.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    //�ж���0-PA0
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,0,0);       //��ռ���ȼ�Ϊ0�������ȼ�Ϊ0
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             //ʹ���ж���0
}


//�жϷ�����
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);		//�����жϴ����ú���
}



//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    
}
