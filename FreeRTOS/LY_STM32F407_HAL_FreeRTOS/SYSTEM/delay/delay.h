#ifndef _DELAY_H
#define _DELAY_H
#include <sys.h>	  


void delay_init(u8 SYSCLK);
void delay_ms(u32 nms);//delay_ms()�Ƕ� FreeRTOS �е���ʱ���� vTaskDelay()�ļ򵥷�װ��������ʹ�� delay_ms()��ʱ��ͻᵼ�������л���
void delay_us(u32 nms);//us ����ʱ����
void delay_xms(u32 nms);//us ����ʱ����
#endif

