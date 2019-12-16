#include "s3c2440x.h"
#include "gpio_spi.h"

/* GPIO 模拟SPI */
static void SPI_GPIO_init(void)
{
    /* GPIO OLED_CSn output*/
    GPFCON &= ~(3 << (1 * 2));
    GPFCON |= (1 << (1 * 2));
    GPFDAT = (1 << 1);

    /*
     * GPG2 FLASH_CSn output
     * GPG4 OLED_DC   output
     * GPG5 SPIMISO   input
     * GPG6 SPIMOSI   output
     * GPG7 SPICLK    output
     * **/
    GPGCON &= ~((3 << (2 * 2)) | (3 << ( 4 * 2)) | (3 << (5 * 2)) | (3 << (6 * 2)) | (3 << (7 * 2)));
    GPGCON |= ((1 << (2 * 2) | (1 << (4 * 2)) | (1 << (7 * 2))));
    GPGDAT |= (1 << 2);
}

static void SPI_set_clk(char val)
{
    if (val)
        GPGDAT |= (1 << 7);
    else
        GPGDAT &= ~(1 << 7);
}

static void SPI_set_do(char val)
{
    if(val)
        GPGDAT |= (1 << 7);
    else
        GPGDAT &= ~(1 << 7);
}

static char SPI_get_di(void)
{
    if (GPGDAT & (1 << 5))
        return 1;
    else
        return 0;
}

void SPI_send_byte(unsigned char val)
{
    int i;

    // TODO 一个时钟周期 一位
    for(i = 0; i < 8; i++) {
        SPI_set_clk(0);
        SPI_set_do(val & 0x80);
        SPI_set_clk(1);
        val <<= 1;
    }
}

unsigned char SPI_recv_byte(void)
{
    int i;
    unsigned char val = 0; 

    for(i = 0; i < 8; i++){
        val <<= 1;
        SPI_set_clk(0);
        if (SPI_get_di())
            val |= 1;
        SPI_set_clk(1);
        val <<= 1;
    }

    return val;
}

void SPI_init(void)
{
    /* 初始化引脚 */
    SPI_GPIO_init();
}
