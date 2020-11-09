#ifndef __SERIAL_H__
#define __SERIAL_H__

void uart0_init(void);
void putc(unsigned char c);
unsigned char getc(void);
int is_digit(unsigned char c);
int is_letter(unsigned char c);
#endif /* __SERIAL_H__ */
