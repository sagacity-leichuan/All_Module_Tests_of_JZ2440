/*******************************************************************************************
  * @brief      : 	JZ2440v2开发板红外接收传感器nec协议实现代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
********************************************************************************************/

#include "irda_rawdata.h"
#include "circle_buffer.h"
#include "timer.h"
#include "my_printf.h"
#include "nand_flash.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"
#include "string_utils.h"

/*
 * 从环型缓冲区中获得脉冲数据,
 * 解析得出address, data
 */

#define DURATION_BASE  563
#define DURATION_DELTA (DURATION_BASE/2)

#define DURATION_HEAD_LOW    (16*DURATION_BASE)
#define DURATION_HEAD_HIGH   (8*DURATION_BASE)
#define DURATION_REPEAT_HIGH (4*DURATION_BASE)
#define DURATION_DATA_LOW    (1*DURATION_BASE)
#define DURATION_DATA1_HIGH  (3*DURATION_BASE)
#define DURATION_DATA0_HIGH  (1*DURATION_BASE)
#define DURATION_END_LOW     (1*DURATION_BASE)

/**************************************************************************************************
  * @brief       : 	把接收的红外原始数据根据相应的NEC解码规则得到1或0
  * @param[in]   : 	duration	接收的红外原始的边缘数据
  					us		nec协议中的不同的时序值
  * @param[out]  : 	无
  * @return      : 	1	表示得到一位1数据
  					0	标识得到一位0数据
  * @others      : 	无
***************************************************************************************************/
static int DurationInMargin(int duration, int us)
{
	if ((duration > (us - DURATION_DELTA)) && (duration < us + DURATION_DELTA))
		return 1;
	else
		return 0;
}

/**************************************************************************************************
  * @brief		 :	得到nec协议的红外数据
  * @param[in]	 :	无
  * @param[out]  :	address	存放读取的红外地址
  					data	存放读取的红外码值
  * @return 	 :	1	得到重复码
					0	得到数据
					-1	失败
  * @others 	 :	无
***************************************************************************************************/
static int ReadIrdaNec(int *address, int *data)
{
	SIrdaRawdataEvent event;	
	int i;
	unsigned int val = 0;
	unsigned char byte[4];

	while (1)
	{

		if(isClickReturn())
		{
			return 2;
		}
		
		if (0 == GetIrEvent(&event))
		{
			/* 解析数据 */
			/* 1. 判断是否为9MS的低脉冲 */
			if (event.iPol == 0 && \
				DurationInMargin(event.iDuration, DURATION_HEAD_LOW))
			{
				/* 进行后续判断 */
				/* 2. 读下一个高脉冲数据 */
				if (0 == GetIrEventTimeout(&event, 10000))
				{
					/* 3. 判断它是否4.5ms的高脉冲
					 *    或者 2.25ms的高脉冲
					 */
					if (event.iPol == 1 && \
						DurationInMargin(event.iDuration, DURATION_HEAD_HIGH))
					{
						/* 4.5ms的高脉冲 */
						/* 4. 重复解析32位数据 */
						for (i = 0; i < 32; i++)
						{
							/* 5. 读0.56ms的低脉冲 */
							if (0 == GetIrEventTimeout(&event, 10000))
							{
								if (event.iPol == 0 && \
									DurationInMargin(event.iDuration, DURATION_DATA_LOW))
								{
									/* 6. 读下一个数据, 判断它是 0.56ms/1.68ms的高脉冲 */
									if (0 == GetIrEventTimeout(&event, 10000))
									{
										if (event.iPol == 1 && \
											DurationInMargin(event.iDuration, DURATION_DATA1_HIGH))
										{
											/* 得到了bit 1 */
											val |= (1<<i);
										}
										else if (event.iPol == 1 && \
												DurationInMargin(event.iDuration, DURATION_DATA0_HIGH))
										{
											/* 得到了bit 0 */
										}
										else
										{
											printf("%s %d\n\r", __FUNCTION__, __LINE__);
											return -1;
										}
									}
									else
									{
										printf("%s %d\n\r", __FUNCTION__, __LINE__);
										return -1;
									}
								}
								else
								{
									printf("%s %d\n\r", __FUNCTION__, __LINE__);
									return -1;
								}
							}
							else
							{
								printf("%s %d\n\r", __FUNCTION__, __LINE__);
								return -1;
							}
						}

						/* 7. 得到了32位数据, 判断数据是否正确 */
						byte[0] = val & 0xff;
						byte[1] = (val>>8) & 0xff;
						byte[2] = (val>>16) & 0xff;
						byte[3] = (val>>24) & 0xff;

						//printf("get data: %x %x %x %x\n\r", byte[0], byte[1], byte[2], byte[3]);
						byte[1] = ~byte[1];
						byte[3] = ~byte[3];
						
						if (byte[0] != byte[1])
						{
							/* 有些遥控器不完全遵守NEC规范 */
							//printf("%s %d\n\r", __FUNCTION__, __LINE__);
							//return -1;
						}
						if (byte[2] != byte[3])
						{
							printf("%s %d\n\r", __FUNCTION__, __LINE__);
							return -1;
						}
						*address = byte[0];
						*data    = byte[2];
						return 0;
						
					}
					else if (event.iPol == 1 && \
						DurationInMargin(event.iDuration, DURATION_REPEAT_HIGH))
					{
						/* 2.25ms的高脉冲 */
						return 1;  /* 重复码 */
					}
					else
					{
						printf("%s %d\n\r", __FUNCTION__, __LINE__);
						return -1;  /* 错误 */
					}
				}
			}
			else
			{
				//printf("%s %d\n\r", __FUNCTION__, __LINE__);
				return -1; /* 有效数据未开始 */
			}
		}
	}
}

