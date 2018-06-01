#include "s3c2440_soc.h"
#include "spi_gpio_simulate.h"

/* ÓÃGPIOÄ£ÄâSPI */

void InitSPIGPIO(void)
{
    /* GPF1 OLED_CSn output */
    GPFCON &= ~(3<<(1*2));
    GPFCON |= (1<<(1*2));
    GPFDAT |= (1<<1);

    /* GPG2 FLASH_CSn output
    * GPG4 OLED_DC   output
    * GPG5 SPIMISO   input
    * GPG6 SPIMOSI   output
    * GPG7 SPICLK    output
    */
    GPGCON &= ~((3<<(2*2)) | (3<<(4*2)) | (3<<(5*2)) | (3<<(6*2)) | (3<<(7*2)));
    GPGCON |= ((1<<(2*2)) | (1<<(4*2)) | (1<<(6*2)) | (1<<(7*2)));
    GPGDAT |= (1<<2);
}

static void SPISetCLK(char val)
{
    if (val)
        GPGDAT |= (1<<7);
    else
        GPGDAT &= ~(1<<7);
}

static void SPISetDO(char val)
{
    if (val)
        GPGDAT |= (1<<6);
    else
        GPGDAT &= ~(1<<6);
}

static char SPIGetDI(void)
{
    if (GPGDAT & (1<<5))
        return 1;
    else 
        return 0;
}

void SendByteSPIGPIO(unsigned char val)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        SPISetCLK(0);
        SPISetDO(val & 0x80);
        SPISetCLK(1);
        val <<= 1;
    }
    
}

unsigned char RecvByteSPIGPIO(void)
{
    int i;
    unsigned char val = 0;
    for (i = 0; i < 8; i++)
    {
        val <<= 1;
        SPISetCLK(0);
        if (SPIGetDI())
            val |= 1;
        SPISetCLK(1);
    }
    return val;    
}


