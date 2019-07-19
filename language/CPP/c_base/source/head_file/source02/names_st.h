#include <string.h>
//常量
#define SLEN 32
//结构声明
struct names_st
{
    char first[SLEN];
    char last[SLEN];
};

//类型定义
typedef struct names_st names;

//函数原型
void get_names(names *);

void show_names(const names *);

char *s_gets(char *st, int n);
