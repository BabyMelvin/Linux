#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

char g_Char = 'A';
const char g_Char2 = 'B';
int g_A = 0;
int g_B;

int main (void)
{
    uart0_init();

    while (1) {
        putchar(g_Char);
        g_Char++;
        delay(1000000);
    }
    
    return 0;
}
