#include "uart.h"

void printException(unsigned int cpsr, char *str)
{
    puts("Exception ! cpsr = ");
    printHex(cpsr);
    puts(" ");
    puts(str);
    puts("\n\r");
}

void print1(void)
{
    puts("abc\n\r");
}

void print2(void)
{
    puts("123\nr\r");
}
