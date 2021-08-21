#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "SEGGER_RTT.h"
#include "math.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO		1				//�������ȼ�
#define TASK1_TASK_PRIO		2				//�������ȼ�
#define TASK2_TASK_PRIO		3 				//�������ȼ�
#define LIST_TASK_PRIO		3				//�������ȼ�

#define START_STK_SIZE 		128  			//�����ջ��С
#define TASK1_STK_SIZE 		128  			//�����ջ��С	
#define TASK2_STK_SIZE 		128  			//�����ջ��С
#define LIST_STK_SIZE 		128  			//�����ջ��С

TaskHandle_t StartTask_Handler;				//������
TaskHandle_t Task1Task_Handler;				//������
TaskHandle_t Task2Task_Handler;				//������
TaskHandle_t ListTask_Handler;				//������

void start_task(void *pvParameters);		//����������
void task1_task(void *pvParameters);		//����������
void task2_task(void *pvParameters);		//����������
void list_task(void *pvParameters);			//����������


//����һ�������õ��б�(����)��3���б���(�ڵ�)
List_t TestList;		//�������б�(����)
ListItem_t ListItem1;	//�������б���(�ڵ�)1
ListItem_t ListItem2;	//�������б���(�ڵ�)2
ListItem_t ListItem3;	//�������б���(�ڵ�)3



int main(void)
{
    HAL_Init();                 //��ʼ��HAL��
    Stm32_Clock_Init(8, 336, 2, 7); //����ʱ��,168Mhz
    delay_init(168);  	        //��ʼ����ʱ����
    KEY_Init();
	LED_Init();		        //��ʼ��LED�˿�
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
	//����LIST����
    xTaskCreate((TaskFunction_t )list_task,     
                (const char*    )"list_task",   
                (uint16_t       )LIST_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LIST_TASK_PRIO,
                (TaskHandle_t*  )&ListTask_Handler); 
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

/* ���k1��k2�����Ƿ��£������ظ��Եļ�ֵ�����ｫk1�ļ�ֵ�趨Ϊ1��k2���趨Ϊ2 */
uint8_t key_Scan(void)
{
    //û��key_up���ᵼ�°��°������ɿ�֮ǰ����ε���KEY_Scanʱ��ÿ�ζ����⵽����
    //�������ˣ�����key_up����һ�ε���KEY_Scanʱ���ؼ�ֵ�����漸�ε���ʱ��ͨ��
    //key_up��⵽�Ѿ����ع�һ�μ�ֵ�ˣ����ٷ��ؼ�ֵ
    static uint8_t key_up = 0;

    int key1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
    int key2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);

    if((key_up == 0) && (key1 == 0 || key2 == 0)) //��⵽�հ��½��룬����ǰ�ס���Ų������
    {
        delay_ms(10);                      //ȥ����
        key_up = 1;                        //���ñ�־λ����ʾ����

        if(key1 == 0)return 1; 				//���k1���¾ͷ���1
        else if(key2 == 0)return 2; 		//���k2���¾ͷ���2
    }
    else if(key1 == 1 && key2 == 1) key_up = 0; 	 //�����ɿ������־λ

    return 0;                              //�ް������»��ɿ���ʱ�ͷ���0
}
uint8_t kValue = 0;

//task1������
void task1_task(void *pvParameters)
{
    for(;;)
    {
        kValue = key_Scan();
        vTaskDelay(200);/* ��ʱ 200 �� tick */
    }
}

//task2������
void task2_task(void *pvParameters)
{

    static uint8_t flag1 = 0;
    static uint8_t flag2 = 0;

    for(;;)
    {
        if(kValue == 1)
        {
            SEGGER_RTT_printf(0, "k1 ��������\r\n");

            if(flag1 == 0) //�ƹ��žʹ�
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
                flag1 = 1;
            }
            else if(flag1 == 1) //���ž͹ر�
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
                flag1 = 0;
            }
        }
        else if(kValue == 2)
        {
            SEGGER_RTT_printf(0, "k2 ��������\r\n");

            if(flag2 == 0) //�ƹ��žʹ�
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
                flag2 = 1;
            }
            else if(flag2 == 1) //���ž͹ر�
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);
                flag2 = 0;
            }
        }
        vTaskDelay(200);
    }
}


