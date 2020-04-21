# set
功能：用来设置所使用shell的执行方式，可依照不同的需求来进行设置

# 语法

`set [+ -abCdefhHklmnpPtuvx]`

# set语法

* a：表示已修改的变量，以供输出至环境变量
* b：使被中止的后台程序立刻回报执行状态
* C：转向所产生的文件无法覆盖已存在的文件
* d：Shell预设会用杂凑表记忆使用过的命令，以加速指令的执行。使用-d参数可取消。
* e：若指令传回值不等于0，则立即退出shell
* f：取消使用通配符
* h：自动记录函数的所在位置
* H Shell：可利用”!”加<指令标号>的方式来执行history中记录的指令
* k：指令所给的参数都会被视为此指令的环境变量
* l：记录for循环的变量名称
* m：使用监视模式
* n：只读取指令，而不实际执行
* p：启动优先顺序模式
* P：启动-P参数后，执行指令时，会以实际的文件或目录来取代符号链接
* t：执行完随后的指令，即退出shell
* u：当执行时使用到未定义过的变量，则显示错误信息
* v：显示shell所读取的输入值
* x：执行指令后，会先显示该指令和该指令之后的参数
* +<参数> 取消某个set曾启动的参数

# set使用

执行脚本时，与到不存在变量，bash报错并停止

```bash
#!/bin/bash

set -u  # 未定义过的变量，则显示错误信息
#set -o nonunset # 和上面执行相同
echo $a
echo hello
```

运行结果之前，先输出执行的指令

```bash
#!/bin/bash

set -x # 执行指令之前，先输出指令

# set -o -xtrace # 和上面效果相同
echo hello
```

执行命令失败后，停止执行后面指令

```bash
set -e 
# set -o -errexit

#hehe || { echo "hehe not exits"; exit 1; } # 等效set -e
#if !hehe; then echo "hehe not exists"; exit 1; fi #等效set -e
#hehe
#if [ "$?" -ne 0 ]; then echo "hehe not exists"; exit 1; fi #等效 set -e
hehe
echo hello
```

**注意**：set -e 不适合管道命令，管道命令需要使用如下命令实现:

```bash
#!/bin/bash

set -eo pipfail

hehe | echo hello1
echo hello2
```
