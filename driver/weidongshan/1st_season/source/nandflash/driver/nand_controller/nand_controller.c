#include "nand_controller.h"

t_nand_controller nand_controller[MAX_NUM_CONTROLLER] = {0};
p_nand_controller current_nand_controller;


int32_t register_nand_controller(t_nand_controller nand_cntl)
{
    int i;
    for(i = 0; i < MAX_NUM_CONTROLLER; i++) {
        if(!nand_controller[i].name) { 
            nand_controller[i] = nand_cntl;
            break;
        }
    }

    if(i > MAX_NUM_CONTROLLER - 1) {
        printf("sorry, register %s fail:exceed MAX_NUM_CONTROLLER\n", nand_cntl.name);
        return -1;
    }

    return 0;
}

int32_t set_nand_controller(int8_t *name)
{
    int i;
    for(i = 0; i < MAX_NUM_CONTROLLER; i++) {
        if(!strcmp(nand_controller[i].name, name)) { 
            current_nand_controller = nand_controller[i];
            break;
        }
    }

    if(i > MAX_NUM_CONTROLLER - 1) {
        printf("sorry, %s not registered\n", nand_cntl.name);
        return -1;
    }

    return 0;
}
int32_t nand_controller_init()
{
   s3c2410_controller_init(); 
   s3c2440_controller_init();
}
