/**************************************************************************
  * @brief      : 	JZ2440v2开发板红外接收传感器用环形缓冲区代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***************************************************************************/

#include "irda_rawdata.h"
#include "timer.h"

#define NEXT_PLACE(i) ((i+1)&0x3FF)

static SIrdaRawdataEvent g_sEvents[1024];     //环形缓冲区
static int g_iR = 0;       	//环形缓冲区读位置
static int g_iW = 0;		//环形缓冲区写位置

/**************************************************************************
  * @brief       : 	判断环形缓冲区是否为空
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      :	0   环形缓冲区不为空
  				  	非 0  环形缓冲区为空
  * @others      : 	无
***************************************************************************/
static int IsIrEventBufEmpty(void)
{
	return g_iR == g_iW;
}

/**************************************************************************
  * @brief       : 	判断环形缓冲区是否为满
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	0   环形缓冲区不为满
  				  	非 0  环形缓冲区为满
  * @others      : 	无
***************************************************************************/
static int IsIrEventBufFull(void)
{
	return NEXT_PLACE(g_iW) == g_iR;
}

/**************************************************************************
  * @brief       : 	将PSIrdaRawdataEvent数据放入环形缓冲区
  * @param[in]   : 	PSIrdaRawdataEvent数据
  * @param[out]  : 	无
  * @return      : 	0  PSIrdaRawdataEvent数据放入环形缓冲区成功
  				  	-1  PSIrdaRawdataEvent数据放入环形缓冲区失败
  * @others      : 	无
***************************************************************************/
int PutIrEvent(PSIrdaRawdataEvent pd)
{
	if (IsIrEventBufFull())
		return -1;
	g_sEvents[g_iW] = *pd;
	g_iW = NEXT_PLACE(g_iW);
	return 0;
}

/**************************************************************************
  * @brief       : 	从环形缓冲区读出一个PSIrdaRawdataEvent数据
  * @param[in]   : 	无
  * @param[out]  : 	pd	PSIrdaRawdataEvent数据
  * @return      :	 0  从环形缓冲区读出PSIrdaRawdataEvent数据成功
  				  	-1  从环形缓冲区读出PSIrdaRawdataEvent数据失败
  * @others      : 	无
***************************************************************************/
int GetIrEvent(PSIrdaRawdataEvent pd)
{
	if (IsIrEventBufEmpty())
		return -1;
	*pd = g_sEvents[g_iR];
	g_iR = NEXT_PLACE(g_iR);
	return 0;
}

/**************************************************************************
  * @brief       : 	带超时时间的从环形缓冲区读出一个PSIrdaRawdataEvent数据
  * @param[in]   : 	timeout_us	超时时间
  * @param[out]  : 	pd	PSIrdaRawdataEvent数据
  * @return      : 	0  从环形缓冲区读出PSIrdaRawdataEvent数据成功
  				  	-1  从环形缓冲区读出PSIrdaRawdataEvent数据失败
  * @others      : 	无
***************************************************************************/
int GetIrEventTimeout(PSIrdaRawdataEvent pd, int timeout_us)
{
	while (timeout_us--)
	{
		if (!IsIrEventBufEmpty())
		{
			*pd = g_sEvents[g_iR];
			g_iR = NEXT_PLACE(g_iR);
			
			return 0;
		}
		uDelay(1);
	}
	return -1;
}

