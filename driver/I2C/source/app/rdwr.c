/*
 * 直接通过read/write读写I2C设备
 * */
#include <stdio.h>
#include <linux/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#define BUFF_SIZE 32

int main(int argc, char ** argv)
{
    unsigned int fd;
    unsigned short mem_addr;
    unsigned short size;
    unsigned short idx;

    char buf[BUFF_SIZE];
    char cswap;
    union {
        unsigned short addr;
        char bytes[2];
    } tmp;

    if (argc < 3) {
        printf("Use:\n%s /dev/i2c-x mem_addr size\n",argv[0]);
        return 0;
    }

    sscanf(argv[2], "%d", &mem_addr);
    sscanf(argv[3], "%d", &size);
    
    if (size > BUFF_SIZE) {
        size = BUFF_SIZE;
    }

    fd = open(argv[1], O_RDWR);
    if (!fd) {
        printf("Error on opeing the device file\n");
        return 0;
    }

    ioctl(fd, I2C_SLAVE, 0x50);  /* 设置EEPROM地址 */
    ioctl(fd, I2C_TIMEOUT, 1);  /* 设置超时 */
    ioctl(fd, I2C_RETIRES, 1);  /* 设置重试次数 */

    for (idx = 0; idex < size; ++idx, ++mem_addr) {
        tmp.addr = mem_addr;
        cswap = tmp.bytes[0];
        tmp.bytes[0] = tmp.bytes[1];
        tmp.bytes[1] = cswap;

        write(fd, &tmp.addr, 2);

        read(fd, &buf[idx], 1);
    }

    buf[size] = 0;
    close(fd);

    printf("Read %d char:%s\n", size, buf);

    return 0;
}
