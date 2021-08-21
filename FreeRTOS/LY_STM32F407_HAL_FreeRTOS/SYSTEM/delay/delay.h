#ifndef _DELAY_H
#define _DELAY_H
#include <sys.h>	  


void delay_init(u8 SYSCLK);
void delay_ms(u32 nms);//delay_ms()是对 FreeRTOS 中的延时函数 vTaskDelay()的简单封装，所以在使用 delay_ms()的时候就会导致任务切换。
void delay_us(u32 nms);//us 级延时函数
void delay_xms(u32 nms);//us 级延时函数
#endif

