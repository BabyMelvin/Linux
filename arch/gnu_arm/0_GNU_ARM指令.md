以前用ARM的IDE工具，使用的是ARM标准的汇编语言。现在要使用GNU的工具，当然要了解一点GNU ARM汇编的不同之处。

ARM汇编语言源程序语句,一般由指令,伪操作,宏指令和伪指令作成.ARM汇编语言的设计基础是汇编伪指令,汇编伪操作和宏指令.

目前常用的ARM编译环境有2种：

* ARMASM: ARM公司的IDE中使用了CodeWarrior的编译器，绝大多数windows下的开发者都在使用这一环境，完全按照ARM的规定；
* GNU ARM ASM: GNU工具的ARM版本，与ARMASM略有不同；

关于CodeWarriror ARM汇编的书和文章很多,本文假定你已经完全了解ARMASM，这里只说明GNU ARM汇编，并针对ARMASM给出说明。本文翻译自：GNU ARM Assembler Quick Reference。

# 第一部分 Linux下ARM汇编语法

Linux下ARM汇编语法尽管在Linux下使用C或C++编写程序很方便，但汇编源程序用于系统最基本的初始化，如初始化堆栈指针、设置页表、操作 ARM的协处理器等。初始化完成后就可以跳转到C代码执行。需要注意的是，GNU的汇编器遵循AT&T的汇编语法，可以从GNU的站点 （www.gnu.org）上下载有关规范。

## 1.1. Linux汇编行结构

任何汇编行都是如下结构：

```asm
[:] [} @ comment //注释用@，[]中的内容为可选，可有可无。
[:] [} @ 注释
```

Linux ARM 汇编中，任何以冒号结尾的标识符都被认为是一个`标号`，而不一定非要在一行的开始。

【例1】定义一个"add"的函数，返回两个参数的和。

```asm
.section .text, "x"
.global add @ give the symbol add external linkage
add:
    ADD r0, r0, r1 @ add input arguments
    MOV pc, lr     @ return from subroutine
@ end of program
```

## 二. Linux 汇编程序中的标号

标号只能由`a～z`，`A～Z`，`0～9`，“`.`”，`_`等字符组成。

当标号为`0～9`的数字时为**局部标号**，局部标号可以重复出现,使用方法如下：

* 标号f: 在引用的地方向前的标号 forward
* 标号b: 在引用的地方向后的标号 backward

【例2】使用局部符号的例子，一段循环程序

```asm
1:
  subs r0, r0, #1 @每次循环使r0=r0-1
  bne 1f        @跳转到1标号去执行
```
局部标号代表它所在的地址,因此也可以当作变量或者函数来使用。

## 三. Linux汇编程序中的分段

### （1）`.section`伪操作

用户可以通过`.section`伪操作来自定义一个段,格式如下:

`.section section_name [, "flags"[, %type[,flag_specific_arguments]]]`

每一个段以段名为开始, **以下一个段名或者文件结尾为结束**。

这些段都有缺省的标志（flags）,**连接器**可以识别这些标志。(与armasm中的AREA相同)。

下面是ELF格式允许的段标志含义

* a 允许段
* w 可写段
* x 执行段

【例3】定义段

```asm
.section .mysection @自定义数据段，段名为 “.mysection”
.align 2
strtemp:
	.ascii "Temp string \n\0"
```

###（2）汇编系统预定义的段名

```asm
.text @代码段
.data @初始化数据段
.bss @未初始化数据段
.sdata @
.sbss @
```

需要注意的是，源程序中`.bss`段应该在`.text`之前。

## 四. 定义入口点

汇编程序的缺省入口是`start标号`，用户也可以在**连接脚本**文件中用**ENTRY**标志指明其它入口点。

【例4】定义入口点

```asm
.section .data
    < initialized data here>

.section .bss
    < uninitialized data here>

.section .text
.globl _start @ give the symbol add external linkage
_start:
    <instruction code goes here>
```

## 五. Linux汇编程序中的宏定义

格式如下:

* `.macro` 宏名 参数名列表 @伪指令`.macro`定义一个宏宏体
* `.endm` @`.endm`表示宏结束

