#include "s3c2440_soc.h"

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

int main()
{
    volatile unsigned int *p_GPFCON = (volatile unsigned int *)0x56000050;
    volatile unsigned int *p_GPFDAT = (volatile unsigned int *)0x56000054;

    int val1 = 0, val2, tmp;

    //将4/5/6配置为输出引脚
    *p_GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
    *p_GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));

    /**
     * 配置3个按键引脚为输入引脚
     *  GPF0(S2) GPF2(S3) GPF3(S4)
     * */
    GPFCON &= ~((3 << 0)|(3 << 4)); // gpf0,2
    GPGCON &= ~((3<<6)); //gpg3

    while (1) {
        val1 = GPFDAT;
        val2 = GPGDAT;

        if (val1 & (1<<0)) { /*s2 -->gpf6*/
            /*松开*/
            GPFDAT |= (1<<6);
        } else {
            /*按下*/
            GPFDAT &= ~(1<<6);
        }

        if (val1 & (1<<2)) { /*s3 -->gpf5*/
            /*松开*/
            GPFDAT |= (1<<5);
        } else {
            /*按下*/
            GPFDAT &= ~(1<<5);
        }

        if (val2 & (1<<3)) { /*s4 -->gpf4*/
            /*松开*/
            GPFDAT |= (1<<4);
        } else {
            /*按下*/
            GPFDAT &= ~(1<<4);
        }

    }

    return 0;
}
