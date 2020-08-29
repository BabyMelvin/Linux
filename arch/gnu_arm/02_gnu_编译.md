# 第二部分 GNU的编译器和调试工具
## 一. 编译工具

> 1．编辑工具介绍

GNU 提供的编译工具包括`汇编器as`、`C编译器gcc`、`C++编译器g++`、`连接器ld`和`二进制转换工具objcopy`。基于ARM平台的工具分别为`arm- linux-as`、`arm-linux-gcc`、`arm-linux-g++`、`arm-linux-ld`和`arm-linux- objcopy`。GNU的编译器功能非常强大，共有上百个操作选项，这也是这类工具让初学者头痛的原因。不过，实际开发中只需要用到有限的几个，大部分可 以采用缺省选项。GNU工具的开发流程如下：编写C、C++语言或汇编源程序，用gcc或g++生成目标文件，编写连接脚本文件，用连接器生成最终目标文 件（elf格式），用二进制转换工具生成可下载的二进制代码。

（1）编写C、C++语言或汇编源程序
通常汇编源程序用于系统最基本的初始化，如初始化堆栈指针、设置页表、操作ARM的协处理器等。初始化完成后就可以跳转到C代码执行。需要注意的 是，GNU的汇编器遵循AT&T的汇编语法，读者可以从GNU的站点（www.gnu.org）上下载有关规范。汇编程序的缺省入口是 start标号，用户也可以在连接脚本文件中用ENTRY标志指明其它入口点（见下文关于连接脚本的说明）。
（2）用gcc或g++生成目标文件
如果应用程序包括多个文件，就需要进行分别编译，最后用连接器连接起来。如笔者的引导程序包括3个文件：`init.s`（汇编代码、初始化硬件）`xmrecever.c`（通信模块，采用Xmode协议）和flash.c（Flash擦写模块）。

分别用如下命令生成目标文件： `arm-linux-gcc -c -O2-oinit.o init.s`, `arm-linux-gcc -c -O2 -o xmrecever.o xmrecever.c`, `arm-linux-gcc -c -O2 -o flash.o flash.c` 其中-c命令表示只生成目标代码，不进行连接；-o命令指明目标文件的名称；-O2表示采用二级优化，采用优化后可使生成的代码更短，运行速度更快。如果 项目包含很多文件，则需要编写makefile文件。关于makefile的内容，请感兴趣的读者参考相关资料。
（3）编写连接脚本文件
gcc 等编译器内置有缺省的连接脚本。如果采用缺省脚本，则生成的目标代码需要操作系统才能加载运行。为了能在**嵌入式系统**上直接运行，需要编写自己的连接脚本文 件。编写连接脚本，首先要对目标文件的格式有一定了解。GNU编译器生成的目标文件缺省为elf格式。elf文件由若干段（section）组成，如不特 殊指明，由C源程序生成的目标代码中包含如下段：`.text`（正文段）包含程序的指令代码；`.data`(数据段)包含固定的数据，如常量、字符 串；`.bss`（未初始化数据段）包含未初始化的变量、数组等。C++源程序生成的目标代码中还包括`.fini`（析构函数代码）和`.init`（构造函数代码）等。连接器的任务就是将多个目标文件的`.text`、`.data`和`.bss`等段连接在一起，而连接脚本文件是告诉连接器从什么地址 开始放置这些段。例如连接文件link.lds为：

```
ENTRY(begin)
SECTION
{
	.=0x30000000;
	.text:{*(.text)}
	.data:{*(.data)}
	.bss:{*(.bss)}
}
```

其中，ENTRY(begin)指明程序的入口点为begin标号；`.=0x00300000`指明目标代码的起始地址为0x30000000，这一段地址 为 MX1的片内RAM；`.text:{*(.text)}`表示从0x30000000开始放置所有目标文件的代码段，随后的`.data:{* (.data)}`表示数据段从代码段的末尾开始，再后是`.bss`段。

（4）用连接器生成最终目标文件
有了连接脚本文件，如下命令可生成最终的目标文件：
`arm-linux-ld –no stadlib –o bootstrap.elf -Tlink.lds init.o xmrecever.o flash.o`

* 其中，ostadlib表示不连接系统的运行库，而是直接从**begin入口**；
* `-o`指明目标文件的名称；
* `-T`指明采用的连接脚本文件（也可以使用`-Ttext address`，address表示执行区地址）；最后是需要连接的目标文件列表。

（5）生成二进制代码
连接生成的elf文件还不能直接下载执行，通过objcopy工具可生成最终的二进制文件：

`arm-linux-objcopy –O binary bootstrap.elf bootstrap.bin`

* 其中`-O binary`指定生成为二进制格式文件。Objcopy还可以生成S格式的文件，只需将参数换成`-O srec`。还可以使用`-S`选项，移除所有的**符号信息及重定位信息**。如果想将生成的目标代码反汇编，还可以用objdump工具：`arm-linux-objdump -D bootstrap.elf`

至此，所生成的目标文件就可以直接写入Flash中运行了。

> 2．Makefile实例

```
example: head.s main.c
arm-linux-gcc -c -o head.o head.s
arm-linux-gcc -c -o main.o main.c
arm-linux-ld -Tlink.lds head.o ain.o -o example.elf
arm-linux-objcopy -O binary -S example_tmp.o example
arm-linux-objdump -D -b binary -m arm example >ttt.s
```

## 二. 调试工具

Linux 下的GNU调试工具主要是`gdb`、`gdbserver`和`kgdb`。其中gdb和gdbserver可完成对目标板上Linux下应用程序的远程调试。 gdbserver是一个很小的应用程序，运行于目标板上，可监控被调试进程的运行，并通过串口与上位机上的gdb通信。开发者可以通过上位机的gdb输 入命令，控制目标板上进程的运行，查看内存和寄存器的内容。gdb5.1.1以后的版本加入了对ARM处理器的支持，在初始化时加入`－ target==arm`参数可直接生成基于ARM平台的gdbserver。gdb工具可以从[gdb](ftp: //ftp.gnu.org/pub/gnu/gdb/)上下载。
对于Linux内核的调试，可以采用kgdb工具，同样需要通过串口与上位机上的gdb通信，对目标板的Linux内核进行调试。可以从[内核调试](http://oss.sgi.com/projects/kgdb/)上了解具体的使用方法。
