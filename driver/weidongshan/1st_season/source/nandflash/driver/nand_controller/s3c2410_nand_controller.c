#include "nand_controller.h"
#define s3c2410CONTROLLER_NFCONF (*(volatile unsigned int))0x4e000000
static void s3c2410_nand_controller_init()
{
    s3c2410CONTROLLER_NFCONF = (1<<15)|(1<<12)|(1<<11)|(TACLS<<8)|(TWRPH0<<4)|(TWRPH1<<0);
}

static void s3c2410_nand_controller_deinit()
{
}

t_nand_controller s3c2410_controller = {
    .name = "s3c2410_controller",
    .init = s3c2410_controller_init,
    .deinit = s3c2410_nand_controller_deinit,
};
