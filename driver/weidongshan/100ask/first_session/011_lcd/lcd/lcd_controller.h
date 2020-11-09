#ifndef __LCD_CONTROLLER_H__
#define __LCD_CONTROLLER_H__
#include "lcd.h"

typedef struct lcd_controller {
    char *name;
    void (*init)(p_lcd_params plcdparams);
    void (*enable)(void);
    void (*disable)(void);
    void (*init_palette)(void);
} lcd_controller, *p_lcd_controller;

int lcd_controller_init(p_lcd_params plcdparams);
void lcd_controller_enable(void);
void lcd_controller_disable(void);
void lcd_contoller_add (void);
#endif /* __LCD_CONTROLLER_H__ */

