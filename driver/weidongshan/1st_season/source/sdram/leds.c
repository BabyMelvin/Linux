#define GPFCON    (*(volatile unsigned long *)0x56000050)
#define GPFDAT    (*(volatile unsigned long *)0x56000054)

#define GPF4_out   (1<<(4*2))
#define GP5_out    (1<<(5*2))
#define GP6_out    (1<<(6*2))

void wait(volatile unsigned long dly)
{
    for(;dly > 0; dly--);
}

int main(void)
{
    unsigned long i = 0;

    GPFCON = GPF4_out | GP5_out | GP6_out;
    while (1) {
        wait (30000);
        GPFDAT = (~(i << 4));
        if (++i == 8)
            i = 0;

    }

    return 0;
}
