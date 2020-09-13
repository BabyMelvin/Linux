#ifndef __UART_H__
#define __UART_H__

void uart0_init();
int putchar (int c);
int getchar (void);
int puts(const char *s);
void printHex(unsigned int val);
#endif /* __UART_H__*/
