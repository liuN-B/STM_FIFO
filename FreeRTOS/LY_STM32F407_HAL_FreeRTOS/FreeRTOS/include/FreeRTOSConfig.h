/*
 * FreeRTOS V202107.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved. *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 * See http://www.freertos.org/a00110.html
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


/* ȷ��stdint���ɱ�����ʹ�ã��������ɻ�����ʹ��.��Բ�ͬ�ı��������ò�ͬ��stdint.h�ļ� */
#if defined (__ICCARM__)|| defined(__CC_ARM)|| defined(__GNUC__)
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif

/***************************************************************************************************************/
/*                                        FreeRTOS������������ѡ��                                              */
/***************************************************************************************************************/
#define configUSE_TIME_SLICING					1	//1ʹ��ʱ��Ƭ����(Ĭ��ʽʹ�ܵ�)
/*
ĳЩ���� FreeRTos��Ӳ�������ַ���ѡ����һ��Ҫִ�е�����
*ͨ�÷������ض���Ӳ���ķ��������¼�ơ����ⷽ��������

*ͨ�÷���
	1.configUSE_PORT_OPTIMISED_TASK_SELECTION0����Ӳ����֧���������ⷽ��
	2.������������FreeRTOS֧�ֵ�Ӳ��
	3.��ȫ��Cʵ�֣�Ч���Ե������ⷽ��
	4.��ǿ��Ҫ���������������ȼ���Ŀ
	
*���ⷽ��

	1.���뽫 configUSE PORT OPTIMISED TASK SELECTIoN����Ϊ1��
	2.����һ�������ض��ܹ��Ļ��ָ�һ�������Ƽ���ǰ����[CL2]ָ�
	3.��ͨ�÷�������Ч
	4.һ��ǿ���޶����������ȼ���ĿΪ32����Ӳ������ǰ����ָ������ʹ�õģ�MCUû����ЩӲ��ָ��Ļ��˺�Ӧ������Ϊ0��
*/
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1    /*1�������ⷽ����ѡ����һ��Ҫ���е�����
                                                     һ����Ӳ������ǰ����ָ������ʹ�õ�
                                                     MCUû����ЩӲ��ָ��Ļ��˺�Ӧ������Ϊ0��*/

#define configUSE_TICKLESS_IDLE			0   //�� 1��ʹ�ܵ͹��� tickless ģʽ���� 0������ϵͳ���ģ�tick���ж�һֱ����
#define configUSE_QUEUE_SETS			1   //Ϊ1ʱ���ö���


#define configUSE_PREEMPTION			1   //1ʹ����ռʽ�ںˣ�0ʹ��Э��
/*

* д��ʵ�ʵ�CPU�ں�ʱ��Ƶ�ʣ�Ҳ����CPUָ��ִ��Ƶ�ʣ�ͨ����ΪFc1k Fc1k
* Ϊ����CPU�ں˵�ʱ���źţ�������˵��CPU��ƵΪ XX MHZ��
* ����ָ�����ʱ���źţ���Ӧ�ģ�1/Fc1k��Ϊcpuʱ�����ڣ�

*/
#define configCPU_CLOCK_HZ				( SystemCoreClock )  //����CPU ʱ��Ƶ��
//RTOS ϵͳ�����жϵ�Ƶ�ʡ���һ���жϵĴ�����ÿ���ж� RTOS ��������������,��������Ϊ1000�����ھ���1ms
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES			( 32 ) //����������ȼ�Ĭ��Ϊ5�����֧��256�����ȼ�
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 130 )  //��������ʹ�õĶ�ջ��С
#define configMAX_TASK_NAME_LEN			( 16 )  //���������ַ������ȣ����ﶨ��ĳ��Ȱ����ַ�����������\0��
#define configUSE_TRACE_FACILITY		1

