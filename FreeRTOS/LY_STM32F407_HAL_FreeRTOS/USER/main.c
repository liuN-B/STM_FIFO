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

#define START_TASK_PRIO		1				//任务优先级
#define TASK1_TASK_PRIO		2				//任务优先级
#define TASK2_TASK_PRIO		3 				//任务优先级
#define LIST_TASK_PRIO		3				//任务优先级

#define START_STK_SIZE 		128  			//任务堆栈大小
#define TASK1_STK_SIZE 		128  			//任务堆栈大小	
#define TASK2_STK_SIZE 		128  			//任务堆栈大小
#define LIST_STK_SIZE 		128  			//任务堆栈大小

TaskHandle_t StartTask_Handler;				//任务句柄
TaskHandle_t Task1Task_Handler;				//任务句柄
TaskHandle_t Task2Task_Handler;				//任务句柄
TaskHandle_t ListTask_Handler;				//任务句柄

void start_task(void *pvParameters);		//任务函数声明
void task1_task(void *pvParameters);		//任务函数声明
void task2_task(void *pvParameters);		//任务函数声明
void list_task(void *pvParameters);			//任务函数声明


//定义一个测试用的列表(链表)和3个列表项(节点)
List_t TestList;		//测试用列表(链表)
ListItem_t ListItem1;	//测试用列表项(节点)1
ListItem_t ListItem2;	//测试用列表项(节点)2
ListItem_t ListItem3;	//测试用列表项(节点)3



int main(void)
{
    HAL_Init();                 //初始化HAL库
    Stm32_Clock_Init(8, 336, 2, 7); //设置时钟,168Mhz
    delay_init(168);  	        //初始化延时函数
    KEY_Init();
	LED_Init();		        //初始化LED端口
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
	//创建LIST任务
    xTaskCreate((TaskFunction_t )list_task,     
                (const char*    )"list_task",   
                (uint16_t       )LIST_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LIST_TASK_PRIO,
                (TaskHandle_t*  )&ListTask_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

/* 检测k1、k2按键是否按下，并返回各自的键值，这里将k1的键值设定为1，k2的设定为2 */
uint8_t key_Scan(void)
{
    //没有key_up，会导致按下按键再松开之前，多次调用KEY_Scan时，每次都会检测到按键
    //被按下了，有了key_up，第一次调用KEY_Scan时返回键值，后面几次调用时会通过
    //key_up检测到已经返回过一次键值了，不再返回键值
    static uint8_t key_up = 0;

    int key1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
    int key2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);

    if((key_up == 0) && (key1 == 0 || key2 == 0)) //检测到刚按下进入，如果是按住不放不会进入
    {
        delay_ms(10);                      //去抖动
        key_up = 1;                        //设置标志位，表示按下

        if(key1 == 0)return 1; 				//如果k1按下就返回1
        else if(key2 == 0)return 2; 		//如果k2按下就返回2
    }
    else if(key1 == 1 && key2 == 1) key_up = 0; 	 //按键松开，清标志位

    return 0;                              //无按键按下或松开了时就返回0
}
uint8_t kValue = 0;

//task1任务函数
void task1_task(void *pvParameters)
{
    for(;;)
    {
        kValue = key_Scan();
        vTaskDelay(200);/* 延时 200 个 tick */
    }
}

//task2任务函数
void task2_task(void *pvParameters)
{

    static uint8_t flag1 = 0;
    static uint8_t flag2 = 0;

    for(;;)
    {
        if(kValue == 1)
        {
            SEGGER_RTT_printf(0, "k1 被按下了\r\n");

            if(flag1 == 0) //灯关着就打开
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
                flag1 = 1;
            }
            else if(flag1 == 1) //开着就关闭
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
                flag1 = 0;
            }
        }
        else if(kValue == 2)
        {
            SEGGER_RTT_printf(0, "k2 被按下了\r\n");

            if(flag2 == 0) //灯关着就打开
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
                flag2 = 1;
            }
            else if(flag2 == 1) //开着就关闭
            {
                HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);
                flag2 = 0;
            }
        }
        vTaskDelay(200);
    }
}


