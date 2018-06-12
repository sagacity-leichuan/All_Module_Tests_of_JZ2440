/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板红外接收传感器原始数据代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

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

 /**************************************************************************************************
   * @brief 	  :  配置GPIO作为中断引脚为接收红外的高低边缘中断触发
   * @param[in]   :  无
   * @param[out]  :  无
   * @return	  :  无
   * @others	  :  无
 ***************************************************************************************************/
static void CfgGPIOAsEintForIrdaData(void)
{
	/* 配置为中断引脚 */
	GPFCON &= ~(3<<2);
	GPFCON |= (2<<2);

	/* 设置中断触发方式: 双边沿触发 */
	EXTINT0 |= (7<<4);  /* eint1 */

}

/**************************************************************************************************
  * @brief		 :	获得IrdaData
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	1	获得一位1数据
  					0	获得一位0数据
  * @others 	 :	无
***************************************************************************************************/
static int GetIrdaData(void)
{
	if (GPFDAT & (1<<1))
		return 1;
	else
		return 0;
}

/**************************************************************************************************
  * @brief		 :	红外接收中断处理函数
  * @param[in]	 :	irq	中断号
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
***************************************************************************************************/
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


/**************************************************************************************************
  * @brief		 :	初始化红外接收功能
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
***************************************************************************************************/
void InitIrda(void)
{
	CfgGPIOAsEintForIrdaData();
	RegisterIrq(1, IrdaIrq);
}

/**************************************************************************************************
  * @brief		 :	测试红外接收原始数据的功能
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
***************************************************************************************************/
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

