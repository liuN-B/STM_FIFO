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


/* 确保stdint仅由编译器使用，而不是由汇编程序使用.针对不同的编译器调用不同的stdint.h文件 */
#if defined (__ICCARM__)|| defined(__CC_ARM)|| defined(__GNUC__)
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif

/***************************************************************************************************************/
/*                                        FreeRTOS基础配置配置选项                                              */
/***************************************************************************************************************/
#define configUSE_TIME_SLICING					1	//1使能时间片调度(默认式使能的)
/*
某些运行 FreeRTos的硬件有两种方法选择下一个要执行的任务：
*通用方法和特定于硬件的方法（以下简称“特殊方法〃）。

*通用方法
	1.configUSE_PORT_OPTIMISED_TASK_SELECTION0或者硬件不支持这种特殊方法
	2.可以用于所有FreeRTOS支持的硬件
	3.完全用C实现，效率略低于特殊方法
	4.不强制要求限制最大可用优先级数目
	
*特殊方法

	1.必须将 configUSE PORT OPTIMISED TASK SELECTIoN设置为1。
	2.依赖一个或多个特定架构的汇编指令（一般是类似计算前导零[CL2]指令）
	3.比通用方法更高效
	4.一般强制限定最大可用优先级数目为32般是硬件计算前导零指令，如果所使用的，MCU没有这些硬件指令的话此宏应该设置为0！
*/
#define configUSE_PORT_OPTIMISED_TASK_SELECTION	1    /*1启用特殊方法来选择下一个要运行的任务
                                                     一般是硬件计算前导零指令，如果所使用的
                                                     MCU没有这些硬件指令的话此宏应该设置为0！*/

#define configUSE_TICKLESS_IDLE			0   //置 1：使能低功耗 tickless 模式；置 0：保持系统节拍（tick）中断一直运行
#define configUSE_QUEUE_SETS			1   //为1时启用队列


#define configUSE_PREEMPTION			1   //1使用抢占式内核，0使用协程
/*

* 写入实际的CPU内核时钟频率，也就是CPU指令执行频率，通常称为Fc1k Fc1k
* 为供给CPU内核的时钟信号，我们所说的CPU主频为 XX MHZ，
* 就是指的这个时钟信号，相应的，1/Fc1k即为cpu时钟周期；

*/
#define configCPU_CLOCK_HZ				( SystemCoreClock )  //设置CPU 时钟频率
//RTOS 系统节拍中断的频率。即一秒中断的次数，每次中断 RTOS 都会进行任务调度,这里设置为1000，周期就是1ms
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES			( 32 ) //最大任务优先级默认为5，最大支持256个优先级
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 130 )  //空闲任务使用的堆栈大小
#define configMAX_TASK_NAME_LEN			( 16 )  //任务名字字符串长度，这里定义的长度包括字符串结束符’\0’
#define configUSE_TRACE_FACILITY		1

#define configUSE_16_BIT_TICKS			0 //系统节拍计数器变量数据类型，1 表示为 16 位无符号整形，0 表示为 32 位无符号整形
#define configIDLE_SHOULD_YIELD			1 //为1时 空闲任务放弃CPU使用权给其他同优先级的用户任务
#define configUSE_MUTEXES				1 //为1时使用互斥信号量
#define configQUEUE_REGISTRY_SIZE		8 //不为0时表示启用队列记录，具体的值是可以
#define configCHECK_FOR_STACK_OVERFLOW	0 /*大于0时启用堆栈溢出检测功能，如果使用此功能
                                            用户必须提供一个栈溢出钩子函数，如果使用的话
                                            此值可以为1或者2，因为有两种栈溢出检测方法。*/
#define configUSE_RECURSIVE_MUTEXES		1 //为1时使用递归互斥信号量
#define configUSE_MALLOC_FAILED_HOOK	0 //1使用内存申请失败钩子函数
#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_COUNTING_SEMAPHORES	1 //为1时使用计数信号量
#define configGENERATE_RUN_TIME_STATS	0


/***************************************************************************************************************/
/*                                FreeRTOS与内存申请有关配置选项                                                */
/**************************************************************************************************************/
#define configSUPPORT_DYNAMIC_ALLOCATION        1   /*支持动态内存申请，一般在系统中采用的内存分配都是动态内存分配。
													   FreeRTOS同时也支持静态分配内存，但是常用的就是动态分配了。*/
#define configSUPPORT_STATIC_ALLOCATION 		0	/*  1支持静态内存
														如果使用静态方法的话需要自己实现两个函数vApplicationGetIdleTaskMemory()和
														vApplicationGetTimerTaskMemory()。通过这两个函数来给空闲任务和定时器服务任务的任务堆
														栈和任务控制块分配内存，*/

#define configTOTAL_HEAP_SIZE					((size_t)(20*1024))//次参数用来定义动态内存分配的长度


/***************************************************************************************************************/
/*                                FreeRTOS与钩子函数有关的配置选项                                              */
/***************************************************************************************************************/

