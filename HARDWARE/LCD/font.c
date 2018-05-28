#include "lcd.h"
#include "framebuffer.h"
#include "font.h"
#include "function.h"




extern const unsigned char fontdata_8x16[];
extern const unsigned char asc2_3216[95][128];

/* 获得LCD参数 */
static unsigned int sg_uiFbBase;
static int sg_iXres, sg_iYres, sg_Bpp;

void InitFont(void)
{
	GetLcdParams(&sg_uiFbBase, &sg_iXres, &sg_iYres, &sg_Bpp);
}

/* 根据字母的点阵在LCD上描画文字 */

void PrintFbChar8x16(int x, int y, char c, unsigned int color)
{
	int i, j;
	
	/* 根据c的ascii码在fontdata_8x16中得到点阵数据 */
	unsigned char *pchDots = (unsigned char *)&fontdata_8x16[c * 16];

	unsigned char uchData;
	int iBit;

	/* 根据点阵来设置对应象素的颜色 */
	for (j = y; j < y+16; j++)
	{
		uchData = *pchDots++;
		iBit = 7;
		for (i = x; i < x+8; i++)
		{
			/* 根据点阵的某位决定是否描颜色 */
			if (uchData & (1<<iBit))
				PutFbPixel(i, j, color);
			iBit--;
		}
	}
}

void PrintFbChar16x32(int x, int y, char c, unsigned int color)
{
	int t,t1;
	unsigned char uchTemp;
	unsigned char ucSize;
	unsigned int uiYstart = y;
	ucSize=(32/8+((32%8)?1:0))*(32/2);		//得到字体一个字符对应点阵集所占的字节数	

	/* 根据点阵来设置对应象素的颜色 */
	c=c-' ';
	for (t = 0; t < ucSize; t++)
	{
		uchTemp = asc2_3216[(int)c][t];

		for(t1=0;t1<8;t1++)
		{			    
			if(uchTemp&0x80)
				PutFbPixel(x,y,color);
				
			uchTemp<<=1;
			y++;
			if(y>=sg_iYres)
				return;		//超区域了
			if((y-uiYstart)== 32)
			{
				y=uiYstart;
				x++;
				if(x>=sg_iXres)
					return;	//超区域了
				break;
			}
		} 
	}
}


/* "abc\n\r123" */
void PrintFbString8x16(int x, int y, char* str, unsigned int color,int isDefer)
{
	int i = 0;
	
	while (str[i])
	{
		if (str[i] == '\n')
			y = y+16;
		else if (str[i] == '\r')
			x = 0;
		else
		{
			PrintFbChar8x16(x, y, str[i], color);
			if(isDefer)
				Delay(8000);
			x = x+8;
			if (x >= sg_iXres) /* 换行 */
			{
				x = 0;
				y = y+16;
			}
		}
		i++;
	}
}

void PrintFbString16x32(int x, int y, char* str, unsigned int color,int isDefer)
{
	int i = 0;
	
	while (str[i])
	{
		if (str[i] == '\n')
			y = y+16;
		else if (str[i] == '\r')
			x = 0;
		else
		{
			PrintFbChar16x32(x, y, str[i], color);
			if(isDefer)
				Delay(8000);
			x = x+16;
			if (x >= sg_iXres) /* 换行 */
			{
				x = 0;
				y = y+32;
			}
		}
		i++;
	}
}

