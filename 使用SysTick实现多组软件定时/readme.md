**摘要**：在单片机中，一想到定时器可能就会想到通用定时器(TIM2 ~ TIM5 和 TIM9 ~ TIM14)或者高级定时器(TIM1和TIM8)。这些定时器的功能很强大，除了基本的功能就是定时，还可以可以测量输入信号的脉冲宽度，可以生产输出波形。

当然使用起来相对也比较复杂。如果我们的项目只想要定时的功能，使用这些定时器可能就有点不必要了，其实系统定时器`SysTick`也可以实现软件定时，只不过在裸机中我们大多是只是把他当做延时功能使用。

# 一、SysTick简介

`SysTick`—系统定时器是属于CM4内核中的一个外设，内嵌在NVIC中。一般我们叫他系统定时器或者滴答定时器。是一个`24bit`的向下递减的计数器，计数器每计数一次的时间为`1/SYSCLK`，当重装载数值寄存器的值递减到 0的时候，系统定时器就产生一次中断，以此循环往复。系统定时器一般用于操作系统，用于产生时基，维持操作系统的心跳。比如`RTOS`的心跳就是`SysTick`产生的。

# 二、SysTick寄存器

SysTick—系统定时有4个寄存器，简要介绍如下。在使用`SysTick`产生定时的时候，只需要配置前三个寄存器，最后一个校准寄存器不需要使用。

