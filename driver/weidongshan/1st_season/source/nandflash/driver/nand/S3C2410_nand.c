#include "nand.h"

/* NAND FLASH (see S3C2410 manual chapter 6) */
typedef struct {
    S3C24X0_REG32   NFCONF;
    S3C24X0_REG32   NFCMD;
    S3C24X0_REG32   NFADDR;
    S3C24X0_REG32   NFDATA;
    S3C24X0_REG32   NFSTAT;
    S3C24X0_REG32   NFECC;
} S3C2410_NAND;

static S3C2410_NAND *s3c2410nand = (s3c2410_NAND *)0x4e000000;

static void s3c2410_nand_reset(void)
{
    s3c2410_nand_select_chip();

    s3c2410_write_cmd(0xff); // 复位命令
    s3c2410_wait_idle();

    s3c2410_nand_deselect_chip();
}

static void s3c2410_wait_idle(void)
{
    int i;
    volatile unsigned char *p = ()&s3c2410nand->NFSTAT;

    while(!(*p & BUSY))
        for(i = 0; i< 10; i++);
}

static void s3c2410_nand_select_chip(void)
{
    int i;
    s3c2410nand->NFCONF &= ~(1 << 11);
    for(i = 0; i< 10; i++);
}

static void s3c2410_nand_deselect_chip(void)
{
    s3c2410nand->NFCONF |= (1<<11);
}

/* 发送命令 */
static void s3c2410_nand_write_cmd(int cmd)
{
    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFADDR;
    *p = cmd;
}

/* 发送地址 */
static void s3c2410_nand_write_addr(unsigned int addr)
{
    int i;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFADDR;

    *p = addr & 0xff;
    for(i = 0; i < 10; i++);
    *p = (addr >> 9) & 0xff;
    for(i = 0; i < 10; i++);
    *p = (addr >> 17) & 0xff;
    for(i = 0; i < 10; i++);
    *p = (addr >> 25) & 0xff;
    for(i = 0; i < 10; i++);
}

/*读取数据*/
static unsigned char s3c2410_nand_read_data(void)
{
    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFDATA;

    return *p;
}

t_nand_chip s3c2410_nand_chip = {
     .name = "s3c2410_nand"
     .nand_reset = s3c2410_nand_reset,
     .nand_idle = s3c2410_wait_idle,
     .nand_select_chip = s3c2410_nand_select_chip,
     .nand_deselect_chip = s3c2410_nand_deselect_chip,
#ifdef LARGET_PAGE
     .write_cmd = s3c2410_nand_write_cmd_lp,
#else
     .write_cmd = s3c2410_nand_write_cmd,
#endif
     .write_addr = s3c2410_nand_read_data,
};
