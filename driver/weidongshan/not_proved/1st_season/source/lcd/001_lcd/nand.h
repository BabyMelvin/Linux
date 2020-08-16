#ifndef __NAND_H__
#define __NAND_H__

#define LARGER_NAND PAGE

#define GSTATUS1    (*(volatile unsigned int *)0x56000080)
#define BUSY        1

#define NAND_SECTOR_SIZE    512
#define BAND_BLOCK_MASK     (NAND_SECTOR_SIZE -1)

#define NAND_SECTOR_SIZE_LP    2048
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

typedef unsigned int S3C24X0_REG32;

/* NAND FLASH (see S3C2410 manual chapter 6) */
typedef struct {
    S3C24X0_REG32   NFCONF;
    S3C24X0_REG32   NFCMD;
    S3C24X0_REG32   NFADDR;
    S3C24X0_REG32   NFDATA;
    S3C24X0_REG32   NFSTAT;
    S3C24X0_REG32   NFECC;
} S3C2410_NAND;

/* NAND FLASH (see S3C2440 manual chapter 6, www.100ask.net) */
typedef struct {
    S3C24X0_REG32   NFCONF;
    S3C24X0_REG32   NFCONT;
    S3C24X0_REG32   NFCMD;
    S3C24X0_REG32   NFADDR;
    S3C24X0_REG32   NFDATA;
    S3C24X0_REG32   NFMECCD0;
    S3C24X0_REG32   NFMECCD1;
    S3C24X0_REG32   NFSECCD;
    S3C24X0_REG32   NFSTAT;
    S3C24X0_REG32   NFESTAT0;
    S3C24X0_REG32   NFESTAT1;
    S3C24X0_REG32   NFMECC0;
    S3C24X0_REG32   NFMECC1;
    S3C24X0_REG32   NFSECC;
    S3C24X0_REG32   NFSBLK;
    S3C24X0_REG32   NFEBLK;
} S3C2440_NAND;

typedef struct {
    void (*nand_reset)(void);
    void (*wait_idle)(void);
    void (*nand_select_chip)(void);
    void (*nand_deselect_chip)(void);
    void (*write_cmd)(int cmd);
    void (*write_addr)(unsigned int addr);
    unsigned char (*read_data)(void);
}t_nand_chip;

/* 供外部调用的函数 */
void nand_init(void);
void nand_read(unsigned char *buf, unsigned long start_addr, int size);

#endif /* __NAND_H__ */
