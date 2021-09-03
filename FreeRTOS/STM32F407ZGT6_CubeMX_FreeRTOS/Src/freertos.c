#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"



//��� defaultTask �����̣߳����߳� IDҲ���Ǿ����ID������� Ϊ�̵߳�Ψһʶ���
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;//��� myTask02 �̵߳��߳� ID�������



void StartDefaultTask(void const * argument);//�̺߳�������
void StartTask02(void const * argument);//�̺߳�������

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
  * @brief  FreeRTOS initialization �� main ��������
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {

  /* definition and creation of defaultTask */
  
  /* �����̵߳����ݽṹ��Ȼ��ʹ�ø����ݽṹ�����̣߳������ú�ͽ��� READY �ȴ�
	*���� */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
   /* ʹ���߳����ݽṹ���� defaultTask �̣߳��߳� ID ���浽 defaultTaskHandle��
	* ���� StartDefaultTask �ͱ�ע��Ϊ���̺߳�����������һ����ͨ�ĺ�����*/
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* �����̵߳����ݽṹ��Ȼ��ʹ�ø����ݽṹ�����̣߳������ú�ͽ��� READY
	*�ȴ����� */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 128);
  /* �����̣߳��� StartTask02 ע��Ϊ�̺߳��� */
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* StartDefaultTask �̺߳�������ע��Ϊ�̺߳����������߳̾��������̺߳����е�ָ�� */
void StartDefaultTask(void const * argument)
{

  for(;;)
  {
    osDelay(1);
  }

}


/* StartTask02 �̺߳��� */
void StartTask02(void const * argument)
{

  for(;;)
  {
    osDelay(1);
  }

}

