/*
 * =====================================================================================
 *
 *       Filename:  chmod.c
 *
 *    Description:  修改文件权限
 *
 *        Version:  1.0
 *        Created:  03/01/2018 10:06:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int mode;
    if (argc < 3) {
        printf("Usage: ./chmod mode filename\n");
        exit(-1);
    }
    //"0644"->八进制
    mode = atoi(argv[1]);
    chmod(argv[2], mode);
    return 0;
}
