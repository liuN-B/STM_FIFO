//ʵ������ledÿһ����˸һ��
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "SEGGER_RTT.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO		1				//�������ȼ�
#define TASK1_TASK_PRIO		2				//�������ȼ�
#define TASK2_TASK_PRIO		3 				//�������ȼ�

#define START_STK_SIZE 		128  			//�����ջ��С
#define TASK1_STK_SIZE 		128  			//�����ջ��С	
#define TASK2_STK_SIZE 		128  			//�����ջ��С

TaskHandle_t StartTask_Handler;				//������
TaskHandle_t Task1Task_Handler;				//������
TaskHandle_t Task2Task_Handler;				//������

void start_task(void *pvParameters);		//����������
void task1_task(void *pvParameters);		//����������
void task2_task(void *pvParameters);		//����������

int main(void)
{
	delay_init();
    LED_Init();	
	SEGGER_RTT_printf(0, "FreeRTOS on stm32f103c8t6 board by zhiiguoxin\r\n");
    //��̬������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������
    //�����������
    /* ����RTOS����ʵ���������������������������֮������������Ϳ�ʼ�����̣߳�
     * ��ʱpc��������������ͻ�ָ��ĳ�̵߳�ָ���ʼ���̲߳������С�
     * ���û�д������̵߳Ļ����Ǿ�ֻ��һ���̡߳�*/
    vTaskStartScheduler();

    /* ���ڵ�����vTaskStartScheduler֮��PC��ָ�����߳��е�ָ����vTaskStartScheduler�������
    * �����ᱻCPUִ�У�����vTaskStartScheduler��Ĵ���û�����塣 */
    while(1)
    {
        //����Ĵ��벻�ᱻִ�У�д��Ҳû��
    }
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//task1������
void task1_task(void *pvParameters)
{
    for(;;)
    {
		SEGGER_RTT_SetTerminal(0);
		SEGGER_RTT_printf(0, "task1_task is running...\r\n");
        vTaskDelay(200);/* ��ʱ 200 �� tick */
    }
}

//task2������
void task2_task(void *pvParameters)
{
    for(;;)
    {
		SEGGER_RTT_SetTerminal(1);
        SEGGER_RTT_printf(0, "task2_task is running...\r\n");
        vTaskDelay(200);/* ��ʱ 200 �� tick */
    }
}




