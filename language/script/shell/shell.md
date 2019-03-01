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


















