/*********************************************************************************************
  * @brief      : 	JZ2440v2开发板LCD控制器相关功能函数代码头文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
**********************************************************************************************/


#ifndef _LCD_CONTROLLER_H
#define _LCD_CONTROLLER_H

#include "lcd.h"

typedef struct SLcdController {
	char *pchName;
	void (*Init)(PSLcdParams plcdparams);
	void (*Enable)(void);
	void (*Disable)(void);
	void (*InitPalette)(void);
}SLcdController, *PSLcdController;

/**********************************************************************************
  * @brief       : 	添加S3c2440 LCD控制器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void AddLcdContollerS3c2440(void);

/**********************************************************************************
  * @brief       : 	根据指定的LCD参数来初始化当前的LCD控制器
  * @param[in]   : 	plcdparams		LCD参数结构体指针
  * @param[out]  : 	无
  * @return      : 	0	成功
  					-1	失败
  * @others      : 	无
***********************************************************************************/
int InitLcdController(PSLcdParams plcdparams);

/**********************************************************************************
  * @brief       : 	注册LCD控制器
  * @param[in]   : 	plcdcon 	待注册的LCD控制器结构体指针
  * @param[out]  : 	无
  * @return      : 	i	注册成功，i表示该LCD控制器在LCD控制器数组中的索引
  					-1	注册失败
  * @others      : 	无
***********************************************************************************/
int RegisterLcdController(PSLcdController plcdcon);

/**********************************************************************************
  * @brief       : 	使能当前的LCD控制器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void EnableLcdController(void);

/**********************************************************************************
  * @brief       : 	关闭当前的LCD控制器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void DisableLcdController(void);

/**********************************************************************************
  * @brief       : 	根据名字选择指定的LCD控制器
  * @param[in]   : 	name 	待注册的LCD控制器的名字
  * @param[out]  : 	无
  * @return      : 	i	选择成功，i表示该选择的LCD控制器在LCD控制器数组中的索引
  					-1	选择失败
  * @others      : 	无
***********************************************************************************/
int SelectLcdController(char *name);

#endif /* _LCD_CONTROLLER_H */

