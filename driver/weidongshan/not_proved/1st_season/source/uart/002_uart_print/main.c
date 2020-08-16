#include "s3c244_soc.h"
#include "uart.h"

int main(void)
{
    unsigned char c;

    uart0_init();

    puts("Hello world\n\r");
    my_printf_test();

    while(1) {
        c = getchar();
        if(c == '\r')
            putchar('\n');
            
        if(c == '\n')
            putchar('\r')

        putchar(c);
    }

    return 0;
}
