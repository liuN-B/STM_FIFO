#include <reg51.h>
#include "multi_timer.h"

Timer timer1 ;
Timer timer2 ;
/*���ڶ�ʱ10s�ļ�����*/
int Counter = 0 ;
/*���ݰ���ԭ��ͼ����λ��P3^3*/
sbit LED = P3^3 ;

/*����Ƶ��Ϊ12M*/
#define FOSC 12000000L
/*ָ���ٶ�Ϊ12T*/
#define command_speed 12
/*��multi_timer�����Ķ�ʱ��1��ʱʱ��  ��λ:ms*/
#define TIMER_TIMEOUT_500MS 500
/*��multi_timer�����Ķ�ʱ��2��ʱʱ��  ��λ:ms*/
#define TIMER_TIMEOUT_1S 1000

void timer0_init(void);
void timer1_callback(void);
void timer2_callback(void);

void main(void)
{	 
   LED = 0;
   timer0_init();
   timer_init(&timer1, timer1_callback, TIMER_TIMEOUT_500MS, TIMER_TIMEOUT_500MS);
   timer_init(&timer2, timer2_callback, TIMER_TIMEOUT_1S, TIMER_TIMEOUT_1S);
   timer_start(&timer1);
   timer_start(&timer2);
   while(1)
   {
   	   timer_loop();
   }
}

/*multi_timer�ص�����1����*/
void timer1_callback(void)
{
   /*LED�Ƶ�ƽ��ת*/
   LED = !LED ;
}
/*multi_timer�ص�����2����*/
void timer2_callback(void)
{
	/*������������10���Ժ�ɾ�����д����������ʱ��
	  ��������0����LED��ƽ��Ϊ1������
	*/
	++Counter ;
	if(Counter == 10)
	{
		Counter = 0 ;
		LED = 1 ;
		timer_stop(&timer1);
		timer_stop(&timer2);
	}
}

/*Ӳ����ʱ����ʼ��*/
void timer0_init(void)                
{
	TMOD = 0x00;
    TH0 = (65536-FOSC/command_speed/1000) >> 8;
    TL0 = (65536-FOSC/command_speed/1000);
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

/*����ϵͳ��ʱ������1ms�Ķ�ʱ�ж�*/
void timer0() interrupt 1 
{
	TH0 = (65536-FOSC/command_speed/1000) >> 8;
    TL0 = (65536-FOSC/command_speed/1000);
	/*multi_timer����������*/
	timer_ticks();
}  
