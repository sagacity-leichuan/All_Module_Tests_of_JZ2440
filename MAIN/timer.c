/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板定时器时间相关代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "interrupt.h"
#include "timer.h"
#include "string_utils.h"

#define TIMER_NUM  32

STimerDesc timer_array[TIMER_NUM];

static unsigned long long g_system_time_10ms_cnt = 0;

/**********************************************************************************
  * @brief       : 	一般的粗糙的延时函数
  * @param[in]   : 	d	延时的值
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void Delay(volatile int d)
{
	while (d--);
}

/**********************************************************************************
  * @brief       : 	注册定时器中断处理函数
  * @param[in]   : 	name	定时器的名字
  					fp		定时器中断处理函数指针
  * @param[out]  : 	无
  * @return      : 	-1	注册失败
  					0	注册成功
  * @others      : 	无
***********************************************************************************/
int RegisterTimer(char *name, TimerFunc fp)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (!timer_array[i].fp)
		{
			timer_array[i].name = name;
			timer_array[i].fp   = fp;
			return 0;
		}
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	注销定时器中断处理函数
  * @param[in]   : 	name	定时器的名字
  * @param[out]  : 	无
  * @return      : 	-1	注销失败
  					0	注销成功
  * @others      : 	无
***********************************************************************************/
int UnregisterTimer(char *name)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (!strcmp(timer_array[i].name, name))
		{
			timer_array[i].name = NULL;
			timer_array[i].fp   = NULL;
			return 0;
		}
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	定时器总中断处理函数
  * @param[in]   : 	irq	中断号
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void TimerIrq(int irq)
{
	int i;
	
	g_system_time_10ms_cnt++;
	
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (timer_array[i].fp)
		{
			timer_array[i].fp();
		}
	}	
}

/**********************************************************************************
  * @brief       : 	初始化定时器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitTimer(void)
{
	/* 设置TIMER0的时钟 */
	/* Timer clk = PCLK / {prescaler value+1} / {divider value} 
	             = 50000000/(4+1)/2
	             = 5000000
	   每减1, 对应0.2us
	   每减5, 对应1us
	   50000-->0 : 对应 10ms
	 */
	 
	TCFG0 = 4;  /* Prescaler 0 = 4, 用于timer0,1 */
	TCFG1 &= ~0xf; /* MUX0 : 1/2 */

	/* 设置TIMER0的初值 */
	TCNTB0 = 50000;  /* 10Ms中断一次 */

	/* 加载初值, 启动timer0 */
	TCON |= (1<<1);   /* Update from TCNTB0 & TCMPB0 */

	/* 设置为自动加载并启动 */
	TCON &= ~(1<<1);
	TCON |= (1<<0) | (1<<3);  /* bit0: start, bit3: auto reload */

	/* 设置中断 */
	RegisterIrq(10, TimerIrq);
}

/**********************************************************************************
  * @brief       : 	关闭定时器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
int StopTimer(void)
{
	TCON &= ~(1<<0);
	return 0;
}

/**********************************************************************************
  * @brief       : 	启动定时器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
int StartTimer(void)
{
	TCON |= (1<<0);
	return 0;
}

/**********************************************************************************
  * @brief       : 	us延时函数
  * @param[in]   : 	n	需要延时的us值
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	尽量少调用函数
***********************************************************************************/
void uDelay(int n)
{
	int cnt = n * 5;  /* u us 对应n*5个计数值 */
	int pre = TCNTO0;
	int cur;
	int delta;

	while (cnt > 0)
	{
		cur = TCNTO0;
		if (cur <= pre)
			delta = pre - cur;
		else
			delta = pre + (50000 - cur);

		cnt = cnt - delta;
		pre = cur;
	}
}

/**********************************************************************************
  * @brief       : 	ms延时函数
  * @param[in]   : 	m  	需要延时的ms值
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	尽量少调用函数
***********************************************************************************/
void mDelay(int m)
{
	uDelay(m*1000);
}

/**********************************************************************************
  * @brief       : 	获得系统的us值
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	返回获得的us值
  * @others      : 	尽量少调用函数
***********************************************************************************/
unsigned long long GetSystemTimeUs(void)
{
	unsigned long long us = (50000 - TCNTO0)/5;
	return g_system_time_10ms_cnt * 10 * 1000 + us;
}

/**********************************************************************************
  * @brief       : 	获得us差值
  * @param[in]   : 	pre	上次获得的us值
  					now	这次获得的us值
  * @param[out]  : 	无
  * @return      : 	返回us差值
  * @others      : 	无
***********************************************************************************/
unsigned int DeltaTimeUs(unsigned long long pre, unsigned long long now)
{
	return (now - pre);
}

