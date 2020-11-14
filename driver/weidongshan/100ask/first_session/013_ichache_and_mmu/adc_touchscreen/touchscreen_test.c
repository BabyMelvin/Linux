void touchscreen_test(void)
{
    unsigned int fb_base;
	int xres, yres, bpp;

	int x, y, pressure;
    /* 获得LCD的参数: fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
    touchscreen_init();
    /* 清屏 */
	clear_screen(0);

	/* 显示文字提示较准 */
	fb_print_string(70, 70, "Touc cross to calibrate touchscreen", 0xffffff);
	ts_calibrate();

    /* 显示文字提示绘画 */
	fb_print_string(70, yres - 70, "OK! To draw!", 0xffffff);

    while (1)
	{
		if (ts_read(&x, &y, &pressure) == 0)
		{
			printf(" x = %d, y = %d\n\r", x, y);

			if (pressure)
			{
				fb_put_pixel(x, y, 0xff00);
			}
		}
	}
}
