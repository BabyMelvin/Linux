# shell 执行的方式

Linux执行文件有如下几种方式：   

* 1.`./xxx.sh`意思是执行当前目录的`xxx.sh`文件，与输入完整路径执行脚本效果是一样的。需要`xxx.sh`有可执行权限。
* 2.`sh xxx.sh`意思是用**shell解释器**来执行a.sh脚本，不要求文件有可执行权限。
* 3.`. xxx.sh`和`source xxx.sh`效果相同，作用都是使刚修改过的配置文件立即生效。

shell中使用`source conf.sh`，是直接运行conf.sh的命令，不创建**子shell**，source不是直接执行一个文件，而是从一个文本文件里面读命令行，然后执行这些命令。`.  XX.sh`，这样相当于**直接系统读取**exit，然后退出，正确的运行方式是`./XX.sh`或者`sh XX.sh`

`sh`是则创建**子shell**，子shell里面的变量父shell无法使用，对环境变量的修改也不影响父shell。父shell中的局部变量，子shell也无法使用，只有父shell的环境变量，子shell能够使用.

# 变量

> 赋值时候前后不能有空格

## 1.基本变量
* 1.`readonly`:只读变量
* 2.`unset`:删除变量

* 局部变量：代码快或者函数中,local关键字
* 环境变量:影响用户接口和shell行为，分配环境空间是有限的
    * 脚本设置为环境变量,export出来,export只能影响自己子进程
    * 脚本命令行调用，那么脚本不能影响环境变量

`IFS`:内部域分隔符。默认是space,tab,newline.可以设置作为输出分隔

* set和env变量
    * set变量设置shell变量，仅在本shell有效。
    * env用户环境变量
    * set变量包含了env变量,set变量可以通过export变为环境变量
    * env变量作用域大些，显然对subshell使用有效

## 2.数组

`数组名=(n1 n2 ... n3)`

```
# 注释

:<<EOF
duo
hang zhu
shi
EOF
arry=(a b c d)
或
arry[0]=a
arry[1]=b
arry[2]=c
```
数组取值:`value =${arry[1]}`,取所有值:`echo ${arry[@]}`

数组长度:`len=${#arry[@]}`或`len=${#arry[*]}`
单个元素长度:`len2=${#arry[2]}`

# 3.$符号
位置参数: `$0`,`$1`,...,`${10}`

* `$*`:将所有输入参数作为一个整体字符串
* `$@`:保持原来参数不进行合并
* `$#`:输入参数的个数
* `$$`:脚本运行的当前进程ID
* `$!`:后台运行的最后一个进程ID号
* `$-`:现实shell使用的当前选项，和set命令相同
* `$?`:现实最后命令的退出状态，0表示没有错误，其他表示有错

输出最后参数的方法

```bash
# 花括号里不能使用美元符号
params=$#
echo the last parameter is $params
echo the last parameter is ${!#}
```

# 条件

* 数值:`-eq`:等于,`-ne`:不等于,`-gt`:大于,`-ge`:大于等于,`-lt`:小于,`-le`:小于等于
* 字符串:`=`:等于，`!=`:不等于,`-z`:为零，`-n`:不为零
* 文件测试：`-e`:文件存在,`-r`:存在且可读，`-w`:存在且可写,`-x`:存在且可执行,`-s`:存在且至少有一个字符
    * `-d`:文件存在且为目录,`-f`:存在普通文件,`-c`:存在字符文件,`-b`:存在块文件
    * `file1 -nt file2`:检查file1是否比file2新
    * `file1 -ot file2`:检查file1是否比file2旧

# 双括号和双方括号

* 双括号能够进行数学运算

```bash
var1=10
if (( $var1 ** 2 > 90))
then
    (( var2 = $var1 ** 2))
    echo "$var1的平方等于$var2"
fi

for (( i=1; i<10; i++))
do
    echo "下一个数字是:$i"
done
```

* 双方括号，用来样式匹配

```bash
if [[ $USR == r* ]]
then 
    echo "Hello $USR"
else
    echo "sorry,I do not know you" 
fi
```

# find 命令

```
#在压缩文件内寻找文本片段
find . -type f -print | xarg bzgrep ZUT
```

# xargs和管道的区别
* `echo '--help' | cat`:该命令输出的是echo的内容，echo命令的输出通过管道定向到cat的输入了。然后cat从其标准输入中读取待处理的文本内容。cat出显示信息是--help
* `echo '--help' | xargs cat`:xargs将其接受的字符串成cat的一个命令参数来运行命令,这里最终显示内容应该是执行`cat --help`命令的显示效果

# 符号
## 1.`|`管道
管道符号，是unix一个很强大的功能,符号为一条竖线:`|`。
用法:`command 1 | command 2`他的功能是把第一个命令command 1执行的结果作为command2的输入传给command 2，例如:`$ls -s|sort -nr` (请注意不要复制$符号进去哦)

## 2.`&&`运算符

