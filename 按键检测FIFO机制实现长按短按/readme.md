**摘要**：今年实验室来了三个学妹，其中一个学妹以前是物联网专业的，进了实验室老师二话没说：先把STM32单片机过一遍，有啥问题就找小师弟。还好单片机小师弟会玩一点点，玩的也不好，所以一起学写吧！邪魅一笑！

上来第一个例程就是使用按键点亮一个LED灯，好家伙。点灯小师弟比较在行，毕竟32、FPGA、Linux的小灯都被小师弟点了一遍。哈哈哈！所以今天还是来说一说按键检测吧！

原文链接：[一个学妹写的按键检测函数把我秀翻了](https://mp.weixin.qq.com/s/JKa99sPD8iVkvxL1Rwv6nA)


# 一、如何进行按键检测
检测按键有中断方式和 GPIO 查询方式两种。推荐大家用GPIO查询方式。

## 1.从裸机的角度分析
* **中断方式**：中断方式可以快速地检测到按键按下，并执行相应的按键程序，但实际情况是由于按键的机械抖动特性，在程序进入中断后必须进行滤波处理才能判定是否有效的按键事件。如果每个按键都是独立的接一个 IO 引脚，需要我们给每个 IO 都设置一个中断，程序中过多的中断会影响系统的稳定性。中断方式跨平台移植困难。

* **查询方式**：查询方式有一个最大的缺点就是需要程序定期的去执行查询，耗费一定的系统资源。实际上耗费不了多大的系统资源，因为这种查询方式也只是查询按键是否按下，按键事件的执行还是在主程序里面实现。

## 2.从OS的角度分析
* **中断方式**：在 OS 中要尽可能少用中断方式，因为在RTOS中过多的使用中断会影响系统的稳定性和可预见性。只有比较重要的事件处理需要用中断的方式。
* **查询方式**：对于用户按键推荐使用这种查询方式来实现，现在的OS基本都带有CPU利用率的功能，这个按键FIFO占用的还是很小的，基本都在1%以下。


# 二、最简单的按键检测程序
先给他说了一种经典的按键检测代码，相信大多数人使用按键函数都见过它，很简单就不过多介绍了！
```c
#define KEY0_PRES	1		//KEY0  
#define KEY1_PRES	2		//KEY1 
#define WKUP_PRES	3		//WK_UP 

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(WK_UP==1)return WKUP_PRES; 
	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	     
	return 0;// 无按键按下
}

int main(void)
{	
	u8 t=0;	  
	delay_init();	    	 //延时函数初始化	  
	LED_Init();		  	 	//初始化与LED连接的硬件接口
	KEY_Init();          	//初始化与按键连接的硬件接口
	LED=0;					//点亮LED
	while(1)
	{
		t=KEY_Scan(0);		//得到键值
		switch(t)
		{				 
			case KEY0_PRES:		//如果KEY0按下
				 LED=!LED;
				break;
			default:
				delay_ms(10);	
		} 
	}
}
```
如果你在工作中使用这种代码，有可能会被同事笑话。当然我这里并不是说这种代码不好，不管黑猫白猫，能抓住老鼠就是好猫。只要能满足项目需求实现对应的功能就是好代码。但是如果你使用下面这种个人感觉可能会更好。

其实也并没有什么神秘感，就是使用了FIFO机制。参考的就是安富莱的按键例程，不过源代码相对比较复杂，对于初学者并不友好，所以小小的修改了一下，仅供参考！

在前面分享了使用系统滴答定时器实现了多个软件定时器，在按键FIFO中也需要使用这个定时器。在系统的开始我们会启动一个10ms的软件定时器。在这个10ms的软件定时器中不断的进行按键扫描，与其他的任务互不影响。

# 三、为什么要了解FIFO
要回答什么是FIFO，先要回答为什么要使用FIFO。只有搞清楚使用FIFO的好处，你才会有意无意的使用FIFO。学习FIFO机制和状态机机制一样，都是在裸机编程中非常重要的编程思想。编程思想很重要。初级coder总是在关注代码具体是怎么写，高级coder关注的是程序的框架逻辑，而不是某个细节。只要你框架逻辑通了，则一通百通。
# 四、什么是FIFO

FIFO是先入先出的意思，即谁先进入队列，谁先出去。比如我们需要串口打印数据，当使用缓存将该数据保存的时候，在输出数据时必然是先进入的数据先出去，那么该如何实现这种机制呢？首先就是建立一个缓存空间，这里假设为10个字节空间进行说明。

![ ](https://img-blog.csdnimg.cn/b680ca5a6352469e94d50ae795f5f866.png)
从这张图就知道如果要使用FIFO，就要定义一个结构体，而这个结构体至少应该有三个成员。**数组buf、读指针read、写指针write**。

```c
typedef struct
{
	uint8_t Buf[10];		/* 缓冲区 */
	uint8_t Read;			/* 缓冲区读指针*/
	uint8_t Write;			/* 缓冲区写指针 */
}KEY_FIFO_T;
```

缓存一开始没有数据，并且用一个变量write指示下一个写入缓存的索引地址，这里下一个存放的位置就是 0，用另一个变量read 指示下一个读出缓存的索引地址，并且下一个读出数据的索引地址也是 0。目前队列中是没有数据的，也就是不能读出数据，队列为空的判断条件在这里就是两个索引值相同。

现在开始存放数据：
![ ](https://img-blog.csdnimg.cn/1d34e17b798a44d0a95fd6c994724925.png)
在这里可以看到队列中加入了 9个数据，并且每加入一个数据后队尾索引加 1，队头不变，这就是数据加入队列的过程。但是缓存空间只有10个，如何判断队列已满呢？如果只是先一次性加数据到队列中，然后再读出数据，那这里的判断条件显然是队尾索引为 9。

好了这就是FIFO的基本原理，下面来看一下**按键FIFO是怎么操作的**。

我们这里以5个字节的FIFO空间进行说明。Write变量表示写位置，Read 变量表示读位置。初始状态时，Read = Write = 0。
![](https://img-blog.csdnimg.cn/ce80299bf8ff46e6b6eaf02172df7397.png)
我们依次按下按键 K1，K2，那么FIFO中的数据变为：

![ ](https://img-blog.csdnimg.cn/edcfbf99d262471ab55a921d9541ab78.png)如果 Write！= Read，则我们认为有新的按键事件。我们通过函数`KEY_FIFO_Get()`读取一个按键值进行处理后，Read 变量变为 1。Write 变量不变。
![ ](https://img-blog.csdnimg.cn/4788e0df1b9e4c3c8d07795579f11385.png)继续通过函数`KEY_FIFO_Get()`读取 3 个按键值进行处理后，Read 变量变为 4。此时 Read = Write= 4。两个变量已经相等，表示已经没有新的按键事件需要处理。

**有一点要特别的注意，如果 FIFO 空间写满了，Write 会被重新赋值为 0，也就是重新从第一个字节空间填数据进去，如果这个地址空间的数据还没有被及时读取出来，那么会被后来的数据覆盖掉，这点要引起大家的注意**。我们的驱动程序开辟了 10 个字节的 FIFO 缓冲区，对于一般的应用足够了。

# 五、按键FIFO的优点
1. 可靠地记录每一个按键事件，避免遗漏按键事件。特别是需要实现按键的按下、长按、自动连发、弹起等事件时。
2. 读取按键的函数可以设计为非阻塞的，不需要等待按键抖动滤波处理完毕。
3. 按键 FIFO 程序在嘀嗒定时器中定期的执行检测，不需要在主程序中一直做检测，这样可以有效地降低系统资源消耗。

# 六、按键 FIFO 的实现
## 1.定义结构体
在我们的key.h文件中定义一个结构体类型为`KEY_FIFO_T`的结构体。就是前面说的那个结构体。这只是类型声明，并没有分配变量空间。

```c
typedef struct
{
	uint8_t Buf[10];		/* 缓冲区 */
	uint8_t Read;			/* 缓冲区读指针*/
	uint8_t Write;			/* 缓冲区写指针 */
}KEY_FIFO_T;
```
接着在key.c 中定义 s_tKey 结构变量, 此时编译器会分配一组变量空间。

```c
static KEY_FIFO_T s_tKey;/* 按键FIFO变量,结构体 */
```
好了按键FIFO的结构体数据类型就定义完了，很简单吧！

## 2.将键值写入FIFO
既然结构体都定义好了，接着就是往这个FIFO的数组中写入数据，也就是按键的键值，用来模拟按键的动作了。
```c
/*
**********************************************************
*	函 数 名: KEY_FIFO_Put
*	功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*	形    参:  _KeyCode : 按键代码
*	返 回 值: 无
**********************************************************
*/
void KEY_FIFO_Put(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}
```
函数的主要功能就是将按键代码`_KeyCode`写入到FIFO中，而这个FIFO就是我们定义结构体的这个数组成员，每写一次，就是每调用一次`KEY_FIFO_Put()`函数，写指针write就`++`一次，也就是向后移动一个空间，如果FIFO空间写满了，也就是`s_tKey.Write  >= KEY_FIFO_SIZE`，Write会被重新赋值为 0。
## 3.从FIFO读出键值
有写入键值当然就有读出键值。

```c
/*
***********************************************************
*	函 数 名: KEY_FIFO_Get
*	功能说明: 从按键FIFO缓冲区读取一个键值。
*	形    参: 无
*	返 回 值: 按键代码
************************************************************
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
```
如果写指针和读出的指针相等，那么返回值就为0，表示按键缓冲区为空，所有的按键时间已经处理完毕。如果不相等就说明FIFO的缓冲区不为空，将Buf中的数读出给`ret`变量。同样，如果FIFO空间读完了，没有缓存了，也就是`s_tKey.Read >= KEY_FIFO_SIZE`，Read也会被重新赋值为 0。按键的键值定义在key.h 文件，下面是具体内容：

```c
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_1_DOWN,				/* 1键按下 */
	KEY_1_UP,				/* 1键弹起 */
	KEY_1_LONG,				/* 1键长按 */

	KEY_2_DOWN,				/* 2键按下 */
	KEY_2_UP,				/* 2键弹起 */
	KEY_2_LONG,				/* 2键长按 */

	KEY_3_DOWN,				/* 3键按下 */
	KEY_3_UP,				/* 3键弹起 */
	KEY_3_LONG,				/* 3键长按 */
}KEY_ENUM;
```
必须按次序定义每个键的按下、弹起和长按事件，即每个按键对象占用 3 个数值。**推荐使用枚举enum, 不用#define**的原因是便于新增键值,方便调整顺序。使用{ } 将一组相关的定义封装起来便于理解。编译器也可帮我们避免键值重复。
## 4.按键检测程序

上面说了如何将按键的键值存入和读出FIFO，但是既然是按键操作，就肯定涉及到按键消抖处理，还有按键的状态是按下还是弹起，是长按还是短按。所以为了以示区分，我们用还需要给每一个按键设置很多参数，就需要再定义一个结构体`KEY_T`，让每个按键对应1个全局的结构体变量。

```c
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	uint8_t (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

	uint8_t  Count;			/* 滤波器计数器 */
	uint16_t LongCount;		/* 长按计数器 */
	uint16_t LongTime;		/* 按键按下持续时间, 0表示不检测长按 */
	uint8_t  State;			/* 按键当前状态（按下还是弹起） */
	uint8_t  RepeatSpeed;	/* 连续按键周期 */
	uint8_t  RepeatCount;	/* 连续按键计数器 */
}KEY_T;
```
在key.c 中定义`s_tBtn`结构体数组变量。

```c
static KEY_T   s_tBtn[3] = {0};
```
每个按键对象都分配一个结构体变量，这些结构体变量以数组的形式存在将便于我们简化程序代码行数。因为我的硬件有3个按键，所以这里的数组元素为3。使用函数指针`IsKeyDownFunc`可以将每个按键的检测以及组合键的检测代码进行统一管理。

因为函数指针必须先赋值，才能被作为函数执行。因此在定时扫描按键之前，必须先执行一段初始化函数来设置每个按键的函数指针和参数。这个函数是`void KEY_Init(void)`。

```c
void KEY_Init(void)
{
	KEY_FIFO_Init();		/* 初始化按键变量 */
	KEY_GPIO_Config();		/* 初始化按键硬件 */
}
```
下面是`KEY_FIFO_Init`函数的定义：

```c
static void KEY_FIFO_Init(void)
{
	uint8_t i;

	/* 对按键FIFO读写指针清零 */
	s_tKey.Read = 0;
	s_tKey.Write = 0;

	/* 给每个按键结构体成员变量赋一组缺省值 */
	for (i = 0; i < HARD_KEY_NUM; i++)
	{
		s_tBtn[i].LongTime = 100;/* 长按时间 0 表示不检测长按键事件 */
		s_tBtn[i].Count = 5/ 2;	/* 计数器设置为滤波时间的一半 */
		s_tBtn[i].State = 0;/* 按键缺省状态，0为未按下 */
		s_tBtn[i].RepeatSpeed = 0;/* 按键连发的速度，0表示不支持连发 */
		s_tBtn[i].RepeatCount = 0;/* 连发计数器 */
	}
	/* 判断按键按下的函数 */
	s_tBtn[0].IsKeyDownFunc = IsKey1Down;
	s_tBtn[1].IsKeyDownFunc = IsKey2Down;
	s_tBtn[2].IsKeyDownFunc = IsKey3Down;
}
```
我们知道按键会有机械抖动，你以为按键按下就是低电平，其实在按下的一瞬间会存在机械抖动，如果不做延时处理，可能会出错，一般如果按键检测到按下后再延时50ms检测一次，如果还是检测低电平，才能说明按键真正的被按下了。反之按键弹起时也是一样的。所以我们程序设置按键滤波时间50ms, 因为代码每10ms扫描一次按键，所以按键的单位我们可以理解为10ms，滤波的次数就为5次。这样只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件，即使按键电路不做硬件滤波(没有电容滤波)，该滤波机制也可以保证可靠地检测到按键事件。

判断按键是否按下，用一个`HAL_GPIO_ReadPin`就可以搞定。

```c
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
```

下面是`KEY_GPIO_Config`函数的定义，这个函数就是配置具体的按键GPIO的，就不需要过多的解释了。

```c
static void KEY_GPIO_Config(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 第1步：打开GPIO时钟 */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	/* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;   			/* 设置输入 */
	GPIO_InitStructure.Pull = GPIO_NOPULL;                 /* 上下拉电阻不使能 */
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  /* GPIO速度等级 */
	
	GPIO_InitStructure.Pin = GPIO_PIN_4;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	GPIO_InitStructure.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}
```
## 5.按键扫描
按键扫描函数`KEY_Scan()`每隔 10ms 被执行一次。`RunPer10ms`函数在 `systick`中断服务程序中执行。
```c
void RunPer10ms(void)
{
	KEY_Scan();
}

void KEY_Scan(void)
{
	uint8_t i;
	for (i = 0; i < HARD_KEY_NUM; i++)
	{
		KEY_Detect(i);
	}
}
/*
```
每隔10ms所有的按键GPIO均会被扫描检测一次。`KEY_Detect`函数实现如下：

```c
static void KEY_Detect(uint8_t i)
{
	KEY_T *pBtn;
	pBtn = &s_tBtn[i];/*读取相应按键的结构体地址，程序里面每个按键都有自己的结构体。*/
	if (pBtn->IsKeyDownFunc())
	{/*这个里面执行的是按键按下的处理*/
		if (pBtn->Count < KEY_FILTER_TIME)/*用于按键滤波前给 Count 设置一个初值*/
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)/*这里实现 KEY_FILTER_TIME 时间长度的延迟*/
		{
			pBtn->Count++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;/*如果按键按下了，这里就将其设置为 1*/

				/* 发送按钮按下的消息 */
				KEY_FIFO_Put((uint8_t)(3 * i + 1));
			}

			if (pBtn->LongTime > 0) /*LongTime初始值是100。单位10ms，持续1秒，认为长按事件*/
			{
				if (pBtn->LongCount < pBtn->LongTime) /*LongCount长按计数器。单位10ms，持续1秒，认为长按事件*/
				{
					/* 发送按钮持续按下的消息 */
					if (++pBtn->LongCount == pBtn->LongTime)/*LongCount等于LongTime(100),10ms进来一次，进来了100次也就是说按下时间为于1s*/
					{
						/* 键值放入按键FIFO */
						KEY_FIFO_Put((uint8_t)(3 * i + 3));
					}
				}
				else/*LongCount大于LongTime(100),也就是说按下时间大于1s*/
				{
					if (pBtn->RepeatSpeed > 0)/* RepeatSpeed连续按键周期 */
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* 长按键后，每隔10ms发送1个按键。因为长按也是要发送键值得，10ms发送一次。*/
							KEY_FIFO_Put((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else
	{/*这个里面执行的是按键松手的处理或者按键没有按下的处理*/
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

				/* 发送按钮弹起的消息 */
				KEY_FIFO_Put((uint8_t)(3 * i + 2));
			}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}
```
这个函数还是比较难以理解的，主要是结构体的操作。所以好好学习结构体，不要见了结构体就跑。

分析：首先读取相应按键的结构体地址赋值给结构体指针变量pBtn ，因为程序里面每个按键都有自己的结构体，只有通过这个方式才能对具体的按键进行操作。(在前面我们使用软件定时器时也使用了这中操作，在滴答定时器的中断服务函数中)。

```c
static KEY_T s_tBtn[3];//程序里面每个按键都有自己的结构体,有三个按键
KEY_T *pBtn;//定义一个结构体指针变量pBtn
pBtn = &s_tBtn[i];//将按键的结构体地址赋值给结构体指针变量pBtn
```
然后接着就是给按键滤波前给Count设置一个初值，前面说按键初始化的时候已经设置了Count =5/2。然后判断是否按下的标志位，如果按键按下了，这里就将其设置为 1，如果没有按下这个变量的值就会一直是 0。这里可能不理解是就是按键按下发送的键值是`3 * i + 1`。按键弹起发送的键值是`3 * i + 2`，按键长按发送的键值是`3 * i + 3`。也就是说按键按下发送的键值是**1和4和7**。按键弹起发送的键值是**2和5和8**，按键长按发送的键值是**3和6和9**。看下面这个枚举enum你就明白了。

```c
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_1_DOWN,				/* 1键按下 */
	KEY_1_UP,				/* 1键弹起 */
	KEY_1_LONG,				/* 1键长按 */

	KEY_2_DOWN,				/* 2键按下 */
	KEY_2_UP,				/* 2键弹起 */
	KEY_2_LONG,				/* 2键长按 */

	KEY_3_DOWN,				/* 3键按下 */
	KEY_3_UP,				/* 3键弹起 */
	KEY_3_LONG,				/* 3键长按 */


}KEY_ENUM;
```
## 7.试验演示

```c
int main(void)
{
	uint8_t KeyCode;/* 按键代码 */
	KEY_Init();
	while (1)
	{		
		/* 按键滤波和检测由后台systick中断服务程序实现，我们只需要调用KEY_FIFO_Get读取键值即可。 */
		KeyCode = KEY_FIFO_Get();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
		if (KeyCode != KEY_NONE)
		{
			switch (KeyCode)
			{
				case KEY_DOWN_K1:			/* K1键按下 */
					printf("K1键按下\r\n");
					break;
				case KEY_UP_K1:				/* K1键弹起 */
					printf("K1键弹起\r\n");
					break;
				case KEY_DOWN_K2:			/* K2键按下 */
					printf("K2键按下\r\n");
					break;
				case KEY_UP_K2:				/* K2键弹起 */
					printf("K2键弹起\r\n");
					break;
				case KEY_DOWN_K3:			/* K3键按下 */
					printf("K3键按下\r\n");
					break;
				case KEY_UP_K3:				/* K3键弹起 */
					printf("K3键弹起\r\n");
					break;								
				default:
					/* 其它的键值不处理 */
					break;
			}
		
		}
	}
}
```
不知道学妹看懂没，没看懂就多看几遍。

备注：文章未经本人允许严禁转载！