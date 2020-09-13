#include "uart.h"

void printException(unsigned int cpsr, char *str)
{
    puts("test:Exception ! cpsr = ");
    printHex(cpsr);
    puts(" ");
    puts(str);
    puts("\n\r");
}

void print1(void)
{
    puts("abc test\n\r");
}

void print2(void)
{
    puts("123\nr\r");
    puts("my test\nr\r");
}

void  printSWIVal(unsigned int *pSWI)
{
    puts("SWI Val = ");
    printHex(*pSWI & ~0xff000000);
    puts("\n\r");
}
