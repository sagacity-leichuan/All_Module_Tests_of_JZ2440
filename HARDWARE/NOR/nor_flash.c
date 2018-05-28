#include "my_printf.h"
#include "string_utils.h"
#include "nor_flash.h"


#define NOR_FLASH_BASE  0  /* jz2440, nor-->cs0, base addr = 0 */


/* 比如:   55H 98 
 * 本意是: 往(0 + (0x55)<<1)写入0x98
 */
void NorWriteWord(unsigned int base, unsigned int offset, unsigned int val)
{
	volatile unsigned short *vpnTemp = (volatile unsigned short *)(base + (offset << 1));
	*vpnTemp = val;
}

/* offset是基于NOR的角度看到 */
void NorCmd(unsigned int offset, unsigned int cmd)
{
	NorWriteWord(NOR_FLASH_BASE, offset, cmd);
}

unsigned int NorReadWord(unsigned int base, unsigned int offset)
{
	volatile unsigned short *vpnTemp = (volatile unsigned short *)(base + (offset << 1));
	return *vpnTemp;
}

unsigned int NorDat(unsigned int offset)
{
	return NorReadWord(NOR_FLASH_BASE, offset);
}


void NorWaitReady(unsigned int addr)
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


/* 进入NOR FLASH的CFI模式
 * 读取各类信息
 */
void DoScanNorFlash(void)
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

void DoEraseNorFlash(void)
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

void DoWriteNorFlash(void)
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

void DoReadNorFlash(void)
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




