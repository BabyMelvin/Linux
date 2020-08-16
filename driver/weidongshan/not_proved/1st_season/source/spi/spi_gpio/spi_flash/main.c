#include <stdio.h>

int main(int argc, char *argv[])
{
    char str[200];
    unsigned int mid, pid;

    SPI_init();

    /* flash get id */
    SPI_flash_read_id(&mid, &pid);
    printf("mid = %d, pid = %d\n", *mid, *pid);

    /* flash program and read */
    SPI_flash_init(); // 去掉写保护
    SPI_flash_erase_sector(4096); // 檫除块
    SPI_flash_program(4096, "100ask", strlen("100ask")); //写入

    SPI_flash_read(4096, str, strlen("100ask")); // 读入
    
    return 0;
}
