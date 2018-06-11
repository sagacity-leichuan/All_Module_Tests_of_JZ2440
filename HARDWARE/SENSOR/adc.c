/**************************************************************************
  * @brief      : JZ2440v2开发板ADC采集代码源文件
  * @version    : V0.0
  * @note       : 无
  * @history    : 无
***************************************************************************/

#include "s3c2440_soc.h"

/**************************************************************************
  * @brief       :  初始化ADC
  * @param[in]   :  channel 代表ADC的采集通道，取值为 {0,1,2,3,4,5,7}
  * @param[out]  :  无
  * @return      :  无
  * @others      :  无
***************************************************************************/
static void InitADC(int channel)
{
	/* [15] : ECFLG,  1 = End of A/D conversion
	 * [14] : PRSCEN, 1 = A/D converter prescaler enable
	 * [13:6]: PRSCVL, adc clk = PCLK / (PRSCVL + 1)
	 * [5:3] : SEL_MUX, 000 = AIN 0
	 * [2]   : STDBM
	 * [0]   : 1 = A/D conversion starts and this bit is cleared after the startup.
	 */
	ADCCON = (1<<14) | (49<<6) | (channel<<3);

	ADCDLY = 0xff;	
}

/**************************************************************************
  * @brief       :  读取ADC转换的数据
  * @param[in]   :  channel 代表ADC的采集通道，取值为 {0,1,2,3,4,5,7}
  * @param[out]  :  无
  * @return      :  ADC读取并转换完成之后的数据
  * @others      :  无
***************************************************************************/
int ReadADC(int channel)
{
	InitADC(channel);
	
	/* 启动ADC */
	ADCCON |= (1<<0);

	while (!(ADCCON & (1<<15)));  /* 等待ADC结束 */

	return ADCDAT0 & 0x3ff;
}

