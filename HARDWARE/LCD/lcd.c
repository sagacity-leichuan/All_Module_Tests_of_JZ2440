#include "lcd.h"
#include "lcd_controller.h"
#include "string_utils.h"
#include "lcd_4.3.h"
#include "function.h"
#include "geometry.h"
#include "font.h"




#define LCD_NUM 10

static PSLcdParams g_psArrayLcd[LCD_NUM];
static PSLcdParams g_psLcdSelected;

int RegisterLcd(PSLcdParams plcd)
{
	int i;
	for (i = 0; i < LCD_NUM; i++)
	{
		if (!g_psArrayLcd[i])
		{
			g_psArrayLcd[i] = plcd;
			return i;
		}
	}
	return -1;		
}

int SelectLcd(char *name)
{
	int i;
	for (i = 0; i < LCD_NUM; i++)
	{
		if (g_psArrayLcd[i] && !strcmp(g_psArrayLcd[i]->pchName, name))
		{
			g_psLcdSelected = g_psArrayLcd[i];
			return i;
		}
	}
	return -1;		
}

void GetLcdParams(unsigned int *fb_base, int *xres, int *yres, int *bpp)
{
	*fb_base = g_psLcdSelected->nFbBase;
	*xres = g_psLcdSelected->iXres;
	*yres = g_psLcdSelected->iYres;
	*bpp = g_psLcdSelected->iBpp;
}

void EnableLcd(void)
{
	EnableLcdController();
}

void DisableLcd(void)
{
	DisableLcdController();
}

int InitLcd(void)
{
	/* 注册LCD */
	Add4Point3Lcd();

	/* 注册LCD控制器 */
	AddLcdContollerS3c2440();
	
	/* 选择某款LCD */
	SelectLcd("lcd_4.3");

	/* 选择某款LCD控制器 */
	SelectLcdController("s3c2440");

	/* 使用LCD的参数, 初始化LCD控制器 */
	InitLcdController(g_psLcdSelected);

	return 0;
}


void LCDTest(void)
{
	unsigned int uiFbBase;
	int iXres, iYres, iBpp;
	int x, y;
	unsigned char *puchBpp8;
	unsigned short *pusBpp16;
	unsigned int *puiBpp32;
	
	GetLcdParams(&uiFbBase, &iXres, &iYres, &iBpp);

	/* 往framebuffer中写数据 */
	if (iBpp == 8)
	{
		/* 让LCD输出整屏的红色 */

		/* bpp: palette[12] */

		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 12;

		Delay(500000);

		/* palette[47] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 47;

		Delay(500000);

		/* palette[88] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 88;
			
		Delay(500000);

		/* palette[0] */
		puchBpp8 = (unsigned char *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puchBpp8++ = 0;
			
	}
	else if (iBpp == 16)
	{
		/* 让LCD输出整屏的红色 */

		/* 565: 0xf800 */

		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = 0xf800;
			
		Delay(500000);

		/* green */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = 0x7e0;

		Delay(500000);

		/* blue */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = 0x1f;

		Delay(500000);

		/* black */
		pusBpp16 = (unsigned short *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*pusBpp16++ = 0;
			
	}
	else if (iBpp == 32)
	{
		/* 让LCD输出整屏的红色 */

		/* 0xRRGGBB */

		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0xff0000;

		Delay(500000);

		/* green */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0x00ff00;

		Delay(500000);

		/* blue */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0x0000ff;

		Delay(500000);

		/* black */
		puiBpp32 = (unsigned int *)uiFbBase;
		for (x = 0; x < iXres; x++)
			for (y = 0; y < iYres; y++)
				*puiBpp32++ = 0;

	}

	Delay(1000000);
	
	/* 画线 */
	DrawLine(0, 0, iXres - 1, 0, 0xff0000);
	DrawLine(iXres - 1, 0, iXres - 1, iYres - 1, 0xffff00);
	DrawLine(0, iYres - 1, iXres - 1, iYres - 1, 0xff00aa);
	DrawLine(0, 0, 0, iYres - 1, 0xff00ef);
	DrawLine(0, 0, iXres - 1, iYres - 1, 0xff4500);
	DrawLine(iXres - 1, 0, 0, iYres - 1, 0xff0780);

	Delay(1000000);

	/* 画圆 */
	DrawCircle(iXres/2, iYres/2, iYres/4, 0xff00);

	/* 输出文字 */
	PrintFbString8x16(10, 10, "leichuan   sagacity_lc@163.com", 0xffff, 1);
}



