#include "s3c2440_soc.h"

void key_eint_irq (int irq);
typedef void (*irq_func)(int);

static irq_func irq_array[32];
/**
 * SRCPND 用来显示哪个中断产生了，需要清楚对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 *
 *
 * INTMASK 用来屏蔽中断，1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 *
 * INTPND用来显示当前优先级最高的，正在发生的中断
 * bit0-eint0
 * bit2-eint2
 * bit5-bit8_23
 *
 * INTOFFSET:用来显示INTPND中哪一位被设置为1
 * */


/**
 * 初始化中断控制器 
 * */
void interrupt_init(void)
{
    INTMSK &= ~((1 << 0) | (1 << 2) | (1 << 5));
}

/**
 * 注册中断
 * */
void register_irq (int irq, irq_func fp)
{
    irq_array[irq] = fp;
    INTMSK &= ~(1 << irq);
}

/**
 *  初始化按键，设为中断源
 * */
void key_eint_init(void)
{
    /* 配置GPIO为中断引脚*/
    GPFCON &= ~((3 << 0) | (3 << 4)); 
    GPFCON |= ((2 << 0) | (2 << 4)); // S2,S3被配置为中断引脚
    
    GPGCON &= ~((3 << 6) | (3 << 22)); 
    GPGCON |= ((2 << 6) | (2 << 22)); // /S4,S5被配置为中断引脚
    

    /* 设置中断出发方式： 双边沿触发 */
    EXTINT0 |= ((7 << 0) | (7 << 8));  // S2, S3
    EXTINT1 |= (7 << 12);            // S4
    EXTINT2 |= (7 << 12);            // S5

    /* 设置EINTMASK使能 eint11, 19*/
    EINTMASK &= ~((1 << 11) | (1 << 19));

    register_irq (0, key_eint_irq);
    register_irq (2, key_eint_irq);
    register_irq (5, key_eint_irq);
}

/**
 * 读EINTPEND分辨哪个EINT产生(eint4~23)
 * 清除中断时，写EINTPEND的相应位
 * */
void key_eint_irq (int irq)
{
    unsigned int val = EINTPEND;
    unsigned int val1 = GPFDAT;
    unsigned int val2 = GPGDAT;

    if (irq == 0) {  //eint 0: s2控制D12
        if (val1 & (1 << 0)) { // S2--->gpf6
            // 松开
            GPFDAT |= (1 << 6);
        } else {
            // 按下
            GPFDAT &= ~(1 << 6);
        }
    } else if (irq == 2) { // eint2:s3控制D11
       if (val1 & (1 << 2)) {
           // 松开
           GPFDAT |= (1 << 5);
       } else {
           // 按下
           GPFDAT &= ~(1 << 5);
       } 
    } else if (irq == 5) { //eint8_23,eint11--s4控制D10，eint19---s5控制所有LED
        if (val & (1 << 11)) { // eint11
            if (val2 & (1 << 3)) { //s4--->gpf4
                // 松开
                GPFDAT |= (1 << 4);
            } else {
                // 按下
                GPFDAT &= ~(1 << 4);
            }
        } else if (1 << 19) { // eint19
            if (val2 & (1 << 11)) {
                // 松开,熄灭所有LED
                GPFDAT |= ((1 << 4) | (1 << 5) | (1 << 6));
            } else {
                // 按下，点亮所有LED
                GPFDAT &= ~((1 << 4) | (1 << 5) | (1 << 6));
            }
        }
    }
    EINTPEND = val;
}

void handle_irq_c (void)
{
    // 分辨中断源
    int bit = INTOFFSET;

    puts("handle irq c =");
    putchar(bit);
    puts("handle end\n");

#if 0
    // 调用对应的处理函数,eint0,2 eint8_23
    if (bit == 0 || bit == 2 || bit == 5) {
        key_eint_irq(bit); // 处理中断，清中断源EINTPEND
    }
#endif

    // 调用对应的处理函数
    irq_array[bit](bit);

    // 清中断：从源头开始清
    SRCPND = (1 << bit);
    INTPND = (1 << bit);
}
