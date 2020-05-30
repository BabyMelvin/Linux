# 1.可执行文件

ELF（Executable Linkable Format).可执行文件

按照**可执行文件格式存储**的有：

* 可执行文件ELF
* 动态链接库(so)
* 静态链接库(a)，理解为很多.o加上一些索引

ELF可执行文件归类：

* **可重定位文件**(Relocatable file)，代码和数据。可被用作可执行文件或者共享目标文件，如下两种。
	* `.o`文件
	* 静态链接库
* **可执行文件**（Executable File)
	* 如bin/bash文件
* **共享目标文件**:(Shared Object File),包含代码和数据。
	* 一种使用，链接器可以使用，和其他可重定位文件一起链接产生目标文件
	* 另一种使用，动态链接器可以将这种共享目标文件与可执行文件结合，作为进程映射的一部分进行。
* **核心转储文件**(Core dump file),进程意外终止，系统可以将进程的地址空间的内容及终止时的一些信息转储到核心转储文件
	* core dump

```
// relocatable
$ file for.o
for.o: ELF 64-bit LSB  relocatable, x86-64, version 1 (SYSV), not stripped
```

* not stripped 带有调试信息，可以通过`strip for.o`减少代码大小。

```
//executable
$file /bin/bash
/bin/bash: ELF 64-bit LSB  executable, x86-64, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=7e4c4de7a4d259aeb0896fd579609bb6c27fae8d, stripped

//shared object
~$ file /lib/libiptc.so.0.0.0
/lib/libiptc.so.0.0.0: ELF 64-bit LSB  shared object, x86-64, version 1 (SYSV), dynamically linked, BuildID[sha1]=cf33e59f80575303a1a3855e17ec83494fc3dcb9, stripped
```

## 2目标文件存储

目标文件中：

* 1.文件头：符号表、调试信息、字符串等
* 2.`.text section`
* 3.`.data section`
* 4.`.bss section`

```c
//for.c
//gcc -c -o for.o for.c
#include <stdio.h>

int main() {
    int i= 0;
    for (i = 0 ; i < 1; i++) {
       printf("hello world ,i = %d\n",i); 
       if(i == 0) {
           continue;
       }
    }
    return 0;
}
```

### 2.1 查看具体内容(for.o)

专门解析ELF工具 `readelf`

1.查看段内容`objdump -h`

```
$objdump -h for.o
for.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000044  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000000  0000000000000000  0000000000000000  00000084  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  0000000000000000  0000000000000000  00000084  2**0
                  ALLOC
  3 .rodata       00000015  0000000000000000  0000000000000000  00000084  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      0000002c  0000000000000000  0000000000000000  00000099  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000c5  2**0
                  CONTENTS, READONLY
  6 .eh_frame     00000038  0000000000000000  0000000000000000  000000c8  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
```

* 代码段，数据段，BSS段和只读数据段(`.rodata`)、注释信息段(`.comment`)和堆栈提示段(`.note.GNU-stack`)
* 文件的属性
	* SIZE(段长度)和段所在位置(File Offset)
* 段的第二行，CONTENTS,ALLOC等各种属性
	* CONTENTS表示该段在文件中存在（BSS段没有CONTENTS，实际上ELF不存在BSS段内容）
* 00000040则表示File Header长度
* VMA表示虚拟地址Visual Memory Address
* LMA表示加载地址Load Memory Address 正常和VMA相同

2.查看段的长度(size)

```
$size for.o
text	   data	    bss	    dec	    hex	filename
    145	      0	      0	    145	     91	for.o
```

### 2.2 代码段

* objdump -s段内容以十六进制方式打印出来
* objdump -d参数将所以包含指令的段烦汇编。

```
// objdump -s -d for.o

Contents of section .text:
 0000 554889e5 4883ec10 c745fc00 000000c7  UH..H....E......
 0010 45fc0000 0000eb1f 8b45fc89 c6bf0000  E........E......
 0020 0000b800 000000e8 00000000 837dfc00  .............}..
 0030 75019083 45fc0183 7dfc007e dbb80000  u...E...}..~....

Disassembly of section .text:

0000000000000000 <.text>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	48 83 ec 10          	sub    $0x10,%rsp
   8:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
   f:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
  16:	eb 1f                	jmp    0x37
  18:	8b 45 fc             	mov    -0x4(%rbp),%eax
  1b:	89 c6                	mov    %eax,%esi
  1d:	bf 00 00 00 00       	mov    $0x0,%edi
  22:	b8 00 00 00 00       	mov    $0x0,%eax
  27:	e8 00 00 00 00       	callq  0x2c
  2c:	83 7d fc 00          	cmpl   $0x0,-0x4(%rbp)
  30:	75 01                	jne    0x33
  32:	90                   	nop
  33:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
  37:	83 7d fc 00          	cmpl   $0x0,-0x4(%rbp)
  3b:	7e db                	jle    0x18
  3d:	b8 00 00 00 00       	mov    $0x0,%eax
  42:	c9                   	leaveq 
  43:	c3                   	retq   
```

objcopy 工具，将图片转成ELF格式文件

```
objcopy -I binary -O elf32-i386 -B i386 image.jpg image.o
$objdump -ht image.o
```

