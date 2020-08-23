#include <stdio.h>

/**
 *  需要32bit, 64bit有問題
 * */

struct person {
    char *name;
    int age;
    char score;
    int id;
};


/**
 * 1:packed:按照实际占用字节进行对齐
 *
 * 2.aligned(n):让成员对其n字节边界上
 *  如果结构体有成员变量字节长度大于n，则按照
 *  最大成员变量的字节长度来对齐
 * */

struct person_packed {
    char *name;
    int age;
    char score;
    int id;
} __attribute__((packed));


struct person_aligned {
    char *name;
    int age;
    char score;
    int id;
} __attribute__((aligned(4)));

/*
 * int printf(const char *format, ...); 
 * 依据:x86平台,函数调用时参数传递是使用堆栈来实现的
 *  目的:将所有传入的参数全部打印出来
 * */
int push_test (const char *format, ...)
{
    char *p = (char *)&format;
    int  i;
    struct person per;
    char c;
    double d;

    printf("arg1: %s\n", format);


    p = p + sizeof(char *);
    i = *((int *)p);
    printf("arg2: %d\n", i);

    p = p + sizeof(int);
    per = *((struct person *)p);
    printf("arg3: .name = %s, .age = %d, .scor=%c .id=%d\n",
            per.name, per.age, per.score, per.id);

    p = p + sizeof(struct person);
    c = *((char *)p);
    printf("arg4:%c\n", c);

    p = p + ((sizeof(char) + 3) & ~3);
    d = *((double *)p);
    printf("arg5: %f\n", d);

    return 0;
}

int main(int argc, char *argv[])
{
    struct person per = {"hange.com", 10, 'A', 123};

    printf("sizeof(char   )=%d\n",sizeof(char   ));
    printf("sizeof(int    )=%d\n",sizeof(int    ));
    printf("sizeof(char  *)=%d\n",sizeof(char  *));
    printf("sizeof(char **)=%d\n",sizeof(char **));
    
    /**
     * 64bit->24
     * 32bit->12
     * */
    printf("sizeof(struct  person)=%d\n",sizeof(struct  person));
    printf("sizeof (person_packed) = %d\n", sizeof(struct person_packed));
    printf("sizeof (person_align) = %d\n", sizeof(struct person_aligned));

    //push_test("abcd");
    //push_test("abcd", 123); 
    //push_test("abcd", 123, per);                
    //push_test("abcd",123,per,'c');       
    push_test("abcd", 123, per, 'c', 2.79);

    return 0;
}
