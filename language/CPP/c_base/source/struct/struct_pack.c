#include <stdio.h>

/*
 * 两种测试平台：
 *  gcc -m32 -o strcut_pack struct_pack.c
 *  gcc -o struct_pack struct_pack
 * */

/*
 * 在x86(32平台)，GCC编译器默认按4字节对齐
 *      如:结构体4字节对齐，即结构体成员所在的内存地址是4的整数倍 
 * 可以通过gcc中__attribute__选项来设置指定的对齐大小
 * 1:__attribute__((packed))，让所作用的结构体取消在编译过程的优化对齐，按照
 *  实际占用字节数进行对齐
 * 2:__attribute__(aligned (n)),让所作用的结构体成员对齐在n字节边界上。
 *  如果结构体中有成员变量的字节长度大于n，则按照最大成员变量的字节长度来对齐
 **/

#if defined(_WIN64) || defined(WIN64)
#warning "64 bit platform\n"
#elif defined(_WIN32) || defined(WIN32)
#warning "32 bit paltform\n"
#elif defined(__x86_64__)
#warning "x86_64"
#define PRI_INT  "ld"
#elif defined(__i386__)
#warning "__i386__"
#define PRI_INT  "d"
#else
#warning "else"
#endif


struct person {
    char *name;
    int age;
    char score;
    int id;
};

struct person1 {
    char *name;
    int age;
    char score;
    int id;
}__attribute__((packed));

struct person2 {
    char *name;
    int age;
    char score;
    int id;
}__attribute__((aligned(4)));

struct person3 {
    char name[10];
    int age;
    char score;
    int id;
};

int main(int argc, char **argv)
{
    struct person per = {"www.baidu.com", 10, 'A', 123};
    printf("sizeof(char   ) = %" PRI_INT "\n", sizeof(char)); // 1
    printf("sizeof(int    ) = %"PRI_INT"\n", sizeof(int));  // 4
    printf("sizeof(char * ) = %"PRI_INT"\n", sizeof(char *)); //4(m32)   8(m64)
    printf("sizeof(char **) = %"PRI_INT"\n", sizeof(char **)); //4(m32)  8(m32) 

    printf("sizeof(struct persion)  = %"PRI_INT"\n", sizeof(struct person)); //16(m32)   24(m64)
    printf("sizeof(struct persion1) = %"PRI_INT"\n", sizeof(struct person1)); //13(m32)  17(m64)
    printf("sizeof(struct persion2) = %"PRI_INT"\n", sizeof(struct person2)); //16(m32)   24(m64)
    printf("sizeof(struct persion2) = %"PRI_INT"\n", sizeof(struct person3)); //24(m32)   24(m64) 

    return 0;
}