`command1  && command2`:&&左边的命令（命令1）返回真(即返回0，成功被执行）后，&&右边的命令（命令2）才能够被执行；换句话说，“如果这个命令执行成功&&那么执行这个命令”。
语法格式如下：
`command1 && command2 && command3 ...`

* 1.命令之间使用 `&&` 连接，实现逻辑与的功能。
* 2.只有在 && 左边的命令返回真（命令返回值`$? == 0`），`&&` 右边的命令才会被执行。
* 3.只要有一个命令返回假（命令返回值`$? == 1`），后面的命令就不会被执行。

## 3.`||` 运算符

`command1 || command2`:`||`则与`&&`相反。如果`||`左边的命令（command1）未执行成功，那么就执行`||`右边的命令（command2）；或者换句话说，“如果这个命令执行失败了`||`那么就执行这个命令。

* 1.命令之间使用`||`连接，实现逻辑或的功能。
* 2.只有在`||`左边的命令返回假（命令返回值 $? == 1），|| 右边的命令才会被执行。这和 c 语言中的逻辑或语法功能相同，即实现短路逻辑或操作。

只要有一个命令返回真（命令返回值 $? == 0），后面的命令就不会被执行。

## 4.`()` 运算符

如果希望把几个命令合在一起执行，shell提供了两种方法。既可以在当前shell也可以在子shell中执行一组命令。
格式:`(command1;command2;command3....)`             多个命令之间用;分隔

* 1.一条命令需要独占一个物理行，如果需要将多条命令放在同一行，命令之间使用命令分隔符（;）分隔。执行的效果等同于多个独立的命令单独执行的效果。
* 2.`()`表示在当前 shell 中将多个命令作为一个整体执行。需要注意的是，使用`()`括起来的命令在执行前面都不会切换当前工作目录，也就是说命令组合都是在当前工作目录下被执行的，尽管命令中有切换目录的命令。
* 3.命令组合常和命令执行控制结合起来使用。

## 5.`{}`运算符

如果使用`{}`来代替`()`，那么相应的命令将在**子shell**而不是当前shell中作为一个整体被执行，只有在`{}`中所有命令的输出作为一个整体被重定向时，其中的命令才被放到子shell中执行，否则在当前shell执行。
它的一般形式为：
`{ command1;command2;command3… }`

注意：在使用`{}`时，`{}`与命令之间必须使用**一个空格**

## 6.重定向
STDERR并不会随着STDOUT重定向而发生改变

重定向错误：可以选择只重定向错误
方法：将文件描述符放在重定向符号前

`ls -al badfile 2> test4`

两种方法在脚本中重定向输出：

* 临时重定向输出
* 永久重定向脚本中的所有命令

 ### 6.1 临时重定向

 将单独一行重定向到STDERR,在重定向文件描述符前添加一个`&`符号.(>后面没有空格)

 `echo "The is an error message >&2"`

 临时重定向和对整个文件错误输出重定向结合使用，将会达到错误文件重定向的效果.

 ```bash
 #!/bin/bash
 echo "This is an error" >&2
 echo "This is normal output"
 ```
 需要这样执行达到很好的效果：`./test 2> test_out`,只会把错误信息放入test_out

### 6.2 永久重定向
使用exec 命令让shell脚本执行期间重定向某个特定文件描述符.

```bash
#!/bin/bash

# 重定向所有输出到testout文件
exec 1>testout
echo "This is a test of redirections all output"
echo "from a script to another file."

# exec会启动一个新shell将STDOUT文件描述符重定向到文件
exec 2>testerr
echo "without having to redirect every individual line"
```

同样可以使用`exec 0< testfile`

### 6.3 重定向文件描述符

将SDOUT的原来位置重定向到另一个文件描述符，然后再利用该文件描述符重定向回STDOUT

```bash
#!/bin/bash
exec 3>&1
exec 1>testout
echo "This should store in the output file"
echo "along with this line"

exec 1>&3
echo "Now things should be back to normal"
```

### 6.4 创建和关闭文件描述符
文件描述符，内部会维护一个当前文件中位置的指针。任何读或写都会从文件指针上次的位置开始。

1.打开文件描述符
```bash
#!/bin/bash

# 将文件描述符3分配给testfile进行文件读写
exec 3<>testfile
read line <&3
echo "Read:$line"
echo "This is a test file line" >&3

```

2.关闭文件描述符
`exec 3>&-`

3.列出打开的文件描述符

`lsof`,常用选项`-p`执行PID($$当前进程)和`-d`:执行文件描述符,`-a`:逻辑运算，与.

`lsof -a  -p $$ -d 0,1,2`

* COMMAND:正在运行的命令名的前9个字符
* PID:进程的PID 
* USER:进程属主的登录名
* FD:文件描述符以及访问类型(r读，w写,u读写)
* TYPE:文件类型，CHR,BLK,DIR,REG
* DEVICE:设备的设备号
* SIZE:如果有的话，表示文件的大小
* NODE:本地文件的节点号
* NAME:文件名

### 6.5阻止文件输出
将STDERR重定向到一个叫做null文件的特殊文件.shell输出到这里文件不会保存都会被丢弃掉.

`$ls -al > /dev/null`

清楚文件的方法:`cat /dev/null > testfile`

## 7.临时文件
大多数Linux发行版配置系统在启动时自动删除`/tmp`目录的所有文件.并且所有用户都有权限读写`/tmp`.

特殊命令来创建临时文件。`mktemp`在`tmp`中中创建一个唯一的临时文件.一旦创建脚本有了完整的读写权限，其他人没办法访问它.

### 7.1 创建临时文件
文件名模板可以包含任意文本文件，在文件名末尾添加6个X就可以了:`$mktemp testing.XXXXXX`

`tee`命令 既能将数据保存，也能将数据显示在屏幕上
