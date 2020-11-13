/*
 * =====================================================================================
 *
 *       Filename:  system.c
 *
 *    Description:  指定系统一些命名
 *
 *        Version:  1.0
 *        Created:  03/01/2018 11:52:34 AM
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

int main()
{
    system("ls -al /etc/passwd");
    for (;;)
        ;
    return 0;
}
