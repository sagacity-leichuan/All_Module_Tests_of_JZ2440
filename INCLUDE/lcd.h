
#ifndef _LCD_H
#define _LCD_H


enum {
	kNORMAL = 0,
	kINVERT = 1,
};

/* NORMAL : 正常极性
 * INVERT : 反转极性
 */
typedef struct SLcdPinsPolarity {
	int iDe;    /* normal: 高电平时可以传输数据 */
	int iPwren; /* normal: 高电平有效 */
	int iVclk;  /* normal: 在下降沿获取数据 */
	int iRgb;   /* normal: 高电平表示1 */
	int iHsync; /* normal: 高脉冲 */
	int iVsync; /* normal: 高脉冲 */
}SLcdPinsPolarity, *PSLcdPinsPolarity;

typedef struct SLcdTimeSequence {
	/* 垂直方向 */
	int iTvp; /* vysnc脉冲宽度 */
	int iTvb; /* 上边黑框, Vertical Back porch */
	int iTvf; /* 下边黑框, Vertical Front porch */

	/* 水平方向 */
	int iThp; /* hsync脉冲宽度 */
	int iThb; /* 左边黑框, Horizontal Back porch */
	int iThf; /* 右边黑框, Horizontal Front porch */

	int iVclk;
}SLcdTimeSequence, *PSLcdTimeSequence;


typedef struct SLcdParams {
	char *pchName;
	
	/* 引脚极性 */
	SLcdPinsPolarity sPinsPol;
	
	/* 时序 */
	SLcdTimeSequence sTimeSeq;
	
	/* 分辨率, bpp */
	int iXres;
	int iYres;
	int iBpp;
	
	/* framebuffer的地址 */
	unsigned int nFbBase;
}SLcdParams, *PSLcdParams;

int InitLcd(void);
void GetLcdParams(unsigned int *fb_base, int *xres, int *yres, int *bpp);
void EnableLcd(void);
void DisableLcd(void);
int RegisterLcd(PSLcdParams plcd);
void TestLCD(void);



#endif /* _LCD_H */


