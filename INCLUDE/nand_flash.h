
#ifndef _NAND_FLASH_H
#define _NAND_FLASH_H

void InitNand(void);
void ReadNand(unsigned int addr, unsigned char *buf, unsigned int len);
void NandWrite(unsigned int addr, unsigned char *buf, unsigned int len);
int NandErase(unsigned int addr, unsigned int len);


void PrintNandChipID(void);
void DoReadNandFlash(void);
void DoEraseNandFlash(void);
void DoWriteNandFlash(void);

#endif /* _NAND_FLASH_H */

