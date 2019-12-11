#ifndef __NAND_CONTROLLER_H__
#define __NAND_CONTROLLER_H__

#define MAX_NUM_CONTROLLER 5

typedef struct {
    int8_t *name;
    void (*init)(void);
    void (*deinit)(void);
} t_nand_controller, *p_nand_controller;

int32_t nand_controller_init();
#endif
