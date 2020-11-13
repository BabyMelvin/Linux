/*
 * =====================================================================================
 *
 *       Filename:  access.c
 *
 *    Description:  检测文件的属性
 *                  F_OK:文件是否存在
 *                  R_OK:文件是否可读
 *                  W_OK:文件是否可写
 *                  X_OK:文件是否可执行
 *
 *        Version:  1.0
 *        Created:  03/01/2018 09:58:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hang Cao (Melvin), hang.yasuo@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    if (access("file", F_OK) < 0) {
        perror("access");
        exit(1);
    }
    printf("file 文件存在\n");
    return 0;
}
