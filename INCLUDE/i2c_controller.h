/**********************************************************************************
  * @brief      : 	i2c控制器代码头文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***********************************************************************************/

#ifndef _I2C_CONTROLLER_H
#define _I2C_CONTROLLER_H

typedef struct SI2cMsg {
	unsigned int iAddr;  /* 7bits */
	int iFlags;  /* 0 - write, 1 - read */
	int iLen;
	int iCntTransferred;
	int iErr;
	unsigned char *pchBuf;
}SI2cMsg, *PSI2cMsg;

typedef struct SI2cController {
	int (*Init)(void);
	int (*MasterXfer)(PSI2cMsg msgs, int num);
	char *pchName;
}SI2cController, *PSI2cController;

/**********************************************************************************
  * @brief       : 	初始化I2C控制器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitI2c(void);

/**********************************************************************************
  * @brief       : 	注册I2C控制器
  * @param[in]   : 	p	待注册的I2C控制器
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void RegisterI2cController(PSI2cController p);

/**********************************************************************************
  * @brief       : 	实现 i2c_transfer 接口函数，用以传输I2C消息
  * @param[in]   : 	msgs	待传输的I2C消息缓冲区首地址
  					num		待传输的I2C消息的个数
  * @param[out]  : 	无
  * @return      : 	0	传输成功
  					err	传输错误
  * @others      : 	无
***********************************************************************************/
int TransferI2c(PSI2cMsg msgs, int num);

#endif /* _I2C_CONTROLLER_H */

