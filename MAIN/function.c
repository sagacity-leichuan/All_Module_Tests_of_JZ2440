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
#include "nand_flash.h"
#include "key.h"
#include "i2c_controller.h"
#include "oled.h"
#include "spi_flash.h"
#include "spi_gpio_simulate.h"


STsParameters g_sTsParam;
SButton g_sButton[12];
SButton g_sButtonReturn;


void Delay(volatile int d)
{
	while (d--);
}

void InitButton(void)
{
	g_sButton[0].iID = TEST_LED;
	g_sButton[0].iX = 20;
	g_sButton[0].iY = 45;
	g_sButton[0].iLength = BUTTON_LENGTH;
	g_sButton[0].iHigh = BUTTON_HIGH;
	g_sButton[0].sName = "LED Test";

	g_sButton[1].iID = TEST_KEY;
	g_sButton[1].iX = 170;
	g_sButton[1].iY = 45;
	g_sButton[1].iLength = BUTTON_LENGTH;
	g_sButton[1].iHigh = BUTTON_HIGH;
	g_sButton[1].sName = "KEY Test";

	
	g_sButton[2].iID = TEST_LCD;
	g_sButton[2].iX = 320;
	g_sButton[2].iY = 45;
	g_sButton[2].iLength = BUTTON_LENGTH;
	g_sButton[2].iHigh = BUTTON_HIGH;
	g_sButton[2].sName = "LCD Test";
	

	g_sButton[3].iID = TEST_TS;
	g_sButton[3].iX = 20;
	g_sButton[3].iY = 99;
	g_sButton[3].iLength = BUTTON_LENGTH;
	g_sButton[3].iHigh = BUTTON_HIGH;
	g_sButton[3].sName = " TS Test";

	g_sButton[4].iID = TEST_I2C;
	g_sButton[4].iX = 170;
	g_sButton[4].iY = 99;
	g_sButton[4].iLength = BUTTON_LENGTH;
	g_sButton[4].iHigh = BUTTON_HIGH;
	g_sButton[4].sName = "I2C Test";

	g_sButton[5].iID = TEST_SPI;
	g_sButton[5].iX = 320;
	g_sButton[5].iY = 99;
	g_sButton[5].iLength = BUTTON_LENGTH;
	g_sButton[5].iHigh = BUTTON_HIGH;
	g_sButton[5].sName = "SPI Test";

	g_sButton[6].iID = TEST_IDR;
	g_sButton[6].iX = 20;
	g_sButton[6].iY = 153;
	g_sButton[6].iLength = BUTTON_LENGTH;
	g_sButton[6].iHigh = BUTTON_HIGH;
	g_sButton[6].sName = "IDR Test";

	g_sButton[7].iID = TEST_DHT1;
	g_sButton[7].iX = 170;
	g_sButton[7].iY = 153;
	g_sButton[7].iLength = BUTTON_LENGTH;
	g_sButton[7].iHigh = BUTTON_HIGH;
	g_sButton[7].sName = "DHT1Test";

	g_sButton[8].iID = TEST_DS18;
	g_sButton[8].iX = 320;
	g_sButton[8].iY = 153;
	g_sButton[8].iLength = BUTTON_LENGTH;
	g_sButton[8].iHigh = BUTTON_HIGH;
	g_sButton[8].sName = "DS18Test";

	g_sButton[9].iID = TEST_0038T;
	g_sButton[9].iX = 20;
	g_sButton[9].iY = 207;
	g_sButton[9].iLength = BUTTON_LENGTH;
	g_sButton[9].iHigh = BUTTON_HIGH;
	g_sButton[9].sName = "0038Test";

	g_sButton[10].iID = TEST_NOR;
	g_sButton[10].iX = 170;
	g_sButton[10].iY = 207;
	g_sButton[10].iLength = BUTTON_LENGTH;
	g_sButton[10].iHigh = BUTTON_HIGH;
	g_sButton[10].sName = "NOR Test";

	g_sButton[11].iID = TEST_NAND;
	g_sButton[11].iX = 320;
	g_sButton[11].iY = 207;
	g_sButton[11].iLength = BUTTON_LENGTH;
	g_sButton[11].iHigh = BUTTON_HIGH;
	g_sButton[11].sName = "NANDTest";

	g_sButtonReturn.iID = TEST_RETURN;
	g_sButtonReturn.iX = RETURN_BUTTON_X;
	g_sButtonReturn.iY = RETURN_BUTTON_Y;
	g_sButtonReturn.iLength = BUTTON_LENGTH;
	g_sButtonReturn.iHigh = BUTTON_HIGH;
	g_sButtonReturn.sName = " RETURN ";

}

