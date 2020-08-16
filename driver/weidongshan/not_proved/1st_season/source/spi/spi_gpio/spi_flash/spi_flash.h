#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

void SPI_flash_read_id(int *p_mid, int *p_did);
void SPI_flash_init(void);
void SPI_flash_erase_sector(unsigned int addr);
void SPI_flash_program(unsigned int addr, unsigned char *buf, int len);
void SPI_flash_read(unsigned int addr, unsigned char *buf, int len);
#endif
