#include "bsp.h"

int main(void)
{	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init();   				//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������

    while(1)
    {

    }
}

/***************************** �ǹ�о www.zhiguoxin.cn (END OF FILE) *********************************/

