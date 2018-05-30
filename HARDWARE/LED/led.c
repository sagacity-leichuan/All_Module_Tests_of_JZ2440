#include "s3c2440_soc.h"
#include "init.h"
#include "function.h"
#include "tslib.h"
#include "framebuffer.h"
#include "font.h"

extern SButton g_sButtonReturn;

int InitLed(void)
{
	/* 设置GPFCON让GPF4/5/6配置为输出引脚 */
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));
	return 0;
}

void TestLED(void)
{
	int iVal = 0;  /* val: 0b000, 0b111 */
	int iTmp = 0;

	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(176, 5, "LED TEST", 0x7cfc00, 0);
	PrintFbString8x16(100, 110, "     LED testing is in progress......", 0x0,0);
	DisplayTestItemIcon(g_sButtonReturn.iX,g_sButtonReturn.iY,g_sButtonReturn.sName,0x0);
	
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



