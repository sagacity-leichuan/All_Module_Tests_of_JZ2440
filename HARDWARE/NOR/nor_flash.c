/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板nor flash代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#include "my_printf.h"
#include "string_utils.h"
#include "nor_flash.h"
#include "function.h"
#include "tslib.h"
#include "framebuffer.h"
#include "font.h"
#include "timer.h"

#define NOR_FLASH_BASE  0  /* jz2440, nor-->cs0, base addr = 0 */

/**********************************************************************************
  * @brief       : 	向nor flash写入一个字
  * @param[in]   : 	base	nor flash基址
  					offset	地址偏移量
  					val		待写入的字
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	比如:   55H 98 
					本意是: 往(0 + (0x55)<<1)写入0x98
***********************************************************************************/
static void NorWriteWord(unsigned int base, unsigned int offset, unsigned int val)
{
	volatile unsigned short *vpnTemp = (volatile unsigned short *)(base + (offset << 1));
	*vpnTemp = val;
}

/**********************************************************************************
  * @brief       : 	向nor flash写入一个命令
  * @param[in]   : 	offset	地址偏移量
  					cmd		待写入的命令
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	offset是基于NOR的角度看到
***********************************************************************************/
static void NorCmd(unsigned int offset, unsigned int cmd)
{
	NorWriteWord(NOR_FLASH_BASE, offset, cmd);
}

/**********************************************************************************
  * @brief       : 	向nor flash读取一个字的数据
  * @param[in]   : 	base	nor flash基址
  					offset	地址偏移量
  * @param[out]  : 	无
  * @return      : 	返回一个字的数据
  * @others      : 	无
***********************************************************************************/
static unsigned int NorReadWord(unsigned int base, unsigned int offset)
{
	volatile unsigned short *vpnTemp = (volatile unsigned short *)(base + (offset << 1));
	return *vpnTemp;
}

/**********************************************************************************
  * @brief       : 	向nor flash读取数据
  * @param[in]   : 	offset	地址偏移量
  * @param[out]  : 	无
  * @return      : 	返回的数据
  * @others      : 	无
***********************************************************************************/
static unsigned int NorDat(unsigned int offset)
{
	return NorReadWord(NOR_FLASH_BASE, offset);
}

/**********************************************************************************
  * @brief       : 	等待nor flash就绪
  * @param[in]   : 	addr	待操作的地址
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void NorWaitReady(unsigned int addr)
{
	unsigned int iVal;
	unsigned int iPre;

	iPre = NorDat(addr>>1);
	iVal = NorDat(addr>>1);
	while ((iVal & (1<<6)) != (iPre & (1<<6)))
	{
		iPre = iVal;
		iVal = NorDat(addr>>1);		
	}
}

/**********************************************************************************
  * @brief		 :	测试nor flash用扫描函数
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	进入NOR FLASH的CFI模式
    				读取各类信息
***********************************************************************************/
static void DoScanNorFlash(void)
{
	char szStr[4];
	unsigned int uiSize;
	int iRegions;
	int i;
	int iRegionInfoBase;
	int iBlockAddr, iBlocks, iBlockSize;
	int j;
	int iCnt;

	int iVendor, iDevice;
	
	/* 打印厂家ID、设备ID */
	NorCmd(0x555, 0xaa);    /* 解锁 */
	NorCmd(0x2aa, 0x55); 
	NorCmd(0x555, 0x90);    /* read id */
	iVendor = NorDat(0);
	iDevice = NorDat(1);
	NorCmd(0, 0xf0);        /* reset */
	
	NorCmd(0x55, 0x98);  /* 进入cfi模式 */

	szStr[0] = NorDat(0x10);
	szStr[1] = NorDat(0x11);
	szStr[2] = NorDat(0x12);
	szStr[3] = '\0';
	printf("str = %s\n\r", szStr);

	/* 打印容量 */
	uiSize = 1<<(NorDat(0x27));
	printf("vendor id = 0x%x, device id = 0x%x, nor size = 0x%x, %dM\n\r", iVendor, iDevice, uiSize, uiSize/(1024*1024));

	/* 打印各个扇区的起始地址 */
	/* 名词解释:
	 *    erase block region : 里面含有1个或多个block, 它们的大小一样
	 * 一个nor flash含有1个或多个region
	 * 一个region含有1个或多个block(扇区)
	 * Erase block region information:
	 *    前2字节+1    : 表示该region有多少个block 
	 *    后2字节*256  : 表示block的大小
	 */

	iRegions = NorDat(0x2c);
	iRegionInfoBase = 0x2d;
	iBlockAddr = 0;
	printf("Block/Sector start Address:\n\r");
	iCnt = 0;
	for (i = 0; i < iRegions; i++)
	{
		iBlocks = 1 + NorDat(iRegionInfoBase) + (NorDat(iRegionInfoBase+1)<<8);
		iBlockSize = 256 * (NorDat(iRegionInfoBase+2) + (NorDat(iRegionInfoBase+3)<<8));
		iRegionInfoBase += 4;

		//printf("\n\rregion %d, blocks = %d, block_size = 0x%x, block_addr = 0x%x\n\r", i, blocks, block_size, block_addr);

		for (j = 0; j < iBlocks; j++)
		{
			/* 打印每个block的起始地址 */
			//printf("0x%08x ", block_addr);
			printHex(iBlockAddr);
			PutChar(' ');
			iCnt++;
			iBlockAddr += iBlockSize;
			if (iCnt % 5 == 0)
				printf("\n\r");
		}
	}
	printf("\n\r");
	/* 退出CFI模式 */
	NorCmd(0, 0xf0);
}

