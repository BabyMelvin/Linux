# gdb启动

## 1.编译
编译添加 `-g`选项
```
gcc -o main main.c -g
```
## 2.加载

两种方式:

* `gdb programe`
* 先输入`gdb`,然后输入`file yourpram`

## 3.运行

* 1.`run`或者`r`开始执行
* 2.添加参数`run parameter`

# gdb 命令


## list

`list`或`l`,显示多行源代码.

* `list`当前位置为中心显示10行
* `list n`以行数n为中心，一次显示10行
* `list function`以函数为中心显示10行
* `list -`显示刚打印过的代码

## break

`break`或`b`,设置断点,程序运行到断点的位置会停下来

`break location`:location位置设置断点

location可以为`某一行`,`某函数名`或者`其它结构的地址`

**删除断点**
使用`delete breakpoints 断点号`,删除断点.

这里的断点号表示的是第几个断点,刚才执行`break 10`返回 `breakpoint 1 at 0x40050a: file e.c, line 10.`中的1表示该断点的标号，因此使用 `delete breakpoints 1`,表示删除第10行所定义的断点

`clear n`表示清除第n行的断点,因此`clear 10`等同于`delete breakpoints 1`

`disable/enable n`表示使得编号为n的断点暂时失效或有效
可使用info查看断点相关的信息，`info breakpoints`或`info b`

## info

`info`或`i`，描述程序的状态,使用`help info`查看

# run

`run`或`r`:开始运行程序

# display

`display`或`disp`，跟踪查看某个变量,每次停下来都显示它的值

也可以使用`disable`,`enable`,`delete`,`info`命令修改及查看其状态,用法与对断点的一样

## step
`step`或`s`执行下一条语句,如果该语句为函数调用,则进入函数执行其中的第一条语句

## next
`next`或`n`执行下一条语句,如果该语句为函数调用,不会进入函数内部执行(即不会一步步地调试函数内部语句)

## print
`print`或`p`打印内部变量值

## continue
`continue`或`c`继续程序的运行,直到遇到下一个断点

## set var name=v

设置变量的值

## start
`start`或`st`开始执行程序,在main函数的第一条语句前面停下来

## file
`file`装入需要调试的程序

## kill
`kill`或`k`终止正在调试的程序

## watch

监视变量值的变化

## backtrace
`backtrace`或`bt`产看函数调用信息(堆栈)

## frame
`frame`或`f`查看栈帧

## quit
`quit`或`q`:退出GDB环境