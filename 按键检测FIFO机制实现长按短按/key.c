#include "key.h"
#include "delay.h"

#define HARD_KEY_NUM	    3	   				/* ʵ�尴������ */
static KEY_T      s_tBtn[HARD_KEY_NUM] = {0};
static KEY_FIFO_T s_tKey;						/* ����FIFO����,�ṹ�� */

static void KEY_FIFO_Init(void);
static void KEY_GPIO_Config(void);
static uint8_t IsKey1Down(void);
static uint8_t IsKey2Down(void);
static uint8_t IsKey3Down(void);
static void KEY_Detect(uint8_t i);

/*
*********************************************************************************************************
*	�� �� ��: KEY_Init
*	����˵��: ��ʼ������. �ú������������Ŀ�ͷ�����á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void KEY_Init(void)
{
	KEY_FIFO_Init();		/* ��ʼ���������� */
	KEY_GPIO_Config();		/* ��ʼ������Ӳ�� */
}
/*
*********************************************************************************************************
*	�� �� ��: KEY_FIFO_Init
*	����˵��: ��ʼ����������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void KEY_FIFO_Init(void)
{
	uint8_t i;

	/* �԰���FIFO��дָ������ */
	s_tKey.Read = 0;
	s_tKey.Write = 0;

	/* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
	for (i = 0; i < HARD_KEY_NUM; i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;		/* ����������Ϊ�˲�ʱ���һ�� */
		s_tBtn[i].State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
		s_tBtn[i].RepeatSpeed = 0;						/* �����������ٶȣ�0��ʾ��֧������ */
		s_tBtn[i].RepeatCount = 0;						/* ���������� */
	}
	/* �жϰ������µĺ��� */
	s_tBtn[0].IsKeyDownFunc = IsKey1Down;
	s_tBtn[1].IsKeyDownFunc = IsKey2Down;
	s_tBtn[2].IsKeyDownFunc = IsKey3Down;
}
/*
*********************************************************************************************************
*	�� �� ��: KEY_GPIO_Config
*	����˵��: ���ð�����Ӧ��GPIO
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void KEY_GPIO_Config(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* ��1������GPIOʱ�� */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;   			/* �������� */
	GPIO_InitStructure.Pull = GPIO_NOPULL;                 /* ���������費ʹ�� */
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* GPIO�ٶȵȼ� */
	
	GPIO_InitStructure.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	GPIO_InitStructure.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: IsKeyDownFunc
*	����˵��: �жϰ����Ƿ��¡���������ϼ����֡������¼������������������¡�
*	��    ��: ��
*	�� �� ֵ: ����ֵ1 ��ʾ����(��ͨ����0��ʾδ���£��ͷţ�
*********************************************************************************************************
*/
static uint8_t IsKey1Down(void) 
{
	if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET) 
		return 1;
	else 
		return 0;
}
static uint8_t IsKey2Down(void) 
{
	if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3) == GPIO_PIN_RESET) 
		return 1;
	else 
		return 0;
}

static uint8_t IsKey3Down(void) 
{
	if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_RESET) 
		return 1;
	else 
		return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_FIFO_Put
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    ��:  _KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void KEY_FIFO_Put(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_FIFO_Get
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    ��: ��
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t KEY_FIFO_Get(void)
{
	uint8_t ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}



/*
*********************************************************************************************************
*	�� �� ��: KEY_GetState
*	����˵��: ��ȡ������״̬
*	��    ��:  _ucKeyID : ����ID����0��ʼ
*	�� �� ֵ: 1 ��ʾ���£� 0 ��ʾδ����
*********************************************************************************************************
*/
uint8_t KEY_GetState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_SetParam
*	����˵��: ���ð�������
*	��    �Σ�_ucKeyID : ����ID����0��ʼ
*			_LongTime : �����¼�ʱ��
*			 _RepeatSpeed : �����ٶ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void KEY_SetParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;			/* �����������ٶȣ�0��ʾ��֧������ */
	s_tBtn[_ucKeyID].RepeatCount = 0;						/* ���������� */
}

/*
*********************************************************************************************************
*	�� �� ��: KEY_FIFO_Clear
*	����˵��: ��հ���FIFO������
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void KEY_FIFO_Clear(void)
{
	s_tKey.Read = s_tKey.Write;
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyScan10ms
*	����˵��: ɨ�����а���������������systick�ж������Եĵ��ã�10msһ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void KEY_Scan(void)
{
	uint8_t i;

	for (i = 0; i < HARD_KEY_NUM; i++)
	{
		KEY_Detect(i);
	}
}
/*
*********************************************************************************************************
*	�� �� ��: KEY_Detect
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    ��: IO��id�� ��0��ʼ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void KEY_Detect(uint8_t i)
{
	KEY_T *pBtn;

	pBtn = &s_tBtn[i];
	if (pBtn->IsKeyDownFunc())
	{
		if (pBtn->Count < KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)
		{
			pBtn->Count++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;

				/* ���Ͱ�ť���µ���Ϣ */
				KEY_FIFO_Put((uint8_t)(3 * i + 1));
			}

			if (pBtn->LongTime > 0)
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if (++pBtn->LongCount == pBtn->LongTime)
					{
						/* ��ֵ���밴��FIFO */
						KEY_FIFO_Put((uint8_t)(3 * i + 3));
					}
				}
				else
				{
					if (pBtn->RepeatSpeed > 0)
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* ��������ÿ��10ms����1������ */
							KEY_FIFO_Put((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else
	{
		if(pBtn->Count > KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count != 0)
		{
			pBtn->Count--;
		}
		else
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* ���Ͱ�ť�������Ϣ */
				KEY_FIFO_Put((uint8_t)(3 * i + 2));
			}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}


/***************************** �ǹ�о www.zhiguoxin.cn (END OF FILE) *********************************/
