#include "s3c2440_soc.h"
#include "spi_gpio_simulate.h"
#include "spi_s3c2440_controller.h"
#include "function.h"


static void SetSPIFlashCS(char val)
{
    if (val)
        GPGDAT |= (1<<2);
    else
        GPGDAT &= ~(1<<2);
}

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

/* 
 * 
 */
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

static void SPIFlashWaitWhenBusy(void)
{
    while (SPIFlashReadStatusReg1() & 1);
}

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

static void SPIFlashClearProtectForStatusReg(void)
{
    unsigned char reg1, reg2;

    reg1 = SPIFlashReadStatusReg1();
    reg2 = SPIFlashReadStatusReg2();

    reg1 &= ~(1<<7);
    reg2 &= ~(1<<0);

    SPIFlashWriteStatusReg(reg1, reg2);
}


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

/* erase 4K */
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

/* program */
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


void InitSPIFlash(void)
{
    SPIFlashClearProtectForStatusReg();
    SPIFlashClearProtectForData();
}


