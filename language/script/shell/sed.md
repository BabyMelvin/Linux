# sed使用
sed命令可实现文件的添加删除修改，替换.
## 1.命令格式

```
sed [-hnV][-e<script>][-f<script文件>][文本文件]
```

选项

* `-e<script>`：选项中script来处理文本
* `-f<script>`：选项中指定script文本处理输入文本
* `-n`: 仅显示script处理后的结果
* `-h`:象时帮助
* `-V`或`--version`：显示版本信息

参数

* 文件：指定待处理文本呢文件列表

## sed命令（不改变文本内容，除了-i选项）

* `a`：当前行下面插入文本
* `i`:在当前行上面插入文本
* `c`:把选定行为改为新的文本
* `d`:删除，删除选择的行
* `D`: 删除莫板块的第一行
* `s`:替换指定字符
* `l`:字符不能打印字符的清单
* `n`:读取下一个输入行，用下一个命令处理新的行而不是用一个命令行
* `p`:打印模板块的行
* `q`:退出sed
* `r`:file从file中读行


# 以行为单位
## 1.添加

```bash
# 在第4行后添加新的字符串
# -e 命令行中script
sed -e 4a\newline testfile
```

```
$cat testfile
HELLO LINUX!  
Linux is a free unix-type opterating system.  
This is a linux testfile!  
Linux test 
```

执行完命令

```
$sed -e 4a\newline testfile
HELLO LINUX!  
Linux is a free unix-type opterating system.  
This is a linux testfile!  
Linux test 
newline
```

## 2.删除

```bash
# 删除2~5行(-e可省略),nl相当于cat加上序号
nl /etc/passwd | sed '2,5d'

# 只删除第2行
nl /etc/passwd | sed '2d'

# 删除3到最后一行
nl /etc/passwd | sed '3,$d'
```

## 3.插入

```bash
# 第2行前加入 drink tea
nl /etc/passwd | sed '2a drink tea'

# 第2行后加入 drink tea
nl /etc/passwd | sed '2i drink tea'
```

# 4.替换

```bash
# 将2~5行内容取代为No 2-5 Number
nl /etc/passwd | sed '2,5c No 2-5 Number'
```
## 5.打印

```bash
# 仅列出/etc/passwd 文件内的第5~7行
nl /etc/passwd | sed -n '5,7p'
```


## sed替换标记

* `g`表示行内全面替换。  
* `p`表示打印行。  
* `w`表示把行写入一个文件。  
* `x`表示互换模板块中的文本和缓冲区中的文本。  
* `y`表示把一个字符翻译为另外的字符（但是不用于正则表达式）
* `\1`子串匹配标记
* `&`已匹配字符串标记

# 数据的搜索并显示

```bash
# 搜索/etc/passwd有root关键字的行(这个即输出结果，又输出所有的文件内容)
nl /etc/passwd | sed '/root/p'

# 只显示匹配的结果
nl /etc/passwd | sed -n '/root/p'
```

# 数据的搜索并删除

```
# 将会删除匹配root的行
nl /etc/passwd | sed '/root/d'
```

# 搜索并执行命令

搜索root，并执行花括号中命令，每个命令之间用分号分隔

```
# 把bash 替换为blueshll，再执行命令
nl /etc/passwd | sed -n '/root/{s/bash/blueshell/;p;q}'
```

# 多点编辑

```
nl /etc/passwd | sed -e '3,$d' -e 's/bash/blueshell/'
```

# 数据的搜搜并替换

```
sed 's/oldstring/newstring/g'
```

# 直接修改文件内容

```
sed -i 's/\.$/\!/g' regular_express.txt
```
## sed元字符集

* `^`匹配行开始，如：`/^sed/`匹配所有以sed开头的行。
* `$`匹配行结束，如：`/sed$/`匹配所有以sed结尾的行。
* `.`匹配一个非换行符的任意字符，如：`/s.d/`匹配s后接一个任意字符，最后是d。
* `*`匹配0个或多个字符，如：`/*sed/`匹配所有模板是一个或多个空格后紧跟sed的行。
* `[]`匹配一个指定范围内的字符，如`/[ss]ed/`匹配`sed`和`Sed`。  
* `[^]`匹配一个不在指定范围内的字符，如：`/[^A-RT-Z]ed`/匹配不包含`A-R`和`T-Z`的一个字母开头，紧跟ed的行。
* `\(..\)`匹配子串，保存匹配的字符，如`s/\(love\)able/\1rs`，`loveable`被替换成`lovers`。
* `&`保存搜索字符用来替换其他字符，如`s/love/**&**/`，love这成`**love**`。
* `\<`匹配单词的开始，如:`/\<love/`匹配包含以love开头的单词的行。
* `\>`匹配单词的结束，如`/love\>/`匹配包含以love结尾的单词的行。
* `x\{m\}`重复字符x，m次，如：`/0\{5\}/`匹配包含5个0的行。
* `x\{m,\}`重复字符x，至少m次，如：`/0\{5,\}/`匹配至少有5个0的行。
* `x\{m,n\}`重复字符x，至少m次，不多于n次，如：`/0\{5,10\}/`匹配5~10个0的行。 
