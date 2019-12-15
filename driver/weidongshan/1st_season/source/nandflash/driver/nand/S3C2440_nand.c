#include "nand_controller.h"

/* NAND FLASH S3C2440*/
typedef struct {
    S3C24X0_REG32 NFCONF;
    S3C24X0_REG32 NFCONT;
    S3C24X0_REG32 NFCMD;
    S3C24X0_REG32 NFADDR;
    S3C24X0_REG32 NFDATA;
    S3C24X0_REG32 NFMECCD0;
    S3C24X0_REG32 NFMECCD1;
    S3C24X0_REG32 NFSECCD;
    S3C24X0_REG32 NFSTAT;
    S3C24X0_REG32 NFESTAT0;
    S3C24X0_REG32 NFESTAT1;
    S3C24X0_REG32 NFMECC0;
    S3C24X0_REG32 NFMECC1;
    S3C24X0_REG32 NFSECC;
    S3C24X0_REG32 NFSBLK;
    S3C24X0_REG32 NFEBLK;
} S3C2440_NAND;

static void s3c2440_nand_reset(void);
static void s3c2440_wait_idle(void);
static void s3c2440_nand_select_chip(void);
static void s3c2440_nand_deselect_chip(void);
static void s3c2440_nand_write_cmd(int cmd);
static void s3c2440_nand_write_addr(unsigned int addr);
static void s3c2440_nand_read_data(void);


t_nand_chip s3c2440_nand_chip = {
    .name = "s3c2440_nand",
    .nand_reset = s3c2440_nand_reset,
    .nand_idle = s3c2440_wait_idle,
    .nand_select_chip = s3c2440_nand_select_chip,
    .nand_deselect_chip = s3c2440_nand_deselect_chip,
#ifdef LARGET_PAGE
    .write_cmd = s3c2440_nand_write_cmd_lp,
#else
    .write_cmd = s3c2440_nand_write_cmd,
#endif
    .write_addr = s3c2440_nand_read_data,
};
