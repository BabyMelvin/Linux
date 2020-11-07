#include "s3c2440_soc.h"
#include "nand_flash.h"
#include "string_utils.h"

#define TXD0READY (1 << 2)

void nand_init(void)
{
#define TACLS   (0)
#define TWRPH0  (1)
#define TWRPH1  (0)

    // 设置时序
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);

    // 使能NAND FLASH 控制器，初始化ECC，禁止片选
    NFCONT = (1 << 4)  | (1 << 1) | (1 << 0);
}

void nand_select(void)
{
    NFCONT &= ~(1 << 1);
}

void nand_deselect(void)
{
    NFCONT |= (1 << 1);
}

void nand_cmd (unsigned char cmd)
{
    volatile int i;
    NFCMD = cmd;
    for (i = 0; i < 10; i++);
}

void nand_addr(unsigned int addr)
{
    unsigned int col = addr % 2048;
    unsigned int page = addr / 2048;
    volatile int i;

    NFADDR = col & 0xff;
    for (i = 0; i < 10; i++);
    NFADDR = (col >> 8) & 0xff;
    for (i= 0; i < 10; i++);

    NFADDR = page & 0xff;
    for (i= 0; i < 10; i++);
    NFADDR = (page >> 8 ) & 0xff;
    for (i= 0; i < 10; i++);
    NFADDR = (page >> 16) & 0xff;
    for (i= 0; i < 10; i++);
}

void nand_page(unsigned int page)
{
    volatile int i;

    NFADDR = page & 0xff;
    for (i= 0; i < 10; i++);
    NFADDR = (page >> 8) & 0xff;
    for (i= 0; i < 10; i++);
    NFADDR = (page >> 16) & 0xff;
    for (i= 0; i < 10; i++);
}

void nand_col(unsigned int col)
{
    volatile int i;

    NFADDR = col & 0xff;
    for (i = 0; i < 10; i++);
    NFADDR = (col >> 8) & 0xff;
    for (i = 0; i < 10; i++);
}

void nand_wait_ready(void)
{
    while (!(NFSTAT & 1));
}

unsigned char nand_data(void)
{
    return NFDATA;
}

void nand_addr_byte (unsigned char addr)
{
    volatile int i;
    NFADDR = addr;
    for (i = 0; i < 10; i++);
}

int nand_bad(unsigned int addr)
{
    unsigned int col = addr % 2048;
    unsigned int page = addr / 2048;
    unsigned char val;

    // 1. 选中
    nand_select();

    // 2.发出读命令 00 h
    nand_cmd(0x00);

    // 3.发出地址(分5步发出)
    nand_col(col);
    nand_page(page);

    // 4. 发出读命令30h
    nand_cmd(0x30);

    // 5.判断状态
    nand_wait_ready();

    // 6.读数据
    val = nand_data();

    // 7. 取消选中
    nand_deselect();

    if (0xff != val)
        return 1;  // bad block
    else
        return 0;
}

#if 0
void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{
    int col = addr % 2048;
    int i = 0;

    while (i < len) {
        // 一个block 只判断一次
        if (!(addr & 0x1FFFF) & nand_bad(addr)) {
            addr += (128 * 1024);
            continue;
        }

        // 1. 选中
        nand_select();

        // 2. 发出读命令00h
        nand_cmd(0x00);

        // 3. 发出地址(分5步发出)
        nand_addr(addr);

        // 4. 发出读命令30h
        nand_cmd(0x30);

        // 5. 判断状态
        nand_wait_ready();

        // 6. 读数据
        for (; (col < 2048) && (i < len); col ++) {
            buf[i] = nand_data();
            i++;
            addr++;
        }
        col = 0;

        // 7. 取消选中
        nand_deselect();
    }
}
#endif

void nand_chip_id (void)
{
    unsigned char buf[5] = {0};

    nand_select();
    nand_cmd(0x90);
    nand_addr_byte(0x00);
   
    buf[0] = nand_data(); 
    buf[1] = nand_data();
    buf[2] = nand_data();
    buf[3] = nand_data();
    buf[4] = nand_data();
    nand_deselect();

    printf("maker  id   = 0x%x\n\r", buf[0]);
    printf("device id   = 0x%x\n\r", buf[1]);
    printf("3rd    byte = 0x%x\n\r", buf[2]);
    printf("4th    byte = 0x%x\n\r", buf[3]);
    printf("page   size = %dkb\n\r", 1 << (buf[3] & 0x03));
    printf("block  size = %dkb\n\r", 64 << ((buf[3] >> 4) & 0x03));
    printf("5th    byte = 0x%x\n\r", buf[4]);
}

