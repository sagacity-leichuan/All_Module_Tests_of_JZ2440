/*********************************************************************************************
  * @brief      : 	JZ2440v2开发板LCD控制器相关功能函数代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
**********************************************************************************************/

#include "lcd_controller.h"
#include "s3c2440_lcd_controller.h"
#include "string_utils.h"


#define LCD_CONTROLLER_NUM 10     //lcd控制器总个数

static PSLcdController g_psArrayLcdController[LCD_CONTROLLER_NUM]; //lcd控制器数组 
static PSLcdController g_pSLcdControllerSelected;	//当前被选中的lcd控制器

/**********************************************************************************
  * @brief       : 	注册LCD控制器
  * @param[in]   : 	plcdcon 	待注册的LCD控制器结构体指针
  * @param[out]  : 	无
  * @return      : 	i	注册成功，i表示该LCD控制器在LCD控制器数组中的索引
  					-1	注册失败
  * @others      : 	无
***********************************************************************************/
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

/**********************************************************************************
  * @brief       : 	根据名字选择指定的LCD控制器
  * @param[in]   : 	name 	待注册的LCD控制器的名字
  * @param[out]  : 	无
  * @return      : 	i	选择成功，i表示该选择的LCD控制器在LCD控制器数组中的索引
  					-1	选择失败
  * @others      : 	无
***********************************************************************************/
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


/**********************************************************************************
  * @brief       : 	根据指定的LCD参数来初始化当前的LCD控制器
  * @param[in]   : 	plcdparams		LCD参数结构体指针
  * @param[out]  : 	无
  * @return      : 	0	成功
  					-1	失败
  * @others      : 	无
***********************************************************************************/
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

/**********************************************************************************
  * @brief       : 	使能当前的LCD控制器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void EnableLcdController(void)
{
	if (g_pSLcdControllerSelected)
	{
		g_pSLcdControllerSelected->Enable();
	}
}

/**********************************************************************************
  * @brief       : 	关闭当前的LCD控制器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void DisableLcdController(void)
{
	if (g_pSLcdControllerSelected)
	{
		g_pSLcdControllerSelected->Disable();
	}
}

/**********************************************************************************
  * @brief       : 	添加S3c2440 LCD控制器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void AddLcdContollerS3c2440(void)
{
	AddS3c2440LcdContoller();
}