# 3.强符号和弱符号

重定义问题：强符号(Strong Symbol).

C/C++中默认函数和初始化全局变量为强符号，未初始化的全局变量为弱符号。

强弱符号都是针对定义的，不是针对符号的引用。

* 有强有弱则使用强引用
* 都是弱符号，则使用占用空间最大的一个

# 4. 空间与地址分配
多个目标文件怎么合并成一个可执行文件？

段的装载地址和空间的对齐单位是页，4k。如果一个段的长度只是1个字节，也要在内存中占用4096字节。

将各个目标文件的，相似段进行合并。分成一个大text,data,bss段

bss段不占用文件空间，但是装载时占用空间。 链接器合并各个段的同时，将bss合并，并且分配虚拟空间。空间如何分配?

**链接器为目标文件分配地址和空间**，地址和空间两个含义：

* 第一个输出的可执行文件中的空间，第二个在装载后的虚拟地址汇总的虚拟地址空间。
	* text和data文件和虚拟地址都要分配空间
	* bss局限于虚拟空间中

事实上，这里谈到的空间分配**只关注于虚拟空间的分配**。关系到连机器后面关于地址计算步骤。链接器空间分配的策略基本上采用第二种方法，链接器一般都采用一种叫**两步链接**的方法。(Two-pass Linking)

* 第一步 空间与地址分配 扫描所有输入目标文件，各段长度属性和位置，并将符号表中所有符号定义和符号引用收集起来，统一放在一个全部符号表。
	* 计算各个段合并后的长度与位置
	* 建立映射关系
* 第二步 符号解析与重定位 收集所有信息，读取输入文件中段的数据、重定位信息，并且进行符号解析与重定位、调整代码中的地址等。


```
$ld a.o b.o -e main -o ab
```

* `-e main`表示将main函数作为程序入口，ld链接器默认的程序入口为_start

链接前后的程序中使用的地址已经是程序中进程中的虚拟地址，即我们关心上面各个段中VMA和Size，而忽略文件偏移(File off).

```c
//a.c
int a()
{
    return 12;
}

//b.c
#include <stdlib.h>
#include <stdio.h>

extern int a();
int main()
{
    printf("a = %d\n", a());
    return 0;
}
```
objdump -h查看各段信息

```
$objdump -h a.o
a.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         0000000b  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000000  0000000000000000  0000000000000000  0000004b  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  0000000000000000  0000000000000000  0000004b  2**0
                  ALLOC
  3 .comment      0000002c  0000000000000000  0000000000000000  0000004b  2**0
                  CONTENTS, READONLY
  4 .note.GNU-stack 00000000  0000000000000000  0000000000000000  00000077  2**0
                  CONTENTS, READONLY
  5 .eh_frame     00000038  0000000000000000  0000000000000000  00000078  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA

$objdump -h b.o
b.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000026  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000000  0000000000000000  0000000000000000  00000066  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000000  0000000000000000  0000000000000000  00000066  2**0
                  ALLOC
  3 .rodata       00000008  0000000000000000  0000000000000000  00000066  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      0000002c  0000000000000000  0000000000000000  0000006e  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  0000009a  2**0
                  CONTENTS, READONLY
  6 .eh_frame     00000038  0000000000000000  0000000000000000  000000a0  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA

$objdump -h ab
....
10 .init         0000001a  00000000004003e0  00000000004003e0  000003e0  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 11 .plt          00000040  0000000000400400  0000000000400400  00000400  2**4
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 12 .text         00000192  0000000000400440  0000000000400440  00000440  2**4
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 13 .fini         00000009  00000000004005d4  00000000004005d4  000005d4  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
 14 .rodata       0000000c  00000000004005e0  00000000004005e0  000005e0  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 15 .eh_frame_hdr 0000003c  00000000004005ec  00000000004005ec  000005ec  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 16 .eh_frame     00000114  0000000000400628  0000000000400628  00000628  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 17 .init_array   00000008  0000000000600e10  0000000000600e10  00000e10  2**3
                  CONTENTS, ALLOC, LOAD, DATA
 18 .fini_array   00000008  0000000000600e18  0000000000600e18  00000e18  2**3
                  CONTENTS, ALLOC, LOAD, DATA
 19 .jcr          00000008  0000000000600e20  0000000000600e20  00000e20  2**3
                  CONTENTS, ALLOC, LOAD, DATA
....
```

其中：

* 1.`a.o`和`b.o`VMA都是0,虚拟空间还没分配，都是0.
* 2.链接之后，可执行文件ab各个段分配虚拟地址。`.text`分配为0000000000400440，大小为00000192

Linux下ELF可执行文件默认地址0x08048000开始分配。

> 符号地址的确定

因为各个符号在段内的相对位置是固定的，所以这时候函数`main`,`a`地址已经是确定的。只不过链接器将每个符号加上一个偏移量，调整到正确的虚拟地址。


未初始化为全局变量当作为弱引用,如果一旦一个未初始化的全部变量不是以COMMON块的形式存在,那么相当于一个强符号.`attribute((nocommon))`或者 gcc添加`-fno-common`使用。