#define configUSE_16_BIT_TICKS			0 //ϵͳ���ļ����������������ͣ�1 ��ʾΪ 16 λ�޷������Σ�0 ��ʾΪ 32 λ�޷�������
#define configIDLE_SHOULD_YIELD			1 //Ϊ1ʱ �����������CPUʹ��Ȩ������ͬ���ȼ����û�����
#define configUSE_MUTEXES				1 //Ϊ1ʱʹ�û����ź���
#define configQUEUE_REGISTRY_SIZE		8 //��Ϊ0ʱ��ʾ���ö��м�¼�������ֵ�ǿ���
#define configCHECK_FOR_STACK_OVERFLOW	0 /*����0ʱ���ö�ջ�����⹦�ܣ����ʹ�ô˹���
                                            �û������ṩһ��ջ������Ӻ��������ʹ�õĻ�
                                            ��ֵ����Ϊ1����2����Ϊ������ջ�����ⷽ����*/
#define configUSE_RECURSIVE_MUTEXES		1 //Ϊ1ʱʹ�õݹ黥���ź���
#define configUSE_MALLOC_FAILED_HOOK	0 //1ʹ���ڴ�����ʧ�ܹ��Ӻ���
#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_COUNTING_SEMAPHORES	1 //Ϊ1ʱʹ�ü����ź���
#define configGENERATE_RUN_TIME_STATS	0


/***************************************************************************************************************/
/*                                FreeRTOS���ڴ������й�����ѡ��                                                */
/**************************************************************************************************************/
#define configSUPPORT_DYNAMIC_ALLOCATION        1   /*֧�ֶ�̬�ڴ����룬һ����ϵͳ�в��õ��ڴ���䶼�Ƕ�̬�ڴ���䡣
													   FreeRTOSͬʱҲ֧�־�̬�����ڴ棬���ǳ��õľ��Ƕ�̬�����ˡ�*/
#define configSUPPORT_STATIC_ALLOCATION 		0	/*  1֧�־�̬�ڴ�
														���ʹ�þ�̬�����Ļ���Ҫ�Լ�ʵ����������vApplicationGetIdleTaskMemory()��
														vApplicationGetTimerTaskMemory()��ͨ������������������������Ͷ�ʱ����������������
														ջ��������ƿ�����ڴ棬*/

#define configTOTAL_HEAP_SIZE					((size_t)(20*1024))//�β����������嶯̬�ڴ����ĳ���


/***************************************************************************************************************/
/*                                FreeRTOS�빳�Ӻ����йص�����ѡ��                                              */
/***************************************************************************************************************/

/*
* ��1��ʹ�ÿ��й��ӣ�Idle hook�����ڻص�����������0�����Կ��й���
* ������������һ������������������û���ʵ�֣�
* FreeRTOS�涨�˺��������ֺͲ�����void vApplicationIdleHook(void)��
* ���������ÿ�������������ڶ��ᱻ����
* �����Ѿ�ɾ����RTOS���񣬿�����������ͷŷ�������ǵĶ�ջ�ڴ档
* ��˱��뱣֤����������Ա�CPUִ��
* ʹ�ÿ��й��Ӻ�������CPU����ʡ��ģʽ�Ǻܳ�����
* �����Ե��û������������������API����
*/
#define configUSE_IDLE_HOOK				0  //���й���
/*

* ��1��ʹ��ʱ��Ƭ���ӣ�Tick Hook������0������ʱ��Ƭ����
* 
* ʱ��Ƭ������һ������������������û���ʵ�֣�
* FreerToS�涨�˺��������ֺͲ�����void vApplicationTickHook��void��
* ʱ��Ƭ�жϿ��������Եĵ���
* ��������ǳ���С�����ܴ���ʹ�ö�ջ��
* ���ܵ����ԡ�FromIsR"��"FROM ISR"��β��API����

*/
#define configUSE_TICK_HOOK				0  //�δ�ʱ������

/***************************************************************************************************************/
/*                                FreeRTOS������ʱ�������״̬�ռ��йص�����ѡ��                                 */
/***************************************************************************************************************/
#define configGENERATE_RUN_TIME_STATS	        0     //Ϊ1ʱ ��������ʱ��ͳ�ƹ���
#define configUSE_TRACE_FACILITY				1     //Ϊ1ʱ ���ÿ��ӻ����ٵ���
#define configUSE_STATS_FORMATTING_FUNCTIONS	1    /* ��configUSE_TRACE_FACILITYͬʱΪ1ʱ���������3������
                                                      * prvWriteNameToBuffer(),
													  * vTaskList(),
                                                      * vTaskGetRunTimeStats()
													  */


