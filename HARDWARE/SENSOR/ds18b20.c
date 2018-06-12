/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板温度传感器DS18B20代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#include "s3c2440_soc.h"
#include "timer.h"
#include "my_printf.h"
#include "nand_flash.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"
#include "string_utils.h"

/********************************NOTE**************************************************

使用GPG6作用ds18b20的DATA引脚

******************************************************************************************/

/* rom commands */
#define SEARCH_ROM    0xF0
#define READ_ROM      0x33
#define MATCH_ROM     0x55
#define SKIP_ROM      0xCC
#define ALARM_ROM     0xEC

/* functions commands */
#define CONVERT_TEAMPERATURE 0x44
#define WRITE_SCRATCHPAD     0x4E
#define READ_SCRATCHPAD      0xBE
#define COPY_SCRATCHPAD      0x48
#define RECALL_EEPROM        0xB8
#define READ_POWER_SUPPLY    0xB4


/**********************************************************************************
  * @brief       : 	配置GPIO作为输出引脚为Ds18b20输入数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void CfgGPIOAsOutputForDs18b20(void)
{
	GPGCON &= ~(3<<12);
	GPGCON |= (1<<12);
}

/**********************************************************************************
  * @brief       : 	配置GPIO作为输入引脚用以接受Ds18b20的数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void CfgGPIOAsInputForDs18b20(void)
{
	GPGCON &= ~(3<<12);
}

/**********************************************************************************
  * @brief       : 	使用特定的值设置Ds18b20
  * @param[in]   : 	val	需要设置的数据
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SetDs18b20Data(int val)
{
	if (val)
		GPGDAT |= (1<<6);
	else
		GPGDAT &= ~(1<<6);
}

/**********************************************************************************
  * @brief       : 	得到Ds18b20的一位数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	1或0   代表一个位的值
  * @others      : 	无
***********************************************************************************/
static int GetDs18b20Data(void)
{
	if (GPGDAT & (1<<6))
		return 1;
	else
		return 0;
}

/**********************************************************************************
  * @brief       : 	使用特定的值设置Ds18b20并延时特定的时间
  * @param[in]   : 	val	需要设置的数据
  					us	设置完数据需要延时的时间(单位为us)
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SetDs18b20DataValForTime(int val, int us)
{
	CfgGPIOAsOutputForDs18b20();
	SetDs18b20Data(val);
	uDelay(us);
}

/**********************************************************************************
  * @brief       : 	释放Ds18b20
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void ReleaseDs18b20Data(void)
{
	CfgGPIOAsInputForDs18b20();
}

/**********************************************************************************
  * @brief       : 	初始化Ds18b20
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	0	初始化成功
  					1	初始化失败
  * @others      : 	无
***********************************************************************************/
static int InitializationDs18b20(void)
{
	int val;
	
	SetDs18b20DataValForTime(0, 500);
	ReleaseDs18b20Data();
	uDelay(80);

	val = GetDs18b20Data();
	uDelay(250);
	return val;
}

/**********************************************************************************
  * @brief       : 	向Ds18b20写入一个位的数据
  * @param[in]   : 	val	写入的一个位的数据	取值范围{0,1}
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void WriteDs18b20Bit(int val)
{
	if (0 == val)
	{
		SetDs18b20DataValForTime(0, 60);		
		ReleaseDs18b20Data();
		uDelay(2);
	}
	else
	{
		SetDs18b20DataValForTime(0, 2);		
		ReleaseDs18b20Data();
		uDelay(60);
	}
}

/**********************************************************************************
  * @brief       : 	向Ds18b20读取一个位的数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	val	读取的一个位的数据	取值范围{0,1}
  * @others      : 	无
***********************************************************************************/
static int ReadDs18b20Bit(void)
{
	int val;
	
	SetDs18b20DataValForTime(0, 2);		
	ReleaseDs18b20Data();
	uDelay(10);
	val = GetDs18b20Data();
	uDelay(50);
	return val;
}

/**********************************************************************************
  * @brief       : 	向Ds18b20写入一个字节的数据
  * @param[in]   : 	data	写入的一个字节数据
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void WriteDs18b20Byte(unsigned char data)
{
	int i;
	for (i = 0; i < 8; i++)
	{

		WriteDs18b20Bit(data & (1<<i));
	}
}

/**********************************************************************************
  * @brief       : 	向Ds18b20读取一个字节的数据
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	data	读取的一个字节的数据
  * @others      : 	无
***********************************************************************************/
static unsigned char ReadDs18b20Byte(void)
{
	int i;
	unsigned char data = 0;

	for (i = 0; i < 8; i++)
	{
		if (ReadDs18b20Bit() == 1)
			data |= (1<<i);
	}

	return data;
}


/**********************************************************************************
  * @brief       : 	向Ds18b20写入一个Rom命令
  * @param[in]   : 	cmd		需要写入的Rom命令
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void WriteDs18b20RomCmd(unsigned char cmd)
{
	WriteDs18b20Byte(cmd);
}

/**********************************************************************************
  * @brief       : 	向Ds18b20写入一个Function命令
  * @param[in]   : 	cmd		需要写入的Function命令
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void WriteDs18b20FunctionCmd(unsigned char cmd)
{
	WriteDs18b20Byte(cmd);
}

/**********************************************************************************
  * @brief       : 	读取Ds18b20的Rom中的数据
  * @param[in]   : 	无
  * @param[out]  : 	rom		存放读取的数据
  * @return      : 	0	读取成功
  					1	读取失败
  * @others      : 	无
***********************************************************************************/
static int ReadDs18b20Rom(unsigned char rom[])
{
	int i;
	
	if (InitializationDs18b20() != 0)
	{
		printf("ds18b20_initialization err!\n\r");
		return -1;
	}

	WriteDs18b20RomCmd(READ_ROM);
	
	for (i = 0; i < 8; i++)
	{
		rom[i] = ReadDs18b20Byte();
	}

	return 0;
}

