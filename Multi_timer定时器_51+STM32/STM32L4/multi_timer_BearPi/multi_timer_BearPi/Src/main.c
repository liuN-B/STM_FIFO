#include "main.h"
#include "usart.h"
#include "gpio.h"


/* USER CODE BEGIN PD */
/*��multi_timer�����Ķ�ʱ��1��ʱʱ��  ��λ:ms*/
#define TIMER_TIMEOUT_500MS 500
/*��multi_timer�����Ķ�ʱ��2��ʱʱ��  ��λ:ms*/
#define TIMER_TIMEOUT_1S 1000

Timer timer1 ;
Timer timer2 ;
/*���ڶ�ʱ10s�ļ�����*/
int Counter = 0 ;



void SystemClock_Config(void);

int fputc(int ch, FILE *file)
{
    return HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);
}

/*multi_timer�ص�����1����*/
void timer1_callback(void)
{

    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
/*multi_timer�ص�����2����*/
void timer2_callback(void)
{
    /*������������10���Ժ�ɾ�����д����������ʱ��
      ��������0����LED��ƽ��Ϊ1������
    */
    ++Counter ;

    if(Counter == 10)
    {
        Counter = 0 ;
        //LED = 1 ;
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        printf("LED�Ƴ���\n");
        timer_stop(&timer1);
        printf("�رն�ʱ��1\n");
        timer_stop(&timer2);
        printf("�رն�ʱ��2\n");
    }
}

int main(void)
{

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART1_UART_Init();

    timer_init(&timer1, timer1_callback, TIMER_TIMEOUT_500MS, TIMER_TIMEOUT_500MS);
    timer_init(&timer2, timer2_callback, TIMER_TIMEOUT_1S, TIMER_TIMEOUT_1S);
    timer_start(&timer1);
    timer_start(&timer2);

    while (1)
    {
        timer_loop();
    }


}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 20;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */

