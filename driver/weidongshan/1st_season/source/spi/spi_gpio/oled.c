#include "oled.h"


static void SPISetPageAddrMode(void)
{
    OLEDWriteCmd(0x20);
    OLEDWriteCmd(0x02);
}

static void OLED_set_dc(char val)
{
    if(val)
        GPGDAT |= (1 << 4);
    else
        GPGDAT &= ~(1 << 4);

}

static void OLED_set_cs(char val)
{
    if (val)
        GPFDAT |= (1 << 1);
    else
        GPFDAT &= ~(1 << 1);
}

static void OLEDWriteCmd(unsigned char cmd)
{
    OLED_set_dc(0); /* command */
    OLED_set_cs(0); /* select OLED*/

    SPI_send_byte(cmd);

    OLED_set_cs(1); /*de-select OLED*/
    OLED_set_dc(0); /* data */
}

static void OLEDWriteDat(unsigned char dat)
{
    OLED_set_dc(1);
    OLED_set_cs(0);

    SPI_send_byte(dat);

    OLED_set_cs(1);
    OLED_set_dc(1);
}

static void OLEDSetPos(int page, int col)
{
    OLEDWriteCmd(0xB0 + page);  /* page address */

    OLEDWriteCmd(col & 0xf); /* lower column start address*/
    OLEDWriteCmd(0x10 + (col >> 4)); /* higher column start address*/
}

static void OLEDClear(void)
{
    int page, i;
    for (page = 0; page < 8; page ++) {
        OLEDSetPos(page, 0);
        for (i = 0; i < 128; i++) {
            OLEDWriteDat(0);
        }
    }
}

/*
 * page: 0-7
 * col : 0-127
 * 字符:8 x 16 像素
 * **/
void OLED_put_char(int page, int col, char c)
{
    int i = 0;

    /* 得到字模 */
    const unsigned cahr *dots = oled_asc2_8x16[c - ' '];

    /* 发给OLED */
    OLEDSetPos(page, col);

    /* 发送8字节数据 */
    for (i = 0; i < 8; i++)
        OLEDWriteDat(dots[i]);

    OLEDSetPos(page + 1, col);
    /* 发送8字节数据 */
    for (i = 0; i < 8; i++)
        OLEDWriteDat(dots[i+8]);
}

/*
 * page: 0-7
 * col : 0-127
 * 字符:8 x 16 像素
 * **/
void OLED_print(int page, int col, char *str)
{
    int i = 0;
    while(str[i]) {
        OLED_put_char(page, col, str[i]);
        col += 8;
        if (col > 127) {
            col = 0;
            page += 2;
        }
    }
}

void OLED_init(void)
{
    /* 向OLED发命令以初始化 */
    OLEDWriteCmd(0xAE); /*display off*/ 
    OLEDWriteCmd(0x00); /*set lower column address*/ 
    OLEDWriteCmd(0x10); /*set higher column address*/ 
    OLEDWriteCmd(0x40); /*set display start line*/ 
    OLEDWriteCmd(0xB0); /*set page address*/ 
    OLEDWriteCmd(0x81); /*contract control*/ 
    OLEDWriteCmd(0x66); /*128*/ 
    OLEDWriteCmd(0xA1); /*set segment remap*/ 
    OLEDWriteCmd(0xA6); /*normal / reverse*/ 
    OLEDWriteCmd(0xA8); /*multiplex ratio*/ 
    OLEDWriteCmd(0x3F); /*duty = 1/64*/ 
    OLEDWriteCmd(0xC8); /*Com scan direction*/ 
    OLEDWriteCmd(0xD3); /*set display offset*/ 
    OLEDWriteCmd(0x00); 
    OLEDWriteCmd(0xD5); /*set osc division*/ 
    OLEDWriteCmd(0x80); 
    OLEDWriteCmd(0xD9); /*set pre-charge period*/ 
    OLEDWriteCmd(0x1f); 
    OLEDWriteCmd(0xDA); /*set COM pins*/ 
    OLEDWriteCmd(0x12); 
    OLEDWriteCmd(0xdb); /*set vcomh*/ 
    OLEDWriteCmd(0x30); 
    OLEDWriteCmd(0x8d); /*set charge pump enable*/ 
    OLEDWriteCmd(0x14); 

    OLEDSetPageAddrMode();

    OLEDClear();

    OLEDWriteCmd(0xAF); /* display ON */
}
