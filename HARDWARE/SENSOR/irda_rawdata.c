#include "s3c2440_soc.h"
#include "irda_rawdata.h"
#include "circle_buffer.h"
#include "timer.h"
#include "my_printf.h"
#include "interrupt.h"

/* IRDA引脚 : EINT1/GPF1 */

static unsigned long long gs_ulLastTime = 0;

/* 
 * 配置GPIO, 注册中断
 * 在中断处理函数里:
      记录中断发生的时间,
      跟上次中断的时间比较, 计算出脉冲宽度
      读取引脚极性
      把数据放入环型缓冲区
 */

 /* 先实现GPIO的基本操作 */
static void CfgIrdaDataAsEint(void)
{
	/* 配置为中断引脚 */
	GPFCON &= ~(3<<2);
	GPFCON |= (2<<2);

	/* 设置中断触发方式: 双边沿触发 */
	EXTINT0 |= (7<<4);  /* eint1 */

}

static int GetIrdaData(void)
{
	if (GPFDAT & (1<<1))
		return 1;
	else
		return 0;
}

void IrdaIrq(int irq)
{
	/* 在中断处理函数里:
		 记录中断发生的时间,
		 跟上次中断的时间比较, 计算出脉冲宽度
		 读取引脚极性
		 把数据放入环型缓冲区
	*/
	SIrdaRawdataEvent event;
	unsigned long long cur = GetSystemTimeUs();
	
	event.iDuration = DeltaTimeUs(gs_ulLastTime, cur);
	event.iPol      = !GetIrdaData();
	PutIrEvent(&event);
	gs_ulLastTime = cur;
}


/* 注册中断 */
void InitIrda(void)
{
	CfgIrdaDataAsEint();
	RegisterIrq(1, IrdaIrq);
}

void TestIrdaRawdata(void)
{
	SIrdaRawdataEvent event;
	unsigned long long pre = 0, cur;
	
	InitIrda();

	while (1)
	{
		if (0 == GetIrEvent(&event))
		{
			cur = GetSystemTimeUs();
			if (DeltaTimeUs(pre, cur) > 1000000)
				printf("\n\r");
			pre = cur;
			printf("%s %d us | ", event.iPol? "hight" : "low", event.iDuration);
		}
	}
}

