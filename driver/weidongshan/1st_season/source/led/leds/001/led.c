void delay(volatile int d)
{
    while(d--);
}

int main(void)
{
    volatile unsigned int *pGPFCON = (unsigned int *)0x56000050;
    volatile unsigned int *pGPFDAT = (unsigned int *)0x56000054;

    int val = 0; /*val: 0b000, 0b111*/
    int tmp;

    /* 配置4/5/6为输出引脚 */
    *pGPFCON &= ~((3<<4 * 2) | (3<<5 * 2) | (3<<6 * 2));
    *pGPFCON |= ((1<<4 * 2) | (1<< 5 * 2) | (1<<6 * 2));

    /* 循环点亮 */
    while (1) {
        tmp = ~val;
        tmp &= 7;
        
        *pGPFDAT &= ~(7<<4);
        *pGPFDAT |= (tmp<<4);
        delay(100000);
        val ++;

        if (val == 8)
            val = 0;
    }

    return 0;
}
