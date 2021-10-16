#include "bsp.h"

int main(void)
{	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init();   				//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口

    while(1)
    {

    }
}

/***************************** 智果芯 www.zhiguoxin.cn (END OF FILE) *********************************/

