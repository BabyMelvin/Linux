#include "s3c2440_soc.h"
#include "uart.h"

void delay(volatile int d)
{
    //int i = 0;
    //for (i = 0; i < 10; i++)
        while (d--);
}

#ifdef LED_PARAMS
int led_on(int which)
{
    unsigned int *p_GPFCON = (unsigned int *)0x56000050;
    unsigned int *p_GPFDAT = (unsigned int *)0x56000054;

    if (which == 4) {
        /* 配置GPF4为输出引脚*/
        *p_GPFCON = 0x100;
    } else {
        /* 配置GPF5为输出引脚*/
        *p_GPFCON = 0x400;
    }

    /* 设置GPF4/GPF5输出0 */
    *p_GPFDAT = 0;

    return 0;
}
#endif

int led_init (void)
{
    puts("led init \n\r");

    //将4/5/6配置为输出引脚
    GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
    GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));

    //*p_GPFDAT &= ~((3 << 8) | (3 << 10) | (3 << 12));
    GPFDAT = ~((1 << 6) | (1 << 5) | (1 << 4));
    
}

int led_test()
{
    volatile unsigned int *p_GPFCON = (volatile unsigned int *)0x56000050;
    volatile unsigned int *p_GPFDAT = (volatile unsigned int *)0x56000054;

    int val = 0, tmp;

    //将4/5/6配置为输出引脚
    *p_GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
    *p_GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));

    while (1) {
        tmp = ~(1 << val);
        *p_GPFDAT &= ~(7 << 4);
        *p_GPFDAT |= (tmp << 4);
        delay(100000);
        val ++;
        if (val == 3) val = 0;
    }

    return 0;
}
