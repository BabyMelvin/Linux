#include "lcd.h"

/* 实现画点*/
/* 获得LCD参数 */
static unsigned int fb_base;
static int xres, yres, bpp;

void fb_get_lcd_params(void)
{
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
}

/* rgb: 0x00RRGGBB */
unsigned short convert32bppto16bpp(unsigned int rgb)
{
	int r = (rgb >> 16)& 0xff;
	int g = (rgb >> 8) & 0xff;
	int b = rgb & 0xff;

	/* rgb565 */
	r = r >> 3;
	g = g >> 2;
	b = b >> 3;

	return ((r<<11) | (g<<5) | (b));
}

void fb_put_pixel(int x, int y, unsigned int color)
{
	unsigned char  *pc;  /* 8bpp */
	unsigned short *pw;  /* 16bpp */
	unsigned int   *pdw; /* 32bpp */

	unsigned int pixel_base = fb_base + (xres * bpp / 8) * y + x * bpp / 8;

	switch (bpp)
	{
		case 8:
			pc = (unsigned char *) pixel_base;
			*pc = color;
			break;
		case 16:
			pw = (unsigned short *) pixel_base;
			*pw = convert32bppto16bpp(color);
			break;
		case 32:
			pdw = (unsigned int *) pixel_base;
			*pdw = color;
			break;
	}
}
