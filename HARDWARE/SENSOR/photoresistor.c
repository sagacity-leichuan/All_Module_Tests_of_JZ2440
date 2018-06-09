#include "sensor.h"
#include "timer.h"
#include "my_printf.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"
#include "string_utils.h"
#include "nand_flash.h"

extern int iPhotoresistorEnd;

void TestPhotoresistor(void)
{
	int val, val0;
	double vol, vol0;
	int m, m0; /* 整数部分 */
	int n, n0; /* 小数部分 */
	char chBuffer[10];
	
	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(120, 5, "Photoresistor TEST", 0xe3170d, 0);

	PrintFbString8x16(40, 120, "Please ensure that the corresponding module is properly connected.", 0x4169e1, 1);
	mDelay(5000);
	PrintFbString8x16(40, 120, "Please ensure that the corresponding module is properly connected.", 0xffffff, 1);

	DisplayReturnButton();

	PrintFbString8x16(40, 240, "This test cannot return to the main page using the return button. You can use the eint19 button to return.", 0xff0000, 0);

	while (1)
	{

		if(iPhotoresistorEnd)
		{
			MainPage();
			iPhotoresistorEnd = 0;
			break;
		}
	
		val = ReadADC(1);
		vol = (double)val/1023*3.3;   /* 1023----3.3v */
		m = (int)vol;	/* 3.01, m = 3 */
		vol = vol - m;	/* 小数部分: 0.01 */
		n = vol * 1000;  /* 10 */

		val0 = ReadADC(0);
		vol0 = (double)val0/1023*3.3;   /* 1023----3.3v */
		m0 = (int)vol0;	/* 3.01, m = 3 */
		vol0 = vol0 - m0;	/* 小数部分: 0.01 */
		n0 = vol0 * 1000;  /* 10 */

		/* 在串口上打印 */
		printf("photoresistor vol: %d.%03dv, compare to threshold %d.%03dv\r", m, n, m0, n0);  /* 3.010v */

		/* 在LCD上打印 */
		PrintFbString8x16(90, 100, "Photoresistor vol: ", 0x4169e1, 0);
		Convert(chBuffer, m, 10);
		PrintFbString8x16(242, 100,chBuffer, 0x4169e1, 0);
		PrintFbString8x16(250, 100, ".", 0x4169e1, 0);
		Convert(chBuffer, n, 10);
		PrintFbString8x16(258, 100,chBuffer, 0x4169e1, 0);
		PrintFbString8x16(282, 100, "dv", 0x4169e1, 0);

		PrintFbString8x16(120, 130, "compare to threshold ", 0x4169e1, 0);
		Convert(chBuffer, m0, 10);
		PrintFbString8x16(288, 130,chBuffer, 0x4169e1, 0);
		PrintFbString8x16(296, 130, ".", 0x4169e1, 0);
		Convert(chBuffer, n0, 10);
		PrintFbString8x16(304, 130,chBuffer, 0x4169e1, 0);
		PrintFbString8x16(328, 130, "dv", 0x4169e1, 0);

		mDelay(1000);
		Convert(chBuffer, m, 10);
		PrintFbString8x16(242, 100,chBuffer, 0xffffff, 0);
		Convert(chBuffer, n, 10);
		PrintFbString8x16(258, 100,chBuffer, 0xffffff, 0);

		Convert(chBuffer, m0, 10);
		PrintFbString8x16(288, 130,chBuffer, 0xffffff, 0);
		Convert(chBuffer, n0, 10);
		PrintFbString8x16(304, 130,chBuffer, 0xffffff, 0);
	
	}	
}



