#ifndef __NADN_H__
#define __NADN_H__

#define MAX_NUM_NAND
#define LARGE_NAND_PAGE

#define GSATUS1     (*(volatile unsigned int *)0x5600000B0)
#define NAND_BUSY        1 
#define NAND_SECTOR_SIZE   512
#define NAND_BLOCK_MASK    (NAND_SECTOR_SIZE - 1)

#define NAND_SECTOR_SIZE_LP    2048
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

typedef unsigned int S3C24X0_REG32;


typedef struct {
    int8_t *name;
    void(*nand_reset)(void);
    void(*wait_idle)(void);
    void(*nand_select_chip)(void);
    void(*nand_deselect_chip)(void);
    void (*write_cmd)(int cmd);
    void (*write_addr)(unsigned int addr);
    unsigned char (*read_data)(void);
} t_nand_chip, *p_nand_chip;

void nand_init(void);
void nand_reset(void);
void nand_wait_idle(void);
void nand_select_chip(void);
void nand_deselect_chip(void);
void nand_write_cmd(int cmd);
void nand_write_addr(unsigned int addr);

#endif
