#ifndef _SPI_S3C2440_CONTROLLER_H
#define _SPI_S3C2440_CONTROLLER_H

void InitSPIS3c2440Controller(void);
void SendByteSPIS3c2440Controller(unsigned char val);
unsigned char RecvByteSPIS3c2440Controller(void);

#endif

