#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include "uart.h"

long simple_strtol(const char *cp, char **endp, unsigned int base);
char *gets(char s[]);
int get_int(void);
unsigned int get_uint(void);

#endif
