#include <stdio.h>

/**
 * 64bit: gcc -m32 -o printf_test printf_test.c
 *  sudo  apt-get  update
 *  sudo  apt-get  purge  libc6-dev
 *  sudo  apt-get  install  libc6-dev
 *  sudo  apt-get  install  libc6-dev-i386
 * 32bit: gcc -o printf_test printf_test.c
 * */

void printf_test()
{
    printf("hello world\n");
    printf("This is hange.com\n");
    printf("test char           =%c,%c\n", 'A','a') ;
    printf("test decimal number =%d\n",    123456) ;
    printf("test decimal number =%d\n",    -123456) ;
    printf("test hex     number =0x%x\n",  0x55aa55aa)   ;
    printf("test string         =%s\n",    "www.100ask.org") ;  
}
int main(int argc, char *argv[])
{
    printf_test();
    return 0;
}
