/**********************************************************************************
  * @brief      : 	JZ2440v2开发板按键测试代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***********************************************************************************/

#include "key.h"
#include "s3c2440_soc.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"

int iTestStatus = 0;		//用于表示是否进入到了按键测试
int iPhotoresistorEnd = 0;		//用于在进行光敏电阻测试时返回主页面的标志

/* SRCPND 用来显示哪个中断产生了, 需要清除对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTMSK 用来屏蔽中断, 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTPND 用来显示当前优先级最高的、正在发生的中断, 需要清除对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/**********************************************************************************
  * @brief       : 	按键中断处理函数
  * @param[in]   : 	irq	中断号
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void KeyEintIrq(int irq)
{
	unsigned int val = EINTPEND;
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;
	
	if (irq == 0) /* eint0 : s2 控制 D12 */
	{
		if (val1 & (1<<0)) /* s2 --> gpf6 */
		{
			/* 松开 */
			if(iTestStatus)
			{
				PrintFbString8x16(150, 110, "KEY_EINT0 is released!", 0x0,1);
				PrintFbString8x16(150, 110, "KEY_EINT0 is released!", 0xffffff,1);
			}
			GPFDAT |= (1<<6);
		}
		else
		{
			/* 按下 */
			if(iTestStatus)
			{
				PrintFbString8x16(150, 110, "KEY_EINT0 is pressed!", 0x0,1);
				PrintFbString8x16(150, 110, "KEY_EINT0 is pressed!", 0xffffff,1);
			}
			GPFDAT &= ~(1<<6);
		}
		
	}
	else if (irq == 2) /* eint2 : s3 控制 D11 */
	{
		if (val1 & (1<<2)) /* s3 --> gpf5 */
		{
			/* 松开 */
			if(iTestStatus)
			{
				PrintFbString8x16(150, 110, "KEY_EINT2 is released!", 0x0,1);
				PrintFbString8x16(150, 110, "KEY_EINT2 is released!", 0xffffff,1);
			}
			GPFDAT |= (1<<5);
		}
		else
		{
			/* 按下 */
			if(iTestStatus)
			{
				PrintFbString8x16(150, 110, "KEY_EINT2 is pressed!", 0x0,1);
				PrintFbString8x16(150, 110, "KEY_EINT2 is pressed!", 0xffffff,1);
			}
			GPFDAT &= ~(1<<5);
		}
		
	}
	else if (irq == 5) /* eint8_23, eint11--s4 控制 D10, eint19---s5 控制所有LED */
	{
		if (val & (1<<11)) /* eint11 */
		{
			if (val2 & (1<<3)) /* s4 --> gpf4 */
			{
				/* 松开 */
				if(iTestStatus)
				{
					PrintFbString8x16(150, 110, "KEY_EINT11 is released!", 0x0,1);
					PrintFbString8x16(150, 110, "KEY_EINT11 is released!", 0xffffff,1);
				}
				GPFDAT |= (1<<4);
			}
			else
			{
				/* 按下 */
				if(iTestStatus)
				{
					PrintFbString8x16(150, 110, "KEY_EINT11 is pressed!", 0x0,1);
					PrintFbString8x16(150, 110, "KEY_EINT11 is pressed!", 0xffffff,1);
				}
				GPFDAT &= ~(1<<4);
			}
		}
		else if (val & (1<<19)) /* eint19 */
		{
			if (val2 & (1<<11))
			{
				/* 松开 */
				/* 熄灭所有LED */
				if(iTestStatus)
				{
					PrintFbString8x16(150, 110, "KEY_EINT19 is released!", 0x0,1);
					PrintFbString8x16(150, 110, "KEY_EINT19 is released!", 0xffffff,1);
				}
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* 按下: 点亮所有LED */
				if(iTestStatus)
				{
					PrintFbString8x16(150, 110, "KEY_EINT19 is pressed!", 0x0,1);
					PrintFbString8x16(150, 110, "KEY_EINT19 is pressed!", 0xffffff,1);
				}

				iPhotoresistorEnd = 1;
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
		}
	}

	EINTPEND = val;
}

/****************************************************************************************
  * @brief       : 	初始化按键的GPIO管脚
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
*****************************************************************************************/
void InitKey(void)
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));

	/* 配置GPIO为中断引脚 */
	GPFCON &= ~((3<<0) | (3<<4));
	GPFCON |= ((2<<0) | (2<<4));   /* S2,S3被配置为中断引脚 */

	GPGCON &= ~((3<<6) | (3<<22));
	GPGCON |= ((2<<6) | (2<<22));   /* S4,S5被配置为中断引脚 */
	

	/* 设置中断触发方式: 双边沿触发 */
	EXTINT0 |= (7<<0) | (7<<8);     /* S2,S3 */
	EXTINT1 |= (7<<12);             /* S4 */
	EXTINT2 |= (7<<12);             /* S5 */

	/* 设置EINTMASK使能eint11,19 */
	EINTMASK &= ~((1<<11) | (1<<19));

	RegisterIrq(0, KeyEintIrq);
	RegisterIrq(2, KeyEintIrq);
	RegisterIrq(5, KeyEintIrq);	
}

/**********************************************************************************************
  * @brief       : 	测试按键功能，并在LCD中显示按键按下松开状态
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************************/
void TestKey(void)
{

	iTestStatus = 1;
	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(176, 5, "KEY TEST", 0xe3170d, 0);
	DisplayReturnButton();
	
	/* 循环点亮 */
	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			iTestStatus = 0;
			break;
		}	
	}		
}