如果**宏使用参数**,那么在宏体中使用该参数时添加前缀“`\`”。宏定义时的参数还可以使用**默认值**。

* 可以使用`.exitm`伪指令来退出宏。

【例5】宏定义

```asm
.macro SHIFTLEFT a, b
.if \b < 0
MOV \a, \a, ASR #-\b
.exitm
.endif
MOV \a,\a, LSL #\b
.endif
.endm
```
## 六. Linux汇编程序中的常数

（1）十进制数以非0数字开头,如:123和9876；
（2）二进制数以0b开头,其中字母也可以为大写；
（3）八进制数以0开始,如:0456,0123；
（4）十六进制数以0x开头,如:0xabcd,0X123f；
（5）字符串常量需要用引号括起来,中间也可以使用转义字符,如: "You are welcome!\n"；
（6）当前地址以“`.`”表示,在汇编程序中可以使用这个符号代表当前指令的地址；

例如:  "`.long .`"    //这里"`.`"是**链接地址**，如果**开启MMU**，就是虚拟地址

（7）表达式:在汇编程序中的表达式可以使用常数或者数值, “-”表示取负数,“~”表示取补,`<>` 表示不相等,其他的符号如:`+`、`-`、`*`、`/`、`%`、
`<`、`<<`、`>`、`>>`、`|`、`&`、`^`、`!`、`==`、`>=`、`<=`、`&&`、`||`跟C语言中的用法相似。

## 七. Linux下ARM汇编的常用伪操作
在前面已经提到过了一些伪操作，还有下面一些伪操作：
数据定义伪操作：`.byte`，`.short`，`.long`，`.quad`，`.float`，`.string/.asciz/.ascii`，重复定义伪操作`.rept`，赋值语句`.equ/.set`；

函数的定义 ；

* 对齐方式伪操作 `.align`；
* 源文件结束伪操作`.end`；
* `.include`伪操作；
* `.if`伪操作；
* `.global/ .globl` 伪操作 ；
* `.type`伪操作 ；

列表控制语句 ；

区别于**gas汇编**的通用伪操作,下面是ARM特有的伪操作 ：`.reg` ，.`unreq` ，`.code` ，`.thumb` ，`.thumb_func` ，`.thumb_set`， `.ltorg` ，`.pool`

> 1. 数据定义伪操作

* （1） `.byte`:单字节定义，如：`.byte 1,2,0b01,0x34,072,'s' `；
* （2） `.short`:定义双字节数据，如:`.short 0x1234,60000 `；
* （3） `.long`:定义4字节数据，如:`.long 0x12345678,23876565`
* （4） `.quad:`定义8字节，如:`.quad 0x1234567890abcd`
* （5） `.float`：定义浮点数，如:`.float 0f-314159265358979323846264338327\95028841971.693993751E-40 @ - pi`
* （6） `.string/.asciz/.ascii`：定义多个字符串，如:
  * `.string "abcd", "efgh", "hello!"`
  * `.asciz "qwer", "sun", "world!"`
  * `.ascii "welcome\0"`
* 需要注意的是：`.ascii`伪操作定义的字符串需要自行添加结尾字符`'\0'`。
* （7） `.rept`:重复定义伪操作, 格式如下:
  * `.rept `重复次数数据定义
  * `.endr` @结束重复定义

例如:
```asm
  .rept 3
  .byte 0x23
  .endr
```

* （8） `.equ/.set`: 赋值语句, 格式如下:`.equ(.set) 变量名,表达式`
 
例如:`.equ abc 3 @让abc=3`

> 2.函数的定义伪操作

（1）函数的定义,格式如下:

```asm
  函数名:
  函数体
  返回语句
```

一般的,**函数如果需要在其他文件中调用**, 需要用到`.global`伪操作将函数声明为全局函数。为了不至于在其他程序在调用某个C函数时发生混乱,对寄存器的使用我们需要遵循**APCS准则**。函数编译器将处理为函数代码为一段`.global`的汇编码。

（2）函数的编写应当遵循如下规则:

* `a1-a4`寄存器（参数、结果或暂存寄存器，r0到r3 的同义字）以及浮点寄存器f0-f3(如果存在浮点协处理器)在函数中是不必保存的；

* 如果函数返回一个不大于一个字大小的值，则在函数结束时应该把这个值送到 r0 中；
* 如果函数返回一个浮点数，则在函数结束时把它放入浮点寄存器f0中；
* 如果函数的过程改动了sp（堆栈指针，r13）、fp（框架指针，r11）、sl（堆栈限制，r10）、lr（连接寄存 器，r14）、v1-v8（变量寄存器，r4 到 r11）和 f4-f7,那么函数结束时这些寄存器应当被恢复为包含在进入函数时它所持有的值。

> 3.`.align` `.end` `.include` `.incbin`伪操作

(1)`.align`:用来指定数据的对齐方式,格式如下:

```asm
  .align [absexpr1, absexpr2]
```
以某种对齐方式,在未使用的存储区域填充值. 第一个值表示对齐方式,4, 8,16或 32. 第二个表达式值表示填充的值。

（2）`.end`:表明源文件的结束。
（3）`.include`:可以将指定的文件在使用`.include` 的地方展开,一般是头文件,例如:
```
  .include "myarmasm.h"