/**********************************************************************************
  * @brief		 :	测试nor flash用擦除函数
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
***********************************************************************************/
static void DoEraseNorFlash(void)
{
	unsigned int iAddr;
	
	/* 获得地址 */
	printf("Enter the address of sector to erase: ");
	iAddr = get_uint();

	printf("erasing ...\n\r");
	NorCmd(0x555, 0xaa);    /* 解锁 */
	NorCmd(0x2aa, 0x55); 
	NorCmd(0x555, 0x80);	 /* erase sector */
	
	NorCmd(0x555, 0xaa);    /* 解锁 */
	NorCmd(0x2aa, 0x55); 
	NorCmd(iAddr>>1, 0x30);	 /* 发出扇区地址 */
	NorWaitReady(iAddr);
}

/**********************************************************************************
  * @brief		 :	测试nor flash用写入函数
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
***********************************************************************************/
static void DoWriteNorFlash(void)
{
	unsigned int iAddr;
	unsigned char szStr[100];
	int i, j;
	unsigned int ival;
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	iAddr = get_uint();

	printf("Enter the string to write: ");
	gets((char *)szStr);

	printf("writing ...\n\r");

	/* str[0],str[1]==>16bit 
	 * str[2],str[3]==>16bit 
	 */
	i = 0;
	j = 1;
	while (szStr[i] && szStr[j])
	{
		ival = szStr[i] + (szStr[j]<<8);
		
		/* 烧写 */
		NorCmd(0x555, 0xaa);	 /* 解锁 */
		NorCmd(0x2aa, 0x55); 
		NorCmd(0x555, 0xa0);	 /* program */
		NorCmd(iAddr>>1, ival);
		/* 等待烧写完成 : 读数据, Q6无变化时表示结束 */
		NorWaitReady(iAddr);

		i += 2;
		j += 2;
		iAddr += 2;
	}

	ival = szStr[i];
	/* 烧写 */
	NorCmd(0x555, 0xaa);	 /* 解锁 */
	NorCmd(0x2aa, 0x55); 
	NorCmd(0x555, 0xa0);	 /* program */
	NorCmd(iAddr>>1, ival);
	/* 等待烧写完成 : 读数据, Q6无变化时表示结束 */
	NorWaitReady(iAddr);
}

/**********************************************************************************
  * @brief		 :	测试nor flash用读取函数
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
***********************************************************************************/
static void DoReadNorFlash(void)
{
	unsigned int iAddr;
	volatile unsigned char *vpchTemp;
	int i, j;
	unsigned char chTemp;
	unsigned char szStr[16];
	
	/* 获得地址 */
	printf("Enter the address to read: ");
	iAddr = get_uint();

	vpchTemp = (volatile unsigned char *)iAddr;

	printf("Data : \n\r");
	/* 长度固定为64 */
	for (i = 0; i < 4; i++)
	{
		/* 每行打印16个数据 */
		for (j = 0; j < 16; j++)
		{
			/* 先打印数值 */
			chTemp = *vpchTemp++;
			szStr[j] = chTemp;
			printf("%02x ", chTemp);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* 后打印字符 */
			if (szStr[j] < 0x20 || szStr[j] > 0x7e)  /* 不可视字符 */
				PutChar('.');
			else
				PutChar(szStr[j]);
		}
		printf("\n\r");
	}
}

