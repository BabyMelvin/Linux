#ifndef __ELLA_NADND_H__
#define __ELLA_NADND_H__

#define GSTATUS1 (*(volatile unsigned int *)0x560000B0)

int32_t ella_nand_read(int8_t *addr);
int32_t ella_nand_write_cmd(int cmd);
int32_t ella_nand_write_data();
int32_t ella_nand_init();
#endif
