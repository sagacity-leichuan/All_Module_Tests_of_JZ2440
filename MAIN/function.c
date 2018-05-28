#include "function.h"
#include "my_printf.h"
#include "init.h"
#include "s3c2440_soc.h"
#include "uart.h"
#include "font.h"
#include "timer.h"
#include "led.h"
#include "lcd.h"
#include "framebuffer.h"
#include "geometry.h"
#include "touchscreen.h"
#include "tslib.h"


void Delay(volatile int d)
{
	while (d--);
}

void InitCalibration(void)
{
	unsigned int iFbBase;
	int iXres, iYres, iBpp;
	int iRes;

	/* 获得LCD的参数: fb_base, xres, yres, bpp */
	GetLcdParams(&iFbBase, &iXres, &iYres, &iBpp);

	/* 清屏 */
	ClearScreen(0x0);

	/* 显示文字提示较准 */
	PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0xffffff,0);

	while(1)
	{
		iRes = CalibrateTs();
		if(iRes == 1)
		{
			break;
		}
		PrintFbString8x16(70, iYres - 95, "Calibration failed, Please try again!", 0xffffff,1);
		Delay(2500000);
		PrintFbString8x16(70, iYres - 95, "Calibration failed, Please try again!", 0x0,1);		
	}
	
	/* 显示文字提示绘画 */
	PrintFbString8x16(70, iYres - 95, "OK! Calibration Triumphantly !", 0xffffff,1);
	Delay(2500000);
}

void DisplayTestItemIcon(int x,int y,char * name,unsigned int color)
{
	DrawRectangle(x,y,140,45,0);

	PrintFbString16x32(x+8, y+6, name, color, 0);
}

void BootInit(void)
{
	InitLed();
	InitLcd();
	EnableLcd();
	InitFont();
	InitTimer();
	InitTouchScreen();
	GetLcdParamsforFb();
	InitCalibration();	
}


void BootScreen(void)
{
	unsigned int uiFbBase;
	int iXres, iYres, iBpp;
	int x, y;
	unsigned char *puchBpp8;
	unsigned short *pusBpp16;
	unsigned int *puiBpp32;
	unsigned int iRgb = 0X0;
	
	GetLcdParams(&uiFbBase, &iXres, &iYres, &iBpp);

	/* 往framebuffer中写数据 */
	if (iBpp == 8)
	{
		/* palette[0] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 0;		
	}
	else if (iBpp == 16)
	{

		/* black */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = Convert32BppTo16Bpp(0x7cfc00);
			
	}
	else if (iBpp == 32)
	{
		/* black */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0x7cfc00;
	}

	Delay(500000);

	PrintFbString8x16(10, 10, "Welcome to the project show: JZ2440v2 full module test", iRgb, 1);

	Delay(2000000);

	PrintFbString8x16(10, 50, "Author: LeiChuan", iRgb, 1);

	Delay(2000000);

	PrintFbString8x16(10, 90, "QQ: 793805526", iRgb, 1);

	Delay(2000000);

	PrintFbString8x16(10, 130, "Email: sagacity_lc@163.com", iRgb, 1);

	Delay(2000000);

	PrintFbString8x16(10, 170, "Github: git@github.com:sagacity-leichuan/", iRgb, 1);	
	PrintFbString8x16(10, 190, "        All_Modules_Tested_of_JZ2440.git", iRgb, 1);

	
	Delay(2000000);
	PrintFbString8x16(10, 230, "Now,Project show starts right away......", iRgb, 1);

	Delay(5000000);

}


void MainPage(void)
{
	unsigned int uiFbBase;
	int iXres, iYres, iBpp;
	int x, y;
	unsigned char *puchBpp8;
	unsigned short *pusBpp16;
	unsigned int *puiBpp32;
	unsigned int iRgb = 0X0;

	
	GetLcdParams(&uiFbBase, &iXres, &iYres, &iBpp);

	/* 往framebuffer中写数据 */
	if (iBpp == 8)
	{
		/* palette[0] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 0;		
	}
	else if (iBpp == 16)
	{

		/* black */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = Convert32BppTo16Bpp(0xf4a460);
			
	}
	else if (iBpp == 32)
	{
		/* black */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0xf4a460;
	}

	PrintFbString16x32(176, 5, "MainPage", iRgb, 0);
	DisplayTestItemIcon(20,45, "LED Test",iRgb);
	DisplayTestItemIcon(170,45,"KEY Test",iRgb);
	DisplayTestItemIcon(320,45,"LCD Test",iRgb);

	DisplayTestItemIcon(20,99, " TS Test",iRgb);
	DisplayTestItemIcon(170,99,"I2C Test",iRgb);
	DisplayTestItemIcon(320,99,"SPI Test",iRgb);

	DisplayTestItemIcon(20,153, "IDR Test",iRgb);
	DisplayTestItemIcon(170,153,"DHT1Test",iRgb);
	DisplayTestItemIcon(320,153,"DS18Test",iRgb);

	DisplayTestItemIcon(20,207, "0038Test",iRgb);
	DisplayTestItemIcon(170,207,"NOR Test",iRgb);
	DisplayTestItemIcon(320,207,"NANDTest",iRgb);
	
}






