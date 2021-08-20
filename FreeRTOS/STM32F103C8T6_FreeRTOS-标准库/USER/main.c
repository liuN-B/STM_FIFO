//实验现象：led每一秒闪烁一次
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "SEGGER_RTT.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO		1				//任务优先级
#define TASK1_TASK_PRIO		2				//任务优先级
#define TASK2_TASK_PRIO		3 				//任务优先级

#define START_STK_SIZE 		128  			//任务堆栈大小
#define TASK1_STK_SIZE 		128  			//任务堆栈大小	
#define TASK2_STK_SIZE 		128  			//任务堆栈大小

TaskHandle_t StartTask_Handler;				//任务句柄
TaskHandle_t Task1Task_Handler;				//任务句柄
TaskHandle_t Task2Task_Handler;				//任务句柄

void start_task(void *pvParameters);		//任务函数声明
void task1_task(void *pvParameters);		//任务函数声明
void task2_task(void *pvParameters);		//任务函数声明

int main(void)
{
	delay_init();
    LED_Init();	
	SEGGER_RTT_printf(0, "FreeRTOS on stm32f103c8t6 board by zhiiguoxin\r\n");
    //动态创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
    //开启任务调度
    /* 启动RTOS，其实就是启动“任务管理器”，启动之后任务管理器就开始调度线程，
     * 此时pc（程序计数器）就会指向某线程的指令，开始多线程并发运行。
     * 如果没有创建多线程的话，那就只有一个线程。*/
    vTaskStartScheduler();

    /* 由于调用了vTaskStartScheduler之后，PC就指向了线程中的指令，因此vTaskStartScheduler后面代码
    * 并不会被CPU执行，所以vTaskStartScheduler后的代码没有意义。 */
    while(1)
    {
        //这里的代码不会被执行，写了也没用
    }
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//task1任务函数
void task1_task(void *pvParameters)
{
    for(;;)
    {
		SEGGER_RTT_SetTerminal(0);
		SEGGER_RTT_printf(0, "task1_task is running...\r\n");
        vTaskDelay(200);/* 延时 200 个 tick */
    }
}

//task2任务函数
void task2_task(void *pvParameters)
{
    for(;;)
    {
		SEGGER_RTT_SetTerminal(1);
        SEGGER_RTT_printf(0, "task2_task is running...\r\n");
        vTaskDelay(200);/* 延时 200 个 tick */
    }
}




