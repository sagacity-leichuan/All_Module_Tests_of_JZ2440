/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板SPI Flash代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/
#include "s3c2440_soc.h"
#include "spi_gpio_simulate.h"
#include "spi_s3c2440_controller.h"
#include "timer.h"
#include "my_printf.h"
#include "function.h"
#include "string_utils.h"

/**********************************************************************************
  * @brief       : 	设置SPI Flash的CS状态
  * @param[in]   : 	val	待设置的值   		取值{0,1}
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SetSPIFlashCS(char val)
{
    if (val)
        GPGDAT |= (1<<2);
    else
        GPGDAT &= ~(1<<2);
}

/**********************************************************************************
  * @brief       : 	向SPI Flash发送地址
  * @param[in]   : 	addr	待发送的地址
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SendSPIFlashAddr(unsigned int addr)
{
#ifdef SPIGPIO

    SendByteSPIGPIO(addr >> 16);
    SendByteSPIGPIO(addr >> 8);
    SendByteSPIGPIO(addr & 0xff);
#else

	SendByteSPIS3c2440Controller(addr >> 16);
	SendByteSPIS3c2440Controller(addr >> 8);
	SendByteSPIS3c2440Controller(addr & 0xff);	
#endif
}

/**********************************************************************************
  * @brief       : 	读取SPI Flash的ID
  * @param[in]   : 	无
  * @param[out]  : 	pMID	存放获取的MID
  					pDID	存放获取的DID
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void ReadSPIFlashID(int *pMID, int *pDID)
{
#ifdef SPIGPIO     //使用gpio模拟
    SetSPIFlashCS(0); /* 选中SPI FLASH */

    SendByteSPIGPIO(0x90);

    SendSPIFlashAddr(0);

    *pMID = RecvByteSPIGPIO();
    *pDID = RecvByteSPIGPIO();

    SetSPIFlashCS(1);
#else	 //使用时s3c2440里面的spi控制器
	SetSPIFlashCS(0); /* 选中SPI FLASH */

	SendByteSPIS3c2440Controller(0x90);

	SendSPIFlashAddr(0);

	*pMID = RecvByteSPIS3c2440Controller();
	*pDID = RecvByteSPIS3c2440Controller();

	SetSPIFlashCS(1);
#endif
}

/**********************************************************************************
  * @brief       : 	SPI Flash写使能
  * @param[in]   : 	enable	使能状态 取值{0,1}
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SPIFlashWriteEnable(int enable)
{
#ifdef SPIGPIO
    if (enable)
    {
        SetSPIFlashCS(0);
        SendByteSPIGPIO(0x06);
        SetSPIFlashCS(1);
    }
    else
    {
        SetSPIFlashCS(0);
        SendByteSPIGPIO(0x04);
        SetSPIFlashCS(1);
    }
 #else
	 if (enable)
	 {
		 SetSPIFlashCS(0);
		 SendByteSPIS3c2440Controller(0x06);
		 SetSPIFlashCS(1);
	 }
	 else
	 {
		 SetSPIFlashCS(0);
		 SendByteSPIS3c2440Controller(0x04);
		 SetSPIFlashCS(1);
	 }
 #endif
}

/**********************************************************************************
  * @brief       : 	读取SPI Flash状态寄存器1
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	val	状态寄存器1的值
  * @others      : 	无
***********************************************************************************/
static unsigned char SPIFlashReadStatusReg1(void)
{
#ifdef SPIGPIO
    unsigned char val;
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x05);
    val = RecvByteSPIGPIO();
    SetSPIFlashCS(1);
    return val;
#else
	unsigned char val;
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x05);
	val = RecvByteSPIS3c2440Controller();
	SetSPIFlashCS(1);
	return val;
#endif
}

/**********************************************************************************
  * @brief       : 	读取SPI Flash状态寄存器2
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	val	状态寄存器2的值
  * @others      : 	无
***********************************************************************************/
static unsigned char SPIFlashReadStatusReg2(void)
{
#ifdef SPIGPIO
    unsigned char val;
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x35);
    val = RecvByteSPIGPIO();
    SetSPIFlashCS(1);
    return val;
#else
	unsigned char val;
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x35);
	val = RecvByteSPIS3c2440Controller();
	SetSPIFlashCS(1);
	return val;
#endif
}

