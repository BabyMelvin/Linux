# 静态库链接

静态库：一组目标文件的集合(/usr/lib/libc.a glibc项目的一部分)

一种语言会附带语言库，是对系统API进行封装.

printf:Linux下是调用write系统调用，WINDOWS是WriteConsole系统API

ar工具管理静态库`ar -t libc.a`查看文件内容

```c
// 查看文件内容(.o有哪些函数)
$objdump -t libc.a

printf.o: file format elf32-i386

SYMBOL TABLE:
...
...     F .text 00000026 printf
```

> $gcc -c -fno-builtin hello.o

默认情况下，GCC会自作聪明将printf替换成puts函数，提高运行速度。`-fno-builtin`关闭这个内置函数优化选项

> $ar -x libc.a  //解压libc.a

`--verbose`表示将整个编译链接过程的中间步骤打印出来
> $gcc -static --verbose -fno-builtin hello.c

输出信息
```
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/4.8/lto-wrapper
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 4.8.4-2ubuntu1~14.04.4' --with-bugurl=file:///usr/share/doc/gcc-4.8/README.Bugs --enable-languages=c,c++,java,go,d,fortran,objc,obj-c++ --prefix=/usr --program-suffix=-4.8 --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --with-gxx-include-dir=/usr/include/c++/4.8 --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --enable-gnu-unique-object --disable-libmudflap --enable-plugin --with-system-zlib --disable-browser-plugin --enable-java-awt=gtk --enable-gtk-cairo --with-java-home=/usr/lib/jvm/java-1.5.0-gcj-4.8-amd64/jre --enable-java-home --with-jvm-root-dir=/usr/lib/jvm/java-1.5.0-gcj-4.8-amd64 --with-jvm-jar-dir=/usr/lib/jvm-exports/java-1.5.0-gcj-4.8-amd64 --with-arch-directory=amd64 --with-ecj-jar=/usr/share/java/eclipse-ecj.jar --enable-objc-gc --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --with-tune=generic --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.4) 
COLLECT_GCC_OPTIONS='-static' '-v' '-fno-builtin' '-mtune=generic' '-march=x86-64'
 /usr/lib/gcc/x86_64-linux-gnu/4.8/cc1 -quiet -v -imultiarch x86_64-linux-gnu for.c -quiet -dumpbase for.c -mtune=generic -march=x86-64 -auxbase for -version -fno-builtin -fstack-protector -Wformat -Wformat-security -o /tmp/ccEE0zC1.s
 GNU C (Ubuntu 4.8.4-2ubuntu1~14.04.4) version 4.8.4 (x86_64-linux-gnu)
    compiled by GNU C version 4.8.4, GMP version 5.1.3, MPFR version 3.1.2-p3, MPC version 1.0.1
    GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
    ignoring nonexistent directory "/usr/local/include/x86_64-linux-gnu"
    ignoring nonexistent directory "/usr/lib/gcc/x86_64-linux-gnu/4.8/../../../../x86_64-linux-gnu/include"
    #include "..." search starts here:
    #include <...> search starts here:
     /usr/lib/gcc/x86_64-linux-gnu/4.8/include
      /usr/local/include
       /usr/lib/gcc/x86_64-linux-gnu/4.8/include-fixed
        /usr/include/x86_64-linux-gnu
         /usr/include
         End of search list.
         GNU C (Ubuntu 4.8.4-2ubuntu1~14.04.4) version 4.8.4 (x86_64-linux-gnu)
            compiled by GNU C version 4.8.4, GMP version 5.1.3, MPFR version 3.1.2-p3, MPC version 1.0.1
            GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
            Compiler executable checksum: 57fa36089caee02f75f0ef5cfa9d597b
            COLLECT_GCC_OPTIONS='-static' '-v' '-fno-builtin' '-mtune=generic' '-march=x86-64'
             as -v --64 -o /tmp/ccg5xciL.o /tmp/ccEE0zC1.s
             GNU assembler version 2.24 (x86_64-linux-gnu) using BFD version (GNU Binutils for Ubuntu) 2.24
             COMPILER_PATH=/usr/lib/gcc/x86_64-linux-gnu/4.8/:/usr/lib/gcc/x86_64-linux-gnu/4.8/:/usr/lib/gcc/x86_64-linux-gnu/:/usr/lib/gcc/x86_64-linux-gnu/4.8/:/usr/lib/gcc/x86_64-linux-gnu/
             LIBRARY_PATH=/usr/lib/gcc/x86_64-linux-gnu/4.8/:/usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu/:/usr/lib/gcc/x86_64-linux-gnu/4.8/../../../../lib/:/lib/x86_64-linux-gnu/:/lib/../lib/:/usr/lib/x86_64-linux-gnu/:/usr/lib/../lib/:/usr/lib/gcc/x86_64-linux-gnu/4.8/../../../:/lib/:/usr/lib/
             COLLECT_GCC_OPTIONS='-static' '-v' '-fno-builtin' '-mtune=generic' '-march=x86-64'
              /usr/lib/gcc/x86_64-linux-gnu/4.8/collect2 --sysroot=/ --build-id -m elf_x86_64 --hash-style=gnu --as-needed -static -z relro /usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu/crt1.o /usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/4.8/crtbeginT.o -L/usr/lib/gcc/x86_64-linux-gnu/4.8 -L/usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu -L/usr/lib/gcc/x86_64-linux-gnu/4.8/../../../../lib -L/lib/x86_64-linux-gnu -L/lib/../lib -L/usr/lib/x86_64-linux-gnu -L/usr/lib/../lib -L/usr/lib/gcc/x86_64-linux-gnu/4.8/../../.. /tmp/ccg5xciL.o --start-group -lgcc -lgcc_eh -lc --end-group /usr/lib/gcc/x86_64-linux-gnu/4.8/crtend.o /usr/lib/gcc/x86_64-linux-gnu/4.8/../../../x86_64-linux-gnu/crtn.o
```

