/*
 * 读取I2C控制器上某I2C从设备的某寄存器，如读I2C控制器0上的地址为0x18的从设备，从寄存器0x20开始读取
 * #i2c-test /dev/i2c-0 0x18 0x20
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

int main(int argc, char **argv)
{
    struct i2c_rdwr_ioctl_data work_queue;
    unsigned  int idx;
    unsigned int slave_address, reg_address;
    unsigned char val;
    int i;
    int ret;

    if (argc < 4) {
        printf("Usage:\n%s /dev/i2c-x start_addr reg_addr\n", argv[0]);
        return 0;
    }

    fd = open(argv[1], O_RDWR);
    if (!fd) {
        printf("Error on opening the device file\n");
        return 0;
    }

    sscanf(argv[2], "%x", &slave_address); 
    sscanf(argv[3], "%x", &reg_address);

    work_queue.nmsgs = 2; /* 消息数量 */
    work_queue.msgs = (struct i2c_msg *)malloc(work_queue.nmsgs * sizeof(struct i2c_msg));

    if (!work_queue.msgs) {
        printf("Memory alloc error\n");
        close(fd);
        return 0;
    }

    ioctl(fd, I2C_TIMEOUT, 2); /* 设置超时时间 */
    ioctl(fd, I2C_RETRIES, 1); /* 设置重试次数 */

    for (i = reg_address; i < reg_address + 16; i++) {
        val  = i;
        (work_queue.msgs[0]).len = 1;
        (work_queue.msgs[0]).addr = slave_address;
        (work_queue.msgs[0]).buf = &val;

        (work_queue.msgs[1]).len = 1;
        (work_queue.msgs[1]).flags = I2C_M_RD;
        (work_queue.msgs[1]).addr = slave_address;
        (work_queue.msgs[1]).buf = &val;

        ret = ioctl(fd, I2C_RDWR, (unsigned long)&work_queue);
        if (ret <0)
            printf("Error during I2C_RDWR ioctl with error code:%d\n",ret);
        else
            printf("reg:%02x val:%02x\n", i, val);

    }
    close(fd);
    return 0;
}
