/**
 * 嵌套结构体
 **/

#include <stdio.h>
#include <stdlib.h>
#define LEN 20

const char *msgs[] = {
    "   Thank you for the wonderful evening",
    "You certainly prove that a",
    "is a special kind of guy. We must get together",
    "over a delicious",
    "and have a few laughs"
};

struct names {
    char first[LEN];
    char last[LEN];
};

struct guy {
    struct names handle;
    char favfood[LEN];
    char job[LEN];
    float income;
};

int main(void)
{
    struct guy fellow = {
        {"Bwen", "Villard"},
        "grilled salmon",
        "personally coach",
        58112.00
    };

    //成员访问 fellow.handle.last
    printf("Dear %s, \n\n", fellow.handle.first);
    printf("%s%s.\n", msgs[0], fellow.handle.first);
    printf("%s%s.\n", msgs[1], fellow.job);
    printf("%s.\n", msgs[2]);
    if(fellow.income > 150000.0)
        puts("!!");
    else if(fellow.income > 75000.0)
        puts("!");

    //输出40个空格方法
    printf("\n%40s%s\n", " ", "See you soon\n");
    printf("%40s%s\n", " ", "Shalala");

    return 0;
}
