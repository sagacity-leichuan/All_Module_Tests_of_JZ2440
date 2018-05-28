
#include "s3c2440_soc.h"
#include "nand_flash.h"
#include "my_printf.h"
#include "string_utils.h"


#define TXD0READY   (1<<2)

void InitNand(void)
{
#define TACLS   0
#define TWRPH0  1
#define TWRPH1  0
	/* 设置时序 */
	NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	/* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
	NFCONT = (1<<4)|(1<<1)|(1<<0);	
}

void SelectNand(void)
{
	NFCONT &= ~(1<<1);	
}

void DeselectNand(void)
{
	NFCONT |= (1<<1);	
}

void NandCmd(unsigned char cmd)
{
	volatile int i;
	NFCCMD = cmd;
	for (i = 0; i < 10; i++);
}

void NandAddrByte(unsigned char addr)
{
	volatile int i;
	NFADDR = addr;
	for(i=0; i<10; i++);
}


void NandAddr(unsigned int addr)
{
	unsigned int iCol  = addr % 2048;
	unsigned int iPage = addr / 2048;
	volatile int i;

	NFADDR = iCol & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (iCol >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	
	NFADDR  = iPage & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (iPage >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (iPage >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}

void NandPage(unsigned int page)
{
	volatile int i;
	
	NFADDR  = page & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 8) & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR  = (page >> 16) & 0xff;
	for (i = 0; i < 10; i++);	
}

void NandCol(unsigned int col)
{
	volatile int i;

	NFADDR = col & 0xff;
	for (i = 0; i < 10; i++);
	NFADDR = (col >> 8) & 0xff;
	for (i = 0; i < 10; i++);
}


void NandWaitReady(void)
{
	while (!(NFSTAT & 1));
}

unsigned char NandData(void)
{
	return NFDATA;
}

int NandBad(unsigned int addr)
{
	unsigned int col  = 2048;
	unsigned int page = addr / 2048;
	unsigned char val;

	/* 1. 选中 */
	SelectNand();
	
	/* 2. 发出读命令00h */
	NandCmd(0x00);
	
	/* 3. 发出地址(分5步发出) */
	NandCol(col);
	NandPage(page);
	
	/* 4. 发出读命令30h */
	NandCmd(0x30);
	
	/* 5. 判断状态 */
	NandWaitReady();

	/* 6. 读数据 */
	val = NandData();
	
	/* 7. 取消选中 */		
	DeselectNand();


	if (val != 0xff)
		return 1;  /* bad blcok */
	else
		return 0;
}


void ReadNand(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int iCol = addr % 2048;
	int i = 0;
		
	while (i < len)
	{

		if (!(addr & 0x1FFFF) && NandBad(addr)) /* 一个block只判断一次 */
		{
			addr += (128*1024);  /* 跳过当前block */
			continue;
		}

		/* 1. 选中 */
		SelectNand();
		
		
		/* 2. 发出读命令00h */
		NandCmd(0x00);

		/* 3. 发出地址(分5步发出) */
		NandAddr(addr);

		/* 4. 发出读命令30h */
		NandCmd(0x30);

		/* 5. 判断状态 */
		NandWaitReady();

		/* 6. 读数据 */
		for (; (iCol < 2048) && (i < len); iCol++)
		{
			buf[i] = NandData();
			i++;
			addr++;
		}
		
		iCol = 0;


		/* 7. 取消选中 */		
		DeselectNand();
		
	}
}

void NandWData(unsigned char val)
{
	NFDATA = val;
}



void PrintNandChipID(void)
{ 
	unsigned char buf[5]={0};
	
	SelectNand(); 
	NandCmd(0x90);
	NandAddrByte(0x00);

	buf[0] = NandData();
	buf[1] = NandData();	
	buf[2] = NandData();
	buf[3] = NandData();
	buf[4] = NandData();	
	DeselectNand(); 	

	printf("maker   id  = 0x%x\n\r",buf[0]);
	printf("device  id  = 0x%x\n\r",buf[1]);	
	printf("3rd byte    = 0x%x\n\r",buf[2]);		
	printf("4th byte    = 0x%x\n\r",buf[3]);			
	printf("page  size  = %d kb\n\r",1  <<  (buf[3] & 0x03));	
	printf("block size  = %d kb\n\r",64 << ((buf[3] >> 4) & 0x03));	
	printf("5th byte    = 0x%x\n\r",buf[4]);

	
}

int NandErase(unsigned int addr, unsigned int len)
{
	int page = addr / 2048;

	if (addr & (0x1FFFF))
	{
		printf("nand_erase err, addr is not block align\n\r");
		return -1;
	}
	
	if (len & (0x1FFFF))
	{
		printf("nand_erase err, len is not block align\n\r");
		return -1;
	}
	
	SelectNand(); 

	while (1)
	{
		page = addr / 2048;
		
		NandCmd(0x60);
		
		/* row/page addr */
		NandAddrByte(page & 0xff);
		NandAddrByte((page>>8) & 0xff);
		NandAddrByte((page>>16) & 0xff);

		NandCmd(0xD0);

		NandWaitReady();

		len -= (128*1024);
		if (len == 0)
			break;
		addr += (128*1024);
	}
	
	DeselectNand(); 	
	return 0;
}

void NandWrite(unsigned int addr, unsigned char *buf, unsigned int len)
{
	int page = addr / 2048;
	int col  = addr & (2048 - 1);
	int i = 0;

	SelectNand(); 

	while (1)
	{
		NandCmd(0x80);

		/* 发出地址 */
		/* col addr */
		NandAddrByte(col & 0xff);
		NandAddrByte((col>>8) & 0xff);
		
		/* row/page addr */
		NandAddrByte(page & 0xff);
		NandAddrByte((page>>8) & 0xff);
		NandAddrByte((page>>16) & 0xff);

		/* 发出数据 */
		for (; (col < 2048) && (i < len); )
		{
			NandWData(buf[i++]);
		}
		NandCmd(0x10);
		NandWaitReady();

		if (i == len)
			break;
		else
		{
			/* 开始下一个循环page */
			col = 0;
			page++;
		}
		
	}
	
	DeselectNand(); 	
}



void DoReadNandFlash(void)
{
	unsigned int addr;
	volatile unsigned char *p;
	int i, j;
	unsigned char c;
	unsigned char str[16];
	unsigned char buf[64];
	
	/* 获得地址 */
	printf("Enter the address to read: ");
	addr = get_uint();

	ReadNand(addr, buf, 64);
	p = (volatile unsigned char *)buf;

	printf("Data : \n\r");
	/* 长度固定为64 */
	for (i = 0; i < 4; i++)
	{
		/* 每行打印16个数据 */
		for (j = 0; j < 16; j++)
		{
			/* 先打印数值 */
			c = *p++;
			str[j] = c;
			printf("%02x ", c);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* 后打印字符 */
			if (str[j] < 0x20 || str[j] > 0x7e)  /* 不可视字符 */
				PutChar('.');
			else
				PutChar(str[j]);
		}
		printf("\n\r");
	}
}

void DoEraseNandFlash(void)
{
	unsigned int addr;
	
	/* 获得地址 */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");
	NandErase(addr, 128*1024);
}


void DoWriteNandFlash(void)
{
	unsigned int addr;
	unsigned char str[100];
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets((char *)str);

	printf("writing ...\n\r");
	NandWrite(addr, str, strlen((char *)str)+1);

}




