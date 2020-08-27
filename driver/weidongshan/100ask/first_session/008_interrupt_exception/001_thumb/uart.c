#include "s3c2440_soc.h"

/* 115200,8n1 ->8:有效数据,n:无校验位,1:停止位*/
void uart0_init()
{
    /* 1.设置引脚用于串口 */
    // 查询电路图 GPH2,3 用于TXD0,RxD0
    GPHCON &= ~((3 << 4) | (3 << 6));
    GPHCON |= ((2 << 4) | (2 << 6));
    GPHUP &= ~((1 << 2) | (1 << 3)); // 使能内部上拉

    /* 2.设置波特率
     * UBRDIVn = (int)(UART clock/(buad rate x 16) - 1)
     *  UART clock = 50 M
     *  UBRDIVn = (int)(5000000 / (115200 * 16) -1） = 26
     * */
    UCON0 = 0x00000005; /*PCLK, 中断/查询模式 */
    UBRDIV0 = 26;

    /* 3. 设置数据格式*/
    ULCON0 = 0x00000003; /* 8n1:8个数据位，无校验位, 1个停止位*/

    /* 4. */
}


int putchar (int c)
{
    /* UTRSTAT 0*/
    /*UTXH0*/
    while (!(UTRSTAT0 & (1 << 2)));

    UTXH0 = (unsigned char)c;
}

int getchar (void)
{
    while (!(UTRSTAT0 & (1 << 0)));

    return URXH0;
}

int puts(const char *s)
{
    while (*s) {
        putchar (*s);
        s ++;
    }
}
