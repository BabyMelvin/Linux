#include "nand.h"

static S3C2440_NAND *s3c2440nand = (S3C2440_NAND *)0x4e000000;
static t_nand_chip nand_chip[MAX_NUM_NAND] = {0};

static p_nand_chip nand_chip_c;

void nand_reset(void)
{
    if(nand_chip_c != NULL) {
        nand_chip_c->nand_init();
    }
}

void wait_idle(void)
{
    if(nand_chip_c != NULL) {
        nand_chip_c->wait_idle();
    }
}

void nand_select_chip(void)
{
    if(nand_chip_c != NULL) {
        nand_chip_c->nand_select_chip();
    }
}

void nand_deselect_chip(void)
{
    if(nand_chip_c != NULL) {
        nand_chip_c->nand_deselect_chip();
    }
}

void nand_write_cmd(int cmd)
{
    if(nand_chip_c != NULL) {
        nand_chip_c->write(cmd);
    }
}

void nand_write_addr(unsigned int addr)
{
    if(nand_chip_c != NULL) {
        nand_chip_c->write_addr(addr);
    }
}

void add_nand(t_nand_chip nand_c)
{
    int i;
    for(i = 0; i < MAX_NUM_NAND; i++) {
        if(nand_chip[i].name) {
            nand_chip[i] = nand_c;
            break; 
        }
    }
}

void set_nand_chip(int8_t *name)
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

void register_nand(void)
{
    extern t_nand_chip s3c2410_nand_chip;
    extern t_nand_chip s3c2440_nand_chip;

    add_nand(s3c2410nand);
    add_nand(s3c2440nand);
}
