#ifndef __MY_PRINTF__
#define __MY_PRINTF__

#include "uart.h"
//#include <stdio.h>
#define __out_putchar putchar
#define MAX_NUMBER_BYTES 64

extern int my_printf_test(void);
int printf(const char *fmt, ...);
#endif
