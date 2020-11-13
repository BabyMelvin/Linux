/*
 * =====================================================================================
 *
 *       Filename:  env.c
 *
 *    Description:  环境变量操作
 *
 *        Version:  1.0
 *        Created:  03/01/2018 11:22:47 AM
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

int main(void)
{
    extern char** environ;
    int i;
    for (i = 0; environ[i] != NULL; i++) {
        printf("环境：%s\n", environ[i]);
    }

    printf("PATH=%s\n", getenv("PATH"));
    //1:override
    setenv("PATH", "hello", 1);
    printf("PATH=%s\n", getenv("PATH"));

    return 0;
}
