void delay(volatile int d)
{
    while(d--);
}

int main(void)
{
    int val = 0; /*val: 0b000, 0b111*/
    int tmp;

    /* 配置4/5/6为输出引脚 */
    GPFCON &= ~((3<<4 * 2) | (3<<5 * 2) | (3<<6 * 2));
    GPFCON |= ((1<<4 * 2) | (1<< 5 * 2) | (1<<6 * 2));

    /* 循环点亮 */
    while (1) {
        tmp = ~val;
        tmp &= 7;
        
        GPFDAT &= ~(7<<4);
        GPFDAT |= (tmp<<4);
        delay(100000);
        val ++;

        if (val == 8)
            val = 0;
    }

    return 0;
}