/**************************************************************************************************
  * @brief		 :	nec协议的红外接收测试
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
***************************************************************************************************/
void TestIrdaNec(void)
{
	int address = 0;
	int data = 0;
	int ret;
	int count = 0;
	char chBuffer[10];
	
	InitIrda();

	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(140, 5, "IrdaNec TEST", 0xe3170d, 0);

	PrintFbString8x16(40, 120, "Please ensure that the corresponding module is properly connected.", 0x4169e1, 1);
	mDelay(5000);
	PrintFbString8x16(40, 120, "Please ensure that the corresponding module is properly connected.", 0xffffff, 1);

	DisplayReturnButton();

	while (1)
	{

		if(isClickReturn())
		{
			MainPage();
			break;
		}
		
		ret = ReadIrdaNec(&address, &data);
		if (ret == 0)
		{
			count = 1;
			printf("irda_nec_read: address = 0x%x, data = 0x%x\n\r", address, data);

			PrintFbString8x16(50, 70, "irda_nec_read: ", 0x4169e1, 0);
			Convert(chBuffer, address, 16);
			PrintFbString8x16(170,95, "address = 0x", 0x4169e1, 0);
			PrintFbString8x16(266,95,chBuffer, 0x4169e1, 0);			
			Convert(chBuffer, data, 16);
			PrintFbString8x16(170, 120, "data = 0x", 0x4169e1, 0);
			PrintFbString8x16(242, 120,chBuffer, 0x4169e1, 0);
			Delay(1000000);
			PrintFbString8x16(50, 70, "irda_nec_read: ", 0xffffff, 0);
			Convert(chBuffer, address, 16);
			PrintFbString8x16(170,95, "address = 0x", 0xffffff, 0);
			PrintFbString8x16(266,95,chBuffer, 0xffffff, 0);			
			Convert(chBuffer, data, 16);
			PrintFbString8x16(170, 120, "data = 0x", 0xffffff, 0);
			PrintFbString8x16(242, 120,chBuffer, 0xffffff, 0);
			
		}
		else if (ret == 1)
		{
			count++;
			printf("This is a repeat code\n\r");
			Convert(chBuffer, count, 10);
			if(sizeof(chBuffer)==1)
			{
				chBuffer[1] = chBuffer[0];
				chBuffer[0] = '0';
				chBuffer[2] = '\0';
			}
			PrintFbString8x16(142, 120, chBuffer, 0x4169e1, 1);
			PrintFbString8x16(158, 120, "th iteration of 0x", 0x4169e1, 1);
			Convert(chBuffer, data, 16);
			PrintFbString8x16(302, 120, chBuffer, 0x4169e1, 1);
			PrintFbString8x16(318, 120, " code.", 0x4169e1, 1);

			Delay(400000);
			
			Convert(chBuffer, count, 10);
			if(sizeof(chBuffer)==1)
			{
				chBuffer[1] = chBuffer[0];
				chBuffer[0] = '0';
				chBuffer[2] = '\0';
			}
			PrintFbString8x16(142, 120, chBuffer, 0xffffff, 1);
			PrintFbString8x16(158, 120, "th iteration of 0x", 0xffffff, 1);
			Convert(chBuffer, data, 16);
			PrintFbString8x16(302, 120, chBuffer, 0xffffff, 1);
			PrintFbString8x16(318, 120, " code.", 0xffffff, 1);
		}
		else if (ret == 2)
		{
			MainPage();
			return;
		}
	}
}