int isClickReturn(void)
{
	int i;
	int x = 0;
	int y = 0;
	int iPressure = 0;
	int iXsum = 0;
	int iYsum = 0;
	int iPreX = 0;
	int iPreY = 0;
	int iCnt = 0;
	int iRes = 0;

	ReadTs(&x, &y, &iPressure);

	if(x == 0 && y == 0 && iPressure == 0)
	{
		return iRes;	
	}
	else
	{
		iXsum += x;
		iYsum += y;
		iPreX = x;
	   	iPreY = y;		
		iCnt++;

		for(i=0; i<5; i++)
		{
			ReadTs(&x, &y, &iPressure);
			if(x != 0 || y != 0 || iPressure != 0)
			{
				if(x<iXsum+20 && x>iXsum-20 && y<iPreY+20 && y>iPreY-20)
				{
					iXsum += x;
					iYsum += y;
					iCnt++;
				}
			}

		}
	}


	x = iXsum/iCnt;
	y = iYsum/iCnt;
	
	if( (x > g_sButtonReturn.iX) && (x < g_sButtonReturn.iX+BUTTON_LENGTH) && (y > g_sButtonReturn.iY) && (y < g_sButtonReturn.iY+BUTTON_HIGH))
	{
		iRes = 1;
		ClickedButton(g_sButtonReturn.iX,g_sButtonReturn.iY,g_sButtonReturn.sName,0xFFFFFF);
		Delay(2000);
	}

	return iRes;
}

void ClickedButton(int x,int y,char * name,unsigned int color)
{
	int i = 0;
	for(i = 0; i < 45; i++)
		DrawLine(x, y + i, x + 140, y+i, 0x03a89e);
			
	PrintFbString16x32(x+8, y+6, name, color, 0);
	return;
}

