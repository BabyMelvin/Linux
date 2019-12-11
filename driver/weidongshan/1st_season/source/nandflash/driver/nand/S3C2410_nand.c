#include "nand_controller.h"

static void s3c2410_nand_reset(void);
static void s3c2410_wait_idle(void);
static void s3c2410_nand_select_chip(void);
static void s3c2410_nand_deselect_chip(void);
static void s3c2410_nand_write_cmd(int cmd);
static void s3c2410_nand_write_addr(unsigned int addr);
static void s3c2410_nand_read_data(void);



int32_t s3c2410_init()
{
    t_nand_chip s3c2410_nand_chip = {
        .nand_reset = s3c2410_nand_reset,
        .nand_idle = s3c2410_wait_idle,
        .nand_select_chip = s3c2410_nand_select_chip,
        .nand_deselect_chip = s3c2410_nand_deselect_chip,
#ifdef LARGET_PAGE
        .write_cmd = s3c2410_nand_write_cmd_lp,
#else
        .write_cmd = s3c2410_nand_write_cmd,
#endif
        .write_addr = s3c2410_nand_read_data,
    };


    nand_register(s3c2410_nand_chip);
}

