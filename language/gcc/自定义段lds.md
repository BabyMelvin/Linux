lds文件用于指定程序链接时的不同段内容的位置安排。linux内核镜像里就是利用lds文件，把不同的内容划分成很多不同的段.
uboot里的命令也就是利用此功能，把所有的命令统一到一个段里.

`arm-linux-gnueabihf-ld –verbose > test.lds`把编译器默认的链接脚本输出到test.lds里

修改test.lds,删除掉注释行
在84行加入自己的段:

```lds
__mysection_start = . ;
.mysection  :
{
*(.mysection);
}
.align = 4;
__mysection_end = . ;
```

* `__mysection_start` 和 `__mysection_end`用于记录段的开始地址与结束地址.
* `*(.mysection)`表示所有的`.o`文件的`.mysection`段内容集中放起来

在代码里指定变量是分配在.mysection段:

```lds
int num  __attribute__ ((section(".mysection"))) = 1;
int num2 __attribute__ ((section(".mysection"))) = 2;
int num3 __attribute__ ((section(".mysection"))) = 3;
```
编译时指定使用test.lds链接脚本:

`arm-linux-gnueabihf-gcc test.c -Ttest.lds -o test`

编译完成后，可以反汇编确认.mysection里的状况:`arm-linux-gnueabihf-objdump -D test `
查看到的输出:

```lds
    Disassembly of section .mysection:

    000083d4 <num>:
        83d4:       00000001        andeq   r0, r0, r1

    000083d8 <num2>:
        83d8:       00000002        andeq   r0, r0, r2

    000083dc <num3>:
        83dc:       00000003        andeq   r0, r0, r3
```
加进一个段后, 可以通过`__mysection_start`和`__mysection_end`算出一个段的大小，及段里的变量个数.
也可以取出每个段里变量的值.

在代码里:
* `__mysection_start`相当于一个变量(类型不重要)，它的地址就是段的开始地址
* `__mysection_end`， 它的地址就是段的结束地址
实验代码:
```c
//test.c
#include <stdio.h>

int num __attribute__ ((section(".mysection"))) = 1;
int num2 __attribute__ ((section(".mysection"))) = 2;
int num3 __attribute__ ((section(".mysection"))) = 3;
int num4 __attribute__ ((section(".mysection"))) = 55; 

extern int __mysection_start; //这个变量是在链接脚本里声明的
extern int __mysection_end;
int main(void)
{
    int *p = &__mysection_start;

    for (; p < &__mysection_end; p++)
    {   
    printf("%d\n", *p);
    }   

    return 0;
}
```
以后再加变量到`.mysection`段里，main函数的代码可以不用修改都可以获取到相应的变量的值

实现命令的功能:
```c
//test.c

#include <stdio.h>

typedef struct {
    char *name;  //命令的名字
    char *help; // 命令的帮助信息
    void (*func)(void); //命令的功能函数
}cmd_t;

#define MySection  __attribute__ ((section(".mysection")))

#define ADD_CMD(name, help, func) \
    cmd_t __cmd_##name MySection = {#name, help , func}  

void do_cmd1(void)
{
    printf("in %s\n", __func__);
}
ADD_CMD(cmd1, "help of cmd1", do_cmd1);


extern cmd_t __mysection_start;
extern cmd_t __mysection_end;
int main(void)
{
    cmd_t *p = &__mysection_start;      

    for (; p < &__mysection_end; p++)
    {
        printf("%s\n", p->name);
        p->func();
        printf("#################\n");
    }
    return 0;
}
```
再增加命令时，只需实现命令的功能函数后， 调用ADD_CMD宏即可.

也可以把每个命令用一个源文件来实现，最后一起编译即可。uboot里的命令就是采用这样的方法.