![ ](https://img-blog.csdnimg.cn/b2249e8ba4534826a38b67a4f1ba6464.png)

`SysTick`—系统定时器是属于 CM4内核中的一个外设,所以在`core_cm4.h`文件中可以看对它对应结构体的介绍。

![ ](https://img-blog.csdnimg.cn/43beb8a896fd4a788a29a532721ae2d9.png)

# 三、配置SysTick寄存器

`Systick`是一个 24 位的递减计数器，我们仅需掌握`ARM`的`CMSIS`软件提供的一个函数

`SysTick_Config`即可，原代码如下：

```c
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
{
	// 不可能的重装载值，超出范围
	if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
	{
		return (1UL);
	}

	// 设置重装载寄存器
	SysTick->LOAD = (uint32_t)(ticks - 1UL);

	// 设置中断优先级,优先级最低
	NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);

	// 设置当前数值寄存器
	SysTick->VAL = 0UL;

	// 设置系统定时器的时钟源为 AHBCLK=180M
	// 使能系统定时器中断
	// 使能定时器
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_TICKINT_Msk |
					SysTick_CTRL_ENABLE_Msk;	
	return (0UL);
}
```
![ ](https://img-blog.csdnimg.cn/66dd83bbcc2948dcbcfdbbf4b4e46a8f.png)
* 第 2024 行，函数的形参用于配置滴答定时器` LOAD `寄存器的数值，由于滴答定时器是一个递减计数器，启动后是将`LOAD`寄存器的数值赋给`VAL`寄存器，然后`VAL`寄存器做递减操作，等递减到 0 的时候重新加载`LOAD`寄存器的数值继续做递减操作。
函数的形参表示内核时钟多少个周期后触发一次 Systick 定时中断，比如形参配置为如下数值。

```c
-- SystemCoreClock / 1000 表示定时频率为 1000Hz， 也就是定时周期为 1ms。
-- SystemCoreClock / 500 表示定时频率为 500Hz， 也就是定时周期为 2ms。
-- SystemCoreClock / 2000 表示定时频率为 2000Hz， 也就是定时周期为 500us。
注：SystemCoreClock 是 STM32F407 的系统主频 168MHz。
```

* 第 2029 行，此函数设置滴答定时器为最低优先级。
* 第 2032 行，配置滴答定时器的控制寄存器，使能滴答定时器中断。滴答定时器的中断服务程序实现比较简单，没有清除中断标志这样的操作，仅需填写用户要实现的功能即可。

用固件库编程的时候我们只需要调用库函数`SysTick_Config()`即可，形参`ticks`用来设置重装载寄存器的值，当重装载寄存器的值递减到0的时候产生中断，然后重装载寄存器的值又重新装载往下递减计数，以此循环往复。紧随其后设置好中断优先级，最后配置系统定时器的时钟，使能定时器和定时器中断，这样系统定时器就配置好了，一个库函数搞定。


# 四、SysTick实现延时功能

实现延时功能就很简单了，调用SysTick_Config函数，将定时器的重装载值传递进去就可以了，然后结合时钟就可以定时中断。

![ ](https://img-blog.csdnimg.cn/259f3811d45b43a485ceb338bac6a7f8.png)

**那这个参数到底是怎么设置的呢？**

答：`SystemCoreClock`是固件中定义的系统内核时钟，对于STM32F4xx，一般为 168MHz，不要跟我说你的SystemCoreClock是160MHz。

SysTick定时器的计数器是向下递减计数的，计数一次的时间$T_{DEC}=\frac{1}{CLK_{\mathrm{AHB}}}$，当重装载寄存器中的值$LOAD$减到0的时候，产生中断，可知中断一次的时间

$T_{\mathrm{INT}}= LOAD * \frac{1}{CLK_{\mathrm{AHB}}}$ =$\frac{LOAD}{CLK_{\mathrm{AHB}}}$，

其中${CLK_{\mathrm{AHB}}}$ $=$ SystemCoreClock $=168 \mathrm{MHz}$。如果设置为168，那中断一次的时间$T_{\mathrm{INT}}$=$168/168MHz = 1us$。不过 $1us $的中断没啥意义，整个程序的重心都花在进出中断上了，根本没有时间处理其他的任务。所以我们设置重装载值为SystemCoreClock / 1000 =168 0000，那中断一次的时间T_INT = 168 0000/168MHz = 1ms。因此也就不难理解：	

```c
SystemCoreClock / 1000  表示定时频率为 1000Hz， 也就是定时周期为  1ms
SystemCoreClock / 500   表示定时频率为 500Hz，  也就是定时周期为  2ms
SystemCoreClock / 2000  表示定时频率为 2000Hz， 也就是定时周期为  500us
```
对于常规的应用，我们一般取定时周期1ms。对于低速CPU或者低功耗应用，可以设置定时周期为 10ms。

然后定时周期设置好了以后，等到时间到了，就会跳转到`SysTick `定时器中断服务函数中，这个函数在`stm32fxxx.it.c`文件中。如果你想改变这里面的代码，或者在别的地方重写，就要注释掉这个函数。

```c
void SysTick_Handler(void)
{
	TimingDelay_Decrement();	
}
```
# 五、SysTick实现多组软件定时
前面是铺垫，讲的是SysTick的基础知识。下面才是重点，既然是一个定时器，我们就不能简单是使用他的延时功能。在单片机中，一想到定时器可能就会想到通用定时器(TIM2 ~ TIM5 和 TIM9 ~TIM14)或者高级定时器(TIM1  和 TIM8)。这些定时器的功能很强大，除了基本的功能就是定时，还可以可以测量输入信号的脉冲宽度，可以生产输出波形。当然使用起来也比较麻烦，如果我们的项目只想要定时的功能，使用这些定时器可能就有点不必要了，其实系统定时器SysTick也可以实现软件定时，只不过在裸机中我们大多是只是把他当做延时功能使用。

既然要实现定时功能，我们肯定需要定时不同的时间，比如定时500ms，LED灯翻转一次。定时200ms，蜂鸣器响一次等等。所以为了实现多组延时我们就需要一个结构体。
## 1、定时器结构体
```c
/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
	volatile uint8_t Mode;		/* 计数器模式，1次性 */
	volatile uint8_t Flag;		/* 定时到达标志  */
	volatile uint32_t Count;	/* 计数器 */
	volatile uint32_t PreLoad;	/* 计数器预装值 */
}SOFT_TMR;
```
* Mode：计数器模式，1次性还是多次。
* Flag：定时到达标志，定时时间到了，flag为1。
* Count：计数器。
* PreLoad：计数器预装值 。

然后定义一个结构体数组变量，因为是多组软件定时。

```c
#define TMR_COUNT	4		/* 软件定时器的个数 （定时器ID范围 0 - 3) */
/* 定于软件定时器结构体变量 */
static SOFT_TMR s_tTmr[TMR_COUNT];
```
在此定义若干个软件定时器全局变量。这里必须增加`__IO`即`volatile`，因为这个变量在中断和主程序中同时被访问，有可能造成编译器错误优化。`TMR_COUNT`是定时器的个数，你可以设置为其他值，实现多个定时器。

## 2、定时器初始化

定时器初始化主要是清空结构体变量的值，然后只需要调用`SysTick_Config();`函数即可，这在前面已经详细的介绍过了。
```c
void Soft_TimerInit(void)
{
	uint8_t i;
	/* 清零所有的软件定时器 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TMR_ONCE_MODE;	/* 缺省是1次性工作模式 */
	}
	SysTick_Config(SystemCoreClock / 1000);/*SystemCoreClock / 1000是重装载寄存器的值LOAD*/	
}
```
## 3、启动定时器
这个函数主要给定时器赋重装载值。结构体变量赋值前后做了开关中断操作，因为此结构体变量在滴答定时器中断里面也要调用，防止变量赋值出问题。重装载值赋值完成了程序就可以周期性的进行定时中断了，也就是到**定时器中断服务函数**中执行相应代码了。
```c
void StartTimer(uint8_t _id, uint32_t _period)
{
	DISABLE_INT(); /* 关中断 */	
	s_tTmr[_id].Count = _period;		/* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 1次性工作模式 */	
	ENABLE_INT();  				/* 开中断 */
}
```

```c
void SysTick_Handler(void)
{
  SysTick_ISR();	/* 滴答定时中断服务程序 */
}
```
## 4、定时器中断服务函数
比如我们设置定时器的定时周期为`1ms`，那么每隔1ms程序就会进入`SysTick_Handler`中一次，在`SysTick_Handler`函数中调用`SysTick_ISR`函数来对软件定时器的计数器进行减一操作，因为这里设置了`TMR_COUNT`组软件定时，就需要对每一组的`count`进行减一操作，如果定时器变量减到1则设置定时器到达标志，表示定时结束。
```c
void SysTick_ISR(void)
{
	uint8_t i;
	/* 每隔1ms，对软件定时器的计数器进行减一操作 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		SoftTimerDec(&s_tTmr[i]);
	}
}

static void SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->Count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--_tmr->Count == 0)
		{
			_tmr->Flag = 1; /* Flag = 1 在检查定时器时间中会用到 */
		}
	}
}
```
## 5、检测定时器是否超时
前面已经打开了软件定时器，那么在程序中就需要来检测定时时间是否到达。比如我定时了500ms，500ms时间到了我要干什么，就需要有一个函数来检测定时器是否超时，如果没有超时无操作，如果时间已经到Flag就会等于1，需要重新将其清0。简单来说这个函数就是清0标志位的。
```c
uint8_t CheckTimer(uint8_t _id)
{
	/*判断时间到标志值 Flag 是否置位，如果置位表示时间已经到，如果为 0，表示时间还没有到*/
	if (s_tTmr[_id].Flag == 1)
	{
		s_tTmr[_id].Flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}
```

至此使用SysTick滴答定时器做软件定时器就已经完成，下面看一下如何使用。

# 六、实验例程

```c
int main(void)
{
	HAL_Init(); //初始化HAL库 
	Stm32_Clock_Init();//初始化系统时钟 
	Soft_TimerInit();	//初始化软件定时器
	Bsp_Init();//初始化底层硬件 
	StartAutoTimer(0, 1000);	/* 启动1个1000ms的自动重装的定时器 */		
	StartAutoTimer(1, 500);		/* 启动1个500ms的自动重装的定时器 */		
	StartAutoTimer(2, 200);		/* 启动1个200ms的自动重装的定时器 */		
	StartAutoTimer(3, 100);		/* 启动1个100ms的自动重装的定时器 */		
	while(1)
	{
		if (CheckTimer(0))
		{
			/* 每隔1000ms 进来一次. */
			......执行相应代码.......
		}
		if (CheckTimer(1))
		{
			/* 每隔500ms 进来一次. */
			......执行相应代码.......
		}
		if (CheckTimer(2))
		{
			/* 每隔200ms 进来一次.*/
			......执行相应代码.......
		}
		if (CheckTimer(3))
		{
			/* 每隔100ms 进来一次.*/
			......执行相应代码.......
		}
	}
}
```

# 七、驱动移植和使用
可以直接把文件移植到你的工程中使用。按键移植步骤如下：
1. 第1步：复制timer.c和timer.h到自己的工程目录，并添加到工程里面。
2. 第2步：根据需要的宏定义个数，修改下面的宏定义即可

```c
#define TMR_COUNT 4/*软件定时器的个数（定时器ID范围0-3）*/
```

源码以上传至gitee仓库。
```c
https://gitee.com/zhiguoxin/Wechat-Data.git
```