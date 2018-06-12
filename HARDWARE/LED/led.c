/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板led代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "led.h"
#include "timer.h"
#include "tslib.h"
#include "framebuffer.h"
#include "font.h"

/**********************************************************************************
  * @brief       : 	初始化led相关资源
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	0	无实际意义
  * @others      : 	无
***********************************************************************************/
int InitLed(void)
{
	/* 设置GPFCON让GPF4/5/6配置为输出引脚 */
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));
	return 0;
}

/**********************************************************************************
  * @brief       : 	led测试函数
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void TestLED(void)
{
	int iVal = 0;  /* val: 0b000, 0b111 */
	int iTmp = 0;

	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(176, 5, "LED TEST", 0xe3170d, 0);
	PrintFbString8x16(100, 110, "     LED testing is in progress......", 0x0,0);
	DisplayReturnButton();
	
	/* 循环点亮 */
	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			break;
		}
		
		iTmp = ~iVal;
		iTmp &= 7;
		GPFDAT &= ~(7<<4);
		GPFDAT |= (iTmp<<4);
		Delay(100000);
		iVal++;
		if (iVal == 8)
			iVal =0;
		
	}	
}

