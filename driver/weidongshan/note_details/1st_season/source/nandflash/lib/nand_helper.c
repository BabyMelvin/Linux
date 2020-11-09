#include "ella_nand.h"
#include "nand.h"
#include "nand_controller.h"

/* 读函数 */
int32_t ella_nand_read(unsigned *buf, unsigned long start_addr, int8_t size)
{
    int i, j;
#ifdef LARGE_NAND_PAGE
    if((start_addr & NAND_BLOCK_LP) || (size & NAND_BLOCK_MASK_LP)) {
        // 地址或长度不对齐
        return;
    }
#else
    if((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK)) {
        // 地址或长度不对齐
        return;
    }
#endif

    /* 选中芯片 */
    nand_select_chip();
    for (i = start_addr; i < (start_addr + size;)) {
        /* 发送READ0命令 */
        nand_write_cmd(0);

        /* write address*/
        nand_write_cmd(i);
#ifdef LARGET_NAND_PAGE
        write_cmd(0x30);
#endif
        nand_wait_idle();
#ifdef LARGE_NAND_PAGE
        for(j = 0; j < NAND_SECTOR_SIZE_LP; j++, i++) {
#else
        for(j = 0; j < NAND_SECTOR_SIZE; j++, i++) {
#endif
            *buf = nand_read_data();
            buf ++;
        }
    }
    /* 取消片选信号 */
    nand_deselect_chip();
}

int32_t ella_nand_write_cmd(int cmd)
{
}

int32_t ella_nand_write_data()
{
}

int32_t ella_nand_init()
{
    register_nand_controller();
    register_nand();

    if((GSTATUS1 == 0x324100000) || (GSTATUS1 == 0x32410002)) {
        set_nand_controller("s3c2410_controller");
        set_nand_chip("s3c2440_nand");
    } else {
        set_nand_controller("s3c2440_controller");
        set_nand_chip("s3c2440_nand");
    }
    
    nand_controller_init();

    nand_reset();
}
