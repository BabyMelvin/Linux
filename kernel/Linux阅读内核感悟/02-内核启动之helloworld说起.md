> 内核是从哪里开始执行的呢？

几乎任何一本 Linux 内核源代码分析的书都会给出详细的答案。不过，我试图从一个不同的角度（一个初学者的角度）来叙述，而不是一上来就给出答案。从熟悉的事物入手，慢慢接近陌生的事物，这是比较常见的思路。既然都是二进制代码，那么不妨从最简单的用户态 C 程序，hello world 开始。说不定能找到共同点。恰好我是一个喜欢寻根究底的人。也许，理解了 hello world 程序的启动过程，有助于更好地理解内核的启动。

好，开始寻根究底吧。从普通的 C 语言用户态程序开始写。先写一个简单的 hello world 程序。

```c
/* helloworld.c*/
#include <stdio.h>
int main()
{
    printf("hello world\n");
    return 0;
}
```

然后`gcc helloworld.c -o helloworld`，一个最简单的 hello world 程序出现了。

# 1.汇编
它是从哪里开始执行的呢？这还不简单？main 函数么。地球人都知道。为什么一定要从 main 函数开始呢？于是，我开始琢磨这个 hello world 程序。`file helloworld`可知，它是一个 elf 可执行文件。反汇编试试`objdump -d helloworld`。

反汇编的结果令人吃惊，因为出现了`_start()`等一堆函数。一定是 gcc 编译时默认链接了一些库函数。其实，只要运行`gcc -v helloworld.c -o helloworld`就会显示gcc详细的编译链接过程。其中包括链接`/usr/lib/`下的`crti.o crt1.o crtn.o`等等文件。用 objdump 查看，`_start()`函数就定义在 crt1.o 文件中。


# 2.readelf
那么 helloworld 的真正执行的入口在哪里呢？我们可以使用`readelf`来查看，看有没有有用信息。`readelf -a helloworld`helloworld 作为一个 elf 文件，有 elf 文件头，section table 和各个 section 等等。有兴趣可以去看看 elf 文件格式的文档。

用 readelf 可知，在 helloworld 的 elf 文件头的信息中，有这么一项信息：

```
入口点地址： 0x80482c0
```
可见，helloworld 程序的入口地址在 0x80482c0 处，而由 objdump 得：`080482c0 <_start>:`可见，`_start()`是 helloworld 程序首先执行的函数。`_start()`执行完一些初始化工作后，经过层层调用，最终调用`main()`.可以设想，如果_start()里最终调用的是 foo()，那么 C 程序的主函数就不再是 main()，而是 foo()了。

# 3.strace
helloworld 程序具体是如何执行的呢。我们只能猜测是由 bash 负责执行的。然而具体看 bash 代码就太复杂了。我们可以用 strace 跟踪 helloworld 的执行。`strace ./helloworld`

出来一大堆函数调用。其中第一个是`execve()`.这是一个关键的系统调用，它负责载入helloworld 可执行文件并运行。其中有很关键的一步，就是把用户态的 **eip 寄存器**（实际上是它在内存中对应的值）设置为 elf 文件中的入口点地址，也就是`_start()`。具体可见内核中的`sys_execve()`函数。


# 4.小节
由此可见，程序从哪里开始执行，取决于在刚开始执行的那一刻的 eip 寄存器的值。
而这个 eip 是由其它程序设置的，在这里，eip 是由 Linux 内核设置的。具体过程如下：

* 1.用户在 shell 里运行./helloworld。
* 2.shell（这里是 bash）调用系统调用 execve()。
* 3.execve 陷入到内核里执行 sys_execve()，把用户态的 eip 设置为_start()。
* 4.当系统调用执行完毕，helloworld 进程开始运行时，就从_start()开始执行
* 5.helloworld 进程最后才执行到 main()。

参考:[elf 文件格式](http://www.skyfree.org/linux/references/ELF_Format.pdf)

