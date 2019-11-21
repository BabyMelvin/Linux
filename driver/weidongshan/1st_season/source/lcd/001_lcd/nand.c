#include "nand.h"

static S3C2410_NAND * s3c2410nand = (S3C2410_NAND *)0x4e000000;
static S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;
static t_nand_chip nand_chip;

/* NAND Flash操作的总入口, 它们将调用S3C2410或S3C2440的相应函数 */
static void nand_reset(void);
static void wait_idle(void);
static void nand_select_chip(void);
static void nand_deselect_chip(void);
static void write_cmd(int cmd);
static void write_addr(unsigned int addr);
static unsigned char read_data(void);

/* S3C2410的NAND Flash处理函数 */
static void s3c2410_nand_reset(void);
static void s3c2410_wait_idle(void);
static void s3c2410_nand_select_chip(void);
static void s3c2410_nand_deselect_chip(void);
static void s3c2410_write_cmd(int cmd);
static void s3c2410_write_addr(unsigned int addr);
static unsigned char s3c2410_read_data();

/* S3C2440的NAND Flash处理函数 */
static void s3c2440_nand_reset(void);
static void s3c2440_wait_idle(void);
static void s3c2440_nand_select_chip(void);
static void s3c2440_nand_deselect_chip(void);
static void s3c2440_write_cmd(int cmd);
static void s3c2440_write_addr(unsigned int addr);
static unsigned char s3c2440_read_data(void);
/**************************************************************
 * S3C2410的NAND Flash操作函数
 **************************************************************/
/* 复位 */
static void s3c2410_nand_reset(void)
{
    s3c2410_nand_select_chip();
    s3c2410_write_cmd(0xff);  // 复位命令
    s3c2410_wait_idle();
    s3c2410_nand_deselect_chip();
}

/* 等待NAND Flash就绪 */
static void s3c2410_wait_idle(void)
{
    int i;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFSTAT;
    while(!(*p & BUSY))
        for(i=0; i<10; i++);
}

/* 发出片选信号 */
static void s3c2410_nand_select_chip(void)
{
    int i;
    s3c2410nand->NFCONF &= ~(1<<11);
    for(i=0; i<10; i++);    
}

/* 取消片选信号 */
static void s3c2410_nand_deselect_chip(void)
{
    s3c2410nand->NFCONF |= (1<<11);
}

/* 发出命令 */
static void s3c2410_write_cmd(int cmd)
{
    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFCMD;
    *p = cmd;
}

/* 发出地址 */
static void s3c2410_write_addr(unsigned int addr)
{
    int i;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFADDR;
    
    *p = addr & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 9) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 17) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 25) & 0xff;
    for(i=0; i<10; i++);
}

/* 读取数据 */
static unsigned char s3c2410_read_data(void)
{
    volatile unsigned char *p = (volatile unsigned char *)&s3c2410nand->NFDATA;
    return *p;
}

/**************************************************************
 * S3C2440的NAND Flash操作函数
 **************************************************************/
/* 复位 */
static void s3c2440_nand_reset(void)
{
    s3c2440_nand_select_chip();
    s3c2440_write_cmd(0xff);  // 复位命令
    s3c2440_wait_idle();
    s3c2440_nand_deselect_chip();
}

/* 等待NAND Flash就绪 */
static void s3c2440_wait_idle(void)
{
    int i;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFSTAT;
    while(!(*p & BUSY))
        for(i=0; i<10; i++);
}

/* 发出片选信号 */
static void s3c2440_nand_select_chip(void)
{
    int i;
    s3c2440nand->NFCONT &= ~(1<<1);
    for(i=0; i<10; i++);    
}

/* 取消片选信号 */
static void s3c2440_nand_deselect_chip(void)
{
    s3c2440nand->NFCONT |= (1<<1);
}
 /* 发出命令 */
static void s3c2440_write_cmd(int cmd)
{
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFCMD;
    *p = cmd;
}

