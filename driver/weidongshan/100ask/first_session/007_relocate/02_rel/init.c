#include "s3c2440_soc.h"
#include "init.h"
void sdram_init(void)
{
   BWSCON = 0x22000000;

    BANKCON6 = 0x18001;
    BANKCON7 = 0x18001;

    REFRESH  = 0x8404f5;

    BANKSIZE = 0xb1;

    MRSRB6   = 0x20;
    MRSRB7   = 0x20; 
}

/**
 * 测试程序  读写内存
 * */
int sdram_test(void)
{
   volatile unsigned char *p = (volatile unsigned char *)0x30000000;
    int i;

    // write sdram
    for (i = 0; i < 1000; i++)
        p[i] = 0x55;

    // read sdram
    for (i = 0; i < 1000; i++)
          if (p[i] != 0x55) return -1;
    
    return 0; 
}

