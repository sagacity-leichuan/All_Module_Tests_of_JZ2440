/**************************************************************************
  * @brief      : 	JZ2440v2开发板红外接收传感器用环形缓冲区代码头文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***************************************************************************/

#ifndef _CIRCLE_BUFFER_H
#define _CIRCLE_BUFFER_H

/**************************************************************************
  * @brief       : 	将PSIrdaRawdataEvent数据放入环形缓冲区
  * @param[in]   : 	PSIrdaRawdataEvent数据
  * @param[out]  : 	无
  * @return      : 	0  PSIrdaRawdataEvent数据放入环形缓冲区成功
  				  	-1  PSIrdaRawdataEvent数据放入环形缓冲区失败
  * @others      : 	无
***************************************************************************/
int PutIrEvent(PSIrdaRawdataEvent pd);

/**************************************************************************
  * @brief       : 	从环形缓冲区读出一个PSIrdaRawdataEvent数据
  * @param[in]   : 	无
  * @param[out]  : 	pd	PSIrdaRawdataEvent数据
  * @return      : 	0  从环形缓冲区读出PSIrdaRawdataEvent数据成功
  				  	-1  从环形缓冲区读出PSIrdaRawdataEvent数据失败
  * @others      : 	无
***************************************************************************/
int GetIrEvent(PSIrdaRawdataEvent pd);

/**************************************************************************
  * @brief       : 	带超时时间的从环形缓冲区读出一个PSIrdaRawdataEvent数据
  * @param[in]   : 	timeout_us	超时时间
  * @param[out]  : 	pd	PSIrdaRawdataEvent数据
  * @return      : 	0  从环形缓冲区读出PSIrdaRawdataEvent数据成功
  				  	-1  从环形缓冲区读出PSIrdaRawdataEvent数据失败
  * @others      : 	无
***************************************************************************/
int GetIrEventTimeout(PSIrdaRawdataEvent pd, int timeout_us);

#endif