/* 发出地址 */
static void s3c2440_write_addr(unsigned int addr)
{
    int i;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
    
    *p = addr & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 9) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 17) & 0xff;
    for(i=0; i<10; i++);
    *p = (addr >> 25) & 0xff;
    for(i=0; i<10; i++);
}

static void s3c2440_write_addr_lp(unsigned int addr)
{
	int i;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
	int col, page;

	col = addr & NAND_BLOCK_MASK_LP;
	page = addr / NAND_SECTOR_SIZE_LP;

	*p = col & 0xff;			/* Column Address A0~A7 */
	for(i=0; i<10; i++);		
	*p = (col >> 8) & 0x0f; 	/* Column Address A8~A11 */
	for(i=0; i<10; i++);
	*p = page & 0xff;			/* Row Address A12~A19 */
	for(i=0; i<10; i++);
	*p = (page >> 8) & 0xff;	/* Row Address A20~A27 */
	for(i=0; i<10; i++);
	*p = (page >> 16) & 0x03;	/* Row Address A28~A29 */
	for(i=0; i<10; i++);
}

/* 读取数据 */
static unsigned char s3c2440_read_data(void)
{
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFDATA;
    return *p;
}

/**************************************************************
 * NAND Flash操作的总入口
 **************************************************************/
/* 在第一次使用NAND Flash前，复位一下NAND Flash */
static void nand_reset(void)
{
    nand_chip.nand_reset();
}

static void wait_idle(void)
{
    nand_chip.wait_idle();
}

static void nand_select_chip(void)
{
    int i;
    nand_chip.nand_select_chip();
    for(i=0; i<10; i++);
}

static void nand_deselect_chip(void)
{
    nand_chip.nand_deselect_chip();
}

static void write_cmd(int cmd)
{
    nand_chip.write_cmd(cmd);
}
static void write_addr(unsigned int addr)
{
    nand_chip.write_addr(addr);
}

static unsigned char read_data(void)
{
    return nand_chip.read_data();
}

/* 初始化NAND Flash */
void nand_init(void)
{
#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0
	 /* 判断是S3C2410还是S3C2440 */
    if ((GSTATUS1 == 0x32410000) || (GSTATUS1 == 0x32410002)) {
        nand_chip.nand_reset         = s3c2410_nand_reset;
        nand_chip.wait_idle          = s3c2410_wait_idle;
        nand_chip.nand_select_chip   = s3c2410_nand_select_chip;
        nand_chip.nand_deselect_chip = s3c2410_nand_deselect_chip;
        nand_chip.write_cmd          = s3c2410_write_cmd;
        nand_chip.write_addr         = s3c2410_write_addr;
        nand_chip.read_data          = s3c2410_read_data;

		/* 使能NAND Flash控制器, 初始化ECC, 禁止片选, 设置时序 */
        s3c2410nand->NFCONF = (1<<15)|(1<<12)|(1<<11)|(TACLS<<8)|(TWRPH0<<4)|(TWRPH1<<0);
    } else {
        nand_chip.nand_reset         = s3c2440_nand_reset;
        nand_chip.wait_idle          = s3c2440_wait_idle;
        nand_chip.nand_select_chip   = s3c2440_nand_select_chip;
        nand_chip.nand_deselect_chip = s3c2440_nand_deselect_chip;
        nand_chip.write_cmd          = s3c2440_write_cmd;
#ifdef LARGER_NAND_PAGE
        nand_chip.write_addr         = s3c2440_write_addr_lp;
#else
		nand_chip.write_addr		 = s3c2440_write_addr;
#endif
        nand_chip.read_data          = s3c2440_read_data;

		/* 设置时序 */
        s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
        /* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
        s3c2440nand->NFCONT = (1<<4)|(1<<1)|(1<<0);
    }
	
	 /* 复位NAND Flash */
    nand_reset();
}
