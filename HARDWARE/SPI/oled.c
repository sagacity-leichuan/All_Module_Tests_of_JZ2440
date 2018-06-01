#include "oledfont.h"
#include "spi_gpio_simulate.h"
#include "spi_s3c2440_controller.h"
#include "function.h"
#include "s3c2440_soc.h"
#include "oled.h"


static void SetOLEDDC(char val)
{
    if (val)
        GPGDAT |= (1<<4);
    else
        GPGDAT &= ~(1<<4);
}

static void SetOLEDCS(char val)
{
    if (val)
        GPFDAT |= (1<<1);
    else
        GPFDAT &= ~(1<<1);
}

static void WriteOLEDCmd(unsigned char cmd)
{


    SetOLEDDC(0); /* command */
    SetOLEDCS(0); /* select OLED */
#ifdef SPIGPIO

    SendByteSPIGPIO(cmd);

#else

	SendByteSPIS3c2440Controller(cmd);

#endif

    SetOLEDCS(1); /* de-select OLED */
    SetOLEDDC(1); /*  */
    
}

static void WriteOLEDDat(unsigned char dat)
{
    SetOLEDDC(1); /* data */
    SetOLEDCS(0); /* select OLED */

#ifdef SPIGPIO
	
	SendByteSPIGPIO(dat);
	
#else
	
	SendByteSPIS3c2440Controller(dat);
	
#endif


    SetOLEDCS(1); /* de-select OLED */
    SetOLEDDC(1); /*  */
}

static void SetOLEDPageAddrMode(void)
{
    WriteOLEDCmd(0x20);
    WriteOLEDCmd(0x02);
}

static void SetOLEDPos(int page, int col)
{
    WriteOLEDCmd(0xB0 + page); /* page address */

    WriteOLEDCmd(col & 0xf);   /* Lower Column Start Address */
    WriteOLEDCmd(0x10 + (col >> 4));   /* Lower Higher Start Address */
}


static void ClearOLED(void)
{
    int page, i;
    for (page = 0; page < 8; page ++)
    {
        SetOLEDPos(page, 0);
        for (i = 0; i < 128; i++)
            WriteOLEDDat(0);
    }
}

void InitOLED(void)
{
    /* 向OLED发命令以初始化 */
    WriteOLEDCmd(0xAE); /*display off*/ 
    WriteOLEDCmd(0x00); /*set lower column address*/ 
    WriteOLEDCmd(0x10); /*set higher column address*/ 
    WriteOLEDCmd(0x40); /*set display start line*/ 
    WriteOLEDCmd(0xB0); /*set page address*/ 
    WriteOLEDCmd(0x81); /*contract control*/ 
    WriteOLEDCmd(0x66); /*128*/ 
    WriteOLEDCmd(0xA1); /*set segment remap*/ 
    WriteOLEDCmd(0xA6); /*normal / reverse*/ 
    WriteOLEDCmd(0xA8); /*multiplex ratio*/ 
    WriteOLEDCmd(0x3F); /*duty = 1/64*/ 
    WriteOLEDCmd(0xC8); /*Com scan direction*/ 
    WriteOLEDCmd(0xD3); /*set display offset*/ 
    WriteOLEDCmd(0x00); 
    WriteOLEDCmd(0xD5); /*set osc division*/ 
    WriteOLEDCmd(0x80); 
    WriteOLEDCmd(0xD9); /*set pre-charge period*/ 
    WriteOLEDCmd(0x1f); 
    WriteOLEDCmd(0xDA); /*set COM pins*/ 
    WriteOLEDCmd(0x12); 
    WriteOLEDCmd(0xdb); /*set vcomh*/ 
    WriteOLEDCmd(0x30); 
    WriteOLEDCmd(0x8d); /*set charge pump enable*/ 
    WriteOLEDCmd(0x14); 

    SetOLEDPageAddrMode();

    ClearOLED();
    
    WriteOLEDCmd(0xAF); /*display ON*/    
}


/* page: 0-7
 * col : 0-127
 * 字符: 8x16象素
 */
void PutOLEDChar(int page, int col, char c)
{
    int i = 0;
    /* 得到字模 */
    const unsigned char *dots = oled_asc2_8x16[c - ' '];

    /* 发给OLED */
    SetOLEDPos(page, col);
    /* 发出8字节数据 */
    for (i = 0; i < 8; i++)
        WriteOLEDDat(dots[i]);

    SetOLEDPos(page+1, col);
    /* 发出8字节数据 */
    for (i = 0; i < 8; i++)
        WriteOLEDDat(dots[i+8]);

}


/* page: 0-7
 * col : 0-127
 * 字符: 8x16象素
 */
void PrintOLED(int page, int col, char *str)
{
    int i = 0;
    while (str[i])
    {
        PutOLEDChar(page, col, str[i]);
        col += 8;
        if (col > 127)
        {
            col = 0;
            page += 2;
        }
        i++;
    }
}

