#include "bsp.h"

const char buf1[] = "接收到串口命令1\r\n";
const char buf2[] = "接收到串口命令2\r\n";
const char buf3[] = "接收到串口命令3\r\n";
const char buf4[] = "接收到串口命令4\r\n";

int main(void)
{
    uint8_t  read[100] = {0} ;	
    bsp_Init();    

    while(1)
    {
        /* 接收到的串口命令处理 */
        if (UartGetChar(read)!=0)
        {
			UartSendBuf((uint8_t *)read, sizeof(read)/sizeof(uint8_t));
            switch (read[0])
            {
                case '1':
                    UartSendBuf((uint8_t *)buf1, strlen(buf1));
                    break;

                case '2':
                    UartSendBuf((uint8_t *)buf2, strlen(buf2));
                    break;

                case '3':
                    UartSendBuf((uint8_t *)buf3, strlen(buf3));
                    break;

                case '4':
                    UartSendBuf((uint8_t *)buf4, strlen(buf4));
                    break;

                default:
                    break;
            }
        }
    }
}


/***************************** 智果芯 www.zhiguoxin.cn (END OF FILE) *********************************/
