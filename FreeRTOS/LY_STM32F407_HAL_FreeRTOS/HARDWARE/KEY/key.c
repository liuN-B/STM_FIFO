#include "key.h"
#include "delay.h"


//������ʼ������
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();           //����GPIOEʱ��

    
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4; //PE2,3,4
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{
	//û��key_up���ᵼ�°��°������ɿ�֮ǰ����ε���KEY_Scanʱ��ÿ�ζ����⵽����
	//�������ˣ�����key_up����һ�ε���KEY_Scanʱ���ؼ�ֵ�����漸�ε���ʱ��ͨ��
	//key_up��⵽�Ѿ����ع�һ�μ�ֵ�ˣ����ٷ��ؼ�ֵ	
	
    static u8 key_up=1;     //�����ɿ���־λ
    if(mode==1)key_up=1;    //֧������
    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1)) //��⵽�հ��½��룬����ǰ�ס���Ų������
    {
        delay_ms(10);
        key_up=0;  //�ͷű�־λ
        if(KEY0==0)       return KEY0_PRES;//1
        else if(KEY1==0)  return KEY1_PRES;//2
        else if(KEY2==0)  return KEY2_PRES;//3
        else if(WK_UP==1) return WKUP_PRES;//4       
    }else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1;
    return 0;   //�ް�������
}
