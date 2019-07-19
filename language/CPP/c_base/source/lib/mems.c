/**
 *void *memcpy(void *restrict s1, const void *restrict s2, size_t n); 
 *void *memmove(void *s1, const void *s2, size_t n); 
 * memcpy带关键字:restrict，假设两个内存之间没有重叠
 * */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 10

void show_array(const int ar[], int n);

int main(void)
{
    int values[SIZE] = {1,2,3,4,5,6,7,8,9,10};
    int targets[SIZE];
    double curious[SIZE/2] = {2.0,2.0e5,2.0e10,2.0e20,5.0e30};

    puts("memcpy() used:");
    puts("values(orignial data):");
    show_array(values, SIZE);
    puts("\nUsing memmove() with overlapping ranges:");
    memmove(values+2, values, 5*sizeof(int));
    puts("values -- elements 0-4 copied to 2-6");
    show_array(values, SIZE);
    puts("\nUsing memcpy() to copy doule to int:");
    //memcpy()函数不知道也不关心数据的类型，只负责从一个位置把一些字节拷贝到
    //另一个位置,double类型转换为int类型
    memcpy(targets, curious, (SIZE/2)*sizeof(double));
    puts("targets -- 5 doubles into 10 int positions:");
    show_array(targets, SIZE/2);
    show_array(targets + 5, SIZE/2);

    return 0;
}

void show_array(const int a[], int n)
{
    int i;
    for(i =0; i<n; i++)
        printf("%d ", a[i]);

    putchar('\n');
}
