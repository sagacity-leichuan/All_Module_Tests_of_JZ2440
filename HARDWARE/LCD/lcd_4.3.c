/**********************************************************************************
  * @brief      : 	JZ2440v2开发板4.3寸LCD显示屏代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***********************************************************************************/

#include "lcd.h"
#include "lcd_4.3.h"

#define LCD_FB_BASE 0x33c00000     //显存基址

SLcdParams lcd_4_3_params = {
	.pchName = "lcd_4.3",
	.sPinsPol = {
		.iDe    = kNORMAL,	/* normal: 高电平时可以传输数据 */
		.iPwren = kNORMAL,    /* normal: 高电平有效 */
		.iVclk  = kNORMAL,	/* normal: 在下降沿获取数据 */
		.iRgb   = kNORMAL,	/* normal: 高电平表示1 */
		.iHsync = kINVERT,    /* normal: 高脉冲 */
		.iVsync = kINVERT, 	/* normal: 高脉冲 */
	},
	.sTimeSeq = {
		/* 垂直方向 */
		.iTvp=	10, /* vysnc脉冲宽度 */
		.iTvb=	2,  /* 上边黑框, Vertical Back porch */
		.iTvf=	2,  /* 下边黑框, Vertical Front porch */

		/* 水平方向 */
		.iThp=	41, /* hsync脉冲宽度 */
		.iThb=	2,  /* 左边黑框, Horizontal Back porch */
		.iThf=	2,  /* 右边黑框, Horizontal Front porch */

		.iVclk=	9,  /* MHz */
	},
	.iXres = 480,
	.iYres = 272,
	.iBpp  = 16,  /* 16, no 24bpp */
	.nFbBase = LCD_FB_BASE,
};

/**********************************************************************************
  * @brief       : 	向LCD数组中增加4.3寸的lcd显示屏
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void Add4Point3Lcd(void)
{
	RegisterLcd(&lcd_4_3_params);
}

