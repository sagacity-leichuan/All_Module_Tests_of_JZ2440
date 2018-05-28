
#include "lcd_controller.h"
#include "s3c2440_lcd_controller.h"
#include "string_utils.h"


#define LCD_CONTROLLER_NUM 10

static PSLcdController g_psArrayLcdController[LCD_CONTROLLER_NUM];
static PSLcdController g_pSLcdControllerSelected;

int RegisterLcdController(PSLcdController plcdcon)
{
	int i;
	for (i = 0; i < LCD_CONTROLLER_NUM; i++)
	{
		if (!g_psArrayLcdController[i])
		{
			g_psArrayLcdController[i] = plcdcon;
			return i;
		}
	}
	return -1;		
}

int SelectLcdController(char *name)
{
	int i;
	for (i = 0; i < LCD_CONTROLLER_NUM; i++)
	{
		if (g_psArrayLcdController[i] && !strcmp(g_psArrayLcdController[i]->pchName, name))
		{
			g_pSLcdControllerSelected = g_psArrayLcdController[i];
			return i;
		}
	}
	return -1;		
}


/* 向上: 接收不同LCD的参数
 * 向下: 使用这些参数设置对应的LCD控制器
 */

int InitLcdController(PSLcdParams plcdparams)
{
	/* 调用所选择的LCD控制器的初始化函数 */
	if (g_pSLcdControllerSelected)
	{
		g_pSLcdControllerSelected->Init(plcdparams);
		return 0;
	}
	return -1;
}

void EnableLcdController(void)
{
	if (g_pSLcdControllerSelected)
	{
		g_pSLcdControllerSelected->Enable();
	}
}

void DisableLcdController(void)
{
	if (g_pSLcdControllerSelected)
	{
		g_pSLcdControllerSelected->Disable();
	}
}


void AddLcdContollerS3c2440(void)
{
	AddS3c2440LcdContoller();
}


