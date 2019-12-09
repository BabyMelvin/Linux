/*
 * lds.c: 循环点亮4个LED
 * 属于第二部分程序，此时已经开启MMU，使用虚拟地址
 * */

#define GPFCON   (*(volatile unsigned long *)0xA0000050)   //物理地址0x56000050
#define GPFDAT   (*(volatile unsigned long *)0xA0000054)   //物理地址0x56000054

#define GPF4_out    (1<<(4*2))
#define GPF5_out    (1<<(5*2))
#define GPF6_out    (1<<(6*2))

/*
 * wait函数加“static inline” 是有原因的
 * 这样可使得编译leds.c是，wait嵌入main中，编译得到只有一个main函数
 * 于是连接时，main函数的地址就是由连接文件指定的运行时装载地址
 * 
 * 而连接文件mmu.lds中，指定leds.o运行时地址就是装载地址为0xB4004000
 * 这样,head.S中的"ldr pc, =0xB4004000"就是跳去执行main函数
 * */
static inline void wait(unsigned long dly)
{
    for(; dly > 0; dly--);
}

int main(void)
{
    unsigned long i = 0;

    GPFCON = GPF4_out | GPF5_out | GPF6_out;  //LED 1, 2, 4对应的GPF4/5/6三个引脚设为输出

    while(1) {
        wait(3000);
        GPFDAT = (~(i << 4));   // 根据i的值，点亮LED1,2,4
        if(++i == 8)
            i = 0;
    }

    return 0;
}
