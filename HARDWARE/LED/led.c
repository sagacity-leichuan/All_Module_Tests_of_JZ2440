#include "s3c2440_soc.h"
#include "init.h"
#include "function.h"



int InitLed(void)
{
	/* 设置GPFCON让GPF4/5/6配置为输出引脚 */
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));
	return 0;
}

void LEDTest(void)
{
	int iVal = 0;  /* val: 0b000, 0b111 */
	int iTmp = 0;

	/* 循环点亮 */
	while (1)
	{
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



