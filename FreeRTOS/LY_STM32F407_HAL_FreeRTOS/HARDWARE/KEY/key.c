#include "key.h"
#include "delay.h"


//按键初始化函数
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();           //开启GPIOE时钟

    
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
	GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4; //PE2,3,4
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{
	//没有key_up，会导致按下按键再松开之前，多次调用KEY_Scan时，每次都会检测到按键
	//被按下了，有了key_up，第一次调用KEY_Scan时返回键值，后面几次调用时会通过
	//key_up检测到已经返回过一次键值了，不再返回键值	
	
    static u8 key_up=1;     //按键松开标志位
    if(mode==1)key_up=1;    //支持连按
    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1)) //检测到刚按下进入，如果是按住不放不会进入
    {
        delay_ms(10);
        key_up=0;  //释放标志位
        if(KEY0==0)       return KEY0_PRES;//1
        else if(KEY1==0)  return KEY1_PRES;//2
        else if(KEY2==0)  return KEY2_PRES;//3
        else if(WK_UP==1) return WKUP_PRES;//4       
    }else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1;
    return 0;   //无按键按下
}
