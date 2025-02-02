#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "sys.h"

/*
	在此定义若干个软件定时器全局变量
	注意，必须增加__IO 即 volatile，因为这个变量在中断和主程序中同时被访问，有可能造成编译器错误优化。
*/
#define TMR_COUNT	4		/* 软件定时器的个数 （定时器ID范围 0 - 3) */

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* 一次工作模式 */
	TMR_AUTO_MODE = 1		/* 自动定时工作模式 */
}TMR_MODE_E;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
	volatile uint8_t Mode;		/* 计数器模式，1次性 */
	volatile uint8_t Flag;		/* 定时到达标志  */
	volatile uint32_t Count;	/* 计数器 */
	volatile uint32_t PreLoad;	/* 计数器预装值 */
}SOFT_TMR;

/* 提供给其他C文件调用的函数 */
void Soft_TimerInit(void);
void Delay_ms(uint32_t n);
void Delay_us(uint32_t n);
void StartTimer(uint8_t _id, uint32_t _period);
void StartAutoTimer(uint8_t _id, uint32_t _period);
void StopTimer(uint8_t _id);

uint8_t CheckTimer(uint8_t _id);
int32_t GetRunTime(void);
int32_t CheckRunTime(int32_t _LastTime);

#endif