/**********************************************************************************
  * @brief       : 	当SPI Flash忙是进行等待
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SPIFlashWaitWhenBusy(void)
{
    while (SPIFlashReadStatusReg1() & 1);
}

/**********************************************************************************
  * @brief       : 	写SPI Flash状态寄存器
  * @param[in]   : 	reg1	待写入的状态寄存器1的值
  					reg2	待写入的状态寄存器2的值
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SPIFlashWriteStatusReg(unsigned char reg1, unsigned char reg2)
{  

    SPIFlashWriteEnable(1);  
#ifdef SPIGPIO    
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x01);
    SendByteSPIGPIO(reg1);
    SendByteSPIGPIO(reg2);
    SetSPIFlashCS(1);
#else
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x01);
	SendByteSPIS3c2440Controller(reg1);
	SendByteSPIS3c2440Controller(reg2);
	SetSPIFlashCS(1);
#endif
    SPIFlashWaitWhenBusy();
}

/**********************************************************************************
  * @brief       : 	清除SPI Flash的寄存器保护
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SPIFlashClearProtectForStatusReg(void)
{
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(1<<7);
    reg2 &= ~(1<<0);

    SPIFlashWriteStatusReg(reg1, reg2);
}

/**********************************************************************************
  * @brief       : 	清除SPI Flash的数据保护
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SPIFlashClearProtectForData(void)
{
    /* cmp=0,bp2,1,0=0b000 */
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(7<<2);
    reg2 &= ~(1<<6);

    SPIFlashWriteStatusReg(reg1, reg2);
}

/**********************************************************************************
  * @brief       : 	擦除SPI Flash的数据，固定擦除4K内容
  * @param[in]   : 	addr	擦除的首地址
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void EraseSPIFlashSector(unsigned int addr)
{
    SPIFlashWriteEnable(1);  
#ifdef SPIGPIO
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x20);
    SendSPIFlashAddr(addr);
    SetSPIFlashCS(1);
#else
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x20);
	SendSPIFlashAddr(addr);
	SetSPIFlashCS(1);
#endif
    SPIFlashWaitWhenBusy();
}

/**********************************************************************************
  * @brief       : 	向SPI Flash写入数据
  * @param[in]   : 	addr	写入的首地址
  					buf		待写入的数据缓冲区地址
  					len		待写入的数据长度
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void ProgramSPIFlash(unsigned int addr, unsigned char *buf, int len)
{
    int i;
#ifdef SPIGPIO   
    SPIFlashWriteEnable(1);  

    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x02);
    SendSPIFlashAddr(addr);

    for (i = 0; i < len; i++)
        SendByteSPIGPIO(buf[i]);
    
    SetSPIFlashCS(1);
#else
	SPIFlashWriteEnable(1);  

	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x02);
	SendSPIFlashAddr(addr);

	for (i = 0; i < len; i++)
		SendByteSPIS3c2440Controller(buf[i]);

	SetSPIFlashCS(1);
#endif

    SPIFlashWaitWhenBusy();
    
}

/**********************************************************************************
  * @brief       : 	向SPI Flash读取数据
  * @param[in]   : 	addr	读取的首地址
  					buf		读取的数据存放的缓冲区地址
  					len		待读取的数据长度
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void ReadSPIFlash(unsigned int addr, unsigned char *buf, int len)
{
    int i;
#ifdef SPIGPIO    
    SetSPIFlashCS(0);
    SendByteSPIGPIO(0x03);
    SendSPIFlashAddr(addr);
    for (i = 0; i < len; i++)
        buf[i] = RecvByteSPIGPIO();

    SetSPIFlashCS(1); 
#else
	SetSPIFlashCS(0);
	SendByteSPIS3c2440Controller(0x03);
	SendSPIFlashAddr(addr);
	for (i = 0; i < len; i++)
		buf[i] = RecvByteSPIS3c2440Controller();

	SetSPIFlashCS(1); 

#endif
}

/**********************************************************************************
  * @brief       : 	初始化SPI Flash
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitSPIFlash(void)
{
    SPIFlashClearProtectForStatusReg();
    SPIFlashClearProtectForData();
}

/**********************************************************************************
  * @brief       : 	测试用SPI Flash写入函数
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void DoWriteSPIFLASH(void)
{
	unsigned int addr;
	unsigned char str[100];
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets((char *)str);

	printf("writing ...\n\r");
	ProgramSPIFlash(addr, str, strlen((char *)str)+1);
}

/**********************************************************************************
  * @brief       : 	测试用SPI Flash读取函数
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void DoReadSPIFLASH(void)
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

	ReadSPIFlash(addr, buf, 64);
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

/**********************************************************************************
  * @brief       : 	测试用SPI Flash擦除函数
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void DoEraseSPIFLASH(void)
{
	unsigned int addr;
	
	/* 获得地址 */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");
	EraseSPIFlashSector(addr);
}

