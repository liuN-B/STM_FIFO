#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"



//存放 defaultTask 任务（线程）的线程 ID也就是句柄，ID（句柄） 为线程的唯一识别号
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;//存放 myTask02 线程的线程 ID（句柄）



void StartDefaultTask(void const * argument);//线程函数声明
void StartTask02(void const * argument);//线程函数声明

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;

}


/**
  * @brief  FreeRTOS initialization 由 main 函数调用
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {

  /* definition and creation of defaultTask */
  
  /* 定义线程的数据结构，然后使用该数据结构创建线程，创建好后就进入 READY 等待
	*调度 */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
   /* 使用线程数据结构创建 defaultTask 线程，线程 ID 保存到 defaultTaskHandle，
	* 至此 StartDefaultTask 就被注册为了线程函数，不再是一个普通的函数。*/
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* 定义线程的数据结构，然后使用该数据结构创建线程，创建好后就进入 READY
	*等待调度 */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 128);
  /* 创建线程，将 StartTask02 注册为线程函数 */
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* StartDefaultTask 线程函数，被注册为线程函数，运行线程就是运行线程函数中的指令 */
void StartDefaultTask(void const * argument)
{

  for(;;)
  {
    osDelay(1);
  }

}


/* StartTask02 线程函数 */
void StartTask02(void const * argument)
{

  for(;;)
  {
    osDelay(1);
  }

}

