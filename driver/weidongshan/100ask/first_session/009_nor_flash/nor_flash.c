#include "my_printf.h"
#include "string_utils.h"

// jz2440,nor -->cs0, base addr = 0
#define NOR_FLASH_BASE (0)


/**
 * 比如：55H 98
 * 本意是：往(0+(0x55)<<1) 写入0x98 
 */
void nor_write_word(unsigned int base, unsigned int offset, unsigned int val)
{
    volatile unsigned short *p = (volatile unsigned short *)(base + (offset << 1));
    *p = val;
}

unsigned int nor_read_word(unsigned int base, unsigned int offset)
{
   volatile unsigned short *p = (volatile unsigned short *)(base + (offset << 1));
   return *p;
}

void nor_cmd(unsigned int offset, unsigned int cmd)
{
    nor_write_word(NOR_FLASH_BASE, offset, cmd);
}


unsigned int nor_dat(unsigned int offset)
{
    return nor_read_word(NOR_FLASH_BASE, offset);
}

void do_read_nor_flash(void)
{
}

void do_write_nor_flash(void)
{
}

void do_erase_nor_flash (void)
{
}

/**
 * 进入NOR FLASH 的CFIMOSHI
 * 读取各类信息
 * */
void do_scan_nor_flash (void)
{
    char str[4];
    unsigned int size;
    int regions, i, region_info_base;
    int block_addr, blocks, block_size, j, cnt;
    int  vendor, device;

    // 打印厂商ID，设备ID
    nor_cmd(0x555, 0xaa); // 解锁
    nor_cmd(0x2aa, 0x55);

    nor_cmd(0x555, 0x90); // read id

    vendor = nor_dat(0);
    device = nor_dat(1);
    nor_cmd(0, 0xf0); // reset
    nor_cmd(0x55, 0x98); // 进入cfi模式

    str[0] = nor_dat(0x10);
    str[1] = nor_dat(0x11);
    str[2] = nor_dat(0x12);
    str[3] = '\0';
    printf("str = %s\n\r", str);

    // 打印容量
    size = 1 << (nor_dat(0x27));
    printf("vendor id = 0x%x, device id = 0x%x, nor size = 0x%x, %dM\n\r",
            vendor, device, size, size/(1024*1024));

    regions = nor_dat(0x2c);
    region_info_base = 0x2d;
    block_addr = 0;
    printf("Block/Sector start Address:\n\r");

    for (i = 0; i< regions; i++) {
        blocks = 1 + nor_dat(region_info_base) + 
            ((nor_dat(region_info_base + 1)) << 8);
        block_size = 256 * (nor_dat(region_info_base + 2) + 
                (nor_dat(region_info_base + 3) << 8));
        region_info_base += 4;
        printf("\n\r regions %2d, blocks = %2d, block_size = 0x%08x,block_addr = \n\r",
                i, blocks, block_size);
        cnt = 0;
        for (j = 0; j < blocks; j++) {
            // 打印每个block的起始地址
            //printf("0x%08x", block_addr);
            printHex(block_addr);
            putchar(' ');
            cnt ++;
            block_addr += block_size;
            if (cnt % 5 == 0)
                printf("\n\r");
        }
    }
    printf("\n\r");

    /* 退出CFI模式 */
    nor_cmd(0, 0xf0);
}

void nor_flash_test (void)
{
    char c;

    while  (1) {
        // 打印菜单，供我们选择测试内容
        printf("[s] Scan nor flash\n\r");
        printf("[e] Erase nor flash\n\r");
        printf("[w] Write nor flash\n\r");
        printf("[r] Read nor flash\n\r");
        printf("[q] quit\n\r");
        printf("Enter selection:");

        c = getchar();
        printf("%c\n\r", c);

        /**
         * 测试内容
         * 1. 识别nor flash
         * 2. 擦除nor flash 某个扇区
         * 3. 编写某个地址
         * 4. 读某个地址
         * */
        switch (c) {
            case 'q':
            case 'Q':
                return;
                break;
            case 's':
            case 'S':
                do_scan_nor_flash();
                break;
            case 'e':
            case 'E':
                do_erase_nor_flash();
                break;
            case 'w':
            case 'W':
                do_write_nor_flash();
                break;
            case 'r':
            case 'R':
                do_read_nor_flash();
                break;
            default:
                break;
        }
    }

}