/**********************************************************************************
  * @brief		 :	测试nor flash功能函数
  * @param[in]	 :	无
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
***********************************************************************************/
void TestNorFlash(void)
{
	char c;
	unsigned int uiSize;
	int iVendor, iDevice;
	unsigned char result[33];

	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(176, 5, "NOR TEST", 0xe3170d, 0);

	if(!isBootFromNorFlash())
	{
		PrintFbString8x16(40, 120, "This start is nand flash start, can not test nor flash! ! !", 0x4169e1, 1);
		Delay(3500000);
		PrintFbString8x16(40, 120, "This start is nand flash start, can not test nor flash! ! !", 0xffffff, 1);

		MainPage();

		return;
	}
	
	DisplayReturnButton();


	/* 打印厂家ID、设备ID */
	NorCmd(0x555, 0xaa);    /* 解锁 */
	NorCmd(0x2aa, 0x55); 
	NorCmd(0x555, 0x90);    /* read id */
	iVendor = NorDat(0);
	iDevice = NorDat(1);
	NorCmd(0, 0xf0);        /* reset */
	
	NorCmd(0x55, 0x98);  /* 进入cfi模式 */

	/* 获取容量 */
	uiSize = 1<<(NorDat(0x27));
	/* 退出CFI模式 */
	NorCmd(0, 0xf0);
	
	printf("vendor id = 0x%x, device id = 0x%x, nor size = 0x%x, %dM\n\r", iVendor, iDevice, uiSize, uiSize/(1024*1024));

	Convert(result,(unsigned int)iVendor,16);
	PrintFbString8x16(20, 70, "vendor id  =  0x", 0x0, 0);
	PrintFbString8x16(148, 70, result, 0x0, 0);

	Convert(result,(unsigned int)iDevice,16);
	PrintFbString8x16(20, 90, "device id  =  0x", 0x0, 0);
	PrintFbString8x16(148, 90, result, 0x0, 0);
	
	Convert(result,(unsigned int)uiSize,16);
	PrintFbString8x16(20, 110, "nor  size  =  0x", 0x0, 0);
	PrintFbString8x16(148, 110, result, 0x0, 0);
	PrintFbString8x16(212, 110, "byte, ", 0x0, 0);
	
	Convert(result,(unsigned int)(uiSize/(1024*1024)),10);
	PrintFbString8x16(20, 130, "nor  size  =    ", 0x0, 0);
	PrintFbString8x16(148, 130, result, 0x0, 0);
	PrintFbString8x16(210, 130, "M", 0x0, 0);

	PrintFbString8x16(40, 240, "If you want to test the nor read-write function, please connect to the PC serial port tool and re-enter this page.", 0xff0000, 0);

	PrintFbString16x32(288, 40, "NOTE:", 0x0b1746, 0);
	PrintFbString8x16(320, 85, 	"Please carefully per", 0x4169e1, 0);
	PrintFbString8x16(288, 110, "form the erase and write", 0x4169e1, 0);
	PrintFbString8x16(288, 130, "operations, otherwise th", 0x4169e1, 0);
	PrintFbString8x16(288, 155, "e program may die!", 0x4169e1, 0);

	/* 打印菜单, 供我们选择测试内容 */
	printf("\n\r");
	printf("       Nor Flash Test\n\r");		
	printf("[s] Scan nor flash\n\r");
	printf("[e] Erase nor flash\n\r");
	printf("[w] Write nor flash\n\r");
	printf("[r] Read nor flash\n\r");
	printf("[q] quit\n\r");
	printf("Enter selection: ");
	while (1)
	{
		if(isClickReturn())
		{
			MainPage();
			break;
		}

		c = GetChar();
		printf("%c\n\r", c);
		if(c == 0)
		{
			break;
		}
		

		/* 测试内容:
		 * 1. 识别nor flash
		 * 2. 擦除nor flash某个扇区
		 * 3. 编写某个地址
		 * 4. 读某个地址
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				MainPage();
				return;
				break;
				
			case 's':
			case 'S':
				DoScanNorFlash();
				printf("\n\r");
				printf("       Nor Flash Test\n\r");		
				printf("[s] Scan nor flash\n\r");
				printf("[e] Erase nor flash\n\r");
				printf("[w] Write nor flash\n\r");
				printf("[r] Read nor flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'e':
			case 'E':
				DoEraseNorFlash();
				printf("\n\r");
				printf("	   Nor Flash Test\n\r");		
				printf("[s] Scan nor flash\n\r");
				printf("[e] Erase nor flash\n\r");
				printf("[w] Write nor flash\n\r");
				printf("[r] Read nor flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'w':
			case 'W':
				DoWriteNorFlash();
				printf("\n\r");
				printf("	   Nor Flash Test\n\r");		
				printf("[s] Scan nor flash\n\r");
				printf("[e] Erase nor flash\n\r");
				printf("[w] Write nor flash\n\r");
				printf("[r] Read nor flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'r':
			case 'R':
				DoReadNorFlash();
				printf("\n\r");
				printf("	   Nor Flash Test\n\r");		
				printf("[s] Scan nor flash\n\r");
				printf("[e] Erase nor flash\n\r");
				printf("[w] Write nor flash\n\r");
				printf("[r] Read nor flash\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;
			default:
				break;
		}
	}

}