//list任务函数
void list_task(void *pvParameters)
{
	//第一步：初始化列表(链表)和列表项(节点)
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	ListItem1.xItemValue=40;			//ListItem1列表项(节点1)值为40
	ListItem2.xItemValue=60;			//ListItem2列表项(节点2)值为60
	ListItem3.xItemValue=50;			//ListItem3列表项(节点3)值为50
	
	SEGGER_RTT_SetTerminal(1);                                                    
	//第二步：打印列表和其他列表项的地址
	SEGGER_RTT_printf(0,"/*******************列表和列表项地址*******************/\r\n");
	SEGGER_RTT_printf(0,"项目                              地址				    \r\n");
	SEGGER_RTT_printf(0,"TestList                          0x%#x					\r\n",(int)&TestList);
	SEGGER_RTT_printf(0,"TestList->pxIndex                 0x%#x					\r\n",(int)TestList.pxIndex);
	SEGGER_RTT_printf(0,"TestList->xListEnd                0x%#x					\r\n",(int)(&TestList.xListEnd));
	SEGGER_RTT_printf(0,"ListItem1                         0x%#x					\r\n",(int)&ListItem1);
	SEGGER_RTT_printf(0,"ListItem2                         0x%#x					\r\n",(int)&ListItem2);
	SEGGER_RTT_printf(0,"ListItem3                         0x%#x					\r\n",(int)&ListItem3);
	SEGGER_RTT_printf(0,"/************************结束**************************/\r\n");
	SEGGER_RTT_printf(0,"按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下,不按下就不执行下面操作
	
	//第三步：向列表TestList添加列表项ListItem1，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem1);		//插入列表项ListItem1
	SEGGER_RTT_printf(0,"/******************添加列表项ListItem1*****************/\r\n");
	SEGGER_RTT_printf(0,"项目                              地址				    \r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"/*******************前后向连接分割线********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"/************************结束**************************/\r\n");
	SEGGER_RTT_printf(0,"按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下，不按下就不执行下面操作
	
	//第四步：向列表TestList添加列表项ListItem2，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem2);	//插入列表项ListItem2
	SEGGER_RTT_printf(0,"/******************添加列表项ListItem2*****************/\r\n");
	SEGGER_RTT_printf(0,"项目                              地址				    \r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"ListItem2->pxNext                 0x%#x					\r\n",(int)(ListItem2.pxNext));
	SEGGER_RTT_printf(0,"/*******************前后向连接分割线********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem2->pxPrevious             0x%#x					\r\n",(int)(ListItem2.pxPrevious));
	SEGGER_RTT_printf(0,"/************************结束**************************/\r\n");
	SEGGER_RTT_printf(0,"按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下，不按下就不执行下面操作
	
	//第五步：向列表TestList添加列表项ListItem3，并通过串口打印所有
	//列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
	//项在列表中的连接情况。
	vListInsert(&TestList,&ListItem3);	//插入列表项ListItem3
	SEGGER_RTT_printf(0,"/******************添加列表项ListItem3*****************/\r\n");
	SEGGER_RTT_printf(0,"项目                              地址				    \r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"ListItem3->pxNext                 0x%#x					\r\n",(int)(ListItem3.pxNext));
	SEGGER_RTT_printf(0,"ListItem2->pxNext                 0x%#x					\r\n",(int)(ListItem2.pxNext));
	SEGGER_RTT_printf(0,"/*******************前后向连接分割线********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem3->pxPrevious             0x%#x					\r\n",(int)(ListItem3.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem2->pxPrevious             0x%#x					\r\n",(int)(ListItem2.pxPrevious));
	SEGGER_RTT_printf(0,"/************************结束**************************/\r\n");
	SEGGER_RTT_printf(0,"按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下，不按下就不执行下面操作
	
	//第六步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
	//pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
	uxListRemove(&ListItem2);						//删除ListItem2
	SEGGER_RTT_printf(0,"/******************删除列表项ListItem2*****************/\r\n");
	SEGGER_RTT_printf(0,"项目                              地址				    \r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"ListItem3->pxNext                 0x%#x					\r\n",(int)(ListItem3.pxNext));
	SEGGER_RTT_printf(0,"/*******************前后向连接分割线********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem3->pxPrevious             0x%#x					\r\n",(int)(ListItem3.pxPrevious));
	SEGGER_RTT_printf(0,"/************************结束**************************/\r\n");
	SEGGER_RTT_printf(0,"按下KEY_UP键继续!\r\n\r\n\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);					//等待KEY_UP键按下，不按下就不执行下面操作
	
	//第七步：删除ListItem2，并通过串口打印所有列表项中成员变量pxNext和
	//pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
	TestList.pxIndex=TestList.pxIndex->pxNext;			//pxIndex向后移一项，这样pxIndex就会指向ListItem1。
	vListInsertEnd(&TestList,&ListItem2);				//列表末尾添加列表项ListItem2
	SEGGER_RTT_printf(0,"/***************在末尾添加列表项ListItem2***************/\r\n");
	SEGGER_RTT_printf(0,"项目                              地址				    \r\n");
	SEGGER_RTT_printf(0,"TestList->pxIndex                 0x%#x					\r\n",(int)TestList.pxIndex);
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxNext        0x%#x					\r\n",(int)(TestList.xListEnd.pxNext));
	SEGGER_RTT_printf(0,"ListItem2->pxNext                 0x%#x					\r\n",(int)(ListItem2.pxNext));
	SEGGER_RTT_printf(0,"ListItem1->pxNext                 0x%#x					\r\n",(int)(ListItem1.pxNext));
	SEGGER_RTT_printf(0,"ListItem3->pxNext                 0x%#x					\r\n",(int)(ListItem3.pxNext));
	SEGGER_RTT_printf(0,"/*******************前后向连接分割线********************/\r\n");
	SEGGER_RTT_printf(0,"TestList->xListEnd->pxPrevious    0x%#x					\r\n",(int)(TestList.xListEnd.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem2->pxPrevious             0x%#x					\r\n",(int)(ListItem2.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem1->pxPrevious             0x%#x					\r\n",(int)(ListItem1.pxPrevious));
	SEGGER_RTT_printf(0,"ListItem3->pxPrevious             0x%#x					\r\n",(int)(ListItem3.pxPrevious));
	SEGGER_RTT_printf(0,"/************************结束**************************/\r\n\r\n\r\n");
	while(1)
	{
		LED1=!LED1;
        vTaskDelay(1000);                           //延时1s，也就是1000个时钟节拍	
	}
}

