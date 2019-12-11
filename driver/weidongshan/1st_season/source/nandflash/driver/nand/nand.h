#include "nand_controller.h"

static S3C2440_NAND *s3c2440nand = (S3C2440_NAND *)0x4e000000;
static t_nand_chip nand_chip[MAX_NUM_NAND] = {0};
static p_nand_chip nand_chip_c;

/* 通用接口 */
static void nand_reset(void);
static void wait_idle(void);
static void nand_deselect_chip(void);
static void write_cmd();
static void write_addr(unsigned int addr);
static unsigned char read_data(void);


void nand_register(t_nand_chip nand_c)
{
    int i;
    for(i = 0; i < MAX_NUM_NAND; i++) {
        if(nand_chip[i].name) {
            nand_chip[i] = nand_c;
            break; 
        }
    }
}

void set_current_nand_chip(int8_t *name)
{
    int i;
    for(i = 0; i < MAX_NUM_NAND; i++) {
        if(!strcmp(nand_chip[i].name, name)) {
            nand_chip_c = &nand_chip[i];
            break; 
        }
    }

    printf("wtf, cann't find:%s nand chip\n", name);
}

void nand_init(void)
{
    s3c2410_init();
    s3c2440_init();

    if((STATUS1 == 0x32410000) || (GSTATUS1 == 0x32410002)) {
        set_current_nand_chip("s3c2410");
    } else {
        set_current_nand_chip("s3c2440");
    }
    nand_init();
}