关键三步:

* ccl程序：GCC的c语言编译器，将for.c编译成一个临时汇编程序`/tmp/ccEE0zC1.s`
* as程序是GUN汇编器，将`/tmp/ccEE0zC1.s`会变成`/tmp/ccg5xciL.o`
* `collect2`执行最后的链接。
    * collect2看作一个ld链接器的包装，调用ld链接器完成对目标文件的链接

将collect2看成是链接器,最后一步包含下面几个库文件

* crt1.o
* crti.o
* crtbeginT.o
* libgcc.a
* libgcc_eh.a
* libc.a
* crtend.o
* crtn.o

## 2.链接过程的控制

大多数使用默认链接器对目标文件进行链接就可以了。但是特殊操作系统内核，对程序的各个段地址有着特殊的要求.

整个链接过程需要确定：

* 使用哪些目标文件
* 使用哪些库文件
* 是否在最终可执行文件中保留调试信息
* 输出文件格式
* 考虑是否到处某些符号以供调试器或程序本身或其他程序使用等

Windows内核： windows/system32/notskernl.exe

### 2.1 链接控制脚本

链接器提供多种控制整个链接过程的方法:

* 使用命令行
* 指令存在目标文件里面，编译器经常通过这种方法象链接器传递指令。
* 使用链接控制脚本，灵活和强大


> $ld -verbose //查看默认的链接脚本

默认的脚本位置`/usr/lib/ldscripts`,不同的机器平台输出格式都有相应的链接脚本

* Intel IA32下普通可执行ELF文件链接脚本为elf_i386.x,共享库的脚本为elf_i386.xs

> ld -T link.script //制定控制脚本

### 2.2 最"小"的程序

* 程序脱离 C语言库，独立于任何库的纯正的"程序"
* 使用nomain作为函数入口程序
* 经典C库会产生很多段，我们将小程序所有段都合并到一个叫tinytext的段(为任意名称，由链接脚本控制链接过程生成的)

```c
// TinyHelloWorld.c 源码
char *str = "Hello world!\n";

void print()
{
    // 调用Write 系统调用
    asm( "movl $13,%%edx \n\t"
         "movl %0,%%ecx \n\t"
         "movl $0,%%ebx \n\t"
         "movl $4,%%eax \n\t"
         "int $0x80 \n\t"
         ::"r"(str):"edx","ecx","ebx");
}

void exit()
{
     // 调用EXIT系统调用
     asm("movl $42,%ebx \n\t"
         "movl $1,%eax \n\t"
         "int $0x80 \n\t");
}
void nomain()
{
     print();
     exit();
}
```

`int write(int filedesc, char *buffer, int size)`

* WRITE系统调用号为4，eax=0
* filedesc输入文件句柄，使用ebx寄存器，stdout为0，则ebx=0
* buffer要写入缓冲区，ecx寄存器传递，ecx=str
* size写入字节数，edx寄存器传递,edx=13

EXIT系统调用，进程退出码.平时main程序return数值会返回给系统库，由系统库将该数值传递给EXIT系统调用.这样父进程接收到子进程的退出码。EXIT系统调用为1，eax=1 `$?` 获得退出码

64bit编译时候要指定成`-m32`和`-m elf_i386`目标平台(找到正确的链接脚本)

```makefile
all:
    gcc -c -fno-builtin -m32 tinyhello.c
    ld -static -e nomain -m elf_i386 -o tinyhello tinyhello.o
all_lds:
   gcc -c -fno-builtin -m32 tinyhello.c
   ld -static -T tiny_hello.lds -m elf_i386 -o tinyhello tinyhello.o
```

* `-fno-builtin`关闭gcc内置优化内容
* `-e`制定程序入口

这时候还是有几个段`objdump -h`

```
tinyhello:     file format elf32-i386

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .eh_frame     0000007c  08048074  08048074  00000074  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 tinytext      00000052  080480f0  080480f0  000000f0  2**2
                  CONTENTS, ALLOC, LOAD, CODE
```

`make all_lds`得到最小ELF可执行文件，45字节，退出码是42.

42来源`<<银河系漫游指南>>`里面的终极电脑给出的关于生命、宇宙及万物终极答案是42
