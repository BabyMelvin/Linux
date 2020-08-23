#include "s3c2440_soc.h"
#include "uart.h"
#include "my_printf.h"

int main (void)
{
    unsigned char c;

    uart0_init();

    puts("Hello, world!\n\r");

    my_printf_test();

    /* 回显，将收到字符原本的在显示出来 
     * 比如输入A -> 则 A发送给串口，此时不会显示
     *              main通过getchar然后putchar回显给PC
     * */
    while (1) {
        c = getchar();

        if (c == '\r') {
            putchar('\n');
        }

        if (c == '\n') {
            putchar('\r');
        }

        putchar(c);
    }

    return 0;
}
