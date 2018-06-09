#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

void ReadSPIFlashID(int *pMID, int *pDID);
void InitSPIFlash(void);
void EraseSPIFlashSector(unsigned int addr);
void ProgramSPIFlash(unsigned int addr, unsigned char *buf, int len);
void ReadSPIFlash(unsigned int addr, unsigned char *buf, int len);
void DoWriteSPIFLASH(void);
void DoReadSPIFLASH(void);
void DoEraseSPIFLASH(void);

#endif

