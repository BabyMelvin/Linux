/**
 * 使用参数
 * */

#include <stdio.h>

//宏函数和函数一样，后面需要加()
#define SQURE(X) (X)*(X)
#define PX(X) printf("The result is %d.\n", (X))

#define GET_SWITCH(RET) \
          ({ \
            int a; \
            switch(RET){ \
            case 1: a = 2;break; \
            case 2: a = 5;break; \
            default: a = 3;break; \
           } a;})

int main(void)
{
    int x = 4;
    int z;

    printf("x = %d\n", x);
    z = SQURE(x);

    printf("Evaluating SQURE(x):");
    PX(x);

    return 0;
}