/*
* 置1：使用空闲钩子（Idle hook类似于回调函数）；置0：忽略空闲钩子
* 空闲任务钩子是一个函数，这个函数由用户来实现，
* FreeRTOS规定了函数的名字和参数：void vApplicationIdleHook(void)，
* 这个函数在每个空闲任务周期都会被调用
* 对于已经删除的RTOS任务，空闲任务可以释放分配给它们的堆栈内存。
* 因此必须保证空闲任务可以被CPU执行
* 使用空闲钩子函数设置CPU进入省电模式是很常见的
* 不可以调用会引起空闲任务阻塞的API函数
*/
#define configUSE_IDLE_HOOK				0  //空闲钩子
/*

* 置1：使用时间片钩子（Tick Hook）；置0：忽略时间片钩子
* 
* 时间片钩子是一个函数，这个函数由用户来实现，
* FreerToS规定了函数的名字和参数：void vApplicationTickHook（void）
* 时间片中断可以周期性的调用
* 函数必须非常短小，不能大量使用堆栈，
* 不能调用以〃FromIsR"或"FROM ISR"结尾的API函数

*/
#define configUSE_TICK_HOOK				0  //滴答定时器钩子

/***************************************************************************************************************/
/*                                FreeRTOS与运行时间和任务状态收集有关的配置选项                                 */
/***************************************************************************************************************/
#define configGENERATE_RUN_TIME_STATS	        0     //为1时 启用运行时间统计功能
#define configUSE_TRACE_FACILITY				1     //为1时 启用可视化跟踪调试
#define configUSE_STATS_FORMATTING_FUNCTIONS	1    /* 宏configUSE_TRACE_FACILITY同时为1时会编译下面3个函数
                                                      * prvWriteNameToBuffer(),
													  * vTaskList(),
                                                      * vTaskGetRunTimeStats()
													  */


/***************************************************************************************************************/
/*                                FreeRTOS与协程有关的配置选项                                                  */
/***************************************************************************************************************/

#define configUSE_CO_ROUTINES 		0          //为1时启用协程，启用协程以后必须添加文件croutine.c
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )  //协程的有效优先级数目

/***************************************************************************************************************/
/*                                FreeRTOS与软件定时器有关的配置选项                                            */
/***************************************************************************************************************/
#define configUSE_TIMERS				1								 //为1时启用软件定时器
#define configTIMER_TASK_PRIORITY		( 2 )							 //软件定时器优先级
#define configTIMER_QUEUE_LENGTH		10								 //软件定时器队列长度
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 ) //软件定时器任务堆栈大小

/***************************************************************************************************************/
/*                                FreeRTOS可选函数配置选项                                                      */
/***************************************************************************************************************/
#define INCLUDE_vTaskPrioritySet		1 //如果要使用函数 vTaskPrioritySet() 的话需要将宏 vTaskPrioritySet 定义为1。
#define INCLUDE_uxTaskPriorityGet		1 //如果要使用函数 uxTaskPriorityGet() 的话需要将宏 uxTaskPriorityGet 定义为1。
#define INCLUDE_vTaskDelete				1 //如果要使用函数 vTaskDelete() 的话需要将宏 vTaskDelete 定义为1。
#define INCLUDE_vTaskCleanUpResources	1 //如果要使用函数 vTaskCleanUpResources() 的话需要将宏 vTaskCleanUpResources 定义为1。
#define INCLUDE_vTaskSuspend			1 //如果要使用函数 vTaskSuspend() 的话需要将宏 vTaskSuspend 定义为1。
#define INCLUDE_vTaskDelayUntil			1 //如果要使用函数 vTaskDelayUntil() 的话需要将宏 vTaskDelayUntil 定义为1
#define INCLUDE_vTaskDelay				1 //如果要使用函数 vTaskDelay() 的话需要将宏 INCLUDE_vTaskDelay 定义为1
#define INCLUDE_xTaskGetSchedulerState  1 //如果要使用函数 xTaskGetSchedulerState() 的话需要将宏 xTaskGetSchedulerState 定义为1

/***************************************************************************************************************/
/*                                FreeRTOS与中断有关的配置选项                                                  */
/***************************************************************************************************************/
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif

/* 此宏定义是用来配置FreeRTOS用到的Systick中断和PendSV中断优先级。在NVIC分组中设置
为4的情况下，次宏定义的范围就是0~15，即专门配置抢占优先级。这里配置了为0xf，即Systick
和PendSV都是配置为了最低优先级，实际项目中也建议大家配置为最低优先级即可。
任务优先级数值越小，任务优先级越低。*/
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf

/* 定义了系统可管理的最高中断优先级为5。优先级越高，数值越低。 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* 内核端口层本身使用的中断优先级。这些是所有Cortex-M端口的通用端口，不依赖于任何特定的库函数。 */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!! See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
	
/* 断言 */
#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
	
/****************************************************************
		FreeRTOS 与中断服务函数有关的配置选项
****************************************************************/
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
//#define xPortSysTickHandler SysTick_Handler

#endif 

