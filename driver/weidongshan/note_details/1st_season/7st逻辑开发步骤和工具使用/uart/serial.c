#include "s3c24xx.h"
#include "serial.h"

#define TXD0_READY      (1<<2)
#define RXD0_READY      (1)

#define PCLK            50000000 // init.c 中的clock_init函数设置PCLK为50MHz
#define UART_CLK        PCLK     // UART0的时钟设置为PCLK
#define UART_BAUD_RATE  115200   // 波特率
#define UART_BRD        ((UART_CLK / (UART_BAUD_RATE * 16)) - 1)

/*
 * 初始化UART0
 * 115200,8IN1,无流控
 * */
void uart0_init(void)
{
    GPHCON  |= 0xa0;    // GPH2，GPH3用作TXD0，RXD0
    GPHUP   =  0xc0;    // GPH2, GPH3内部上拉

    ULCON0 = 0x03;      // 8IN1(8个数据位，无校验位，1个停止位)
    UCON0  = 0x05;      // 查询方式,URAT时钟源为PCLK
    UFCON0 = 0x00;      // 不适用FIFO
    UMCON0 = 0x00;      // 不适用流控
    UBRDIV0 = UART_BRD; // 波特率为115200
}

/*
 * 发送一个字符
 * */
void putc(unsigned char c)
{
    /* 等待，直到发送缓冲区中的数据已经全部发送出去 */
    while(!(UTRSTAT0 & TXD0_READY));

    /* 向UTXH0寄存器中写入数据，URAT即自动将它发送出去 */
    UTXH0 = c;
}

/*
 * 接收字符
 * */
unsigned char getc(void)
{
    /* 等待，直到接收数据缓冲区中的有效数据 */
    while(!(UTRSTAT0 & RXD0_READY));

    /* 直到读取 URXH0 寄存器，即可获得接收到的数据 */
    return URXH0;
}

/*
 * 判断一个字符是否是数字
 * */
int is_digit(unsigned char c)
{
    if (c >= '0'  && c <= '9')
        return 1;
    else
        return 0;
}

/*
 * 判断一个字符是否是英文字母
 * */
int is_letter(unsigned char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if(c >='A' && c <= 'Z')
        return 1;
    else
        return 0;
}
