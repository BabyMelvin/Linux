#ifndef __STDIO_H__
#define __STDIO_H__
#include "type.h"

#ifndef _VALIST
#define _VALIST
typedef char *va_list;
#endif /* _VALIST */

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int snprintf(char * buf, size_t size, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char * buf, const char *fmt, ...);
int vsscanf(const char * buf, const char * fmt, va_list args);
int sscanf(const char * buf, const char * fmt, ...);

void putc(unsigned char c);
unsigned char getc(void);

int printf(const char *fmt, ...);
int scanf(const char * fmt, ...);
#endif /* __STDIO_H__ */
