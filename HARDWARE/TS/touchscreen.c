/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板触摸屏代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "touchscreen.h"
#include "interrupt.h"
#include "my_printf.h"
#include "timer.h"
#include "tslib.h"
#include "framebuffer.h"
#include "font.h"
#include "lcd.h"
#include "function.h"
#include "geometry.h"

#define ADC_INT_BIT (10)
#define TC_INT_BIT  (9)

#define INT_ADC_TC   (31)


/* ADCTSC's bits */
#define WAIT_PEN_DOWN    (0<<8)
#define WAIT_PEN_UP      (1<<8)

#define YM_ENABLE        (1<<7)
#define YM_DISABLE       (0<<7)

#define YP_ENABLE        (0<<6)
#define YP_DISABLE       (1<<6)

#define XM_ENABLE        (1<<5)
#define XM_DISABLE       (0<<5)

#define XP_ENABLE        (0<<4)
#define XP_DISABLE       (1<<4)

#define PULLUP_ENABLE    (0<<3)
#define PULLUP_DISABLE   (1<<3)

#define AUTO_PST         (1<<2)

#define WAIT_INT_MODE    (3)
#define NO_OPR_MODE      (0)

static volatile int gs_viTsTimerEnable = 0;

static volatile int gs_viTsX;
static volatile int gs_viTsY;
static volatile int gs_viTsPressure;
static volatile int gs_viTsDataValid = 0;


static int s_TestXArray[16];
static int s_TestYArray[16];

/**********************************************************************************
  * @brief       : 	进入等待触摸笔点击模式
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void EnterWaitPenDownMode(void)
{
	ADCTSC = WAIT_PEN_DOWN | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

/**********************************************************************************
  * @brief       : 	进入等待触摸笔离开模式
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void EnterWaitPenUpMode(void)
{
	ADCTSC = WAIT_PEN_UP | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

/**********************************************************************************
  * @brief       : 	进入自动测量模式
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void EnterAutoMeasureMode(void)
{
	ADCTSC = AUTO_PST | NO_OPR_MODE;
}

/**********************************************************************************
  * @brief       : 	是否进入自动测量模式
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static int isInAutoMode(void)
{
	return ADCTSC & AUTO_PST;
}

/**********************************************************************************
  * @brief       : 	触摸屏中断处理
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void IsrTc(void)
{
	if (ADCDAT0 & (1<<15))
	{
		EnterWaitPenDownMode();
		ReportTsXY(0,0,0);
	}
	else	
	{
		/* 进入"自动测量"模式 */
		EnterAutoMeasureMode();

		/* 启动ADC */
		ADCCON |= (1<<0);
	}
}

/**********************************************************************************
  * @brief       : 	使能触摸屏定时器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void EnableTsTimer(void)
{
	gs_viTsTimerEnable = 1;
}

/**********************************************************************************
  * @brief       : 	关闭触摸屏定时器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void DisableTsTimer(void)
{
	gs_viTsTimerEnable = 0;
}

/**********************************************************************************
  * @brief       : 	获取触摸屏定时器状态
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static int GetStatusOfTsTimer(void)
{
	return gs_viTsTimerEnable;
}

/**********************************************************************************
  * @brief       : 	上报触摸笔点击的信息
  * @param[in]   : 	x	触摸笔点击的x坐标
  					y	触摸笔点击的y坐标
  					pressure	触摸笔压力状态
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void ReportTsXY(int x, int y, int pressure)
{
	if (gs_viTsDataValid == 0)
	{
		gs_viTsX = x;
		gs_viTsY = y;
		gs_viTsPressure = pressure;
		gs_viTsDataValid = 1;
		//printf("ReportTsXY gs_viTsPressure = %d\n\r", gs_viTsPressure);
	}
	
}

/**********************************************************************************
  * @brief       : 	读取触摸笔点击的信息
  * @param[in]   : 	无
  * @param[out]  : 	px	存放触摸笔点击的x坐标
  					py	存放触摸笔点击的y坐标
  					ppressure	存放触摸笔压力状态
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void ReadTsRaw(int *px, int *py, int *ppressure)
{
	if(gs_viTsDataValid == 0)
	{
		*px = 0;
		*py = 0;
		*ppressure = 0;
		return;
	}
		
	
	*px = gs_viTsX;
	*py = gs_viTsY;
	*ppressure = gs_viTsPressure;

	gs_viTsDataValid = 0;

}

/**********************************************************************************
  * @brief		 :	触摸屏定时器中断处理函数
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	每10ms该函数被调用一次
***********************************************************************************/
void TouchscreenTimerIrq(void)
{
	
	/* 如果触摸屏仍被按下, 进入"自动测量模式", 启动ADC */
	if (GetStatusOfTsTimer() == 0)
		return;

	if (isInAutoMode())
		return;

	/* 只有在"等待中断模式"下才可以使用ADCDAT0'BIT 15来判断触摸笔状态 */

	if (ADCDAT0 & (1<<15)) /* 如果松开 */
	{
		//printf("touchscreen_timer_irq timer set pen down\n\r");
		DisableTsTimer();
		EnterWaitPenDownMode();
		//printf("touchscreen_timer_irq clear pressure\n\r");
		ReportTsXY(0, 0, 0);
		Delay(10);
		ReportTsXY(0, 0, 0);
		//printf("leave touchscreen_timer_irq\n\r");
		return;
	}
	else  /* 按下状态 */
	{
		/* 进入"自动测量"模式 */
		EnterAutoMeasureMode();
		
		/* 启动ADC */
		ADCCON |= (1<<0);

		//printf("continue adc measure\n\r");
	}
}

