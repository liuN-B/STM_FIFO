#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "sys.h"

/*
	�ڴ˶������ɸ������ʱ��ȫ�ֱ���
	ע�⣬��������__IO �� volatile����Ϊ����������жϺ���������ͬʱ�����ʣ��п�����ɱ����������Ż���
*/
#define TMR_COUNT	4		/* �����ʱ���ĸ��� ����ʱ��ID��Χ 0 - 3) */

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* һ�ι���ģʽ */
	TMR_AUTO_MODE = 1		/* �Զ���ʱ����ģʽ */
}TMR_MODE_E;

/* ��ʱ���ṹ�壬��Ա���������� volatile, ����C�������Ż�ʱ���������� */
typedef struct
{
	volatile uint8_t Mode;		/* ������ģʽ��1���� */
	volatile uint8_t Flag;		/* ��ʱ�����־  */
	volatile uint32_t Count;	/* ������ */
	volatile uint32_t PreLoad;	/* ������Ԥװֵ */
}SOFT_TMR;

/* �ṩ������C�ļ����õĺ��� */
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