/***************************************************************************************************************/
/*                                FreeRTOS��Э���йص�����ѡ��                                                  */
/***************************************************************************************************************/

#define configUSE_CO_ROUTINES 		0          //Ϊ1ʱ����Э�̣�����Э���Ժ��������ļ�croutine.c
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )  //Э�̵���Ч���ȼ���Ŀ

/***************************************************************************************************************/
/*                                FreeRTOS�������ʱ���йص�����ѡ��                                            */
/***************************************************************************************************************/
#define configUSE_TIMERS				1								 //Ϊ1ʱ���������ʱ��
#define configTIMER_TASK_PRIORITY		( 2 )							 //�����ʱ�����ȼ�
#define configTIMER_QUEUE_LENGTH		10								 //�����ʱ�����г���
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 ) //�����ʱ�������ջ��С

/***************************************************************************************************************/
/*                                FreeRTOS��ѡ��������ѡ��                                                      */
/***************************************************************************************************************/
#define INCLUDE_vTaskPrioritySet		1 //���Ҫʹ�ú��� vTaskPrioritySet() �Ļ���Ҫ���� vTaskPrioritySet ����Ϊ1��
#define INCLUDE_uxTaskPriorityGet		1 //���Ҫʹ�ú��� uxTaskPriorityGet() �Ļ���Ҫ���� uxTaskPriorityGet ����Ϊ1��
#define INCLUDE_vTaskDelete				1 //���Ҫʹ�ú��� vTaskDelete() �Ļ���Ҫ���� vTaskDelete ����Ϊ1��
#define INCLUDE_vTaskCleanUpResources	1 //���Ҫʹ�ú��� vTaskCleanUpResources() �Ļ���Ҫ���� vTaskCleanUpResources ����Ϊ1��
#define INCLUDE_vTaskSuspend			1 //���Ҫʹ�ú��� vTaskSuspend() �Ļ���Ҫ���� vTaskSuspend ����Ϊ1��
#define INCLUDE_vTaskDelayUntil			1 //���Ҫʹ�ú��� vTaskDelayUntil() �Ļ���Ҫ���� vTaskDelayUntil ����Ϊ1
#define INCLUDE_vTaskDelay				1 //���Ҫʹ�ú��� vTaskDelay() �Ļ���Ҫ���� INCLUDE_vTaskDelay ����Ϊ1
#define INCLUDE_xTaskGetSchedulerState  1 //���Ҫʹ�ú��� xTaskGetSchedulerState() �Ļ���Ҫ���� xTaskGetSchedulerState ����Ϊ1

/***************************************************************************************************************/
/*                                FreeRTOS���ж��йص�����ѡ��                                                  */
/***************************************************************************************************************/
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif

/* �˺궨������������FreeRTOS�õ���Systick�жϺ�PendSV�ж����ȼ�����NVIC����������
Ϊ4������£��κ궨��ķ�Χ����0~15����ר��������ռ���ȼ�������������Ϊ0xf����Systick
��PendSV��������Ϊ��������ȼ���ʵ����Ŀ��Ҳ����������Ϊ������ȼ����ɡ�
�������ȼ���ֵԽС���������ȼ�Խ�͡�*/
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf

/* ������ϵͳ�ɹ��������ж����ȼ�Ϊ5�����ȼ�Խ�ߣ���ֵԽ�͡� */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* �ں˶˿ڲ㱾��ʹ�õ��ж����ȼ�����Щ������Cortex-M�˿ڵ�ͨ�ö˿ڣ����������κ��ض��Ŀ⺯���� */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!! See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
	
/* ���� */
#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
	
/****************************************************************
		FreeRTOS ���жϷ������йص�����ѡ��
****************************************************************/
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
//#define xPortSysTickHandler SysTick_Handler

#endif 

