
#include "lcd.h"
#include "framebuffer.h"

/* 实现画点 */

/* 获得LCD参数 */
static unsigned int sg_uiFbBase;
static int sg_iXres, sg_iYres, sg_Bpp;


void GetLcdParamsforFb(void)
{
	GetLcdParams(&sg_uiFbBase, &sg_iXres, &sg_iYres, &sg_Bpp);
}

/* rgb: 0x00RRGGBB */
unsigned short Convert32BppTo16Bpp(unsigned int rgb)
{
	int r = (rgb >> 16)& 0xff;
	int g = (rgb >> 8) & 0xff;
	int b = rgb & 0xff;

	/* rgb565 */
	r = r >> 3;
	g = g >> 2;
	b = b >> 3;

	return ((r<<11) | (g<<5) | (b));
}


/* color : 32bit, 0x00RRGGBB
 *
 */
void PutFbPixel(int x, int y, unsigned int color)
{
	unsigned char  *pchBpp8;  /* 8bpp */
	unsigned short *pusBpp16;  /* 16bpp */
	unsigned int   *puiBpp32; /* 32bpp */

	unsigned int pixel_base = sg_uiFbBase + (sg_iXres * sg_Bpp / 8) * y + x * sg_Bpp / 8;

	switch (sg_Bpp)
	{
		case 8:
			pchBpp8 = (unsigned char *) pixel_base;
			*pchBpp8 = color;
			break;
		case 16:
			pusBpp16 = (unsigned short *) pixel_base;
			*pusBpp16 = Convert32BppTo16Bpp(color);
			break;
		case 32:
			puiBpp32 = (unsigned int *) pixel_base;
			*puiBpp32 = color;
			break;
	}
}

void ClearScreen(unsigned int color)
{
	int x, y;
	unsigned char  *pchBpp8;  /* 8bpp */
	unsigned short *pusBpp16;  /* 16bpp */
	unsigned int   *puiBpp32; /* 32bpp */


	/* 往framebuffer中写数据 */
	if (sg_Bpp == 8)
	{
		/* bpp: palette[color] */

		pchBpp8 = (unsigned char *)sg_uiFbBase;
		for (x = 0; x < sg_iXres; x++)
			for (y = 0; y < sg_iYres; y++)
				*pchBpp8++ = color;
	}
	else if (sg_Bpp == 16)
	{
		/* 让LCD输出整屏的红色 */

		/* 565: 0xf800 */

		pusBpp16 = (unsigned short *)sg_uiFbBase;
		for (x = 0; x < sg_iXres; x++)
			for (y = 0; y < sg_iYres; y++)
				*pusBpp16++ = Convert32BppTo16Bpp(color);
			
	}
	else if (sg_Bpp == 32)
	{
		puiBpp32 = (unsigned int *)sg_uiFbBase;
		for (x = 0; x < sg_iXres; x++)
			for (y = 0; y < sg_iYres; y++)
				*puiBpp32++ = color;

	}
}

