#include <reg51.h>
#include "multi_timer.h"

Timer timer1 ;
Timer timer2 ;
/*用于定时10s的计数器*/
int Counter = 0 ;
/*根据板子原理图，灯位于P3^3*/
sbit LED = P3^3 ;

/*晶振频率为12M*/
#define FOSC 12000000L
/*指令速度为12T*/
#define command_speed 12
/*用multi_timer创建的定时器1定时时间  单位:ms*/
#define TIMER_TIMEOUT_500MS 500
/*用multi_timer创建的定时器2定时时间  单位:ms*/
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

/*multi_timer回调函数1调用*/
void timer1_callback(void)
{
   /*LED灯电平翻转*/
   LED = !LED ;
}
/*multi_timer回调函数2调用*/
void timer2_callback(void)
{
	/*当计数器到达10次以后删除所有创建的软件定时器
	  计数器清0，将LED电平置为1，常亮
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

/*硬件定时器初始化*/
void timer0_init(void)                
{
	TMOD = 0x00;
    TH0 = (65536-FOSC/command_speed/1000) >> 8;
    TL0 = (65536-FOSC/command_speed/1000);
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}

/*利用系统定时器产生1ms的定时中断*/
void timer0() interrupt 1 
{
	TH0 = (65536-FOSC/command_speed/1000) >> 8;
    TL0 = (65536-FOSC/command_speed/1000);
	/*multi_timer计数器自增*/
	timer_ticks();
}  
