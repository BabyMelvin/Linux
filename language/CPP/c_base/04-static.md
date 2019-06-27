# static 

全局静态变量只会进行一次初始化

```
#include <stdio.h>

int get_static() {
    //只初始化一次,只在预编译时候
    static int a = 0;
    int b = 0;
    a ++;
    b ++;
    printf("a = %d,b = %d\n",a,b);
    //?? 省略return ,居然也会返回 a
    return a;
}
int main() {
   int i = 0;
   for(i; i<5; i++){
      printf("get _static_ a = %d\n",get_static());
   }
   return 0;
 }

```
