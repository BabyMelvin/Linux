AWK是一种处理文本文件的语言，是一个强大的文本分析工具。

将文本划分后提取出来，如空格分开等

# 语法

```
awk [选项参数] 'script' var=value file(s)
或
awk [选项参数] -f scriptfile var=value file(s)
```

选项参数说明：

* `-F fs or --field-separator fs`:指定输入文件**折分隔符**，fs是一个字符串或者是一个正则表达式，如`-F:`。
* `-v var=value or --asign var=value`:赋值一个用户定义变量。
* `-f scripfile or --file scriptfile`:从脚本文件中读取awk命令。

# 基本用法1

```
# 行匹配语句 awk '' 只能用单引号
awk '{[pattern] action}' {filenames}   
```

实例一

```
# 匹配按空格或TAB分割，输出文本中1 4 项(每行)
awk '{print $1,$4}' log.txt

# 格式化输出,8s表示8 space空格
awk '{printf "%-8s %-10s\n",$1,$4}' log.txt
```

# 用法2-分割字符

```
#-F相当于内置变量FS, 指定分割字符
awk -F 
```
实例

```
# 使用","分割
awk -F, '{print $1,$2}'   log.txt

# 或者使用内建变量
awk 'BEGIN{FS=","} {print $1,$2}'     log.txt

# 使用多个分隔符.先使用空格分割，然后对分割结果再使用","分割
awk -F '[ ,]'  '{print $1,$2,$5}'   log.txt
```

# 用法3-设置变量

```
awk -v
```

实例

```
awk -va=1 '{print $1,$1+a}' log.txt
awk -va=1 -vb=s '{print $1,$1+a,$1b}' log.txt
```

# 用法4-awk脚本

```
awk -f {awk脚本} {文件名}
```
实例

```
awk -f cal.awk log.txt
```