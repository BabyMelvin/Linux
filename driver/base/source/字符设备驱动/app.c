/*
 * =====================================================================================
 *
 *       Filename:  app_mem.c
 *
 *    Description:  字符设备应用层
 *                      read write ioctl
 *
 *        Version:  1.0
 *        Created:  04/08/2018 03:43:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "memdev.h"
#include <stdio.h>
#include <stdlib.h>
int main()
{
    FILE* fp0 = NULL;
    char buf[4096];
    //初始化buf
    strcpy(buf, "buf is char dev!");
    printf("buf:%s\n", buf);

    //打开设备文件
    fp0 = fopen("/dev/memdev0", "r+");
    if (fp0 == NULL) {
        printf("open memdev0 error\n");
        return -1;
    }
    //写入设备
    fwrite(buf, sizeof(buf), 1, fp0);
    //重新定位文件位置
    fseek(fp0, 0, SEEK_SET);
    //清楚buf
    strcpy(buf, "buf is NULL");
    printf("buf:%s\n", buf);

    //读出设备
    fread(buf, sizeof(buf), 1, fp0);
    //检查结果
    printf("buf:%s\n", buf);

    //--------------IOCTL-------------------------
    //调用命令MEMDEV_IOCPRINT
    int cmd;
    int arg;
    cmd = MEMDEV_IOCPRINT;
    if (ioctl(fd, cmd, &arg) < 0) {
        printf("call cmd MEMDEV_IOCPRINT fail\n");
        return -1;
    }
    //调用命令MEMDEV_IOCSETDATA
    cmd = MEMDEV_IOCSETDATA;
    arg = 2007;
    if (ioctl(fd, cmd, &arg) < 0) {
        printf("call cmd MEMDEV_IOCSETDATA fail\n");
        return -1;
    }
    //调用命令MEMDEV_IOCGETDATA
    cmd = MEMDEV_IOCGETDATA;
    if (ioctl(fd, cmd, &arg) < 0) {
        printf("call cmd MEMDEV_IOCGETDATA fail\n");
        return -1;
    }
    printf("in user space memdev_iocgetdata get data is %d\n", arg);
    close(fp0);
    //--------------------POLL-----------------------------
    fd_set rds;
    fp0 = open("dev/memdev0", O_RDWR);
    FD_ZERO(&rds);
    FD_SET(fd, &rds);
    ret = select(fd + 1, &rds, NULL, NULL, NULL);
    if (ret < 0) {
        printf("select error\n");
        exit(1);
    }
    if (FD_ISSET(fd, &rds)) {
        read(fd, buf, sizeof(buf));
    }
    close(fp0);
    return 0;
}
