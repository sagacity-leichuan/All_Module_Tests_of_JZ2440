/****************************************************************************************************
  * @brief      : 	JZ2440v2开发板oled显示屏代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
*****************************************************************************************************/

#include "oledfont.h"
#include "spi_gpio_simulate.h"
#include "spi_s3c2440_controller.h"
#include "function.h"
#include "s3c2440_soc.h"
#include "oled.h"

/**********************************************************************************
  * @brief       : 	设置oled的DC状态
  * @param[in]   : 	val	待设置的值
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SetOLEDDC(char val)
{
    if (val)
        GPGDAT |= (1<<4);
    else
        GPGDAT &= ~(1<<4);
}

/**********************************************************************************
  * @brief       : 	设置oled的CS状态
  * @param[in]   : 	val	待设置的值
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SetOLEDCS(char val)
{
    if (val)
        GPFDAT |= (1<<1);
    else
        GPFDAT &= ~(1<<1);
}

/**********************************************************************************
  * @brief       : 	向oled写入DC命令
  * @param[in]   : 	cmd		待写入的命令
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
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

/**********************************************************************************
  * @brief       : 	向oled写入数据
  * @param[in]   : 	dat	待写入的数据
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
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

/**********************************************************************************
  * @brief       : 	设置oled页地址模式
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SetOLEDPageAddrMode(void)
{
    WriteOLEDCmd(0x20);
    WriteOLEDCmd(0x02);
}

/**********************************************************************************
  * @brief       : 	设置oled指针位置
  * @param[in]   : 	page	指针所在的页
  					col		指针所在的列
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
static void SetOLEDPos(int page, int col)
{
    WriteOLEDCmd(0xB0 + page); /* page address */

    WriteOLEDCmd(col & 0xf);   /* Lower Column Start Address */
    WriteOLEDCmd(0x10 + (col >> 4));   /* Lower Higher Start Address */
}

/**********************************************************************************
  * @brief       : 	对oled清屏操作
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
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

/**********************************************************************************
  * @brief       : 	初始化OLED
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
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

/**********************************************************************************
  * @brief		 :	在OLED显示屏上显示特定字符
  * @param[in]	 :	page	字符显示的页
  					col		字符显示的列
  					c		待显示的字符
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	page: 0-7
					col : 0-127
					字符: 8x16象素
***********************************************************************************/
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

 /**********************************************************************************
  * @brief		 :	在OLED显示屏上显示特定字符串
  * @param[in]	 :	page	字符串显示的页
  					col		字符串显示的列
  					str		待显示的字符串首地址
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	page: 0-7
					col : 0-127
					字符: 8x16象素
***********************************************************************************/
void PrintOLEDString(int page, int col, char *str)
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

