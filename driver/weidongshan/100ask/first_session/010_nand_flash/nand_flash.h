#ifndef __NAND_FLASH_H__
#define __NAND_FLASH_H__

void nand_init(void);
void nand_read(unsigned int addr, unsigned char *buf,
        unsigned int len);
void nand_flash_test(void);
#endif