```
（4）`.incbin`伪操作可以将原封不动的一个二进制文件编译到当前文件中,使用方法如下:

```asm
  .incbin "file"[,skip[,count]]
```

* skip表明是从文件开始跳过skip个字节开始读取文件
* count是读取的字数.

> 4. `.if`伪操作

根据一个表达式的值来决定是否要编译下面的代码, 用`.endif`伪操作来表示条件判断的结束, 中间可以使用`.else`来决定`.if`的条件不满足的情况下应该编译哪一部分代码。

`.if`有多个变种:

* `.ifdef symbol @判断symbol是否定义`
* `.ifc string1,string2 @字符串string1和string2是否相等,字符串可以用单引号括起来`
* `.ifeq expression_r @判断expression_r的值是否为0`
* `.ifeqs string1,string2 @判断string1和string2是否相等,字符 串必须用双引号括起来`
* `.ifge expression_r @判断expression_r的值是否大于等于0`
* `.ifgt absolute expression_r @判断expression_r的值是否大于0`
* `.ifle expression_r @判断expression_r的值是否小于等于0`
* `.iflt absolute expression_r @判断expression_r的值是否小于0`
* `.ifnc string1,string2 @判断string1和string2是否不相等, 其用法跟.ifc恰好相反。`
* `.ifndef symbol, .ifnotdef symbol @判断是否没有定义symbol, 跟.ifdef恰好相反`
* `.ifne expression_r @如果expression_r的值不是0, 那么编译器将编译下面的代码`
* `.ifnes string1,string2 @如果字符串string1和string2不相 等, 那么编译器将编译下面的代码.`

> `5. .global .type .title .list`

（1）`.global/ .globl` ：用来定义（不是声明，这里要分配内存的哦！！！）一个全局的符号，格式如下:
  `.global symbol 或者 .globl symbol`

（2）`.type`：用来指定一个符号的类型是函数类型或者是对象类型, 对象类型一般是数据, 格式如下:
  `.type 符号, 类型描述`

【例6】

```asm
.globl a
.data
.align 4
.type a, @object
.size a, 4
a:
    .long 10
```
【例7】

```asm
.section .text
.type asmfunc, @function
.globl asmfunc
asmfunc:
	mov pc, lr
```
（3）列表控制语句:

* `.title`：用来指定汇编列表的标题,例如:`.title “my program”`
* `.list`：用来输出列表文件.

> 6. ARM特有的伪操作

（1） `.req`: 用来给寄存器赋予别名,格式如下:
  `别名 .req 寄存器名`
（2） `.unreq`: 用来取消一个寄存器的别名,格式如下:`.unreq 寄存器别名`注意被取消的别名必须事先定义过,否则编译器就会报错,这个伪操作也可以用来取消系统预制的别名, 例如r0, 但如果没有必要的话不推荐那样做。
（3）`.code`伪操作用来选择ARM或者Thumb指令集,格式如下:`.code 表达式`如果表达式的值为16则表明下面的指令为Thumb指令,如果表达式的值为32则表明下面的指令为ARM指令.
（4）`.thumb`伪操作等同于`.code 16`, 表明使用Thumb指令, 类似的`.arm`等同于`.code 32`
（5）`.force_thumb`伪操作用来强制目标处理器选择thumb的指令集而不管处理器是否支持
（6）`.thumb_func`伪操作用来指明一个函数是thumb指令集的函数
（7）`.thumb_set`伪操作的作用类似于.set, 可以用来给一个标志起一个别名, 比`.set`功能增加的一点是可以把一个标志标记为thumb函数的入口, 这点功能等同于`.thumb_func`
（8）`.ltorg`用于声明一个数据缓冲池(`literal pool`)的开始,它可以分配很大的空间。
（9）`.pool`的作用等同`.ltorg`
（10）`.space  {,}`分配number_of_bytes字节的数据空间，并填充其值为`fill_byte`，若未指定该值，缺省填充0。（与armasm中的SPACE功能相同）
（11）`.word  {,} …`插入一个32-bit的数据队列。（与armasm中的DCD功能相同）可以使用`.word`把标识符作为常量使用

例如：

```asm
Start:
valueOfStart:
  .word Start
```
这样程序的开头Start便被存入了内存变量valueOfStart中。
（12）`.hword  {,} …`插入一个16-bit的数据队列。（与armasm中的DCW相同）

## 八. GNU ARM汇编特殊字符和语法

代码行中的注释符号: `@`
整行注释符号: `#`
语句分离符号: `;`
直接操作数前缀: `#` 或 `$`
