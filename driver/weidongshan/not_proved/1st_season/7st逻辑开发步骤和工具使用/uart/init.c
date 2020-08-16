/**
 * init.c：进行一些初始化
 * */

#include "s3c24xx.h"

void disable_watch_dog(void);
void clock_init(void);
void memsetup(void);
void copy_steppingstone_to_sdram(void);

/**
 * 关闭WATCHDOGS，否则CPU会不断重启
 * */

void disable_watch_dog(void)
{
    WTCON = 0;  //关闭看门狗很简单，往这个寄存器写0即可
}

/* 设置时钟：
 *      对MPLLCON寄存器，[19:12]为MDIV, [9:4]为PDIV,[1:0]为SDIV
 *      S3C2410:MPLL(FCLK) = (2 * m * Fin) / (p * 2^s)
 *      其中： m = MDIV + 8 , p = PDIV + 2, s = SDIV,开发板，Fin = 12MHz
 * 设置CLKDIVN
 *      分频比为：FCLK:HCLK:PCLK = 1:2:4 
 *      FCLK=200MHz, HCLK=100MHz, PCLK=50MHz
 * */

#define S3C2410_MPLL_200MHZ     ((0x5c << 12)||(0x04 << 4)||(0x00))
#define S3C2440_MPLL_200MHZ     ((0x5c << 12)||(0x01 << 4)||(0x02))

void clock_init(void)
{
    // LOCKTIME = 0X00FFFFFF;  //使用默认值即可
    CLKDIVN = 0x03;   //FCLK:HCLK:PCLK = 1:2:4, HDVIN = 1, PDVIN = 1

    /* 如果HDIVN非0， CPU的总线模式应该从“fast bus mode”变为"asynchronous bus mode"*/
__asm__(
        "mrc p15, 0, r1, c1, c0, 0\n"  /* 读出控制寄存器 */
        "orr r1,  r1, #0xc0000000\n"  /* 设置为"asynchronous bus mode"*/
        "mcr p15, 0, r1, c1, c0, 0\n"     /* 写入控制寄存器 */
        );

    /* 判断是S3C2410还是S3C2440 */
    if((GSTATUS1 == 0x32410000) || (GSTATUS1 == 0X32410002)) {
        MPLLCON = S3C2410_MPLL_200MHZ;
        /* 现在 FCLK = 200MHz， HCLK=100MHz, PCK=50MHz*/
    } else {
        MPLLCON = S3C2440_MPLL_200MHZ;
        /* 现在,FCL=200MHz, HCLK=100MHz, PCLK=50MHz*/
    }
}

/* 设置存储控制器以使用SRAM */
void memsetup(void)
{
    volatile unsigned long *p = (volatile unsigned long *)MEM_CTL_BASE;

    /* 这个函数之所以这样赋值，不是写在数组里，因为要生成"位置无关的代码"
     * 使得函数可以在被复制到SDRAM之前就可以在settingstone中运行
     * */
    
    // 存储控制器13个寄存器的值
    p[0] = 0x22011110; // BWSCON
    p[1] = 0x00000700; // BANKCON0
    p[2] = 0x00000700; // BANKCON1
    p[3] = 0x00000700; // BANKCON2
    p[4] = 0x00000700; // BANKCON3
    p[5] = 0x00000700; // BANKCON5
    p[6] = 0x00000700; // BANKCON6
    p[7] = 0x00000700; // BANKCON7
    p[8] = 0x00000700; // BANKCON8

    /*
     * Refresh,
     *  HCLK = 12MHz:   0x008C07A3,
     *  HCLK = 100MHz:  0x008C04F4
     * */
    p[9]  = 0x008C04F4;
    p[10] = 0x000000B1; // BANKSIZE
    p[11] = 0x008C04F4; // MRSRB6
    p[12] = 0x008C04F4; // MRSRB7
}

void copy_steppingstone_to_sram(void)
{
    unsigned int *pdwSrc = (unsigned int *)0;
    unsigned int *pwdDst = (unsigned int *)0x3000000;

    while (pdwSrc < (unsigned int *)4096) {
        *pwdDst++ = *pdwSrc++;
    }
}
