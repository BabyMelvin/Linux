/**
 * 字符需要加'',否则会被转换成整型
 * NULL或'\0' ->0
 * '0'        ->48
 * 'a'        ->97
 * 'A'        ->65
 * */
#include <stdio.h>

int main(void)
{
   //字符'','\0'的Ascii码值是0
   char a = '\0';
   printf("a = %d\n", a); 

   //字符'0','0'的Ascii码值是48
   char b = '0';
   printf("b = %d\n", b); 

   char c = NULL; //NULL的ascii码值是0，所以NULL相当于0或者'\0'
   printf("c = %d\n", c);

   char d = 66; 
   printf("d = %c\n", d);
   return 0;
}
