void delay(volatile int d)
{
    while(d--);
}

int led_on(volatile in which)
{
    unsigned int *pGPFCON = (unsigned int *)0x56000050;
    unsigned int *pGPFDAT = (unsigned int *)0x56000054;

    if (which == 4) {
        /*配置GPF4为输出引脚*/
        *pGPFCON = 0x100;
    }else if(which == 5) {
        /*配置GPF5为输出引脚*/
        *pGPFCON = 0x400;
    }
    
    /*设置GPF4输出高电平,0*/
    *pGPFDAT = 0;

    return 0;
}
