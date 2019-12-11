static void s3c2440_nand_controller_init()
{
}

static void s3c2440_nand_controller_deinit()
{
}

int32_t s3c2440_controller_init()
{
    t_nand_controller s3c2440_controller = {
        .name = "s3c2440_controller",
        .init = s3c2440_controller_init,
        .deinit = s3c2440_nand_controller_deinit,
    };

    nand_controller_register(s3c2440_controller);
}
