#include "nand_controller.h"
#define s3c2440CONTROLLER_NFCONF  (*(volatile unsigned int*)0x4e0000000)
#define s3c2440CONTROLLER_NFCONT   (*(volatile unsigned int*)0x4e000004)

static void s3c2440_nand_controller_init()
{
    // 设置时序
    s3c2440CONTROLLER_NFCONF =(TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
    // 使能NAND flash 控制器，初始化ECC，禁止片选
    s3c2440CONTROLLER_NFCONT = (1 << 4) | (1 << 1) | (1 << 0);
}

static void s3c2440_nand_controller_deinit()
{
}

t_nand_controller s3c2440_controller = {
     .name = "s3c2440_controller",
     .init = s3c2440_controller_init,
     .deinit = s3c2440_nand_controller_deinit,
};
