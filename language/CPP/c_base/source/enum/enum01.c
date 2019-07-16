/**
 * 枚举类型
 * */

#include <stdio.h>
#include <stdlib.h>

enum spectrum {
    red,
    orange,
    yellow,
    green,
    blue,
    violet
};

int main(void)
{
    //TODO c++ 不支持color++操作，需要将声明成:int colort;
    enum spectrum color;

    int c;
    color = blue;

    if(color == blue) {
        printf("current color = %d \n", color);
    }

    for(color = red; color <= violet; color++) {
        printf("spectrum colors: %d\n", color);
    }

    return 0;
}
