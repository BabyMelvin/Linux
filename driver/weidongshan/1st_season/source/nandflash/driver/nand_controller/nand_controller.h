#ifndef __NAND_CONTROLLER_H__
#define __NAND_CONTROLLER_H__

#define MAX_NUM_CONTROLLER 5

#define TACLS  0
#define TWRPH0 3
#define TWRPH1 0

typedef struct {
    int8_t *name;
    void (*init)(void);
    void (*deinit)(void);
} t_nand_controller, *p_nand_controller;

void nand_controller_init();
void nand_controller_deinit();

int32_t register_nand_controller();
int32_t set_nand_controller(int8_t *name);
#endif
