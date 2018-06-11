/**********************************************************************************
  * @brief      : 	JZ2440v2开发板温湿度dht11传感器代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***********************************************************************************/

#include "s3c2440_soc.h"
#include "timer.h"
#include "my_printf.h"
#include "nand_flash.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"
#include "string_utils.h"

/********************************NOTE**************************************************
使用GPG5作用dht11的DATA引脚 

控制GPIO读取DHT11的数据 
1. 主机发出至少18MS的低脉冲: start信号
2. start信号变为高, 20-40us之后, dht11会拉低总线维持80us
	然后拉高80us: 回应信号
3. 之后就是数据, 逐位发送
	bit0 : 50us低脉冲, 26-28us高脉冲
	bit1 : 50us低脉冲, 70us高脉冲
4. 数据有40bit: 8bit湿度整数数据+8bit湿度小数数据
				 +8bit温度整数数据+8bit温度小数数据
				 +8bit校验和
***********************************************************************************/


/**********************************************************************************
  * @brief       : 	配置GPIO作为输出引脚为Dht11输入数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void CfgGPIOAsOutputForDht11(void)
{
	GPGCON &= ~(3<<10);
	GPGCON |= (1<<10);
}

/**********************************************************************************
  * @brief       : 	配置GPIO作为输入引脚用以接受Dht11的数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void CfgGPIOAsInputForDht11(void)
{
	GPGCON &= ~(3<<10);
}

/**********************************************************************************
  * @brief       : 	使用特定的值设置Dht11
  * @param[in]   : 	val	需要设置的数据
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SetDht11Data(int val)
{
	if (val)
		GPGDAT |= (1<<5);
	else
		GPGDAT &= ~(1<<5);
}

/**********************************************************************************
  * @brief       : 	得到Dht11的一位数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	1或0   代表一个位的值
  * @others      : 	无
***********************************************************************************/
static int GetDht11Data(void)
{
	if (GPGDAT & (1<<5))
		return 1;
	else
		return 0;
}

/**********************************************************************************
  * @brief       :	初始化Dht11
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void InitDht11(void)
{
	CfgGPIOAsOutputForDht11();
	SetDht11Data(1);
	mDelay(2000);
}

/**********************************************************************************
  * @brief       : 	启动Dht11
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void StartDht11(void)
{
	SetDht11Data(0);
	mDelay(20);
	CfgGPIOAsInputForDht11();
}

/**********************************************************************************
  * @brief       : 	等待Dht11的回应
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	0	收到Dht11的回应
  					1	未收到Dht11的回应
  * @others      : 	无
***********************************************************************************/
static int WaitDht11Ack(void)
{
	uDelay(60);
	return GetDht11Data();
}

/**********************************************************************************
  * @brief       : 	对于特定的值而等待Dht11的回应
  * @param[in]   : 	val	需要等待的值
  					timeout_us	超时时间(单位为us)
  * @param[out]  : 	无
  * @return      : 	0	收到Dht11的回应
  					1	未收到Dht11的回应
  * @others      : 	无
***********************************************************************************/
static int WaitDht11ForVal(int val, int timeout_us)
{
	while (timeout_us--)
	{
		if (GetDht11Data() == val)
			return 0; /* ok */
		uDelay(1);
	}
	return -1; /* err */
}

/**********************************************************************************
  * @brief       : 	得到Dht11的一个字节的数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	data	一个字节的Dht11数据
  					-1		获取一个字节的Dht11数据失败
  * @others      : 	无
***********************************************************************************/
static int RecvDht11Byte(void)
{
	int i;
	int data = 0;
	
	for (i = 0; i < 8; i++)
	{
		if (WaitDht11ForVal(1, 1000))
		{
			printf("dht11 wait for high data err!\n\r");
			return -1;
		}
		
		uDelay(40);
		data <<= 1;
		
		if (GetDht11Data() == 1)
			data |= 1;
		
		if (WaitDht11ForVal(0, 1000))
		{
			printf("dht11 wait for low data err!\n\r");
			return -1;
		}
	}	
	return data;
}

/**********************************************************************************
  * @brief       : 	读取Dht11的数据
  * @param[in]   : 	无
  * @param[out]  : 	hum		湿度数据
  					temp	温度数据
  * @return      : 	0	读取成功
  					-1	读取失败
  * @others      : 	无
***********************************************************************************/
static int ReadDht11(int *hum, int *temp)
{
	unsigned char hum_m, hum_n;
	unsigned char temp_m, temp_n;
	unsigned char check;	
	
	StartDht11();
	
	if (0 != WaitDht11Ack())
	{
		printf("dht11 not ack, err!\n\r");
		return -1;
	}

	if (0 != WaitDht11ForVal(1, 1000))  /* 等待ACK变为高电平, 超时时间是1000us */
	{
		printf("dht11 wait for ack high err!\n\r");
		return -1;
	}

	if (0 != WaitDht11ForVal(0, 1000))  /* 数据阶段: 等待低电平, 超时时间是1000us */
	{
		printf("dht11 wait for data low err!\n\r");
		return -1;
	}

	hum_m  = RecvDht11Byte();
	hum_n  = RecvDht11Byte();
	temp_m = RecvDht11Byte();
	temp_n = RecvDht11Byte();
	check  = RecvDht11Byte();

	CfgGPIOAsOutputForDht11();
	SetDht11Data(1);

	if (hum_m + hum_n + temp_m + temp_n == check)
	{
		*hum  = hum_m;
		*temp = temp_m;
		mDelay(2000);  /* 读取周期是2S, 不能读太频繁 */
		return 0;
	}
	else
	{
		printf("dht11 checksum err!\n\r");
		return -1;
	}
}

/**********************************************************************************
  * @brief       : 	Dht11测试函数
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void TestDht11(void)
{
	int hum, temp;
	char chBuffer[10];
	
	InitDht11();

	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(160, 5, "Dht11 TEST", 0xe3170d, 0);

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
		
		if (ReadDht11(&hum, &temp) !=0 )
		{
			printf("\n\rdht11 read err!\n\r");
			InitDht11();

			PrintFbString8x16(105, 120, "dht11 read err! Please return!!!", 0x4169e1, 0);
			mDelay(1000);
			PrintFbString8x16(105, 120, "dht11 read err! Please return!!!", 0xffffff, 0);
		}
		else
		{
			printf("\n\rDHT11 : %d humidity, %d temperature\n\r", hum, temp);

			PrintFbString16x32(120, 75, "DHT11 : ", 0x4169e1, 0);
			Convert(chBuffer, hum, 10);
			PrintFbString8x16(176, 115, chBuffer, 0x4169e1, 0);
			PrintFbString8x16(200, 115, "humidity", 0x4169e1, 0);
			Convert(chBuffer, temp, 10);
			PrintFbString8x16(176, 140, chBuffer, 0x4169e1, 0);
			PrintFbString8x16(200, 140, "temperature", 0x4169e1, 0);

			mDelay(1000);

			PrintFbString16x32(120, 75, "DHT11 : ", 0xffffff, 0);
			Convert(chBuffer, hum, 10);
			PrintFbString8x16(176, 115, chBuffer, 0xffffff, 0);
			PrintFbString8x16(200, 115, "humidity", 0xffffff, 0);
			Convert(chBuffer, temp, 10);
			PrintFbString8x16(176, 140, chBuffer, 0xffffff, 0);
			PrintFbString8x16(200, 140, "temperature", 0xffffff, 0);		
		}	
	}
}