void GetTestItem(int* value)
{
	int i = 0;
	int x = 0;
	int y = 0;
	int iXsum = 0;
	int iYsum = 0;
	int iCnt = 0;
	int iPreX = 0;
	int iPreY = 0;
	int iPressure = 0;

	ReadTs(&x, &y, &iPressure);

	if(x == 0 && y == 0 && iPressure == 0)
	{
		return ;	
	}
	else
	{
		iXsum += x;
		iYsum += y;
		iPreX = x;
	   	iPreY = y;		
		iCnt++;

		for(i=0; i<5; i++)
		{
			ReadTs(&x, &y, &iPressure);
			if(x != 0 || y != 0 || iPressure != 0)
			{
				if(x<iPreX+20 && x>iPreX-20 && y<iPreY+20 && y>iPreY-20)
				{
					iXsum += x;
					iYsum += y;
					iPreX = x;
	   				iPreY = y;
					iCnt++;
				}
				
			}

		}
	}

	x = iXsum/iCnt;
	y = iYsum/iCnt;

	if( (x > g_sButton[TEST_LED].iX) && (x < g_sButton[TEST_LED].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_LED].iY) && (y < g_sButton[TEST_LED].iY+BUTTON_HIGH))
		*value = TEST_LED;
	else if( (x > g_sButton[TEST_KEY].iX) && (x < g_sButton[TEST_KEY].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_KEY].iY) && (y < g_sButton[TEST_KEY].iY+BUTTON_HIGH))
		*value = TEST_KEY;
	else if( (x > g_sButton[TEST_LCD].iX) && (x < g_sButton[TEST_LCD].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_LCD].iY) && (y < g_sButton[TEST_LCD].iY+BUTTON_HIGH))
		*value = TEST_LCD;
	else if( (x > g_sButton[TEST_TS].iX) && (x < g_sButton[TEST_TS].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_TS].iY) && (y < g_sButton[TEST_TS].iY+BUTTON_HIGH))
		*value = TEST_TS;
	else if( (x > g_sButton[TEST_I2C].iX) && (x < g_sButton[TEST_I2C].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_I2C].iY) && (y < g_sButton[TEST_I2C].iY+BUTTON_HIGH))
		*value = TEST_I2C;
	else if( (x > g_sButton[TEST_SPI].iX) && (x < g_sButton[TEST_SPI].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_SPI].iY) && (y < g_sButton[TEST_SPI].iY+BUTTON_HIGH))
		*value = TEST_SPI;
	else if( (x > g_sButton[TEST_IDR].iX) && (x < g_sButton[TEST_IDR].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_IDR].iY) && (y < g_sButton[TEST_IDR].iY+BUTTON_HIGH))
		*value = TEST_IDR;
	else if( (x > g_sButton[TEST_DHT1].iX) && (x < g_sButton[TEST_DHT1].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_DHT1].iY) && (y < g_sButton[TEST_DHT1].iY+BUTTON_HIGH))
		*value = TEST_DHT1;
	else if( (x > g_sButton[TEST_DS18].iX) && (x < g_sButton[TEST_DS18].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_DS18].iY) && (y < g_sButton[TEST_DS18].iY+BUTTON_HIGH))
		*value = TEST_DS18;
	else if( (x > g_sButton[TEST_0038T].iX) && (x < g_sButton[TEST_0038T].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_0038T].iY) && (y < g_sButton[TEST_0038T].iY+BUTTON_HIGH))
		*value = TEST_0038T;
	else if( (x > g_sButton[TEST_NOR].iX) && (x < g_sButton[TEST_NOR].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_NOR].iY) && (y < g_sButton[TEST_NOR].iY+BUTTON_HIGH))
		*value = TEST_NOR;
	else if( (x > g_sButton[TEST_NAND].iX) && (x < g_sButton[TEST_NAND].iX+BUTTON_LENGTH) && (y > g_sButton[TEST_NAND].iY) && (y < g_sButton[TEST_NAND].iY+BUTTON_HIGH))
		*value = TEST_NAND;

	if(*value != ORIGINAL)
	{
		printf(" x = %d,y = %d\n\r",x,y);
		ClickedButton(g_sButton[*value].iX,g_sButton[*value].iY,g_sButton[*value].sName,0xFFFFFF);
		Delay(20000);
	}

	DisplayTestItemIcon(g_sButton[*value].iX,g_sButton[*value].iY,g_sButton[*value].sName,0x0);

	ReadTs(&x, &y, &iPressure);
	Delay(1000);
	
	ReadTs(&x, &y, &iPressure);
	Delay(1000);
	
	return;
}

static void InitTsParam(void)
{
	g_sTsParam.iFlag = 0;
	g_sTsParam.iA_ts_x = 0;
	g_sTsParam.iA_ts_y = 0;
	g_sTsParam.iB_ts_x = 0;
	g_sTsParam.iB_ts_y = 0;
	g_sTsParam.iC_ts_x = 0;
	g_sTsParam.iC_ts_y = 0;
	g_sTsParam.iD_ts_x = 0;
	g_sTsParam.iD_ts_y = 0;
	g_sTsParam.iE_ts_x = 0;
	g_sTsParam.iE_ts_y = 0;

}

