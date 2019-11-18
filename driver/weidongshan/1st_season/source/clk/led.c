#include "s3c244_soc.h"

void delay(volatile int d)
{
    while (d--);
}

int main(void)
{
    int val = 0;
    int tmp;

    /* 设置GPFCON 让GPF4/5/6配置为输出引脚 */
    GPFCON &= ~((3<<8) | (3<<10) | (3<<12))
    GPFCON |=  ((1<<8) | (1<<10) | (1<<12))

    /*循环点灯*/
    while(1) {
        tmp = ~val;
        tmp &= 7;
        GPFDAT &= ~(7<<4);
        GPFDAT |=  (tmp<<4);
        
        delay(100000);
        val ++;

        if(val == 8)
            val = 0;
    }
}
