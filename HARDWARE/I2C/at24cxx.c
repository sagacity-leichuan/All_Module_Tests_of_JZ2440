
#include "i2c_controller.h"
#include "function.h"
#include "framebuffer.h"
#include "font.h"
#include "string_utils.h"
#include "my_printf.h"
#include "timer.h"
#include "spi_flash.h"
#include "spi_gpio_simulate.h"



#define AT24CXX_ADDR 0x50

int WriteAt24cxx(unsigned int addr, unsigned char *data, int len)
{
	SI2cMsg msg;
	int i;
	int err;
	unsigned char buf[2];

	
	for (i = 0; i < len; i++)
	{
		buf[0] = addr++;
		buf[1] = data[i];
		
		/* 构造i2c_msg */
		msg.iAddr  = AT24CXX_ADDR;
		msg.iFlags = 0; /* write */
		msg.iLen   = 2;
		msg.pchBuf   = buf;
		msg.iErr   = 0;
		msg.iCntTransferred = -1;

		printf("WriteAt24cxx addr = %d\n\r",addr);
		printf("WriteAt24cxx data[%d] = %c\n\r",i,data[i]);

		/* 调用i2c_transfer */
		err = TransferI2c(&msg, 1);
		printf("WriteAt24cxx err = %d\n\r",err);
		
		if (err)
			return err;
	}
	
	return 0;
}


int ReadAt24cxx(unsigned int addr, unsigned char *data, int len)
{
	SI2cMsg msg[2];
	int err;
	
	/* 构造i2c_msg */
	msg[0].iAddr  = AT24CXX_ADDR;
	msg[0].iFlags  = 0; /* write */
	msg[0].iLen   = 1;
	msg[0].pchBuf   = (unsigned char *)&addr;
	msg[0].iErr   = 0;
	msg[0].iCntTransferred = -1;

	msg[1].iAddr  = AT24CXX_ADDR;
	msg[1].iFlags  = 1; /* read */
	msg[1].iLen   = len;
	msg[1].pchBuf   = data;
	msg[1].iErr   = 0;
	msg[1].iCntTransferred = -1;

	/* 调用i2c_transfer */
	err = TransferI2c(msg, 2);
	if (err)
		return err;
	return 0;
}

void DoWriteAt24cxx(void)
{
	unsigned int addr;
	unsigned char str[100];
	int err;
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	if (addr > 256)
	{
		printf("address > 256, error!\n\r");
		return;
	}

	printf("Enter the string to write: ");
	gets(str);

	printf("writing ...\n\r");
	printf("addr = %d, str = %s, len = %d\n\r", addr,str, strlen(str)+1);
	err = WriteAt24cxx(addr, str, strlen(str)+1);
	printf("at24cxx_write ret = %d\n\r", err);
}

void DoReadAt24cxx(void)
{
	unsigned int addr;
	int i, j;
	unsigned char c;
	unsigned char data[100];
	unsigned char str[16];
	int len;
	int err;
	int cnt = 0;
	
	/* 获得地址 */
	printf("Enter the address to read: ");
	addr = get_uint();

	if (addr > 256)
	{
		printf("address > 256, error!\n\r");
		return;
	}

	/* 获得长度 */
	printf("Enter the length to read: ");
	len = get_int();

	err = ReadAt24cxx(addr, data, len);
	printf("at24cxx_read ret = %d\n\r", err);

	printf("Data : \n\r");
	/* 长度固定为64 */
	for (i = 0; i < 4; i++)
	{
		/* 每行打印16个数据 */
		for (j = 0; j < 16; j++)
		{
			/* 先打印数值 */
			c = data[cnt++];
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


void TestAt24cxx(void)
{
	char c;
	unsigned int mid = 0, pid = 0;

	/* 清屏 */
	ClearScreen(0xffffff);

	/* 显示文字提示 */
	PrintFbString16x32(130, 5, "I2C-AT24cxx TEST", 0xe3170d, 0);	

	/* 初始化 */
#ifdef SPIGPIO
	InitSPIGPIO();
#else
	InitSPIS3c2440Controller();
#endif
	ReadSPIFlashID(&mid, &pid);

	if((mid == 0xff) || (pid  == 0xff))
	{
		PrintFbString8x16(40, 120, "The corresponding module failed to connect properly!!!", 0x4169e1, 1);
		mDelay(5000);
		MainPage();
		return;
	}

	/* 初始化 */
	InitI2c();

	DisplayReturnButton();	
	PrintFbString16x32(90, 40, "NOTE:", 0x0b1746, 0);
	PrintFbString8x16(122, 85, 	"You can feel free to", 0x4169e1, 0);
	PrintFbString8x16(90, 110, "test the read and write f", 0x4169e1, 0);
	PrintFbString8x16(90, 130, "unctions without any imp", 0x4169e1, 0);
	PrintFbString8x16(90, 155, "act on the program!", 0x4169e1, 0);

	PrintFbString8x16(40, 240, "If you want to test AT24cxx read-write function, please connect to the PC serial port tool and re-enter this page.", 0xff0000, 0);

	/* 打印菜单, 供我们选择测试内容 */
	printf("\n\r[w] Write at24cxx\n\r");
	printf("[r] Read at24cxx\n\r");
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
		 * 3. 编写某个地址
		 * 4. 读某个地址
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				MainPage();
				return;
				return;
				break;
				
			case 'w':
			case 'W':
				DoWriteAt24cxx();
				printf("\n\r[w] Write at24cxx\n\r");
				printf("[r] Read at24cxx\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;

			case 'r':
			case 'R':
				DoReadAt24cxx();
				printf("\n\r[w] Write at24cxx\n\r");
				printf("[r] Read at24cxx\n\r");
				printf("[q] quit\n\r");
				printf("Enter selection: ");
				break;
			default:
				break;
		}
	}

}


