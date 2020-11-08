#include "gemetory.h"

void lcd_test(void)
{
    unsigned int fb_base;
    int xres, yres, bpp;
    int x, y;
    unsigned char *p0;
    unsigned short *p;
    unsigned int *p2;
    
    // 初始化LCD
    lcd_init();

    // 使能LCD
    lcd_enable();

    // 获得LCD参数:fb_base, xres, yres, bpp
    get_lcd_params(&fb_base, &xres, &yres, &bpp);
    printf("xres=%d, yres=%d, bpp=%d\r\n", xres, yres, bpp);
    
    fb_get_lcd_params();
    // font 初始化
    font_init();

    /* 往framebuffer写数据*/
    if (bpp == 8) {
        // 使用调色板,硬件支持的是bpp 32
        /* bpp: palette[12], framebuffer对应palette索引*/
		p0 = (unsigned char *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p0++ = 12;

		/* palette[47] */
		p0 = (unsigned char *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p0++ = 47;

		/* palette[88] */
		p0 = (unsigned char *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p0++ = 88;

		/* palette[0] */
		p0 = (unsigned char *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p0++ = 0;
    } else if (bpp == 16) {
        // 让LCD输出整屏幕红色,565:0xf800
        p = (unsigned short *)fb_base;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p++ = 0xf800;

        // 让LCD输出整屏幕green,565:0x7e00
        p = (unsigned short *)fb_base;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p++ = 0x7e00;

        // 让LCD输出整屏blue,565:0x71f
        p = (unsigned short *)fb_base;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p++ = 0x1f;
    } else if (bpp == 32) {
        // 让LCD输出整屏幕红色,RRGGBB:0xff0000
        p2 = (unsigned int *)fb_base;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p2++ = 0xff0000;

        // 让LCD输出整屏幕green,RRGGBB:0x00ff00
        p2 = (unsigned int *)fb_base;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p2++ = 0x00ff00;

        // 让LCD输出整屏blue,RRGGBB:0x0000ff
        p2 = (unsigned int *)fb_base;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p2++ = 0x0000ff;

    }

    delay(1000000);
    /* 画线 */
	draw_line(0, 0, xres - 1, 0, 0x23ff77);
	draw_line(xres - 1, 0, xres - 1, yres - 1, 0xffff);
	draw_line(0, yres - 1, xres - 1, yres - 1, 0xff00aa);
	draw_line(0, 0, 0, yres - 1, 0xff00ef);
	draw_line(0, 0, xres - 1, yres - 1, 0xff45);
	draw_line(xres - 1, 0, 0, yres - 1, 0xff0780);

	delay(1000000);

	/* 画圆 */
	draw_circle(xres/2, yres/2, yres/4, 0xff);

    /* 输出文字 */
	fb_print_string(10, 10, "hello ,\n\rbaby melvin!", 0xff);
}
