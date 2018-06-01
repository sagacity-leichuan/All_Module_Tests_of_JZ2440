#ifndef _SPI_GPIO_SIMULATE_H
#define _SPI_GPIO_SIMULATE_H

void InitSPIGPIO(void);
void SendByteSPIGPIO(unsigned char val);
unsigned char RecvByteSPIGPIO(void);

#endif