int nand_erase(unsigned int addr, unsigned int len)
{
    int page = addr / 2048;
    if (addr & 0x1FFFF) {
        printf("nand erase err, adr is not block algin\n\r");
        return -1;
    }

    if (len & 0x1FFFF) {
        printf("nand_earse err, len is not block align\n\r");
        return -1;
    }

    nand_select();
    while (1) {
        page = addr / 2048;
        nand_cmd(0x60);

        /* row/page addr*/
        nand_addr_byte(page & 0xff);
        nand_addr_byte((page >> 8) & 0xff);
        nand_addr_byte((page >> 16) & 0xff);

        nand_cmd(0xD0);
        nand_wait_ready();
        len  -= (128 * 1024);
        if (len == 0)
            break;

        addr += (128 *1024);
    }

    nand_deselect();
    return 0;
}

void do_erase_nand_flash(void)
{
    unsigned int addr;

    /* 获得地址 */
    printf("Enter the address of sector to erase:");
    addr = get_uint();

    printf("erasing ...\n\r");
    nand_erase(addr, 128 * 1024);
}

void nand_w_data(unsigned char val)
{
    NFDATA = val;
}

void nand_write(unsigned int addr, unsigned char *buf, unsigned int len)
{
    int page = addr / 2048;
    int col = addr & (2048 - 1);
    int i = 0;

    nand_select();
    while (1) {
        nand_cmd(0x80);

        // 发出地址
        // col addr
        nand_addr_byte(col & 0xff);
        nand_addr_byte((col>>8) & 0xff);

        // row/page addr
        nand_addr_byte(page & 0xff);
        nand_addr_byte((page>>8) & 0xff);

        // 发送数据
        for (; (col < 2048) && (i<len);) {
            nand_w_data(buf[i++]);
        }
        nand_cmd(0x10);
        nand_wait_ready();
        
        if (i == len) {
            break;
        } else {
            // 开始下一个循环page
            col = 0;
            page++;
        }
    }
    nand_deselect();
}

void do_write_nand_flash(void)
{
    unsigned int addr;
    unsigned char str[100];
    int i, j;
    unsigned int val;

    // 获得地址
    printf("Enter the address of sector to write:");
    addr = get_uint();

    printf("Enter the string to write:");
    gets(str);

    printf("writing ...\n\r");
    nand_write(addr, str, strlen(str) + 1);
}

void nand_read(unsigned int addr, unsigned char*buf, unsigned int len)
{
    int i  = 0;
    int  page = addr/2048;
    int col = addr & (2048 - 1);

    nand_select();

    while (i < len) {
        // 发出00h命令
        nand_cmd(00);

        // 发出地址, col addr
        nand_addr_byte(col & 0xff);
        nand_addr_byte((col >> 8) & 0xff);

        // row/page addr
        nand_addr_byte(page & 0xff);
        nand_addr_byte((page >> 8) & 0xff);
        nand_addr_byte((page >> 16) & 0xff);

        // 发出30h命令
        nand_cmd(0x30);

        // 等待就绪
        nand_wait_ready();

        // 读数据
        for (; (col < 2048) && (i < len); col++) {
            buf[i++] = nand_data();
        }
        
        if (i = len)
            break;
        col = 0;
        page ++;
    }

    nand_deselect();
}

void do_read_nand_flash(void)
{
    unsigned int addr;
    volatile unsigned char *p;
    int i, j;
    unsigned char c;
    unsigned char str[16];
    unsigned char buf[64];

    // 获得地址
    printf("Enter the address to read:");
    addr = get_uint();

    nand_read(addr, buf, 64);
    p = (volatile unsigned char *)buf;
    printf("Data:\n\r");
    
    // 长度固定为64
    for  (i = 0; i < 4; i++) {
        // 每行打印16个数据
        for (j=0; j < 16; j++) {
            // 先打印数值
            c = *p++;
            str[j] = c;
            printf("%02x ", c);
        }
        printf(" ;");
        for  (j = 0; j < 16; j++) {
            // 后打印字符
            if (str[j] < 0x20 || str[j] > 0x7e) { //不可视
                putchar('.');
            } else {
                putchar(str[j]);
            }
        }
        printf("\n\r");
    }
}

void nand_flash_test(void)
{
    char c;
    while (1) {
        printf("[s] Scan nand flash\n\r");
        printf("[e] Erase nand flash\n\r");
        printf("[w] Write nand flash\n\r");
        printf("[r] Read nand flash\n\r");
        printf("[q] quit \n\r");
        printf("Enter selection:");
        c = getchar();
    
        printf("%c\n\r", c);
        switch (c) {
            case 'q':
            case 'Q': 
                return;
                break;
            case 's':
            case 'S': 
                nand_chip_id();
                break;
            case 'e':
            case 'E': 
                do_erase_nand_flash();
                break;
            case 'w':
            case 'W': 
                do_write_nand_flash();
                break;
            case 'r':
            case 'R': 
                do_read_nand_flash();
                break;
            default:
                break;
        }
    }
}
