void delay(volatile int d)
{
    while(d--);
}

int main(void)
{
    int val1,val2 = 0; /*val: 0b000, 0b111*/

    /* 配置4/5/6为输出引脚 */
    GPFCON &= ~((3<<4 * 2) | (3<<5 * 2) | (3<<6 * 2));
    GPFCON |= ((1<<4 * 2) | (1<< 5 * 2) | (1<<6 * 2));

    /*配置3个按键引脚为输入引脚
     * GPF0(S2),GPF2(S3),GPG3(S4)
     * */
    GPFCON &= ~((3<<0) | (3<<4)); /*GPF0,2*/
    GPGCON &= ~((3<<6)); /*GPG3*/

    /* 循环点亮 */
    while (1) {
        val1 = GPFDAT;
        val2 = GPGDAT;
        if(val1 & (1 << 0)) { /*s2 --> gpf6*/
            /*松开*/
            GPFDAT |= (1<<6);
        }else {
            /*按下*/
            GPFDAT &= ~(1<<6);
        }

        if(val1 & (1<<2)) {
            /*松开*/
            GPFDAT |= (1<<5);
        }else{
            /*按下*/
            GPFDAT &= ~(1<<5)
        }

        if(val2 & (1<<3)) {
            /*松开*/
            GPFDAT |= (1<<4);
        }else{
            /*按下*/
            GPFDAT &= ~(1<<4);
        }
    }

    return 0;
}
