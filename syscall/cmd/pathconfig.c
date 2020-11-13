/*
 * =====================================================================================
 *
 *       Filename:  pathconfig.c
 *
 *    Description: 运行时限定值 
 *                 pathconfig:获取路径一些限定信息
 *                 sysconf:获得与文件目录无关限定值
 *                 fpathconf:文件相关限定
 *
 *        Version:  1.0
 *        Created:  03/01/2018 11:41:30 AM
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
    char pathname[256] = "/home";
    printf("%s:\n", pathname);
    printf("NAME_MAX=%d\n", pathconf(pathname, _PC_NAME_MAX));
    printf("PATH_MAX=%d\n", pathconf(pathname, _PC_PATH_MAX));
    printf("LINK_MAX=%d\n", pathconf(pathname, _PC_LINK_MAX));
    printf("MAX_INPUT=%d\n", pathconf(pathname, _PC_MAX_INPUT));
    printf("PIPE_BUF=%d\n", pathconf(pathname, _PC_PIPE_BUF));
    return 0;
}
