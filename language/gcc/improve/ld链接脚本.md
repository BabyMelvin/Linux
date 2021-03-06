# 使用ld链接脚本

把整个链接过程比作一台计算机，那么ld链接器就是计算机的CPU，所有目标文件、库文件就是输入，链接结果输出的可执行文件就是输出，而链接控制脚本就是这台计算机的"程序",控制着CPU的运行

ld脚本语言，无论输入和输出文件，主要数据就是文件中的各种段.

输入文件叫做：输入段(Input Sections)，输出文件中段为输出段(Output Sections)

空间链接过程就是控制输入段如何成为输出段，哪些输入段要合并一个输出段.

* 指明输入段哪些合并，哪些丢弃
* 指明输出段的名字、装载地址、属性等等

# 1.链接语法

语法类似C语言,链接脚本一系列语句组成，分为两种：

* 一种是命令语句
* 一种是赋值语句

和C有类似语法：

* 语句之间使用分号;作为分割
    * 命令语句可以使用换行结束
    * 赋值语句必须以`;` 作为结束
* 表达式与运算符
    * C类似，`+,-,*,/,+=,-=,*=`包括`&,|,>>,<<`等位操作
* 注释和字符引用
    * `/**/`作为注释
    * 文件名、格式名或段名等，凡是包含`;`或其他的分隔符的，都要使用引号将该名字全称引用起来，如果文件名包含引号，**将无法处理**.

# 2.命令

* `ENTRY(symbol)`
    * 指定symbol的值为入口地址.被指定在ELF文件头Elf32_Ehdr的c_entry成员中.ld多种方法设置，优先级如下(高到低):
        * 1.ld命令`-e`选项
        * 2.链接脚本`ENTRY(symbol)`命令
        * 3.如果定义了`_start`
        * 4.存在`.text`段，使用`.text`第一个字节地址
        * 使用值0
* `STARTUP(filename)`
    * 将文件filename作为链接过程第一个输入文件
* `SEARCH_DIR(path)`
    * 将路径path加入到ld链接器的库查找目录。同`-L`
* `INPUT(file,file,...)`
    * 将制定文件作为链接过程的输入文件
* INCLUDE filename
    * 将文件包含进本链接脚本
* PROVIDE(symbol)
    * 在链接脚本中定义某个符号
        * 符号可以被程序引用(特殊符号是系统默认链接脚本通过PROVIDED命令定义在脚本中)

# 3. SECTION命令

最基本格式:

```
SECTIONS
{
    ...
    secname : {contents}
    ...
}
```

secname表示输出的段名,secname后必须有一个空格符，这样不会有歧义,后面跟冒号和大括号
