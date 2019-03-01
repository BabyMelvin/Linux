# arm-linux-ld选项
arm-linux-ld 用于将多个目标文件、库文件连接成可执行文件。在arm-linux-gcc中已经介绍(连接器选项Linker Option)
本章主要介绍`-T`选项：

* 1.`-T`:可以直接用来执行代码段、数据段、bss段的起始地址，也可以来执行一个连接脚本，在连接脚本中进行更为复杂的地址设置。

* 2.`-T`:只在bootloader，内核等“没有底层软件支持”的软件；连接运行于操作系统的应用程序，无需执行`-T`选项，使用默认方式进行链接。

## 1.直接指定代码段、数据段、bss段的起始地址

格式如下:

```
-Ttext startaddr
-Tdata startaddr
-Tbss  startaddr
```
其中startaddr分别表示代码段、数据段和bss段的起始地址，它是一个16进制数。

比如：

```
arm-linux-ld -Ttext 0x0000000 -g led_on.o -o led_on_elf
```
表示：

* 代码段运行地址为0x000000，
* 没有定义数据段、bss段起始地址，则依次放在代码段后面

例子：`-Ttext`选项：

link.S文件内容如下,使用两种跳转方式1.b跳转指令，2.直接向pc寄存器赋值：

```
.text
.global _start
_start:
	b step1
step1:
	ldr pc,=step2

step2:
	b step2
```

使用下面命令编译、连接、反汇编：

```
arm-linux-gcc -c -o link.o link.s
arm-linux-ld -Ttext 0x00000000 link.o -o link_elf_0x0000000
arm-linux-ld -Ttext 0x30000000 link.o -o link_elf_0x3000000
arm-linux-objdump -D link_elf_0x00000000 link_0x00000000.dis
arm-linux-objdump -D link_elf_0x30000000 link_0x30000000.dis
```

反汇编的结果：

```
//link_0x0000000.dis,其中eafffffff是机器码
0: eaffffff b     0x4
4: e59ff000 ldr   pc, [pc, #0] ; 0xc
8: eafffffe b     0x8
c: 00000008 andeq r0, r0, r8

//link_0x30000000.dis
0: eaffffff b     0x4
4: e59ff000 ldr   pc, [pc, #0] ; 0xc
8: eafffffe b     0x8
c: 30000008 tsteq r0, r0, r8
```

先看link.S中第一条指令`b step1`.b跳转指令是相对跳转指令，其机器码格式如下：

> | Cond |1|0|1|L|      Offset     |

* 1.`[31:28]`位是条件码；
* 2.`[27:24]`位为1010时，表示b跳转指令，为1011表示bl跳转指令；
* 3.`[23:0]`表示偏移地址；

使用b或bl下一条指令的地址计算：将指令中24位带符号的补码扩展为32(扩展其符号位)；将此32位数左移两位；将得到值加到pc寄存器中，即得到跳转的目标地址。

**第一条**指令`b step1`的机器码为`eaffffff`:

* ①24带符号的补码为0xffffff,将它扩展为32位得到：0xffffffff
* ②将此32位数左移两位得到：0xfffffffc,其值就是-4
* ③pc的值是当前质量的下一条指令的地址，加上步骤2得到的-4，恰好是第二条指令step1的地址

**注意**：请读者不要被被反汇编代码中的`b 0x4`给迷惑了，它不是说跳到绝对地址 0x4 处执行，绝对地址需要按照上述 3 个步骤计算。

可以发现，b 跳转指令依赖于当前`pc`寄存器的值，这个特性使得使用 b 指令的程序不依赖于代码存储的位置——即**不管这条代码放在什么位置**，b指令都可以跳到正确的位置。这类指令被称为**位置无关码**，使用不同的`-Ttext`选项，生成的代码仍是一样的。

再看**第二条**指令`ldr pc, =step2`.从汇编码`ldr pc, [pc, #0]`可以看出，这条指令从内存的某个位置读出数据，并赋给 pc 寄存器。这个位置的地址是当前 pc 寄存器的值加上偏移值 0，其中存放的值依赖于**连接命令**的`-Ttext`选项。

执行这条指令后，对于link_0x00000000.dis，pc=0x00000008；对于 link_0x30000000.dis， pc=0x30000008。执行第三条指令`b step2`后，程序的运行地址就不同了：分别是 0x00000008、0x30000008

Bootloader、内核等程序刚开始执行时，它们所处的地址通常不等于运行地址。在程序的开始，先使用b,bl,mov等"位置无关"的指令将代码从Flash等设备中复制到内存的“运行地址”处；然后再跳到“运行地址”去执行。


# 2.使用连接脚本设置地址
Makefile中如下语句：

```
arm-linux-ld -Ttimer.lds -o timer_elf head.o init.o interrupt.o main.o
```

使用的连接脚本timer.lds来设置可执行文件timer_elf的地址信息，timer_elf文件内容如下：

```
SECTIONS {
	. = 0x30000000
	.text            : {*(.text)}
	.rodata ALIGN(4) : {*(.rodata)}
	.data ALIGN(4)   : {*(.data)}
	.bss ALIGN(4)    : {*(.bss) *(COMMON)}
}
```

连接脚本的基本命令是SECTIONS命令，描述输出文件"映射图"：输出文件中各段、各文件怎么放置。一个SECTIONS命令内部包含一个或多个段，段(Section)是连接脚本的基本单元，表示输入文件中的某部分怎么放置。

完整的连接脚本格式如下，核心部分是段(Section)：

```
SECTIONS {
...
secname start ALIGN(align) (NOLOAD) : AT(lddr)
	{contents} > region :phdr = fill
...
}
```
secname和contects是必须的，前者用来命令这个段，后者用来确定代码中的什么部分在这个段中。

* start：是定义这个段重定位地址，也称为运行地址。如果代码中有位置相关的指令，程序运行时，这个段必须放在这个地址上。
* ALIGN（align)：虽然start指定了运行了地址，但是仍然可以使用BLOCK(align)来指定对齐的要求-这个对齐的地址才是真正的运行地址
* (NOLOAD):用来告诉加载器，在运行时不用加载这个段。显然这个选项只有在有操作系统的情况才有意义。
* AT（laddr)：指定这个段在编译出来的映像文件中的地址--加载地址(load address).如果不适用这个选项，则加载地址等于运行地址。通过这个选项，可以控制各段分别保存输出文件不同的位置，便于把文件保存到单板上：A段放在A处，B段放在B处，运行前再把A、B段分别读出来组装成一个完成的可执行程序。

后面的三个`>region:phdr= fill`没用到，自己看下。

现在看下连接脚本timer.lds含义:

* 第2行表示设置"当前运行地址"为0x30000000
* 第 3 行定义了一个名为“.text”的段，它的内容为“*(.text)”，表示所有输入文件
的代码段。这些代码段被集合在一起，起始运行地址为 0x30000000。
* 第 4 行定义了一个名为“.rodata”的段，在输出文件 timer_elf 中，它紧挨着“.text”段存放。其中的“ALIGN(4)”表示起始运行地址为 4 字节对齐。假设面“.text”段的地址范围是0x30000000～0x300003f1，则“.rodata”段的地址是 4 字节对齐后的 0x300003f4。
* 第 5、6 行的含义与第 4 行类似