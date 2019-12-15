#ifndef __OLED_H__
#define __OLED_H__

void OLED_init(void);
void OLED_print(int page, int col, char *str);
void OLED_put_char(int page, int col, char c);
#endif /* __OLED_H__ */
