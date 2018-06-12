/**********************************************************************************
  * @brief      : 	i2c控制器代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***********************************************************************************/

#include "s3c2440_soc.h"
#include "function.h"
#include "i2c_controller.h"
#include "my_printf.h"
#include "string_utils.h"
#include "s3c2440_i2c_controller.h"

#define I2C_CONTROLLER_NUM 10

/* 有一个i2c_controller数组用来存放各种不同芯片的操作结构体 */
static PSI2cController g_psI2cControllers[I2C_CONTROLLER_NUM];
static PSI2cController g_psI2cConSelected = NULL;

/**********************************************************************************
  * @brief       : 	注册I2C控制器
  * @param[in]   : 	p	待注册的I2C控制器
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void RegisterI2cController(PSI2cController p)
{
	int i;
	for (i = 0; i < I2C_CONTROLLER_NUM; i++)
	{
		if (!g_psI2cControllers[i])
		{
			g_psI2cControllers[i] = p;
			return;
		}
	}
}

/**********************************************************************************
  * @brief       : 	根据名字来选择某款I2C控制器 
  * @param[in]   : 	name	待选择的I2C控制器名字
  * @param[out]  : 	无
  * @return      : 	0	选择成功
  					-1	选择失败
  * @others      : 	无
***********************************************************************************/
static int SelectI2cController(char *name)
{
	int i;
	for (i = 0; i < I2C_CONTROLLER_NUM; i++)
	{
		if (g_psI2cControllers[i] && !strcmp(name, g_psI2cControllers[i]->pchName))
		{
			g_psI2cConSelected = g_psI2cControllers[i];
			return 0;
		}
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	实现 i2c_transfer 接口函数，用以传输I2C消息
  * @param[in]   : 	msgs	待传输的I2C消息缓冲区首地址
  					num		待传输的I2C消息的个数
  * @param[out]  : 	无
  * @return      : 	0	传输成功
  					其他	传输错误
  * @others      : 	无
***********************************************************************************/
int TransferI2c(PSI2cMsg msgs, int num)
{
	if(g_psI2cConSelected)
	{
		return g_psI2cConSelected->MasterXfer(msgs, num);
	}
	else
	{
		return -1;
	}	
}

/**********************************************************************************
  * @brief       : 	初始化I2C控制器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitI2c(void)
{
	/* 注册下面的I2C控制器 */
	AddS3c2440I2cCon();

	/* 选择某款I2C控制器 */
	SelectI2cController("s3c2440");

	/* 调用它的init函数 */
	g_psI2cConSelected->Init();
}

