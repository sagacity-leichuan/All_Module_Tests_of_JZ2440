
#ifndef _NAND_FLASH_H
#define _NAND_FLASH_H

void nand_init(void);
void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);

void nand_chip_id(void);
void do_read_nand_flash(void);
void do_erase_nand_flash(void);
void do_write_nand_flash(void);

#endif /* _NAND_FLASH_H */

