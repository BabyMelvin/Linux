/*
 * =====================================================================================
 *
 *       Filename:  app_mem.c
 *
 *    Description:  字符设备应用层
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
#include <stdlib.h>
#include <stdio.h>
int main(){
    FILE*fp0=NULL;
    char buf[4096];
    //初始化buf
    strcpy(buf,"buf is char dev!");
    printf("buf:%s\n",buf);

    //打开设备文件
    fp0=fopen("/dev/memdev0","r+");
    if(fp0==NULL){
        printf("open memdev0 error\n");
        return -1;
    }
    //写入设备
    fwrite(buf,sizeof(buf),1,fp0);
    //重新定位文件位置
    fseek(fp0,0,SEEK_SET);
    //清楚buf
    strcpy(buf,"buf is NULL");
    printf("buf:%s\n",buf);

    //读出设备
    fread(buf,sizeof(buf),1,fp0);
    //检查结果
    printf("buf:%s\n",buf);
    return 0;
}