void InitCalibration(void)
{
	unsigned int iFbBase;
	int iXres, iYres, iBpp;
	int iRes;
	
	ReadNand(0x00020000, (unsigned char*)&g_sTsParam, sizeof(STsParameters));

	if(g_sTsParam.iFlag == 0x12345678)
	{
		SetTsCalibrateParams(&g_sTsParam);
		printf(" SetTsCalibrateParams\n\r");
		return;
	}
	
	/* 获得LCD的参数: fb_base, xres, yres, bpp */
	GetLcdParams(&iFbBase, &iXres, &iYres, &iBpp);

	/* 清屏 */
	ClearScreen(0x0);

	/* 显示文字提示较准 */
	PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0xffffff,0);

	while(1)
	{
		iRes = CalibrateTs(&g_sTsParam);
		if(iRes == 1)
		{
			g_sTsParam.iFlag = 0x12345678;
			NandErase(0x00020000, 128*1024);
			NandWrite(0x00020000, (unsigned char*)&g_sTsParam, sizeof(STsParameters));
			break;
		}

		PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0x0,0);
		PrintFbString8x16(70, iYres - 95, "Calibration failed, Please try again!", 0xffffff,1);
		Delay(2500000);
		PrintFbString8x16(70, iYres - 95, "Calibration failed, Please try again!", 0x0,1);
		PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0xffffff,0);
	}
	
	/* 显示文字提示绘画 */
	PrintFbString8x16(70, 70, "Touch cross to calibrate touchscreen", 0x0,0);
	PrintFbString8x16(70, iYres - 95, "OK! Calibration Triumphantly !", 0xffffff,1);
	Delay(2500000);
}

void DisplayTestItemIcon(int x,int y,char * name,unsigned int color)
{
	int i;
	
	for(i = 0; i < 45; i++)
		DrawLine(x, y + i, x + 140, y+i, 0xffffff);

	DrawRectangle(x,y,140,45,0);

	PrintFbString16x32(x+8, y+6, name, color, 0);

	return;
}

void DisplayReturnButton(void)
{
	int i;
	
	for(i = 0; i < 45; i++)
		DrawLine(g_sButtonReturn.iX, g_sButtonReturn.iY + i, g_sButtonReturn.iX + 140, g_sButtonReturn.iY+i, 0x03a89e);

	DrawRectangle(g_sButtonReturn.iX,g_sButtonReturn.iY,140,45,0);

	PrintFbString16x32(g_sButtonReturn.iX+8, g_sButtonReturn.iY+6, g_sButtonReturn.sName, 0xffffff, 0);

	return;
}

void BootInit(void)
{
	InitTsParam();
	InitButton();
	InitLed();
	InitKey();
	InitNand();
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

	Delay(3500000);

}


void MainPage(void)
{
	unsigned int uiFbBase;
	int iXres, iYres, iBpp;
	int x, y;
	int i;
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


	PrintFbString16x32(176, 5, "MainPage", 0xffffff, 0);

	for(i = 0; i < 12; i++)
	{
		DisplayTestItemIcon(g_sButton[i].iX,g_sButton[i].iY,g_sButton[i].sName,iRgb);
	}
	
}

void TestSPI(void)
{
    unsigned int mid, pid;
    char str[200];
#ifdef SPIGPIO
    InitSPIGPIO();
#else
	InitSPIS3c2440Controller();
#endif
    
    InitOLED();
    PrintOLED(0,0,"leichuan, sagacity_lc@163.com");

    ReadSPIFlashID(&mid, &pid);
    printf("SPI Flash : MID = 0x%02x, PID = 0x%02x\n\r", mid, pid);
    
    InitSPIFlash();
    
    EraseSPIFlashSector(4096);
    ProgramSPIFlash(4096, "leichuan", 8);
    ReadSPIFlash(4096, str, 8);
    printf("SPI Flash read from 4096: %s\n\r", str);
    PrintOLED(6,0,str);

}