/**********************************************************************************
  * @brief       : 	ADC中断处理
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void IsrAdc(void)
{
	int x = ADCDAT0;
	int y = ADCDAT1;

	static int s_iAdcCnt = 0;
	static int s_iAdcX = 0;
	static int S_iAdcY = 0;

	/* 进入ADC中断时, TS处于"自动测量模式" */

	/* 只有在"等待中断模式"下才可以使用ADCDAT0'BIT 15来判断触摸笔状态 */

	EnterWaitPenUpMode();
	
	if (!(ADCDAT0 & (1<<15))) /* 如果仍然按下才打印 */
	{			
		/* 第1次启动ADC后:
		 *   a. 要连续启动N次, 获得N个数据, 求平均值并上报
		 *   b. 得到N次数据后, 再启动TIMER 
		 */
		s_iAdcX += (x & 0x3ff);
		S_iAdcY += (y & 0x3ff);

		s_TestXArray[s_iAdcCnt] = (x & 0x3ff);
		s_TestYArray[s_iAdcCnt] = (y & 0x3ff);
		
		s_iAdcCnt++;

		if (s_iAdcCnt == 16)
		{
			s_iAdcX >>= 4;
			S_iAdcY >>= 4;
			
			//printf("Isr_Adc adc_x = %08d,adc_y = %08d\n\r",s_iAdcX,S_iAdcY);
			ReportTsXY(s_iAdcX, S_iAdcY, 1);

			s_iAdcCnt = 0;
			s_iAdcX = 0;
			S_iAdcY = 0;
			
			/* 启动定时器以再次读取数据 */
			/* 先设置TS进入"等待中断模式" */
			EnterWaitPenUpMode();
			EnableTsTimer();
		}
		else
		{
			/* 再次启动ADC */
			/* 进入"自动测量"模式 */
			EnterAutoMeasureMode();
			
			/* 启动ADC */
			ADCCON |= (1<<0);
		}
		
	}
	else
	{
		s_iAdcCnt = 0;
		s_iAdcX = 0;
		S_iAdcY = 0;
		DisableTsTimer();
		//printf("timer disable\n\r");
		EnterWaitPenDownMode();
		ReportTsXY(0, 0, 0);
		Delay(10);
		ReportTsXY(0, 0, 0);
	}

	//enter_wait_pen_up_mode();  /* 启动ADC时不应该进入这个模式, 它会影响数据 */

}

/**********************************************************************************
  * @brief       : 	ADC与触摸屏中断处理
  * @param[in]   : 	irq	中断号
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void AdcTsIntHandle(int irq)
{
	if (SUBSRCPND & (1<<TC_INT_BIT))  /* 如果是触摸屏中断 */
		IsrTc();

	if (SUBSRCPND & (1<<ADC_INT_BIT))  /* ADC中断 */
		IsrAdc();
		
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);
}

/**********************************************************************************
  * @brief       : 	初始化ADC与触摸屏中断处理
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitAdcTsInt(void)
{
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);

	/* 注册中断处理函数 */
	RegisterIrq(31, AdcTsIntHandle);	

	/* 使能中断 */
	INTSUBMSK &= ~((1<<ADC_INT_BIT) | (1<<TC_INT_BIT));
}

/**********************************************************************************
  * @brief       : 	初始化ADC与触摸屏中断寄存器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitAdcTsReg(void)
{
	/* [15] : ECFLG,  1 = End of A/D conversion
	 * [14] : PRSCEN, 1 = A/D converter prescaler enable
	 * [13:6]: PRSCVL, adc clk = PCLK / (PRSCVL + 1)
	 * [5:3] : SEL_MUX, 000 = AIN 0
	 * [2]   : STDBM
	 * [0]   : 1 = A/D conversion starts and this bit is cleared after the startup.
	 */
	ADCCON = (1<<14) | (49<<6) | (0<<3);

	/*  按下触摸屏, 延时一会再发出TC中断
	 *  延时时间 = ADCDLY * 晶振周期 = ADCDLY * 1 / 12000000 = 5ms
	 */
	ADCDLY = 60000;	
}

/**********************************************************************************
  * @brief       : 	初始化触摸屏
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitTouchScreen(void)
{
	/* 设置触摸屏接口:寄存器 */
	InitAdcTsReg();

	/* 设置中断 */
	InitAdcTsInt();

	/* 注册定时器处理函数 */
	RegisterTimer("touchscreen", TouchscreenTimerIrq);

	/* 让触摸屏控制器进入"等待中断模式" */
	EnterWaitPenDownMode();
}

/**********************************************************************************
  * @brief       : 	触摸屏测试函数
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void TestTouchScreen(void)
{
	unsigned int iFbBase;
	int iXres, iYres, iBpp;

	int x, y, iPressure;
	int i;

	/* 获得LCD的参数: fb_base, xres, yres, bpp */
	GetLcdParams(&iFbBase, &iXres, &iYres, &iBpp);

	/* 清屏 */
	ClearScreen(0x0);

	/* 显示文字提示绘画 */
	PrintFbString16x32(176, 5, " TS TEST", 0xe3170d, 0);
	PrintFbString8x16(70, 70, "OK! To draw!", 0xffffff,0);
	DisplayReturnButton();

	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			break;
		}
		
		i = ReadTs(&x, &y, &iPressure);
		if (i == 0)
		{
			if (iPressure)
			{
				DrawLine(x-1, y-1,x+1,y+1, 0xff0000);
				DrawLine(x-1, y, x+1, y, 0xff0000);
				DrawLine(x-1, y+1, x+1, y+1, 0xff0000);
			}
		}
		
	}
}

