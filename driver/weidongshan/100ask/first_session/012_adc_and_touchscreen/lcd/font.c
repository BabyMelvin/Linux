extern const unsigned char fontdata_8x16[];
/* 获得LCD参数 */
static unsigned int fb_base;
static int xres, yres, bpp;

void font_init(void)
{
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
}

void fb_print_char(int x, int y, char c, unsigned int color)
{
	int i, j;
	
	/* 根据c的ascii码在fontdata_8x16中得到点阵数据 */
	unsigned char *dots = (unsigned char *)&fontdata_8x16[c * 16];

	unsigned char data;
	int bit;

	/* 根据点阵来设置对应象素的颜色 */
	for (j = y; j < y+16; j++)
	{
		data = *dots++;
		bit = 7;
		for (i = x; i < x+8; i++)
		{
			/* 根据点阵的某位决定是否描颜色 */
			if (data & (1<<bit))
				fb_put_pixel(i, j, color);
			bit--;
		}
	}
}

/* "abc\n\r123" */
void fb_print_string(int x, int y, char* str, unsigned int color)
{
	int i = 0, j = 0;
	
	while (str[i])
	{
		if (str[i] == '\n'){
			y = y+16;
        } else if (str[i] == '\r') {
			x = 0;
        } else {
			fb_print_char(x, y, str[i], color);
			x = x + 8;
            printf("x = %d, xres = %d\r\n", x, xres);
			if (x >= xres) /* 换行 */
			{
				x = 0;
				y = y+16;
			}
		}
		i++;
	}
}
