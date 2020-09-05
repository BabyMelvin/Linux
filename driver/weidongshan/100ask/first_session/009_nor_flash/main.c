#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

char g_Char = 'A';
const char g_Char2 = 'B';
int g_A = 0;
int g_B;

int main (void)
{
    puts("main enter\n\r");
    led_init();
    //interrupt_init();  // 初始化中断控制器
    //key_eint_init();  // 初始化按键，设为中断源
    //timer_init();

    nor_flash_test();
#if 0
    while (1) {
        putchar(g_Char);
        g_Char++;
        delay(1000000);
    }
#endif
    
    return 0;
}
