#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"
#include "nand_flash.h"
#include "led.h"

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

    //nor_flash_test();
    //nand_init();
    //nand_flash_test();
    
    return 0;
}