//list������
void list_task(void *pvParameters)
{
	//��һ������ʼ���б�(����)���б���(�ڵ�)
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	ListItem1.xItemValue=40;			//ListItem1�б���(�ڵ�1)ֵΪ40
	ListItem2.xItemValue=60;			//ListItem2�б���(�ڵ�2)ֵΪ60
	ListItem3.xItemValue=50;			//ListItem3�б���(�ڵ�3)ֵΪ50
	
	SEGGER_RTT_SetTerminal(1);                                                    
	//�ڶ�������ӡ�б�������б���ĵ�ַ
	SEGGER_RTT_printf(0,"/*******************�б���б����ַ*******************/\r\n");
	SEGGER_RTT_printf(0,"��Ŀ                              ��ַ				    \r\n");
	SEGGER_RTT_printf(0,"TestList                          0x%#x					\r\n",(int)&TestList);
	SEGGER_RTT_printf(0,"TestList->pxIndex                 0x%#x					\r\n",(int)TestList.pxIndex);
	SEGGER_RTT_printf(0,"TestList->xListEnd                0x%#x					\r\n",(int)(&TestList.xListEnd));
	SEGGER_RTT_printf(0,"ListItem1                         0x%#x					\r\n",(int)&ListItem1);
	SEGGER_RTT_printf(0,"ListItem2                         0x%#x					\r\n",(int)&ListItem2);
	SEGGER_RTT_printf(0,"ListItem3                         0x%#x					\r\n",(int)&ListItem3);
	SEGGER_RTT_printf(0,"/************************����**************************/\r\n");
	SEGGER_RTT_printf(0,"����KEY_UP������!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//�ȴ�KEY_UP������,�����¾Ͳ�ִ���������
	
	//�����������б�TestList����б���ListItem1����ͨ�����ڴ�ӡ����
	//�б����г�Ա����pxNext��pxPrevious��ֵ��ͨ��������ֵ�۲��б�
	//�����б��е����������
	vListInsert(&TestList,&ListItem1);		//�����б���ListItem1
	SEGGER_RTT_printf(0,"/******************����б���ListItem1*****************/\r\n");
	SEGGER_RTT_printf(0,"��Ŀ                              ��ַ				    \r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"/*******************ǰ�������ӷָ���********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"/************************����**************************/\r\n");
	SEGGER_RTT_printf(0,"����KEY_UP������!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//�ȴ�KEY_UP�����£������¾Ͳ�ִ���������
	
	//���Ĳ������б�TestList����б���ListItem2����ͨ�����ڴ�ӡ����
	//�б����г�Ա����pxNext��pxPrevious��ֵ��ͨ��������ֵ�۲��б�
	//�����б��е����������
	vListInsert(&TestList,&ListItem2);	//�����б���ListItem2
	SEGGER_RTT_printf(0,"/******************����б���ListItem2*****************/\r\n");
	SEGGER_RTT_printf(0,"��Ŀ                              ��ַ				    \r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"ListItem2->pxNext                 0x%#x					\r\n",(int)(ListItem2.pxNext));
	SEGGER_RTT_printf(0,"/*******************ǰ�������ӷָ���********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem2->pxPrevious             0x%#x					\r\n",(int)(ListItem2.pxPrevious));
	SEGGER_RTT_printf(0,"/************************����**************************/\r\n");
	SEGGER_RTT_printf(0,"����KEY_UP������!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//�ȴ�KEY_UP�����£������¾Ͳ�ִ���������
	
	//���岽�����б�TestList����б���ListItem3����ͨ�����ڴ�ӡ����
	//�б����г�Ա����pxNext��pxPrevious��ֵ��ͨ��������ֵ�۲��б�
	//�����б��е����������
	vListInsert(&TestList,&ListItem3);	//�����б���ListItem3
	SEGGER_RTT_printf(0,"/******************����б���ListItem3*****************/\r\n");
	SEGGER_RTT_printf(0,"��Ŀ                              ��ַ				    \r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"ListItem3->pxNext                 0x%#x					\r\n",(int)(ListItem3.pxNext));
	SEGGER_RTT_printf(0,"ListItem2->pxNext                 0x%#x					\r\n",(int)(ListItem2.pxNext));
	SEGGER_RTT_printf(0,"/*******************ǰ�������ӷָ���********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem3->pxPrevious             0x%#x					\r\n",(int)(ListItem3.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem2->pxPrevious             0x%#x					\r\n",(int)(ListItem2.pxPrevious));
	SEGGER_RTT_printf(0,"/************************����**************************/\r\n");
	SEGGER_RTT_printf(0,"����KEY_UP������!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//�ȴ�KEY_UP�����£������¾Ͳ�ִ���������
	
	//��������ɾ��ListItem2����ͨ�����ڴ�ӡ�����б����г�Ա����pxNext��
	//pxPrevious��ֵ��ͨ��������ֵ�۲��б������б��е����������
	uxListRemove(&ListItem2);						//ɾ��ListItem2
	SEGGER_RTT_printf(0,"/******************ɾ���б���ListItem2*****************/\r\n");
	SEGGER_RTT_printf(0,"��Ŀ                              ��ַ				    \r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"ListItem3->pxNext                 0x%#x					\r\n",(int)(ListItem3.pxNext));
	SEGGER_RTT_printf(0,"/*******************ǰ�������ӷָ���********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem3->pxPrevious             0x%#x					\r\n",(int)(ListItem3.pxPrevious));
	SEGGER_RTT_printf(0,"/************************����**************************/\r\n");
	SEGGER_RTT_printf(0,"����KEY_UP������!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//�ȴ�KEY_UP�����£������¾Ͳ�ִ���������
	
	//���߲���ɾ��ListItem2����ͨ�����ڴ�ӡ�����б����г�Ա����pxNext��
	//pxPrevious��ֵ��ͨ��������ֵ�۲��б������б��е����������
	TestList.pxIndex=TestList.pxIndex->pxNext;			//pxIndex�����һ�����pxIndex�ͻ�ָ��ListItem1��
	vListInsertEnd(&TestList,&ListItem2);				//�б�ĩβ����б���ListItem2
	SEGGER_RTT_printf(0,"/***************��ĩβ����б���ListItem2***************/\r\n");
	SEGGER_RTT_printf(0,"��Ŀ                              ��ַ				    \r\n");
	SEGGER_RTT_printf(0,"TestList->pxIndex                 0x%#x					\r\n",(int)TestList.pxIndex);
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem2->pxNext                 0x%#x					\r\n",(int)(ListItem2.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"ListItem3->pxNext                 0x%#x					\r\n",(int)(ListItem3.pxNext));
	SEGGER_RTT_printf(0,"/*******************ǰ�������ӷָ���********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem2->pxPrevious             0x%#x					\r\n",(int)(ListItem2.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem3->pxPrevious             0x%#x					\r\n",(int)(ListItem3.pxPrevious));
	SEGGER_RTT_printf(0,"/************************����**************************/\r\n\r\n\r\n");
	while(1)
	{
		LED1=!LED1;
        vTaskDelay(1000);                           //��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}

