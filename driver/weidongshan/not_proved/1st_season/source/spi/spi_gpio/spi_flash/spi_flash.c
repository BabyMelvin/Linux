#include "s3c24xx.h"
#include "gpio_spi.h"

/*********************Flash get id*******************************/
static void SPI_flash_set_cs(char val)
{
    if(val)
        GPGDAT |= (1 << 2);
    else
        GPGDAT &= ~(1 << 2);
}

static void SPI_flash_send_byte(unsigned char val)
{
    SPI_send_byte(val);
}

static unsigned char SPI_flash_recv_byte()
{
    return SPI_recv_byte();
}

static void SPI_flash_send_addr(unsigned int addr)
{
    SPI_flash_send_byte((addr >> 16) & 0xff);
    SPI_flash_send_byte((addr >> 8) & 0xff);
    SPI_flash_send_byte(addr & 0xff);
}

/********************************************************************/
void SPI_flash_read_id(int *p_mid, int *p_did)
{
    SPI_flash_set_cs(0);

    /* 发送0x90, 之后再发送一个字节数据 */
    SPI_flash_send_byte(0x90);
    SPI_flash_send_addr(0);

    /* 可以顺序读到相应的数据 */
    *p_mid = SPI_flash_recv_byte();
    *p_did = SPI_flash_recv_byte();

    SPI_flash_set_cs(1);
}

/*********************Flash program and read***************************/
static void SPI_flash_write_enable(unsigned char enable)
{
    SPI_flash_set_cs(0);
    if (enable)
        SPI_send_byte(0x06);    
    else
        SPI_send_byte(0x04);

    SPI_flash_set_cs(0);
}

static void SPI_flash_write_status_reg(unsigned char *reg, unsigned int len)
{
    unsigned int i =0;

    if(len > 0) {
        SPI_flash_set_cs(0);
        SPI_flash_send_byte(0x01);
        while (i < len) {
            SPI_flash_send_byte(reg[i]);
            i++ ;
        }

        SPI_flash_set_cs(1);
    }
}

static unsigned char SPI_flash_read_status_reg(unsigned char cmd)
{
    unsigned char val;

    SPI_flash_set_cs(0);
    SPI_flash_send_byte(cmd);
    val = SPI_flash_recv_byte();
    SPI_flash_set_cs(1);

    return val;
}

static void SPI_flash_clear_protect_for_status_reg()
{
    unsigned char reg[2];

    reg[0] = SPI_flash_read_status_reg(0x05);
    reg[1] = SPI_flash_read_status_reg(0x35);

    reg[0] &= ~(1 << 7);
    reg[1] &= ~(1 << 0);

    SPI_flash_write_status_reg(reg, sizeof(reg)/sizeof(reg[0]));
}

static void SPI_flash_clear_protect_for_data(void)
{
    /* cmp = 0,bp2, 1,0 = 0b000 */
    unsigned char reg[2];

    reg[0] = SPI_flash_read_status_reg(0x05);
    reg[1] = SPI_flash_read_status_reg(0x35);

    SPI_flash_write_status_reg(reg, sizeof(reg)/sizeof(reg[0]));
}

static void SPI_flash_wait_when_busy(void)
{
    while(SPI_flash_read_status_reg(0x05) & 1);
}

/* erase 4k*/
void SPI_flash_erase_sector(unsigned int addr)
{
    SPI_flash_write_enable(1);
    
    SPI_flash_set_cs(0);
    SPI_flash_send_byte(0x20);
    SPI_flash_send_addr(addr);
    SPI_flash_set_cs(1);

    SPI_flash_wait_when_busy();
}

void SPI_flash_program(unsigned int addr, unsigned char *buf, int len)
{
    int i;

    SPI_flash_write_enable(1);
    SPI_flash_set_cs(0);
    SPI_send_byte(0x20);
    SPI_flash_send_addr(addr);

    for (i = 0; i < len; i++)
        SPI_flash_send_byte(buf[i]);

    SPI_flash_set_cs(1);
    SPI_flash_wait_when_busy();
}

void SPI_flash_read(unsigned int addr, unsigned char *buf, int len)
{
    int i;

    SPI_flash_set_cs(0);
    SPI_flash_send_addr(0x03);
    SPI_flash_send_addr(addr);

    for (i = 0; i < len; i++)
        buf[i] = SPI_flash_recv_byte();

    SPI_flash_set_cs(1);
}

void SPI_flash_init(void)
{
    SPI_flash_clear_protect_for_status_reg();
    SPI_flash_clear_protect_for_data();
}
