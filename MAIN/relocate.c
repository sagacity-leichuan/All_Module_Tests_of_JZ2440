/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板裸板程序重定位代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "nand_flash.h"

/**********************************************************************************
  * @brief       : 	判断裸板程序是从nand启动还是nor启动
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	0	nand启动
  					1	nor启动
  * @others      : 	无
***********************************************************************************/
int isBootFromNorFlash(void)
{
	volatile int *p = (volatile int *)0;
	int val;

	val = *p;
	*p = 0x12345678;
	if (*p == 0x12345678)
	{
		/* 写成功, 是nand启动 */
		*p = val;
		return 0;
	}
	else
	{
		/* NOR不能像内存一样写 */
		return 1;
	}
}

/*************************************************************************************************
  * @brief       : 	把裸板程序代码复制到程序的链接地址（SDRAM）处
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
**************************************************************************************************/
void Copy2Sdram(void)
{
	/* 要从lds文件中获得 __code_start, __bss_start
	 * 然后从0地址把数据复制到__code_start
	 */

	extern int __code_start, __bss_start;

	volatile unsigned int *dest = (volatile unsigned int *)&__code_start;
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *src = (volatile unsigned int *)0;
	unsigned int len = (unsigned int)(&__bss_start) - (unsigned int)(&__code_start);

	if (isBootFromNorFlash())
	{
		while (dest < end)
		{
			*dest++ = *src++;
		}
	}
	else
	{
		InitNand();
		ReadNand((unsigned int)src, (unsigned char *)dest, len);
	}

}

/**********************************************************************************
  * @brief       : 	清零裸板程序的bss段
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void CleanBss(void)
{
	/* 要从lds文件中获得 __bss_start, _end
	 */
	extern int _end, __bss_start;

	volatile unsigned int *start = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *end = (volatile unsigned int *)&_end;


	while (start <= end)
	{
		*start++ = 0;
	}
}

/**********************************************************************************
  * @brief       : 	初始化SDRAM，用以重定位代码
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitSdram(void)
{
	BWSCON = 0x22000000;

	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;

	REFRESH  = 0x8404f5;

	BANKSIZE = 0xb1;

	MRSRB6   = 0x20;
	MRSRB7   = 0x20;
}

