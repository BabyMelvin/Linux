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
#include <stdlib.h>
#include <stdio.h>

int main(void){
    printf("PATH=%s\n",getenv("PATH");
    //1:override
    setenv("PATH","hello",1);
    printf("PATH=%s\n",getenv("PATH")); 
    return 0;
}