/**********************************************************************************
  * @brief       : 	在DS18B20工作时等待其就绪
  * @param[in]   : 	time_out 	超时时间
  * @param[out]  : 	无
  * @return      : 	0	收到DS18B20的回应信号
  					-1	未收到DS18B20的回应信号
  * @others      : 	无
***********************************************************************************/
static int WaitDs18b20WhenProcessing(int timeout_us)
{
	while (timeout_us--)
	{
		if (ReadDs18b20Bit() == 1)
			return 0;  /* ok */
		uDelay(1);
	}
	return -1;
}

/**********************************************************************************
  * @brief       : 	启动DS18B20转换
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	0	启动成功
  					-1	启动失败
  * @others      : 	无
***********************************************************************************/
static int StartDs18b20Convert(void)
{
	if (InitializationDs18b20() != 0)
	{
		printf("ds18b20_initialization err!\n\r");
		return -1;
	}

	WriteDs18b20RomCmd(SKIP_ROM);
	WriteDs18b20FunctionCmd(CONVERT_TEAMPERATURE);

	/* 等待/判断转换成功 */
	if (0 != WaitDs18b20WhenProcessing(1000000))
	{
		printf("ds18b20_wait_when_processing err!\n\r");
		return -1;
	}

	return 0;	
}

/**********************************************************************************
  * @brief       : 	读取DS18B20中ram数据
  * @param[in]   : 	无
  * @param[out]  : 	ram		用于存放读取的ram的数据
  * @return      : 	0	读取成功
  					-1	读取失败
  * @others      : 	无
***********************************************************************************/
static int ReadDs18b20Ram(unsigned char ram[])
{
	int i;
	
	if (InitializationDs18b20() != 0)
	{
		printf("ds18b20_initialization err!\n\r");
		return -1;
	}

	WriteDs18b20RomCmd(SKIP_ROM);
	WriteDs18b20FunctionCmd(READ_SCRATCHPAD);

	for (i = 0; i < 9; i++)
	{
		ram[i] = ReadDs18b20Byte();
	}

	return 0;
}


/**********************************************************************************
  * @brief       : 	读取DS18B20中的温度数据
  * @param[in]   : 	无
  * @param[out]  : 	temp	用于存放读取的温度数据
  * @return      : 	0	读取成功
  					err	读取失败
  * @others      : 	无
***********************************************************************************/
static int ReadDs18b20Temperature(double *temp)
{
	int err;
	unsigned char ram[9];
	double val[] = {0.0625, 0.125, 0.25, 0.5, 1, 2, 4, 8, 16, 32, 64};
	double sum = 0;
	int i;
	
	err = StartDs18b20Convert();
	if (err)
		return err;

	err = ReadDs18b20Ram(ram);
	if (err)
		return err;

	/* 计算温度 */

	/* 先判断精度 */
	if ((ram[4] & (3<<5)) == 0) /* 精度: 9bit */
		i = 3;
	else if ((ram[4] & (3<<5)) == (1<<5)) /* 精度: 10bit */
		i = 2;
	else if ((ram[4] & (3<<5)) == (2<<5)) /* 精度: 11bit */
		i = 1;
	else
		/* 精度是 12 bit */
		i = 0;
	
	for (; i < 8; i++)
	{
		if (ram[0] & (1<<i))
			sum += val[i];
	}

	for (i = 0; i < 3; i++)
	{
		if (ram[1] & (1<<i))
			sum += val[8+i];
	}

	if (ram[1] & (1<<3))
		sum = 0 - sum;

	*temp = sum;
	return 0;
}

/**********************************************************************************
  * @brief       : 	初始化DS18B20状态
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void InitDs18b20State(void)
{
	ReleaseDs18b20Data();
}

/**********************************************************************************
  * @brief       : 	测试DS18B20温度传感器
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void TestDs18b20(void)
{
	double temp = 0;
	int m = 0,n = 0;
	char chBuffer[10];

	InitDs18b20State();

	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(140, 5, "Ds18b20 TEST", 0xe3170d, 0);

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
		
		if (0 == ReadDs18b20Temperature(&temp))
		{

			PrintFbString8x16(130, 110, "ds18b20 temperature: ", 0xffffff, 0);
			Convert(chBuffer, m, 10);
			PrintFbString8x16(298, 110,chBuffer, 0xffffff, 0);
			PrintFbString8x16(314, 110, ".", 0xffffff, 0);
			Convert(chBuffer, n, 10);
			PrintFbString8x16(322, 110,chBuffer, 0xffffff, 0);
			
			m = (int)temp;	/* 3.01, m = 3 */
			temp = temp - m;	/* 小数部分: 0.01 */
			n = temp * 10000;  /* 10 */
			
			/* 在串口上打印 */
			printf("ds18b20 temperature: %d.%04d\n\r", m, n);  /* 3.010v */


			PrintFbString8x16(130, 110, "ds18b20 temperature: ", 0x4169e1, 0);
			Convert(chBuffer, m, 10);
			PrintFbString8x16(298, 110,chBuffer, 0x4169e1, 0);
			PrintFbString8x16(314, 110, ".", 0x4169e1, 0);
			Convert(chBuffer, n, 10);
			PrintFbString8x16(322, 110,chBuffer, 0x4169e1, 0);

			mDelay(1500);
		}
	}
}

